
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>
#include <boost/move/move.hpp>
#include <boost/tuple/tuple.hpp>

typedef boost::coro::coroutine< int() > coroutine_t;

struct X
{
    int i;

    int f( coroutine_t::self_t & self)
    {
        self.yield( 3);
        self.yield_break();
        return 7;
    }
};

int main( int argc, char * argv[])
{
    try
    {
        X x; x.i = 0;
        boost::coro::coroutine< int() > coro(
            boost::bind( & X::f, boost::ref( x), _1) );
        int result = coro();
        std::cout << result << std::endl;
        result = coro();
        std::cout << result << std::endl;
    }
    catch(boost::coro::coroutine_terminated const&)
    { std::cerr << "coroutine was terminated" << std::endl; }
    catch(...)
    { std::cerr << "exception catched" << std::endl; }

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}
