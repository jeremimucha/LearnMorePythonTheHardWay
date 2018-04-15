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
template<typename,typename,typename> class UnorderedMap_iterator;
template<typename,typename,typename> class UnorderedMap_const_iterator;

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
    return true;
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
    UnorderedMapBucket() noexcept
    {}

// --- iterators
    class const_iterator
    {
        using value_type = typename UnorderedMapBucket::value_type;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = std::ptrdiff_t;

        explicit const_iterator(const node* pnode_)
            : pnode{pnode_} { Expects(pnode_); }

        reference operator*() noexcept
            { return pnode->data; }
        pointer operator->() noexcept
            { return &(this->operator*()); }
        const_iterator& operator++() noexcept
            { pnode = pnode->next; return *this; }
        const_iterator operator++(int) noexcept
            {
                auto rv = *this;
                ++*this;
                return rv;
            }

        const node* pnode{nullptr};
    };

    struct iterator
    {
        using value_type = typename UnorderedMapBucket::value_type;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = std::ptrdiff_t;

        explicit iterator(node* pnode_)
            : pnode{pnode_} { Expects(pnode_); }

        reference operator*() noexcept
            { return pnode->data; }
        pointer operator->() noexcept
            { return &(this->operator*()); }
        iterator& operator++() noexcept
            { pnode = pnode->next; return *this; }
        iterator operator++(int) noexcept
            {
                auto rv = *this;
                ++*this;
                return rv;
            }

    operator const_iterator() { return const_iterator{pnode}; }

        node* pnode{nullptr};
    };

    iterator begin() { return iterator{head}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator{head}; }
    iterator end() { return iterator{}; }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator{}; }

// --- capacity
    bool empty() const noexcept { return head == nullptr; }
    size_type size() const noexcept
    {
        auto count = size_type{0};
        for(auto it = head; it != nullptr; it = it->next)
            ++count;
        return count;
    }

private:
    Allocator alloc{Allocator{}};
    node* head{nullptr};
};


template<typename K, typename V>
bool operator==(const typename UnorderedMapBucket<K,V>::iterator& lhs,
                const typename UnorderedMapBucket<K,V>::iterator& rhs)
{ return lhs.pnode == rhs.pnode; }
template<typename K, typename V>
bool operator!=(const typename UnorderedMapBucket<K,V>::iterator& lhs,
                const typename UnorderedMapBucket<K,V>::iterator& rhs)
{ return lhs.pnode != rhs.pnode; }
template<typename K, typename V>
bool operator==(const typename UnorderedMapBucket<K,V>::const_iterator& lhs,
                const typename UnorderedMapBucket<K,V>::iterator& rhs)
{ return lhs.pnode == rhs.pnode; }
template<typename K, typename V>
bool operator!=(const typename UnorderedMapBucket<K,V>::const_iterator& lhs,
                const typename UnorderedMapBucket<K,V>::iterator& rhs)
{ return lhs.pnode != rhs.pnode; }
template<typename K, typename V>
bool operator==(const typename UnorderedMapBucket<K,V>::iterator& lhs,
                const typename UnorderedMapBucket<K,V>::const_iterator& rhs)
{ return lhs.pnode == rhs.pnode; }
template<typename K, typename V>
bool operator!=(const typename UnorderedMapBucket<K,V>::iterator& lhs,
                const typename UnorderedMapBucket<K,V>::const_iterator& rhs)
{ return lhs.pnode != rhs.pnode; }
template<typename K, typename V>
bool operator==(const typename UnorderedMapBucket<K,V>::const_iterator& lhs,
                const typename UnorderedMapBucket<K,V>::const_iterator& rhs)
{ return lhs.pnode == rhs.pnode; }
template<typename K, typename V>
bool operator!=(const typename UnorderedMapBucket<K,V>::const_iterator& lhs,
                const typename UnorderedMapBucket<K,V>::const_iterator& rhs)
{ return lhs.pnode != rhs.pnode; }


template<typename Key, typename Value, typename Hash=std::hash<Key>>
class UnorderedMap
{
    friend class UnorderedMap_iterator<Key,Value,Hash>;
    friend class UnorderedMap_const_iterator<Key,Value,Hash>;
    using bucket_type = UnorderedMapBucket<Key,Value>;
    using bnode_iterator = typename bucket_type::iterator;
    using bnode_const_iterator = typename bucket_type::const_iterator;
    using Allocator = std::allocator<bucket_type>;
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
    using inner_iterator = bucket_type*;
    using inner_const_iterator = const bucket_type*;
    using iterator = UnorderedMap_iterator<Key,Value,Hash>;
    using const_iterator = UnorderedMap_iterator<Key,Value,Hash>;

    explicit UnorderedMap( size_type bucket_count=19 )
        : count{bucket_count}, buckets{alloc_n(19)}
        { }
    
    ~UnorderedMap() noexcept { free(); }

// --- capacity
    size_type size() const noexcept
    {
        return std::accumulate(buckets.cbegin(), buckets.cend(),
                    static_cast<size_type>(0),
                    [](size_type val, const bucket_type& bucket)
                    {return val + bucket.size();});
    }

