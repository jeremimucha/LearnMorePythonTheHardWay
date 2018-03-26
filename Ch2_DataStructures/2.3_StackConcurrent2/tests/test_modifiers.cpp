#include "catch/catch.hpp"
#include "ThreadsafeStack/ThreadsafeStack.h"


TEST_CASE( "Adding elements to the stack", "[push][emplace]" )
{
    auto cut_stack = ThreadsafeStack<int>{};
    REQUIRE(assert_invariant(cut_stack));
    
    SECTION( "void push(const T&)" ){
        const auto value = 42;
        cut_stack.push(value);
        REQUIRE_FALSE(cut_stack.empty());
        auto ret = cut_stack.try_pop();
        REQUIRE(cut_stack.empty());
        REQUIRE(ret != nullptr);
        REQUIRE(*ret == value);
    }

    SECTION( "void push(T&&)" ){
        cut_stack.push(42);
        REQUIRE_FALSE(cut_stack.empty());
        auto ret = cut_stack.try_pop();
        REQUIRE(cut_stack.empty());
        REQUIRE(ret != nullptr);
        REQUIRE(*ret == 42);
    }

    SECTION( "void emplace(Args&&...)" ){
        cut_stack.emplace(42);
        REQUIRE_FALSE(cut_stack.empty());
        auto ret = cut_stack.try_pop();
        REQUIRE(cut_stack.empty());
        REQUIRE(ret != nullptr);
        REQUIRE(*ret == 42);
    }
}

TEST_CASE( "Removing elements from the stack", "[try_pop][wait_and_pop]" )
{
    auto cut_stack = ThreadsafeStack<int>{};
    cut_stack.push(1);
    cut_stack.push(2);
    cut_stack.push(3);
    REQUIRE(assert_invariant(cut_stack));
    REQUIRE_FALSE(cut_stack.empty());

    SECTION( "bool try_pop(T&)" ){
        auto value = 0;
        REQUIRE(cut_stack.try_pop(value));
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 3);
        REQUIRE(cut_stack.try_pop(value));
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 2);
        REQUIRE(cut_stack.try_pop(value));
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 1);
        REQUIRE(cut_stack.empty());
    }

    SECTION( "void wait_and_pop(T&)" ){
        auto value = 0;
        cut_stack.wait_and_pop(value);
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 3);
        cut_stack.wait_and_pop(value);
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 2);
        cut_stack.wait_and_pop(value);
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(value == 1);
        REQUIRE(cut_stack.empty());
    }

    SECTION( "std::unique_ptr<int> try_pop()" ){
        auto pval1 = cut_stack.try_pop();
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(pval1 != nullptr);
        REQUIRE(*pval1 == 3);
        auto pval2 = cut_stack.try_pop();
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(pval2 != nullptr);
        REQUIRE(*pval2 == 2);
        auto pval3 = cut_stack.try_pop();
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(pval3 != nullptr);
        REQUIRE(*pval3 == 1);
        REQUIRE(cut_stack.empty());
        auto pnull = cut_stack.try_pop();
        REQUIRE(assert_invariant(cut_stack));
        REQUIRE(pnull == nullptr);
    }

// will pass only in concurrent code apparently
    // SECTION( "std::unique_ptr<int> wait_and_pop()" ){
    //     auto pval1 = cut_stack.wait_and_pop();
    //     REQUIRE(assert_invariant(cut_stack));
    //     REQUIRE(pval1 != nullptr);
    //     REQUIRE(*pval1 == 3);
    //     auto pval2 = cut_stack.wait_and_pop();
    //     REQUIRE(assert_invariant(cut_stack));
    //     REQUIRE(pval2 != nullptr);
    //     REQUIRE(*pval2 == 2);
    //     auto pval3 = cut_stack.wait_and_pop();
    //     REQUIRE(assert_invariant(cut_stack));
    //     REQUIRE(pval3 != nullptr);
    //     REQUIRE(*pval3 == 1);
    //     REQUIRE(cut_stack.empty());
    //     auto pnull = cut_stack.wait_and_pop();
    //     REQUIRE(assert_invariant(cut_stack));
    //     REQUIRE(pnull == nullptr);
    // }
}
