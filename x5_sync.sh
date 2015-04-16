#!/bin/bash

if [ $# -ne 2 ];
    then echo "usage: ./x5_sync.sh folder_to_scan x5_fiio_folder"
    exit
fi

echo "folder to scan = $1"
echo "destination folder = $2"

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
for i in $(find $1 -name "best.m3u" -o -name "all.m3u" ) ; do
    basefolder=$(dirname $i)
    artist=$(basename $basefolder)
    echo "i = $i"
    echo "Basefolder = $basefolder and artist = $artist"
    sed "s|^$1/||g" $i | rsync -h --progress -v --update --files-from=- $1 $2
    cp $basefolder/*.m3u $2/$artist
    sed --in-place '/^#EXTREM/ d' $2/$artist/*.m3u
    sed --in-place -i "s|^$basefolder/||g" $2/$artist/*.m3u
done
IFS=$SAVEIFS

