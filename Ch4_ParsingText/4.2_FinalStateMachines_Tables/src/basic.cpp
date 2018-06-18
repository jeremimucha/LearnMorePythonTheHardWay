#include <cstdio>

#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)

/* 
 * Another 'traditional' approach to FSM design in C.
 * This solution uses transition-table of events and states, also supports transition actions.
 */

template<typename T>
void* void_cast(T&& variable) noexcept { return reinterpret_cast<void*>(variable); }

enum State {
    stopped,
    started,
    NO_OF_STATES
};


enum Event {
    stopEvent,
    startEvent,
    NO_OF_EVENTS
};


struct DigitalStopWatch;
using Display = void(*)(DigitalStopWatch*);

struct DigitalStopWatch {
    State state;
    Display watchDisplay;
};


static State TransitionTable[NO_OF_STATES][NO_OF_EVENTS] = {
    { State::stopped, State::started },
    { State::stopped, State::started }
    };


void printTime(DigitalStopWatch* instance)
{
    printf("DigitalStopWatch[%p]: The time is 3:14\n", void_cast(instance));
}

void startWatch(DigitalStopWatch* instance)
{
    const auto current_state = instance->state;
    DEBUG("[%p]: startEvent from state %d", void_cast(instance), current_state);
    if(current_state != State::started)
        instance->watchDisplay(instance);
    instance->state = TransitionTable[current_state][startEvent];
}

void stopWatch(DigitalStopWatch* instance)
{
    const auto current_state = instance->state;
    DEBUG("[%p]: stopEvent from state %d", void_cast(instance), current_state);
    if(current_state != State::stopped)
        instance->watchDisplay(instance);
    instance->state = TransitionTable[current_state][stopEvent];
}


int main()
{
    auto watch = DigitalStopWatch{State::stopped, printTime};
    startWatch(&watch);
    startWatch(&watch);
    stopWatch(&watch);
    stopWatch(&watch);
}
