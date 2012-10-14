
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coro::coroutine< void() >   coro_t;

void echo( coro_t & self, int i)
{
    std::cout << i; 
    self();
}

void runit( coro_t & self)
{
    std::cout << "started! ";
//  for ( int i = 0; i < 10; ++i)
//  {
//      coro_t c( boost::bind( echo, _1, i) );
//      while ( c)
//          c();
//      self();
//  }
}

int main( int argc, char * argv[])
{
    {
        coro_t c( runit);
        while ( c) {
            std::cout << "-";
            c();
        }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
