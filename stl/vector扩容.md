```c++
if (size == capacity)
{
    // 1. 算新容量
    size_t newCapacity = grow(capacity);

    // 2. 申请 raw memory
    T* newMemory = allocate(newCapacity);

    // 3. move/copy 旧元素
    for (size_t i = 0; i < size; ++i)
    {
        new (newMemory + i)
            T(std::move(oldMemory[i]));
    }

    // 4. 构造新增元素
    new (newMemory + size) T(value);

    // 5. 析构旧对象
    for (size_t i = 0; i < size; ++i)
    {
        oldMemory[i].~T();
    }

    // 6. 释放旧内存
    deallocate(oldMemory);

    // 7. 更新内部状态
    oldMemory = newMemory;
    ++size;
    capacity = newCapacity;
}
```