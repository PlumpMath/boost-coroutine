
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_FLAGS_H
#define BOOST_CORO_DETAIL_FLAGS_H

#include <boost/config.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

enum flag_t
{
    flag_started                = 1 << 1,
    flag_resumed                = 1 << 2,
    flag_running                = 1 << 3,
    flag_complete               = 1 << 4,
    flag_unwind_stack           = 1 << 5,
    flag_force_unwind           = 1 << 6,
    flag_dont_force_unwind      = 1 << 7,
    flag_has_exception          = 1 << 8
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_FLAGS_H
