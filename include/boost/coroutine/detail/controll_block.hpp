
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_CONTROLL_BLOCK_H
#define BOOST_COROUTINES_DETAIL_CONTROLL_BLOCK_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/detail/config.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

class BOOST_COROUTINES_DECL controll_block : private noncopyable,
                                             private context::fcontext_t
                    
{
private:
    context::fcontext_t *   ctx_;

public:
    typedef void( * ctx_fn)( intptr_t);

    controll_block();

    explicit controll_block( ctx_fn fn, void * sp, std::size_t size) :
        fcontext_t(),
        ctx_( 
            context::make_fcontext(
                sp, size, fn) )
    {}

    controll_block( controll_block const&);

    controll_block& operator=( controll_block const&);

    intptr_t jump( controll_block &, intptr_t = 0, bool preserve_fpu = true);

    context::stack_t stack() const BOOST_NOEXCEPT;
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_CONTROLL_BLOCK_H
