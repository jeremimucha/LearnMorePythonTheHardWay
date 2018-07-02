#include <cstdio>

#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


/* 
 * This is a 'traditiona' C - style FSM using conditionals - swtich/case statement.
 * It's a very simple design, but has some drawbacks - major code duplication in larger
 * FSM's, it's difficoult to maintain and extend, but it's very simple and easy to
 * understand if the FSM is small.
 */

template<typename T>
void* void_cast(T&& variable) noexcept { return reinterpret_cast<void*>(variable); }


struct WatchState;
struct DigitalStopWatch;

using EventStartFunction = void(*)(DigitalStopWatch*);
using EventStopFunction = void(*)(DigitalStopWatch*);

struct WatchState {
    const char* name;
    EventStartFunction start;
    EventStopFunction stop;
};

struct DigitalStopWatch {
    WatchState* state;
    // other instance variables here...
};

// Introduce a default implementation to break the dependency cycle.
// Introducing new events won't require a change in the existing states if the default
// implementation is ok.

void defaultStop(DigitalStopWatch* context)
{
    DEBUG("Default Stop event in state %s", context->state->name);
}

void defaultStart(DigitalStopWatch* context)
{
    DEBUG("Default Start event in state %s", context->state->name);
}

void defaultImplementation(WatchState* state)
{
    state->name = "DEFAULT";
    state->stop = defaultStop;
    state->start = defaultStart;
}

void changeState(DigitalStopWatch* instance, WatchState* next_state)
{
    DEBUG("Changing state from %s, to %s", instance->state->name, next_state->name);
    instance->state = next_state;
}

WatchState* transitionToStopped();
WatchState* transitionToStarted();

void startWatch(DigitalStopWatch* context)
{
    DEBUG("Starting watch..., state = %s", context->state->name);
    changeState(context, transitionToStarted());
}

WatchState* transitionToStopped()
{
    static WatchState stoppedState;
    static bool initialized = false;
    if(!initialized){
        defaultImplementation(&stoppedState);
        stoppedState.name = "STOPPED";
        stoppedState.start = startWatch;
        initialized = true;
    }
    DEBUG("Transitioning to Stopped, state = %s", stoppedState.name);
    // init with default implementation before specifying the events to be handled in
    // the stopped state
    return &stoppedState;
}

void stopWatch(DigitalStopWatch* context)
{
    DEBUG("Stopping watch..., state = %s", context->state->name);
    changeState(context, transitionToStopped());
}

WatchState* transitionToStarted()
{
    static WatchState startedState;
    static bool initialized = false;
    if(!initialized){
        defaultImplementation(&startedState);
        startedState.name = "STARTED";
        startedState.stop = stopWatch;
        initialized = true;
    }
    DEBUG("Transitioning to Started, state = %s", startedState.name);
    return &startedState;
}

// --- client code
DigitalStopWatch make_watch()
{
    auto tempstate = WatchState{"INITIAL STATE", nullptr, nullptr};
    auto watch = DigitalStopWatch{&tempstate};
    changeState(&watch, transitionToStopped());
    return watch;
}


int main()
{
    auto watch = make_watch();
    watch.state->start(&watch);
    watch.state->start(&watch);
    watch.state->stop(&watch);
    watch.state->stop(&watch);
}
