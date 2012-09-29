
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdio>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/move/move.hpp>
#include <boost/range.hpp>
#include <boost/ref.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/all.hpp>

namespace coro = boost::coro;
namespace ctx = boost::context;

int value1 = 0;
std::string value2 = "";
std::vector< int > vec;

typedef coro::coroutine< void() > coro_void_void;
typedef coro::coroutine< int() > coro_int_void;
typedef coro::coroutine< std::string() > coro_string_void;
typedef coro::coroutine< void(int) > coro_void_int;
typedef coro::coroutine< void(std::string const&) > coro_void_string;
typedef coro::coroutine< double(double,double) > coro_double;
typedef coro::coroutine< int(int,int) > coro_int;
typedef coro::coroutine< int*(int*) > coro_ptr;
typedef coro::coroutine< int&(int&) > coro_ref;
typedef coro::coroutine< boost::tuple<int&,int&>(int&,int&) > coro_tuple;

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

    int operator()( coro_int_void::self_t &)
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

    int operator()( coro_int_void::self_t &)
    { return i_; }
};

struct my_exception {};

void f1( coro_void_void::self_t & s)
{}

void f2( coro_void_void::self_t &)
{ ++value1; }

void f3( coro_void_void::self_t & self)
{
    ++value1;
    self();
    ++value1;
}

int f4( coro_int_void::self_t & self)
{
    self( 3);
    return 7;
}

std::string f5( coro_string_void::self_t & self)
{
    std::string res("abc");
    self( res);
    return "xyz";
}

void f6( coro_void_int::self_t & self, int i)
{ value1 = i; }

void f7( coro_void_string::self_t & self, std::string const& str)
{ value2 = str; }

double f8( coro_double::self_t & self, double a, double b)
{
    double tmp = a + b;
    boost::tuple< double, double > ret = self( tmp).get();
    return ret.get< 0 >() + ret.get< 1 >();
}

int * f9( coro_ptr::self_t & self, int * a)
{ return a; }

int & f10( coro_ref::self_t & self, int & a)
{ return a; }

boost::tuple<int&,int&> f11( coro_tuple::self_t & self, int & a, int & b)
{
    boost::tuple<int&,int&> tpl( a, b);
    return tpl;
}

int f12( coro_int::self_t & self, int a, int b)
{
    X x;
    int tmp = a + b;
    boost::tuple< int, int > ret = self( tmp).get();
    value1 = 1;
    return ret.get< 0 >() + ret.get< 1 >();
}

template< typename E >
void f14( coro_void_void::self_t & self, E const& e)
{ throw e; }

int f16( coro_int_void::self_t & self)
{
    self( 1);
    self( 2);
    self( 3);
    self( 4);
    return 5;
}

void f17( coro_void_int::self_t & self, int i)
{
    int x = i;
    while ( 6 > x)
    {
        vec.push_back( x);
        x = self().get();
    }
}

void test_move()
{
    {
        coro_void_void coro1;
        coro_void_void coro2( f1);
        BOOST_CHECK( ! coro1);
        BOOST_CHECK( coro1.empty() );
        BOOST_CHECK( coro2);
        BOOST_CHECK( ! coro2.empty() );
        coro1 = boost::move( coro2);
        BOOST_CHECK( coro1);
        BOOST_CHECK( ! coro1.empty() );
        BOOST_CHECK( ! coro2);
        BOOST_CHECK( coro2.empty() );
    }

    {
        copyable cp( 3);
        coro_int_void coro( cp);
    }

    {
        moveable mv( 7);
        coro_int_void coro( boost::move( mv) );
    }
}

void test_complete()
{
    value1 = 0;

    coro_void_void coro( f2);
    BOOST_CHECK( coro);
    coro();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( ( int)1, value1);
}

void test_jump()
{
    value1 = 0;

    coro_void_void coro( f3);
    BOOST_CHECK( coro);
    coro();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( ( int)1, value1);
    coro();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( ( int)2, value1);
}

void test_result_int()
{
    coro_int_void coro( f4);
    BOOST_CHECK( coro);
    int result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( 3, result);
    result = coro().get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( 7, result);
}

void test_result_string()
{
    coro_string_void coro( f5);
    BOOST_CHECK( coro);
    std::string result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( std::string("abc"), result);
    result = coro().get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( std::string("xyz"), result);
}

void test_arg_int()
{
    value1 = 0;

    coro_void_int coro( f6);
    BOOST_CHECK( coro);
    coro( 3);
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( 3, value1);
}

void test_arg_string()
{
    value2 = "";

    coro_void_string coro( f7);
    BOOST_CHECK( coro);
    coro( std::string("abc") );
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( std::string("abc"), value2);
}

