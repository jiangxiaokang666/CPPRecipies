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
        const NodeBase *cur = root_;
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
            root_ = y;
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
            root_ = y;
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
                if (uncle->color == Color::Red)
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
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
					//          grand(red)
					//  parent(black)     uncle(black)
					//z(red)
                    rotateRight(z->parent->parent);
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
                    rotateLeft(z->parent->parent);
					//           parent(black)
					//      grand(red)   z(red)
					//  uncle(black)                          
                }
            }
        }
        root_->color = Color::Black;
    }

    void transplant(NodeBase* oldNode, NodeBase* newNode)
    {
        if (oldNode->parent == &nil_)
        {
            root_ = newNode;
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

    NodeBase* minimum(NodeBase* node) const
    {
        while (node->left != &nil_)
        {
            node = node->left;
        }
        return node;
    }

    void fixErase(NodeBase* x)
    {
        while (x != root_ && x->color == Color::Black)
        {
            
            if (x == x->parent->left)
            {
                //想办法把兄弟的右孩子变成红,这样可以把父节点和兄弟的右孩子变黑+左旋 保证黑高不变
                //那么兄弟就不能为红，因为不能两个红在一起
                NodeBase* brother = x->parent->right;
                //优先处理兄弟为红的情况
                if (brother->color == Color::Red)
                {
                    /*       parent(B)                                 (B)          
                    *    x(DB）    brother(R)         ->      parent(R)      (B)
                    *            bl(B)      br(B)          x(DB)   brother(B)   
                    */
                    brother->color = Color::Black;
                    brother->parent->color = Color::Red;
                    rotateLeft(brother->parent);
                    brother = x->parent->right;
                }
                /*
                *              parent(*)
                *          x(BB)   brother(B) 
                */
                //再处理兄弟的孩子
                if (brother->left->color == Color::Black && brother->right->color == Color::Black)
                {
                    /*          parent(*)                       parent(DB)
                    *       x(DB)      brother(B)       ->    x          brother(R) 
                    *                 bl(B)    br(B)                    bl(B)   br(B)
                    */
                    brother->color = Color::Red;
                    x = x->parent;
                    continue;
                }
                /*          parent(*)                          parent(*)
                *          x(DB)   brother(B)      ->       x(DB)    bl(B)
                *                bl(R)   br(B)                         brother(R)
                *                                                             br(B)
                */
                if (brother->left->color == Color::Red && brother->right->color == Color::Black)
                {
                    //有一个红色，变色左旋直接用
                    brother->color = Color::Red;
                    brother->left->color = Color::Black;
                    rotateRight(brother);
                    brother = x->parent->right;
                }
                /*
                *            parent(o)                              brother(o)   
                *          x(DB)   brother(B)           ->    parent(B）    br(B)
                *                 bl   br(R)                  x     bl 
                *               
                *            
                */
                brother->color = x->parent->color;
                x->parent->color = Color::Black;
                brother->right->color = Color::Black;
                rotateLeft(x->parent);
                x = root_;
            }
            else
            {
                //反过来
                NodeBase* brother = x->parent->left;
                if (brother->color == Color::Red)
                {
                    brother->color = Color::Black;
                    x->parent->color = Color::Red;
                    rotateRight(x->parent);
                    brother = x->parent->left;
                }
                if (brother->left->color == Color::Black && brother->right->color == Color::Black)
                {
                    brother->color = Color::Red;
                    x = x->parent;
                    continue;
                }
                if (brother->left->color == Color::Black && brother->right->color == Color::Red)
                {
                    brother->color = Color::Red;
                    brother->right->color = Color::Black;
                    rotateRight(brother);
                    brother = x->parent->left;
                }
                brother->color = x->parent->color;
                x->parent->color = Color::Black;
                brother->left->color = Color::Black;
                rotateRight(x->parent);
                x = root_;
            }
        }
    }

    template<class F>
    void traverse(const NodeBase* node, F& fn) const
    {
        if (node == &nil_)
        {
            return;
        }
        traverse(node->left, fn);
        fn(asNode(node)->data);
        traverse(node->right, fn);
    }
    //返回子树黑高，-1表示失败
    int checkNode(const NodeBase* node, const NodeBase* parent, const Key* lower,
        const Key* upper, size_t& count) const
    {
        if (node == &nil_)
        {
            return 1;
        }
        if (node->parent != parent)
        {
            return -1;
        }
        const Key& key = keyOf(node);
        if (lower && !comp_(*lower, key))
        {
            return -1;
        }
        if (upper && !comp_(key, *upper))
        {
            return -1;
        }
        if (node->color == Color::Red && (node->left->color == Color::Red ||
            node->right->color == Color::Red))
        {
            return -1;
        }
        ++count;
        const int left = checkNode(node->left, node,  lower, std::addressof(key), count);
        const int right = checkNode(node->right, node, std::addressof(key), upper, count);
        if (left < 0 || right < 0 || left != right)
        {
            return -1;
        }

        return left + (node->color == Color::Black ? 1 : 0);
    }

public:
    explicit RBTree(Compare comp = Compare{}) : comp_(std::move(comp)), root_(&nil_)
    {
        resetNil();
    }

    ~RBTree()
    {
        clear();
    }

    void clear() noexcept
    {
        destroy(root_);
        root_ = &nil_;
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
        return node == &nil_ ? nullptr : std::addressof(asNode(node)->data.second);
    }

    bool contains(const Key &key) const
    {
        return findNode(key) != &nil_;
    }

    template <class K, class V>
    std::pair<Value *, bool> insert(K &&key, V &&value)
    {
        NodeBase *cur = root_;
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
				return { std::addressof(asNode(cur)->data.second),false };
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
            root_ = z;
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
        NodeBase* removedNode = z;
        Color removedColor = z->color;
        NodeBase* x = &nil_;

        if (z->left == &nil_)
        {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == &nil_)
        {
            x = z->left;
            transplant(z, z->left);
        }
        else
        {
            //取中序后继
            removedNode = minimum(z->right);
            removedColor = removedNode->color;
            x = removedNode->right;
            if (removedNode->parent == z)
            {
                x->parent = removedNode;//if x == &nil_
            }
            else
            {
                //接z右节点
                transplant(removedNode, removedNode->right);
                removedNode->right = z->right;
                removedNode->right->parent = removedNode;
            }
            //接z父节点
            transplant(z, removedNode);
            //接z左节点
            removedNode->left = z->left;
            removedNode->left->parent = removedNode;
            removedNode->color = z->color;
        }

        delete asNode(z);
        --size_;

        if (removedColor == Color::Black)
        {
            fixErase(x);
        }

        resetNil();//nil_.parent
        return true;

        return false;
    }

    template<class F>
    void forEach(F&& fn) const
    {
        traverse(root_, fn);
    }

    bool validate() const
    {
        if (nil_.color != Color::Black ||
            nil_.parent != &nil_ ||
            nil_.left != &nil_ ||
            nil_.right != &nil_)
        {
            return false;
        }
        if (root_ == &nil_)
        {
            return size_ == 0;
        }

        if (root_->parent != &nil_ || root_->color != Color::Black)
        {
            return false;
        }

        size_type count = 0;
        const int blackHeight = checkNode(root_, &nil_, nullptr, nullptr, count);
        return blackHeight >= 0 && count == size_;
    }

};
