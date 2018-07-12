#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>
#include <stdexcept>

#include "debug.h"
#include "Production.h"
// class  Production;


struct Bad_declaration : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class PunyPyWorld
{
public:
    using variable_t = std::pair<std::string, int>;
    using iterator = std::unordered_map<std::string, int>::iterator;
    using const_iterator = std::unordered_map<std::string, int>::const_iterator;

    PunyPyWorld() = default;
    explicit PunyPyWorld(const std::vector<variable_t>& vars)
        : variables_{vars.cbegin(), vars.cend()} { }


    PunyPyWorld(const PunyPyWorld&) = delete;
    PunyPyWorld& operator=(const PunyPyWorld&) = delete;
    PunyPyWorld(PunyPyWorld&&) noexcept = default;
    PunyPyWorld& operator=(PunyPyWorld&&) = default;
    ~PunyPyWorld() noexcept = default;

    // auto begin() noexcept { return variables_.begin(); }
    // auto end() noexcept   { return variables_.end(); }
    // auto begin() const noexcept { return variables_.begin(); }
    // auto end() const noexcept { return variables_.end(); }
    // auto cbegin() const noexcept { return variables_.cbegin(); }
    // auto cend() const noexcept { return variables_.cend(); }

    std::pair<int, bool> get_var(const std::string& name) const noexcept;
    void set_var(const std::string& name, int val);
    Production* get_func(const std::string& name);
    void set_func(const std::string& name, Production&& prod);

private:
    std::unordered_map<std::string, int> variables_{};
    std::unordered_map<std::string, Production> functions_{};
};
