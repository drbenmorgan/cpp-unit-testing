---
title: "Unit testing with `assert()`"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How can we implement unit tests in C++?
- What are the limitations of using `assert()` for testing?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Write a test for a pure function using assert()
- Explain what happens at runtime when an assert() passes and when it fails
- Compile and run a test program manually and interpret the output
- Identify the friction of manual compilation as the number of test files grows
- Recognise that assert() cannot easily test for exceptions or produce informative failure output

- Understand how testing documents our intent and encodes this in tests.
- Write simple unit test cases using `assert()`.
- Understand the limits of this approach.

::::::::::::::::::::::::::::::::::::::::::::::::

## What should we test?

Let's revisit our `invariant_mass()` function that we've sketched out. It's rather trivial,
so as smart scientists we might think "that's obviously correct", to which the obvious
(scientific!) response is _how do you know_? We actually need to take a slight side step
into _documentation_ here because the first part of "how do you know?" is "_what **contract**
is this function supposed to offer?_" because this what we want to verify:

```cpp
//! \file test_invariant_mass.cpp
#include <cmath>
#include <stdexcept>

// 1. Return invariant mass $m = sqrt(E^2 - p^2) in natural units
// 2. throws std::domain_error if E < 0
// 3. throws std::domain_error if E^2 - p^2 < 0
double invariant_mass(double energy, double momentum)
{
   if (energy < 0)
   {
      throw std::domain_error("unphysical negative energy");
   }
   double mass_squared = energy*energy - momentum*momentum;
   if (mass_squared < 0)
   {
      throw std::domain_error("unphysical mass^2");  
   }
   return std::sqrt(mass_squared);
}
```

You should think of _documentation_ and _testing_ being symbiotic - the former helps
you reason and record (for "future you" and users of your code) _what_ the code should do,
and this provides a written specification for _what we need to test_. 

::::: callout

This is the ideal case - you may need to work with code that isn't documented like this!
Adding a specification like we've done is always a good first step if you find yourself in
this position, because it will either complement any existing tests, or provide a foundation for writing them if they don't exist.

