#include "pch.h"

class A
{
    int a = 0;
};

class B
{
    int b = 0;
};

class C : public A, public B
{
    int c = 0;
};

class A0
{
    int a = 0;
    virtual void funcA() {}
};

class B0
{
    int b = 0;
    virtual void funcB() {}
};

class C0 : public A0, public B0
{
    int c = 0;
};

class A1
{
    int a = 0;
};

class B1 : public A1
{
    int b = 0;
};

class C1 : public A1
{
    int c = 0;
};

class D1 : public B1, public C1
{
    int d = 0;
};

class A2
{
    int a = 0;
};

class B2 : virtual public A2
{
    int b = 0;
};

class C2 : public A2
{
    int c = 0;
};


class A3
{
    int a = 0;
};

class B3 : virtual public A3
{
    int b = 0;
};

class C3 : virtual public A3
{
    int c = 0;
};

class D3 : public B3, public C3
{
    int d = 0;
};

int main()
{
    // x64 platform msvc
    std::cout << "size of C " << sizeof(C) << std::endl;
    /*
        A::a  -> 4 -> align 8
        A::vptr -> 8
        B::b -> 4 -> align 8
        B::vptr -> 8
        C::c -> 4 -> align 8
     */
    std::cout << "size of C0 " << sizeof(C0) << std::endl;
    /*
        A1::a -> 4
        B1::b -> 4
        A1::a -> 4
        C1::c -> 4
        D1::d -> 4
    */
    std::cout << "size of D1 " << sizeof(D1) << std::endl;
    /*
        B2::vbptr -> 8
        B2::b    -> 4 align -> 8
        A2::a    -> 4 align -> 8
    */
    std::cout << "size of B2 " << sizeof(B2) << std::endl;
    /*
        A2::a   -> 4
        C2::c   -> 4
    */
    std::cout << "size of C2 " << sizeof(C2) << std::endl;
    /*
        B3::vbptr -> 8
        B3::b     -> 4 + padding 4
        C3::vbptr -> 8
        C3::c     -> 4 + padding 4
        D3::d     -> 4 + padding 4
        A3::a     -> 4 + padding 4
    */
    std::cout << "size of D3 " << sizeof(D3) << std::endl;

    // 验证虚继承下 D3 里两个 vbptr 与共享的 A3
    D3 d3;
    D3* pd = &d3;

    // D3* 转成两条路径上的子对象指针
    B3* pb = pd;  // 指向 D3 里的 B3 子对象
    C3* pc = pd;  // 指向 D3 里的 C3 子对象

    // D3* 分别经 B3、C3 路径转成 A3*，编译器会各自查 vbptr 表调整地址
    A3* pa_from_b = static_cast<A3*>(pb);
    A3* pa_from_c = static_cast<A3*>(pc);

    // 对象内存里前 8 字节就是 B3 子对象的 vbptr；C3 子对象起点处 8 字节是 C3 的 vbptr
    void* vbptr_b = *reinterpret_cast<void**>(pb);
    void* vbptr_c = *reinterpret_cast<void**>(pc);

    std::cout << std::endl;
    std::cout << "d3   addr = " << pd << std::endl;
    std::cout << "B3   addr = " << pb << std::endl;
    std::cout << "C3   addr = " << pc << std::endl;
    std::cout << "vbptr(B3) = " << vbptr_b << std::endl;
    std::cout << "vbptr(C3) = " << vbptr_c << std::endl;
    std::cout << "A3 via B3 = " << pa_from_b << std::endl;
    std::cout << "A3 via C3 = " << pa_from_c << std::endl;
    std::cout << "vbptr same?  " << (vbptr_b == vbptr_c) << std::endl;
    std::cout << "A3   same?  " << (pa_from_b == pa_from_c) << std::endl;

    return 0;
}