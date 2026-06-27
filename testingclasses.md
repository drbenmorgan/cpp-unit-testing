---
title: "Testing stateful classes"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---

:::::::::::::::::::::::::::::::::::::: questions 
- How do I test code that has to be constructed and populated before I can interrogate it?
- How do I verify results that are collections rather than single values?
::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Explain why a stateful class requires a different testing approach to a pure function
- Read a class header and its Doxygen comments to identify what should be tested before writing any test
- Write a suite of `TEST()` cases covering construction and filling of Histogram
- Use `EXPECT_EQ` to verify integer-valued bin counts and explain when this is appropriate
- Write a helper function to compare two `std::vector<float>` values element-wise with `EXPECT_NEAR`
- Use `EXPECT_NEAR` on the result of `mean()` and justify the choice of tolerance
::::::::::::::::::::::::::::::::::::::::::::::::


:::::::::::::::::::::::::::::::::::::: keypoints
- A stateful class is testable if its state is explicit and controlled through a well-defined interface — the difficulty arises from hidden or global state, not from state itself.
- Reading the specification before writing tests is not optional — it determines what the tests should assert makes any ambiguities obvious.
- Each test case should verify one behaviour — if a test needs "and" in its name it is probably two tests
- GoogleTest provides helpers in GMock for more complex checks.
::::::::::::::::::::::::::::::::::::::::::::::::
