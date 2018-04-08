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
    public:
        using bucket_value          = std::pair<Key,Value>;
        using bucket_data           = std::forward_list<bucket_value>;
        using bucket_iterator       = typename bucket_data::iterator;
        using bucket_const_iterator = typename bucket_data::const_iterator;
        using shared_mutex_type     = std::shared_mutex;
        Value value_for(const Key& key, const Value& default_value) const
        {
            std::shared_lock<shared_mutex_type> lock{mutex};
            const bucket_const_iterator found_entry = find_entry_for(key);
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
            data.remove_if([&key](const bucket_value& kvpair)
                           {return kvpair.first == key;});
            // const auto entry_pair = find_entry_before(key);
            // if(entry_pair.second != data.end()){
            //     data.erase_after(entry_pair.first);
            // }
        }

    private:
        bucket_const_iterator find_entry_for( const Key& key ) const noexcept
        {
            return std::find_if(data.begin(), data.end(),
                                [this,&key](const bucket_value& item)
                                {return item.first == key;});
        }

        bucket_iterator find_entry_for( const Key& key ) noexcept
        {
            return std::find_if(data.begin(), data.end(),
                                [this,&key](const bucket_value& item)
                                {return item.first == key;});
        }

        std::pair<bucket_iterator,bucket_iterator> find_entry_before( const Key& key ) const noexcept
        {
            // TODO implement finding entry in a forward list that allows for
            // deleting an entry -> needs a node and a node-before
            auto it = data.begin();
            auto before_it = data.before_begin();
            while( it != data.end() && it->first != key ){
                ++it;
                ++before_it;
            }
            return {before_it, it};
        }

        bucket_data data;
        mutable shared_mutex_type mutex;
    }; // HashBucket

    using bucket_type = HashBucket;

public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = typename bucket_type::bucket_value;
    using shared_mutex_type = typename bucket_type::shared_mutex_type;

    ThreadsafeHashMap( unsigned num_buckets=19,
                       const Hash& hasher_=Hash() )
        : buckets(num_buckets), hasher(hasher_)
        {
            for(unsigned i=0; i<num_buckets; ++i){
                buckets[i].reset(new bucket_type);
            }
        }

    ThreadsafeHashMap(const ThreadsafeHashMap&) = delete;
    ThreadsafeHashMap(ThreadsafeHashMap&&) noexcept = delete;
    ThreadsafeHashMap& operator=(const ThreadsafeHashMap&) = delete;
    ThreadsafeHashMap& operator=(ThreadsafeHashMap&&) noexcept = delete;

    Value value_for( const Key& key, const Value& default_value )
    {
        return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(const Key& key, const Value& value)
    {
        get_bucket(key).add_or_update_mapping(key,value);
    }

    void remove_mapping( const Key& key )
    {
        get_bucket(key).remove_mapping(key);
    }

    std::map<Key, Value> get_map() const
    {
        std::vector<std::unique_lock<shared_mutex_type>> locks;
        for(unsigned i=0; i<buckets.size(); ++i){
            locks.push_back(
                std::unique_lock<shared_mutex_type>(buckets[i].mutex));
        }
        std::map<Key,Value> res;
        for(unsigned i=0; i<buckets.size(); ++i){
            for( const auto& val : buckets[i].data ){
                res.insert(val);
            }
        }
        return res;
    }

protected:
    bucket_type& get_bucket( const Key& key ) const
    {
        const auto bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

private:
    std::vector<std::unique_ptr<bucket_type>> buckets;
    Hash hasher;
};
/* ------------------------------------------------------------------------- */


// --- implementation
#include "ThreadsafeHashMap.impl.h"


#endif /* THREADSAFE_HASH_MAP_H_ */
