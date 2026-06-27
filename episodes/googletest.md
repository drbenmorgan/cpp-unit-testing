---
title: "Introducing GoogleTest"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- What are the remaining limitations of `assert()` that prevent it scaling to a real test suite?
- How does a testing framework address those limitations?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Explain the limitations of `assert()` with respect to test output and boilerplate, and why we should use a dedicated testing framework.
- Add GoogleTest to a CMake project using `find_package`.
- Refactor an existing `assert()`-based test into a GoogleTest `TEST()` case.
- Run tests via CTest and interpret the output of a passing and a failing GoogleTest test.
- Explain what additional information GoogleTest provides compared to `assert()` on failure.
- Distinguish between `EXPECT_*` and `ASSERT_*` and explain when each is appropriate.

::::::::::::::::::::::::::::::::::::::::::::::::

## Limitations of using `assert`

Whilst we've got a decent unit test for `invariant_mass` working using `assert`, we've already
run into a couple of friction points:

1. We get a report on which assertion failed but not _why_. For example, an assertion might
   print
  
   ```bash
   Assertion failed: ((invariant_mass(100,100) == 0.0) && "photon mass should be 0")
   ```

   but we don't _actually_ know what `invariant_mass(100,100)` returned here, so we don't have
   much to go on to solve the issue.
2. When we get a failure the program immediately terminates and no further tests run - which might
   give us additional information to solve the issue.

::::: challenge
1. How could you write a unit test using `assert()` to get more information printed when a failure occurs?
2. How would this scale as more tests are added?

::::: solution

1. As we know that `expression` in `assert(expression)` should evaluate to `false` for a failing 
   test, we _could_ wrap the actual test in another function:

   ```cpp
   bool expect_invariant_mass(double energy, double momentum, double expectation)
   {
      double res = invariant_mass(energy, momentum);
      if (res != expectation) {
        std::cerr << "error: expected invariant_mass(" << energy << ", " << momentum << ") == " << expectation << " but got " << res << std::endl;
        return false; 
      }

      return true;
   }

   void test_physical_domain()
   {
      assert(expect_invariant_mass(100, 100, 0.0) && "mass of photon is not 0");
   }
   ```

2. **It wouldn't scale**, for several reasons.
   - It's locked to one function, so we'd need a new function for every unit with the same logic
     but different arguments and internal calls and output.
   - We'd also need more functions to express other expectations like "less than" "not equal to"
   - In the spirit of testing, how do you _test_ `expect_invariant_mass` itself?
   
The bottom line is that `assert` is good as a defensive programming tool but for anything beyond
trivial unit tests we quickly run into the need more code to handle these cases. What we actually
need is a _unit testing framework_.

::::: caution
You may find some scientific codes that have basically ended up implementing such a framework
themselves. This almost universally a _bad_ practice and is better served by using a professional
framwork.
:::::::::::::
::::::::::::::
:::::::::::::::

## Unit testing frameworks: GoogleTest

Writing tests clearly involves a lot of boilerplate coding. Since it's such a common
need there are a lot of C++ packages out there that provide all of this for us. Not only
does this free us from having to write this so we can focus on the actual task of testing,
we don't have to worry about the "testing code to test the testing code" recursion loop.

::::: caution
This obviously implies that we should choose a well maintained, widely trusted framework!
Even testing frameworks can have bugs of course - this is why we talk about mitigation not
solution.
:::::::::::::

