#ifndef UNORDERED_MAP_STABLE_ABI_H_
#define UNORDERED_MAP_STABLE_ABI_H_

#include <memory>
#include <utility>
#include <stdexcept>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif


template<typename,typename,typename,typename> class UnorderedMap;
template<typename,typename,typename> class UnorderedMapBucket;
template<typename,typename> struct BucketNode;
template<typename,typename,typename,typename> class UnorderedMap_iterator;
template<typename,typename,typename,typename> class UnorderedMap_const_iterator;

/* Invariant */
/* ------------------------------------------------------------------------- */
struct Invariant_violation_exception : public std::logic_error {
    using std::logic_error::logic_error;
};

template<typename K, typename V, typename H, typename A>
bool assert_invariant( const UnorderedMap<K,V,H,A>& obj )
{
    // invariant - every bucket contains zero, one or more nodes.
    // A bucket may contain only nodes with keys of hash values modulo bucket count
    using size_type = typename UnorderedMap<K,V,H,A>::size_type;
    using bucket_node_type = typename UnorderedMap<K,V,H,A>::bucket_node_type;
    auto* buckets = obj.buckets;
    for( size_type i=0; i<obj.count; ++i ){
        for( auto* node = static_cast<bucket_node_type*>(buckets[i].head.next);
             node != nullptr;
             node = static_cast<bucket_node_type*>(node->next) ){
            if((obj.hasher(node->data.first) % obj.count) != i)
                throw Invariant_violation_exception("UnorderedMap invariant violated");
        }
    }
    return true;
}
/* ------------------------------------------------------------------------- */

/* UnorderedMapBucket */
/* ------------------------------------------------------------------------- */
struct BucketNodeBase{
    BucketNodeBase* next{nullptr};
};

template<typename Key, typename Value>
struct BucketNode : public BucketNodeBase{
    using value_type = std::pair<const Key, Value>;
    using key_type = Key;
    using mapped_type = Value;

    template<typename... Args>
    explicit BucketNode( Args&&... args )
        : BucketNodeBase{}, data{std::forward<Args>(args)...}
        { }

    value_type data;
};

class UnorderedMapBucketBase{

    template<typename,typename,typename,typename>
    friend class UnorderedMap_iterator;
    template<typename,typename,typename,typename>
    friend class UnorderedMap_const_iterator;
    template<typename K,typename V,typename H,typename A>
    friend bool assert_invariant(const UnorderedMap<K,V,H,A>& );
protected:
    using node_base = BucketNodeBase;
    using size_type = std::ptrdiff_t;

    bool empty() const noexcept { return head.next == nullptr; }

    size_type size() const noexcept
    {
        auto count = size_type{0};
        for(const auto* p = head.next; p != nullptr; ++count, p=p->next)
            ;
        return count;
    }

    void push_front( node_base* n ) noexcept
    {
        n->next = head.next;
        head.next = n;
    }

    node_base* unlink_front() noexcept
    {
        auto* const target = head.next;
        head.next = target->next;
        return target;
    }

    node_base* unlink_after( node_base* node ) noexcept
    {
        auto* const target = node->next;
        node->next = target->next;
        return target;
    }

    node_base head{nullptr};
};

template< typename Key, typename Value
        , typename Alloc=std::allocator<std::pair<const Key,Value>> >
class UnorderedMapBucket : public UnorderedMapBucketBase{
    template<typename,typename,typename,typename>
    friend class UnorderedMap_iterator;
    template<typename,typename,typename,typename>
    friend class UnorderedMap_const_iterator;
    template<typename K,typename V,typename H,typename A>
    friend bool assert_invariant(const UnorderedMap<K,V,H,A>& );
    using node           = BucketNode<Key,Value>;
    using V_Allocator    = typename std::allocator_traits<Alloc>::template
                             rebind_alloc<std::pair<const Key,Value>>;
    using V_alloc_traits = std::allocator_traits<V_Allocator>;
    using Allocator      = typename std::allocator_traits<Alloc>::template
                             rebind_alloc<node>;
    using alloc_traits   = std::allocator_traits<Allocator>;
    using self           = UnorderedMapBucket;
    using base           = UnorderedMapBucketBase;
public:
    using value_type      = typename std::pair<const Key, Value>;
    using key_type        = const Key;
    using mapped_type     = Value;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using difference_type = typename alloc_traits::difference_type;
    using size_type       = difference_type;
    using node_type       = node;

// --- constructors
    UnorderedMapBucket() noexcept = default;
    ~UnorderedMapBucket() noexcept { free(); }


// --- capacity
    bool empty() const noexcept { return base::empty(); }
    size_type size() const noexcept { return base::size(); }

// --- lookup
    const node* find(const key_type& key) const noexcept
    {
        const auto* p = static_cast<const node>(head.next);
        while(p && p->data.first != key){
            p = static_cast<const node>(p->next);
        }
        return p;
    }

