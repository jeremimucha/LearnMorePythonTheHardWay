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
    auto out = SingleLinkedList<int>(42, 42);
    REQUIRE( !A.empty() );
    REQUIRE( A.front() == 0 );
    REQUIRE( !out.empty() );
    REQUIRE( out.front() == 42 );

    SECTION( "SLL can be copy assigned", "[copy][assignment]" ){
        auto A_copy = A;    // to later check if A hasn't ben mutated
        out = A;
        REQUIRE( out.size() == A_copy.size() );
        REQUIRE( std::equal(out.cbegin(), out.cend(), A_copy.cbegin()) );
        REQUIRE( A.size() == A_copy.size() );
        REQUIRE( std::equal(A.cbegin(), A.cend(), A_copy.cbegin()) );
    }

    SECTION( "SLL can be move assigned", "[move][assignment]" ){
        auto out_copy = out;
        {
            auto A_copy = A;
            out = std::move(A_copy);
            REQUIRE( out.size() == A.size() );
            REQUIRE( std::equal(out.cbegin(), out.cend(), A.cbegin()) );
            // A_copy temporarily manages the elements previously owned by 'out'
            REQUIRE( A_copy.size() == out_copy.size() );
            REQUIRE( std::equal(A_copy.cbegin(), A_copy.cend(), out_copy.cbegin()) );
        }
        // How do we test if A_copy actually destroyed the elements?
    }

    SECTION( "SLL can be assigned an initializer_list", "[assignment]" ){
        auto ilist = {9,8,7,6,5,4,3,2,1,0};
        out = ilist;
        REQUIRE( out.size() == ilist.size() );
        REQUIRE( std::equal(out.cbegin(), out.cend(), ilist.begin()) );
    }
}
