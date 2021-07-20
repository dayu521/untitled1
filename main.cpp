#include<iostream>
//#include<future>
//#include<thread>
//#include <filesystem>
//#include"BlockingCollection.h"
#include<string>
//#include<ranges>
//#include<loki/SmallObj.h>
#include"Singleton.h"
//#include<coroutine>

template<typename T>
struct Atraits
{
    using AccT=int;
};

template<typename T>
struct Atraits2
{
    using AccT=char;
};

template<typename T ,typename AT=Atraits<T>>
struct Accum
{
    static typename AT::Acct accum(const T * beg,const T * end)
    {
        return typename AT::Acct{};
    }
};

template<typename T>
inline typename Atraits<T>::AccT accum(const T * beg,const T * end)
{
    std::cout<<"first"<<std::endl;
    return {};
}

template<typename T,typename At>
inline  typename  At::AccT accum(const T * beg,const T * end)
{
     std::cout<<"second"<<std::endl;
}

int a[]={};

auto m=accum(a,a);

struct OL
{
    int a=5;
    ~OL()
    {
        std::cout<<"fuck"<<std::endl;
    }
};
LOKI_SINGLETON_INSTANCE_DEFINITION(SingletonHolder<OL>)

auto sing=Loki::Singleton<OL>().Instance();

int main()
{

    std::vector<int> ints{0,1,2,3,4,5};
    auto even = [](int i){ return 0 == i % 2; };
    auto square = [](int i) { return i * i; };

//    for (int i : ints | std::views::filter(even) | std::views::transform(square)) {
//        std::cout << i << ' ';
//    }

    int i=10;
    auto lamn=[i=i]()mutable{
        return i++;
    };

    return 0;
}
