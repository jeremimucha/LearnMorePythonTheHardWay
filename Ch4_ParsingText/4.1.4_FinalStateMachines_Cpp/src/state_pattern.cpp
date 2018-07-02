#include <cstdio>
#include <utility>
#include <type_traits>
#include <boost/type_index.hpp>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


/* 

 */


struct Event { };
struct Connect : public Event { };
struct Accept  : public Event { };
struct Read    : public Event { };
struct Write   : public Event { };
struct Close   : public Event { };
struct eError  : public Event { };

class State
{
  public:
    void event(const Event) noexcept;
    virtual void event(const Connect) noexcept;
    virtual void event(const Accept) noexcept;
    virtual void event(const Read) noexcept;
    virtual void event(const Write) noexcept;
    virtual void event(const Close) noexcept;
    virtual void event(const eError) noexcept;
    virtual const State& self() const noexcept { return *this; };

    virtual void entry() noexcept;
    virtual void exit() noexcept;
};

class sError : public State
{
public:
    void entry() noexcept override;
    const sError& self() const noexcept final { return *this; }
};

class Listening : public State
{
public:
    void event(const Connect) noexcept override;
    void event(const eError) noexcept override;
    const Listening& self() const noexcept final { return *this; }
};

class Connected : public State
{
public:
    void event(const Accept) noexcept override;
    void event(const Close) noexcept override;
    const Connected& self() const noexcept final { return *this; }
};

class Accepted : public State
{
public:
    void event(const Close) noexcept override;
    void event(const Read) noexcept override;
    void event(const Write) noexcept override;
    const Accepted& self() const noexcept final { return *this; }
};

class Reading : public State
{
public:
    void event(const Read) noexcept override;
    void event(const Write) noexcept override;
    void event(const Close) noexcept override;
    const Reading& self() const noexcept final { return *this; }
};

class Writing : public State
{
public:
    void event(const Read) noexcept override;
    void event(const Write) noexcept override;
    void event(const Close) noexcept override;
    const Writing& self() const noexcept final { return *this; }
};

class Closed : public State
{
public:
    void event(const Connect) noexcept override;
    void event(const Accept) noexcept override;
    void event(const Read) noexcept override;
    void event(const Write) noexcept override;
    void event(const Close) noexcept override;
    void event(const eError) noexcept override;
    const Closed& self() const noexcept final { return *this; }
};

template<typename S, typename E> struct Event_transition_impl
{ using state_type = State; };

template<> struct Event_transition_impl<Listening, Connect>
{ using state_type = Connected; };
template<> struct Event_transition_impl<Listening, eError>
{ using state_type = Listening; };
template<typename E> struct Event_transition_impl<Listening, E>
{ using state_type = sError; };

template<> struct Event_transition_impl<Connected, Accept>
{ using state_type = Accepted; };
template<> struct Event_transition_impl<Connected, Close>
{ using state_type = Closed; };
template<typename E> struct Event_transition_impl<Connected, E>
{ using state_type = sError; };

template<> struct Event_transition_impl<Accepted,Close>
{ using state_type = Closed; };
template<> struct Event_transition_impl<Accepted,Read>
{ using state_type = Reading; };
template<> struct Event_transition_impl<Accepted,Write>
{ using state_type = Writing; };
template<typename E> struct Event_transition_impl<Accepted,E>
{ using state_type = sError; };

template<> struct Event_transition_impl<Reading, Read>
{ using state_type = Reading; };
template<> struct Event_transition_impl<Reading, Write>
{ using state_type = Writing; };
template<> struct Event_transition_impl<Reading, Close>
{ using state_type = Closed; };
template<typename E> struct Event_transition_impl<Reading, E>
{ using state_type = sError; };

template<> struct Event_transition_impl<Writing, Read>
{ using state_type = Reading; };
template<> struct Event_transition_impl<Writing, Write>
{ using state_type = Writing; };
template<> struct Event_transition_impl<Writing, Close>
{ using state_type = Closed; };
template<typename E> struct Event_transition_impl<Writing, E>
{ using state_type = sError; };

template<typename E> struct Event_transition_impl<Closed, E>
{ using state_type = Listening; };

template<typename E> struct Event_transition_impl<sError, E>
{ using state_type = sError; };

template<typename S, typename E>
using Event_transition = typename Event_transition_impl<S,E>::state_type;

template<typename S>
struct State_instance_impl
{
    using value_type = S;
    using type = State_instance_impl<S>;
    static value_type value;
};
// template<typename S>
// typename State_instance_impl<S>::value_type State_instance_impl<S>::value{};
using boost::typeindex::type_id_with_cvr;
template<typename S>
static inline typename State_instance_impl<S>::value_type State_instance{};

template<typename S, typename E>
constexpr decltype(auto) transit_event(const S&, const E&) noexcept
{ /* DEBUG("S: %s", type_id_with_cvr<S>().pretty_name().c_str());
  DEBUG("S: %s", type_id_with_cvr<E>().pretty_name().c_str()); */
    DEBUG("S: %s", __PRETTY_FUNCTION__);
    return &State_instance<Event_transition<
    std::remove_pointer_t<std::remove_cv_t<S>>,
    std::remove_reference_t<std::remove_cv_t<E>>
    >>; }


