#ifndef BINARY_TREE_GUARD_H_
#define BINARY_TREE_GUARD_H_

#include <memory>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <stack>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif


template<typename,typename,typename> class BinaryTree;
template<typename,typename> struct BinaryTreeNode;
template<typename,typename> class BinaryTree_iterator_base;
template<typename,typename,typename> class BinaryTree_iterator;
template<typename,typename,typename> class BinaryTree_const_iterator;

struct inorder_tag{};
struct preorder_tag{};
struct postorder_tag{};

template<typename T> struct BinaryTree_iterator_trait
{ using tag = preorder_tag; };
template<> struct BinaryTree_iterator_trait<inorder_tag>
{ using tag = inorder_tag; };
template<> struct BinaryTree_iterator_trait<postorder_tag>
{ using tag = postorder_tag; };

/* BinaryTreeNode */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value>
struct BinaryTreeNode
{
    using self        = BinaryTreeNode;
    using key_type    = Key;
    using mapped_type = Value;
    using value_type  = std::pair<const Key,Value>;

    template<typename... Args>
    explicit BinaryTreeNode( Args&&... args )
        : data{std::forward<Args>(args)...} { }

    self*      left{nullptr};
    self*      right{nullptr};
    value_type data;
};
/* ------------------------------------------------------------------------- */

/* BinaryTree */
/* ------------------------------------------------------------------------- */
template< typename Key, typename Value
        , typename Alloc=std::allocator<std::pair<const Key,Value>> >
class BinaryTree
{
    template<typename,typename> friend class BinaryTree_iterator_base;
    template<typename,typename,typename> friend class BinaryTree_iterator;
    template<typename,typename,typename> friend class BinaryTree_const_iterator;
    using self = BinaryTree;
    using node = BinaryTreeNode<const Key,Value>;
    using Allocator = typename std::allocator_traits<Alloc>::template
                        rebind_alloc<node>;
    using alloc_traits = std::allocator_traits<Allocator>;
public:
    using value_type      = std::pair<const Key,Value>;
    using key_type        = Key;
    using mapped_type     = Value;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = typename alloc_traits::pointer;
    using const_pointer   = typename alloc_traits::const_pointer;
    using difference_type = typename alloc_traits::difference_type;
    using size_type       = typename alloc_traits::size_type;
    using node_type       = node;

    // using iterator = BinaryTree_iterator<Key,Value,Alloc>;
    // using const_iterator = BinaryTree_const_iterator<Key,Value,Alloc>;
    // using iterator = BinaryTree_iterator_base<node*, reference>;
    // using const_iterator = BinaryTree_iterator_base<const node*, const_reference>;
    using iterator = BinaryTree_iterator<node*, reference, BinaryTree_iterator_trait<void>>;
    using const_iterator = BinaryTree_iterator<const node*, const_reference, BinaryTree_iterator_trait<void>>;
    using inorder_iterator = BinaryTree_iterator<node*, reference, inorder_tag>;
    using const_inorder_iterator = BinaryTree_iterator<const node*, const_reference, inorder_tag>;
    using postorder_iterator = BinaryTree_iterator<node*, reference, postorder_tag>;
    using const_postorder_iterator = BinaryTree_iterator<const node*, const_reference, postorder_tag>;
    explicit BinaryTree() noexcept = default;
    ~BinaryTree() noexcept { free(); }

// --- capacity
    bool empty() const noexcept { return root == nullptr; }

