
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_CONTEXT_EXEC_H
#define BOOST_CORO_DETAIL_CONTEXT_EXEC_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_base.hpp>
#include <boost/coroutine/detail/context_self.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
class context_exec;

template< typename Signature, typename D >
class context_exec< Signature, D, void, 0 > :
    public context_base< Signature, void, 0 >
{
private:
    void exec_()
    {
        D * dp = static_cast< D * >( this);

        context_self< Signature, void, 0 > self( this);
        dp->fn_( self);
    }

public:
    template< typename StackAllocator >
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        context_base< Signature, void, 0 >( attr, alloc)
    {}
};

template< typename Signature, typename D, typename Result >
class context_exec< Signature, D, Result, 0 > :
    public context_base< Signature, Result, 0 >
{
private:
    typedef Result      result_t;

    result_t exec_()
    {
        D * dp = static_cast< D * >( this);

        context_self< Signature, Result, 0 > self( this);
        return dp->fn_( self);
    }

public:
    template< typename StackAllocator >
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        context_base< Signature, Result, 0 >( attr, alloc)
    {}
};

template< typename Signature, typename D >
class context_exec< Signature, D, void, 1 > :
    public context_base< Signature, void, 1 >
{
private:
    typedef typename arg< Signature >::type_t   arg_t;

    void exec_( arg_t a)
    {
        D * dp = static_cast< D * >( this);

        context_self< Signature, void, 1 > self( this);
        dp->fn_( self, a);
    }

public:
    template< typename StackAllocator >
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        context_base< Signature, void, 1 >( attr, alloc)
    {}
};

template< typename Signature, typename D, typename Result >
class context_exec< Signature, D, Result, 1 > :
    public context_base< Signature, Result, 1 >
{
private:
    typedef typename arg< Signature >::type_t   arg_t;
    typedef Result                              result_t;

    result_t exec_( arg_t a)
    {
        D * dp = static_cast< D * >( this);

        context_self< Signature, Result, 1 > self( this);
        return dp->fn_( self, a);
    }

public:
    template< typename StackAllocator >
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        context_base< Signature, Result, 1 >( attr, alloc)
    {}
};

#define BOOST_CONTEXT_EXEC_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_CONTEXT_EXEC_VAL(z,n,unused) BOOST_CONTEXT_EXEC_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_EXEC_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_EXEC_VAL,~)
#define BOOST_CONTEXT_EXEC_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_CONTEXT_EXEC_ARG(z,n,unused) BOOST_CONTEXT_EXEC_COMMA(n) BOOST_CONTEXT_EXEC_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_EXEC_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_EXEC_ARG,~)
#define BOOST_CONTEXT_EXEC(z,n,unused) \
template< typename Signature, typename D > \
class context_exec< Signature, D, void, n > : \
    public context_base< Signature, void, n > \
{ \
private:\
    void exec_( BOOST_CONTEXT_EXEC_ARGS(n)) \
    { \
        D * dp = static_cast< D * >( this); \
\
        context_self< Signature, void, n > self( this); \
        dp->fn_( self, BOOST_CONTEXT_EXEC_VALS(n)); \
    } \
\
public: \
    template< typename StackAllocator > \
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT : \
        context_base< Signature, void, n >( attr, alloc) \
    {} \
}; \
\
template< typename Signature, typename D, typename Result > \
class context_exec< Signature, D, Result, n > : \
    public context_base< Signature, Result, n > \
{ \
private: \
    typedef Result      result_t; \
\
    result_t exec_( BOOST_CONTEXT_EXEC_ARGS(n)) \
    { \
        D * dp = static_cast< D * >( this); \
\
        context_self< Signature, Result, n > self( this); \
        return dp->fn_( self, BOOST_CONTEXT_EXEC_VALS(n)); \
    } \
\
public: \
    template< typename StackAllocator > \
    context_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT : \
        context_base< Signature, Result, n >( attr, alloc) \
    {} \
};
BOOST_PP_REPEAT_FROM_TO(2,11,BOOST_CONTEXT_EXEC,~)
#undef BOOST_CONTEXT_EXEC
#undef BOOST_CONTEXT_EXEC_ARGS
#undef BOOST_CONTEXT_EXEC_ARG
#undef BOOST_CONTEXT_EXEC_ARG_TYPE
#undef BOOST_CONTEXT_EXEC_VALS
#undef BOOST_CONTEXT_EXEC_VAL
#undef BOOST_CONTEXT_EXEC_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_EXEC_H
