<img src="docs/logo/logo-indexed.png" width="200"/>FeatVIS: **Feat**ures **VIS**ualiziation

### Description

FeatVIS helps visualize multidimensional features space.

GUI is written using QT + OpenSceneGraph with simplicity in mind.

## Specs

![VIEW](docs/images/featvis-view.gif)

Currently implements and use:

 * [GIST](https://people.csail.mit.edu/torralba/code/spatialenvelope/) (fourier domain) for high-dimensional description of images
 * [MDS](https://en.wikipedia.org/wiki/Multidimensional_scaling) (eigen decomposition) for embedding projections in low-dim (3D)
 * [BH-TSNE](http://lvdmaaten.github.io/publications/papers/JMLR_2014.pdf) (space partitioning) for embedding projections in low-dim (3D)
 * [FLANN](https://github.com/flann-lib/flann) (fast knn) clustering and nearest neighbouring

## Docs

  Follow the steps and explanations in the [sample](/sample) folder.
