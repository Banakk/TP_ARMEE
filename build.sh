#!/bin/bash


echo "================================CLEAN & BUILD START================================"
./clean.sh
make clean
make all
echo "================================CLEAN & BUILD END================================"

echo "================================GENERAL START================================"
./general
make clean
echo "================================GENERAL END================================"

