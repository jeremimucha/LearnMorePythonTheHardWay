#pragma once

#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <variant>
#include "Token.h"
#include "Scanner.h"

/**
 * PunyPy grammar
 *
 * root       := (Funccall | Funcdef)*
 * Funcdef    := DEF Name LPAREN Params RPAREN COLON Body
 * Funccal    := Name LPAREN Params RPAREN
 * Params     := Expression (COMMAA Expression)*
 * Expression := Name | Plus | Integer
 * Body       := (INDENT Expression | Funccal)*
 * Plus       := Expression PLUS Expression
 * PLUS       := "+"
 * LPAREN     := "("
 * RPAREN     := ")"
 * COLON      := ":"
 * COMMA      := ","
 * DEF        := "def"
 * INDENT     := "    " ; some amount of whitespace at the start of a line
 *
 * TODO:
 * Statement
 *      := (Funccall | Funcdef)
 *       | (Funccall | Funcdef) Statement
 *
 * Funcdef
 *      := DEF Name LPAREN Params RPAREN COLON Body
 *
 * Funccall
 *      := name LPAREN params RPAREN
 * 
 * Params
 *      := Expression
 *       | Expression COMMA expression
 *
 * Expression
 *      := Primary
 *       | Expression PLUS Primary
 *       | Expression MINUS Primary
 *
 * Primary 
 *      := Integer
 *       | Name
 *       | PLUS Primary
 *       | MINUS Primary
 *       | LPAREN Expression RPAREN
 * 
 * PLUS   = "+"
 * MINUS  = "-"
 * LPAREN = "("
 * RPAREN = ")"
 * COLON  = ":"
 * COMMA  = ","
 * DEF    = "def"
 */

class Prod_base
{
public:
    virtual ~Prod_base() noexcept = default;
protected:
    friend class Production;
    Prod_base() noexcept = default;
    Prod_base(const Prod_base&) noexcept = default;
    Prod_base(Prod_base&&) noexcept = default;
    // Prod_base& Prod_base(const Prod_base&) noexcept = default;
    // Prod_base& Prod_base(Prod_base&&) noexcept = default;

    // virtual void eval() const noexcept = 0;
    virtual std::string rep() const = 0;
};

struct Funcdef_tag { };
static constexpr Funcdef_tag funcdef_tag{};
struct Funccall_tag { };
static constexpr Funccall_tag funccall_tag{};
struct Expr_tag { };
static constexpr Expr_tag expr_tag{};
struct Funcbod_tag { };
static constexpr Funcbod_tag funcbod_tag{};
struct Eof_tag { };
static constexpr Eof_tag eof_tag{};

class Production
{
public:
    explicit Production(Eof_tag);
    explicit Production(int i);
    explicit Production(std::string);
    Production(int a, int b);
    Production(std::string a, std::string b);
    Production(int a, std::string b);
    Production(std::string a, int b);
    Production(Production&&, Production&&);
    Production(Expr_tag, Production&&);
    explicit Production(std::vector<Production>&&);
    Production(Funcdef_tag, std::string name, Production&& params, Production&& body);
    Production(Funcbod_tag, std::string indent, std::vector<Production>&& body);
    Production(Funccall_tag, std::string name, Production&& params);

    std::string rep() const
    {
        return prod_->rep();
    }

friend std::ostream& operator<<(std::ostream&, const Production&);
private:
    std::unique_ptr<Prod_base> prod_;
};

class Eof_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Eof_prod>
        std::make_unique<Eof_prod>();
    Eof_prod() noexcept = default;

    std::string rep() const final
    {
        return "\nDone.";
    }
};

inline auto Eof() noexcept
{
    return Production{eof_tag};
}

class FuncDef_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<FuncDef_prod>
        std::make_unique<FuncDef_prod>(std::string&&, Production&&, Production&&);
    FuncDef_prod(std::string name, Production&& params, Production&& body)
        : name_{std::move(name)}, params_{std::move(params)}, body_{std::move(body)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Function\n  - name = " << name_ 
            << "\n  " << params_.rep()
            << "\n  " << body_.rep();
        return oss.str();
    }

    std::string name_;
    Production params_;
    Production body_;
};

