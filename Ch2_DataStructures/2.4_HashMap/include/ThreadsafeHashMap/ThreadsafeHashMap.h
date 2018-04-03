#ifndef THREADSAFE_HASH_MAP_H_
#define THREADSAFE_HASH_MAP_H_


#include <forward_list>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <utility>
#include <mutex>
#include <shared_mutex>


/* ThreadsafeHashMap */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash=std::hash<Key>>
class ThreadsafeHashMap
{
    class HashBucket
    {
        using backet_value          = std::pair<Key,Value>;
        using bucket_data           = std::forward_list<bucket_value>;
        using bucket_iterator       = typename bucket_data::iterator;
        using bucket_const_iterator = typename bucket_data::const_iterator;
        using shared_mutex_type     = std::shared_mutex;
    public:
        Value value_for(const Key& key, const Value& default_value) const
        {
            std::shared_lock<shared_mutex_type> lock{mutex};
            const bucket_iterator found_entry = find_entry_for(key);
            return (found_entry == data.end()) ?
                        default_value : found_entry->second;
        }

        void add_or_update_mapping(const Key& key, const Value& value)
        {
            std::unique_lock<shared_mutex_type> lock{mutex};
            const bucket_iterator found_entry = find_entry_for(key);
            if(found_entry == data.end()){
                data.push_front(bucket_value{key,value});
            }
            else{
                found_entry->second = value;
            }
        }

        void remove_mapping(const Key& key)
        {
            std::unique_lock<shared_mutex_type> lock{mutex};
            const bucket_iterator found_entry = find_entry_for(key);
            if(found_entry != data.end())
        }

    private:
        bucket_iterator find_entry_for( const Key& key ) const noexcept
        {
            return std::find_if(data.begin(), data.end(),
                                [this,&key](const bucket_value& item)
                                {return item.first == key});
        }

        std::pair<bucket_iterator,bucket_iterator> find_entry_before( const Key& key ) const noexcept
        {
            // TODO implement finding entry in a forward list that allows for
            // deleting an entry -> needs a node and a node-before
            for(auto it = data.begin(), before_it = data.before_begin();
                )
        }

        bucket_data data;
        mutable shared_mutex mutex;
    }
};
/* ------------------------------------------------------------------------- */


// --- implementation
#include "ThreadsafeHashMap.impl.h"


#endif /* THREADSAFE_HASH_MAP_H_ */
