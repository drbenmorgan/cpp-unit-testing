---
title: "Testing with static analysers"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 

- The tests all pass — so why does the program crash?
- What classes of bug are invisible to unit tests?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Explain what AddressSanitizer instrument at compile time.
- Build the test executable with sanitizer instrumentation.
- Observe a specific case that all unit tests miss
- Describe the relationship between unit testing, coverage, and sanitizers as complementary tools

::::::::::::::::::::::::::::::::::::::::::::::::



::::::::::::::::::::::::::::::::::::: keypoints 

- Unit tests check that your code does what you intended; sanitizers check for errors your intentions did not anticipate
- A test suite that is green and fully covered can still contain memory errors and undefined behaviour
- AddressSanitizer detects out-of-bounds memory access and use-after-free at runtime — errors that produce no compiler warning and may crash only rarely in production
- Sanitizers diagnose bugs that already exist; a well-chosen test prevents their reintroduction
- No single tool is sufficient — unit tests, coverage measurement, and sanitizers answer different questions and catch different bugs; together they give you the best practical assurance that your code is correct

::::::::::::::::::::::::::::::::::::::::::::::::
