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


struct Invariant_violation_exception : public std::logic_error {
    using std::logic_error::logic_error;
};

template<typename K, typename V, typename H>
bool assert_invariant( const UnorderedMap<K,V,H>& obj )
{
    // invariant - every bucket contains zero, one or more nodes.
    // A bucket may contain only nodes with keys of hash values modulo bucket count
    using size_type = typename UnorderedMap<K,V,H>::size_type;
    auto* buckets = obj.buckets;
    for( size_type i=0; i<obj.count; ++i ){
        for( auto* node = buckets[i].head; node != nullptr; node = node->next ){
            if((obj.hasher(node->data.first) % obj.count) != i)
                throw Invariant_violation_exception("UnorderedMap invariant violated");
        }
    }
    return true;
}


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
    using value_type      = typename node::value_type;
    using key_type        = typename node::key_type;
    using mapped_type     = typename node::mapped_type;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using difference_type = typename V_alloc_traits::difference_type;
    using size_type       = difference_type;

// --- constructors
    UnorderedMapBucket() noexcept = default;
    ~UnorderedMapBucket() noexcept { free(); }

// --- iterators
    struct iterator;
    struct const_iterator
    {
        using value_type      = typename UnorderedMapBucket::value_type;
        using reference       = const value_type&;
        using pointer         = const value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        const_iterator() = default;
        explicit const_iterator(const node* pnode_)
            : pnode{pnode_} { Expects(pnode_); }

        reference operator*() const noexcept
            { return pnode->data; }
        pointer operator->() const noexcept
            { return &(this->operator*()); }
        const_iterator& operator++() noexcept
            { pnode = pnode->next; return *this; }
        const_iterator operator++(int) noexcept
            {
                auto rv = *this;
                ++*this;
                return rv;
            }

        bool operator==(const iterator& other)const { return pnode == other.pnode; }
        bool operator!=(const iterator& other)const { return pnode != other.pnode; }
        bool operator==(const const_iterator& other)const { return pnode == other.pnode; }
        bool operator!=(const const_iterator& other)const { return pnode != other.pnode; }            

        const node* pnode{nullptr};
    };

    struct iterator
    {
        using value_type      = typename UnorderedMapBucket::value_type;
        using reference       = value_type&;
        using pointer         = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        iterator() = default;
        explicit iterator(node* pnode_)
            : pnode{pnode_} { Expects(pnode_); }

        reference operator*() const noexcept
            { return pnode->data; }
        pointer operator->() const noexcept
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

        bool operator==(const iterator& other)const { return pnode == other.pnode; }
        bool operator!=(const iterator& other)const { return pnode != other.pnode; }
        bool operator==(const const_iterator& other)const { return pnode == other.pnode; }
        bool operator!=(const const_iterator& other)const { return pnode != other.pnode; }

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
        return std::count_if(cbegin(), cend(),
                    [](const value_type&){return true;});
    }

// -- modifiers
    std::pair<iterator,bool> insert( const value_type& value )
    {
        const auto it = std::find_if(begin(), end(),
                        [&key=value.first](const value_type& v)
                        {return v.first == key;});
        if(it != end()){
            return {it,false};
        }
        else{
            push_front(value);
            return {begin(),true};
        }
    }

    std::pair<iterator,bool> insert(value_type&& value)
    {
        const auto it = std::find_if(begin(), end(),
                            [&key=value.first](const value_type& v)
                            {return v.first == key;});
        if(it != end()){
            return {it,false};
        }
        else{
            push_front(std::move(value));
            return {begin(),true};
        }
    }

    iterator find( const Key& key )
    {
        return std::find_if(begin(), end(), [&key](const value_type& val)
                                            {return val.first == key;});
    }
    const_iterator find( const Key& key ) const
    {
        return std::find_if(cbegin(), cend(), [&key](const value_type& val)
                                            {return val.first == key;});
    }

    iterator erase(const_iterator iter)
    {
        if(iter == cend()) return end();
    // if iter is not an end() iterator there must be at least one element
        auto pred = begin();
        auto it = begin();
        while( it != end() && it != iter ){
            pred = it++;
        }
        if(pred == it){
            head = it.pnode->next;
            free(it.pnode);
            return iterator{head};
        } else{
            return erase_after(pred);
        }
    }

    size_type erase(const key_type& key)
    {
        if(!head) return 0;
        auto pred = head;
        auto it = head;
        while(it && it->data.first != key){
            pred = it;
            it = it->next;
        }
        if(it && it->data.first == key){
            pred->next = it->next;
            if(it == head)
                head = it->next;
            free(it);
            return 1;
        }
        else
            return 0;
    }

