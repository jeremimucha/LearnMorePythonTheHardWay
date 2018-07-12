#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "debug.h"


class PunyPyWorld;

class Bad_production : public std::logic_error
{
    using std::logic_error::logic_error;
};

class Prod_base
{
public:
    virtual ~Prod_base() noexcept = default;
protected:
    friend class Production;
    Prod_base() noexcept = default;
    Prod_base(const Prod_base&) noexcept = default;
    Prod_base(Prod_base&&) noexcept = default;
    Prod_base& operator=(const Prod_base&) noexcept = default;
    Prod_base& operator=(Prod_base&&) noexcept = default;

    virtual std::unique_ptr<Prod_base> clone() const& = 0;
    virtual std::unique_ptr<Prod_base> clone() && = 0;

    // virtual void eval() const noexcept = 0;
    virtual std::string rep() const = 0;
    virtual void analyze(PunyPyWorld&) = 0;
    virtual int value() const
    {
        throw Bad_production{"Value called on a non Int_prod production"};
    }
    virtual int param_count() const
    {
        throw Bad_production{
            "param_count() called on a non Param_prod/FuncCal_prod/FuncDef_prod production"
            };
    }
};

struct Funcdef_tag { };
static constexpr Funcdef_tag funcdef_tag{};
struct Funccall_tag { };
static constexpr Funccall_tag funccall_tag{};
struct Expr_tag { };
static constexpr Expr_tag expr_tag{};
struct Funcbod_tag { };
static constexpr Funcbod_tag funcbod_tag{};
struct Declaration_tag { };
static constexpr Declaration_tag declaration_tag{};
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
    // Production(Expr_tag, Production&&);
    explicit Production(std::vector<Production>&&);
    Production(Funcdef_tag, std::string name, Production&& params, Production&& body);
    Production(Funcbod_tag, std::string indent, std::vector<Production>&& body);
    Production(Funccall_tag, std::string name, Production&& params);
    Production(Declaration_tag, std::string name, Production&& val);

    ~Production() noexcept = default;
    Production(Production&&) noexcept = default;
    Production& operator=(Production&&) noexcept = default;
    Production(const Production& other)
        : prod_{other.prod_->clone()} { }
    Production& operator=(const Production& other)
    {
        auto copy = other.prod_->clone();
        prod_ = std::move(copy);
        return *this;
    }
    // Support cloning, should be private with all Prod_base derived classes declared friends
    Production(std::unique_ptr<Prod_base> prod) noexcept
        : prod_{std::move(prod)} { }

    std::string rep() const
    {
        return prod_->rep();
    }

    void analyze(PunyPyWorld& world)
    {
        prod_->analyze(world);
    }

    int value() const
    {
        return prod_->value();
    }

    int param_count() const
    {
        return prod_->param_count();
    }

friend std::ostream& operator<<(std::ostream&, const Production&);
private:
    std::unique_ptr<Prod_base> prod_;
};

class Decl_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Decl_prod>
        std::make_unique<Decl_prod>(std::string&&, Production&&);
    friend std::unique_ptr<Decl_prod>
        std::make_unique<Decl_prod>(const Decl_prod&);
    friend std::unique_ptr<Decl_prod>
        std::make_unique<Decl_prod>(Decl_prod&&);

    Decl_prod(std::string&& name, Production&& val)
        : name_{std::move(name)}, val_{std::move(val)}
        { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Decl_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Decl_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Declaration\n  - name = " << name_
            << "\n  - value =\n" << val_.rep();
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final;

    std::string name_;
    Production  val_;
};

inline auto Declaration(std::string&& name, Production&& val)
{
    return Production(declaration_tag, std::move(name), std::move(val));
}

