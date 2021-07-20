#include<thread>
#include<iostream>

#include"doctest/doctest.h"

using namespace std;

struct Mk
{
    Mk()
    {
        cout<<"fucks"<<endl;
    }
};

inline Mk & get_int()
{
    static thread_local Mk m{};
    cout<<(void *)&m<<endl;
    return m;
}

TEST_CASE("test thread_local")
{
    thread t([]{
        get_int();
    });
    get_int();
    t.join();
}
