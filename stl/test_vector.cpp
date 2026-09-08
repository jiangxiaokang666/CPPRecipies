#include <vector>
#include <iostream>

void PrintSubtitle(std::string_view title)
{
    constexpr std::size_t width = 20;
    const std::size_t contentWidth = std::min(title.size(), width);
    const std::size_t leftPadding = (width - contentWidth) / 2;
    const std::size_t rightPadding = width - contentWidth - leftPadding;
    std::cout << "======"
              << std::string(leftPadding, ' ')
              << title.substr(0, contentWidth)
              << std::string(rightPadding, ' ')
              << "======\n";
}

#define SUBTITLE(str) PrintSubtitle(str)

int main()
{
    SUBTITLE("sizeof");
    {
        /*
             vector 对象本身并不保存元素，元素在堆上
             栈上数据：
                T* begin
                T* end
                T* capacityEnd
         */
        std::vector<int> a;
        std::cout << "size of vector : " << sizeof(a) << "\n"; // 24:3x8 (begin end capacity)
        /*
            vector 对cache非常友好，堆上的连续空间
        */
        std::vector<int> v = {10, 20, 30, 40};
        std::cout << v.data() << "\n";
        for (int i = 0; i < v.size(); ++i)
        {
            std::cout << &v[i] << "\n";
        }
    }

    /*
        size 和 capacity
    */
    SUBTITLE("size and capacity");
    {
        std::vector<int> v;
        std::cout << v.size() << "\n";
        std::cout << v.capacity() << "\n";
        std::cout << "data=" << static_cast<const void *>(v.data()) << "\n";
        auto foo = [&v](int a)
        {
            std::cout << "---" << "\n";
            std::cout << "push back " << a << "\n";
            v.push_back(a);
            std::cout << v.size() << "\n";
            std::cout << v.capacity() << "\n";
            std::cout << "data=" << static_cast<const void *>(v.data()) << "\n";
        };
        for (int i = 0; i < 10; ++i)
        {
            foo(i * 10);
        }
    }
    SUBTITLE("reserve and resize");
    {
        /*
            reserve 不会构造对象
                - 减少 malloc / new
                - 减少 memcpy / move
                - 减少 destructor
                - 减少 iterator invalidation
                - 性能更加稳定
            resize 会构造对象
            扩容： allocate memory -> placement new + std::move or memcpy ->  old.~T() -> deallocate memory -> update vector member pointer
         */
        std::vector<int> v;
        v.reserve(1000);
        std::cout << v.size() << " " << v.capacity() << "\n";
        struct Player
        {
            Player()
            {
                std::cout << "Construct Player\n";
            }
        };
        std::vector<Player> vec_player;
        vec_player.reserve(10);
        std::cout << vec_player.size() << " " << vec_player.capacity() << "\n";
        std::vector<Player> vec_player1;
        vec_player1.resize(10);
        std::cout << vec_player1.size() << " " << vec_player1.capacity() << "\n";
    }
    SUBTITLE("emplace_back and push_back");
    /*
        c++ 有了移动语义后，emplace_back和push_back性能差异不一定很明显
    */
    SUBTITLE("clear");
    {
        /*
            clear 只析构元素，但是不释放底层buffer
        */
        std::vector<int> v;
        v.reserve(1000);
        for (int i = 0; i < 1000; ++i)
        {
            v.push_back(i);
        }
        v.clear();
        std::cout << "size " << v.size() << "\n";
        std::cout << "capacity " << v.capacity() << "\n";
    }
    SUBTITLE("shrink_to_fit");
    {
        /*
        请求vector尽量释放多余的capacity，让capacity靠近size。
        是一个非强制请求。
        实际开发里，不要在持有 vector 元素指针/引用的时候随便 shrink，会触发realloc
        生命周期进入稳定低水位，而且确定短期不会重新增长时，再考虑 shrink。
        */
        std::vector<int> v;
        v.reserve(1000);
        for(int i = 0; i < 100; ++i)
        {
            v.push_back(100);
        }
        std::cout<< "size " << v.size() <<"\n";
        std::cout<< "capacity " << v.capacity() <<"\n";
        v.shrink_to_fit();
        std::cout<<"size after shrink "<< v.size() <<"\n";
        std::cout<<"capacity after shrink "<<v.capacity() <<"\n";
    }

    return 0;
}