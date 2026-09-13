#!/usr/bin/env bash

path=test_float_2.txt

for line in $(cat "$path"); do
    echo $line | ./app.exe >> result.txt
done 
