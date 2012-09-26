
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_FLAGS_H
#define BOOST_CORO_FLAGS_H

namespace boost {
namespace coro {

enum flag_unwind_t
{
    stack_unwind = 0,
    no_stack_unwind
};

enum flag_fpu_t
{
    fpu_preserved = 0,
    fpu_not_preserved
};

}}

#endif // BOOST_CORO_FLAGS_H
