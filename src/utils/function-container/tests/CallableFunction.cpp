#include <gtest/gtest.h>

#include "CallableFunction.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <string>

int FreeFunction(int x) { return x + 1; }

struct Functor {
    int operator()(int x) const { return x + 2; }
};

namespace ES::Utils::FunctionContainer {

// Test fixture for CallableFunction
class CallableFunctionTest : public ::testing::Test {
  public:
    using FunctionBase = BaseFunction<int, int>;
};

// Lambda test
TEST_F(CallableFunctionTest, LambdaCallable)
{
    auto lambda = [](int x) { return x + 3; };
    CallableFunction<decltype(lambda), int, int> func(lambda);
    EXPECT_EQ(func(5), 8);
}

// Free function test
TEST_F(CallableFunctionTest, FreeFunctionCallable)
{
    CallableFunction<decltype(&FreeFunction), int, int> func(&FreeFunction);
    EXPECT_EQ(func(4), 5);
}

// Function pointer test (explicit)
TEST_F(CallableFunctionTest, FunctionPointerCallable)
{
    int (*funcPtr)(int) = &FreeFunction;
    CallableFunction<decltype(funcPtr), int, int> func(funcPtr);
    EXPECT_EQ(func(7), 8);
}

// Functor test
TEST_F(CallableFunctionTest, FunctorCallable)
{
    Functor functor;
    CallableFunction<Functor, int, int> func(functor);
    EXPECT_EQ(func(2), 4);
}

// std::function test
TEST_F(CallableFunctionTest, StdFunctionCallable)
{
    std::function<int(int)> stdFunc = [](int x) { return x + 4; };
    CallableFunction<decltype(stdFunc), int, int> func(stdFunc);
    EXPECT_EQ(func(3), 7);
}

} // namespace ES::Utils::FunctionContainer
