#include <gtest/gtest.h>
#include <string>
#include "FunctionContainer.hpp"
#include "CallableFunction.hpp"

using namespace ES::Utils::FunctionContainer;

// Test fixture
class FunctionContainerTest : public ::testing::Test {
protected:
    FunctionContainer<int, int> container;

    static int FreeFunction(int x) { return x + 10; }

    struct Functor {
        int operator()(int x) const { return x + 20; }
    };
};

// Test: Add single lambda function
TEST_F(FunctionContainerTest, AddSingleLambdaFunction) {
    container.AddFunction([](int x) { return x + 1; });
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(5), 6);
}


// Test: Add free function
TEST_F(FunctionContainerTest, AddFreeFunction) {
    container.AddFunction(&FreeFunction);
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(3), 13);
}


// Test: Add functor
TEST_F(FunctionContainerTest, AddFunctor) {
    Functor functor;
    container.AddFunction(functor);
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(2), 22);
}

// Test: Add multiple functions of various types
TEST_F(FunctionContainerTest, AddMultipleFunctions) {
    auto lambda = [](int x) { return x * 2; };
    Functor functor;
    container.AddFunctions(lambda, functor, &FreeFunction);

    auto &functions = container.GetFunctions();
    ASSERT_EQ(functions.size(), 3);

    auto it = functions.begin();
    EXPECT_EQ((**it++)(5), 10);  // lambda
    EXPECT_EQ((**it++)(1), 21);  // functor
    EXPECT_EQ((**it++)(3), 13);  // free function
}

// Test: All functions invoked in order
TEST_F(FunctionContainerTest, FunctionsInvokeInOrder) {
    std::vector<int> expected = {6, 25, 15};
    container.AddFunctions([](int x) { return x + 1; }, Functor{}, &FreeFunction);

    auto &functions = container.GetFunctions();
    ASSERT_EQ(functions.size(), 3);

    int index = 0;
    for (const auto &func : functions) {
        EXPECT_EQ((*func)(5), expected[index++]);
    }
}