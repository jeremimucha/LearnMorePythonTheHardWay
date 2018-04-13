#ifndef UNORDERED_MAP_GUARD_H
#define UNORDERED_MAP_GUARD_H


#include <memory>
#include <utility>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <forward_list>
#include <type_traits>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif


template<typename,typename,typename> class UnorderedMap;
template<typename,typename> class UnorderedMapBucket;
// template<typename T>
// using UnorderedMapBucket = SingleLinkedList<T>;
template<typename,typename,typename> class UnorderedMap_iterator;
template<typename,typename,typename> class UnorderedMap_const_iterator;

template<typename Key, typename Value, typename Hash>
bool assert_invariant( const UnorderedMap<Key,Value,Hash>& );

/* UnorderedMapBucket */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value>
class UnorderedMapBucket
{
public:
    using bucket_value    = std::pair<const Key,Value>;
    using bucket_data     = std::forward_list<bucket_value>;
    using value_type      = typename bucket_data::value_type;
    using reference       = typename bucket_data::reference;
    using const_reference = typename bucket_data::const_reference;
    using pointer         = typename bucket_data::pointer;
    using const_pointer   = typename bucket_data::const_pointer;
    using iterator        = typename bucket_data::iterator;
    using const_iterator  = typename bucket_data::const_iterator;
    using size_type       = std::size_t;


// --- iterators
    iterator begin() { return data.begin(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator cbegin() const { return data.cbegin(); }
    iterator end() { return data.end(); }
    const_iterator end() const { return data.end(); }
    const_iterator cend() const { return data.cend(); }

    bool empty() const noexcept
    { return data.empty(); }

    size_type size() const noexcept
    {
        return std::count_if(data.cbegin(), data.cend(),
            [](const bucket_value&){return true;});
    }

    std::pair<iterator,bool> insert( const value_type& value )
    {
        const auto it = std::find_if(data.begin(), data.end(),
                                    [&key = value.first](const value_type& v)
                                    {v.first == key;});
        if(it != data.end()){
            return {it,false};
        }
        else{
            data.push_front(value);
            return {data.begin(), true};
        }
    }
    std::pair<iterator,bool> insert( value_type&& value )
    {
        const auto it = std::find_if(data.begin(), data.end(),
                                    [&key = value.first](const value_type& v)
                                    {return v.first == key;});
        if(it != data.end())
            return {it,false};
        else{
            data.push_front(std::move(value));
            return {data.begin(), true};
        }
    }

    iterator find( const Key& key )
    {
        return std::find_if(data.begin(), data.end(),
                            [&key](const value_type& val)
                            {return val.first == key;});
    }
    const_iterator find( const Key& key ) const
    {
        return std::find_if(data.cbegin(), data.cend(),
                            [&key](const value_type& val)
                            {return val.first == &key;});
    }

    iterator erase(const_iterator iter)
    {
    // potentially expensive, but buckets should be small
        auto pred = data.before_begin();
        for(auto it = data.begin(); it != iter && it != data.end();
            ++it, ++pred)
            ;
        data.erase_after(pred);
        return ++pred;
    }
    // template<typename P, typename = std::enable_if_t<
    //                         std::is_constructible_v<value_type,P&&>>>
    // std::pair<iterator,bool> insert( P&& value );

private:
    bucket_data data;
};
/* ------------------------------------------------------------------------- */


/* UnorderedMap */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash=std::hash<Key>>
class UnorderedMap
{
    friend class UnorderedMap_iterator<Key,Value,Hash>;
    friend class UnorderedMap_const_iterator<Key,Value,Hash>;
    using Allocator             = std::allocator<std::pair<Key,Value>>;
    using bucket_type           = UnorderedMapBucket<Key,Value>;
    using bucket_iter           = typename bucket_type::iterator;
    using bucket_container      = std::vector<bucket_type>;
    using bucket_container_iter = typename bucket_container::iterator;
    using bucket_container_const_iter = typename bucket_container::const_iterator;
    using self                  = UnorderedMap;
public:
    using allocator_type   = Allocator;
    using hasher_type      = Hash;
    using key_type         = Key;
    using mapped_type      = Value;
    using value_type       = std::pair<const Key,Value>;
    using reference        = value_type&;
    using const_reference  = const value_type&;
    using pointer          = value_type*;
    using const_pointer    = const value_type*;
    using size_type        = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using iterator         = UnorderedMap_iterator<Key,Value,Hash>;
    using const_iterator   = UnorderedMap_const_iterator<Key,Value,Hash>;

    UnorderedMap( size_type bucket_count=19 )
        : buckets{bucket_count}
        { }
    ~UnorderedMap() noexcept = default;

// --- capacity
    bool size() const noexcept
    {
        return std::accumulate(buckets.cbegin(), buckets.cend(),
                static_cast<size_type>(0),
                [](size_type val,const bucket_type& bucket)
                {return val + bucket.size();});
    }
    bool empty() const noexcept
    {
        return std::all_of(buckets.cbegin(), buckets.cend(),
            [](const bucket_type& bucket){return bucket.empty();});
    }

// --- iterators
    iterator begin() { return iterator{buckets.begin(), buckets.end()}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const
        { return const_iterator{buckets.cbegin(), buckets.cend()}; }
    iterator end() { return iterator{buckets.end(), buckets.end()}; }
    const_iterator end() const { return cend(); }
    const_iterator cend() const
        { return const_iterator{buckets.cend(), buckets.cend()}; }

// --- modifiers
    void clear() noexcept
    {
        for(auto& b : buckets)
            b.clear();
    }
    
    std::pair<iterator,bool> insert( const value_type& value )
    {
        auto bucket_it = get_bucket(value.first);
        const auto res = bucket_it->insert(value);
        return {iterator{bucket_it, buckets.end(), res.first}, res.second};
    }
    std::pair<iterator,bool> insert( value_type&& value )
    {
        auto bucket_it = get_bucket(value.first);
        const auto res = bucket_it->insert(std::move(value));
        return {iterator{bucket_it, buckets.end(), res.first}, res.second};
    }
    // template<typename P, typename = std::enable_if_t<
    //                     std::is_constructible_v<value_type,P&&>>>
    // std::pair<iterator,bool> insert( P&& value );

    iterator erase( const_iterator pos )
    {
        // get the bucket iterator from hint
        const auto bucket_it = pos.outer;
        const auto it = bucket_it->erase(pos.inner);
        return iterator{pos.outer, pos.outer_end, it};
    }

    iterator erase( const_iterator first, const_iterator last )
    {
        while(first != last){
            const auto temp = erase(first);
            first = temp;
        }
    }
    
    size_type erase( const key_type& key )
    {
        auto pred = [&key](const value_type& val){return val.first == key;};
        auto bucket = get_bucket(key);
        auto it = std::find_if(bucket->cbegin(), bucket->cend(), pred);
        const auto ret = it != bucket->cend() ? 1 : 0;
        if(ret)
            bucket->remove_if(pred);
        return ret;
    }

    mapped_type& operator[](const key_type& key)
    {
        auto bucket_it = get_bucket(key);
        auto it = bucket_it->insert({key,mapped_type{}});
        return it.first->second;
    }

// --- lookup
    iterator find(const key_type& key)
    {
        const auto bucket_it = get_bucket(key);
        const auto it = bucket_it->find(key);
        return iterator{bucket_it, buckets.end(), it};
    }
    const_iterator find(const key_type& key) const
    {
        const auto bucket_it = get_bucket(key);
        const auto it = bucket_it->find(key);
        return const_iterator{bucket_it, buckets.end(), it};
    }

protected:
    bucket_container_const_iter get_bucket( const key_type& key ) const
    {
        const auto bucket_index = hasher(key) % buckets.size();
        auto it = buckets.cbegin();
        std::advance(it, bucket_index);
        return it;
    }

    bucket_container_iter get_bucket( const key_type& key )
    {
        const auto bucket_index = hasher(key) % buckets.size();
        auto it = buckets.begin();
        std::advance(it, bucket_index);
        return it;
    }
private:
    bucket_container buckets;
    Hash             hasher{};
};
/* ------------------------------------------------------------------------- */

/* UnorderedMap_iterator */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash>
class UnorderedMap_iterator
{
    friend class UnorderedMap<Key,Value, Hash>;
    friend class UnorderedMapBucket<Key, Value>;
    using map_type = UnorderedMap<Key,Value,Hash>;
    using bucket_type = typename map_type::bucket_type;
    using self = UnorderedMap_iterator;
    using bucket_container_iterator = typename map_type::bucket_container::iterator;
    using bucket_iterator = typename bucket_type::iterator;
public:
    using value_type = typename bucket_type::value_type;
    using reference = typename bucket_type::reference;
    using pointer = typename bucket_type::pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename map_type::difference_type;
    using const_iterator = UnorderedMap_const_iterator<Key,Value,Hash>;

    explicit UnorderedMap_iterator( bucket_container_iterator outer_
                                  , bucket_container_iterator outer_end_ )
        : outer{outer_}, outer_end{outer_end_}, inner{outer_->begin()}
        { }

    UnorderedMap_iterator( bucket_container_iterator outer_
                         , bucket_container_iterator outer_end_
                         , bucket_iterator inner_ )
        : outer{outer_}, outer_end{outer_end_}, inner{inner_}
        {
            if( inner == outer->end() ) ++*this;
        }

    reference operator*() const noexcept
    { return *inner; }

    pointer operator->() const noexcept
    { return &(this->operator*()); }

    self& operator++() noexcept
    {
        if( outer != outer_end ){
            if(inner == outer->end() || ++inner == outer->end())
            {
                ++outer;
                if(outer != outer_end) inner = outer->begin();
            }
        }
        return *this;
    }

    self operator++(int) noexcept
    {
        auto rv = *this;
        ++*this;
        return rv;
    }

    operator const_iterator() { return const_iterator{outer, outer_end, inner}; }

template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;

private:
    bucket_container_iterator outer;
    bucket_container_iterator outer_end;
    bucket_iterator           inner;
};
/* ------------------------------------------------------------------------- */

/* UnorderedMap_const_iterator */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash>
class UnorderedMap_const_iterator
{
    friend class UnorderedMap<Key,Value, Hash>;
    friend class UnorderedMapBucket<Key, Value>;
    using map_type        = UnorderedMap<Key,Value,Hash>;
    using bucket_type     = typename map_type::bucket_type;
    using self            = UnorderedMap_const_iterator;
    using bucket_container_iterator
                          = typename map_type::bucket_container::const_iterator;
    using bucket_iterator = typename bucket_type::const_iterator;
public:
    using value_type        = typename bucket_type::value_type;
    using reference         = typename bucket_type::const_reference;
    using pointer           = typename bucket_type::const_pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = typename map_type::difference_type;
    using const_iterator    = UnorderedMap_const_iterator<Key,Value,Hash>;

    explicit UnorderedMap_const_iterator( bucket_container_iterator outer_
                                        , bucket_container_iterator outer_end_ )
        : outer{outer_}, outer_end{outer_end_}, inner{outer_->cbegin()}
        { }

    UnorderedMap_const_iterator( bucket_container_iterator outer_
                         , bucket_container_iterator outer_end_
                         , bucket_iterator inner_ )
        : outer{outer_}, outer_end{outer_end_}, inner{inner_}
        {
            if(inner == outer->cend()) ++*this;
        }

    reference operator*() const noexcept
    { return *inner; }

    pointer operator->() const noexcept
    { return inner; }

    self& operator++() noexcept
    {
        if( outer != outer_end ){
            if(inner == outer->cend() || ++inner == outer->end())
            {
                ++outer;
                if(outer != outer_end) inner = outer->cbegin();
            }
        }
        return *this;
    }

    self operator++(int) noexcept
    {
        auto rv = *this;
        ++*this;
        return rv;
    }

template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;
template<typename K, typename V, typename H>
friend bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept;

private:
    bucket_container_iterator outer;
    bucket_container_iterator outer_end;
    bucket_iterator           inner;
};
/* ------------------------------------------------------------------------- */

template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner == rhs.inner && lhs.outer == rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner != rhs.inner && lhs.outer != rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner == rhs.inner && lhs.outer == rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner != rhs.inner && lhs.outer != rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner == rhs.inner && lhs.outer == rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner != rhs.inner && lhs.outer != rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner == rhs.inner && lhs.outer == rhs.outer;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.inner != rhs.inner && lhs.outer != rhs.outer;
}

// /* UnorderedMapBucketNode */
// /* ------------------------------------------------------------------------- */
// template<T>
// struct UnorderedMapBucketNode
// {
//     template<typename... Args>
//     explicit UnorderedMapBucketNode( Args&&... args )
//         : data{std::forward<Args>(args)...} { }

//     UnorderedMapBucketNode* next{nullptr};
//     T                       data;
// };
// /* ------------------------------------------------------------------------- */

// /* UnorderedMapBucket */
// /* ------------------------------------------------------------------------- */
// template<typename T>
// class UnorderedMapBucket
// {
//     using node = UnorderedMapBucketNode<T>;
//     using Allocator = std::allocator<node>;
//     using T_allocator = std::allocator<T>;
//     using alloc_traits = std::allocator_traits<Allocator>;
//     using T_alloc_traits = std::allocator_traits<T_allocator>;
//     using self = UnorderedMapBucket;

// public:
//     using allocator_type            = Allocator;
//     using value_type                = typename T_alloc_traits::value_type;
//     using reference                 = value_type&;
//     using const_reference           = const value_type&;
//     using pointer                   = typename T_alloc_traits::pointer;
//     using const_pointer             = typename T_alloc_traits::const_pointer;
//     using difference_type           = typename T_alloc_traits::difference_type;
//     using size_type                 = typename T_alloc_traits::size_type;
//     using iterator                  = UnorderedMapBucket_iterator<T>;
//     using const_iterator            = UnorderedMapBucket_const_iterator<T>;
    
//     UnorderedMapBucket() noexcept = default;
//     ~UnorderedMapBucket() noexcept { free(); }

//     bool empty() const noexcept { return head.next == nullptr; }
//     void clear() noexcept { free(); }

// protected:
//     template<typename... Args>
//     inline node* make_node(Args&&... args)
//     {
//         auto* const nn = alloc.allocate(1);
//         alloc.construct(nn, std::forward<Args>(args)...);
//         return nn;
//     }

//     void free(node* target) noexcept
//     { Expects(target != nullptr);
//         alloc.destroy(target);
//         alloc.deallocate(target, 1);
//     }

//     void free() noexcept
//     {
//         while( head.next != nullptr ){
//             auto* const target = head.next;
//             head.next = target->next;
//             free(target);
//         }
//     }
// private:
//     Allocator alloc{Allocator()};
//     node head{};
// };
// /* ------------------------------------------------------------------------- */
#endif  /* UNORDERED_MAP_GUARD_H */
