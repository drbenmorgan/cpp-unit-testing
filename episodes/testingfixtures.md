---
title: "Test fixtures"
teaching: 10 # teaching time in minutes
exercises: 2 # exercise time in minutes
---
:::::::::::::::::::::::::::::::::::::: questions 

- I am writing the same setup code in every test — is there a better way?
- How do I share a complex starting state across many tests without tests interfering with each other?

::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::: objectives

- Identify repeated setup code across tests as a signal that a fixture is appropriate
- Write a TEST_F() fixture class with a SetUp() method for Histogram
- Explain that SetUp() runs fresh before every test and that tests do not share state
- Refactor existing Histogram tests to use a fixture where appropriate
- Decide which tests belong in the fixture and which should remain as plain TEST() cases
- Explain why construction tests should remain outside the fixture

::::::::::::::::::::::::::::::::::::::::::::::::



::::::::::::::::::::::::::::::::::::: keypoints 

- A fixture eliminates repeated setup code and makes the intended starting state of each test explicit
- SetUp() runs before every individual test — each test starts from a clean, identical state regardless of what other tests do
- Fixtures do not change what is being tested, only how the starting state is prepared
- Construction tests belong outside the fixture — the fixture assumes construction succeeds and tests behaviour from that point

::::::::::::::::::::::::::::::::::::::::::::::::