protected:
    template<typename... Args>
    node* make_node( Args&&... args )
    {
        auto* const nn = alloc.allocate(1);
        alloc.construct(nn, std::forward<Args>(args)...);
        return nn;
    }

    void free( node* target ) noexcept
    { Expects(target != nullptr);
        alloc.destroy(target);
        alloc.deallocate(target, 1);
    }
    void free() noexcept
    {
        while(head != nullptr){
            auto* const target = head;
            head = head->next;
            free(target);
        }
    }

    void push_front( const value_type& value )
    {
        auto* const nn = make_node(value);
        nn->next = head;
        head = nn;
    }
    void push_front( value_type&& value )
    {
        auto* const nn = make_node(std::move(value));
        nn->next = head;
        head = nn;
    }

    iterator erase_after( iterator iter )
    {
        auto* const before_target = iter.pnode;
        if(!before_target || !before_target->next)
            return end();
        auto* const target = before_target->next;
        before_target->next = target->next;
        free(target);
        return iterator{before_target};
    }

private:
    Allocator alloc{Allocator{}};
    node* head{nullptr};
};


template<typename Key, typename Value, typename Hash=std::hash<Key>>
class UnorderedMap
{
    template<typename K, typename V, typename H>
    friend bool assert_invariant( const UnorderedMap<K,V,H>& );
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
    using const_iterator = UnorderedMap_const_iterator<Key,Value,Hash>;

    explicit UnorderedMap( size_type bucket_count=19 )
        : count{bucket_count}, buckets{alloc_n(19)}
        { }
    
    ~UnorderedMap() noexcept { free(); }


// --- iterators
    iterator begin() { return iterator{ibegin(), iend()}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator{icbegin(), icend()}; }
    iterator end() { return iterator{iend(), iend(), bnode_iterator{}}; }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator{icend(), icend(), bnode_const_iterator{}}; }

