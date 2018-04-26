#include "UnorderedMap/UnorderedMap.h"
#include <string>



int main()
{
    UnorderedMap<int, std::string> map{};
    for(int i=0; i<100000; ++i){
        map.insert({i, std::to_string(i)});
    }
    for(int i=0; i<100000; ++i){
        map.erase(i);
    }

    return 0;
}
