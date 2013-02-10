
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_COROUTINES_SOURCE

#include "boost/coroutine/detail/controll_block.hpp"

#if defined(BOOST_USE_SEGMENTED_STACKS)
extern "C" {

void __splitstack_getcontext( void * [BOOST_COROUTINES_SEGMENTS]);

void __splitstack_setcontext( void * [BOOST_COROUTINES_SEGMENTS]);

void __splitstack_releasecontext (void * [BOOST_COROUTINES_SEGMENTS]);

void __splitstack_block_signals_context( void * [BOOST_COROUTINES_SEGMENTS], int *, int *);

}
#endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

controll_block::controll_block() :
    fcontext_t(),
    ctx_( this)
#if defined(BOOST_USE_SEGMENTED_STACKS)
    , seg_()
#endif
{}

#if defined(BOOST_USE_SEGMENTED_STACKS)
controll_block::controll_block( controll_block const& other) :
    fcontext_t(),
    ctx_( other.ctx_),
    seg_()
{ seg_ = const_cast< controll_block * >( & other)->seg_; }
#else
controll_block::controll_block( controll_block const& other) :
    fcontext_t(),
    ctx_( other.ctx_)
{}
#endif

controll_block &
controll_block::operator=( controll_block const& other)
{
    if ( this == & other) return * this;

    ctx_ = other.ctx_;
#if defined(BOOST_USE_SEGMENTED_STACKS)
    seg_ = const_cast< controll_block * >( & other)->seg_;
#endif

    return * this;
}

intptr_t
controll_block::jump( controll_block & other, intptr_t param, bool preserve_fpu)
{
#if defined(BOOST_USE_SEGMENTED_STACKS)
    if ( seg_) __splitstack_getcontext( & seg_[0]);
    if ( other.seg_) __splitstack_setcontext( & other.seg_[0]);
#endif
    return context::jump_fcontext( ctx_, other.ctx_, param, preserve_fpu);
#if defined(BOOST_USE_SEGMENTED_STACKS)
    if ( seg_) __splitstack_setcontext( & seg_[0]);
#endif
}

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif
