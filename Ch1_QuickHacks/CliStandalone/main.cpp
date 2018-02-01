#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <unordered_map>
#include <memory>
#include <utility>


/*
 * Parsing command line arguments.
 *  - Count is known beforehand - we're done once we reach argc
 * 
 *  - Types of arguments:
 *  - Option - argument that takes a value parameter (optional?)
 *             It should be possible to specify the type of the parameter.
 *             Allow strictly single-value options for now.
 *  - Flag   - argument that takes no parameters - implicitly a bool Option.
 *  - Positional arguments - args passed on the command line without specifying
 *  - an option.
 * 
 *  - Short (-a) and long (--alpha) forms are essentially a user interface
 *  - Determine the type of arg/option based on how it's been defined.
 *  - Consider other characters that might appear on the command line.
 *  - Handle only '-' '--' (option indicators) and '=' (option parameter)
 */

int main()
{
    std::cout << "Hello World!" << std::endl;
}