void State::event(const Event) noexcept
{
    INFO("Default Event handler");
}
void State::event(const Connect) noexcept
{
    INFO("Default Connect handler");
}
void State::event(const Accept) noexcept
{
    INFO("Default Accept handler");
}
void State::event(const Read) noexcept
{
    INFO("Default Read handler");
}
void State::event(const Write) noexcept
{
    INFO("Default Write handler");
}
void State::event(const Close) noexcept
{
    INFO("Default Close handler");
}
void State::event(const eError) noexcept
{
    INFO("Default Error handler");
}
void State::entry() noexcept
{
    INFO("Default entry event");
}
void State::exit() noexcept
{
    INFO("Default exit event");
}


void sError::entry() noexcept
{
    INFO("ENTER ERROR STATE");
}


void Listening::event(const Connect) noexcept
{
    INFO("* Listening * -> Connect");
}
void Listening::event(const eError) noexcept
{
    INFO("* Listening * -> sError");
}

void Connected::event(const Accept) noexcept
{
    INFO("* Connected * -> Accept");
}
void Connected::event(const Close) noexcept
{
    INFO("* Connected * -> Close");
}

void Accepted::event(const Close) noexcept
{
    INFO("* Accepted * -> Close");
}
void Accepted::event(const Read) noexcept
{
    INFO("* Accepted * -> Read");
}
void Accepted::event(const Write) noexcept
{
    INFO("* Accepted * -> Write");
}


void Reading::event(const Read) noexcept
{
    INFO("* Reading * -> Read");
}
void Reading::event(const Write) noexcept
{
    INFO("* Reading * -> Write");
}
void Reading::event(const Close) noexcept
{
    INFO("* Reading * -> Close");
}

void Writing::event(const Read) noexcept
{
    INFO("* Writing * -> Read");
}
void Writing::event(const Write) noexcept
{
    INFO("* Writing * -> Write");
}
void Writing::event(const Close) noexcept
{
    INFO("* Writing * -> Close");
}

void Closed::event(const Connect) noexcept
{
    INFO("* Closed * -> Connect event");
}
void Closed::event(const Accept) noexcept
{
    INFO("* Closed * -> Accept event");
}
void Closed::event(const Read) noexcept
{
    INFO("* Closed * -> Read event");
}
void Closed::event(const Write) noexcept
{
    INFO("* Closed * -> Write event");
}
void Closed::event(const Close) noexcept
{
    INFO("* Closed * -> Close event");
}
void Closed::event(const eError) noexcept
{
    INFO("* Closed * -> eError event");
}

class Fsm
{
public:
    constexpr Fsm() noexcept
        : p_state_{&State_instance<Listening>}
    {
    }

    template<typename S>
    static constexpr decltype(auto) state() noexcept
    {
        return State_instance<S>;
    }

    template<typename E>
    void event(E&& e) noexcept
    {
        p_state_->event(std::forward<E>(e));
        transit(e);
    }

    template<typename E>
    void transit(const E& e) noexcept
    {
        p_state_->exit();
DEBUG("S: %s",type_id_with_cvr<decltype(p_state_->self())>().pretty_name().c_str());
        if(Listening* ps = dynamic_cast<Listening*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Connected* ps = dynamic_cast<Connected*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Accepted* ps = dynamic_cast<Accepted*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Reading* ps = dynamic_cast<Reading*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Writing* ps = dynamic_cast<Writing*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Closed* ps = dynamic_cast<Closed*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(sError* ps = dynamic_cast<sError*>(p_state_))
            p_state_ = transit_event(ps, e);
        else
            p_state_ = transit_event(p_state_, e);
        p_state_->entry();
    }

    template<typename E, typename Action>
    void transit(const E& e, Action action) noexcept(noexcept(action()))
    {
        p_state_->exit();
DEBUG("S: %s",type_id_with_cvr<decltype(p_state_->self())>().pretty_name().c_str());
        action();
                if(Listening* ps = dynamic_cast<Listening*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Connected* ps = dynamic_cast<Connected*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Accepted* ps = dynamic_cast<Accepted*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Reading* ps = dynamic_cast<Reading*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Writing* ps = dynamic_cast<Writing*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Closed* ps = dynamic_cast<Closed*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(sError* ps = dynamic_cast<sError*>(p_state_))
            p_state_ = transit_event(ps, e);
        else
            p_state_ = transit_event(p_state_, e);
        p_state_->entry();
    }

    template<typename E, typename Action, typename Condition>
    void transit(const E& e, Action action, Condition condition) noexcept(noexcept(action()))
    {
        p_state_->exit();
DEBUG("S: %s",type_id_with_cvr<decltype(p_state_->self())>().pretty_name().c_str());
        if(condition())
            action();
        if(Listening* ps = dynamic_cast<Listening*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Connected* ps = dynamic_cast<Connected*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Accepted* ps = dynamic_cast<Accepted*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Reading* ps = dynamic_cast<Reading*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Writing* ps = dynamic_cast<Writing*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(Closed* ps = dynamic_cast<Closed*>(p_state_))
            p_state_ = transit_event(ps, e);
        else if(sError* ps = dynamic_cast<sError*>(p_state_))
            p_state_ = transit_event(ps, e);
        else
            p_state_ = transit_event(p_state_, e);
        p_state_->entry();
    }

private:
    State* p_state_;
};


int main()
{
    Fsm fsm;
    fsm.event(Connect{});
    fsm.event(Accept{});
    fsm.event(Read{});
    fsm.event(Read{});
    fsm.event(Write{});
    fsm.event(Write{});
    fsm.event(Close{});
    fsm.event(Connect{});
}