    node* find(const key_type& key) noexcept
    {
        auto* p = static_cast<node*>(head.next);
        while(p && p->data.first != key){
            p = static_cast<node*>(p->next);
        }
        return p;
    }

// --- modifiers
    std::pair<node*,bool> insert( const value_type& value )
    {
        auto* pnode = find(value.first);
        if(pnode != nullptr){ // value already present
            return {pnode,false};
        } else {
            push_front(value);
            return {static_cast<node*>(head.next),true};
        }
    }

    std::pair<node*,bool> insert( value_type&& value )
    {
        auto* pnode = find(value.first);
        if(pnode != nullptr){ // value already present
            return {pnode,false};
        } else {
            push_front(std::move(value));
            return {static_cast<node*>(head.next),true};
        }
    }

    size_type erase(const key_type& key)
    {
        auto* pred = &head;
        auto* succ = static_cast<node*>(head.next);
        while(succ != nullptr && succ->data.first != key){
            pred = succ;
            succ = static_cast<node*>(succ->next);
        }
        if(succ){
            succ = static_cast<node*>(base::unlink_after(pred));
            free(succ);
            return 1;
        } else
            return 0;
    }

    node* erase(const node* it)
    {
        auto* pred = &head;
        auto* succ = head.next;
        while(succ && succ != it){
            pred = succ;
            succ = succ->next;
        }
        if(succ){
            succ = base::unlink_after(pred);
            free(static_cast<node*>(succ));
            return static_cast<node*>(pred);
        }
            return static_cast<node*>(succ);
    }

protected:
    template<typename... Args>
    node* make_node( Args&&... args )
    {
        auto* const nn = alloc.allocate(1);
        alloc.construct(nn, std::forward<Args>(args)...);
        return nn;
    }

    void push_front( const value_type& value )
    {
        auto* const nn = make_node(value);
        base::push_front(nn);
    }
    void push_front( value_type&& value )
    {
        auto* const nn = make_node(std::move(value));
        base::push_front(nn);
    }

    void free(node* target) noexcept
    { Expects(target);
        alloc.destroy(target);
        alloc.deallocate(target, 1);
    }

    void free(node* first, node* last) noexcept
    {
        while(first != last){
            auto* const target = first;
            first = static_cast<node*>(first->next);
            free(target);
        }
    }

    void free() noexcept
    {
        free(static_cast<node*>(head.next), nullptr);
    }

private:
    Allocator alloc{Allocator{}};
};
/* ------------------------------------------------------------------------- */

/* UnorderedMap */
/* ------------------------------------------------------------------------- */
template< typename Key, typename Value, typename Hash=std::hash<Key>
        , typename Alloc=std::allocator<std::pair<const Key, Value>> >
class UnorderedMap{
    template<typename,typename,typename,typename>
    friend class UnorderedMap_iterator;
    template<typename,typename,typename,typename>
    friend class UnorderedMap_const_iterator;
    template<typename K,typename V,typename H,typename A>
    friend bool assert_invariant(const UnorderedMap<K,V,H,A>& );
    using bucket_type     = UnorderedMapBucket<Key,Value,Alloc>;
    using bucket_node_type= typename bucket_type::node_type;
    using V_Allocator     = typename std::allocator_traits<Alloc>::template
                                rebind_alloc<std::pair<const Key,Value>>;
    using V_alloc_traits  = std::allocator_traits<V_Allocator>;
    using Allocator       = typename std::allocator_traits<Alloc>::template
                              rebind_alloc<bucket_type>;
    using alloc_traits    = std::allocator_traits<Allocator>;
    using self            = UnorderedMap;
public:
    using allocator_type  = Allocator;
    using hasher_type     = Hash;
    using key_type        = Key;
    using mapped_type     = Value;
    using value_type      = std::pair<const Key, Value>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using difference_type = typename alloc_traits::difference_type;
    using size_type       = typename alloc_traits::size_type;

    using local_iterator       = bucket_type*;
    using const_local_iterator = const bucket_type*;
    using iterator       = UnorderedMap_iterator<Key,Value,Hash,Alloc>;
    using const_iterator = UnorderedMap_const_iterator<Key,Value,Hash,Alloc>;

    explicit UnorderedMap( size_type bucket_count=19 )
        : count{bucket_count}, buckets{alloc_n(bucket_count)}
        { }

