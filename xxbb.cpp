//#include "xxbb.h"

//#include "json_struct.h"
#include<cstddef>
#include<tuple>
#include<string>
#include"BlockingCollection.h"
#include<iostream>

#define FOO(x, y) x y
#define BAR() x, y

/*
  The following expression results in an error
  This is because FOO is expanded first
  As a result, Since Foo only has 1 parameter Bar
  It complains we didnt provide the correct arguments
*/
//FOO(BAR())

template <class T>
struct Base
{
    void interface()
    {
        // ...
        static_cast<T*>(this)->implementation();
        // ...
    }

    static void static_func()
    {
        // ...
        T::static_sub_func();
        // ...
        typename T::Ks ml;
    }
};

struct Derived : Base<Derived>
{
    void implementation();

    using Ks =int;

//    static void static_sub_func();
};

Base<Derived> * p=new Derived;

template <typename T, typename U, typename NAMETUPLE>
struct MI
{
  NAMETUPLE names;
  T U::*member;
  typedef T type;
};

template <size_t SIZE>
struct StringLiteral
{
  const char *data;
  static constexpr const size_t size = SIZE;
};

template <size_t SIZE>
constexpr StringLiteral<SIZE - 1> makeStringLiteral(const char (&literal)[SIZE])
{
  return {literal};
}

template <typename T, typename U, size_t NAME_SIZE, typename... Aliases>
constexpr auto makeMemberInfo(const char (&name)[NAME_SIZE], T U::*member, Aliases &... aliases)
  -> MI<T, U, decltype(std::make_tuple(makeStringLiteral(name), makeStringLiteral(aliases)...))>
{
  return {std::make_tuple(makeStringLiteral(name), makeStringLiteral(aliases)...), member};
}


#define JS_OBJECT_INTERNAL_IMPL(super_list, member_list)                                                               \
  template <typename JS_OBJECT_T>                                                                                      \
  struct JsonStructBase                                                                                                \
  {                                                                                                                    \
    using TT = decltype(member_list);                                                                                  \
    static inline constexpr const TT js_static_meta_data_info()                                                        \
    {                                                                                                                  \
      return member_list;                                                                                              \
    }                                                                                                                  \
    static inline constexpr const decltype(super_list) js_static_meta_super_info()                                     \
    {                                                                                                                  \
      return super_list;         /*JS::makeTuple(
                                        MI=JS::makeMemberInfo(#member1, &JS_OBJECT_T::member1),
                                        MI=JS::makeMemberInfo(#member2, &JS_OBJECT_T::member2))*/                                                                                      \
    }                                                                                                                  \
  }
#define JS_MEMBER(member) makeMemberInfo(#member, &JS_OBJECT_T::member)
#define JS_OBJECT(...) JS_OBJECT_INTERNAL_IMPL(std::make_tuple(), std::make_tuple(__VA_ARGS__))

//struct JsonObject
//{
//    int One;
//    std::string Two;
//    double Three;

//        JS_OBJECT(JS_MEMBER(One)
//                , JS_MEMBER(Two)
//                , JS_MEMBER(Three));
//};
struct JsonObject
{
    int One;
    std::string Two;
    double Three;

    template <typename JS_OBJECT_T>
    struct JsonStructBase
    {
        using TT = decltype(std::make_tuple(makeMemberInfo("One", &JS_OBJECT_T::One) , makeMemberInfo("Two", &JS_OBJECT_T::Two) , makeMemberInfo("Three", &JS_OBJECT_T::Three)));
        static inline constexpr const TT js_static_meta_data_info()
        {
            return std::make_tuple(makeMemberInfo("One", &JS_OBJECT_T::One) , makeMemberInfo("Two", &JS_OBJECT_T::Two) , makeMemberInfo("Three", &JS_OBJECT_T::Three));
        }
        static inline constexpr const decltype(std::make_tuple()) js_static_meta_super_info()
        {
            return std::make_tuple();
        }
    };
};

void text()
{
//    JS::ParseContext context(R"(
//    {
//    "One" : 1,
//    "Two" : "two",
//    "Three" : 3.333
//    }
//)");
//    JsonObject obj;
//    context.parseTo(obj);
}

using namespace code_machina;
void test_block()
{
    // A bounded collection. It can hold no more
    // than 100 items at once.
    BlockingCollection<std::string *> collection(100);

    // a simple blocking consumer
    std::thread consumer_thread([&collection]() {

      while (!collection.is_completed())
      {
          std::string * data=new std::string;

          // take will block if there is no data to be taken
          auto status = collection.take(data);

          if(status == BlockingCollectionStatus::Ok)
          {
              std::cout<<*data<<std::endl;
          }

          // Status can also return BlockingCollectionStatus::Completed meaning take was called
          // on a completed collection. Some other thread can call complete_adding after we pass
          // the is_completed check but before we call take. In this example, we can ignore that
          // status since the loop will break on the next iteration.
      }
    });

    // a simple blocking producer
    int a=5;
    std::thread producer_thread([&a,&collection]() {

      while (a>0)
      {
          std::string * data =new std::string("dasd");

          // blocks if collection.size() == collection.bounded_capacity()
          collection.add(data);
          a--;
      }

      // let consumer know we are done
      collection.complete_adding();
    });

    consumer_thread.join();
    producer_thread.join();
}