    size_type size() noexcept
    {
        auto count = size_type{0};
        auto counter = [&count](node*){++count;};
        traverse_preorder(counter);
        return count;
    }

// --- iterators
    iterator begin() { return iterator{root}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator{root}; }
    iterator end() { return iterator{}; }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator{}; }

    inorder_iterator inorder_begin() { return inorder_iterator{root}; }
    const_inorder_iterator inorder_begin() const { return cbegin(); }
    const_inorder_iterator inorder_cbegin() const { return const_inorder_iterator{root}; }
    inorder_iterator inorder_end() { return inorder_iterator{}; }
    const_inorder_iterator inorder_end() const { return cend(); }
    const_inorder_iterator inorder_cend() const { return const_inorder_iterator{}; }

    postorder_iterator postorder_begin() { return postorder_iterator{root}; }
    const_postorder_iterator postorder_begin() const { return cbegin(); }
    const_postorder_iterator postorder_cbegin() const { return const_postorder_iterator{root}; }
    postorder_iterator postorder_end() { return postorder_iterator{}; }
    const_postorder_iterator postorder_end() const { return cend(); }
    const_postorder_iterator postorder_cend() const { return const_postorder_iterator{}; }

    std::pair<node_type*,bool> insert(const value_type& value)
    {
        if(root == nullptr){
            auto* const nn = make_node(value);
            root = nn;
            return {root, true};
        }
        for( auto* target = root; target != nullptr; /**/ ){
            if(value.first < target->data.first){
                if(target->left == nullptr){
                    auto* const nn = make_node(value);
                    target->left = nn;
                    return {target->left, true};
                }
                target = target->left;
            }
            else if(target->data.first < value.first){
                if(target->right == nullptr){
                    auto* const nn = make_node(value);
                    target->right = nn;
                    return {target->right, true};
                }
                target = target->right;
            }
            else // target->data.first == value.first
                return {target, false};
        }
        return {nullptr, false};
    }

    node_type* find(const key_type& key) noexcept
    {
        auto pred = [&key](const node* n){
            if(key < n->data.first) return Cmp::LT;
            if(n->data.first < key) return Cmp::GT;
            return Cmp::EQ;
        };
        return find_node(pred);
    }

protected:
    enum class Cmp{ LT, LE, EQ, GE, GT};

    template<typename... Args>
    node* make_node(Args&&... args)
    {
        auto* const nn = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, nn, std::forward<Args>(args)...);
        return nn;
    }

    template<typename Function>
    void traverse_preorder_recursive(node* target, Function func) noexcept
    {
        if(target){
            func(target);
            traverse_preorder(target->left, func);
            traverse_preorder(target->right, func);
        }
    }

    template<typename Function>
    void traverse_inorder_recursive(node* target, Function func) noexcept
    {
        if(target){
            traverse_inorder(target->left, func);
            func(target);
            traverse_inorder(target->right, func);
        }
    }

    template<typename Function>
    void traverse_postorder_recursive(node* target, Function func) noexcept
    {
        if(target){
            traverse_postorder(target->left, func);
            traverse_postorder(target->right, func);
            func(target);
        }
    }

    template<typename Function>
    node* find_node(Function predicate) noexcept
    {
        return find_node_impl(*this, predicate);
    }

    template<typename Function>
    const node* find_node(Function predicate) const noexcept
    {
        return find_node_impl(*this, predicate);
    }

    template<typename T, typename Function>
    static auto find_node_impl(T& obj, Function predicate)
    -> decltype(obj.find_node(predicate))
    {
        for( auto* target = obj.root; target != nullptr; /**/ ){
            const auto res = predicate(target);
            if(res == Cmp::EQ) return target;
            if(res == Cmp::LT) target = target->left;
            else if(res == Cmp::GT) target = target->right;
        }
        return nullptr;
    }

    template<typename Function>
    void traverse_inorder(Function func) noexcept
    {
        std::stack<node*> nodes;
        for( auto* target = root; target != nullptr; /**/ ){
            nodes.push(target);
            target = target->left;
            if(target == nullptr){
                while(!nodes.empty()){
                    target = nodes.top();
                    nodes.pop();
                    func(target);
                    target = target->right;
                    if(target)
                        break;
                }
            }
        }
    }

    template<typename Function>
    void traverse_preorder(Function func) noexcept
    {
        std::stack<node*> nodes;
        for( auto* target = root; target != nullptr; /**/ ){
            func(target);
            if(target->right)
                nodes.push(target->right);
            target = target->left;
            if(target == nullptr && !nodes.empty()){
                target = nodes.top();
                nodes.pop();
            }
        }
    }

    template<typename Function>
    void traverse_postorder(Function func) noexcept
    {
        std::stack<node*> nodes;
        for( auto* target = root; target != nullptr; /**/ ){
            nodes.push(target);
            target = target->right;
            if(target == nullptr){
                while(!nodes.empty()){
                    target = nodes.top();
                    nodes.pop();
                    func(target);
                    target = target->left;
                    if(target)
                        break;
                }
            }
        }
    }

    void free(node* target) noexcept
    {
        alloc_traits::destroy(alloc, target);
        alloc_traits::deallocate(alloc, target, 1);
    }

    void free() noexcept
    {
        // auto free_node = [this](node* target){
        //     alloc_traits::destroy(alloc, target);
        //     alloc_traits::deallocate(alloc, target, 1);
        //     };
        // if(root) traverse_inorder(root, free_node);
        std::stack<node*> nodes;
        for(auto* target = root; target != nullptr;){
            auto* const temp = target;
            if(target->right)
                nodes.push(target->right);
            target = target->left;
            if(target == nullptr && !nodes.empty()){
                target = nodes.top();
                nodes.pop();
            }
            free(temp);
        }
    }
