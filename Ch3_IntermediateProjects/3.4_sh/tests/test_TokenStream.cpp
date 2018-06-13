#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "TokenStream.h"


namespace
{
using namespace std::string_literals;

class TokenStreamTest : public ::testing::Test{
};

TEST_F(TokenStreamTest, get)
{
    auto iss = std::istringstream{"command ; another | \\;more\n"s};
    auto ts = TokenStream{iss};
    ASSERT_TRUE(ts);
    auto token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Terminator);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Pipe);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Terminator);

    iss = std::istringstream{Quit};
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Quit);
}

TEST_F(TokenStreamTest, putback)
{
    auto iss = std::istringstream{"command; another"};
    auto ts = TokenStream{iss};
    ASSERT_TRUE(ts);

    auto token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    ASSERT_EQ(token.value(), "command");

    ts.putback(token);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    ASSERT_EQ(token.value(), "command");
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Terminator);
}

TEST_F(TokenStreamTest, ignore)
{
    auto iss = std::istringstream{"command; ; another"};
    auto ts = TokenStream{iss};
    ASSERT_TRUE(ts);

    auto token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
    ts.putback(token);
    ts.ignore(token.value());
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Terminator);
    ts.ignore(Kind::Terminator);
    token = ts.get();
    ASSERT_EQ(token.kind(), Kind::Cmd);
}

} // namespace