    ~UnorderedMap() noexcept { free(); }

// --- iterators
    iterator       begin() { return iterator{ibegin(), iend()}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator{icbegin(), icend()}; }
    iterator       end() { return iterator{iend(), iend()}; }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator{icend(), icend()}; }

// --- capacity
    bool empty() const noexcept
    {
        return std::all_of(buckets, buckets+count,
                [](const bucket_type& bucket){return bucket.empty();});
        // for(const auto* pb = buckets; pb != buckets+count; ++pb){
        //     if(!pb->empty()) return false;
        // }
        // return true;
    }

    size_type size() const noexcept
    {
        // auto nbuckets = size_type{0};
        // for(const auto* pb = buckets; pb != buckets+count; ++pb){
        //     nbuckets += pb->size();
        // }
        // return nbuckets;
        return std::accumulate(buckets, buckets+count, static_cast<size_type>(0)
                              ,[](size_type& n, const bucket_type& bucket)
                               {return n+bucket.size();});
    }

// --- lookup
    iterator find(const key_type& key)
    {
        const auto bucket_it = get_bucket(key);
        auto* const res = bucket_it->find(key);
        if(res)
            return iterator{bucket_it, iend(), res};
        else
            return end();
    }

    const_iterator find(const key_type& key) const noexcept
    {
        const auto bucket_it = get_bucket(key);
        const auto* const res = bucket_it->find(key);
        if(res)
            return const_iterator{bucket_it, icend(), res};
        else
            return end();
    }

// --- modifiers

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

    void clear() noexcept
    {
        for(auto* pb = buckets; pb != buckets+count; ++pb){
            pb->clear();
        }
    }

    size_type erase(const key_type& key)
    {
        auto bucket = get_bucket(key);
        return bucket->erase(key);
    }

    iterator erase( const_iterator pos )
    {
    // get the bucket from hint
        auto bucket_it = const_cast<local_iterator>(pos.bucket_it);
        const auto bucket_end = const_cast<local_iterator>(pos.bucket_end);
        auto* const res = bucket_it->erase(pos.node);
        return iterator{bucket_it, bucket_end, res};
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        while(first != last){
            auto temp = erase(first);
            first = temp;
        }
        return iterator{first.bucket_it, first.bucket_end, first.node};
    }

protected:
    local_iterator       ibegin() { return buckets; }
    const_local_iterator ibegin() const { return icbegin(); }
    const_local_iterator icbegin() const { return buckets; }
    local_iterator       iend() { return buckets + count; }
    const_local_iterator iend() const { return icend(); }
    const_local_iterator icend() const { return buckets + count; }

    bucket_type* alloc_n(size_type n, const bucket_type& val=bucket_type{})
    {
        auto* const pb = alloc.allocate(n);
        std::uninitialized_fill_n(pb, n, val);
        return pb;
    }

    const_local_iterator get_bucket(const key_type& key) const
    {
        const auto bucket_index = hasher(key) % count;
        auto it = icbegin();
        std::advance(it, bucket_index);
        return it;
    }
    local_iterator get_bucket(const key_type& key)
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
/* ------------------------------------------------------------------------- */

/* Iterators */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value, typename Hash, typename Alloc>
class UnorderedMap_iterator{
    template<typename,typename,typename,typename>friend class UnorderedMap;
    template<typename,typename,typename> friend class UnorderedMapBucket;
    using map_type             = UnorderedMap<Key,Value,Hash,Alloc>;
    using bucket_type          = typename map_type::bucket_type;
    using bucket_node_type     = typename bucket_type::node;
    using local_iterator       = typename map_type::local_iterator;
    using const_local_iterator = typename map_type::const_local_iterator;
    using self                 = UnorderedMap_iterator;
public:
    using value_type        = typename bucket_type::value_type;
    using reference         = typename bucket_type::reference;
    using pointer           = typename bucket_type::pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = typename map_type::difference_type;
    using const_iterator    = UnorderedMap_const_iterator<Key,Value,Hash,Alloc>;

    UnorderedMap_iterator( local_iterator begin_, local_iterator end_ )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node{ bucket_it != bucket_end ?
                  static_cast<bucket_node_type*>(begin_->head.next)
                : nullptr}
        { first_nonempty(); }

    UnorderedMap_iterator( local_iterator begin_, local_iterator end_, 
                           bucket_node_type* node_, bool ensure_nonempty=false )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node{node_}
        { if(ensure_nonempty || node == nullptr) first_nonempty(); }

    reference operator*() const noexcept
    { return node->data; }

