
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/assert.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coroutines::coroutine< void() >   coro_t;

int count = 100;

void access( char *buf) __attribute__ ((noinline));
void access( char *buf)
{
  buf[0] = '\0';
}

void bar( int i)
{
    char buf[4 * 1024];

    if ( i > 0)
    {
        access( buf);
        std::cout << i << ". iteration" << std::endl;
        bar( i - 1);
    }
}

void foo( coro_t & c)
{
    bar( count);
    c();
}

int main( int argc, char * argv[])
{
#if defined(BOOST_USE_SEGMENTED_STACKS)
    std::cout << "using segmented stacks: allocates " << count << " * 4kB on stack, ";
    std::cout << "initial stack size = " << boost::coroutines::stack_allocator::default_stacksize() / 1024 << "kB" << std::endl;
    std::cout << "application should not fail" << std::endl;
#else
    std::cout << "using standard stacks: allocates " << count << " * 4kB on stack, ";
    std::cout << "initial stack size = " << boost::coroutines::stack_allocator::default_stacksize() / 1024 << "kB" << std::endl;
    std::cout << "application must fail" << std::endl;
#endif

    {
        coro_t c( foo);
        c();
        int i = 0;
    }

    return 0;
}
