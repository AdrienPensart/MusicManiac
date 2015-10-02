#!/bin/bash

if [ $# -ne 3 ];
    then echo "usage: ./x5_sync.sh folder_to_scan dst_folder base_folder"
    exit
fi

echo "folder to scan = $1"
echo "destination folder = $2"
echo "base folder = $3"

rsync --prune-empty-dirs -Prtvu --delete --append --inplace --size-only $1/ $2/

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
for i in $(find $2 -name "*.m3u") ; do
    basefolder=$(dirname $i)
    artist=$(basename $basefolder)
    echo "basefolder $basefolder"
    echo "replace $3/$artist/ in $i"
    sed --in-place "/^#EXTREM/ d" $i
    sed --in-place -i "s|^$3/$artist/||g" $i
done
IFS=$SAVEIFS

# use a temp directory for cleaning deleted files
# rm -rf $temp
# rsync -r --link-dest=$dst --files-from=filelist.txt user@server:$source/ $temp
# rsync -ra --delete --link-dest=$temp $temp/ $dest
