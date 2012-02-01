#!/bin/sh

# Segment vectors and create centers and labels
# Use exact 8 class for segmentation
# Insert results back into HDF4 dataset

../bin/vsegment -cls 8 vectors-gist.hdf