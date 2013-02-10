
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_COROUTINES_SOURCE

#if defined(BOOST_USE_SEGMENTED_STACKS)

#include <boost/coroutine/detail/segmented_stack_allocator.hpp>

#include <boost/assert.hpp>
#include <boost/context/fcontext.hpp>

extern "C" {
    
void *__splitstack_makecontext( std::size_t,
                                void * [BOOST_COROUTINES_SEGMENTS],
                                std::size_t *);

void __splitstack_releasecontext (void * [BOOST_COROUTINES_SEGMENTS]);

void __splitstack_resetcontext (void * [BOOST_COROUTINES_SEGMENTS]);

}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

#if !defined (SIGSTKSZ)
# define SIGSTKSZ (8 * 1024)
# define UDEF_SIGSTKSZ
#endif

namespace boost {
namespace coroutines {
namespace detail {

std::size_t
segmented_stack_allocator::default_stacksize()
{ return SIGSTKSZ + sizeof( context::fcontext_t) + 15; }

void *
segmented_stack_allocator::allocate(
    std::size_t min_size, void ** seg,
    std::size_t * size) const
{
    BOOST_ASSERT( default_stacksize() <= min_size);

    void * limit = __splitstack_makecontext( min_size, & seg[0], size);
    BOOST_ASSERT( limit);
    return static_cast< char * >( limit) + * size;
}

void
segmented_stack_allocator::deallocate( void ** seg) const
{ 
    //__splitstack_releasecontext( & seg[0]);
    __splitstack_resetcontext( & seg[0]);
}

}}}

#ifdef UDEF_SIGSTKSZ
# undef SIGSTKSZ
#endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif
