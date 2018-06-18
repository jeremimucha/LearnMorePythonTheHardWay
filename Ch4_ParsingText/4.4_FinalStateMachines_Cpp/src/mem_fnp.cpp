#include <cstdio>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


/* 
 * Very simple, non-object oriented design using enums for Events and States.
 * Adding a new state implies adding a member state handler function, adding
 * an entry to the StateTable and a row to the TransitionTable. The new state handler
 * needs to handle calling the `transition()` member, potentially with an action
 * and condition guard.
 * Adding an event implies adding a column to the Transition table and potentially
 * extending the existing states with appropriate transition actions and cond guards.
 */

template<typename T>
void* void_cast(T&& variable) noexcept { return reinterpret_cast<void*>(variable); }

enum Event {
    eConnect,
    eAccept,
    eRead,
    eWrite,
    eClose,
    eError,

    eNumEvents
};
constexpr int num_events() noexcept { return static_cast<int>(Event::eNumEvents); }

enum State {
    sListening,
    sConnected,
    sAccepted,
    sReading,
    sWriting,
    sClosed,
    sError,

    sNumStates
};
constexpr int num_states() noexcept {return static_cast<int>(State::sNumStates); }

class FsmFoo
{
public:

    void event(Event e)
    {
        (this->*StateTable[cur_state_])(e);
    }

protected:
    void do_listening(Event e)
    {
        DEBUG("* LISTENING *, event = %i", static_cast<int>(e));
        transit(e);
    }

    void do_connected(Event e)
    {
        DEBUG("* CONNECTED *, event = %i", static_cast<int>(e));
        transit(e);
    }

    void do_accepted(Event e)
    {
        DEBUG("* ACCEPTED *, event = %i", static_cast<int>(e));

        const auto cond = [](Event e){
            return e == Event::eRead || e == Event::eWrite;
        };
        const auto act = [this](Event e){
            (this->*StateTable[cur_state_])(e);
        };
        transit(e, act, cond);
    }

    void do_reading(Event e)
    {
        DEBUG("* READING *, event = %i", static_cast<int>(e));

        const auto cond = [](Event e){
            return e == Event::eWrite;
        };
        const auto act = [this](Event e){
            (this->*StateTable[cur_state_])(e);
        };
        transit(e, act, cond);
    }

    void do_writing(Event e)
    {
        DEBUG("* WRITING *, event = %i", static_cast<int>(e));

        const auto cond = [](Event e){
            return e == Event::eRead;
        };
        const auto act = [this](Event e){
            (this->*StateTable[cur_state_])(e);
        };
        transit(e, act, cond);
    }

    void do_closed(Event e)
    {
        DEBUG("* CLOSED *, event = %i", static_cast<int>(e));
        const auto cond = [](Event){ return true; };
        const auto act = [this](Event e){
            (this->*StateTable[cur_state_])(e);
        };
        transit(e, act, cond);
    }

    void do_error(Event e)
    {
        DEBUG("* ERROR *, event = %i", static_cast<int>(e));
        transit(e);
    }

    void transit(Event e)
    {
        cur_state_ = TransitionTable[cur_state_][e];
    }

    template<typename Action>
    void transit(Event e, Action act)
    {
        cur_state_ = TransitionTable[cur_state_][e];
        act(e);
    }

    template<typename Action, typename Condition>
    void transit(Event e, Action act, Condition cond)
    {
        cur_state_ = TransitionTable[cur_state_][e];
        if(cond(e)){
            act(e);
        }
    }

private:
    State cur_state_{sListening};
    
    using State_mfp_t = void (FsmFoo::*)(Event);
    static constexpr State_mfp_t StateTable[State::sNumStates] = {
        &FsmFoo::do_listening,
        &FsmFoo::do_connected,
        &FsmFoo::do_accepted,
        &FsmFoo::do_reading,
        &FsmFoo::do_writing,
        &FsmFoo::do_closed,
        &FsmFoo::do_error
    };

    static constexpr State TransitionTable[State::sNumStates][Event::eNumEvents] = {
//                Connect   , Accept    , Read      , Write     , Close     , Error
/* Listening */ { sConnected, sError    , sError    , sError    , sError    , sListening }
/* Connected */,{ sError    , sAccepted , sError    , sError    , sClosed   , sError     }
/* Accepted  */,{ sError    , sError    , sReading  , sWriting  , sClosed   , sError     }
/* Reading   */,{ sError    , sError    , sReading  , sWriting  , sClosed   , sError     }
/* Writing   */,{ sError    , sError    , sReading  , sWriting  , sClosed   , sError     }
/* Closed    */,{ sListening, sListening, sListening, sListening, sListening, sListening }
/* Error     */,{ sError    , sError    , sError    , sError    , sError    , sError     }
    };
};

int main()
{
    FsmFoo fsm;
    Event events[] = {Event::eConnect, Event::eAccept, Event::eRead,
                      Event::eWrite, Event::eClose, Event::eConnect
                     };
    for(size_t i=0; i<sizeof(events)/sizeof(events[0]); ++i){
        fsm.event(events[i]);
    }
}
