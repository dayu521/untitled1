#include<iostream>
//#include<future>
//#include<thread>
//#include <filesystem>
//#include"BlockingCollection.h"
#include<string>
//#include<ranges>
//#include<loki/SmallObj.h>
//#include"Singleton.h"

#include <stdexcept>
#include <thread>
#ifdef COROUTINENEEDWRAP
#include "coroutine_wrap.h"
#else
#include<coroutine>
#endif

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

//auto m=accum(a,a);

struct OL
{
    int a=5;
    ~OL()
    {
        std::cout<<"fuck"<<std::endl;
    }
};
//LOKI_SINGLETON_INSTANCE_DEFINITION(SingletonHolder<OL>)

//auto sing=Loki::Singleton<OL>().Instance();

auto switch_to_new_thread(std::jthread& out) {
  struct awaitable {
    std::jthread* p_out;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> h) {
      std::jthread& out = *p_out;
      if (out.joinable())
        throw std::runtime_error("Output jthread parameter not empty");
//      out = std::jthread([h] { h.resume(); });
      // 潜在的未定义行为：访问潜在被销毁的 *this
      // std::cout << "New thread ID: " << p_out->get_id() << "\n";
      std::cout << "New thread ID: " << out.get_id() << "\n"; // 这 OK
    }
    void await_resume() {}
  };
  return awaitable{&out};
}

struct task {
    struct promise_type {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

task resuming_on_new_thread(std::jthread& out) {
  std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << "\n";
  co_await switch_to_new_thread(out);
  // 等待器销毁于此
  std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << "\n";
  auto i=4;
  i=5*i;
}

int main()
{

//    std::vector<int> ints{0,1,2,3,4,5};
//    auto even = [](int i){ return 0 == i % 2; };
//    auto square = [](int i) { return i * i; };

////    for (int i : ints | std::views::filter(even) | std::views::transform(square)) {
////        std::cout << i << ' ';
////    }

//    int i=10;
//    auto lamn=[i=i]()mutable{
//        return i++;
//    };
    std::jthread out;
    resuming_on_new_thread(out);
    return 0;
}
