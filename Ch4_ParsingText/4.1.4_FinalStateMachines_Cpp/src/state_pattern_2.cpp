#include <cstdio>
#include <utility>
#include <boost/type_index.hpp>

using boost::typeindex::type_id;

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__, __LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", \
                                   __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct Event
{
};

template<typename S>
class Fsm
{
  public:
    using state_type = S;

    constexpr explicit Fsm(state_type* initial_state) noexcept
        : p_state{initial_state}
    {
    }

    void start() noexcept
    {
        enter();
    }

    template<typename E>
    void handle(E&& e) noexcept
    {
        auto* const ns = p_state->react(std::forward<E>(e));
        transit(ns);
    }

    void enter() noexcept
    {
        p_state->entry();
    }

    void transit(state_type* next_state) noexcept
    {
        p_state->exit();
        p_state = next_state;
        p_state->entry();
    }

  private:
    state_type *p_state;
};


struct eConnect : Event { };
struct eAccept  : Event { };
struct eRead    : Event { };
struct eWrite   : Event { };
struct eClose   : Event { };
struct eError   : Event { };

template<typename S>
static inline S State_instance{};

struct State
{
    virtual ~State() noexcept = default;
    constexpr State() noexcept = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) noexcept = delete;
    State& operator=(State&&) noexcept = delete;

    virtual State *react(const eConnect& e) noexcept;
    virtual State *react(const eAccept& e) noexcept;
    virtual State *react(const eRead& e) noexcept;
    virtual State *react(const eWrite& e) noexcept;
    virtual State *react(const eClose& e) noexcept;
    virtual State *react(const eError& e) noexcept;
    virtual void entry() noexcept;
    virtual void exit() noexcept;
};


class Listening : public State
{
public:
    constexpr Listening() noexcept = default;
    State* react(const eConnect& e) noexcept final;
    State* react(const eError& e) noexcept final;
};

class Connected : public State
{
public:
    constexpr Connected() noexcept = default;
    State* react(const eAccept& e) noexcept final;
    State* react(const eClose& e) noexcept final;
};

class Accepted : public State
{
public:
    constexpr Accepted() noexcept = default;
    State* react(const eRead& e) noexcept final;
    State* react(const eWrite& e) noexcept final;
    State* react(const eClose& e) noexcept final;
};

class Reading : public State
{
public:
    constexpr Reading() noexcept = default;

    State* react(const eRead& e) noexcept final;
    State* react(const eWrite& e) noexcept final;
    State* react(const eClose& e) noexcept final;
};

class Writing : public State
{
public:
    constexpr Writing() noexcept = default;
    State* react(const eRead& e) noexcept final;
    State* react(const eWrite& e) noexcept final;
    State* react(const eClose& e) noexcept final;
};

class Closed : public State
{
public:
    constexpr Closed() noexcept = default;
    State* react(const eConnect& e) noexcept final;
    State* react(const eAccept& e) noexcept final;
    State* react(const eRead& e) noexcept final;
    State* react(const eWrite& e) noexcept final;
    State* react(const eClose& e) noexcept final;
    State* react(const eError& e) noexcept final;
};

class Error : public State
{
public:
    constexpr Error() noexcept = default;
    State* react(const eConnect& e) noexcept final;
    State* react(const eAccept& e) noexcept final;
    State* react(const eRead& e) noexcept final;
    State* react(const eWrite& e) noexcept final;
    State* react(const eClose& e) noexcept final;
    State* react(const eError& e) noexcept final;
};

State *State::react(const eConnect&) noexcept
{
    INFO("Ignored event: Connect");
    return &State_instance<Error>;
}
State *State::react(const eAccept&) noexcept
{
    INFO("Ignored event: Accept");
    return &State_instance<Error>;
}
State *State::react(const eRead&) noexcept
{
    INFO("Ignored event: Read");
    return &State_instance<Error>;
}
State *State::react(const eWrite&) noexcept
{
    INFO("Ignored event: Write");
    return &State_instance<Error>;
}
State *State::react(const eClose&) noexcept
{
    INFO("Ignored event: Close");
    return &State_instance<Error>;
}
State *State::react(const eError&) noexcept
{
    INFO("Ignored event: Error");
    return &State_instance<Error>;
}
void State::entry() noexcept { INFO("Default entry"); }
void State::exit() noexcept { INFO("Default exit"); }

State* Listening::react(const eConnect& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Connected>;
}

State* Listening::react(const eError& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Connected::react(const eAccept& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Accepted>;
}

State* Connected::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Closed>;
}

State* Accepted::react(const eRead& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Reading>;
}

State* Accepted::react(const eWrite& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Writing>;
}

State* Accepted::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Closed>;
}

State* Reading::react(const eRead& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Reading>;
}

State* Reading::react(const eWrite& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Writing>;
}

State* Reading::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Closed>;
}

State* Writing::react(const eRead& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Reading>;
}

State* Writing::react(const eWrite& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Writing>;
}

State* Writing::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Closed>;
}

State* Closed::react(const eConnect& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Closed::react(const eAccept& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Closed::react(const eRead& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Closed::react(const eWrite& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Closed::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Closed::react(const eError& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Listening>;
}

State* Error::react(const eConnect& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

State* Error::react(const eAccept& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

State* Error::react(const eRead& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

State* Error::react(const eWrite& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

State* Error::react(const eClose& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

State* Error::react(const eError& e) noexcept
{
    DEBUG("%s, event: %s", __PRETTY_FUNCTION__, type_id<decltype(e)>().pretty_name().c_str());
    return &State_instance<Error>;
}

int main()
{
    auto fsm = Fsm<State>{&State_instance<Listening>};
    fsm.start();
    fsm.handle(eConnect{});
    fsm.handle(eAccept{});
    fsm.handle(eRead{});
    fsm.handle(eWrite{});
    fsm.handle(eClose{});
    fsm.handle(eConnect{});
}
