---
title: "Introducing GoogleTest"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- What are the remaining limitations of assert() that prevent it scaling to a real test suite?
- How does a testing framework address those limitations?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Explain the limitations of assert() with respect to test output, boilerplate, and release builds
- Add GoogleTest to a CMake project using find_package
- Refactor an existing assert()-based test into a GoogleTest TEST() case
- Distinguish between EXPECT_* and ASSERT_* and explain when each is appropriate
- Run tests via CTest and interpret the output of a passing and a failing GoogleTest test
- Explain what additional information GoogleTest provides compared to assert() on failure

::::::::::::::::::::::::::::::::::::::::::::::::

## Limitations of using `assert`

At the end of the last episode we introduced a deliberately failing test, so first let's remove
that and get `test_invariant_mass` passing again

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

1. Whilst we know an `assert` failed, we don't directly get information on why it failed: it doesn't automatically print _what_ the result of `divide` was.
2. The "pass/fail" check _must_ be implemented as a C++ statement that should evaluate to "not 0" for "pass". Combined with point 1, this means we'll need to implement a lot of functionality for more complex checks (see for example what we have to do just to check a required exception is thrown).

::::::::::::::::::::::::::::::::::::::::::::::::



Writing tests clearly involves a lot of boilerplate coding. Since it's such a common
need there are a lot of C++ packages out there that provide all of this for us.
We’ve chosen the [GoogleTest] unit testing framework for this course because of its general
wide use (and author familiarity), but others are available, e.g.

- [Catch2] is probably the most popular after GoogleTest
- [Boost.Test](https://www.boost.org/library/latest/test/) is part of the widely used, but
  heavyweight, Boost libraries
- [doctest](https://github.com/doctest/doctest) lightweight, header only, allows "tests alongside
  code" model used in languages like Rust

All have similar concepts, so adapting what you learn here to projects that use a
different framework is generally just a dialect/terminology difference.


::::::::::::::::::::::::::::::::::::: keypoints 

- assert() gives you an abort; GoogleTest tells you which test failed, what the actual value was, and what the expected value was
- EXPECT_* continues after a failure; ASSERT_* stops the current test — use ASSERT_* when continuing would be meaningless
- GoogleTest integrates with CTest so your existing build workflow does not change
- A well-named test reads like a specification — the test name should describe the behaviour being verified, not the mechanism

::::::::::::::::::::::::::::::::::::::::::::::::