inline auto Funcdef(std::string&& name, Production&& prod, Production&& body) -> Production
{
    return Production{funcdef_tag, std::move(name), std::move(prod), std::move(body)};
}

class Funcbod_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Funcbod_prod>
        std::make_unique<Funcbod_prod>(std::string&&, std::vector<Production>&&);
    Funcbod_prod(std::string indent, std::vector<Production>&& body)
        : indent_{std::move(indent)}, body_{std::move(body)} { }
    
    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "- function body";
        for(const auto& p : body_)
            oss << "\n    " << '|' << indent_ << '|' << p.rep();
        return oss.str();
    }

    std::string indent_;
    std::vector<Production> body_;
};

inline auto Function_body(const std::string&& indent, std::vector<Production>&& body) -> Production
{
    return Production{funcbod_tag, std::move(indent), std::move(body)};
}

class FuncCall_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<FuncCall_prod>
        std::make_unique<FuncCall_prod>(std::string&&, Production&&);
    FuncCall_prod(std::string name, Production&& params)
        : name_{std::move(name)}, params_{std::move(params)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Call:\n  - name = " << name_ 
            << "\n  " << params_.rep();
        return oss.str();
    }

    std::string name_;
    Production params_;
};

inline auto Funccall(const std::string&& name, Production&& prod) -> Production
{
    return Production{funccall_tag, std::move(name), std::move(prod)};
}

class Val_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Val_prod>
        std::make_unique<Val_prod>(int&&);
    friend std::unique_ptr<Val_prod>
        std::make_unique<Val_prod>(std::string&&);
    explicit Val_prod(int i)
        : v_{i} { }
    explicit Val_prod(std::string s)
        : v_{std::move(s)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        if(std::holds_alternative<int>(v_))
            oss << "        - {literal = " << std::get<int>(v_) << "}";
        else
            oss << "        - {var = " << std::get<std::string>(v_) << "}";
        return oss.str();
    }

    std::variant<int, std::string> v_;
};


class Plus_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(int&, int&);
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(int&, std::string&&);
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(std::string&&, int&);
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(std::string&&, std::string&&);
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(Production&&, Production&&);
    Plus_prod(int a, int b) noexcept
        : left_{a}, right_{b} { }
    Plus_prod(std::string a, std::string b)
        : left_{std::move(a)}, right_{std::move(b)} { }
    Plus_prod(int a, std::string b)
        : left_{a}, right_{std::move(b)} { }
    Plus_prod(std::string a, int b)
        : left_{std::move(a)}, right_{b} { }
    Plus_prod(Production&& lhs, Production&& rhs)
        : left_{std::move(lhs)}, right_{std::move(rhs)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << " - Plus\n  " << left_.rep() << "\n  " << right_.rep();
        return oss.str();
    }

    Production left_;
    Production right_;
};

class Expr_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Expr_prod>
        std::make_unique<Expr_prod>(Production&&);
    Expr_prod(Production&& prod)
        : val_{std::move(prod)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Expression:\n    " << val_.rep();
        return oss.str();
    }

    Production val_;
};

inline auto Expression(Production&& prod) -> Production
{
    return Production{expr_tag, std::move(prod)};
}

class Params_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Params_prod>
        std::make_unique<Params_prod>(std::vector<Production>&&);
    Params_prod(std::vector<Production>&& vec)
        : params_{std::move(vec)} { }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << " - Parameters =\n";
        for(const auto& p : params_)
            oss << "        " << p.rep() << "\n";
        return oss.str();
    }

    std::vector<Production> params_;
};



class Bad_token : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/**
 * \brief 'PunyPy' Parser used to parse an extremely simplified python grammar
 */
class Parser
{
    using scanner_t = Scanner;
public:
    explicit Parser(std::istream& is)
        : scanner_{is}
        { }

    auto root() -> Production;
    auto function_def() -> Production;
    auto function_body() -> Production;
    auto function_call() -> Production;
    auto parameters() -> Production;
    auto expression() -> Production;
    auto plus(Production&& left) -> Production;

    explicit operator bool() const noexcept
    {
        return scanner_.operator bool();
    }

private:
    scanner_t scanner_;
};