private:
    Allocator alloc{Allocator{}};
    node* root{nullptr};
};
/* ------------------------------------------------------------------------- */

/* Iterators */
/* ------------------------------------------------------------------------- */

template<typename NodePointer, typename Reference>
class BinaryTree_iterator_base
{
protected:
    using self = BinaryTree_iterator_base;
    using node = std::remove_pointer_t<NodePointer>;
    using stack_type = std::stack<node*>;
public:
    using value_type = std::remove_cv_t< std::remove_reference_t<Reference> >;
    using reference = Reference;
    using pointer = std::add_pointer_t< std::remove_reference_t<Reference> >;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    BinaryTree_iterator_base() = default;

    explicit BinaryTree_iterator_base( node* n )
        : node_stack{}, current{n} { }

    template<typename OtherNodePointer, typename OtherReference,
        typename = std::enable_if_t<std::is_convertible_v<OtherNodePointer, NodePointer>>
        >
    BinaryTree_iterator_base( BinaryTree_iterator_base<OtherNodePointer, OtherReference>& other )
        : node_stack{other.node_stack}, current{other.current} { }

    reference operator*() const noexcept
    { return current->data; }

    pointer operator->() const noexcept
    { return &(self::operator*()); }

    // self& operator++() noexcept
    // {
    //     if(current->right)
    //         node_stack.push(current->right);
    //     current = current->left;
    //     if(!current && !node_stack.empty()){
    //         current = node_stack.top();
    //         node_stack.pop();
    //     } // else == end iterator
    //     return *this;
    // }

    // self operator++(int) noexcept
    // {
    //     auto ret = *this;
    //     ++*this;
    //     return ret;
    // }

template<typename NP1, typename R1, typename NP2, typename R2>
friend bool operator==( const BinaryTree_iterator_base<NP1,R1>& lhs,
                        const BinaryTree_iterator_base<NP2,R2>& rhs ) noexcept;
template<typename NP1, typename R1, typename NP2, typename R2>
friend bool operator!=( const BinaryTree_iterator_base<NP1,R1>& lhs,
                        const BinaryTree_iterator_base<NP2,R2>& rhs ) noexcept;

template<typename T1, typename NP1, typename R1, typename T2, typename NP2, typename R2>
friend bool operator==( const BinaryTree_iterator<T1,NP1,R1>& lhs,
                        const BinaryTree_iterator<T2,NP2,R2>& rhs ) noexcept;
template<typename T1, typename NP1, typename R1, typename T2, typename NP2, typename R2>
friend bool operator!=( const BinaryTree_iterator<T1,NP1,R1>& lhs,
                        const BinaryTree_iterator<T2,NP2,R2>& rhs ) noexcept;

protected:
    stack_type node_stack;
    node*      current;
};

template<typename NP1, typename R1, typename NP2, typename R2>
bool operator==( const BinaryTree_iterator_base<NP1,R1>& lhs,
                        const BinaryTree_iterator_base<NP2,R2>& rhs ) noexcept
{ return lhs.current == rhs.current && lhs.node_stack.empty() == rhs.node_stack.empty(); }
template<typename NP1, typename R1, typename NP2, typename R2>
bool operator!=( const BinaryTree_iterator_base<NP1,R1>& lhs,
                        const BinaryTree_iterator_base<NP2,R2>& rhs ) noexcept
{ return lhs.current != rhs.current /* && lhs.node_stack.empty() == rhs.node_stack.empty() */; }

// struct inorder_tag{};
// struct preorder_tag{};
// struct postorder_tag{};

// template<typename T> struct BinaryTree_iterator_trait
// { using tag = preorder_tag; };
// template<> struct BinaryTree_iterator_trait<inorder_tag>
// { using tag = inorder_tag; };
// template<> struct BinaryTree_iterator_trait<postorder_tag>
// { using tag = postorder_tag; };

template<typename NodePointer, typename Reference, typename Tag=preorder_tag>
class BinaryTree_iterator : public BinaryTree_iterator_base<NodePointer, Reference>
{
    using self = BinaryTree_iterator;
public:
    using BinaryTree_iterator_base<NodePointer,Reference>::BinaryTree_iterator_base;