Documenting code for developers and users is a huge topic itself, and like testing is best formalised
through dedicated tools for the job, like [Doxygen](https://www.doxygen.nl).

:::::::::::::

## How should we test?

Now that we know what the code is supposed to do, we can write unit tests to verify that it actually
does this instead of our minimal smoke test. An extremely common method here is the Mk1 human eye:

```cpp
//! \file test_invariant_mass.cpp
#include "invariant_mass.hpp"

#include <iostream>

// Case 1. Test physical domain
void test_physical_domain()
{
   std::cout << "photon mass should be zero: " << invariant_mass(100,100) << std::endl;
}

// Case 2. Test unphysical energy
void test_unphysical_energy()
{
   try {
      std::cout << "negative energy should throw exception: ";
      double bad_result = invariant_mass(-3.14,3.0);
      std::cout << "fail" << std::endl;
   }
   catch (const std::domain_error&)
   {
      std::cout << "pass" << std::endl;
   }
}

// Run the tests
int main()
{
   test_physical_domain();
   test_unphysical_energy();
}
```

Whilst we're working with _very_ simple code here, we can see the basic structure that
we'll continue with (but gradually refactor):

1. The "unit" we want to test - `invariant_mass`.
2. Function(s) that implement the tests for that unit, divided into so called **test cases**.
3. We explicitly check both normal operation and failure modes.
4. The overall unit test is a program that runs all of the test cases.

If we compile and run this, then we'll get output:

```bash
# use clang++ if on macOS
g++ -std=c++17 test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
photon mass should be zero: 0
negative energy should throw exception: pass
```

That's o.k. but there at least two issues here:

1. The tests always **pass** in the sense that the program itself succeeds, even if we don't get the expected result. 
2. The fallible and inefficient human eye/brain is used to check for **failure**, and this doesn't scale (imagine you have 100 test results).

Both issues can be addressed in the testing code: if we know what the result should be, we can
get the computer to compare the calculations with our expected values, and fail the test, i.e. 
emit an error, if these don't match.

## Basic use of `assert` to implement unit tests

We could use C++ conditional blocks to write the tests, but the underlying C library provides a macro
that can help us here: [`assert` macro](https://en.cppreference.com/cpp/error/assert). This 
wraps a C++ statement which must not be equal to `0` otherwise a message will be written to 
standard error, and `std::abort()` called to terminate the program.

```cpp
//! \file test_invariant_mass.cpp
#include "invariant_mass.hh"

#include <cassert>
#include <iostream>

// Case 1. Test physical domain
void test_physical_domain()
{
   assert((invariant_mass(100,100) == 0.0) && "mass of photon is not 0");  
}

// Case 2. Test unphysical energy
void test_unphysical_energy()
{
   try {
      double bad_result = invariant_mass(-3.14,3.0);
      assert(false && "std::domain_error not thrown for negative energy");
   }
   catch (const std::domain_error&)
   {
      std::cout << "pass" << std::endl;
   }
}

// Run the tests
int main()
{
   test_physical_domain();
   test_unphysical_energy();
}
```

Now we compile and run again:

```bash
# use clang++ if on macOS
g++ -std=c++17 test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
pass
```

So with `assert` we _don't_ get output by default unless we explicitly add it, though the 
program still ran and in this case successfully: our tests _passed_. It also provides a 
cleaner way to express _what_ is being checked compared to our by-eye version.

::::::::::::::::::::::::::::::::::::: challenge 

## Making a failing assertion

Add an `assert` in `test_physical_domain` that you know will fail, then compile and run again.

1. What output do you get now?
2. What is the exit code of the application?
3. Did all of the tests run?

:::::::::::::::::::::::: solution 

The simplest way to do this is with a deliberately wrong answer:

```cpp
   assert(invariant_mass(100,100) == 0.1 && "deliberate fail with unphysical answer");
```

1. When you compile this and run you would see something like

   ```bash
   Assertion failed: (invariant_mass(100,100) == 0.1 && "deliberate fail with unphysical answer"), function test_physical_domain, file test_invariant_mass.cpp, line 27.
   ```

   We see that the assertion failed and we get the assertion printed. This is why we put
   a message in here so that we have some information on _what_ was being asserted. We also 
   get the file and line of code in that file where the assertion happened, adding debugging.

   Note that we _don't_ get information on what `invariant_mass` actually returned here.
   
2. The return code, which we can get from `$?` immediately after executing the test
   will be something like:

   ``` bash
   echo $?
   134
   ```

   Your number might differ depending on platform, but the important point is that it is
   not "success". Programs on most systems return `0` for success, so this provides a way for
   the computer to check for failing test programs. We'll use this later.

3. **No!** We didn't actually run `test_unphysical_domain()` because `assert()` terminates
   execution immediately when an assertion fails.

   A failure is a failure, but we generally don't want to stop running if we _could_ continue
   (this is the case here). The pass/fail of other tests might offer insight into the cause
   of failure.

:::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::


## Limitations of our approach so far

Using `assert` has solved the two primary issues we identified with "smoke/by-eye" testing: the 
computer is now verifying results, both expected and exceptional, for us, and we get an 
error message and failing program if a test case fails.

However, whilst this has automated the _verification_ part, we still have to manually recompile
our test program on every change, then run the test manually, and check that it didn't fail.
Before going further with adding more tests, let's automate these steps as well.


::::::::::::::::::::::::::::::::::::: keypoints 

- Documentation and testing are symbiotic:
  - Documentation records our expections of the code's behaviour.
  - Tests encode the _verification_ of this behaviour in _test cases_.
- For C++, `assert` provides a simple mechanism to verify a calculation or exceptional condition
- Failure of an assertion results in an error message and program termination, providing a clear _test failure_ condition.


- assert(expression) aborts the program if the expression is false — silence means the test passed
- A failing assert() tells you something went wrong but not what the values were or which assertion fired
- Manual compilation of multiple test files does not scale — this friction motivates a build system
- Testing with assert() is better than not testing, but it does not scale to a real project
::::::::::::::::::::::::::::::::::::::::::::::::
