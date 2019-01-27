#!/usr/bin/env bash

cd ./cmake-build-debug/CMakeFiles/jsonlite_tests.dir/src

lcov --capture --directory . --output-file coverage_full.info --rc lcov_branch_coverage=1
lcov --remove coverage_full.info --output-file coverage.info --rc lcov_branch_coverage=1
genhtml coverage.info -o ../../../../coverage --rc lcov_branch_coverage=1

lcov -z -d .