    self& operator++() noexcept
    {
        if(this->current->right)
            this->node_stack.push(this->current->right);
        this->current = this->current->left;
        if(!this->current && !this->node_stack.empty()){
            this->current = this->node_stack.top();
            this->node_stack.pop();
        } // else == end iterator
        return *this;
    }

    self operator++(int) noexcept
    {
        auto ret = *this;
        ++*this;
        return ret;
    }

// template<typename NP1, typename R1, typename T1, typename NP2, typename R2, typename T2>
// friend bool operator==( const BinaryTree_iterator<NP1,R1,T1>& lhs,
//                         const BinaryTree_iterator<NP2,R2,T2>& rhs ) noexcept;
// template<typename NP1, typename R1, typename T1, typename NP2, typename R2, typename T2>
// friend bool operator!=( const BinaryTree_iterator<NP1,R1,T1>& lhs,
//                         const BinaryTree_iterator<NP2,R2,T2>& rhs ) noexcept;
};

template<typename NP1, typename R1, typename T1, typename NP2, typename R2, typename T2>
bool operator==( const BinaryTree_iterator<NP1,R1,T1>& lhs,
                        const BinaryTree_iterator<NP2,R2,T2>& rhs ) noexcept
{ return lhs.current == rhs.current && lhs.node_stack.empty() == rhs.node_stack.empty(); }
template<typename NP1, typename R1, typename T1, typename NP2, typename R2, typename T2>
bool operator!=( const BinaryTree_iterator<NP1,R1,T1>& lhs,
                        const BinaryTree_iterator<NP2,R2,T2>& rhs ) noexcept
{ return lhs.current != rhs.current /* && lhs.node_stack.empty() == rhs.node_stack.empty() */; }


template<typename NodePointer, typename Reference>
class BinaryTree_iterator<NodePointer, Reference, inorder_tag>
    : public BinaryTree_iterator_base<NodePointer, Reference>
{
    using self = BinaryTree_iterator;
    using base = BinaryTree_iterator_base<NodePointer,Reference>;
    using node = typename base::node;
public:
    using BinaryTree_iterator_base<NodePointer,Reference>::BinaryTree_iterator_base;
    explicit BinaryTree_iterator( node* n )
        : base{n}
        {
            if(!this->current) return;
            while(this->current->left){
                this->node_stack.push(this->current);
                this->current = this->current->left;
            }
        }

    self& operator++() noexcept
    {
        this->current = this->current->right;
        if(this->current){
            while(this->current->left){
                this->node_stack.push(this->current);
                this->current = this->current->left;
            }
        }else if(!this->node_stack.empty()){
            this->current = this->node_stack.top();
            this->node_stack.pop();
        } // else == end iterator

        return *this;
    }

    self operator++(int) noexcept
    {
        auto ret = *this;
        ++*this;
        return ret;
    }
};

template<typename NodePointer, typename Reference>
class BinaryTree_iterator<NodePointer,Reference,postorder_tag>
    : public BinaryTree_iterator_base<NodePointer,Reference>
{
    using self = BinaryTree_iterator;
    using base = BinaryTree_iterator_base<NodePointer,Reference>;
    using node = typename base::node;
public:
    using base::BinaryTree_iterator_base;
    explicit BinaryTree_iterator( node* n )
        : base{n}
        {
            if(!this->current) return;
            while(this->current->right){
                this->node_stack.push(this->current);
                this->current = this->current->right;
            }
        }

    self& operator++() noexcept
    {
        this->current = this->current->left;
        if(this->current){
            while(this->current->right){
                this->node_stack.push(this->current);
                this->current = this->current->right;
            }
        } else if(!this->node_stack.empty()){
            this->current = this->node_stack.top();
            this->node_stack.pop();
        } // else == end_iterator
        return *this;
    }

    self operator++(int) noexcept
    {
        auto ret = *this;
        ++*this;
        return ret;
    }
};

// template<typename Key,typename Value, typename Alloc>
// class BinaryTree_iterator
// {
//     using container = BinaryTree<Key,Value,Alloc>;
//     using node = typename container::node;
//     using self = BinaryTree_iterator;
//     using stack_type = std::stack<node*>;
// public:
//     using value_type = typename container::value_type;
//     using reference  = typename container::reference;
//     using pointer    = typename container::pointer;
//     using iterator_category = std::forward_iterator_tag;
//     using difference_type = typename container::difference_type;
//     using const_iterator = BinaryTree_const_iterator<Key,Value,Alloc>;