We’ve chosen the [GoogleTest](https://google.github.io/googletest/) unit testing framework for this lesson because of its general
wide use and well maintained nature (it is also one of the most commonly used for C++ in
scientific software), but others are available, e.g.

- [Catch2] is probably the most popular after GoogleTest
- [Boost.Test](https://www.boost.org/library/latest/test/) is part of the widely used, but
  heavyweight, Boost libraries
- [doctest](https://github.com/doctest/doctest) lightweight, header only, allows "tests alongside
  code" model used in languages like Rust

All have similar concepts, so adapting what you learn here to projects that use a
different framework is generally just a dialect/terminology difference. Throughout this episode
and subsequent ones, have the [GoogleTest Docs](https://google.github.io/googletest/primer.html)
open in case you want to look up anythin in more detail.

::::: checklist
1. [The GoogleTest Primer](https://google.github.io/googletest/primer.html) gives an excellent high level overview.
2. [The Testing Reference](https://google.github.io/googletest/reference/testing.html) covers test cases and test suites.
3. [The Assertions Reference](https://google.github.io/googletest/reference/assertions.html) covers the testing assertions that we use in the test cases.
:::::::::::::::

To get familiar with GoogleTest, we'll start by reimplementing our unit tests for `invariant_mass`.
GoogleTest comes as a header and library, just as we have built for `ccptepp`, and we have 
preinstalled it in the `pixi` development environment. Let's start by updating our `CMakeLists.txt`
file as follows:

```cmake
# ...

# - Find GoogleTest or fail
find_package(GTest REQUIRED)

# - Build test_invariant_mass
add_executable(test_invariant_mass test/test_invariant_mass.cpp)
target_link_libraries(test_invariant_mass ccptepp GTest::gtest_main)

...
```

::::: checklist
### What has changed?
1. We use CMake's [`find_package`](https://cmake.org/cmake/help/v4.2/command/find_package.html) command to find GoogleTest's header and library and make them available to the build.
2. GoogleTest names their package `GTest` for some reason!
3. We use `REQUIRED` so that CMake will exit with an error if it cannot find GoogleTest.
4. We link the GoogleTest _library_ to `test_invariant_mass` so this can use the header and library.
5. The odd `::` is a CMake convention to distinguish _imported libraries_ (i.e. those from outside
   the project) form those the project builds itself.
:::::::::::::::

Before we can build, we need to update `test_invariant_mass.cpp` to use GoogleTest. Open up
this file and modify it to:

```cpp
//! \file test_invariant_mass.cpp
#include "invariant_mass.hpp"

#include <gtest/gtest.h>
```

Yes, that's really all there is to it! GoogleTest actually provides a default `main()` program
for us that handles registering and running _test cases_ which we'll see how to write in next.
First let's just confirm we can still configure, build and run as follows:

```bash
(ccptepp-test) $ cmake --build build
```

We should see output similar to:

```bash
[0/1] Re-running CMake...
-- Found GTest: /tmp/ccptepp-test/.pixi/envs/default/lib/cmake/GTest/GTestConfig.cmake (found version "1.17.0")
-- Configuring done (0.5s)
-- Generating done (0.0s)
-- Build files have been written to: /tmp/ccptepp-test/build
[3/3] Linking CXX executable test_invariant_mass
```

So we can see that CMake has found the GTest (GoogleTest) package and has recompiled and relinked
it against this.

::::: challenge
1. Run `ctest --test-dir build`. What do you see?
2. Run again with the `-V` flag this time. What do you notice?

::::: solution

1. You should get output similar to:

   ```bash
   (ccptepp-test) $ ctest --test-dir build            
   Test project /tmp/ccptepp-test/build
       Start 1: TestInvariantMass
   1/1 Test #1: TestInvariantMass ................   Passed    0.15 sec

   100% tests passed, 0 tests failed out of 1

   Total Test time (real) =   0.15 sec
   ```

   The test has actually passed, which isn't really what we want when we start developing.

2. You should get output similar to:

   ``` bash
   (ccptepp-test) $ ctest --test-dir build -V
   UpdateCTestConfiguration  from :/tmp/ccptepp-test/build/DartConfiguration.tcl
   Test project /tmp/ccptepp-test/build
   Constructing a list of tests
   Done constructing a list of tests
   Updating test list for fixtures
   Added 0 tests to meet fixture requirements
   Checking test dependency graph...
   Checking test dependency graph end
   test 1
       Start 1: TestInvariantMass

   1: Test command: /tmp/ccptepp-test/build/test_invariant_mass
   1: Working Directory: /tmp/ccptepp-test/build
   1: Test timeout computed to be: 10000000
   1: Running main() from /Users/runner/miniforge3/conda-bld/gtest-split_1748319995326/work/googletest/src/gtest_main.cc
   1: This test program does NOT link in any test case. Please make sure this is intended.
   1: [==========] Running 0 tests from 0 test suites.
   1: [==========] 0 tests from 0 test suites ran. (0 ms total)
   1: [  PASSED  ] 0 tests.
   1/1 Test #1: TestInvariantMass ................   Passed    0.01 sec

   100% tests passed, 0 tests failed out of 1

   Total Test time (real) =   0.01 sec
   ```

   Here's where `-V` comes in useful - we can see that things run and GoogleTest is actually
   giving us some output. That's telling us, not surprisingly that we don't have any tests implemented yet.
::::::::::::::
:::::::::::::::

## Using `TEST` for test cases

We now need to start reimplementing the test cases we had before. GoogleTest uses [_C++ macros_](https://en.cppreference.com/cpp/preprocessor/replace) to define _test cases_ (like our functions
before) within _test suites_. Open up `test_invariant_mass.cpp` and add the following lines:

```cpp
//! \file test_invariant_mass.cpp
#include "invariant_mass.hpp"

#include <gtest/gtest.h>

// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
  FAIL() << "Not implemented yet";
}

// Case 2. Test unphysical energies
TEST(InvariantMass, UnphysicalEnergy)
{
  FAIL() << "Not implemented yet";
}
```

Here we've basically replicated the structure we had originally. We use the `TEST` macro to
declare each test case, with the arguments being the name of the test suite and the name of
the test case respectively.

::::: caution
These names must be valid [C/C++ identifiers](https://en.cppreference.com/cpp/language/identifiers),
but **cannot** include underscores.
:::::::::::::

What the suite name does won't become obvious until we look at _test fixtures_ in a later episode. 
Whilst `TEST` is a macro, for all intents and purposes you can wriet everything between the curly braces as you would a normal function.  

We've implemented both cases using just the `FAIL()` assertion. This, and other assertions, are
also macros, but again behave for all intents and purposes like normal functions. `FAIL()` will
explicitly fail the test case, and we follow it with the `<<` streaming operator to output a custom
failure message.

::::: challenge

Use CMake and CTest to build and run the updated test. Use `--output-on-failure` to see the failure
messages.

1. What is the same as when we used `assert()` to trigger a failure?
2. What is different?

::::: solution

You should get output similar to:

```bash
(ccptepp-test) [macbook]$ ctest --test-dir build --output-on-failure
Test project /tmp/ccptepp-test/build
    Start 1: TestInvariantMass
1/1 Test #1: TestInvariantMass ................***Failed    0.01 sec
Running main() from /Users/runner/miniforge3/conda-bld/gtest-split_1748319995326/work/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from InvariantMass
[ RUN      ] InvariantMass.PhysicalDomain
/tmp/ccptepp-test/test/test_invariant_mass.cpp:10: Failure
Failed
Not implemented yet

[  FAILED  ] InvariantMass.PhysicalDomain (0 ms)
[ RUN      ] InvariantMass.UnphysicalEnergy
/tmp/ccptepp-test/test/test_invariant_mass.cpp:16: Failure
Failed
Not implemented yet

[  FAILED  ] InvariantMass.UnphysicalEnergy (0 ms)
[----------] 2 tests from InvariantMass (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] InvariantMass.PhysicalDomain
[  FAILED  ] InvariantMass.UnphysicalEnergy

 2 FAILED TESTS


0% tests passed, 1 tests failed out of 1

Total Test time (real) =   0.01 sec

The following tests FAILED:
	  1 - TestInvariantMass (Failed)
Errors while running CTest
```

1. We still get an error message that points to the line in `test_invariant_mass.cpp` where the
   failure happened, and our custom error message.
2. **Both test cases ran**: the failure of one didn't prevent the other from running!

This is great - we can ensure that all tests run even if one fails.
::::::::::::::
:::::::::::::::

It's good practice to start writing test cases with `FAIL()` as a marker/reminder to implement
them later.

## `ASSERT_EQ` and `EXPECT_EQ` for checking

Let's start reimplementing the `PhysicalDomain` test case use the `ASSERT_EQ` macro to see what happens when we put in a deliberately failing test. Open up `test_invariant_mass.cpp` and
update this test case with the following lines:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
  ASSERT_EQ(invariant_mass(10, 0.0), 10.1) << "at rest particle does not have correct mass";
  ASSERT_EQ(invariant_mass(100, 100), 0.1) << "massless particle not massless";
}
```

`ASSERT_EQ` is the closest thing is GoogleTest to the raw `assert()` we used earlier, and
we can see the arguments largely map, but we don't have write the conditional or messaging
ourself. This is basically what we tried to do in the first challenge - but GoogleTest is
doing it better and more generally than we could. 

::::: challenge

### How does `ASSERT_EQ` affect which tests and assertions run?

Rebuild and retest with `--output-on-failure`. 

1. What extra information do we now have compared to `assert`?
2. Have both assertions been tested?

::::: solution

1. After rebuilding and retesting, you should get output similar to:

   ```bash
   (ccptepp-test) [macbook]$ ctest --test-dir build --output-on-failure
   Test project /tmp/ccptepp-test/build
       Start 1: TestInvariantMass
   1/1 Test #1: TestInvariantMass ................***Failed    0.15 sec
   Running main() from /Users/runner/miniforge3/conda-bld/gtest-split_1748319995326/work/googletest/src/gtest_main.cc
   [==========] Running 2 tests from 1 test suite.
   [----------] Global test environment set-up.
   [----------] 2 tests from InvariantMass
   [ RUN      ] InvariantMass.PhysicalDomain
   /tmp/ccptepp-test/test/test_invariant_mass.cpp:10: Failure
   Expected equality of these values:
     invariant_mass(10, 0.0)
       Which is: 10
     10.1
   at rest particle does not have correct mass

   [  FAILED  ] InvariantMass.PhysicalDomain (0 ms)
   [ RUN      ] InvariantMass.UnphysicalEnergy
   /tmp//ccptepp-test/test/test_invariant_mass.cpp:17: Failure
   Failed
   Not implemented yet

   [  FAILED  ] InvariantMass.UnphysicalEnergy (0 ms)
   [----------] 2 tests from InvariantMass (0 ms total)

   [----------] Global test environment tear-down
   [==========] 2 tests from 1 test suite ran. (0 ms total)
   [  PASSED  ] 0 tests.
   [  FAILED  ] 2 tests, listed below:
   [  FAILED  ] InvariantMass.PhysicalDomain
   [  FAILED  ] InvariantMass.UnphysicalEnergy

   2 FAILED TESTS


   0% tests passed, 1 tests failed out of 1

   Total Test time (real) =   0.16 sec

   The following tests FAILED:
	    1 - TestInvariantMass (Failed)
   ```
   
   We now have the _result_ of the call to `invariant_mass` printed as well as what we were
   comparing it to, and our custom error message.

2. **No**. We would have expected the second assertion to have reported failure as well if it had run.

   In this sense `ASSERT_EQ` in GoogleTest behave like `assert` in that they _stop execution
   of the current test case_. It _doesn't_ prevent other tests cases like `UnphysicalEnergy` from
   executing.
::::::::::
::::::::::

::::: challenge

## Comparing with use of `EXPECT_EQ`

1. Change `ASSERT_EQ` to `EXPECT_EQ` in the `PhysicalDomain` test case, then rebuild and retest with `--output-on-failure`.
2. What do you notice as different before?

::::: solution

1. After building and retesting, the output should now contain:

   ```bash
   [----------] 2 tests from InvariantMass
   [ RUN      ] InvariantMass.PhysicalDomain
   /tmp/ccptepp-test/test/test_invariant_mass.cpp:10: Failure
   Expected equality of these values:
     invariant_mass(10, 0.0)
       Which is: 10
     10.1
   at rest particle does not have correct mass

   /tmp/ccptepp-test/test/test_invariant_mass.cpp:11: Failure
   Expected equality of these values:
     invariant_mass(100, 100)
       Which is: 0
     0.1
   massless particle not massless

   [  FAILED  ] InvariantMass.PhysicalDomain (0 ms)
   ```

2. Both assertions now ran, and both reported the failure log. The `UnphysicalEnergy` case still
   ran as before.

::::::::::
::::::::::

Most GoogleTest assertions come in `EXPECT_*` and `ASSERT_*` forms. Both still "fail" if what they
are asserting doesn't happen, but only the later stops the _test case_ from continuing, not the
whole _test suite_. In the case of `invariant_mass` we should use `EXPECT_*` because the
success of an assertion does not impact that of any later ones - by running these as well, we
get more data points that could help us locate the source of a bug.

We'd use `ASSERT_*` if subsequent assertions would be meaningless or impossible to run. 
The classic use case here is if we were testing memory management:

```cpp
TEST(ParticleMaker, HasCorrectProperties)
{
   Particle* p = make_particle("electron");
   ASSERT_NOT_EQ(p, nullptr) << "got a nullptr!";

   EXPECT_EQ(p->GetName(), "electron") << "incorrect name";
}
```

It would be pointless to run the `EXPECT_EQ` assertion as we know that trying
this with a null pointer would result in a crash. Other cases can require a little
more thought to decide when `EXPECT_*` or `ASSERT_*` is appropriate, but in general
starting with `EXPECT_*` is the right choice.

::::::::::::::::::::::::::::::::::::: keypoints 

- assert() gives you an abort; GoogleTest tells you which test failed, what the actual value was, and what the expected value was
- GoogleTest integrates with CMake/CTest so your existing build workflow does not change
- A failing `TEST` does not prevent further `TEST`s from running.
- `EXPECT_*` continues after a failure; `ASSERT_*` stops the current `TEST` — use `ASSERT_*` when continuing would be meaningless.

::::::::::::::::::::::::::::::::::::::::::::::::
