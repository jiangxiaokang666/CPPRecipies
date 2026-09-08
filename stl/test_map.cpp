#include <map>

#include "../common/common_helper.h"

int main()
{
    /*
    AVL 树：严格平衡，查找快，插入/删除旋转多，适合读多写少。
    红黑树：弱平衡，插入/删除旋转少，性能稳定，适合读写频繁的场景。
    */
    SUBTITLE("sizeof");
    {
        std::cout<< "size of map "<< sizeof(std::map<int,int>) << "\n";
    }

    return 0;
}