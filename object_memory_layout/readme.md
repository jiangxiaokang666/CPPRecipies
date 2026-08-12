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