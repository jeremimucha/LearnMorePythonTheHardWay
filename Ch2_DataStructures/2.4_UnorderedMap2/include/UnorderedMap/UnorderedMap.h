#ifndef UNORDERED_MAP_GUARD_H_
#define UNORDERED_MAP_GUARD_H_

#include <memory>
#include <utility>
#include <algorithm>
#include <iterator>
#include <stdexcept>
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
template<typename,typename> struct BucketNode;


template<typename Key, typename Value>
struct BucketNode
{
    using value_type  = std::pair<const Key, Value>;
    using key_type    = Key;
    using mapped_type = Value;

    template<typename... Args>
    BucketNode( Args&&... args )
        : data{std::forward<Args>(args)...} { }

    BucketNode* next{nullptr};
    value_type data;
};

template<typename K, typename V, typename H>
bool assert_invariant( const UnorderedMap<K,V,H>& obj )
{
    // TODO implement invariant
}

template<typename Key, typename Value>
class UnorderedMapBucket
{
    template<typename K, typename V, typename H>
    friend bool assert_invariant( const UnorderedMap<K,V,H>& );
    
    using node           = BucketNode<Key,Value>;
    using Allocator      = std::allocator<node>;
    using V_allocator    = std::allocator<typename node::value_type>;
    using alloc_traits   = std::allocator_traits<Allocator>;
    using V_alloc_traits = std::allocator_traits<V_allocator>;
    using self           = UnorderedMapBucket;
public:
    using value_type = typename node::value_type;
    using key_type = typename node::key_type;
    using mapped_type = typename node::mapped_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = typename V_alloc_traits::difference_type;
    using size_type = difference_type;

// --- constructors
    UnorderedMap() noexcept
    {}

private:
    node* head{nullptr};
};


#endif /* UNORDERED_MAP_GUARD_H_ */
