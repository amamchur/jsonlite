#!/usr/bin/env bash

cd ./cmake-build-debug/CMakeFiles/jsonlite_tests.dir/src

lcov --capture --directory . --output-file coverage_full.info
lcov --remove coverage_full.info --output-file coverage.info
genhtml coverage.info -o ../../../../coverage

lcov -z -d .
