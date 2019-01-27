#!/usr/bin/env bash

cd ./cmake-build-debug
xcrun llvm-profdata merge -o default.profdata default.profraw
xcrun llvm-cov show ./jsonlite_tests \
-instr-profile=default.profdata \
-format=html \
-output-dir=../coverage
