#pragma once
#include <functional>
#include <cassert>

template <class Key, class Value, class Compare = std::less<Key>>
class RBTree
{
public:
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;

private:
    enum class Color
    {
        Red,
        Black
    };
    struct NodeBase
    {
        Color color = Color::Black;
        NodeBase *parent = nullptr;
        NodeBase *left = nullptr;
        NodeBase *right = nullptr;
    };

    struct Node : NodeBase
    {
        value_type data;
        template <class K, class V>
        Node(K &&key, V &&value)
            : data(
                  std::piecewise_construct,
                  std::forward_as_tuple(std::forward<K>(key)),
                  std::forward_as_tuple(std::forward<V>(value))) {}
    };
    Compare comp_;
    NodeBase nil_; // 终止节点标记
    NodeBase *root_;
    size_type size_ = 0;

    void resetNil()
    {
        nil_.color = Color::Black;
        nil_.left = &nil_;
        nil_.right = &nil_;
        nil_.parent = &nil_;
    }
    // 性能敏感，不用dynamic_cast，NodeBase不使用虚函数
    static Node *asNode(NodeBase *p)
    {
        return static_cast<Node *>(p);
    }

    static const Node *asNode(const NodeBase *p)
    {
        return static_cast<const Node *>(p);
    }

    static const Key &keyOf(const NodeBase *p)
    {
        return asNode(p)->data.first;
    }

    void destroy(NodeBase *node) noexcept
    {
        if (node == &nil_)
        {
            return;
        }
        destroy(node->left);
        destroy(node->right);
        delete asNode(node);
    }

    NodeBase *findNode(const Key &key)
    {
        return const_cast<NodeBase *>(std::as_const(*this).findNode(key));
    }

    const NodeBase *findNode(const Key &key) const
    {
        const NodeBase *cur = root;
        while (cur != &nil_)
        {
            if (comp_(key, keyOf(cur)))
            {
                cur = cur->left;
            }
            else if (comp_(keyOf(cur), key))
            {
                cur = cur->right;
            }
            else
            {
                return cur;
            }
        }
        return &nil_;
    }

    void rotateLeft(NodeBase* x)
    {
		assert(x != &nil_);
		assert(x->right != &nil_);

        NodeBase* y = x->right;

        x->right = y->left;
        if (y->left != &nil_)
        {
            y->left->parent = x;
        }

        y->parent = x->parent;
        if (x->parent == &nil_)
        {
            root = y;
        }
        else if(x->parent->left == x)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
       
    }

    void rotateRight(NodeBase* x)
    {
		assert(x != &nil_);
		assert(x->left != &nil_);
        NodeBase* y = x->left;

        x->left = y->right;
        if (y->right != &nil_)
        {
            y->right->parent = x;
        }

        y->parent = x->parent;
        if (x->parent == &nil_)
        {
            root = y;
        }
        else if (x->parent->left == x)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }

