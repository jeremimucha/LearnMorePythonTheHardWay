#include <cstdio>
#include <utility>
#include <type_traits>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)

template<typename S, typename E>
struct Fsm_state_map
{
    // INFO( " UNHANDLED STATE TRANSITION " );
    // static_assert(false, "Unhandled state transition");
    using state_type = S;
};

template<typename Derived>
class Fsm
{
public:
    using fsm_type = Fsm<Derived>;
    using state_ptr_t = Derived*;

    static void set_initial_state() noexcept;

    template<typename S>
    static constexpr decltype(auto) state() noexcept
    {
        static_assert(std::is_same_v<fsm_type, typename S::fsm_type>,
            "accessing state of a different state machine");
        return state_instance<S>;
    }

    template<typename S>
    static constexpr bool is_in_state()
    {
        return current_state == &state_instance<S>;
    }

    static void reset() noexcept
    {
        Derived::reset();
    }

    static void enter() noexcept
    {
        current_state->entry();
    }

    static void start() noexcept
    {
        set_initial_state();
        enter();
    }

    template<typename E>
    static void dispatch(const E& event) noexcept
    {
        current_state->react(event);
    }

protected:

    Fsm() noexcept = default;
    friend Derived;

    template<typename S, typename E>
    using tr_evt = typename Fsm_state_map<S,E>::state_type;

    template<typename S, typename E>
    constexpr decltype(auto) transit_event() noexcept
    {
        DEBUG("%s",__PRETTY_FUNCTION__);
        return &state_instance<tr_evt<S,E>>;
    }

    template<typename S, typename E>
    void transit() noexcept
    {
        current_state->exit();
        current_state = transit_event<S,E>();
        current_state->entry();
    }

    template<typename S, typename E, typename Action>
    void transit(Action action) noexcept
    {
        current_state->exit();
        action();
        current_state = transit_event<S,E>();
    }

    template<typename S, typename E, typename Action, typename Condition>
    void transit(Action action, Condition condition) noexcept
    {
        if(condition()) { transit<S,E>(action); }
    }

    template<typename S>
    static inline S state_instance{};
    static state_ptr_t current_state;
};

template<typename Derived>
typename Fsm<Derived>::state_ptr_t Fsm<Derived>::current_state{};


template<typename... Fs>
struct FsmList;

template<> struct FsmList<>
{
    static void set_initial_state() { }
    static void reset() { }
    static void enter() { }
    template<typename E>
    static void dispatch(const E&) { }
};

template<typename F, typename... Fs>
struct FsmList<F, Fs...>
{
    using fsm_type = Fsm<F>;

    static void set_initial_state()
    {
        fsm_type::set_initial_state();
        FsmList<Fs...>::set_initial_state();
    }

    static void reset()
    {
        fsm_type::reset();
        FsmList<Fs...>::reset();
    }

    static void enter()
    {
        fsm_type::enter();
        FsmList<Fs...>::enter();
    }

    static void start()
    {
    // sets the initial states of ALL listed machines
        set_initial_state();
    //THEN enters() the initial state of each machine
        enter();
    }

    template<typename E>
    static void dispatch(E&& event)
    {
        fsm_type::template dispatch<E>(std::forward<E>(event));
        FsmList<Fs...>::template dispatch<E>(event);
    }
};

template<typename... Ss>
struct StateList;

template<> struct StateList<>
{
    static void reset() { }
};

template<typename S, typename... Ss>
struct StateList<S, Ss...>
{
    using fsm_type = typename S::fsm_type;
    static void reset()
    {
        fsm_type::template state_instance<S> = S{};
        StateList<Ss...>::reset();
    }
};

struct Event { };
struct Connect : public Event { };
struct Accept  : public Event { };
struct Read    : public Event { };
struct Write   : public Event { };
struct Close   : public Event { };
struct eError  : public Event { };

class FooFsm : public Fsm<FooFsm>
{
    friend class Fsm;
public:

    virtual void react(const Event&) noexcept;

