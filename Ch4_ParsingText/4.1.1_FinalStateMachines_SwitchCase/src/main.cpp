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

enum class State {
    stopped,
    started
};

struct DigitalStopWatch;
using Display = void(*)(DigitalStopWatch*);

struct DigitalStopWatch {
    State state;
    Display watchDisplay;
};

void printTime(DigitalStopWatch* instance)
{
    printf("DigitalStopWatch[%p]: The time is 3:14\n", void_cast(instance));
}

void startWatch(DigitalStopWatch* instance)
{
    switch(instance->state)
    {
    case State::started:
        DEBUG("[%p]: Already Started -> do nothing", void_cast(instance));
        break;
    case State::stopped:
        DEBUG("[%p]: Starting the digital stopwatch...", void_cast(instance));
        instance->watchDisplay(instance);
        instance->state = State::started;
        break;
    default:
        DEBUG("[%p]: ERROR: illegal state!", void_cast(instance));
        break;
    }
}

void stopWatch(DigitalStopWatch* instance)
{
    switch(instance->state)
    {
    case State::started:
        DEBUG("[%p]: Stopping the stopwatch...", void_cast(instance));
        instance->watchDisplay(instance);
        instance->state = State::stopped;
        break;
    case State::stopped:
        DEBUG("[%p]: Already stopped -> do nothing", void_cast(instance));
        break;
    default:
        DEBUG("[%p]: ERROR: illegal state", void_cast(instance));
        break;
    }
}


int main()
{
    auto watch = DigitalStopWatch{State::stopped, printTime};
    startWatch(&watch);
    startWatch(&watch);
    stopWatch(&watch);
    stopWatch(&watch);
}
