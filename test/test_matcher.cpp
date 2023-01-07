// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2012
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <turtle/detail/expectation.hpp>
#include <turtle/matcher.hpp>
#include <boost/test/unit_test.hpp>
#include <functional>
#include <string>

namespace {
template<typename Expected, typename Actual>
bool match(Expected expected, Actual actual)
{
    return mock::matcher<Actual, Expected>(expected)(actual);
}
} // namespace

BOOST_AUTO_TEST_CASE(int_and_int_can_be_compared)
{
    BOOST_CHECK(match(3, 3));
    BOOST_CHECK(!match(3, 4));
    BOOST_CHECK(!match(4, 3));
}

BOOST_AUTO_TEST_CASE(ref_to_int_and_int_can_be_compared)
{
    const int i = 3;
    BOOST_CHECK(match(3, std::cref(i)));
    BOOST_CHECK(!match(4, std::cref(i)));
}

namespace {
struct fixture
{
    fixture() : text("same text")
    {
        // Get a pointer to unique memory containing "same text"
        actual = text.c_str();
    }
    const char* actual;

private:
    std::string text;
};
} // namespace

BOOST_FIXTURE_TEST_CASE(const_char_pointer_and_const_char_pointer_can_be_compared, fixture)
{
    const char* expected = "same text";
    BOOST_REQUIRE(expected != actual);
    BOOST_REQUIRE(std::string(expected) == actual);

    BOOST_CHECK(match(expected, actual));
    const char* unexpected = "different text";
    BOOST_CHECK(!match(unexpected, actual));
}

BOOST_FIXTURE_TEST_CASE(const_char_pointer_and_string_literal_can_be_compared, fixture)
{
    BOOST_CHECK(match("same text", actual));
    BOOST_CHECK(!match("different text", actual));
}

BOOST_FIXTURE_TEST_CASE(const_char_pointer_and_const_char_array_can_be_compared, fixture)
{
    const char expected[] = "same text";
    BOOST_REQUIRE(expected != actual);
    BOOST_CHECK(match(expected, actual));
    const char unexpected[] = "different text";
    BOOST_CHECK(!match(unexpected, actual));
}

BOOST_FIXTURE_TEST_CASE(const_char_pointer_and_std_string_can_be_compared, fixture)
{
    BOOST_CHECK(match(std::string("same text"), actual));
    BOOST_CHECK(match(actual, std::string("same text")));
    BOOST_CHECK(!match(std::string("different text"), actual));
    BOOST_CHECK(!match(actual, std::string("different text")));
}

BOOST_AUTO_TEST_CASE(null_const_char_pointers_can_be_compared)
{
    const char* const null_str = nullptr;
    BOOST_CHECK(match(null_str, null_str));
    BOOST_CHECK(!match(null_str, "non-null string"));
    BOOST_CHECK(!match("non-null string", null_str));
}

namespace {
template<typename T>
std::string serialize(const T& t)
{
    std::ostringstream s;
    s << t;
    return s.str();
}
} // namespace

BOOST_AUTO_TEST_CASE(default_matcher_is_serialized_to_any)
{
    using mock::detail::default_matcher;
    BOOST_TEST(serialize(default_matcher<>{}) == "");
    BOOST_TEST(serialize(default_matcher<int>{}) == "any");
    BOOST_TEST(serialize(default_matcher<int, int>{}) == "any, any");
    BOOST_TEST(serialize(default_matcher<int, int, int, int, int>{}) == "any, any, any, any, any");
}

namespace {
struct custom_constraint
{
    int expected_;
    custom_constraint(int expected = 42) : expected_(expected) {}
    friend std::ostream& operator<<(std::ostream& s, const custom_constraint& c)
    {
        return s << "custom" << c.expected_;
    }
    bool operator()(int actual) { return actual == expected_; }
};

bool custom_constraint_func(int)
{
    return false;
}
} // namespace

BOOST_AUTO_TEST_CASE(single_matcher_serializes)
{
    using mock::detail::single_matcher;
    BOOST_TEST(serialize(single_matcher<void(int), int>(1)) == "1");
    BOOST_TEST(serialize(single_matcher<void(int, int), int, int>(1, 2)) == "1, 2");
    BOOST_TEST(
      serialize(single_matcher<void(int, int, mock::constraint<custom_constraint>, int, int), int, int, int, int, int>(
        1, 2, custom_constraint(), 4, 5)) == "1, 2, custom42, 4, 5");
}

BOOST_AUTO_TEST_CASE(string_matcher_serializes)
{
    using mock::detail::single_matcher;
    BOOST_TEST(serialize(single_matcher<void(const char*), const char*>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(const std::string&), const char*>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(const char*), const std::string&>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(const std::string&), const std::string&>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(std::string), const char*>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(const char*), std::string>("foo")) == "\"foo\"");
    BOOST_TEST(serialize(single_matcher<void(std::string), std::string>("foo")) == "\"foo\"");
    // Mixed types
    BOOST_TEST(serialize(single_matcher<void(const char*, int), const char*, int>("bar", 2)) == "\"bar\", 2");
}

BOOST_AUTO_TEST_CASE(functor_matcher_serializes)
{
    using mock::detail::single_matcher;
    using Functor = decltype(custom_constraint_func);
    BOOST_TEST(serialize(single_matcher<void(Functor), int>(custom_constraint_func)) == "?");
    BOOST_TEST(serialize(single_matcher<void(Functor), short>(custom_constraint_func)) == "?");
    // Mixed types
    BOOST_TEST(serialize(single_matcher<void(Functor, int), short, int>(custom_constraint_func, 2)) == "?, 2");
}

BOOST_AUTO_TEST_CASE(multi_matcher_serializes)
{
    using mock::detail::multi_matcher;
    BOOST_TEST(serialize(multi_matcher<custom_constraint, int>(custom_constraint(1337))) == "custom1337");
}
