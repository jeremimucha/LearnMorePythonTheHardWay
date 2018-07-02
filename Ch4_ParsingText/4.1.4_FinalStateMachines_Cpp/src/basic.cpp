#include <cstdio>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


/* 

 */

template<typename T>
void* void_cast(T&& variable) noexcept { return reinterpret_cast<void*>(variable); }

enum class Event {
    connect,
    accept,
    read,
    write,
    close,
    error
};

class FsmFoo
{
public:
    enum State {
        sListening,
        sConnected,
        sAccepted,
        sReading,
        sWriting,
        sClosed,
        sError
    };

    FsmFoo() = default;

    void event(Event e)
    {
        switch(state_)
        {
        case sListening:
            do_listening(e);
            break;
        case sConnected:
            do_connected(e);
            break;
        case sAccepted:
            do_accepted(e);
            break;
        case sReading:
            do_reading(e);
            break;
        case sWriting:
            do_writing(e);
            break;
        case sClosed:
            do_closed(e);
            break;
        case sError:
        default:
            do_error(e);
            break;
        }
    }

protected:
    void do_listening(Event e)
    {
        DEBUG("* LISTENING *, event = %i", static_cast<int>(e));
        switch(e)
        {
        case Event::connect:
            INFO(" Connecting... ");
            state_ = sConnected;
            break;
        case Event::error:
            INFO(" Handling error... ");
            break;
        default:
            INFO(" Invalid event in listening state...");
            state_ = sError;
            break;
        }
    }

    void do_connected(Event e)
    {
        DEBUG("* CONNECTED *, event = %i", static_cast<int>(e));
        switch(e)
        {
        case Event::accept:
            INFO(" Accepting... ");
            state_ = sAccepted;
            break;
        case Event::close:
            INFO(" Closing... ");
            state_ = sClosed;
            break;
        default:
            INFO(" Invalid event ");
            state_ = sError;
            break;
        }
    }

    void do_accepted(Event e)
    {
        DEBUG("* ACCEPTED *, event = %i", static_cast<int>(e));
        switch(e)
        {
        case Event::close:
            INFO(" Closing...");
            state_ = sClosed;
            break;
        case Event::read:
            INFO(" reading...");
            state_ = sReading;
            do_reading(e);
            break;
        case Event::write:
            INFO(" Writing...");
            state_ = sWriting;
            do_writing(e);
            break;
        default:
            INFO(" Invalid event");
            state_ = sError;
            break;
        }
    }

    void do_reading(Event e)
    {
        DEBUG("* READING *, event = %i", static_cast<int>(e));
        switch(e)
        {
        case Event::read:
            INFO(" Reading...");
            state_ = sReading;
            break;
        case Event::write:
            INFO(" Writing...");
            do_writing(e);
            break;
        case Event::close:
            INFO(" Closing...");
            state_ = sClosed;
            break;
        default:
            INFO(" Invalid event");
            state_ = sError;
            break;
        }
    }

    void do_writing(Event e)
    {
        DEBUG("* WRITING *, event = %i", static_cast<int>(e));
        switch(e)
        {
        case Event::read:
            INFO(" Reading...");
            do_reading(e);
            break;
        case Event::write:
            INFO(" Writing...");
            state_ = sWriting;
            break;
        case Event::close:
            INFO(" Closing...");
            state_ = sClosed;
            break;
        default:
            INFO(" Invalid state...");
            state_ = sError;
            break;
        }
    }

    void do_closed(Event e)
    {
        DEBUG("* CLOSED *, event = %i", static_cast<int>(e));
        state_ = sListening;
    }

    void do_error(Event e)
    {
        DEBUG("* ERROR *, event = %i", static_cast<int>(e));
        state_ = sError;
    }
private:
    State state_{sListening};
};


int main()
{
    FsmFoo fsm;
    Event events[] = {Event::connect, Event::accept, Event::read,
                      Event::write, Event::close, Event::connect
                     };
    for(size_t i=0; i<sizeof(events)/sizeof(events[0]); ++i){
        fsm.event(events[i]);
    }
}
