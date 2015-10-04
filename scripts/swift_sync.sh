#!/bin/bash

cd $2
diff <(find * -type f -print | sort) <(swift list $1 | sort) | while read x; do
if [[ $x == \>* ]]; then
        echo "Need to delete ${x:2}"
        swift delete $1 "${x:2}"
    elif [[ $x == \<* ]]; then
        echo "Need to upload ${x:2}"
        swift upload $1 "${x:2}"
    fi
done
cd -

