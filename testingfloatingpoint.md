---
title: "Floating point comparisons"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- Why does `EXPECT_EQ` fail for values I believe are correct?
- How do I test numerical code reliably?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives
- Demonstrate a case where `EXPECT_EQ` fails on values that are mathematically equal
- Explain why exact equality is unreliable for floating point values
- Use `EXPECT_DOUBLE_EQ` and `EXPECT_NEAR` with an appropriate absolute tolerance
- Write tests for invariant_mass() that correctly handle floating point results
::::::::::::::::::::::::::::::::::::::::::::::::

# Testing with floating point numbers

Now we understand how GoogleTest deals with failures, we can start expanding the range of
assertions used in the `PhysicalDomain`. Let's start with the two basic massless ones we set
up and write what we _actually_ expect `invariant_mass()` to return:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
   EXPECT_EQ(invariant_mass(10, 0.0), 10) << "at rest particle does not have correct mass";
   EXPECT_EQ(invariant_mass(100, 100), 0.0) << "massless particle not massless";
}
```

Building and running again, we should now see that `PhysicalDomain` test case passes:

```bash
...
[ RUN      ] InvariantMass.PhysicalDomain
[       OK ] InvariantMass.PhysicalDomain (0 ms)
...
```

::: callout
GoogleTest _doesn't_ print anything for passing tests. That might seem surprising, but it makes
sense: you've encoded the expectation in the test code, and repeated it here would be superfluous.
In addition, it would clutter the output with passing test info when we want to see _failures_.

The one exception here is if you're debugging a test that's passing when you expect it to fail.
One technique here is to reverse the logic of the test, but don't forget to re-reverse it for production! You could also just use good old "debug by `std::cout`".

Unfortunately, there's no easy way to get GoogleTest to be fully verbose here.
:::::::::::

These are trivial cases, so let's think about assertions for a particle with mass in motion.
Since $E^2 = p^2 + m^2$ is Pythagoras's rule, we could also use this:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
   EXPECT_EQ(invariant_mass(10, 0.0), 10) << "at rest particle does not have correct mass";
   EXPECT_EQ(invariant_mass(100, 100), 0.0) << "massless particle not massless";

   EXPECT_EQ(invariant_mass(5, 3), 4) << "off mass shell";
   EXPECT_EQ(invariant_mass(5, 4), 3) << "off mass shell";
}
```

Building and running again, these new assertions pass:

```bash
...
[ RUN      ] InvariantMass.PhysicalDomain
[       OK ] InvariantMass.PhysicalDomain (0 ms)
...
```

Since we're dealing with floating point numbers, let's add the same assertion rule but with
the inputs a factor of ten smaller:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
   EXPECT_EQ(invariant_mass(10, 0.0), 10) << "at rest particle does not have correct mass";
   EXPECT_EQ(invariant_mass(100, 100), 0.0) << "massless particle not massless";

   EXPECT_EQ(invariant_mass(5, 3), 4) << "off mass shell";
   EXPECT_EQ(invariant_mass(5, 4), 3) << "off mass shell";

   EXPECT_EQ(invariant_mass(0.5, 0.3), 0.4) << "off mass shell";
   EXPECT_EQ(invariant_mass(0.5, 0.4), 0.3) << "off mass shell";
}
```

If we build and run this now however, we get what may be a slightly surprising failure:

```bash
...
[ RUN      ] InvariantMass.PhysicalDomain
/Users/benmorgan/tmp/pix/ccptepp-test/test/test_invariant_mass.cpp:17: Failure
Expected equality of these values:
  invariant_mass(0.5, 0.4)
    Which is: 0.29999999999999993
  0.3
off mass shell

