
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coro::generator< int >    gen_t;

int power( gen_t::self_t & self, int number, int exponent)
{
    int counter = 0;
    int result = 1;
    while ( counter++ < exponent)
    {
            result = result * number;
            self.yield( result);
    }
    self.yield_break();
    return -1;
}

int main()
{
    {
        gen_t pw( boost::bind( power, _1, 2, 8) );
        while ( pw) {
            std::cout << pw() <<  " ";
        }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
