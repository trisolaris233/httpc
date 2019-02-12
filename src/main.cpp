#include <tuple>
#include <array>
#include <utility>
#include <typeinfo>
#include <iostream>


template <typename T>
void fun(T& i) {

}

template <typename T>
void func(T&& ri) {
    std::cout << typeid(T).name() << " " << typeid(ri).name() << std::endl;
    ri = 8;
}

template<typename T>
void f(const T&& param) {
    auto&& ref = const_cast<typename std::remove_const<T>::type&>(param);
    ref = 15;
}

template <typename T>
void f2(T& param) {
    typename std::remove_reference<T>::type a = 50;
    T b = a;
    b = 25;
    // if T is reduced as reference it will show 25.
    std::cout << a << std::endl;
}

template <typename PT>
void f3(PT param) {
    PT ptr = nullptr;
    ptr = "234324";
    ptr = "dfsfsdf";
}

template <typename T, std::size_t N>
std::size_t f4(const T (&) [N]) {
    return N;
}


struct log {
    void begin() {
        std::cout << "log start" << std::endl;
    }
    void after() {
        std::cout << "log end" << std::endl;
    }
};

struct check {
    void begin() {
        std::cout << "check start" << std::endl;
    }  
    void after() {
        std::cout << "check end" << std::endl;
    }
};

struct lg {
    void begin() {
        std::cout << "lg start" << std::endl;
    }
    void after() {
        std::cout << "lg end" << std::endl;
    }
};

template <typename TupleType, std::size_t N>
struct AspectCaller {
    static void invoke(const TupleType& tuple) {
        AspectCaller<
            typename std::remove_reference<typename std::remove_cv<decltype(tuple)>::type>::type,
            N - 1
        >::invoke(tuple);
        auto x = std::get<N - 1>(tuple);
        x.begin();
    }
};

template <typename TupleType>
struct AspectCaller<TupleType, 1> {
    static void invoke(const TupleType& tuple) {
        auto x = std::get<0>(tuple);
        x.begin();
    }
};

template <typename TupleType>
void CallAspect(TupleType tuple, bool is_begin) {
    AspectCaller<TupleType, 
        std::tuple_size<
            typename std::remove_reference<
                typename std::remove_cv<TupleType>::type
            >::type
        >::value
    >::invoke(tuple);
}

class A {
public:
    template <typename... Args>
    void bussiness(Args&&... ap) {
        std::tuple<Args&&...>t(std::forward<Args>(ap)...); 
        CallAspect(t, true);
        //CallAspect(t, false);
    }
};

void testAspect() {
    std::tuple<log, check, lg> t = {log{}, check{}, lg{}};
    CallAspect(t,true);
    CallAspect(std::make_tuple(check{}, lg{}, log{}, lg{}, check{}),true);
}


int main(int argc, char* argv[]) {
    testAspect();
    return 0;
}