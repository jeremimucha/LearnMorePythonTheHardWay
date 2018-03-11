#include "catch/catch.hpp"
#include "single_linked_list/single_linked_list.hpp"


TEST_CASE( "SLL can be copy and move constructed", "[constructor][copy][move]" )
{
    auto A = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};
    
    SECTION( "SLL can be copy constructed" ){
        auto out = SingleLinkedList<int>(A);
        REQUIRE( !A.empty() );
        REQUIRE( !out.empty() );
        REQUIRE( out.size() == A.size() );
        REQUIRE( std::equal(A.cbegin(), A.cend(), out.cbegin()) );
    }

    SECTION( "SLL can be move constructed" ){
        auto B = SingleLinkedList<int>(A);
        REQUIRE( !B.empty() );
        auto out = SingleLinkedList<int>(std::move(B));
        REQUIRE( !out.empty() );
        REQUIRE( out.size() == A.size() );
        REQUIRE( std::equal(A.cbegin(), A.cend(), out.cbegin()) );
        REQUIRE( B.empty() );
        REQUIRE( B.size() == 0 );
    }
}


TEST_CASE( "SLL assignment operator", "[assignment]" )
{
    auto A = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};

    SECTION( "SLL can be copy assigned", "[copy][assignment]" ){
        auto out = SingleLinkedList<int>{42};
    }
}
TEST_CASE( "SLL can be copy assigned", "[copy][assignment]" )
{

}

TEST_CASE( "SLL can be move assigned", "[move][assignment]" )
{

}

TEST_CASE( "SLL can be assigned an initializer_list", "[assignment]" )
{

}
