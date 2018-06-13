#include <gtest/gtest.h>

#include "Token.h"


namespace
{

class TokenTest : public ::testing::Test {
};

TEST_F(TokenTest, KindCtor)
{
    const auto t_quit = Token(Kind::Quit);
    ASSERT_EQ(t_quit.kind(), Kind::Quit);
}

TEST_F(TokenTest, ColonTerminatorCtor)
{
    const auto t_term1 = Token(Colon);
    ASSERT_EQ(t_term1.kind(), Kind::Terminator);
}

TEST_F(TokenTest, EolTerminatorCtor)
{
    const auto t_term2 = Token(Eol);
    ASSERT_EQ(t_term2.kind(), Kind::Terminator);
}

TEST_F(TokenTest, EscapeSlashCtor)
{
    const auto t_EscapeSlash = Token(EscapeSlash);
    ASSERT_EQ(t_EscapeSlash.kind(), Kind::EscapeSlash);
}

TEST_F(TokenTest, PipeCtor)
{
    const auto t_Pipe = Token(Pipe);
    ASSERT_EQ(t_Pipe.kind(), Kind::Pipe);
}

TEST_F(TokenTest, ValueCtor)
{
    const auto t_cmd = Token("some_command");
    ASSERT_EQ(t_cmd.kind(), Kind::Cmd);
}

TEST_F(TokenTest, GetValue)
{
    const auto cmd = "command";
    const auto t_cmd = Token(cmd);
    ASSERT_EQ(t_cmd.value(), cmd);
}

TEST_F(TokenTest, Comparison)
{
    const auto t1 = Token(Kind::Terminator);
    const auto t2 = t1;
    ASSERT_EQ(t1,t2);
    const auto t3 = Token("some_command");
    ASSERT_NE(t1,t3);
}

} // namespace
