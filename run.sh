#!/usr/bin/env bash

path=test_float.txt

for line in $(cat "$path"); do
    echo $line | ./app.exe >> result.txt
done 
