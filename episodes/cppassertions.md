---
title: "Unit testing with `assert()`"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How can we implement unit tests in C++?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

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

:::::::::::::

## How should we test?

Now that we know what the code is supposed to do, we can write unit tests to verify that it actually
does this. An extremely common method here is the Mk1 human eye:

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
      std::cout << "false" << std::endl;
   }
   catch (const std::domain_error&)
   {
      std::cout << "true" << std::endl;
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
3. The overall unit test is a program that runs all of the test cases.

If we compile and run this, then we'll get output:

```bash
# use clang++ if on macOS
g++ -std=c++17 test/test_invariant_mass.cpp -o test_invariant_mass    
./test_invariant_mass
photon mass should be zero: 0
negative energy should throw exception: true
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

#include <cassert>

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
      // nothing to do!
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
```

So with `assert` we _don't_ get output by default, though the program still ran and in
this case successfully: our tests _passed_. It also provides a cleaner way to express 
_what_ is being checked compared to our by-eye version.

::::::::::::::::::::::::::::::::::::: challenge 

## Making a failing assertion

Add an `assert` in `test_physical_domain` that you know will fail, then compile and run again.

1. What output do you get now?
2. What is the exit code of the application?

:::::::::::::::::::::::: solution 

The simplest way to do this is with a deliberately wrong answer:

```cpp
   assert(invariant_mass(100,100) == 0.1 && "what?"); // Yes this is dumb!
```

When you compile this and run you would see something like

```bash
Assertion failed: (invariant_mass(100,100) == 0.1), function test_physical_domain, file test_invariant_mass.cpp, line 27.
```

and the return code, which we can get from `$?` immediately after executing the test
will be something like:

``` bash
echo $?
134
```

The message and code will obviously differ depending on what failure you used and what platform
you are on, but the key things are that:

1. You get the "Assertion failed" message.
2. You get the line of code where this happened, and your message if you added one.
3. The return code (which you get from running `$?` immediately after executing the test) is non-zero.

:::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::


## Limitations of our approach so far

Using `assert` has solved the two primary issues we identified with "by eye/smoke" testing: the 
computer is now verifying results, both expected and exceptional, for us, and we get an 
error message and failing program if a test case fails.

However, whilst this has automated the _verification_ part, we still have to manually recompile
our test program on every change, then run the test manually, and check that it didn't fail.
Before going further with adding more tests, let's automate these steps as well.





## Limitations of using `assert`


::::::::::::::::::::::::::::::::::::: challenge 

Let's try adding another test for the physical domain. We know that $m^2 = E^2 - p^2$, so we
could test this as a $3^2 = 5^2 - 4^2$ triangle condition here.

1. Add an `assert` for this check, compile and run to confirm the test program still runs successfully.
2. We're dealing with decimal numbers here, so add another `assert` for the case $0.3^2 = 0.5^2 - 0.4^2$ and compile and run again. What do you find, and why do you think you got success or failure?

:::: solution
You should have observed the assertion fail:

```bash
./test_invariant_mass                                          
Assertion failed: (invariant_mass(0.5, 0.4) == 0.3), function test_physical_domain, file test_invariant_mass.cpp, line 28
```

We've run into our first major friction point with unit testing scientific software: _floating point numbers_. 
:::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::


::::::::::::::::::::::::::::::::::::: challenge

### What 

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

- Documentation and testing are symbiotic:
  - Documentation records our expections of the code's behaviour.
  - Tests encode the _verification_ of this behaviour in _test cases_.
- For C++, `assert` provides a simple mechanism to verify a calculation or exceptional condition
- Failure of an assertion results in an error message and program termination, providing a clear _test failure_ condition.
::::::::::::::::::::::::::::::::::::::::::::::::
