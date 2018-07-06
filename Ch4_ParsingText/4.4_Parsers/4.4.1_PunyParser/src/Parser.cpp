#include "Parser.h"
#include <iostream>

Production::Production(Eof_tag)
    : prod_{std::make_unique<Eof_prod>()}
{
}

Production::Production(int i)
    // : prod_{std::make_unique<Val_prod>(i)}
    : prod_{new Val_prod{i}}
{
}
Production::Production(std::string name)
    // : prod_{std::make_unique<Val_prod>(std::move(name))}
    : prod_{new Val_prod{std::move(name)}}
{
}

Production::Production(int a, int b)
    : prod_{std::make_unique<Plus_prod>(a, b)}
{
}
Production::Production(std::string a, std::string b)
    : prod_{std::make_unique<Plus_prod>(std::move(a),std::move(b))}
{
}
Production::Production(int a, std::string b)
    : prod_{std::make_unique<Plus_prod>(a, std::move(b))}
{
}
Production::Production(std::string a, int b)
    : prod_{new Plus_prod{std::move(a), b}}
{
}
Production::Production(Production&& lhs, Production&& rhs)
    : prod_{new Plus_prod{std::move(lhs), std::move(rhs)}}
{
}

Production::Production(Expr_tag, Production&& prod)
    : prod_{std::make_unique<Expr_prod>(std::move(prod))}
{
}

Production::Production(std::vector<Production>&& params)
    : prod_{std::make_unique<Params_prod>(std::move(params))}
{
}

Production::Production(Funcdef_tag, std::string name, Production&& params, Production&& body)
    : prod_{std::make_unique<FuncDef_prod>(std::move(name),std::move(params),std::move(body))}
{
}
Production::Production(Funcbod_tag, std::string indent, std::vector<Production>&& body)
    : prod_{std::make_unique<Funcbod_prod>(std::move(indent), std::move(body))}
{
}
Production::Production(Funccall_tag, std::string name, Production&& params)
    : prod_{std::make_unique<FuncCall_prod>(std::move(name), std::move(params))}
{
}

std::ostream& operator<<(std::ostream& os, const Production& p)
{
    return os << p.rep();
}


auto Parser::root() -> Production
{
    switch(auto token = scanner_.peek(); token.kind())
    {
    case Kind::Def:
        scanner_.ignore(Kind::Def);
        return function_def();
    case Kind::Name:
        return function_call();
    case Kind::Eof:
        return Eof();
    default:
        throw Bad_token("Function definition or function call expected");
    }
}

auto Parser::function_def() -> Production
{
    const auto name = scanner_.get();
    if(name.kind() != Kind::Name)
        throw Bad_token("Invalid token, Name expected");
    auto tok = scanner_.get();
    if(tok.kind() != Kind::LParen)
        throw Bad_token("Invalid token, \"(\" expected");
    auto params = parameters();
    tok = scanner_.get();
    if(tok.kind() != Kind::RParen)
        throw Bad_token("Invalid token, \")\" expected");
    tok = scanner_.get();
    if(tok.kind() != Kind::Colon)
        throw Bad_token("Invalid token, \":\" expected");
    auto body = function_body();
    return Funcdef(name.str_val(), std::move(params), std::move(body));
}

auto Parser::function_call() -> Production
{
    auto tok = scanner_.get();
    if(tok.kind() != Kind::Name)
        throw Bad_token("Invalid token, Name expected");
    const auto name = std::move(tok.str_val());

    tok = scanner_.get();
    if(tok.kind() != Kind::LParen)
        throw Bad_token("Invalid token, \"(\" expected");
    auto params = parameters();
    tok = scanner_.get();
    if(tok.kind() != Kind::RParen)
        throw Bad_token("Invalid token, \")\" expected");
    return Funccall(std::move(name), std::move(params));
}

auto Parser::function_body() -> Production
{
    auto tok = scanner_.get();
    if(tok.kind() != Kind::Indent)
        throw Bad_token("Invalid token, Indent expected");
    const auto indent = tok.str_val();
    auto body = std::vector<Production>();
    while(true)
    {
        tok = scanner_.peek();
        switch(tok.kind())
        {
        case Kind::Int:
            body.emplace_back(expression());
            break;
        case Kind::Name:
            // funccall or expression, assume funccall for now
            std::cerr << "function_body -> expecting function_call\n";
            body.emplace_back(function_call());
            break;
        default:
            throw Bad_token("Invalid token, Name or Int expected");
        }
        tok = scanner_.peek();
        if(tok.kind() != Kind::Indent){
            return Function_body(std::move(indent), std::move(body));
        }
        scanner_.ignore(Kind::Indent);
    }
    throw Bad_token("Empty function body");
}

auto Parser::parameters() -> Production
{
    auto params = std::vector<Production>();
    for(auto tok = scanner_.peek(); tok.kind() != Kind::RParen;)
    {
        params.emplace_back(expression());
        tok = scanner_.peek();
        switch(tok.kind())
        {
        case Kind::Comma:
            scanner_.ignore(Kind::Comma);
            break;
        case Kind::RParen:
            return Production{std::move(params)};
        default:
            throw Bad_token("Invalid token, \",\" or \")\" expected");
        }
    }
    return Production{std::move(params)};
}

auto Parser::expression() -> Production
{
    switch(auto tok = scanner_.get(); tok.kind())
    {
    case Kind::Name:
        if(scanner_.peek().kind() == Kind::Plus)
            return Expression(plus(Production{tok.str_val()}));
        return Expression(Production{tok.str_val()});
    case Kind::Int:
        if(scanner_.peek().kind() == Kind::Plus)
            return Expression(plus(Production{tok.int_val()}));
        return Expression(Production{tok.int_val()});
    default:
        throw Bad_token("Invalid token, 'Name' or 'Int' expected");
    }
}

auto Parser::plus(Production&& left) -> Production
{
    auto tok = scanner_.get();
    if(tok.kind() != Kind::Plus)
        throw Bad_token("Invalid token, \"+\" expected");
    auto right = expression();
    return Production{std::move(left), std::move(right)};
}