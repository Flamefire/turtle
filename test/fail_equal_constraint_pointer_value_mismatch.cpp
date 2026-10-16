// http://turtle.sourceforge.net
//
// Copyright 2026 Alexander Grund
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <turtle/mock.hpp>

MOCK_CLASS(my_class)
{
    MOCK_METHOD(my_method, 1, void(int), my_method)
    MOCK_METHOD(my_ptr_method, 1, void(int*), my_ptr_method)
};
void test_case()
{
    my_class c;
    // A pointer expected value cannot be compared to a non-pointer actual value
    MOCK_EXPECT(c.my_method).with(mock::equal<int*>(0));
    // And vice versa a non-pointer expected value cannot be compared to a pointer actual value
    MOCK_EXPECT(c.my_ptr_method).with(mock::equal<int>(0));
}
