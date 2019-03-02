#include "../src/thread_pool.hpp"
#include "../src/utility.hpp"
#include <iostream>

class A {
public:
    int sum(int from, int to) {
        int res = 0;
        for(auto i = from; i != to; ++i) {
            res += i;
        }
        return res;
    }
};

int main() {
    httpc::ThreadPool thread_pool(httpc::GetCPUCoreNumber());
    std::cout << httpc::GetCPUCoreNumber() << std::endl;
    auto aptr = std::make_shared<A>();
    auto f1 = thread_pool.AddTask(httpc::BindMember(aptr, &A::sum), 1, 100);
    auto f2 = thread_pool.AddTask(httpc::BindMember(aptr, &A::sum), 1, 1000000);
    auto f3 = thread_pool.AddTask(httpc::BindMember(aptr, &A::sum), 1, 10000000);

    // std::function<int(int,int)> f(&sum<int>);
    // auto f1 = thread_pool.AddTask(sum<int>, 1, 1000000);
    // auto f2 = thread_pool.AddTask(sum<int>, 1, 10000000);
    // auto f3 = thread_pool.AddTask(sum<int>, 1, 100000000);
    f1.wait();
    f2.wait();
    f3.wait();

    std::cout << f1.get() << " " << f2.get() << " " << f3.get() << std::endl;
}