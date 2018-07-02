/**
 * A simple implementation of a FSM parsing a single regular expression
 * regex: R"(^[A-Za-z][0-9]+$)"
 */

#include <cstdio>
#include <cctype>
#include <string>
#include <sstream>

#define INFO(M) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n", __FUNCTION__,__LINE__)
#define DEBUG(M, ...) std::fprintf(stdout, "[DEBUG %s, %d]: " M "\n",\
__FUNCTION__,__LINE__,##__VA_ARGS__)


using Event = char;

/**
 * Parses a single regular expression:
 * R"(^[A-Za-z][0-9]+$)"
 */
class ReFsm
{
public:
    enum class State {
        Alpha,
        Digit,
        Invalid
    };

    constexpr ReFsm() = default;

    template<typename Iter>
    bool operator()(Iter begin, Iter end) noexcept
    {
        while(begin != end && state_ != State::Invalid){
            event(*begin);
            ++begin;
        }
        return state_ != State::Invalid;
    }

    void event(Event e) noexcept
    {
        switch(state_)
        {
        case State::Alpha:
            handle_alpha(e);
            break;
        case State::Digit:
            handle_digit(e);
            break;
        case State::Invalid:
        default:
            handle_error(e);
            break;
        }
    }

protected:
    void transit_to(State s) noexcept
    {
        state_ = s;
    }

    void handle_alpha(Event e) noexcept
    {
        DEBUG("* State::Alpha *, event = %c", e);
        if(std::isalpha(static_cast<int>(e)))
        {
            buf_ << static_cast<char>(e);
            transit_to(State::Digit);
        }
        else
        {
            fprintf(stderr, "[Error]: Invalid character, alphabetic character expected\n");
            transit_to(State::Invalid);
        }
    }

    void handle_digit(Event e) noexcept
    {
        DEBUG("* State::Digit *, event = %c", e);
        if(std::isdigit(static_cast<int>(e)))
        {
            buf_ << static_cast<char>(e);
        }
        else
        {
            fprintf(stderr, "[Error]: Invalid character, digit expected\n");
            transit_to(State::Invalid);
        }
    }

    void handle_error(Event e) noexcept
    {
        DEBUG("* State::Invalid *, event = %c", e);
    }

private:
    State state_{State::Alpha};
    std::ostringstream buf_{};
};


int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("Usage: re_fsm string_to_validate\n");
        exit(EXIT_FAILURE);
    }
    auto re = ReFsm{};
    // const auto input = std::string(argv[1]);
    if(const auto input = std::string(argv[1]); re(input.cbegin(), input.cend()))
    {
        printf("matched");
    }
    else
    {
        printf("not matched");
    }
    return EXIT_SUCCESS;
}
