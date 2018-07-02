#include <cstdio>
#include <utility>
#include <type_traits>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)

// template<typename S>
// struct State_instance_
// {
//     using state_type = S;
//     static state_type value;
// };

template<typename Base>
class Fsm
{
public:
    using fsm_type = Fsm<Base>;
    using state_ptr_t = Base*;

    template<typename S>
    static constexpr S& state()
    {
        return state_instance<S>;
    }


    template<typename S>
    static void set_initial_state() noexcept
    {
        current_state = &state_instance<S>;
    }

    static void reset() noexcept { };

    static void enter() noexcept
    {
        current_state->entry();
    }

    template<typename S>
    static void start() noexcept
    {
        set_initial_state<S>();
        enter();
    }

    template<typename E>
    static void dispatch(const E& event) noexcept
    {
        current_state->react(event);
    }

protected:

    Fsm() noexcept = default;
    friend Base;

    template<typename S>
    void transit() noexcept
    {
        current_state->exit();
        current_state = &state_instance<S>;
        current_state->entry();
    }

    template<typename S, typename Action>
    void transit(Action action) noexcept
    {
        current_state->exit();
        action();
        current_state = &state_instance<S>;
    }

    template<typename S, typename Action, typename Condition>
    void transit(Action action, Condition condition) noexcept
    {
        if(condition()) { transit<S>(action); }
    }

    template<typename S>
    static inline S state_instance{};
    static state_ptr_t current_state;
};

template<typename Base>
typename Fsm<Base>::state_ptr_t Fsm<Base>::current_state{};
// template<typename Base>
//     template<typename S>
// S Fsm<Base>::state_instance{};

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
};

class Connected : public FooFsm
{
public:
    void react(const Accept) noexcept override;
    void react(const Close) noexcept override;
};

class Accepted : public FooFsm
{
public:
    void react(const Close) noexcept override;
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
};

class Reading : public FooFsm
{
public:
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void react(const Close) noexcept override;
};

class Writing : public FooFsm
{
public:
    void react(const Read) noexcept override;
    void react(const Write) noexcept override;
    void react(const Close) noexcept override;
};

class Closed : public FooFsm
{
public:
    void react(const Event&) noexcept override;
    // void react(const Connect) noexcept override;
    // void react(const Accept) noexcept override;
    // void react(const Read) noexcept override;
    // void react(const Write) noexcept override;
    // void react(const Close) noexcept override;
    // void react(const eError) noexcept override;
};

void FooFsm::react(const Event&) noexcept { INFO("Default Event handler"); transit<sError>();}
void FooFsm::react(const Connect) noexcept { INFO("Default Connect handler"); transit<Listening>(); }
void FooFsm::react(const Accept) noexcept { INFO("Default Accept handler"); transit<Listening>(); }
void FooFsm::react(const Read) noexcept { INFO("Default Read handler"); transit<Listening>(); }
void FooFsm::react(const Write) noexcept { INFO("Default Write handler"); transit<Listening>(); }
void FooFsm::react(const Close) noexcept { INFO("Default Close handler"); transit<Listening>(); }
void FooFsm::react(const eError) noexcept { INFO("Default Error handler"); transit<Listening>(); }
void FooFsm::entry() noexcept { INFO("Default entry react"); }
void FooFsm::exit() noexcept { INFO("Default exit react"); }

void sError::entry() noexcept { INFO("ENTER ERROR STATE"); }

void Listening::react(const Connect) noexcept { INFO("* Listening * -> Connect"); transit<Connected>(); }
void Listening::react(const eError) noexcept { INFO("* Listening * -> sError"); transit<Listening>(); }

void Connected::react(const Accept) noexcept { INFO("* Connected * -> Accept"); transit<Accepted>();}
void Connected::react(const Close) noexcept { INFO("* Connected * -> Close"); transit<Closed>();}

void Accepted::react(const Close) noexcept { INFO("* Accepted * -> Close"); transit<Closed>();}
void Accepted::react(const Read) noexcept { INFO("* Accepted * -> Read"); transit<Reading>();}
void Accepted::react(const Write) noexcept { INFO("* Accepted * -> Write"); transit<Writing>();}

void Reading::react(const Read) noexcept { INFO("* Reading * -> Read"); transit<Reading>();}
void Reading::react(const Write) noexcept { INFO("* Reading * -> Write"); transit<Writing>();}
void Reading::react(const Close) noexcept { INFO("* Reading * -> Close"); transit<Closed>();}

void Writing::react(const Read) noexcept { INFO("* Writing * -> Read"); transit<Reading>();}
void Writing::react(const Write) noexcept { INFO("* Writing * -> Write"); transit<Writing>();}
void Writing::react(const Close) noexcept { INFO("* Writing * -> Close"); transit<Closed>();}

void Closed::react(const Event&) noexcept { INFO("* Closed * -> Connect react"); transit<Listening>();}
// void Closed::react(const Connect) noexcept { INFO("* Closed * -> Connect react"); }
// void Closed::react(const Accept) noexcept { INFO("* Closed * -> Accept react"); }
// void Closed::react(const Read) noexcept { INFO("* Closed * -> Read react"); }
// void Closed::react(const Write) noexcept { INFO("* Closed * -> Write react"); }
// void Closed::react(const Close) noexcept { INFO("* Closed * -> Close react"); }
// void Closed::react(const eError) noexcept { INFO("* Closed * -> eError react"); }

// template<typename... EEs> struct EventList;
// template<> struct EventList<>
// {
//     static void dispatch() { }
// };

// template<typename E, typename... EEs>
// struct EventList<E, EEs...>
// {
//     using fsm_type = FooFsm;
//     static void dispatch()
//     {
//         fsm_type::template dispatch<E>(E{});
//         EventList<EEs...>::dispatch();
//     }
// };

template<typename M>
struct FsmEventList
{
    template<typename... Es> struct eList;
    
    // template<> struct eList<>
    // {
    //     static void dispatch() { }
    // };

    template<typename E, typename... Es>
    struct eList<E, Es...>
    {
        using fsm_type = M;
        static void dispatch()
        {
            fsm_type::template dispatch<E>(E{});
            eList<Es...>::dispatch();
        }
    };
};

template<>
    template<> struct
FsmEventList<FooFsm>::eList<>
{
    static void dispatch() { }
};

template<typename E, typename... Es>
using EventList = FsmEventList<FooFsm>::eList<E, Es...>;

int main()
{
    FooFsm fsm{};
    fsm.start<Listening>();
    EventList<Connect, Accept, Read, Read, Write, Write, Close, Connect>::dispatch();
}
