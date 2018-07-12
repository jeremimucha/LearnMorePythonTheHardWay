#include "PunyPyWorld.h"
#include "Production.h"


std::pair<int, bool> PunyPyWorld::get_var(const std::string& name) const noexcept
{
    if(auto res = variables_.find(name); res != variables_.cend()){
        return {res->second, true};
    }
    return {0,false};
}

void PunyPyWorld::set_var(const std::string& name, int val)
{
    auto res = variables_.insert({name, val});
    if(!res.second){
        res.first->second = val;
    }
}

Production* PunyPyWorld::get_func(const std::string& name)
{
    if(auto res = functions_.find(name); res != functions_.end()){
        return &res->second;
    }
    return nullptr;
}

void PunyPyWorld::set_func(const std::string& name, Production&& prod)
{
    auto res = functions_.emplace(name, std::move(prod));
    if(!res.second)
        throw Bad_declaration{"Redeclaration of a function " + name};
}