    virtual void react(const Connect) noexcept;
    virtual void react(const Accept) noexcept;
    virtual void react(const Read) noexcept;
    virtual void react(const Write) noexcept;
    virtual void react(const Close) noexcept;
    virtual void react(const eError) noexcept;

    virtual void entry() noexcept;
    void         exit() noexcept;
};

class sError : public FooFsm
{
public:
    void entry() noexcept override;
};

class Listening : public FooFsm
{
public:
    void react(const Connect) noexcept override;
    void react(const eError) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

class Connected : public FooFsm
{
public:
    void react(const Accept) noexcept override;
    void react(const Close) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

class Accepted : public FooFsm
{
public:
    void react(const Close) noexcept override;
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

class Reading : public FooFsm
{
public:
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void react(const Close) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

class Writing : public FooFsm
{
public:
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void react(const Close) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

class Closed : public FooFsm
{
public:
    // void react(const Event&) noexcept override;
    void react(const Connect) noexcept override;
    void react(const Accept) noexcept override;
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void react(const Close) noexcept override;
    void react(const eError) noexcept override;
    void entry() noexcept final { DEBUG("* Enter %s*", __PRETTY_FUNCTION__); }
};

template<typename... Es>
struct EventList;

template<> struct EventList<>
{
    static void dispatch() { }
};

template<typename E, typename... Es>
struct EventList<E, Es...>
{
    using fsm_type = FooFsm;
    static void dispatch()
    {
        fsm_type::template dispatch<E>(E{});
        EventList<Es...>::dispatch();
    }
};

