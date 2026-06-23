---
title: "Measuring unit test coverage"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How do I know which parts of my code my tests actually exercise?
- What does test coverage tell me, and what doesn't it tell me?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Build the project with `gcov/lcov` instrumentation .
- Use `gcovr` to generate a coverage report from the outputs.
- Identify at least one untested branch in the `Histogram` implementation
- Write a test that increases branch coverage, and verify the improvement

::::::::::::::::::::::::::::::::::::::::::::::::




::::::::::::::::::::::::::::::::::::: keypoints 

- Coverage can help you identify untested regions of your code.
- It does not guarantee that the ran code is correct.
- Beware of diminishing returns.

::::::::::::::::::::::::::::::::::::::::::::::::
