
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_SELF_H
#define BOOST_CORO_DETAIL_GENERATOR_SELF_H

#include <boost/config.hpp>
#include <boost/throw_exception.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/generator_base.hpp>
#include <boost/coroutine/detail/param_type.hpp>
#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Result >
class generator_self;

template<>
class generator_self< void >
{
private:
    typedef detail::generator_base< void >     base_t;
    typedef base_t                         *   ptr_t; 

    template< typename X, typename Y, typename Z, typename R >
    friend class generator_object;

    ptr_t  impl_;

    generator_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    void yield()
    {
        BOOST_ASSERT( impl_);
        impl_->suspend();
    }
};

template< typename Result >
class generator_self
{
private:
    typedef detail::generator_base< Result >    base_t;
    typedef base_t                          *   ptr_t; 

    template< typename X, typename Y, typename Z, typename R >
    friend class generator_object;

    ptr_t  impl_;

    generator_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    void yield( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        impl_->suspend( param);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_SELF_H
