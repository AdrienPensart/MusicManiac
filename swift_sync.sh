#!/bin/bash

diff <(find * -type f -print | sort) <(swift list music | sort) | while read x; do
if [[ $x == \>* ]]; then
        echo "Need to delete ${x:2}"
        swift delete music "${x:2}"
    elif [[ $x == \<* ]]; then
        echo "Need to upload ${x:2}"
        swift upload music "${x:2}"
    fi
done

