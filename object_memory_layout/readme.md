# 对象内存布局
- 对象本身
  - 只保存
    - 成员变量
    - 编译器添加的信息(如vptr)
  - 不保存
    - 函数代码
    - 类型信息
    - 静态变量
- 生命周期 != 对象存在
- vtable/vptr
  - call virtual function
    - load vptr
    - find table
    - jump function
  - 内存对齐，成员里最大的那个类型
    - 普通代码
      - 64位最大8字节
      - 32位最大4字节
    - c++最大对齐没有上限

      |类型|对齐|
      |---|---|
      |普通标量（int/double/指针）|8|
      |__int128（GCC/Clang，MSVC 不支持）|16|
      |long double（GCC/Clang 是 16 字节；MSVC 是 8）|8 或 16|
      |SIMD 类型 __m128（SSE）|16|
      |SIMD 类型 __m256（AVX）|32|
      |SIMD 类型 __m512（AVX-512）|64|
- 构造函数不能调用虚函数
  - 原因：子类还没构造完成
  - 析构同理
- sizeof 空类
  - 大小为 1
    - 为了保证不同对象地址不同
- placement new
  - 常用于对象池
  - UE5 FMemory
- 虚函数汇编
  ```
  mov rax,[rcx]
  call [rax]
  ```
  - 基本就是虚函数调用，rcx存的this
- 多继承
  - 一个C++对象并不一定只有一个 VPtr
  - 基类指针转换是有偏移运算的，编译器在背后做加法/减法。
    > 所以不同继承体系的指针不能互相 reinterpret_cast，必须用 static_cast/隐式转换，
    > 否则拿到错位的地址访问会崩
- 虚继承
  - MSVC里的 VBPtr / VBTable
    - Virtual Base Pointer
    - Virtual Base Table
  - 虚继承和虚函数是两件完全不同的事情
  - 虚继承：解决共享基类
  - 虚函数：解决多态
- C++指针类型转换不一定只是改变编译器眼里的类型，也可能改变指针的实际数值。
  - static_cast
    - 编译期确定 -> 简单offset调整
  - dynamic_cast
    - RTTI (run-time type information)
    > 读取类型信息 -> 判断动态类型 -> 分析继承关系 -> 处理多继承/虚继承 -> 计算目标子对象 -> 调整指针
- 不要在构造函数/析构函数中依赖派生类的虚函数行为
- 纯虚函数
  - 纯虚析构函数仍然必须提供定义
- 虚析构
- new 和 operator new
  - new-expression
    - operator new -> raw memory 分配内存
    - construct -> object 构造对象
  - 同理 delete 和 operator delete
- operator new 和 malloc
  
|            | `malloc`  | `operator new`       |
| ---------- | --------- | -------------------- |
| 来源         | C         | C++                  |
| 返回         | `void*`   | `void*`              |
| 构造对象       | ❌         | ❌                    |
| 失败行为       | `nullptr` | 通常抛 `std::bad_alloc` |
| 可重载/替换     | —         | ✅                    |
| 与 `new` 配合 | ❌直接关系     | ✅                    |
  > operator new 底层可能使用 malloc，但标准并没有规定必须这么实现。
  ```c++
  void* operator new(std::size_t size)
  {
      return MyAllocator(size);
  }
  ```
- placement new
  - 在指定地址构造
- 正常匹配
  - new        → delete
  - new[]      → delete[]
  - malloc     → free
  - operator new → operator delete
  
# 内存池
- 解决
  - allocator成本
  - 内存碎片
  - cache locality较差
  - 分配延迟不够稳定
- 核心
  - placement new
  - 连续访问 -> Cache Line -> CPU Cache
# UE
- FMemory
> FMemory 是 UE 提供的统一内存操作入口/封装层

| 操作                | 分配内存 | 调构造函数 |
| ----------------- | ---: | ----: |
| `malloc`          |    ✅ |     ❌ |
| `FMemory::Malloc` |    ✅ |     ❌ |
| `new T`           |    ✅ |     ✅ |
| Placement New     |    ❌ |     ✅ |

| 操作              | 调析构 | 释放内存 |
| --------------- | --: | ---: |
| `free`          |   ❌ |    ✅ |
| `FMemory::Free` |   ❌ |    ✅ |
| `delete`        |   ✅ |    ✅ |
| `p->~T()`       |   ✅ |    ❌ |

# cpu cache
- cache line
  - 通常一次性读取连续的64bytes
  - 空间局部性(Spatial Locality)
- 类型
  - cold miss
  - capacity miss
  - conflict miss
- ECS
  - Entity Component System
- AoS
  - Array of Structures
- SoA
  - Structure of Arrays
# Object Pool 的三个等级
- 减少分配
  - malloc/free -> object pool
- 连续内存
  - linked list -> array pool
- 数据导向
  - AoS -> SoA
  
  |      | AoS                 | SoA                 |
  | ---- | ------------------- | ------------------- |
  | 全称   | Array of Structures | Structure of Arrays |
  | 形式   | 对象数组                | 字段数组                |
  | 优点   | 易写、面向对象             | Cache友好、SIMD友好      |
  | 缺点   | 数据冗余加载              | 编程复杂                |
  | 适合   | 少量对象                | 大量同类计算              |
  | UE场景 | UObject             | Mass/Niagara        |
