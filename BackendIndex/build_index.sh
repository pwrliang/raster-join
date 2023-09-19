#!/usr/bin/env python3
# Input file: /local/storage/liang/Downloads/Datasets/cleaned_shp/dtl_cnty/dtl_cnty.shp
#Number of points: 9438045
#Bounds, lat: 18.910693000 - 71.365162000, lon: -179.148909000 - -66.949607922

#Input file: /local/storage/liang/Downloads/Datasets/cleaned_shp/USAZIPCodeArea/USAZIPCodeArea.shp
#Number of points: 43626439
#Bounds, lat: -14.548692000 - 71.390482000, lon: -179.147340000 - 179.778465000

#./BackendIndex --inputFilename /local/storage/liang/Downloads/Datasets/rasterjoin_point/dtl_cnty/dtl_cnty.txt \
#  --backendIndexName /local/storage/liang/Downloads/Datasets/rasterjoin_backendindex/dtl_cnty/index \
#  --min-long -179.148909000 --max-long 179.778465000 \
#  --min-lat -14.548692000 --max-lat 71.390482000

#./BackendIndex --inputFilename /local/storage/liang/Downloads/Datasets/rasterjoin_point/USAZIPCodeArea/USAZIPCodeArea.txt \
#              --backendIndexName /local/storage/liang/Downloads/Datasets/rasterjoin_backendindex/USAZIPCodeArea/index \
#              --min-long -179.148909000 --max-long 179.778465000 \
#              --min-lat -14.548692000 --max-lat 71.390482000


# Africa Asia Australia Europe North_America South_America USCounty*Zipcode BlockGroup*WaterBodies

set -e
min_lat=("-34.7852074" "-10.819686" "-55" "35.026418" "7.2247576" "-55.9828236" "-14.548692" "18.910693")
max_lat=("37.3455867" "81.2254901" "-10" "82.5" "83.4028185" "12.5802603" "71.390482" "71.365162")
min_lon=("-25.2134684" "-179.9724901" "113" "-31.2515213" "-178.0578473" "-92.682879" "-179.148909" "-179.148909")
max_lon=("63.4371381" "179.9837763" "159" "68.8330601" "179.3017549" "-34.7879701" "179.778465" "-66.949607922")

CONTINENTS=(Africa Asia Australia Europe North_America South_America)
ROOT_DIR="/local/storage/liang/Downloads/Datasets"

function convert() {
  input_file=$1
  output_file=$2
  idx=$3
#  echo "in $input_file"
#  echo "out $output_file"
#  echo "idx $idx"
#  echo "${min_lon[idx]} ${max_lon[idx]} ${min_lat[idx]} ${max_lat[idx]}"

  if [[ ! -f "$input_file" ]]; then
    echo "Cannot open $input_file"
    exit 1
  fi
  if [[ ! -f "${output_file}_config" ]]; then
    echo "Generating $output_file"
    ./BackendIndex --inputFilename "$input_file" \
      --backendIndexName "$output_file" \
      --min-long ${min_lon[idx]} --max-long ${max_lon[idx]} \
      --min-lat ${min_lat[idx]} --max-lat ${max_lat[idx]}
    if [[ $? -ne 0 ]]; then
      echo "Error occurred ${output_file}"
    fi
  else
    echo "Skipping $output_file"
  fi
}

for name in lakes parks; do
  i=0
  for con in ${CONTINENTS[*]}; do
    input_file="$ROOT_DIR"/rasterjoin_point/$name/$con/${name}.txt
    output_file="$ROOT_DIR"/rasterjoin_backendindex/$name/$con/${name}
    convert "$input_file" "$output_file" "$i"
    i=$((i + 1))
  done
done

convert "$ROOT_DIR"/rasterjoin_point/dtl_cnty/dtl_cnty.txt "$ROOT_DIR"/rasterjoin_backendindex/dtl_cnty/dtl_cnty "$i"
convert "$ROOT_DIR"/rasterjoin_point/USAZIPCodeArea/USAZIPCodeArea.txt "$ROOT_DIR"/rasterjoin_backendindex/USAZIPCodeArea/USAZIPCodeArea "$i"

i=$((i + 1))
convert "$ROOT_DIR"/rasterjoin_point/USACensusBlockGroupBoundaries/USACensusBlockGroupBoundaries.txt "$ROOT_DIR"/rasterjoin_backendindex/USACensusBlockGroupBoundaries/USACensusBlockGroupBoundaries "$i"
convert "$ROOT_DIR"/rasterjoin_point/USADetailedWaterBodies/USADetailedWaterBodies.txt "$ROOT_DIR"/rasterjoin_backendindex/USADetailedWaterBodies/USADetailedWaterBodies "$i"
