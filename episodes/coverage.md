---
title: "Code coverage"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- How do I know which parts of my code my tests actually exercise?
- What does test coverage tell me, and what doesn't it tell me?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Build a project with `gcov/lcov` instrumentation .
- Run gcovr and interpret line and branch coverage reports
- Identify at least one untested branch in the Histogram implementation from the coverage report
- Write a test that increases branch coverage and verify the improvement in the report

- Explain the difference between line coverage and branch coverage
- Describe at least one class of bug that 100% line coverage would not catch

::::::::::::::::::::::::::::::::::::::::::::::::




::::::::::::::::::::::::::::::::::::: keypoints 

- Coverage measures which lines and branches were executed during testing — not whether they were tested correctly
- A line shown as covered means it ran; it does not mean the result was checked or that the test would catch a bug there
- Branch coverage is more informative than line coverage — a line can execute without all its branches being taken
- Coverage is a lower bound on thoroughness, not an upper bound — 100% coverage is necessary but not sufficient
- The coverage report is most useful as a guide to where tests are missing, not as a measure of test quality
- Beware of diminishing returns

::::::::::::::::::::::::::::::::::::::::::::::::
