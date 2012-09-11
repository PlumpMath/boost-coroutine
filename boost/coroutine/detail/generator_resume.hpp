
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_RESUME_H
#define BOOST_CORO_DETAIL_GENERATOR_RESUME_H

#include <algorithm>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/optional.hpp>

#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Result, typename D >
class generator_resume
{
public:
    boost::optional< Result > operator()()
    {
        D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);

        optional< Result >  result;
        try
        {
            if ( ! dp->impl_->is_complete() )
            {
                if ( ! dp->impl_->is_started() ) dp->impl_->start( result);
                else dp->impl_->resume( result);
            }
            else result = none;
        }
        catch ( coroutine_terminated const&)
        { result = none; }
        return result;
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_RESUME_H
