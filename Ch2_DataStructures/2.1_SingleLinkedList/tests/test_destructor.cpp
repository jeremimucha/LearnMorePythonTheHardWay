#include "catch/catch.hpp"
#include "single_linked_list/single_linked_list.hpp"


TEST_CASE( "SLL can be destroyed", "[destructor]" )
{
    SECTION( "destructor" ){
        auto p_sll = new SingleLinkedList<int>(42);
        REQUIRE( !p_sll->empty() );
        REQUIRE( p_sll->size() == 42 );
        delete p_sll;
    // object has been destroyed but should still be in a state that let's us
    // use it (?)
        REQUIRE( p_sll->empty() );
        REQUIRE( p_sll->size() == 0 );
    }
}
