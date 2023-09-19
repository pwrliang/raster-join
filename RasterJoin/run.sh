#!/usr/bin/env bash
#Result: ptsSize, polySize, function, noPtPasses, noConstraints, executeTime, ptMemTime, ptRenderTime, polyMemTime, polyRenderTime, setupTime, triangulationTime, polyIndexTime, backendQueryTime, accuracy
#./RasterJoin --nIter 1 \
#  --joinType raster \
#  --indexRes 1024 \
#  --backendIndexName /local/storage/liang/Datasets/rasterjoin-data/Aquifer_polys/points_10k_index \
#  --locAttrib 2 \
#  --polygonList /local/storage/liang/Downloads/Datasets/rasterjoin_polygon/USAZIPCodeArea/polygons.txt \
#  --polygonDataset 1 \
#  --startTime 0 \
#  --endTime 0 \
#  --opAggregation aggOut

./RasterJoin --nIter 1 \
  --joinType raster \
  --indexRes 1024 \
  --backendIndexName /local/storage/liang/Downloads/Datasets/rasterjoin_backendindex/parks/North_America/parks \
  --locAttrib 2 \
  --polygonList /local/storage/liang/Downloads/Datasets/rasterjoin_polygon/lakes/North_America/polygons.txt \
  --polygonDataset 1 \
  --startTime 0 \
  --endTime 0 \
  --opAggregation /tmp


#gdb --args ./RasterJoin --nIter 1 \
#  --joinType raster \
#  --indexRes 1024 \
#  --backendIndexName /local/storage/liang/Downloads/Datasets/rasterjoin_backendindex/dtl_cnty/index \
#  --locAttrib 2 \
#  --polygonList /local/storage/liang/Downloads/Datasets/rasterjoin_polygon/USAZIPCodeArea/polygons.txt \
#  --polygonDataset 1 \
#  --startTime 0 \
#  --endTime 0 \
#  --opAggregation aggOut

#./RasterJoin --nIter 1 \
#  --joinType raster \
#  --indexRes 1024 \
#  --backendIndexName /local/storage/liang/Downloads/Datasets/rasterjoin_backendindex/USAZIPCodeArea/USAZIPCodeArea \
#  --locAttrib 2 \
#  --polygonList /local/storage/liang/Downloads/Datasets/rasterjoin_polygon/dtl_cnty/polygons.txt \
#  --polygonDataset 1 \
#  --startTime 0 \
#  --endTime 0 \
#  --opAggregation aggOut