void test_fp()
{
    coro_double coro( f8);
    BOOST_CHECK( coro);
    double res = coro( 7.35, 3.14).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( ( double) 10.49, res);
    res = coro( 1.15, 3.14).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( ( double) 4.29, res);
}

void test_ptr()
{
    coro_ptr coro( f9);
    BOOST_CHECK( coro);
    int a = 3;
    int * res = coro( & a).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, res);
}

void test_ref()
{
    coro_ref coro( f10);
    BOOST_CHECK( coro);
    int a = 3;
    int & res = coro( a).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, & res);
}

void test_tuple()
{
    coro_tuple coro( f11);
    BOOST_CHECK( coro);
    int a = 3, b = 7;
    boost::tuple<int&,int&> res = coro( a, b).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, & res.get< 0 >() );
    BOOST_CHECK_EQUAL( & b, & res.get< 1 >() );
}

void test_unwind()
{
    value1 = 0;
    {
        coro_int coro( f12);
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 0, value1);
        int res = coro( 3, 7).get();
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 10, res);
    }
    BOOST_CHECK_EQUAL( ( int) 0, value1);
}

void test_no_unwind()
{
    value1 = 0;
    {
        coro_int coro(
            f12,
            coro::attributes(
                ctx::guarded_stack_allocator::default_stacksize(),
                coro::no_stack_unwind) );
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 0, value1);
        int res = coro( 3, 7).get();
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 10, res);
    }
    BOOST_CHECK_EQUAL( ( int) 7, value1);
}

void test_exceptions()
{
    bool thrown = false;
    std::runtime_error ex("abc");
    coro_void_void coro( boost::bind( f14< std::runtime_error >, _1, ex) );
    try
    {
        BOOST_CHECK( coro);
        coro();
        BOOST_CHECK( false);
    }
    catch ( std::runtime_error const&)
    { thrown = true; }
    catch ( std::exception const&)
    {}
    catch (...)
    {}
    BOOST_CHECK( thrown);
    BOOST_CHECK( ! coro);
}

void test_output_iterator()
{
    std::vector< int > vec;
    coro_int_void coro( f16);
    BOOST_FOREACH( int i, coro)
    { vec.push_back( i); }
    BOOST_CHECK_EQUAL( ( std::size_t)5, vec.size() );
    BOOST_CHECK_EQUAL( ( int)1, vec[0] );
    BOOST_CHECK_EQUAL( ( int)2, vec[1] );
    BOOST_CHECK_EQUAL( ( int)3, vec[2] );
    BOOST_CHECK_EQUAL( ( int)4, vec[3] );
    BOOST_CHECK_EQUAL( ( int)5, vec[4] );
}

void test_input_iterator()
{
    int counter = 0;
    coro_void_int coro( boost::bind( f17, _1, _2) );
    coro_void_int::iterator e( boost::end( coro) );
    for ( coro_void_int::iterator i( boost::begin( coro) );
          i != e; ++i)
    {
        i = ++counter;
    }
    BOOST_CHECK_EQUAL( ( std::size_t)5, vec.size() );
    BOOST_CHECK_EQUAL( ( int)1, vec[0] );
    BOOST_CHECK_EQUAL( ( int)2, vec[1] );
    BOOST_CHECK_EQUAL( ( int)3, vec[2] );
    BOOST_CHECK_EQUAL( ( int)4, vec[3] );
    BOOST_CHECK_EQUAL( ( int)5, vec[4] );
}


boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.coroutine: coroutine test suite");

    test->add( BOOST_TEST_CASE( & test_move) );
    test->add( BOOST_TEST_CASE( & test_complete) );
    test->add( BOOST_TEST_CASE( & test_jump) );
    test->add( BOOST_TEST_CASE( & test_result_int) );
    test->add( BOOST_TEST_CASE( & test_result_string) );
    test->add( BOOST_TEST_CASE( & test_arg_int) );
    test->add( BOOST_TEST_CASE( & test_arg_string) );
    test->add( BOOST_TEST_CASE( & test_fp) );
    test->add( BOOST_TEST_CASE( & test_ptr) );
    test->add( BOOST_TEST_CASE( & test_ref) );
    test->add( BOOST_TEST_CASE( & test_tuple) );
    test->add( BOOST_TEST_CASE( & test_unwind) );
    test->add( BOOST_TEST_CASE( & test_no_unwind) );
    test->add( BOOST_TEST_CASE( & test_exceptions) );
    test->add( BOOST_TEST_CASE( & test_output_iterator) );
    test->add( BOOST_TEST_CASE( & test_input_iterator) );

    return test;
}
