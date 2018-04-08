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
template<typename> class UnorderedMap_iterator;
template<typename> class UnorderedMap_const_iterator;

template<typename Key, typename Value, typename Hash>
bool assert_invariant( const UnorderedMap<Key,Value,Hash>& );

/* UnorderedMapBucket */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value>
class UnorderedMapBucket
{
public:
    using bucket_value = std::pair<Key,Value>;
    using bucket_data = std::forward_list<bucket_value>;
    using iterator = typename bucket_data::iterator;
    using const_iterator = typename bucket_data::const_iterator;
    using size_type = std::size_t;

    size_type size() const noexcept
    {
        return std::count_if(data.cbegin(), data.cend(),
            [](const bucket_value&){return true;});
    }


protected:

private:
    bucket_data data;
};
/* ------------------------------------------------------------------------- */


/* UnorderedMap */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash=std::hash<Key>>
class UnorderedMap
{
    using Allocator = std::allocator<std::pair<Key,Value>>;
    using bucket_type = UnorderedMapBucket<Key,Value>;
    using bucket_container = std::vector<bucket_type>;
    using self = UnorderedMap;
public:
    using allocator_type = Allocator;
    using hasher_type = Hash;
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key,Value>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = UnorderedMap_iterator<value_type>;
    using const_iterator = UnorderedMap_const_iterator<value_type>;

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
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

// --- modifiers
    void clear() noexcept;
    std::pair<iterator,bool> insert( const value_type& value );
    template<typename P, typename = std::enable_if_t<
                            std::is_constructible_v<value_type,P&&>>>
    std::pair<iterator,bool> insert( P&& value );
    std::pair<iterator,bool> insert( value_type&& value );

    iterator erase( const_iterator pos );
    iterator erase( const_iterator first, const_iterator last );
    size_type erase( const key_type& key )
    {
        auto pred = [&key](const value_type& val){return val.first == key;};
        auto bucket = get_bucket(key);
        auto it = std::find_if(bucket.cbegin(), bucket.cend(), pred);
        const auto ret = it != bucket.cend() ? 1 : 0;
        if(ret) bucket.remove_if(pred);
        return ret;
    }

// --- lookup
    iterator find(const Key& key);
    const_iterator find(const Key& key) const;

protected:
    bucket_type& get_bucket( const Key& key ) const
    {
        const auto bucket_index = hasher(key) % buckets.size();
        return buckets[bucket_index];
    }

private:
    bucket_container buckets;
    Hash             hasher{};
};
/* ------------------------------------------------------------------------- */

/* UnorderedMap_iterator */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, Hash>
class UnorderedMap_iterator
{
    friend class UnorderedMap<Key,Value, Hash>;
    using Container = UnorderedMap<Key,Value,Hash>;
    using self = UnorderedMap_iterator;
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using pointer = typename Container::pointer
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename Container::difference_type;
    using const_iterator = UnorderedMap_const_iterator<Key,Value,Hash>;

    
};
/* ------------------------------------------------------------------------- */


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