// --- capacity
    size_type size() const noexcept
    {
        return std::count_if(cbegin(), cend(),
                        [](const value_type&){return true;});
    }

    bool empty() const noexcept
    {
        return std::all_of(buckets, buckets+count,
                [](const bucket_type& bucket)
                {return bucket.empty();});
    }

    void clear() noexcept
    {
        for(auto* b = buckets; b != buckets+count; ++b){
            b->clear();
        }
    }

    std::pair<iterator,bool> insert(const value_type& value)
    {
        auto bucket = get_bucket(value.first);
        auto res = bucket->insert(value);
        return {iterator{bucket, iend(), res.first}, res.second};
    }

    std::pair<iterator,bool> insert(value_type&& value)
    {
        auto bucket = get_bucket(value.first);
        auto res = bucket->insert(std::move(value));
        return {iterator{bucket, iend(), res.first}, res.second};
    }

    iterator find(const key_type& key)
    {
        const auto bucket_it = get_bucket(key);
        auto it = bucket_it->find(key);
        if(it.pnode)
            return iterator{bucket_it, iend(), it};
        else
            return end();
    }

    const_iterator find(const key_type& key) const
    {
        const auto bucket_it = get_bucket(key);
        const auto it = bucket_it->find(key);
        if(it.pnode)
            return const_iterator{bucket_it, iend(), it};
        else
            return cend();
    }

    iterator erase( const_iterator pos )
    {
        // get the bucket iterator from hint
        auto bucket_it = const_cast<inner_iterator>(pos.bucket_it);
        const auto bucket_end = const_cast<inner_iterator>(pos.bucket_end);
        auto it = bucket_it->erase(pos.node_it);
        if(it.pnode)
            return iterator{bucket_it, bucket_end, it};
        else
            return iterator{bucket_it, bucket_end, it, true};
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        while(first != last){
            const auto temp = erase(first);
            first = temp;
        }
    }

    size_type erase(const key_type& key)
    {
        auto bucket = get_bucket(key);
        return bucket->erase(key);
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

    UnorderedMap_iterator( inner_iterator begin_, inner_iterator end_ )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node_it{begin_->begin()}
        { first_nonempty(); }

    UnorderedMap_iterator( inner_iterator begin_, inner_iterator end_,
                           bnode_iterator node , bool ensure_nonempty=false )
        : bucket_it{begin_}, bucket_end{end_}, node_it{node}
        { if(ensure_nonempty) first_nonempty(); }

    reference operator*() const noexcept
    { return *node_it; }

    pointer operator->() const noexcept
    { return &(this->operator*()); }

    self& operator++() noexcept
    {
        if(bucket_it != bucket_end){
            if( !node_it.pnode || !(++node_it).pnode ){
                while(++bucket_it != bucket_end && !bucket_it->begin().pnode)
                    ;
                if(bucket_it != bucket_end)
                    node_it = bucket_it->begin();
            }
        }
        return *this;
    }

    self operator++(int) noexcept
    {
        const auto rv = *this;
        ++*this;
        return rv;
    }

    operator const_iterator() { return const_iterator{bucket_it, bucket_end, node_it}; }

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

protected:
    void first_nonempty() noexcept
    {
        if(node_it.pnode) return;
        while(++bucket_it != bucket_end && !bucket_it->begin().pnode)
            ;
        if(bucket_it != bucket_end) node_it = bucket_it->begin();
    }

private:
    inner_iterator bucket_it;
    inner_iterator bucket_end;
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

    UnorderedMap_const_iterator( inner_iterator begin_, inner_iterator end_ )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node_it{begin_->cbegin()}
        { first_nonempty(); }

    UnorderedMap_const_iterator( inner_iterator begin_, inner_iterator end_,
                                    bnode_iterator node, bool ensure_nonempty=false)
        : bucket_it{begin_}, bucket_end{end_}, node_it{node}
        { if(ensure_nonempty) first_nonempty(); }

    reference operator*() const noexcept
    { return *node_it; }

    pointer operator->() const noexcept
    { return &(this->operator*()); }

    self& operator++() noexcept
    { 
        if(bucket_it != bucket_end){
            if( !node_it.pnode || !(++node_it).pnode ){
                while(++bucket_it != bucket_end && !bucket_it->begin().pnode)
                    ;
                if(bucket_it != bucket_end)
                    node_it = bucket_it->cbegin();
            }
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

protected:
    void first_nonempty() noexcept
    {
        if(node_it.pnode) return;
        while(++bucket_it != bucket_end && !bucket_it->cbegin().pnode)
            ;
        if(bucket_it != bucket_end) node_it = bucket_it->cbegin();
    }

private:
    inner_iterator bucket_it;
    inner_iterator bucket_end;
    bnode_iterator node_it;
};


template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it == rhs.bucket_it && lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it != rhs.bucket_it || lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it == rhs.bucket_it && lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it != rhs.bucket_it || lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it == rhs.bucket_it && lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_const_iterator<K,V,H>& lhs,
                       const UnorderedMap_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it != rhs.bucket_it || lhs.node_it != rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator==(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it == rhs.bucket_it && lhs.node_it == rhs.node_it;
}
template<typename K, typename V, typename H>
inline bool operator!=(const UnorderedMap_iterator<K,V,H>& lhs,
                       const UnorderedMap_const_iterator<K,V,H>& rhs) noexcept
{
    return lhs.bucket_it != rhs.bucket_it || lhs.node_it != rhs.node_it;
}

#endif /* UNORDERED_MAP_GUARD_H_ */
