#pragma once
#include <functional>

template<class Key, class Value, class Compare = std::less<Key>>
class RBTree
{
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;

private:
    enum class Color
    {
        Red,
        Black,
    };
    struct NodeBase
    {
        Color color = Color::Black;
        NodeBase* parent = nullptr;
        NodeBase* left = nullptr;
        NodeBase* right = nullptr;
    };
    struct Node : NodeBase
    {
        value_type data;
        template<class K, class V>
        Node(K&& key, V&& value)
            :data(
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<K>(key)),
                std::forward_as_tuple(std::forward<V>(value))    
            ){}
        
    };
};