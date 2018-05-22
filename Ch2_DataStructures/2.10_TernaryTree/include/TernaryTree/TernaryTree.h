#ifndef TERNARY_TREE_GUARD_H_
#define TERNARY_TREE_GUARD_H_

#include <memory>
#include <utility>
#include <stack>
#include <string>
#include <string_view>
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


/* TernaryTreeNode */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value>
struct TernaryTreeNode
{
    using self = TernaryTreeNode;
    using key_type = const typename Key::value_type;
    using value_type = Value;

    template<typename... Args>
    TernaryTreeNode(key_type ch, Args&&... args)
        : key{ch}, value{std::forward<Args>(args)...} { }

    key_type   key;
    value_type value{};
    self*      left{nullptr};
    self*      mid{nullptr};
    self*      right{nullptr};
};
/* ------------------------------------------------------------------------- */

/* TernaryTree */
/* ------------------------------------------------------------------------- */
template<typename Key, typename Value,
    typename Alloc=std::allocator<Value>>
class TernaryTree
{
    using self            = TernaryTree;
    using node            = TernaryTreeNode<Key, Value>;
    using Allocator       = typename std::allocator_traits<Alloc>::template
                            rebind_alloc<node>;
    using alloc_traits    = std::allocator_traits<Allocator>;
    using string_view_t   = std::basic_string_view<typename Key::value_type>;
public:
    using node_key_type   = const typename node::key_type;
    using key_type        = Key;
    using value_type      = Value;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = typename alloc_traits::pointer;
    using const_pointer   = typename alloc_traits::const_pointer;
    using difference_type = typename alloc_traits::difference_type;
    using size_type       = typename alloc_traits::size_type;
    using node_type       = node;

    explicit TernaryTree() noexcept = default;
    ~TernaryTree() noexcept { free(); }

// --- capacity
    bool empty() const noexcept { return root == nullptr; }

// --- lookup

    value_type& find_value(const key_type& key) noexcept
    {
        return find_value_impl(*this, key);
    }

    const value_type& find_value(const key_type& key) const noexcept
    {
        return find_value_impl(*this, key);
    }

    template<typename T>
    static auto find_value_impl(T& obj, const key_type& key)
        -> decltype(obj.find_value(key))
    {
        return find_value_impl_helper(obj.root, string_view_t{key});
    }

    template<typename T>
    static auto find_value_impl_helper(T* pnode, string_view_t keys)
        -> decltype((pnode->value))// note the additional parens -makes the 
                                   // pnode->value an expression which causes
                                   // decltype to evalueate it as an lvalue
    {
        const auto key = keys.front();
        if( key < pnode->key )
            return find_value_impl_helper(pnode->left, keys);
        else if(key == pnode->key){
            if(1 < keys.size())
                return find_value_impl_helper(pnode->mid, keys.substr(1));
            else
                return pnode->value;
        }
        else
            return find_value_impl_helper(pnode->right, keys);
    }

// --- modifiers

    void insert(const key_type& key, const value_type& value)
    {
        auto** root_node = find_node(key.front());
        *root_node = insert_impl(root_node, string_view_t{key}, value);
    }

protected:
    template<typename... Args>
    node* make_node(node_key_type key, Args&&... args)
    {
        auto* const nn = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, nn, key, std::forward<Args>(args)...);
        return nn;
    }

    node** find_node(node_key_type key) noexcept
    {
        return find_node_recur(&root, key);
    }

    static node** find_node_recur(node** target, node_key_type key) noexcept
    {
        if(*target){
            if((*target)->key == key) return target;
            if((*target)->key < key) return find_node_recur(&((*target)->right), key);
            if(key < (*target)->key) return find_node_recur(&((*target)->left), key);
        }
        return target;
    }

    node* insert_impl(node** current, string_view_t keys, const value_type& value)
    {
        const auto k = keys.front();

        if(!*current)
            *current = make_node(k);

        if(k < (*current)->key)
            (*current)->left = insert_impl(&(*current)->left, keys, value);
        else if(k == (*current)->key){
            if(1 < keys.size())
                (*current)->mid = insert_impl(&(*current)->mid, keys.substr(1), value);
            else
                (*current)->value = value;
        }
        else
            (*current)->right = insert_impl(&(*current)->right, keys, value);

        return *current;
    }

    void free(node* target) noexcept
    {
        alloc_traits::destroy(alloc, target);
        alloc_traits::deallocate(alloc, target, 1);
    }

    void free() noexcept
    {
        free_recur(root);
        root = nullptr;
    }

    void free_recur(node* target) noexcept
    {
        if(target){
            free_recur(target->left);
            free_recur(target->mid);
            free_recur(target->right);
            free(target);
        }
    }

private:
    Allocator alloc{};
    node* root;
};
/* ------------------------------------------------------------------------- */

#endif /* TERNARY_TREE_GUARD_H_ */