    pointer operator->() const noexcept
    { return &(self::operator*()); }

    self& operator++() noexcept
    {
        if(bucket_it != bucket_end){
            if( !node || !(node = static_cast<bucket_node_type*>(node->next)) ){
                while(++bucket_it != bucket_end && !bucket_it->head.next)
                    ;
                if(bucket_it != bucket_end)
                    node = static_cast<bucket_node_type*>(bucket_it->head.next);
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

    operator const_iterator() const noexcept
        { return const_iterator{bucket_it, bucket_end, node}; }

template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;

protected:
    UnorderedMap_iterator( const_local_iterator begin_, const_local_iterator end_,
                           const bucket_node_type* node_, bool ensure_nonempty=false )
        : bucket_it{const_cast<local_iterator>(begin_)}
        , bucket_end{const_cast<local_iterator>(end_)}
        , node{const_cast<bucket_node_type*>(node_)}
        { if(ensure_nonempty) first_nonempty(); }

    void first_nonempty() noexcept
    {
        if(node) return;
        while(bucket_it != bucket_end && !bucket_it->head.next)
            ++bucket_it;
        if(bucket_it != bucket_end)
            node = static_cast<bucket_node_type*>(bucket_it->head.next);
    }

private:
    local_iterator    bucket_it;
    local_iterator    bucket_end;
    bucket_node_type* node;
};

template<typename Key, typename Value, typename Hash, typename Alloc>
class UnorderedMap_const_iterator{
    template<typename,typename,typename,typename>friend class UnorderedMap;
    template<typename,typename,typename> friend class UnorderedMapBucket;
    using map_type             = UnorderedMap<Key,Value,Hash,Alloc>;
    using bucket_type          = typename map_type::bucket_type;
    using bucket_node_type     = const typename bucket_type::node;
    using local_iterator       = typename map_type::const_local_iterator;
    using self                 = UnorderedMap_const_iterator;
public:
    using value_type        = typename bucket_type::value_type;
    using reference         = typename bucket_type::const_reference;
    using pointer           = typename bucket_type::const_pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = typename map_type::difference_type;

    UnorderedMap_const_iterator( local_iterator begin_, local_iterator end_ )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node{bucket_it != bucket_end ?
                  static_cast<bucket_node_type*>(begin_->head.next)
                : nullptr}
        { first_nonempty(); }

    UnorderedMap_const_iterator( local_iterator begin_, local_iterator end_, 
                           bucket_node_type* node_, bool ensure_nonempty=false )
        : bucket_it{begin_}
        , bucket_end{end_}
        , node{node_}
        { if(ensure_nonempty || node==nullptr) first_nonempty(); }

    reference operator*() const noexcept
    { return node->data; }

    pointer operator->() const noexcept
    { return &(self::operator*()); }

    self& operator++() noexcept
    {
        if(bucket_it != bucket_end){
            if( !node || !(node = static_cast<bucket_node_type*>(node->next)) ){
                while(++bucket_it != bucket_end && !bucket_it->head.next)
                    ;
                if(bucket_it != bucket_end)
                    node = static_cast<bucket_node_type*>(bucket_it->head.next);
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

template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;
template<typename K, typename V, typename H, typename A>
friend bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept;

protected:
    void first_nonempty() noexcept
    {
        if(node) return;
        while(bucket_it != bucket_end && !bucket_it->head.next)
            ++bucket_it;
        if(bucket_it != bucket_end)
            node = static_cast<bucket_node_type*>(bucket_it->head.next);
    }

private:
    local_iterator    bucket_it;
    local_iterator    bucket_end;
    bucket_node_type* node;
};

template<typename K, typename V, typename H, typename A>
bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node == rhs.node && lhs.bucket_it == rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node != rhs.node || lhs.bucket_it != rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node == rhs.node && lhs.bucket_it == rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node != rhs.node || lhs.bucket_it != rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator==( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node == rhs.node && lhs.bucket_it == rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator!=( const UnorderedMap_const_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node != rhs.node || lhs.bucket_it != rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator==( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node == rhs.node && lhs.bucket_it == rhs.bucket_it;
}
template<typename K, typename V, typename H, typename A>
bool operator!=( const UnorderedMap_iterator<K,V,H,A>& lhs,
                        const UnorderedMap_const_iterator<K,V,H,A>& rhs ) noexcept
{
    return lhs.node != rhs.node || lhs.bucket_it != rhs.bucket_it;
}
/* ------------------------------------------------------------------------- */
#endif /* UNORDERED_MAP_STABLE_ABI_H_ */
