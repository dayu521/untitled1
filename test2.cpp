#include"doctest/doctest.h"
#include<type_traits>

template< class... >
using void_t = void;

struct input_tag{};
struct slow_tag{};
struct fast_tag{};

template<typename T>
struct trait
{
    typedef typename T::tag_type tag_type;
};

template<>
struct trait<int>
{
    typedef  int tag_type;
};

template<typename T,typename tag=slow_tag>
using WithTagType=std::enable_if_t<std::is_same_v<typename trait<T>::tag_type,tag>,T>;

template<typename T>
inline void do_use(WithTagType<T> begin,WithTagType<T> end)
{
    MESSAGE("slow_tag do_use");
}

template<typename T>
inline void do_use(WithTagType<T,fast_tag> begin,WithTagType<T,fast_tag> end)
{
    MESSAGE("fast_tag do_use");
}

template<typename T>
void use_tag(T begin,T end)
{
    do_use<T>(begin,end);
}

struct FS
{
    typedef fast_tag tag_type;
};

struct MFS
{
    typedef slow_tag tag_type;
};

std::enable_if_t<std::is_same_v<typename trait<FS>::tag_type,fast_tag>,FS> l=FS{};

TEST_CASE("fucks ")
{
    use_tag(FS{},FS{});
//    do_use<FS>(FS{},FS{});
    use_tag(MFS{},MFS{});
}

template<bool B, class T = void>
struct enable_if {};

template<>
struct enable_if<true> { typedef int type; };
