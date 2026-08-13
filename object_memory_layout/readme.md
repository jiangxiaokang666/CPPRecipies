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
- vtable
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
