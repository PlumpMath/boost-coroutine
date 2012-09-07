
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_BASE_RESUME_H
#define BOOST_CORO_DETAIL_GENERATOR_BASE_RESUME_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/coroutine/detail/config.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename D, typename Result >
struct generator_base_resume;

template< typename D >
struct generator_base_resume< D, void >
{
    void resume()
    {
        D * dp = static_cast< D * >( this);

        dp->native_resume( 0);
    }
};

template< typename D, typename Result >
struct generator_base_resume
{
    void resume( optional< Result > & result)
    {
        D * dp = static_cast< D * >( this);

        intptr_t ret = dp->native_resume( 0);
        if ( 0 != ret)
            result = * ( typename remove_reference< Result >::type *) ret;
        else
            result = none;
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_BASE_RESUME_H
