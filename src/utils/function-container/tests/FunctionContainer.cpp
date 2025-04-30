#include "FunctionContainer.hpp"
#include "CallableFunction.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace ES::Utils::FunctionContainer;

// Test fixture
class FunctionContainerTest : public ::testing::Test {
  public:
    FunctionContainer<int, int> container;

    static int FreeFunction(int x) { return x + 10; }

    struct Functor {
        int operator()(int x) const { return x + 20; }
    };
};

// Test: Add single lambda function
TEST_F(FunctionContainerTest, AddSingleLambdaFunction)
{
    container.AddFunction([](int x) { return x + 1; });
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(5), 6);
}

// Test: Add free function
TEST_F(FunctionContainerTest, AddFreeFunction)
{
    container.AddFunction(&FreeFunction);
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(3), 13);
}

// Test: Add functor
TEST_F(FunctionContainerTest, AddFunctor)
{
    Functor functor;
    container.AddFunction(functor);
    auto &functions = container.GetFunctions();

    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ((*functions.front())(2), 22);
}

// Test: Add multiple functions of various types
TEST_F(FunctionContainerTest, AddMultipleFunctions)
{
    auto lambda = [](int x) { return x * 2; };
    Functor functor;
    container.AddFunctions(lambda, functor, &FreeFunction);

    auto &functions = container.GetFunctions();
    ASSERT_EQ(functions.size(), 3);

    auto it = functions.begin();
    EXPECT_EQ((**it++)(5), 10); // lambda
    EXPECT_EQ((**it++)(1), 21); // functor
    EXPECT_EQ((**it++)(3), 13); // free function
}

// Test: All functions invoked in order
TEST_F(FunctionContainerTest, FunctionsInvokeInOrder)
{
    std::vector<int> expected = {6, 25, 15};
    container.AddFunctions([](int x) { return x + 1; }, Functor{}, &FreeFunction);

    const auto &functions = container.GetFunctions();
    ASSERT_EQ(functions.size(), 3);

    int index = 0;
    for (const auto &func : functions)
    {
        EXPECT_EQ((*func)(5), expected[index++]);
    }
}

// Test: Check if container is empty
TEST_F(FunctionContainerTest, IsEmpty)
{
    EXPECT_TRUE(container.IsEmpty());
    container.AddFunction([](int x) { return x + 1; });
    EXPECT_FALSE(container.IsEmpty());
}

// Test: Delete function
TEST_F(FunctionContainerTest, DeleteFunction)
{
    auto id = container.AddFunction([](int x) { return x + 1; });
    EXPECT_FALSE(container.IsEmpty());
    EXPECT_TRUE(container.DeleteFunction(id));
    EXPECT_TRUE(container.IsEmpty());
}

// Test: Delete non-existing function
TEST_F(FunctionContainerTest, DeleteNonExistingFunction)
{
    auto id = container.AddFunction([](int x) { return x + 1; });
    EXPECT_TRUE(container.DeleteFunction(id));
    EXPECT_FALSE(container.DeleteFunction(id)); // Try to delete again
}

// Test: Deleting a function doesn't mess up the list and the order
TEST_F(FunctionContainerTest, DeleteFunctionDoesNotMessUpOrder)
{
    container.AddFunction([](int x) { return x + 1; });
    auto id2 = container.AddFunction([](int x) { return x + 2; });
    container.AddFunction([](int x) { return x + 3; });

    EXPECT_TRUE(container.DeleteFunction(id2));

    const auto &functions = container.GetFunctions();
    ASSERT_EQ(functions.size(), 2);

    std::vector<int> expected = {6, 8}; // 5 + 1 and 5 + 3

    for (const auto &func : functions)
    {
        EXPECT_EQ((*func)(5), expected.front());
        expected.erase(expected.begin());
    }
}
