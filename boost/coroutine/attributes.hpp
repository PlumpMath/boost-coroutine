
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_ATTRIBUTES_H
#define BOOST_CORO_ATTRIBUTES_H

#include <cstddef>

#include <boost/config.hpp>
#include <boost/context/stack_utils.hpp>

#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {

struct attributes
{
    std::size_t     size;
    flag_unwind_t   do_unwind;
    bool            preserve_fpu;

    attributes() BOOST_NOEXCEPT :
        size( ctx::default_stacksize() ),
        do_unwind( stack_unwind),
        preserve_fpu( true)
    {}

    explicit attributes( std::size_t size_) BOOST_NOEXCEPT :
        size( size_),
        do_unwind( stack_unwind),
        preserve_fpu( true)
    {}

    explicit attributes( flag_unwind_t do_unwind_) BOOST_NOEXCEPT :
        size( ctx::default_stacksize() ),
        do_unwind( do_unwind_),
        preserve_fpu( true)
    {}

    explicit attributes( bool preserve_fpu_) BOOST_NOEXCEPT :
        size( ctx::default_stacksize() ),
        do_unwind( stack_unwind),
        preserve_fpu( preserve_fpu_)
    {}

    explicit attributes(
            std::size_t size_,
            flag_unwind_t do_unwind_) BOOST_NOEXCEPT :
        size( size_),
        do_unwind( do_unwind_),
        preserve_fpu( true)
    {}

    explicit attributes(
            std::size_t size_,
            bool preserve_fpu_) BOOST_NOEXCEPT :
        size( size_),
        do_unwind( stack_unwind),
        preserve_fpu( preserve_fpu_)
    {}

    explicit attributes(
            flag_unwind_t do_unwind_,
            bool preserve_fpu_) BOOST_NOEXCEPT :
        size( ctx::default_stacksize() ),
        do_unwind( do_unwind_),
        preserve_fpu( preserve_fpu_)
    {}
};

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_ATTRIBUTES_H
