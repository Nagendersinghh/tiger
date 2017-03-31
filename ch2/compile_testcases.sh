#!/bin/bash

for file in ../tiger/testcases/*.tig
do
	./lextest $file
done
