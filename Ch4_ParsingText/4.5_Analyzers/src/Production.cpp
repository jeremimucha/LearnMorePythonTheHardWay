#include "Production.h"
#include "PunyPyWorld.h"


Production::Production(Eof_tag)
    : prod_{std::make_unique<Eof_prod>()}
{
}

// The `move(i)` has no effect, but avoids the need to declare multiple friend specilizations
// of std::unique_ptr and std::make_unique
Production::Production(int i)
    : prod_{std::make_unique<Int_prod>(std::move(i))} // NOLINT
{
}
Production::Production(std::string name)
    : prod_{std::make_unique<Var_prod>(std::move(name))}
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
    : prod_{std::make_unique<Plus_prod>(std::move(a), b)}
{
}
Production::Production(Production&& lhs, Production&& rhs)
    : prod_{std::make_unique<Plus_prod>(std::move(lhs), std::move(rhs))}
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

Production::Production(Declaration_tag, std::string name, Production&& val)
    : prod_{std::make_unique<Decl_prod>(std::move(name), std::move(val))}
{
}

std::ostream& operator<<(std::ostream& os, const Production& p)
{
    return os << p.rep();
}

void Decl_prod::analyze(PunyPyWorld& world)
{
    INFO();
    val_.analyze(world);
    world.set_var(name_, val_.value());
}

void FuncDef_prod::analyze(PunyPyWorld& world)
{
    INFO();
    // make sure the parameters are all variables
    // params_.analyze(world);
    body_.analyze(world);
    world.set_func(name_, Production{clone()});
}

void FuncCall_prod::analyze(PunyPyWorld& world)
{
    INFO();
    params_.analyze(world);
    // TODO:
    // * check if a function with the given `name_` was defined
    const auto* const func = world.get_func(name_);
    if(!func){
        throw Bad_production("Bad function call. Function " + name_ +
                                " has not been declared");
    }
    // * check if number of parameters matches
    if(params_.param_count() != func->param_count()){
        throw Bad_production(
            "Bad function call, invalid number of parameters."
            "\nGot: " + std::to_string(params_.param_count()) +
            ". Expected: " + std::to_string(func->param_count()));
    }
}

void Var_prod::analyze(PunyPyWorld& world)
{
    INFO();
    auto res = world.get_var(var_);
    if(!res.second){
        throw Bad_production{"Invalid use of a variable. Variable " + var_ +
                                " has not been declared"};
    }
}
