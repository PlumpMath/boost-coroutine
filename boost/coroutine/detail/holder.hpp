
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_HOLDER_H
#define BOOST_CORO_DETAIL_HOLDER_H

#include <boost/config.hpp>
#include <boost/optional.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Data >
struct holder
{
    context::fcontext_t *   ctx;
    optional< Data >        data;

    holder( context::fcontext_t * ctx_, Data data_) :
        ctx( ctx_), data( data_)
    {}
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_HOLDER_H
