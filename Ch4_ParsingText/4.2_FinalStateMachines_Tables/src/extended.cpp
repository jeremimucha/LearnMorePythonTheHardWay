#include <cstdio>
#include <cstdlib>

#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


/* 
 * Another 'traditional' approach to FSM design in C.
 * This solution uses transition-table of events and states, also supports transition actions.
 * This is an implementation using function pointers to state actions and transition actions.
 */

enum State {
    State_Initial,
    State_Foo,
    State_Bar,
    NUM_STATES
    };

struct instance_data_t {
    int value;
};

using state_fp_t = State(*)(instance_data_t*);

State do_state_initial( instance_data_t* data );
State do_state_foo( instance_data_t* data);
State do_state_bar( instance_data_t* data);

static const state_fp_t StateTable[NUM_STATES] = {
    do_state_initial, do_state_foo, do_state_bar
    };

using transition_fp_t = void(*)(instance_data_t*);

void transit_initial_to_foo(instance_data_t*);
void transit_foo_to_bar(instance_data_t*);
void transit_bar_to_initial(instance_data_t*);
void transit_bar_to_foo(instance_data_t*);
void transit_bar_to_bar(instance_data_t*);

static const transition_fp_t TransitionTable[NUM_STATES][NUM_STATES] = {
//    initial               , foo                   , bar
    { nullptr               , transit_initial_to_foo, nullptr            }
   ,{ nullptr               , nullptr               , transit_foo_to_bar }
   ,{ transit_bar_to_initial, transit_bar_to_foo    , transit_bar_to_bar }
};

State run_state( State state, instance_data_t* data )
{
    const auto next_state = StateTable[state](data);
    auto* const transit = TransitionTable[state][next_state];
    if(transit) {
        transit(data);
    }
    return next_state;
}

int main()
{
    auto state = State{State_Initial};
    auto data = instance_data_t{1};
    while(true){
        state = run_state(state, &data);
    }
}

State do_state_initial(instance_data_t* data)
{
    DEBUG("Hello from do_state_initial! data = %i", data->value);
    if(data->value > 0)
        return State::State_Foo;
    else{
        DEBUG("We're done here, data = %i", data->value);
        std::exit(0);
    }
}

State do_state_foo(instance_data_t* data)
{
    DEBUG("Hello from do_state_foo! data = %i", data->value);
    return State::State_Bar;
}

State do_state_bar(instance_data_t* data)
{
    DEBUG("Hello from do_state_bar! data = %i", data->value);
    if(data->value < 49)
        return State::State_Foo;
    else if( data->value < 100 )
        return State::State_Bar;
    else
        return State::State_Initial;
}

void transit_initial_to_foo(instance_data_t* data)
{
    DEBUG("Initial startup stuff. data = %i", data->value);
    data->value = 42;
}

void transit_foo_to_bar(instance_data_t* data)
{
    ++data->value;
    DEBUG("foo -> bar, ++data = %i", data->value);
}

void transit_bar_to_initial(instance_data_t* data)
{
    data->value = -1;
    DEBUG("transition bar -> initial, data = %i", data->value);
}

void transit_bar_to_foo(instance_data_t* data)
{
    data->value += 2;
    DEBUG("transition bar -> foo, data = %i", data->value);
}

void transit_bar_to_bar(instance_data_t* data)
{
    data->value *= 2;
    DEBUG("transition bar -> bar, data = %i", data->value);
}
