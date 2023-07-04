#!/bin/sh

# Extract GIST vectors from random sort images.
# Insert them into the initial HDF4 database.

for i in `ls images/*.??? | sort -R`; do

echo "Exec: [findgist $i -o vectors-gist.hdf]"
../bin/findgist $i -o vectors-gist.hdf

done
