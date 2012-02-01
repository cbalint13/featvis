#!/bin/sh

# Extract GIST vectors from random sort images.
# Insert them into the initial HDF4 database.

## BEWARE to make sure you are in same working dir with the images !

cd images/
for i in `ls * | sort -R`; do

echo "Exec: [findgist $i -o ../vectors-gist.hdf]"
../../bin/findgist $i -o ../vectors-gist.hdf

done
cd ..
