
End to end example of the work flow on a large collection of images.

---

**1. Describe all images by GIST vectors.**

```
$ ./1-gist-extract.sh
{...}
Exec: [findgist images/mountain_natu756.jpg -o vectors-gist.hdf]
Load raster file: images/mountain_natu756.jpg
           image: (256P x 256L) pixels
           count: (1R x 256C) blocks
           block: (1P x 256L) pixels
0...10...20...30...40...50...60...70...80...90...100 - done.
Compute GIST descriptor
Insert HDF data in file: vectors-gist.hdf
           merge: HDF Vdata for image at row: 2687
```

**2. Compute L2 distance matrix between all vectors.**
```
$ ./2-dmatrix-calc.sh
Load HDF data from file: vectors-gist.hdf
           found: HDF Vdata: 2688 features at 960 dim
Compute L2 Distance Matrix
Save distance matrix band in file: dmatrix-random.tif
           image: (2688P x 2688L) pixels
           count: (1R x 2688C) blocks
           block: (1P x 2688L) pixels
0...10...20...30...40...50...60...70...80...90...100 - done.
           total: 26273 milliseconds
```

**3. Decompose distance matrix to manifold by MDS (using ***eigen*** or ***bh-tsne***)**
```
$ ./3-mds-cartesian.sh
Load HDF data from file: vectors-gist.hdf
           found: HDF Vdata: 2688 features at 960 dim
Compute Barnes-Hut T-SNE
Using current time as random seed...
Using no_dims = 3, perplexity = 100.000000, and theta = 0.500000
Computing input similarities...
Building tree...
 - point 0 of 2688
Input similarities computed in 26.25 seconds (sparsity = 0.166496)!
Learning embedding...
Iteration 50: error is 65.613757 (50 iterations in 6.05 seconds)
{...}
Iteration 4999: error is 1.139133 (50 iterations in 4.57 seconds)
Fitting performed in 426.09 seconds.
Create HDF data in file: vectors-gist.hdf
           fresh: HDF Vdata for MDS
           total: 464240 milliseconds
```

**4. Sement the dataset into arbitrary classes.**
```
$ ./4-segment-flann.sh
Load HDF data from file: vectors-gist.hdf
           found: HDF Vdata: 2688 features at 960 dim
Cluster vectors using FLANN
Clusters requested: 8, returning 8
Computing FLANN index of vectors.
Entering autotuning, dataset size: 8, sampleSize: 0, testSampleSize: 0, target precision: 0.999
Choosing linear, dataset too small
----------------------------------------------------
Autotuned parameters:
algorithm : 0
----------------------------------------------------
----------------------------------------------------
Search parameters:
checks : 32
eps : 0
sorted : 1
max_neighbors : -1
----------------------------------------------------
           total: 602 milliseconds
Create HDF data in file: vectors-gist.hdf
           fresh: HDF Vdata for Center Vectors

Create HDF data in file: vectors-gist.hdf
           fresh: HDF Vdata for Vector Labels
```

**5. Visualize the manifold space with the image sprites in 3D.**
```
$ 5-visualize.sh
```
