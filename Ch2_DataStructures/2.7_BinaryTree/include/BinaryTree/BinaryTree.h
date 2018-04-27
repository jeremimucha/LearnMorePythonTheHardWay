#ifndef BINARY_TREE_GUARD_H_
#define BINARY_TREE_GUARD_H_

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


template<typename,typename,typename> class BinaryTree;
template<typename,typename> struct BinaryTreeNode;
template<typename,typename,typename> class BinaryTree_iterator;
template<typename,typename,typename> class BinaryTree_const_iterator;

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
    using self = BinaryTree;
    using node = BinaryTreeNode<Key,Value>;
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

    explicit BinaryTree() noexcept = default;

// --- capacity
    bool empty() const noexcept { return root == nullptr; }

protected:
    template<typename... Args>
    node* make_node(Args&&...)
    {
        auto* const nn = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(nn, std::forward<Args>(args)...);
        return nn;
    }

    node* get_smallest() noexcept
    {
        if(!root) return nullptr;
        auto* const target = root;
        while(target && target->left){
            target = target->left;
        }
        return target;
    }

    void free(node* target) noexcept
    {
        alloc_traits::destroy(alloc, traget);
        alloc_traits::deallocate(alloc, target, 1);
    }
private:
    Allocator alloc{Allocator{}};
    node* root{nullptr};
};
/* ------------------------------------------------------------------------- */

#endif /* BINARY_TREE_GUARD_H_ */