    bool empty() const noexcept
    {
        return std::all_of(buckets, buckets+count,
                [](const bucket_type& bucket)
                {return bucket.empty();});
    }

protected:
    inner_iterator ibegin() { return buckets; }
    inner_const_iterator icbegin() const { return buckets; }
    inner_const_iterator ibegin() const { return icbegin(); }
    inner_iterator iend() { return buckets + count; }
    inner_const_iterator icend() const { return buckets + count; }
    inner_const_iterator iend() const { return icend(); }

    bucket_type* alloc_n(size_type n, const bucket_type& val = bucket_type{})
    {
        auto* const pb = alloc.allocate(n);
        std::uninitialized_fill_n(pb, n, val);
        return pb;
    }

    inner_const_iterator get_bucket(const key_type& key) const
    {
        const auto bucket_index = hasher(key) % count;
        auto it = icbegin();
        std::advance(it,bucket_index);
        return it;
    }

    inner_iterator get_bucket(const key_type& key)
    {
        const auto bucket_index = hasher(key) % count;
        auto it = ibegin();
        std::advance(it, bucket_index);
        return it;
    }

    void free() noexcept
    {
        if(buckets){
            for(auto* p = buckets+count; p != buckets;){
                alloc.destroy(--p);
            }
            alloc.deallocate(buckets, count);
        }
    }

private:
    Allocator    alloc{Allocator{}};
    Hash         hasher{};
    size_type    count;
    bucket_type* buckets;
};

template<typename Key, typename Value, typename Hash>
class UnorderedMap_iterator
{
    friend class UnorderedMap<Key,Value,Hash>;
    friend class UnorderedMapBucket<Key,Value>;
    using map_type = UnorderedMap<Key,Value,Hash>;
    using bucket_type = typename map_type::bucket_type;
    using self = UnorderedMap_iterator;
    using inner_iterator = typename map_type::inner_iterator;
    using bnode_iterator = typename map_type::bnode_iterator;
public:
    using value_type = typename bucket_type::value_type;
    using reference = typename bucket_type::reference;
    using pointer = typename bucket_type::pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename map_type::difference_type;
    using const_iterator = UnorderedMap_const_iterator<Key,Value,Hash>;

    UnorderedMap_iterator( inner_iterator begin_ )
        : bucket_it{begin_}, node_it{bucket_it->begin()}
        { }

    UnorderedMap_iterator( inner_iterator begin_, bnode_iterator node )
        : bucket_it{begin_}, node_it{node}
        { }

    reference operator*() const noexcept
    { return *node_it; }

    pointer operator->() const noexcept
    { return &(this->operator*()); }

    self& operator++() noexcept
    { 
        if( ++node_it == nullptr ){
            ++bucket_it;
            node_it = bucket_it->begin();
        }
        return *this;
    }

    self operator++(int) noexcept
    {
        const auto rv = *this;
        ++*this;
        return rv;
    }

    operator const_iterator() { return const_iterator{bucket_it, node_it}; }

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
    inner_iterator bucket_it;
    bnode_iterator node_it;
};

template<typename Key, typename Value, typename Hash>
class UnorderedMap_const_iterator
{
    friend class UnorderedMap<Key,Value,Hash>;
    friend class UnorderedMapBucket<Key,Value>;
    using map_type = UnorderedMap<Key,Value,Hash>;
    using bucket_type = typename map_type::bucket_type;
    using self = UnorderedMap_const_iterator;
    using inner_iterator = typename map_type::inner_const_iterator;
    using bnode_iterator = typename map_type::bnode_const_iterator;
public:
    using value_type = typename bucket_type::value_type;
    using reference = typename bucket_type::const_reference;
    using pointer = typename bucket_type::const_pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename map_type::difference_type;

    UnorderedMap_const_iterator( inner_iterator begin_ )
        : bucket_it{begin_}, node_it{bucket_it->begin()}
        { }

    UnorderedMap_const_iterator( inner_iterator begin_, bnode_iterator node )
        : bucket_it{begin_}, node_it{node}
        { Expects(node != nullptr); }

    reference operator*() const noexcept
    { return *node_it; }

    pointer operator->() const noexcept
    { return &(this->operator*()); }

    self& operator++() noexcept
    { 
        if( ++node_it == nullptr ){
            ++bucket_it;
            node_it = bucket_it->begin();
        }
        return *this;
    }

    self operator++(int) noexcept
    {
        const auto rv = *this;
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
    inner_iterator bucket_it;
    bnode_iterator node_it;
};


template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it == rhs.bucket_it && */ lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it != rhs.bucket_it || */ lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it == rhs.bucket_it && */ lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it != rhs.bucket_it || */ lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it == rhs.bucket_it && */ lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it != rhs.bucket_it || */ lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it == rhs.bucket_it && */ lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return /* lhs.bucket_it != rhs.bucket_it || */ lhs.node_it != rhs.node_it;
}

#endif /* UNORDERED_MAP_GUARD_H_ */
