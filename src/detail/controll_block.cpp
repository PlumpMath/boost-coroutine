
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_COROUTINES_SOURCE

#include "boost/coroutine/detail/controll_block.hpp"

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

controll_block::controll_block() :
    fcontext_t(),
    ctx_( this)
{}

controll_block::controll_block( controll_block const& other) :
    fcontext_t(),
    ctx_( other.ctx_)
{}

controll_block &
controll_block::operator=( controll_block const& other)
{
    if ( this == & other) return * this;

    ctx_ = other.ctx_;

    return * this;
}

intptr_t
controll_block::jump( controll_block & other, intptr_t param, bool preserve_fpu)
{ return context::jump_fcontext( ctx_, other.ctx_, param, preserve_fpu); }

context::stack_t
controll_block::stack() const BOOST_NOEXCEPT
{ return ctx_->fc_stack; }

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif
