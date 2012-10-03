
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coro::coroutine< void() > coroutine_t;

void first( coroutine_t::caller_t & self)
{
    std::cout << "started first! ";
    for ( int i = 0; i < 10; ++i)
    {
        self();
        std::cout << "a" << i;
    }
}

void second( coroutine_t::caller_t & self)
{
    std::cout << "started second! ";
    for ( int i = 0; i < 10; ++i)
    {
        self();
        std::cout << "b" << i;
    }
}

int main( int argc, char * argv[])
{
    {
        coroutine_t c1( boost::bind( first, _1) );
        coroutine_t c2( boost::bind( second, _1) );
        while ( c1 && c2) {
            c1();
            std::cout << " ";
            c2();
            std::cout << " ";
        }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
