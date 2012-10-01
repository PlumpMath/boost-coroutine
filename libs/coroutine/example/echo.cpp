
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coro::coroutine < void() >        coro_void_t;

void echo( coro_void_t::self_t & self, int i)
{
    std::cout << i; 
    self.yield();
}

void runit( coro_void_t::self_t & self)
{
    std::cout << "started! ";
    for ( int i = 0; i < 10; ++i)
    {
        coro_void_t c( boost::bind( echo, _1, i) );
        while ( c)
            c();
        self.yield();
    }
}

int main( int argc, char * argv[])
{
    {
        coro_void_t c( boost::bind( runit, _1) );
        while ( c) {
            std::cout << "-";
            c();
        }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
