#!/usr/bin/env bash
#Result: ptsSize, polySize, function, noPtPasses, noConstraints, executeTime, ptMemTime, ptRenderTime, polyMemTime, polyRenderTime, setupTime, triangulationTime, polyIndexTime, backendQueryTime, accuracy
./RasterJoin --nIter 1 \
  --joinType raster \
  --indexRes 1024 \
  --backendIndexName /local/storage/liang/Datasets/rasterjoin-data/Aquifer_polys/points_10k_index \
  --locAttrib 2 \
  --polygonList /local/storage/liang/Datasets/rasterjoin-data/Aquifer_polys/Aquifer-polygons.txt \
  --polygonDataset 1 \
  --startTime 0 \
  --endTime 0 \
  --opAggregation aggOut