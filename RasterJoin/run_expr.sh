#!/usr/bin/env bash
set -e
#Result: ptsSize, polySize, function, noPtPasses, noConstraints, executeTime, ptMemTime, ptRenderTime, polyMemTime, polyRenderTime, setupTime, triangulationTime, polyIndexTime, backendQueryTime, accuracy

CONTINENTS=(Africa Asia Australia Europe North_America South_America)
ROOT_DIR="/local/storage/liang/Downloads/Datasets"
export MESA_GL_VERSION_OVERRIDE=4.3

function run() {
  input_index=$1
  input_poly=$2
  output_prefix=$3

  if [[ ! -f "${input_index}_config" ]]; then
    echo "Cannot open ${input_index}_config"
    exit 1
  fi

  if [[ ! -f "${input_poly}" ]]; then
    echo "Generating $input_poly"
  fi

  if [[ ! -f "$output_prefix/raster_0_100.csv" ]]; then
    ./RasterJoin --nIter 1 \
      --joinType raster \
      --indexRes 1024 \
      --backendIndexName "$input_index" \
      --locAttrib 2 \
      --polygonList "$input_poly" \
      --polygonDataset 1 \
      --startTime 0 \
      --endTime 0 \
      --gpuMem 20000 \
      --opAggregation "$output_prefix" 2>&1 | tee "${output_prefix}/timing"
    if [[ $? -ne 0 ]]; then
      echo "Failed to evaluate $input_index with $input_poly"
    fi
  fi
}

for con in ${CONTINENTS[*]}; do
  input_index="$ROOT_DIR"/rasterjoin_backendindex/parks/$con/parks
  input_poly="$ROOT_DIR"/rasterjoin_polygon/lakes/$con/polygons.txt
  output_prefix="$ROOT_DIR"/rasterjoin_out/lakes/$con
  run "$input_index" "$input_poly" "$output_prefix"
done

input_index="$ROOT_DIR"/rasterjoin_backendindex/USAZIPCodeArea/USAZIPCodeArea
input_poly="$ROOT_DIR"/rasterjoin_polygon/dtl_cnty/polygons.txt
output_prefix="$ROOT_DIR"/rasterjoin_out/dtl_cnty
run "$input_index" "$input_poly" "$output_prefix"

input_index="$ROOT_DIR"/rasterjoin_backendindex/USADetailedWaterBodies/USADetailedWaterBodies
input_poly="$ROOT_DIR"/rasterjoin_polygon/USACensusBlockGroupBoundaries/polygons.txt
output_prefix="$ROOT_DIR"/rasterjoin_out/USACensusBlockGroupBoundaries
run "$input_index" "$input_poly" "$output_prefix"