class Eof_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Eof_prod>
        std::make_unique<Eof_prod>();
    friend std::unique_ptr<Eof_prod>
        std::make_unique<Eof_prod>(const Eof_prod&);
    friend std::unique_ptr<Eof_prod>
        std::make_unique<Eof_prod>(Eof_prod&&);

    Eof_prod() noexcept = default;

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Eof_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Eof_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        return "\nDone.";
    }

    void analyze(PunyPyWorld& /* world */) final
    {
        INFO();
        // return nullptr;
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
    friend std::unique_ptr<FuncDef_prod>
        std::make_unique<FuncDef_prod>(const FuncDef_prod&);
    friend std::unique_ptr<FuncDef_prod>
        std::make_unique<FuncDef_prod>(FuncDef_prod&&);

    FuncDef_prod(std::string name, Production&& params, Production&& body)
        : name_{std::move(name)}, params_{std::move(params)}, body_{std::move(body)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<FuncDef_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<FuncDef_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Function\n  - name = " << name_ 
            << "\n  " << params_.rep()
            << "\n  " << body_.rep();
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final;

    int param_count() const final
    {
        return params_.param_count();
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
    friend std::unique_ptr<Funcbod_prod>
        std::make_unique<Funcbod_prod>(const Funcbod_prod&);
    friend std::unique_ptr<Funcbod_prod>
        std::make_unique<Funcbod_prod>(Funcbod_prod&&);

    Funcbod_prod(std::string indent, std::vector<Production>&& body)
        : indent_{std::move(indent)}, body_{std::move(body)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Funcbod_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Funcbod_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "- function body";
        for(const auto& p : body_)
            oss << "\n    " << indent_ << p.rep();
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final
    {
        INFO();
        for(auto&& p : body_) {
            p.analyze(world);
        }
    }

    std::string indent_;
    std::vector<Production> body_;
};

inline auto Function_body(std::string&& indent, std::vector<Production>&& body) -> Production
{
    return Production{funcbod_tag, std::move(indent), std::move(body)};
}

class FuncCall_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<FuncCall_prod>
        std::make_unique<FuncCall_prod>(std::string&&, Production&&);
    friend std::unique_ptr<FuncCall_prod>
        std::make_unique<FuncCall_prod>(const FuncCall_prod&);
    friend std::unique_ptr<FuncCall_prod>
        std::make_unique<FuncCall_prod>(FuncCall_prod&&);

    FuncCall_prod(std::string name, Production&& params)
        : name_{std::move(name)}, params_{std::move(params)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<FuncCall_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<FuncCall_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "* Call:\n  - name = " << name_ 
            << "\n  " << params_.rep();
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final;

    int param_count() const final
    {
        return params_.param_count();
    }

    std::string name_;
    Production params_;
};

inline auto Funccall(std::string&& name, Production&& prod) -> Production
{
    return Production{funccall_tag, std::move(name), std::move(prod)};
}

class Expr_prod : public Prod_base
{
};

class Int_prod : public Expr_prod
{
    friend class Production;
    friend class Plus_prod;
    friend std::unique_ptr<Int_prod>
        std::make_unique<Int_prod>(int&&);
    friend std::unique_ptr<Int_prod>
        std::make_unique<Int_prod>(const Int_prod&);
    friend std::unique_ptr<Int_prod>
        std::make_unique<Int_prod>(Int_prod&&);

    explicit Int_prod(int i)
        : val_{i} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Int_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Int_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "    Int: " << val_;
        return oss.str();
    }

    void analyze(PunyPyWorld& /* world */) final
    {
        INFO();
        // Nothing to do?
    }

    int value() const final { return val_; }

    int val_;
};

class Var_prod : public Expr_prod
{
    friend class Production;
    friend std::unique_ptr<Var_prod>
        std::make_unique<Var_prod>(std::string&&);
    friend std::unique_ptr<Var_prod>
        std::make_unique<Var_prod>(const Var_prod&);
    friend std::unique_ptr<Var_prod>
        std::make_unique<Var_prod>(Var_prod&&);

    explicit Var_prod(std::string name)
        : var_{std::move(name)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Var_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Var_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "    Var: " << var_;
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final;

    std::string var_;
};

class Plus_prod : public Expr_prod
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
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(const Plus_prod&);
    friend std::unique_ptr<Plus_prod>
        std::make_unique<Plus_prod>(Plus_prod&&);

    Plus_prod(int a, int b)
        : left_{a}, right_{b} { }
    Plus_prod(std::string a, std::string b)
        : left_{std::move(a)}, right_{std::move(b)} { }
    Plus_prod(int a, std::string b)
        : left_{a}, right_{std::move(b)} { }
    Plus_prod(std::string a, int b)
        : left_{std::move(a)}, right_{b} { }
    Plus_prod(Production&& lhs, Production&& rhs)
        : left_{std::move(lhs)}, right_{std::move(rhs)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Plus_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Plus_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "  - Plus\n    " << left_.rep() << "\n    " << right_.rep();
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final
    {
        INFO();
        left_.analyze(world);
        right_.analyze(world);
    }

    Production left_;
    Production right_;
};

template<typename T
//    ,typename = std::enable_if_t<
//     std::is_integral_v<std::decay_t<T>> ||
//     std::is_convertible_v<std::decay_t<T>,std::string>>
>
inline auto Expression(T&& val) -> Production
{
    static_assert(
        std::is_integral_v<std::decay_t<T>> ||
        std::is_convertible_v<std::decay_t<T>,std::string>,
        "Must be called with either an int or a type convertible to std::string"
    );
    return Production{std::forward<T>(val)};
}

class Params_prod : public Prod_base
{
    friend class Production;
    friend std::unique_ptr<Params_prod>
        std::make_unique<Params_prod>(std::vector<Production>&&);
    friend std::unique_ptr<Params_prod>
        std::make_unique<Params_prod>(const Params_prod&);
    friend std::unique_ptr<Params_prod>
        std::make_unique<Params_prod>(Params_prod&&);

    Params_prod(std::vector<Production>&& vec)
        : params_{std::move(vec)} { }

    std::unique_ptr<Prod_base> clone() const& final
    {
        return std::make_unique<Params_prod>(*this);
    }
    std::unique_ptr<Prod_base> clone() && final
    {
        return std::make_unique<Params_prod>(std::move(*this));
    }

    std::string rep() const final
    {
        auto oss = std::ostringstream{};
        oss << "- Parameters =\n";
        for(const auto& p : params_)
            oss << "    " << p.rep() << "\n";
        return oss.str();
    }

    void analyze(PunyPyWorld& world) final
    {
        INFO();
        for(auto&& p : params_) {
            p.analyze(world);
        }
    }

    int param_count() const final
    {
        return params_.size();
    }

    std::vector<Production> params_;
};
