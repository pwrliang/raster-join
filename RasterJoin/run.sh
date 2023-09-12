#!/usr/bin/env bash

# prev polygonDataset 16384
./RasterJoin --nIter 1 \
  --joinType raster \
  --indexRes 1024 \
  --backendIndexName /local/storage/liang/Datasets/rasterjoin-data/Aquifer_polys/points_index \
  --locAttrib 2 \
  --polygonList /local/storage/liang/Datasets/rasterjoin-data/Aquifer_polys/Aquifer-polygons.txt \
  --polygonDataset 1 \
  --startTime 0 \
  --endTime 0