[  FAILED  ] InvariantMass.PhysicalDomain (0 ms)
...
```

We have run into one of the main issues with floating point operations and arithmetic - 
**they are not exact**. Mathematically $0.3 = \sqrt (0.5^2 - 0.4^2)$, but the implementation
of `sqrt` doesn't algebraically/symbolically calculate the result. Even basic operations can
produce results we can't compare _exactly_. To illustrate this, trying adding the following to
`PhysicalDomain`:

```cpp
  EXPECT_EQ(invariant_mass(0.5, 0.4), 0.3) << "off mass shell";

  EXPECT_EQ(0.1+0.2, 0.3) << "summation not exact";
  EXPECT_EQ(0.1+0.2, 0.2+0.1) << "not commutative";
  EXPECT_EQ((0.1+0.3) + 0.2, 0.1 + (0.3+0.2)) << "not associative";
```

Compiling and running, we'll see:

```cpp
[ RUN      ] InvariantMass.PhysicalDomain
/Users/benmorgan/tmp/pix/ccptepp-test/test/test_invariant_mass.cpp:17: Failure
Expected equality of these values:
  invariant_mass(0.5, 0.4)
    Which is: 0.29999999999999993
  0.3
off mass shell

/Users/benmorgan/tmp/pix/ccptepp-test/test/test_invariant_mass.cpp:19: Failure
Expected equality of these values:
  0.1+0.2
    Which is: 0.30000000000000004
  0.3
not exact

/Users/benmorgan/tmp/pix/ccptepp-test/test/test_invariant_mass.cpp:21: Failure
Expected equality of these values:
  (0.1+0.3)+0.2
    Which is: 0.60000000000000009
  0.1+(0.3+0.2)
    Which is: 0.6
not associative

[  FAILED  ] InvariantMass.PhysicalDomain (0 ms)
```

In general, floating point arithmatic is _commutative_ but _not associative_, and we can't
guarantee _mathematically exact_ equality between two expressions. We got away with this 
in our earlier tests because we were lucky that the numbers chosen are exactly representable and that an algorithm like `sqrt` can get to this exact representation.

Thankfully, GoogleTest provides some specialized assertions to handle floating point comparisons.

::: callout
Floating point representation is a _huge_ topic. The gory details are provided in the
[IEEE754](https://en.wikipedia.org/wiki/IEEE_754) specification, which modern hardware implements.
:::::::::::


## Comparing floating point numbers in GoogleTest

If we look at the actual number returned by `invariant_mass(0.5,0.4)` and `0.1+0.2` reported
by our assertions, we see that the former is slightly less than `0.3` and the later slightly
larger. As you might guess from earlier discussion, floating point numbers are not _continuous_
like real numbers. Rather, adjacent floating point numbers are separated by a _distance_ called
the [_Unit in the Last Place_ (sometimes _Unit of Least Precision_) or "ULP"](https://en.wikipedia.org/wiki/Unit_in_the_last_place). What we're
seeing in the two calculations of "0.3" is _correctness to within a given number of ULPs_.

:::: callout
IEEE754 specifies rounding rules for arithmetic operations and `sqrt` that they be within 0.5ULP
of the mathematically exact result. Good numeric libraries will calculate the transcendental
functions to 0.5-1ULP of the mathematically exact result.
::::::::::::

In GoogleTest, the basic assertion for comparing two `double` floating point numbers is
`EXPECT_DOUBLE_EQ(a, b)` (which naturally has an `ASSERT_*` version). For `float`, the corresponding `EXPECT_FLOAT_EQ` **must** be used because the comparisons are fundamentally different.
Both of these check that `a` and `b` are within 4ULPs of each other, failing of this condition
is not met. Rewriting `PhysicalDomain` to use this is very simple:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
  EXPECT_DOUBLE_EQ(invariant_mass(10, 0.0), 10) << "at rest particle does not have correct mass";
  EXPECT_DOUBLE_EQ(invariant_mass(100, 100), 0.0) << "massless particle not massless";

  EXPECT_DOUBLE_EQ(invariant_mass(5, 3), 4) << "off mass shell";
  EXPECT_DOUBLE_EQ(invariant_mass(5, 4), 3) << "off mass shell";

  EXPECT_DOUBLE_EQ(invariant_mass(0.5, 0.3), 0.4) << "off mass shell";
  EXPECT_DOUBLE_EQ(invariant_mass(0.5, 0.4), 0.3) << "off mass shell";

  EXPECT_DOUBLE_EQ(0.1+0.2, 0.3) << "not exact";
  EXPECT_DOUBLE_EQ(0.7-0.4, 0.3) << "not exact";
  EXPECT_DOUBLE_EQ(0.1+0.2, 0.2+0.1) << "not commutative";
  EXPECT_DOUBLE_EQ((0.1+0.3)+0.2, 0.1+(0.3+0.2)) << "not associative";
}
```

