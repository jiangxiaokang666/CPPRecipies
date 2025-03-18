#include "../thread/ThreadPoolWithPackagedTask.hpp"

#include <stdio.h>

void printSome(int x) {
    printf("Print some %d \n",x);
}

int getResult(int a, int b){
    return a*b;
}

int main() {
    ThreadPoolWithPackgedTask pool;
    pool.start(4);

    auto result1 = pool.run(printSome, 1);
    auto result2 = pool.run([](int x) { printf("Lambda %d\n", x); }, 10);
    auto result3 = pool.run(getResult,2,3);
    // 等待所有任务完成
    result1.get();
    result2.get();
    printf("test get result %d\n",result3.get());
    return 0;
}