        y->right = x;
        x->parent = y;
    }

    void fixInsert(NodeBase* z)
    {
        while (z->parent->color == Color::Red)
        {
            NodeBase* parent = z->parent;
            NodeBase* grand = parent->parent;
            if (parent == grand->left)
            {
                NodeBase* uncle = grand->right;
                if (uncle->color == red)
                {
					//          grand(black)
                    //  parent(red)     uncle(red)
                    //      z(red)
                    parent->color = Color::Black;
                    uncle->color = Color::Black;
                    grand->color = Color::Red;
					//          grand(red)
                    //  parent(black)     uncle(black)
                    //      z(red)
                    z = grand;
                }
                else
                {
                    if (z == parent->right)
                    {
						//          grand(black)
                        //  parent(red)     uncle(black)
                        //           z (red)
                        z = parent;
                        rotateLeft(z);
                    }
					//          grand(black)
                    //  parent(red)     uncle(black)
                    //z(red)
                    z->parent->color = black;
                    z->parent->parent->color = red;
					//          grand(red)
					//  parent(black)     uncle(black)
					//z(red)
                    rotateLeft(z->parent->parent);
                    //                 uncle(black)
					//          grand(red)
					//  parent(black)     
					//z(red)
                }
            }
            else
            {
                NodeBase* uncle = grand->left;

                if (uncle->color == Color::Red)
                {
					//          grand(black)
                    //      uncle(red)   parent(red)
                    //                      z(red)
                    parent->color = Color::Black;
                    uncle->color = Color::Black;
                    grand->color = Color::Red;
                    z = grand;
                }
                else
                {
					//          grand(black)
					//      uncle(black)   parent(red)
					//                       z(red)
                    if (z == parent->left)
                    {
						//          grand(black)
	                    //      uncle(black)   parent(red)
	                    //                    z(red)
                        z = parent;
                        rotateRight(z);
                    }
					//           grand(black)
                    //      uncle(black)   parent(red)
                    //                            z(red)
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
					//           grand(red)
					//      uncle(black)   parent(black)
					//                             z(red)                  
                    rotateRight(z->parent->parent);
					//           parent(black)
					//      grand(red)   z(red)
					//  uncle(black)                          
                }
            }
        }
    }

    void transplant(NodeBase* oldNode, NodeBase* newNode)
    {
        if (oldNode->parent == &nil_)
        {
            root = newNode;
        }
        else if (oldNode == oldNode->parent->left)
        {
            oldNode->parent->left = newNode;
        }
        else
        {
            oldNode->parent->right = newNode;
        }
        newNode->parent = oldNode->parent;
    }

public:
    explicit RBTree(Compare comp = Compare{}) : comp_(std::move(comp)), root(&nil)
    {
        resetNil();
    }

    ~RBTree()
    {
        clear();
    }

    void clear() noexcept
    {
        destroy();
        root = &nil_;
        size_ = 0;
        resetNil();
    }

    RBTree(const RBTree &) = delete;
    RBTree &operator=(const RBTree &) = delete;

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    size_type size() const noexcept
    {
        return siez_;
    }

    const Value *find(const Key &key) const
    {
        const NodeBase *node = findNode(key);
        return node == &nil_ ? nullptr : std::addressof(asNode(node)->data.second);
    }

    Value *find(const Key &key)
    {
        NodeBase *node = findNode(key);
        return node == &nil_ ? nullptr : std::addrressof(asNode(node)->data.second);
    }

    bool contains(const Key &key) const
    {
        return findNode(key) != &nil_;
    }

    template <class K, class V>
    std::pair<Value *, bool> insert(K &&key, V &&value)
    {
        NodeBase *cur = root;
        NodeBase* parent = &nil_;
        // find pos
        bool insertLeft = false;
        while (cur != &nil_)
        {
            parent = cur;
            if (comp_(key, keyOf(cur)))
            {
                insertLeft = true;
                cur = cur->left;
            }
            else if (comp_(keyOf(cur), key))
            {
                insertLeft = false;
                cur = cur->right;
            }
            else
            {
                // already exist
                return {std::addressof(asNode(cur)->data.second,false};
            }
        }
        Node* z = new Node(std::forward<K>(key), std::forward<V>(value));
        z->color = Color::Red;
        z->parent = parent;
        z->left = &nil_;
        z->right = &nil_;
        if (parent == &nil_)
        {
            //root
            root = z;
        }
        else if (insertLeft)
        {
            parent->left = z;
        }
        else
        {
            parent->right = z;
        }
        fixInsert(z);
        ++size_;
        return { std::addressof(asNode(z)->data.second), true };
    }

    bool erase(const Key& key)
    {
        NodeBase* z = findNode(key);
        if (z == &nil_)
        {
            return false;
        }

        return false;//TODO
    }

    void clear() noexcept
    {
        destroy(root);
        root = &nil_;
        size_ = 0;
        resetNil();
    }


};