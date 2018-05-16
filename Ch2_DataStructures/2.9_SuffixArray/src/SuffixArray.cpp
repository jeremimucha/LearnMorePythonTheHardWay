#include "SuffixArray.h"

std::ostream& operator<<(std::ostream& os, const SuffixArray& sa )
{
    os << "data = " << sa.data << "; suffixed = { ";
    for(auto it = sa.suffixes.cbegin(); ; ){
        os << *it;
        if( ++it != sa.suffixes.end() )
            os << ", ";
        else {
            os << " }";
            break;
        }
    }
    return os;
}
