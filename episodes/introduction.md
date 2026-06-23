---
title: "What is unit testing"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- Why test software?
- What is unit testing?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Understand the place of testing in a scientific workflow.
- Understand that testing has many forms.

::::::::::::::::::::::::::::::::::::::::::::::::

## Why test?

<!-- This is a lesson created via The Carpentries Workbench. It is written in
[Pandoc-flavored Markdown](https://pandoc.org/MANUAL.html) for static files and
[R Markdown][r-markdown] for dynamic files that can render code into output. 
Please refer to the [Introduction to The Carpentries 
Workbench](https://carpentries.github.io/sandpaper-docs/) for full documentation.

What you need to know is that there are three sections required for a valid
Carpentries lesson:

 1. `questions` are displayed at the beginning of the episode to prime the
    learner for the content.
 2. `objectives` are the learning objectives for an episode displayed with
    the questions.
 3. `keypoints` are displayed at the end of the episode to reinforce the
    objectives.

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: instructor

Inline instructor notes can help inform instructors of timing challenges
associated with the lessons. They appear in the "Instructor View"

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
-->

Computers only do exactly what we tell them to, and we are fallible, so we should assume
that **mistakes, i.e. bugs, will happen**. There are many techniques in software development
to mitigate against mistakes, and most importantly, to help us determine _where_ they occur
and _why_ they happened so we can fix them. **Testing** is the key methodology here, and it 
should be a seamless part of the scientific software development process, exactly analogous to experimental design and operation:

- For a new project, tests can help guide and design the software's architecture.
- Tests provide a record of how the software should work, and continue to work over time (_almost_ like calibration/reproducibility of an experiment).
- One can even write tests _before_ the code they test is written (this is called _test-driven development_).

We can see from this that outside of the core task of ensuring the scientific accuracy and
reproducibility of the software, testing also helps us in other aspects:

- Documenting requirements and expections on the code's behaviour.
- Expectations allow us to robustly refactor or modify the code (e.g. to optimize it).
- Increase confidence in the code, both for you and for its users.

## Types of test

At the most basic level are **smoke tests**, as in "run it and see if it catches on fire"!
To some extent, this also includes checking results by eye, e.g.

```cpp
#include <iostream>

/* Returns pi to N decimal places */ 
double calculate_pi(size_t number_of_decimals); // Implementation elsewhere

int main() {
   std::cout << calculate_pi(1) << std::endl; // run and check this is "3.1"
   std::cout << calculate_pi(2) << std::endl; // run and check this is "3.14"
   std::cout << calculate_pi(4) << std::endl; // ... and so on ...
   std::cout << calculate_pi(8) << std::endl;
}
```

At the lowest level in code is **defensive programming**, which adds exceptions or checks into the code itself to handle issues, e.g.

```cpp
//! \file test_divide.cpp
#include <stdexcept>

/* Return result of dividing numerator by denominator */
double divide(double numerator, double denominator)
{
   if(denominator == 0) {
       throw std::invalid_argument("Division by zero");
   }

   return numerator / denominator;
}

int main()
{
   double a = divide(4, 2); // OK
}
```

::::::::::::::::::::::::::::::::::::: callout

This file is provided in your starter project in the file `test_divide.cpp`. Try compiling it and running it as:

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

::::::::::::::::::::::::::::::::::::: challenge 

### Exposing the exception

Update the `main()` function with code to expose the exception.

:::::::::::::::::::::::: solution 

This should just require the line

```cpp
    result = divide(2,0);
```

with running giving text along the lines of

```
terminating due to uncaught exception of type std::invalid_argument: Division by zero
```

:::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::

You should see that we get the expected exception.

::::::::::::::::::::::::::::::::::::::::::::::::


We can see a couple of issues with the above techniques:

1. Defensive programming is only checking _exceptional_ circumstances, not normal running.
2. By-eye testing is checking results but with the fallible and inefficient human eye/brain.

The second issue is addressed through **automation**: if we know what the result should be, we can
get the computer to run the tests for us and compare the calculations with our expected values.
The first issue is where testing comes in, with several levels possible above defensive programming
itself:

- **Unit Tests**: these check the behaviour of code _units_ (functions,
  classes, or data structures) individually across the range of inputs/outputs and exceptional
  circumstances. 
- **Integration/Functional Tests**: these check the behaviour and outputs of a set of _units_ 
  working together. For example a detector simulation code might have units for input, geometry,
  physics processes, scoring, and output. An _integration_ test here could involve running a
  simulation of an existing experiment and comparing the simulated outputs against actual recorded
  data.
- **Regression Tests**: these guard against new bugs, or _regressions_ in the code. Unit and
  integration tests are themselves regression tests when they are written. As bugs, inevitably,
  are identified, new unit or integration tests are written to:
  1. Reproduce the bug and cause a failure.
  2. Provide tests that developers can use to help identify the bug and fix it.
  3. These tests are retained in the codebase to avoid the regression again in the future.

The key point here is that these levels provide _defense in depth_ against bugs. Integration and
functional tests are often the "gold standard" especially for scientific codes as these _validate_
against actual experimental data and thus much focus is put on them. However, their higher level 
nature means that if a problem is identified, triaging the cause may involve a lot of work if there
are no unit or regression tests to provide a guide.

This course will focus on unit testing as a foundation for testing scientific software, but don't
forget about the other types!

::::::::::::::::::::::::::::::::::::: keypoints 

- Tests check whether the observed result, from running the code, is what was expected ahead of time.
- Defensive programming embeds alarm-like systems into software, guarding against exceptional bahavior.
- Unit tests try to test the smallest pieces of code possible, usually functions and methods.
- Integration tests make sure that code units work together properly.
- Regression tests ensure that everything works the same today as it did yesterday.

::::::::::::::::::::::::::::::::::::::::::::::::
