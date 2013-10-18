#!/bin/bash

./amalgamate.py -c ./include.json -s .
./amalgamate.py -c ./src.json -p ./src.prologue -s .