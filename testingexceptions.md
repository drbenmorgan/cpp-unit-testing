---
title: "Testing exceptional behaviour"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How do I verify that my code fails in the right way?
- What should happen when a function receives invalid input?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives
- Explain the difference between testing error handling and testing normal behaviour
- Use `EXPECT_THROW` to assert that a specific exception type is raised
- Write tests for the boundary conditions of `invariant_mass()`: negative energy, unphysical mass squared.
- Explain why the choice of exception type matters and how to test for it specifically.
::::::::::::::::::::::::::::::::::::::::::::::::

## Testing error handling

Testing that our software _fails_ under the conditions we expect is just as important as testing
that it succeeds. These failure conditions are as much a part of a function's specification
as those for its success. In `invariant_mass()` for example, we've specified:

```cpp
// 1. Return invariant mass m = sqrt(E^2 - p^2) in natural units
// 2. throws std::domain_error if E < 0
// 3. throws std::domain_error if E^2 - p^2 < 0
double invariant_mass(double energy, double momentum)
```

Thus we _should_ test that (2) and (3) really do result in a thrown exceptions when given inputs
as specified. The key point here is that we are **not** testing "what goes wrong", rather "that the function does what it is supposed to do when given invalid input", in essence:

- _Normal behaviour_: provide valid input, check the return value.
  - Here, the assertion is about _what comes out_.
- _Exceptional behaviour_: provide invalid input, check that the error is triggered _and is the right type of error_. 
  - The assertion here is about what the function _refuses to do_.

This is another reason specifications for (i.e. documentation of) function behaviour is so 
important. We've made a _design choice_ to handle invalid input to `invariant_mass` by throwing
exceptions - but other programmers might make different decisions on error handling:

1. If `E < 0` or `E^2 - p^2 < 0`, return `-1.0` to indicate failure.
2. If `E < 0` return `-1.0`, if `E^2 - p^2 < 0` return `-2.0`
3. Have `bool invariant_mass(double E, double p, double& mass)`, return `false` and set `mass` to `-1.0` if `E < 0` or `E^2 - p^2 < 0`. 
4. If `E < 0`, terminate execution completely.

All of these are defining what happens on and just outside the boundaries of applicability of the function, which are often the most trouble prone parts of our codes. Thus no matter how we handle
errors here, we should always test that they _are_ handled, and as we _expect_.


## Testing for exceptions with GoogleTest

To check that a function throws an exception _and_ that the thrown exception is the correct one,
GoogleTest provides the `EXPECT_THROW` (and corresponding `ASSERT_*` form) assertion. This is
very simple, so let's use it to implement the `E<0` error case in the `UnphysicalEnergies` test
case:

```cpp
// Case 2. Test unphysical energies
TEST(InvariantMass, UnphysicalEnergies)
{
  EXPECT_THROW(invariant_mass(-1.0, 0.0), std::domain_error) << "negative input energy does not throw";
}
```

The first argument is just the expression we want to assert on, the second is the _C++ type_
of what we are asserting the expression will throw. Building and running again, we'll now see
the `UnphysicalEnergies` passes:


```bash
$ ctest --test-dir build --output-on-failure
Test project /tmp/ccptepp-test/build
    Start 1: TestInvariantMass
1/1 Test #1: TestInvariantMass ................   Passed    0.23 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.24 sec
```

Let's go on to add the case for negative mass squared but deliberately make the expected
assertion type wrong:


```cpp
// Case 2. Test unphysical energies
TEST(InvariantMass, UnphysicalEnergies)
{
   EXPECT_THROW(invariant_mass(-1.0, 0.0), std::domain_error) << "negative input energy does not throw correctly";
   EXPECT_THROW(invariant_mass(1.0, 1.1), std::runtime_error) << "negative mass-squared does not throw correctly";
}
```

Building and running now trigger an error:

