#pragma once

#ifndef SPRUE_NO_ASSERT
    //TODO add other OS here
#endif

#ifdef SPRUE_ENGINE
    #ifdef SPRUE_EXPORT
        #define SPRUE __declspec(dllexport)
    #else
        #define SPRUE __declspec(dllimport)
    #endif
#else
    #define SPRUE
#endif

// PLATFORM SPECIFIC
// Add necessary platforms as required
#ifdef _MSC_VER
    #define SPRUE_FUNCTION __FUNCSIG__
    #define SPRUE_FILE __FILE__
    #define SPRUE_LINE __LINE__
#endif

// Define assertion macros so they may be disabled as desired
// Note the difference between ASSERT and VERIFY macros, verify functions the same as MFC in that the expression is considered important to run
#ifndef SPRUE_NO_ASSERT
    #if _DEBUG
        #if WIN32
            #define SPRUE_ASSERT(EXPR, MSG) _ASSERT_EXPR(EXPR, L ## MSG)

            #define SPRUE_VERIFY(EXPR, MSG) _ASSERT_EXPR(EXPR, MSG)
        #endif
    #else
        #define SPRUE_ASSERT(EXPR, MSG) 512
        #define SPRUE_VERIFY(EXPR, MSG) EXPR
    #endif
#else
    #define SPRUE_ASSERT(EXPR, MSG) (void)
    #define SPRUE_ASSERT_DEBUG(EXPR, MSG) (void)
    #define SPRUE_ASSERT_WARNING(EXPR, MSG) (void)
    #define SPRUE_ASSERT_ERROR(EXPR, MSG) (void)
    #define SPRUE_ASSERT_FATAL(EXPR, MSG) (void)

    #define SPRUE_VERIFY(EXPR, MSG) EXPR
    #define SPRUE_VERIFY_DEBUG(EXPR, MSG) EXPR
    #define SPRUE_VERIFY_WARNING(EXPR, MSG) EXPR
    #define SPRUE_VERIFY_ERROR(EXPR, MSG) EXPR
    #define SPRUE_VERIFY_FATAL(EXPR, MSG) EXPR
#endif

/// Used in front of definitions to denote the ROOT level class (conceptually)
#define premiere

/// Used to mark functions that C# bindings should be generated for
#define SHARP_BIND

/// Used to mark the type that should be used for the baseclass when binding to C#
#define SHARP_BASE

/// Prevent automatic copy
#define NOCOPYDEF(MyClass) private: MyClass(const MyClass&); MyClass& operator=(const MyClass&) const;

// Conveniently typedef ourself
#define BASECLASSDEF(MyClass, MyBase) public: typedef MyClass self; typedef MyBase base;

#define TYPE_HASHED(TYPENAME) public: virtual StringHash GetTypeHash() const override { return StringHash( #TYPENAME ); }

#define SPRUE_VERSION(VERSION_NUM) public: virtual unsigned GetClassVersion() const override { return VERSION_NUM; }

#ifdef SPRUE_SHARED_PTR
    #define SPRUE_PTR(TYPENAME) std::shared_ptr<TYPENAME>
    #define SPRUE_NEW(TYPENAME, VALUE) std::shared_ptr<TYPENAME>(VALUE)
    #define SPRUE_DELETE(VALUE) value.reset()
#else
    #define SPRUE_PTR(TYPENAME) TYPENAME*
    #define SPRUE_NEW(TYPENAME, VALUE) VALUE
    #define SPRUE_DELETE(VALUE) if (VALUE) { delete VALUE; VALUE = 0x0; }
#endif