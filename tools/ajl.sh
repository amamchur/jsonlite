#!/bin/bash

./amalgamate.py -c ./include.mcu.json -s .
./amalgamate.py -c ./src.mcu.json -p ./src.prologue -s .

./amalgamate.py -c ./include.all.json -s .
./amalgamate.py -c ./src.all.json -p ./src.prologue -s .
