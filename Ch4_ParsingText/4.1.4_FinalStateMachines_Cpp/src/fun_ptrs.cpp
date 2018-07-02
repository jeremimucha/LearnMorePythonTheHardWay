#include <cstdio>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


enum class Event {
    Connect,
    Accept,
    Read,
    Write,
    Close,
    Error,

    NumEvents
};

enum class State {
    Listening,
    Connected,
    Accepted,
    Reading,
    Writing,
    Closed,
    Error,

    NumStates
};

constexpr int num_events() noexcept { return static_cast<int>(Event::NumEvents); }
constexpr int num_states() noexcept { return static_cast<int>(State::NumStates); }

using state_handler_t = void(*)(Event e) noexcept;

void handle_listening(Event e) noexcept;
void handle_connected(Event e) noexcept;
void handle_accepted(Event e) noexcept;
void handle_reading(Event e) noexcept;
void handle_writing(Event e) noexcept;
void handle_closed(Event e) noexcept;
void handle_error(Event e) noexcept;

static constexpr state_handler_t StateTable[num_states()] = {
    handle_listening,
    handle_connected,
    handle_accepted,
    handle_reading,
    handle_writing,
    handle_closed,
    handle_error
};

static constexpr State TransitionTable[num_states()][num_events()] = {
//                Connect   , Accept    , Read      , Write     , Close     , Error
/* Listening */ { State::Connected, State::Error    , State::Error    , State::Error    , State::Error    , State::Listening }
/* Connected */,{ State::Error    , State::Accepted , State::Error    , State::Error    , State::Closed   , State::Error     }
/* Accepted  */,{ State::Error    , State::Error    , State::Reading  , State::Writing  , State::Closed   , State::Error     }
/* Reading   */,{ State::Error    , State::Error    , State::Reading  , State::Writing  , State::Closed   , State::Error     }
/* Writing   */,{ State::Error    , State::Error    , State::Reading  , State::Writing  , State::Closed   , State::Error     }
/* Closed    */,{ State::Listening, State::Listening, State::Listening, State::Listening, State::Listening, State::Listening }
/* Error     */,{ State::Error    , State::Error    , State::Error    , State::Error    , State::Error    , State::Error     }
    };


template<typename Sht, typename S, int Ns, int Ne>
class Fsm
{
    using S_table_ref = Sht(&)[Ns];
    using Tr_table_ref = const S(&)[Ns][Ne];
public:
    constexpr Fsm(Sht(&state_table_)[Ns], const State(&transit_table_)[Ns][Ne])
        : s_table{state_table_}, tr_table{transit_table_}
        { }
    explicit constexpr Fsm(state_handler_t initial_state) noexcept
        : p_state{initial_state}
        { }

    void start(State state) noexcept
    {
        current_state = state;
        p_state = s_table[static_cast<const int>(state)];
    }

    void react(Event e) noexcept
    {
        p_state(e);
        transit(e);
    }

    void transit(Event e) noexcept
    {
        current_state = tr_table[static_cast<const int>(current_state)][static_cast<const int>(e)];
        p_state = s_table[static_cast<const int>(current_state)];
    }

private:
    S_table_ref s_table;
    Tr_table_ref tr_table;
    State current_state;
    state_handler_t p_state{nullptr};
};

void handle_listening(Event e) noexcept
{
    DEBUG("* LISTENING *, event = %i", static_cast<int>(e));
}

void handle_connected(Event e) noexcept
{
    DEBUG("* CONNECTED *, event = %i", static_cast<int>(e));
}

void handle_accepted(Event e) noexcept
{
    DEBUG("* ACCEPTED *, event = %i", static_cast<int>(e));
}

void handle_reading(Event e) noexcept
{
    DEBUG("* READING *, event = %i", static_cast<int>(e));
}

void handle_writing(Event e) noexcept
{
    DEBUG("* WRITING *, event = %i", static_cast<int>(e));
}

void handle_closed(Event e) noexcept
{
    DEBUG("* CLOSED *, event = %i", static_cast<int>(e));
}

void handle_error(Event e) noexcept
{
    DEBUG("* ERROR *, event = %i", static_cast<int>(e));
}

template<typename Sht, int Ns, typename S, int Ne>
auto make_fsm(Sht(&stt)[Ns], const S(&trt)[Ns][Ne]) noexcept
{
    return Fsm<Sht,S,Ns,Ne>{stt,trt};
}

int main()
{
    auto fsm = make_fsm(StateTable, TransitionTable);
    fsm.start(State::Listening);
    Event events[] = {Event::Connect, Event::Accept, Event::Read,
                      Event::Write, Event::Close, Event::Connect
                     };
    for(size_t i=0; i<sizeof(events)/sizeof(events[0]); ++i){
        fsm.react(events[i]);
    }
}