```bash
...
[ RUN      ] InvariantMass.UnphysicalEnergies
/tmp/ccptepp-test/test/test_invariant_mass.cpp:29: Failure
Expected: invariant_mass(1.0, 1.1) throws an exception of type std::runtime_error.
  Actual: it throws std::domain_error with description "unphysical mass^2".
negative mass-squared does not throw correctly

[  FAILED  ] InvariantMass.UnphysicalEnergies (1 ms)
...
```

Thus we get a helpful message when an exception _is_ thrown but it is not the _right type of 
exception_. We can also confirm that the assertion will fail if the call _does not throw at all_ by
changing the assertion to:

```cpp
// right exception type, but it won't throw!
EXPECT_THROW(invariant_mass(1.1, 1.0), std::domain_error) << "negative mass-squared does not throw correctly";
```

```bash
[ RUN      ] InvariantMass.UnphysicalEnergies
/tmp/ccptepp-test/test/test_invariant_mass.cpp:29: Failure
Expected: invariant_mass(1.1, 1.0) throws an exception of type std::domain_error.
  Actual: it throws nothing.
negative mass-squared does not throw correctly

[  FAILED  ] InvariantMass.UnphysicalEnergies (0 ms)
```

::::: callout
As you might anticipate, GoogleTest also provides `EXPECT_NO_THROW`, which asserts that an 
expression does _not_ throw _any_ exception. This is most useful when a function can throw for some 
inputs and you want to explicitly document that a particular valid input is safe. It's less useful
for general. It is less useful when a test already makes assertions about the return value, since a thrown exception would cause those assertions to fail anyway.
:::::::::::::

Together, these cover all of the possible cases we'll need, but there are two small things to watch 
out for. 

First, we've chosen to throw the _same_ exception type for both error cases. This isn't 
unreasonable since they are both domain errors, but strictly speaking this means our two test cases
_aren't_ completely distinguishing the `E<0` and `E^2 - p^2 < 0` cases. We _are_ testing the
specification is met though, which the main thing here. If we did want to be specific here, we
might introduce our own exception types to distinguish both.

Second, and somewhat related, GoogleTest's check on the _type_ of the exception thrown uses C++ "is-a" inheritance rules if class types (as `std::domain_error` is) are involved. What this means
is that _if_  `invariant_mass` threw an exception, say `foo_exception`, that _inherits_ 
from `std::domain_error` in these assertions, the _test would actually pass_. We can mock this by writing:

```cpp
EXPECT_THROW(invariant_mass(1.0, 1.1), std::exception) << "negative mass-squared does not throw correctly";
```

as `std::domain_error` inherits from `std::exception`. Building and running this will show a passing
test case:


```bash
(ccptepp-test) $ ctest --test-dir build --output-on-failure
Test project /tmp/ccptepp-test/build
    Start 1: TestInvariantMass
1/1 Test #1: TestInvariantMass ................   Passed    0.23 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.24 sec
```

This is intended and semantically correct behaviour from GoogleTest - we have asked it to check 
that a `std::exception` is thrown, and `std::domain_error` "is-a" `std::exception`, so the assertion 
condition is met. There's no real way around this other than to be as specific as possible
when declaring the type you expect to be thrown, and don't have deeply nested inheritance
hierarchies for exceptions!


::::: callout
How would we test the other possible error handling mechanisms we outlined at the start of the
episode? The first three are logically handled by `EXPECT_EQ` or `ASSERT_EQ` and their variants
we've seen already. We _can_ actually test for termination with so-called "death tests". These
are rather specialised, but do have their place.
:::::::::::::


:::::::::::::::::::::::::::::::::::::: keypoints
- Testing what your code refuses to do is as important as testing what it does
- A function's error handling is part of its specification and should be documented and tested like any other behaviour.
- These often determine boundary conditions where bugs most commonly live, making them vital to test.
- `EXPECT_THROW` checks both that an exception was raised and that it was the right type — the type is part of the function's specification.
- With `invariant_mass()` now fully tested, we have seen a near complete range of GoogleTest assertion types — the remaining episodes apply these tools to more complex code.
::::::::::::::::::::::::::::::::::::::::::::::::
