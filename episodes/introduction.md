---
title: "What is unit testing?"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- How do I know my code is correct?
- What is a unit test and how does it relate to how I already think about correctness?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Connect the practice of testing to the scientific method.
- Explain what a unit test is in terms of inputs and expected results.
- Distinguish between a unit test and other forms of testing.

::::::::::::::::::::::::::::::::::::::::::::::::

## Why test?

Computers only do exactly what we tell them to, and we are fallible, so we should assume
that **mistakes, i.e. bugs, will happen**. Scientists often fall into the trap of _"I am a smart, so my code must be correct."_, but software is no different to any other piece of experimental apparatus. We wouldn't plug a new voltage sensor into a dark matter experiment and take data without **testing** it first (e.g. calibrating it), and we should follow the same procedure to _verify_ and _validate_ the behaviour of software used to produce scientific outputs:

- _Verification_: Confirm that the code implements a specific numerical technique or theoretical model correctly.
- _Validation_: Confirm that the code is "calibrated" against known existing data or independent calculations.

This is no more or less than applying the scientific method to software development, which aside from increasing confidence in our
published results, also increases the time we can spend on research rather than tracking down bugs.

::::: caution

Still don't think it's important? There have been, and continue to be, retractions of papers due to unwitting software bugs,
several of which are listed at [danielskatz/errors-due-to-research-software](https://github.com/danielskatz/errors-due-to-research-software/).

That said testing is only a _mitigation_ strategy, as we'll see.
:::::::::::::


## Types of testing

As we build up an experiment from components, so we do with software in the form of **units**: functions, classes, and data structures. For example, we might want a function to calculate the invariant mass of a particle. Within the `ccptepp-test/` directory, open up a new file in the `test/` subdirectory called `test_invariant_mass.cpp`. Let's sketch out the function as follows:

```cpp
//! \file test_invariant_mass.cpp
#include <cmath>
#include <stdexcept>

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

int main()
{
   double photon_mass = invariant_mass(100,100);
}
```

At the lowest level in code is **defensive programming**, which adds exceptions or checks into the function itself to report or handle errors, in this case unphysical inputs like `energy < 0`. 
We can try compiling this and running it from the `ccptepp-test/` directory as follows:

:::::::::::::::: spoiler
### Linux
```bash
g++ -std=c++17 test/test_invariant_mass.cpp -o test_invariant_mass
./test_invariant_mass
```
::::::::::::::::::::::::

:::::::::::::::: spoiler
### MacOS

```bash
clang++ -std=c++17 test/test_invariant_mass.cpp -o test_invariant_mass
./test_invariant_mass
```
::::::::::::::::::::::::

As written, `test_invariant_mass` is an example of a **smoke test**, as is "run and see if it breaks". As with defensive programming though, this only checks exceptional circumstances, not normal operation. This is where **unit testing** comes in, where we _verify_ that we get the return value(s) we expect for given input(s) to a specific
unit, here the `invariant_mass` function, across its domain of applicability.

Effectively unit testing is nothing more than the application of the scientific method to our code:

| Step            | Scientific Method                              | Software Unit Testing                                               |
| -----           | -----------------                              | ---------------------                                               |
| Hypothesis      | I predict this physical system behaves like X. | I expect this function to return `Y` when given `X`.                |
| Experiment      | Run a controlled trial isolating variables.    | Run an isolated test passing known values of `X` into the function. |
| Analysis        | Check if the data matches the prediction.      | Verify that the returned `Y` values matches the expected results.   |
| Iteration       | Update the theory based on anomalies.          | Fix any code bugs exposed by failing verification.                  |
| Reproducibility | Independent experiments should reach the same conclusion. | Re-running the tests should always give the same result  |

Of course, software consists of many units, so unit testing is only part of the story:

| Scientific Stage | Experimental Stage | Software Stage | Testing Type |
| ---------------- | ------------------ | -------------- | ------------ |
| Isolating Variables | Calibrating a single voltage sensor. | Testing a single C++ function with hardcoded inputs. | **Unit Testing** |
| Assembling Apparatus | Hooking voltage, light sensors to the electronics crates and ensuring data flows across the system. | Verifying that the I/O, kinematics, and histogramming code work together. | **Integration Testing** |
| Reproducibility | Modifying a detector component and verifying it still reproduces last week’s baseline calibration. | Modifying code for speed or features, then ensuring it still yields identical results in a known test. | **Regression Testing** |

::::: challenge

### Layering Tests for Defense

Imagine you are working in a major physics collaboration. Below are three critical software failures that could happen during running:

* **A:** A minus sign is changed to a plus sign in a coordinate transformation function. Now, whenever a particle travels in the negative z-direction, its position is completely wrong.
* **B:** A multi-threaded track fitting loop is optimized to run 4x faster. The code runs smoothly, but it subtly truncates the decimal precision of track parameters, shifting your final calculated Higgs mass peak by 2 GeV compared to last year's published configuration.
* **C:** Your kinematic calculator works perfectly in isolation, as does your file-reader isolation. However, when combined, the file-reader outputs data in single-precision floats, but the calculator expects double-precision arrays, causing a segmentation fault.

**Question:** Match each failure to the specific testing layer (Unit, Integration, or Regression) that acts as the primary shield to catch it *before* you submit a paper.

::::: solution

Layers provide **defense in depth**:

* **A is caught by a Unit Test:** You can catch this immediately by passing a single, known four-vector with a negative z-value into that specific function and checking the output against a hand-calculation.
* **B is caught by a Regression Test:** This requires running a benchmark "control dataset" through the new optimized code and asserting that the output physics distributions perfectly overlap with your historical baseline data.
* **C is caught by an Integration Test:** Neither individual unit test will catch this because both pieces work fine alone. You only see the type mismatch when running the components together.
::::::::::::::
:::::::::::::::

Integration tests are often the "gold standard" especially for scientific codes as these _validate_
against actual experimental data and thus much focus is put on them. However, their higher level 
nature means that if a problem is identified, triaging _where_ in the code caused this, and more importantly _why_, may involve a lot of work if there are no unit or regression tests to provide a guide. 

::::: callout
Unit and integration tests are themselves regression tests when they are written. As bugs, 
inevitably, are identified, new unit or integration tests are written to:

1. Reproduce the bug and cause a failure.
2. Provide tests that developers can use to help identify the bug and fix it.
3. These tests are retained in the codebase to avoid the regression again in the future.
::::::::::::::

This course will focus on unit testing as a foundation for testing scientific software, but don't
forget about the other types!

::::::::::::::::::::::::::::::::::::: keypoints 

- Tests check whether the observed result, from running the code, is what was expected ahead of time.
- Defensive programming embeds alarm-like systems into software, guarding against exceptional bahavior.
- Unit tests try to test the smallest pieces of code possible, usually functions and methods.
- Integration tests make sure that code units work together properly.
- Regression tests ensure that everything works the same today as it did yesterday.

::::::::::::::::::::::::::::::::::::::::::::::::
