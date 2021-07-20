////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_SINGLETON_INC_
#define LOKI_SINGLETON_INC_

// $Id: Singleton.h 834 2007-08-02 19:36:10Z syntheticpp $


//#include "LokiExport.h"
//#include "Threads.h"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <new>
#include <vector>
#include <list>
#include <memory>

#ifdef _MSC_VER
#define LOKI_C_CALLING_CONVENTION_QUALIFIER __cdecl 
#else
#define LOKI_C_CALLING_CONVENTION_QUALIFIER 
#endif

///  \defgroup  SingletonGroup Singleton
///  \defgroup  CreationGroup Creation policies
///  \ingroup   SingletonGroup
///  \defgroup  LifetimeGroup Lifetime policies
///  \ingroup   SingletonGroup
///  The lifetimes of the singleton.
///  \par Special lifetime for SmallObjects
///  When the holded object is a Small(Value)Object or the holded object 
///  uses objects which are or inherit from Small(Value)Object
///  then you can't use the default lifetime: you must use the lifetime
///  \code Loki::LongevityLifetime::DieAsSmallObjectChild \endcode
///  Be aware of this when you use Loki::Factory, Loki::Functor, or Loki::Function.



namespace Loki
{
    typedef void (LOKI_C_CALLING_CONVENTION_QUALIFIER *atexit_pfn_t)();

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct CreateUsingNew 
    ///
    ///  \ingroup CreationGroup
    ///  Implementation of the CreationPolicy used by SingletonHolder
    ///  Creates objects using a straight call to the new operator 
    ////////////////////////////////////////////////////////////////////////////////
    template <class T> struct CreateUsingNew
    {
        static T* Create()
        { return new T; }
        
        static void Destroy(T* p)
        { delete p; }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct CreateUsing
    ///
    ///  \ingroup CreationGroup
    ///  Implementation of the CreationPolicy used by SingletonHolder
    ///  Creates objects using a custom allocater.
    ///  Usage: e.g. CreateUsing<std::allocator>::Allocator
    ////////////////////////////////////////////////////////////////////////////////
    template<template<class> class Alloc>
    struct CreateUsing
    {
        template <class T>
        struct Allocator
        {
            static Alloc<T> allocator;

            static T* Create()
            {
                return new (allocator.allocate(1)) T;
            }

            static void Destroy(T* p)
            {
                //allocator.destroy(p);
                p->~T();
                allocator.deallocate(p,1);
            }
        };
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct CreateUsingMalloc
    ///
    ///  \ingroup CreationGroup
    ///  Implementation of the CreationPolicy used by SingletonHolder
    ///  Creates objects using a call to std::malloc, followed by a call to the 
    ///  placement new operator
    ////////////////////////////////////////////////////////////////////////////////
    template <class T> struct CreateUsingMalloc
    {
        static T* Create()
        {
            void* p = std::malloc(sizeof(T));
            if (!p) return 0;
            return new(p) T;
        }
        
        static void Destroy(T* p)
        {
            p->~T();
            std::free(p);
        }
    };
    

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct CreateStatic
    ///
    ///  \ingroup CreationGroup
    ///  Implementation of the CreationPolicy used by SingletonHolder
    ///  Creates an object in static memory
    ///  Implementation is slightly nonportable because it uses the MaxAlign trick 
    ///  (an union of all types to ensure proper memory alignment). This trick is 
    ///  nonportable in theory but highly portable in practice.
    ////////////////////////////////////////////////////////////////////////////////
    template <class T> struct CreateStatic
    {
        
#ifdef _MSC_VER
#pragma warning( push ) 
#pragma warning( disable : 4121 )
// alignment of a member was sensitive to packing
#endif // _MSC_VER

        union MaxAlign
        {
            char t_[sizeof(T)];
            short int shortInt_;
            int int_;
            long int longInt_;
            float float_;
            double double_;
            long double longDouble_;
            struct Test;
            int Test::* pMember_;
            int (Test::*pMemberFn_)(int);
        };
        
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER
        
        static T* Create()
        {
            static MaxAlign staticMemory_;
            return new(&staticMemory_) T;
        }
        
        static void Destroy(T* p)
        {
            p->~T();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct DefaultLifetime
    ///
    ///  \ingroup LifetimeGroup
    ///  Implementation of the LifetimePolicy used by SingletonHolder
    ///  Schedules an object's destruction as per C++ rules
    ///  Forwards to std::atexit
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    struct DefaultLifetime
    {
        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        { std::atexit(pFun); }
        
        static void OnDeadReference()
        { throw std::logic_error("Dead Reference Detected"); }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct  PhoenixSingleton
    ///
    ///  \ingroup LifetimeGroup
    ///  Implementation of the LifetimePolicy used by SingletonHolder
    ///  Schedules an object's destruction as per C++ rules, and it allows object 
    ///  recreation by not throwing an exception from OnDeadReference
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    class PhoenixSingleton
    {
    public:
        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        {
#ifndef ATEXIT_FIXED
            if (destroyedOnce_)
#endif
                std::atexit(pFun);
        }
        
        static void OnDeadReference()
        {
#ifndef ATEXIT_FIXED
            destroyedOnce_ = true;
#endif
        }
        
    private:
#ifndef ATEXIT_FIXED
        static bool destroyedOnce_;
#endif
    };
    
#ifndef ATEXIT_FIXED
    template <class T> bool PhoenixSingleton<T>::destroyedOnce_ = false;
#endif

    ////////////////////////////////////////////////////////////////////////////////
    // Copyright (c) 2004 by Curtis Krauskopf - curtis@decompile.com
    ///
    ///  \struct  DeletableSingleton
    ///
    ///  \ingroup LifetimeGroup
    ///
    ///  A DeletableSingleton allows the instantiated singleton to be 
    ///  destroyed at any time. The singleton can be reinstantiated at 
    ///  any time, even during program termination.
    ///  If the singleton exists when the program terminates, it will 
    ///  be automatically deleted.
    ///
    ///  \par Usage:  
    ///  The singleton can be deleted manually:
    ///
    ///  DeletableSingleton<MyClass>::GracefulDelete();
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    class DeletableSingleton
    {
    public:

        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        {
            static bool firstPass = true;
            isDead = false;
            deleter = pFun;
            if (firstPass || needCallback)
            {
                std::atexit(atexitCallback);
                firstPass = false;
                needCallback = false;
            }
        }
    
        static void OnDeadReference()
        { 
        }
        ///  delete singleton object manually
        static void GracefulDelete()
        {
            if (isDead)
                return;
            isDead = true;
            deleter();
        }
    
    protected:
        static atexit_pfn_t deleter;
        static bool isDead;
        static bool needCallback;
        
        static void atexitCallback()
        {
#ifdef ATEXIT_FIXED
            needCallback = true;
#else
            needCallback = false;
#endif
            GracefulDelete();
        }
    };
    
    template <class T>
    atexit_pfn_t DeletableSingleton<T>::deleter = 0;
    
    template <class T>
    bool DeletableSingleton<T>::isDead = true;
    
    template <class T>
    bool DeletableSingleton<T>::needCallback = true;

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct NoDestroy
    ///
    ///  \ingroup LifetimeGroup
    ///  Implementation of the LifetimePolicy used by SingletonHolder
    ///  Never destroys the object
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    struct NoDestroy
    {
        static void ScheduleDestruction(T*, atexit_pfn_t)
        {}
        
        static void OnDeadReference()
        {}
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    ///  \class  SingletonHolder
    ///
    ///  \ingroup SingletonGroup
    ///
    ///  Provides Singleton amenities for a type T
    ///  To protect that type from spurious instantiations, 
    ///  you have to protect it yourself.
    ///  
    ///  \param CreationPolicy Creation policy, default: CreateUsingNew
    ///  \param LifetimePolicy Lifetime policy, default: DefaultLifetime,
    ///  \param ThreadingModel Threading policy, 
    ///                         default: LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL
    ////////////////////////////////////////////////////////////////////////////////
    template
    <
        typename T,
        template <class> class CreationPolicy = CreateStatic,//CreateUsingNew,
        template <class> class LifetimePolicy = PhoenixSingleton//DefaultLifetime
    >
    class SingletonHolder
    {
    public:

        ///  Type of the singleton object
        typedef T ObjectType;

        ///  Returns a reference to singleton object
        static T& Instance();
        
    private:
        // Helpers
        static void MakeInstance();
        static void LOKI_C_CALLING_CONVENTION_QUALIFIER DestroySingleton();
        
        // Protection
        SingletonHolder();
        
        // Data
        typedef T * PtrInstanceType;
        static PtrInstanceType pInstance_;
        static bool destroyed_;
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder's data
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class C,
        template <class> class L
    >
    typename SingletonHolder<T, C, L>::PtrInstanceType
        SingletonHolder<T, C, L>::pInstance_ = 0;

    template
    <
        class T,
        template <class> class C,
        template <class> class L
    >
    bool SingletonHolder<T, C, L>::destroyed_ = false;

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::Instance
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy
    >
    inline T& SingletonHolder<T, CreationPolicy, LifetimePolicy>::Instance()
    {
        if (!pInstance_)
        {
            MakeInstance();
        }
        return *pInstance_;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::MakeInstance (helper for Instance)
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy
    >
    void SingletonHolder<T, CreationPolicy, LifetimePolicy>::MakeInstance()
    {
        if (!pInstance_)
        {
            if (destroyed_)
            {
                destroyed_ = false;
                LifetimePolicy<T>::OnDeadReference();
            }
            pInstance_ = CreationPolicy<T>::Create();
            LifetimePolicy<T>::ScheduleDestruction(pInstance_, 
                &DestroySingleton);
        }
    }

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class L
    >
    void LOKI_C_CALLING_CONVENTION_QUALIFIER 
    SingletonHolder<T, CreationPolicy, L>::DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy<T>::Destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }


    ////////////////////////////////////////////////////////////////////////////////
    ///  \class  Singleton
    ///
    ///  \ingroup SingletonGroup
    ///
    ///  Convenience template to implement a getter function for a singleton object.
    ///  Often needed in a shared library which hosts singletons.
    ///  
    ///  \par Usage
    ///
    ///  see test/SingletonDll
    ///
    ////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_SINGLETON_EXPORT
#define LOKI_SINGLETON_EXPORT
#endif

    template<class T>
    class LOKI_SINGLETON_EXPORT Singleton
    {
    public:
        static T& Instance();
    };

} // namespace Loki


/// \def LOKI_SINGLETON_INSTANCE_DEFINITION(SHOLDER)
/// Convenience macro for the definition of the static Instance member function
/// Put this macro called with a SingletonHolder typedef into your cpp file.

#define LOKI_SINGLETON_INSTANCE_DEFINITION(SHOLDER)                     \
namespace Loki                                                          \
{                                                                        \
    template<>                                                          \
    SHOLDER::ObjectType&  Singleton<SHOLDER::ObjectType>::Instance()    \
    {                                                                   \
        return SHOLDER::Instance();                                     \
    }                                                                    \
}


#endif // end file guardian

