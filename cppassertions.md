---
title: "Unit testing with `assert()`"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How can we implement unit tests in C++?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Write a simple unit test case using `assert()`.
- Understand the limits of this approach.

::::::::::::::::::::::::::::::::::::::::::::::::

## Basic use of `assert` to implement unit tests

Recall that  **Unit Tests** check the behaviour of code _units_ (functions,
classes, or data structures) individually across the range of inputs/outputs and exceptional
circumstances. We already have a "unit" in the `divide` function, so let's write some unit
tests for it! To do this, we'll use the C library [`assert` macro](https://en.cppreference.com/cpp/error/assert). This wraps a C++ conditional statement which must not be equal to `0` otherwise
a message will be written to standard error, and `std::abort()` called to terminate the program.
Thus by writing suitable conditionals, we can use this to compose unit tests:

```cpp
//! \file test_divide.cpp
#include <cassert> // For assert()
#include <stdexcept>

/* Return result of dividing numerator by denominator */
double divide(double numerator, double denominator)
{
   if(denominator == 0.0)
   {
      throw std::invalid_argument("division by zero");
   }

   return numerator / denominator;
}

/* Unit tests for divide (numeric cases)*/
void test_divide_numeric()
{
   // Our test - assert will fail if the condition evaluates to `0` (or `false`)
   assert(divide(4,2) == 2);

   // We can add a message about the failure, but involves a trick with the conditional
   assert(divide(3,2) == 1.5 && "3/2 is not equal to 1.5");
}

/* Unit tests for divide (error cases)*/
void test_divide_error()
{
  // We can check that an exception is thrown.
   try
   {
      int no_result = divide(1, 0);
      assert(false && "No exception thrown");
   }
   catch (const std::invalid_argument&)
   { // Nothing to do 
   }
}

/* main() just runs the tests... */
int main()
{
   test_divide_numeric();
   test_divide_error()
}
```

Whilst we're working with _very_ simple code here, we can see the basic structure that
we'll continue with (but gradually refactor):

1. The "unit" we want to test.
2. Function(s) that implement the tests for that unit, divided into so called _test cases_.
3. The overall unit test is a program that runs all of the test cases.

::::::::::::::::::::::::::::::::::::: challenge 

## Adding a failing assertion

Update your `test_divide.cpp` as above, and try adding an `assert` in `test_divide_numeric` that you 
know will fail. Compile and run to check that you do indeed get the assertion. When you get the
assertion, what is the exit code of the application?

:::::::::::::::::::::::: solution 

The simplest way to do this is with a deliberately wrong answer:

```cpp
   assert(divide(2,2) == 4 && "2/2 is not equal to 4"); // Yes this is dumb!
```

When you compile this and run you would see something like

```bash
Assertion failed: (divide(2,2) == 4 && "2/2 is not equal to 4"), function test_divide, file test_divide.cpp, line 24.
```

This will obviously differ depending on what failure you used, but the key things are
that:

1. You get the "Assertion failed" message.
2. You get the line of code where this happened, and your message if you added one.
3. The return code (which you get from running `$?` immediately after executing the test) is non-zero.

:::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::


## Limitations of using `assert`

::::::::::::::::::::::::::::::::::::: challenge 

### Discussion

Even with this simplicity, you can probably already see some of the limitations with using
`assert` to implement unit tests. Think about these, and what other problems might be encountered
with more complex test cases.

:::::::::::::::::::::::: solution 

1. Whilst we know an `assert` failed, we don't directly get information on why it failed: it doesn't automatically print _what_ the result of `divide` was.
2. The first failing `assert` causes program termination and no further test cases run even if they could. We therefore don't get additional checks that _might_ help to triage the cause.
3. The "pass/fail" check _must_ be implemented as a C++ statement that should evaluate to "not 0" for "pass". Combined with point 1, this means we'll need to implement a lot of functionality for more complex checks (see for example what we have to do just to check a required exception is thrown).

:::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::

There's another compile-related issue with `assert`: it can be _completely disabled_ though the
preprocessor argument `-DNDEBUG`. Compiling your test case with this flag:

:::::::::::::::: spoiler
### Linux

```bash
g++ -DNDEBUG -std=c++17 test_divide.cpp -o test_divide
./test_divide
```
::::::::::::::::::::::::

:::::::::::::::: spoiler
### MacOS

```bash
clang++ -DNDEBUG -std=c++17 test_divide.cpp -o test_divide
./test_divide 
```
::::::::::::::::::::::::

If you run `test_divide` compiled like this, it will execute successfully no matter what.
The `NDEBUG` symbol is typically used in fully optimized builds of high performance codes
as the check on `assert` does add small overhead. This reveals its purpose as more for
defensive programming and _design by contract verification during development_ than unit testing 

::::::::::::::::::::::::::::::::::::: keypoints 

- We can use `assert` and `try/catch` to implement simple unit tests.
- This has limitations:
  - Limited information on where/why an assertion failed.
  - Execution stops on the first failure.
  - Can be completely disabled.

::::::::::::::::::::::::::::::::::::::::::::::::
