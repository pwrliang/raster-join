#!/usr/bin/env bash

# prev polygonDataset 16384
./RasterJoin --nIter 1 \
  --joinType raster \
  --indexRes 1024 \
  --backendIndexName /local/storage/liang/Datasets/rasterjoin-data/taxi/taxi_full_index \
  --locAttrib 1 \
  --polygonList /local/storage/liang/Datasets/rasterjoin-data/polys/nyc-polygons.txt \
  --polygonDataset 65536 \
  --startTime 1230768000 \
  --endTime 1272808000