                                          /* State     Event                         Next State */
template<> struct           Fsm_state_map <FooFsm   , eError>  { using state_type = sError; };
template<> struct           Fsm_state_map <Listening, Connect> { using state_type = Connected; };
template<> struct           Fsm_state_map <Listening, eError>  { using state_type = Listening; };
template<typename E> struct Fsm_state_map <Listening, E>       { using state_type = sError;    };
template<> struct           Fsm_state_map <Connected, Accept>  { using state_type = Accepted;  };
template<> struct           Fsm_state_map <Connected, Close>   { using state_type = Closed;    };
template<typename E> struct Fsm_state_map <Connected, E>       { using state_type = sError;    };
template<> struct           Fsm_state_map <Accepted , Close>   { using state_type = Closed;    };
template<> struct           Fsm_state_map <Accepted , Read>    { using state_type = Reading;   };
template<> struct           Fsm_state_map <Accepted , Write>   { using state_type = Writing;   };
template<typename E> struct Fsm_state_map <Accepted , E>       { using state_type = sError;    };
template<> struct           Fsm_state_map <Reading  , Read>    { using state_type = Reading;   };
template<> struct           Fsm_state_map <Reading  , Write>   { using state_type = Writing;   };
template<> struct           Fsm_state_map <Reading  , Close>   { using state_type = Closed;    };
template<typename E> struct Fsm_state_map <Reading  , E>       { using state_type = sError;    };
template<> struct           Fsm_state_map <Writing  , Read>    { using state_type = Reading;   };
template<> struct           Fsm_state_map <Writing  , Write>   { using state_type = Writing;   };
template<> struct           Fsm_state_map <Writing  , Close>   { using state_type = Closed;    };
template<typename E> struct Fsm_state_map <Writing  , E>       { using state_type = sError;    };
template<typename E> struct Fsm_state_map <Closed   , E>       { using state_type = Listening; };
template<typename E> struct Fsm_state_map <sError   , E>       { using state_type = sError;    };

/* 
Maybe use macros to define the State map?
#define TRANSITION_TABLE_BEGIN(D) \
template<typename S, typename E> struct Fsm_state_map_ { using state_type_ = D; };
#define TRANSITION_TABLE_ROW(S,E,NS) \
template<> struct Fsm_state_map_<S,E> { using state_type_ = NS; };
#define TRANSITION_TABLE_ROW_(S,NS) \
template<typename E> struct Fsm_state_map<S,E> { using state_type_ = NS; }
#define TRANSITION_TABLE_END() \
template<typename S, typename E> using event_transition = typename Fsm_state_map_<S,E>::state_type_;

TRANSITION_TABLE_BEGIN(Listening);
    TRANSITION_TABLE_ROW(Listening, Connect, Connected);
    TRANSITION_TABLE_ROW(Listening, sError, Listening);
    TRANSITION_TABLE_ROW_(Listening, sError);
    ...
TRANSITION_TABLE_END();
*/

void FooFsm::react(const Event&) noexcept
{
    INFO("Default Event handler");
}
void FooFsm::react(const Connect) noexcept
{
    INFO("Default Connect handler");
}
void FooFsm::react(const Accept) noexcept
{
    INFO("Default Accept handler");
}
void FooFsm::react(const Read) noexcept
{
    INFO("Default Read handler");
}
void FooFsm::react(const Write) noexcept
{
    INFO("Default Write handler");
}
void FooFsm::react(const Close) noexcept
{
    INFO("Default Close handler");
}
void FooFsm::react(const eError) noexcept
{
    INFO("Default Error handler");
    // transit<FooFsm, eError>(); 
}
void FooFsm::entry() noexcept
{
    // INFO("Default entry react"); 
}
void FooFsm::exit() noexcept
{
    // INFO("Default exit react"); 
}

void sError::entry() noexcept
{
    INFO("ENTER ERROR STATE"); 
}

void Listening::react(const Connect) noexcept
{
    INFO("* Listening * -> Connect");
    transit<Listening,Connect>(); 
}
void Listening::react(const eError) noexcept
{
    INFO("* Listening * -> sError");
    transit<Listening, eError>(); 
}

void Connected::react(const Accept) noexcept
{
    INFO("* Connected * -> Accept");
    transit<Connected, Accept>();
}
void Connected::react(const Close) noexcept
{
    INFO("* Connected * -> Close");
    transit<Connected, Close>();
}

void Accepted::react(const Close) noexcept
{
    INFO("* Accepted * -> Close");
    transit<Accepted, Close>();
}
void Accepted::react(const Read) noexcept
{
    INFO("* Accepted * -> Read");
    transit<Accepted, Read>();
}
void Accepted::react(const Write) noexcept
{
    INFO("* Accepted * -> Write");
    transit<Accepted, Write>();
}

void Reading::react(const Read) noexcept
{
    INFO("* Reading * -> Read");
    transit<Reading, Read>();
}
void Reading::react(const Write) noexcept
{
    INFO("* Reading * -> Write");
    transit<Reading, Write>();
}
void Reading::react(const Close) noexcept
{
    INFO("* Reading * -> Close");
    transit<Reading, Close>();
}

void Writing::react(const Read) noexcept
{
    INFO("* Writing * -> Read");
    transit<Writing, Read>();
}
void Writing::react(const Write) noexcept
{
    INFO("* Writing * -> Write");
    transit<Writing, Write>();
}
void Writing::react(const Close) noexcept
{
    INFO("* Writing * -> Close");
    transit<Writing, Close>();
}

void Closed::react(const Connect) noexcept
{
    INFO("* Closed * -> Connect react"); 
    transit<Closed, Connect>();
}
void Closed::react(const Accept) noexcept
{
    INFO("* Closed * -> Accept react"); 
    transit<Closed, Accept>();
}
void Closed::react(const Read) noexcept
{
    INFO("* Closed * -> Read react"); 
    transit<Closed, Read>();
}
void Closed::react(const Write) noexcept
{
    INFO("* Closed * -> Write react"); 
    transit<Closed, Write>();
}
void Closed::react(const Close) noexcept
{
    INFO("* Closed * -> Close react"); 
    transit<Closed, Close>();
}
void Closed::react(const eError) noexcept
{
    INFO("* Closed * -> eError react"); 
    transit<Closed, eError>();
}

// Need to explicitly define the `set_initial_state` function
template<> void Fsm<FooFsm>::set_initial_state() noexcept
{
    current_state = &state_instance<Listening>;
}

int main()
{
    FooFsm fsm{};
    FooFsm::start();
    EventList<Connect, Accept, Read, Read, Write, Write, Close, Connect>::dispatch();
}
