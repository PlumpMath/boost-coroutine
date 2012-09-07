
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/all.hpp>

namespace coro = boost::coro;
namespace ctx = boost::ctx;

int value1 = 0;
std::string value2 = "";

typedef coro::generator< void > gen_void;
typedef coro::generator< int > gen_int;
typedef coro::generator< std::string > gen_string;
typedef coro::generator< double > gen_double;
typedef coro::generator< boost::tuple< int&, int& > > gen_tuple;

struct X : private boost::noncopyable
{
    X() { value1 = 7; }
    ~X() { value1 = 0; }
};

class copyable
{
private:
    int i_;

public:
    copyable( int i) :
        i_( i)
    {}

    int operator()( gen_int::self_t &)
    { return i_; }
};

class moveable
{
private:
    bool    state_;
    int     i_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( moveable);

public:
    moveable() :
        state_( false), i_( 0)
    {}

    moveable( int i) :
        state_( false), i_( i)
    {}

    moveable( BOOST_RV_REF( moveable) other) :
        state_( false), i_( 0)
    {
        std::swap( state_, other.state_);
        std::swap( i_, other.i_);
    }

    moveable & operator=( BOOST_RV_REF( moveable) other)
    {
        if ( this == & other) return * this;
        moveable tmp( boost::move( other) );
        std::swap( state_, tmp.state_);
        std::swap( i_, tmp.i_);
        return * this;
    }

    int operator()( gen_int::self_t &)
    { return i_; }
};

struct my_exception {};

void f1( gen_void::self_t & s)
{}

void f2( gen_void::self_t &)
{ ++value1; }

void f3( gen_int::self_t & self)
{
    self.yield( 1);
    self.yield( 2);
}

void f4( gen_int::self_t & self)
{
    int i = 3;
    self.yield( i);
    self.yield( 7);
}

void f5( gen_string::self_t & self)
{
    std::string str1("abc"), str2("xyz");
    self.yield( str1);
    self.yield( str2);
}

void f8( gen_double::self_t & self, double a, double b)
{
    double tmp = a + b;
    self.yield( tmp);
    self.yield( 3.14 + 8.33);
}

void f11( gen_tuple::self_t & self, int & a, int & b)
{
    boost::tuple<int&,int&> tpl( a, b);
    self.yield( tpl);
    self.yield( tpl);
}

void f12( gen_int::self_t & self, int a, int b)
{
    X x;
    int tmp = a + b;
    self.yield( tmp);
    tmp += a;
    self.yield( tmp);
    self.yield( 3);
}

void f13( gen_int::self_t & self)
{
    self.yield( 3);
}

template< typename E >
void f14( gen_int::self_t & self, E const& e)
{
    throw e;
}

void test_move()
{
    {
        gen_void gen1;
        gen_void gen2( boost::bind( f1, _1) );
        BOOST_CHECK( ! gen1);
        BOOST_CHECK( gen2);
        gen1 = boost::move( gen2);
        BOOST_CHECK( gen1);
        BOOST_CHECK( ! gen2);
    }

    {
        copyable cp( 3);
        gen_int gen( cp);
    }

    {
        moveable mv( 7);
        gen_int gen( boost::move( mv) );
    }
}

void test_complete()
{
    value1 = 0;

    gen_void gen( boost::bind( f2, _1) );
    BOOST_CHECK( gen);
    gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( ( int)1, value1);
}

void test_jump()
{
    value1 = 0;

    gen_int gen( boost::bind( f3, _1) );
    BOOST_CHECK( gen);
    int res = gen();
    BOOST_CHECK( gen);
    BOOST_CHECK_EQUAL( ( int)1, res);
    res = gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( ( int)2, res);
}

void test_result_int()
{
    gen_int gen( boost::bind( f4, _1) );
    BOOST_CHECK( gen);
    int result = gen();
    BOOST_CHECK( gen);
    BOOST_CHECK_EQUAL( 3, result);
    result = gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( 7, result);
}

void test_result_string()
{
    gen_string gen( boost::bind( f5, _1) );
    BOOST_CHECK( gen);
    std::string result = gen();
    BOOST_CHECK( gen);
    BOOST_CHECK_EQUAL( std::string("abc"), result);
    result = gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( std::string("xyz"), result);
}

void test_fp()
{
    gen_double gen( boost::bind( f8, _1, 7.35, 3.14) );
    BOOST_CHECK( gen);
    double res = gen();
    BOOST_CHECK( gen);
    BOOST_CHECK_EQUAL( ( double) 10.49, res);
    res = gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( ( double) 11.47, res);
}

void test_tuple()
{
    int a = 3, b = 7;
    gen_tuple gen( boost::bind( f11, _1, boost::ref( a), boost::ref( b) ) );
    BOOST_CHECK( gen);
    boost::tuple<int&,int&> res = gen();
    BOOST_CHECK( gen);
    BOOST_CHECK_EQUAL( & a, & res.get< 0 >() );
    BOOST_CHECK_EQUAL( & b, & res.get< 1 >() );
    res = gen();
    BOOST_CHECK( ! gen);
    BOOST_CHECK_EQUAL( & a, & res.get< 0 >() );
    BOOST_CHECK_EQUAL( & b, & res.get< 1 >() );
}

void test_unwind()
{
    value1 = 0;
    {
        gen_int gen( boost::bind( f12, _1, 3, 7) );
        BOOST_CHECK( gen);
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        int res = gen();
        BOOST_CHECK_EQUAL( ( int) 10, res);
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        BOOST_CHECK( gen);
    }
    BOOST_CHECK_EQUAL( ( int) 0, value1);
}

void test_no_unwind()
{
    value1 = 0;
    {
        gen_int gen(
            boost::bind( f12, _1, 3, 7),
            coro::attributes( coro::no_stack_unwind) );
        BOOST_CHECK( gen);
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        int res = gen();
        BOOST_CHECK_EQUAL( ( int) 10, res);
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        BOOST_CHECK( gen);
    }
    BOOST_CHECK_EQUAL( ( int) 7, value1);
}

void test_yield_break()
{
    gen_int gen( boost::bind( f13, _1) );
    BOOST_CHECK( gen);
    int res = gen();
    BOOST_CHECK_EQUAL( ( int) 3, res);
    BOOST_CHECK( ! gen);
}

void test_exceptions()
{
    bool thrown = false;
    std::runtime_error ex("abc");
    try
    {
        gen_int gen( boost::bind( f14< std::runtime_error >, _1, ex) );
        BOOST_CHECK( gen);
        gen();
        BOOST_CHECK( false);
    }
    catch ( std::runtime_error const&)
    { thrown = true; }
    catch ( std::exception const&)
    {}
    catch (...)
    {}
    BOOST_CHECK( thrown);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.coroutine: generator test suite");

    test->add( BOOST_TEST_CASE( & test_move) );
    test->add( BOOST_TEST_CASE( & test_complete) );
    test->add( BOOST_TEST_CASE( & test_jump) );
    test->add( BOOST_TEST_CASE( & test_result_int) );
    test->add( BOOST_TEST_CASE( & test_result_string) );
    test->add( BOOST_TEST_CASE( & test_fp) );
    test->add( BOOST_TEST_CASE( & test_tuple) );
    test->add( BOOST_TEST_CASE( & test_unwind) );
    test->add( BOOST_TEST_CASE( & test_no_unwind) );
    test->add( BOOST_TEST_CASE( & test_yield_break) );
    test->add( BOOST_TEST_CASE( & test_exceptions) );

    return test;
}
