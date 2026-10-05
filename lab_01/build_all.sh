#!/usr/bin/env bash

green="\e[32m"
red="\e[31m"
nc="\e[0m"

echo "Building..."

./build_debug.sh

if [[ $? -eq 0 ]]; then
    echo -e "Debug build status: ${green}SUCCESS${nc}"
else
    echo -e "Debug build status: ${red}FAIL${nc}"
fi

./build_release.sh

if [[ $? -eq 0 ]]; then
    echo -e "Release build status: ${green}SUCCESS${nc}"
else
    echo -e "Release build status: ${red}FAIL${nc}"
fi

./build_asan.sh

if [[ $? -eq 0 ]]; then
    echo -e "Address sanitizer build status: ${green}SUCCESS${nc}"
else
    echo -e "Address sanitizer build status: ${red}FAIL${nc}"
fi

./build_msan.sh

if [[ $? -eq 0 ]]; then
    echo -e "Memory sanitizer build status: ${green}SUCCESS${nc}"
else
    echo -e "Memory sanitizer build status: ${red}FAIL${nc}"
fi

./build_ubsan.sh

if [[ $? -eq 0 ]]; then
    echo -e "Undefined behaviour sanitizer build status: ${green}SUCCESS${nc}"
else
    echo -e "Undefined behaviour sanitizer build status: ${red}FAIL${nc}"
fi

./clean.sh
