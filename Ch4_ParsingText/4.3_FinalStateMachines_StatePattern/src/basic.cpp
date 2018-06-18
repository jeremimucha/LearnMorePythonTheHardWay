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

using EventStartFunction = void(*)(WatchState*);
using EventStopFunction = void(*)(WatchState*);

struct WatchState {
    const char* name;
    EventStartFunction start;
    EventStopFunction stop;
};

// Introduce a default implementation to break the dependency cycle.
// Introducing new events won't require a change in the existing states if the default
// implementation is ok.

void defaultStop(WatchState* state)
{
    DEBUG("Default Stop event in state %s", state->name);
}

void defaultStart(WatchState* state)
{
    DEBUG("Default Start event in state %s", state->name);
}

void defaultImplementation(WatchState* state)
{
    state->name = "DEFAULT";
    state->stop = defaultStop;
    state->start = defaultStart;
}

void transitionToStopped(WatchState* state);
void transitionToStarted(WatchState* state);

void startWatch(WatchState* state)
{
    DEBUG("Starting watch..., state = %s", state->name);
    transitionToStarted(state);
}

void transitionToStopped(WatchState* state)
{
    DEBUG("Transitioning to Stopped, state = %s", state->name);
    // init with default implementation before specifying the events to be handled in
    // the stopped state
    defaultImplementation(state);
    state->name = "STOPPED";
    state->start = startWatch;
}

void stopWatch(WatchState* state)
{
    DEBUG("Stopping watch..., state = %s", state->name);
    transitionToStopped(state);
}

void transitionToStarted(WatchState* state)
{
    DEBUG("Transitioning to Started, state = %s", state->name);
    defaultImplementation(state);
    state->name = "STARTED";
    state->stop = stopWatch;
}


// --- client code

struct DigitalStopWatch {
    WatchState state;
};

DigitalStopWatch make_watch()
{
    auto watch = DigitalStopWatch{};
    transitionToStopped(&watch.state);
    return watch;
}


int main()
{
    auto watch = make_watch();
    watch.state.start(&watch.state);
    watch.state.start(&watch.state);
    watch.state.stop(&watch.state);
    watch.state.stop(&watch.state);
}
