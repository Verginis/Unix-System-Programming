#!/bin/bash

for domain in $*; do # for all domains given
    apps=0
    for file in *.out; do                        # read all .out files
        while IFS=' ' read -a line; do           # read file
            readarray -d . -t array <<<"$line"   # put url parts int an array removing "."
            if [ ${array[-1]} == $domain ]; then # if last part of the array is the same as the one given
                apps=$((apps + ${line[1]}))      # count appearances
            fi
        done <$file
    done
    echo $domain $apps
done