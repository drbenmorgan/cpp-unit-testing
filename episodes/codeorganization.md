---
title: "Organizing code to assist unit testing"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
::::::::::::::::::::::::::::::::::::: questions 

- How should we organize C++ code to assist unit testing?
- How should we design C++ code to assist unit testing?

::::::::::::::::::::::::::::::::::::::::::::::::

## C++ Project Organization

The ISO C++ Standard does _not_ enforce or require a particular directory layout of project
code for implementation, testing or otherwise. Projects _generally_ separate library/application
source code from the unit (and other) testing code, though the directory levels at which
this is done vary widely. This results in a project that:

1. Builds a _library_ for the implementation code.
   - _Optionally_ the _end user program_ using this library.
2. 1-N _unit test programs_ each providing the test cases for the 1-N units in the _library_.

To make our `test_device.cpp` code follow this pattern, we only need to:

- Factor the `divide` function interface/implementation into a library.
- Compile and link `test_divide.cpp` against this library.

Just to illustrate the principal for this exercise, we'll make `divide` a _header-only_ library
since we're still compiling manually. Thus we'll have a header:

```cpp
//! \file divide.hpp
#pragma once

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

and our `test_divide.cpp` becomes:

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

If we have `divide.hpp` and `test_divide.cpp` in the same directory, we can compile and run just
as we did before:

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

but we have decoupled _what_ we test from _how_ we test it.


## C++ Design to Assist Unit Testing

- pure functions
- separation of concerns (e.g. don't mix I/O and math)
- avoid mega-functions/loops.
  - basic factoring.
- avoid global state


::::::::::::::::::::::::::::::::::::: objectives

- Refactor our `divide` example into a header-only library and test.

::::::::::::::::::::::::::::::::::::::::::::::::