//     BinaryTree_iterator() = default;
//     explicit BinaryTree_iterator( node* n )
//         : nodes{}, current{n} { }

//     reference operator*() const noexcept
//     { return current->data; }

//     pointer operator->() const noexcept
//     { return &(self::operator*()); }

//     self& operator++() noexcept
//     {
//         if(current->right)
//             nodes.push(current->right);
//         current = current->left;
//         if(!current && !nodes.empty()){
//             current = nodes.top();
//             nodes.pop();
//         } // else == end iterator
//         return *this;
//     }

//     self operator++(int) noexcept
//     {
//         auto ret = *this;
//         ++*this;
//         return ret;
//     }

//     operator const_iterator() const noexcept
//     { return const_iterator{current}; }


// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;

// private:
//     stack_type nodes;
//     node*      current{nullptr};
// };

// template<typename Key,typename Value, typename Alloc>
// class BinaryTree_const_iterator
// {
//     using container = BinaryTree<Key,Value,Alloc>;
//     using node = const typename container::node;
//     using self = BinaryTree_const_iterator;
//     using stack_type = std::stack<node*>;
// public:
//     using value_type = typename container::value_type;
//     using reference  = typename container::const_reference;
//     using pointer    = typename container::const_pointer;
//     using iterator_category = std::forward_iterator_tag;
//     using difference_type = typename container::difference_type;

//     BinaryTree_const_iterator() = default;
//     explicit BinaryTree_const_iterator( node* n )
//         : nodes{}, current{n} { }

//     reference operator*() const noexcept
//     { return current->data; }

//     pointer operator->() const noexcept
//     { return &(self::operator*()); }

//     self& operator++() noexcept
//     {
//         if(current->right)
//             nodes.push(current->right);
//         current = current->left;
//         if(!current && !nodes.empty()){
//             current = nodes.top();
//             nodes.pop();
//         } // else == end iterator
//         return *this;
//     }

//     self operator++(int) noexcept
//     {
//         auto ret = *this;
//         ++*this;
//         return ret;
//     }

// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator==( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;
// template<typename K, typename V, typename A>
// friend bool operator!=( const BinaryTree_const_iterator<K,V,A>& lhs,
//                         const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept;

// private:
//     stack_type nodes;
//     node*      current{nullptr};
// };


// template<typename K, typename V, typename A>
// bool operator==( const BinaryTree_iterator<K,V,A>& lhs,
//                  const BinaryTree_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current == rhs.current && lhs.nodes.empty() == rhs.nodes.empty(); }

// template<typename K, typename V, typename A>
// bool operator!=( const BinaryTree_iterator<K,V,A>& lhs,
//                  const BinaryTree_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current != rhs.current /* || lhs.nodes.empty() != rhs.nodes.empty() */; }

// template<typename K, typename V, typename A>
// bool operator==( const BinaryTree_const_iterator<K,V,A>& lhs,
//                  const BinaryTree_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current == rhs.current && lhs.nodes.empty() == rhs.nodes.empty(); }

// template<typename K, typename V, typename A>
// bool operator!=( const BinaryTree_const_iterator<K,V,A>& lhs,
//                  const BinaryTree_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current != rhs.current /* || lhs.nodes.empty() != rhs.nodes.empty() */; }

// template<typename K, typename V, typename A>
// bool operator==( const BinaryTree_iterator<K,V,A>& lhs,
//                  const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current == rhs.current && lhs.nodes.empty() == rhs.nodes.empty(); }

// template<typename K, typename V, typename A>
// bool operator!=( const BinaryTree_iterator<K,V,A>& lhs,
//                  const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current != rhs.current /* || lhs.nodes.empty() != rhs.nodes.empty() */; }

// template<typename K, typename V, typename A>
// bool operator==( const BinaryTree_const_iterator<K,V,A>& lhs,
//                  const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current == rhs.current && lhs.nodes.empty() == rhs.nodes.empty(); }

// template<typename K, typename V, typename A>
// bool operator!=( const BinaryTree_const_iterator<K,V,A>& lhs,
//                  const BinaryTree_const_iterator<K,V,A>& rhs ) noexcept
// { return lhs.current != rhs.current /* || lhs.nodes.empty() != rhs.nodes.empty() */; }


/* ------------------------------------------------------------------------- */

#endif /* BINARY_TREE_GUARD_H_ */