which we should now see results in a clean pass when rebuilding and running:

```cpp
[ RUN      ] InvariantMass.PhysicalDomain
[       OK ] InvariantMass.PhysicalDomain (0 ms)
```

::::: callout
You might think 4ULPs is a bit wide when we stated IEE754 requires 0.5-1ULP. However, the
IEEE754 requirement is on single operations only. 4ULPs can actually be quite tight when
we have multiple operations, as we do in `invariant_mass`.
:::::::::::::

For this type of simple, mostly mathematical check, `EXPECT_DOUBLE_EQ` suffices. 
However, the numerical methods used in scientific codes are generally more complex, involving
multiple arithmetic operations and transcendental function calls. The use of multithreading/multiprocess introduces the additional complication of _ordering_ of calculations, so repeated
runs might not produce a binary or numerically identical number. 4ULPs is quite a tight bound,
so if the number we write to compare the result against is an experimental result or a numerical calculation only know to `N` significant digits, we could quite easily get a failing test for
a perfectly valid calculation.

Here, it is better to specify the exact _tolerance_ we want between the calculation and expected
result, and we can use the `EXPECT_NEAR(a, b, tol)` (or `ASSERT_NEAR`) assertion for this.
This is general purpose (one can use it for integers as well) and checks that `|a - b| < tol`, 
failing if this condition is not met. For example, say we want to use some arbitrary numbers
to test `invariant_mass`:

```cpp
// Case 1. Test physical domain
TEST(InvariantMass, PhysicalDomain)
{
  ...
  EXPECT_NEAR(invariant_mass(54.9, 3.14), 54.8, 0.1) << "incorrect mass calculation";
}
```

Now $\sqrt(54.9^2 - 3.14^2) = 54.81013045049245$ so why not use that and `EXPECT_DOUBLE_EQ`?
Ultimately, the result can be no more precise than its least precise input. Our inputs are 3 significant figures, so we should not expect more than 3 significant figures in the result — which for a value of order 54 corresponds to a absolute tolerance of about 0.1.

Choosing a reasonable tolerance in realistic cases is dependent on both the precision of the
inputs _and_ the form of the calculation. Ultimately, what we are doing is applying the 
error propagation we do in data analysis to our algorithms, accounting for the nature of
floating point operations. This is a large topic itself, which we defer to other material. 

One final question we might have is have we covered the entire range of numbers possible?
This is a hard question as well and more towards the very difficult task of formal verification.
Covering a representative range of typical inputs, plus the boundaries and error conditions we will 
address next, will give a practically useful test suite — even if formal exhaustive verification is 
out of reach.

:::::::::::::::::::::::::::::::::::::: keypoints
- Floating point arithmetic is not exact — two calculations that are mathematically equal may not be numerically equal.
- `EXPECT_EQ` is appropriate for floating point only when the value is exactly representable.
- `EXPECT_DOUBLE_EQ` and `EXPECT_FLOAT_EQ` check that two floating point numbers are within 4 ULPs of each other.
- `EXPECT_NEAR(a, b, tol)` checks that `|a - b| < tol` — the tolerance should reflect the expected numerical error from the specific type of calculation, not be chosen arbitrarily.
::::::::::::::::::::::::::::::::::::::::::::::::
