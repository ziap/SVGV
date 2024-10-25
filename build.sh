#!/bin/sh -xe

g++ src/*.cpp -o main -O3 -march=native -Wall -Wextra -pedantic -std=c++20
