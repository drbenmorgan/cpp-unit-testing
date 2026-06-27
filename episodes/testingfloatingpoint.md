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
- Explain why exact equality is unreliable for floating point values
- Demonstrate a case where `EXPECT_EQ` fails on values that are mathematically equal
- Use `EXPECT_NEAR` with an appropriate absolute tolerance
- Write tests for invariant_mass() that correctly handle floating point results
::::::::::::::::::::::::::::::::::::::::::::::::


:::::::::::::::::::::::::::::::::::::: keypoints
- Floating point arithmetic is not exact — two calculations that are mathematically equal may not be numerically equal
- `EXPECT_EQ` is appropriate for floating point only when the value is 
- `EXPECT_NEAR(a, b, tol)` checks that `|a - b| < tol` — the tolerance should reflect the expected numerical error from the specific type of calculation, not be chosen arbitrarily.
::::::::::::::::::::::::::::::::::::::::::::::::
