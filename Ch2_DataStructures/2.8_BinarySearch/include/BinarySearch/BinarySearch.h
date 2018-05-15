#ifndef BINARY_SEARCH_GUARD_H_
#define BINARY_SEARCH_GUARD_H_

#include <iostream>
#include <iterator>


template<typename Iterator, typename T>
Iterator binarySearch(Iterator first, Iterator last, const T& val)
{
    auto not_found = last;
    auto mid = first;
    while(first != last){
        auto distance = std::distance(first,last);
        mid = first;
        std::advance(mid, distance/2);
        if(val == *mid) return mid;
        if(val < *mid)
            last = mid;
        else
            first = ++mid;
    }

    return not_found;
}


#endif /* BINARY_SEARCH_GUARD_H_ */
