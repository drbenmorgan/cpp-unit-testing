---
title: "Organizing code to assist unit testing"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
::::::::::::::::::::::::::::::::::::: questions 

- How should we organize C++ code to assist unit testing?
- How should we design C++ code to assist unit testing?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Refactor our `divide` example into a header-only library and test.

::::::::::::::::::::::::::::::::::::::::::::::::

## C++ Package Organization

At present, we have both the unit of code we want to test and the test code in a single file.
Practically, the `divide` function is more likely to be part of a larger C++ _project/package_ 
that compiles a large set of functions and classes into an end-user program or a _library_ of
reusable, pre-compiled code. Thus we usually separate the program/library _implementation_ code
from that which _tests_ it. The ISO C++ Standard does _not_ enforce or require a specific
directory layout of package implementation and testing code, leaving this up to the package
maintainers.

### Splitting `divide` and its tests into a header and program

We'll start to factor `test_divide.cpp` to split the implementation (`divide`) from the test (`test_divide.cpp`). The first step is to create a _header_ file `divide.hpp` alongside `test_divide.cpp` which will hold the implementation of `divide`:

```cpp
//! \file divide.hpp
#pragma once // header guard

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
```

::::: callout
This is an example of a "header-only" _library_ that are now quite common in modern C++. We'll
see the more usual binary libraries in later exercises.
:::::::::::::

We can now modify `test_divide.cpp` to simply include this header, this enabling use of `divide`:

```cpp
//! \file test_divide.cpp
#include "divide.hpp" // Include the interface for what we're testing

#include <cassert> // For assert()

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

We can now compile and run this just as before:

:::::::::::::::: spoiler
### Linux

```bash
g++ -std=c++17 test_divide.cpp -o test_divide
./test_divide
```
::::::::::::::::::::::::

:::::::::::::::: spoiler
### MacOS

```bash
clang++ -std=c++17 test_divide.cpp -o test_divide
./test_divide 
```
::::::::::::::::::::::::

::::: callout
We've deliberately put `divide.hpp` and `test_divide.cpp` in the same directory so that
the compiler will find `divide.hpp` easily.
:::::::::::::

Overall, this isn't much different from what we already have, but we have
decoupled _what_ we test from _how_ we test it.


## C++ Design to Assist Unit Testing

- pure functions
- separation of concerns (e.g. don't mix I/O and math)
- avoid mega-functions/loops.
  - basic factoring.
- avoid global state



::::::::::::::::::::::::::::::::::::: keypoints 

- We should factor our code to help us test.

::::::::::::::::::::::::::::::::::::::::::::::::
