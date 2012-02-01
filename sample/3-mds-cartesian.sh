#!/bin/sh

# Compute MDS over vectors
# Store result in same HDF

#../bin/featmds dmatrix-random.tif -o vectors-gist.hdf
../bin/featsne vectors-gist.hdf -o vectors-gist.hdf