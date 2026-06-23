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

- Explain what AddressSanitizer and UBSan instrument at compile time.
- Build the test executable with sanitizer instrumentation.
- Observe a specific case that all unit tests miss
- Describe the relationship between unit testing, coverage, and sanitizers as complementary tools

::::::::::::::::::::::::::::::::::::::::::::::::


::::: discussion
## When to use other sanitizers?

Only look at ASan, when and where could we use the others.

::::: spoiler

## Spoiler!

Some examples of use

::::::::::::::

::::::::::::::::



::::::::::::::::::::::::::::::::::::: keypoints 

- Sanitizers can help identify issues that unit tests may not

::::::::::::::::::::::::::::::::::::::::::::::::
