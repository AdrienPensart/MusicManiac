#!/bin/bash

if [ $# -ne 2 ];
    then echo "usage: ./sync.sh folder_to_scan destination_folder"
    exit
fi

echo "folder to scan = $1"
echo "destination folder = $2"

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
for i in $(find $1 -name "best.m3u" -o -name "all.m3u" ) ; do
    basefolder=$(dirname $i)
    artist=$(basename $basefolder)
    destination=$2/$artist
    echo "i = $i"
    echo "Basefolder = $basefolder and artist = $artist"
    echo "Destination = $destination"
    rsync -h --progress -v --update --files-from=$i $1 $2
done
IFS=$SAVEIFS

