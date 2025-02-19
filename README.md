# README for the Raster Join Project

## Fix compile error: `export MESA_GL_VERSION_OVERRIDE=4.3`
## Debug mode: `qmake CONFIG+=debug`

## Paper abstract
Visual exploration of spatial data relies heavily on spatial aggregation queries that slice and summarize the data over different regions. These queries comprise computationally-intensive point-in-polygon tests that associate data points to polygonal regions, challenging the responsiveness of visualization tools. This challenge is compounded by the sheer amounts of data, requiring a large number of such tests to be performed. Traditional pre-aggregation approaches are unsuitable in this setting since they fix the query constraints and support only rectangular regions. On the other hand, query constraints are defined interactively in visual analytics systems, and polygons can be of arbitrary shapes. In this paper, we convert a spatial aggregation query into a set of drawing operations on a canvas and leverage the rendering pipeline of the graphics hardware (GPU) to enable interactive response times. Our technique trades-off accuracy for response time by adjusting the canvas resolution, and can even provide accurate results when combined with a polygon index. We evaluate our technique on two large real-world data sets, exhibiting superior performance compared to index-based approaches.

**Link to the publication:** http://www.vldb.org/pvldb/vol11/p352-zacharatou.pdf 

## Subproject directories:

### Backend Index
The code for creating a simple disk-based hash grid index. We only use the backend index as a way to select data of varying sizes for the experiments.

**Usage:**  BackendIndex --inputFilename path_to_file --backendIndexName prefix_path

--inputFilename:  binary file containing the input data, stored record-wise.  
--backendIndexName:  prefix path for the created index. The index stores the data column-wise, and thus creates a separate file for each indexed attribute.  

**Example:** ./BackendIndex --inputFilename ./taxi.bin --backendIndexName ./taxi-backend

**Notes:** 
By default, the source code creates an index for the NYC Taxi data set [1].  
It can be configured to create an index for the Twitter data set by setting the datasetType in line 19 of the main.cpp to Twitter. 
By default, only the location attribute is imported in the index.
This can be adjusted by setting the flag importLocationOnly in line 20 of the main.cpp to false. 
Note that it is easy to extend the code in order to build an index for data sets other than the Taxi and Twitter data sets that we used in our evaluation. 
For that, you need to implement the interface named Record.hpp for your data set. 

[1] TLC Trip Record Data. http://www.nyc.gov/html/tlc/html/about/trip_record_data.shtml, 2015.

### Raster Join
The source code for the three GPU techniques. 

**Usage:** RasterJoin [--nIter number_of_iterations] [--accuracy epsilon_bound_in_meters] --joinType [raster | errorbounds | hybrid | index] --backendIndexName prefix_path --polygonList  path_to_file --polygonDataset name_of_polygon_collection --locAttrib offset_of_the_location_attribute_in_the_record [--indexRes number_of_cells_per_dim] [--nAttrib number_of_filtered_attributes] --startTime start_time --endTime end_time [--inmem] [--opAggregation ] [--inputSize ] [--avg attribute] [--gpuMem size_in_megabytes] [--outputTime path_to_file]

--nIter: Define how many times the query should be executed (for performance measurements only). Optional argument. The default value is 1.

--accuracy: The epsilon-bound in meters. Optional argument. The default value is 100.

--joinType: Choose GPU technique. There are four options: raster, errorbounds, hybrid and index. Here, errorbounds corresponds to the raster approach which computes the aggregation range (see paragraph **Estimating the Result Range** of Section 5 in the paper. Currently implemented only for Count() aggregation).

--backendIndexName: Prefix path to the backend index containing the data records. 

--polygonList: Path to the text file containing the polygon data. 

--polygonDataset: Give a name for the polygon dataset. 

--locAttrib: Denote which is the spatial (location) attribute of the record. For taxi records it is the first attribute (locAttrib = 1). 

--indexRes: The resolution of the polygon index (in number of cells per dimension). Optional argument. The default value is 256.  

--nAttrib: Number of filtering constraints. Optional argument. The default value is 0. The actual filtered attributes and the imposed constraints are currently hardcoded for the Taxi dataset.  

--startTime and --endTime: Define a time range and retrieve the corresponding data from the backend. For our experiments, we fixed the start time and increased the end time in order to get increasing input sizes for the aggregate query. 

--inmem: Choice of in-memory (GPU) versus out-of-core (GPU) execution. Optional argument. The default is out-of-core (GPU) execution. 

--opAggregation:  Optional argument. Choice between printing the aggregation result for every region (when this argument is present) and obtaining only the corresponding timing statistics (the default behavior).

--inputSize: Optional argument. Allows to define the input size in numbers of records rather than by manipulating the startTime and endTime. 

--avg: Computation of avg() aggregation function over the specified attribute. Optional argument. Count() is the default computation. 

--gpuMem: GPU memory usage in Megabytes. Optional argument. The default is 2048 MB. 

--outputTime: Path to output file containing the performance results. Optional argument.

**Examples:** The following examples assume that the executable is in the release folder. 

release/RasterJoin.exe --nIter 3 --joinType raster --accuracy 10 --backendIndexName [path to data folder]/taxi/taxi_full_index --locAttrib 1 --polygonList [path to data folder]/polys/nyc_polys.txt --polygonDataset neigh --startTime 1230768000 --endTime 1272808000

The above command executes the bounded raster join with an accuracy of 10 meters. The join is performed 3 times.

release/RasterJoin.exe --nIter 10 --joinType raster --backendIndexName [path to data folder]/taxi/taxi_full_index --polygonList [path to data folder]/polys/nyc_polys.txt --polygonDataset neigh --locAttrib 1 --nAttrib 2 --startTime 1230768000 --endTime 1272808000 --inmem 
--outputTime [path to output folder]/scalability/taxi-mem-attrib.txt

Here the join is performed 10 times, filtering constraints are applied, and the results are saved in a text file. 
  
release/RasterJoin.exe --nIter 4 --joinType hybrid --indexRes 1024 --backendIndexName [path to data folder]/taxi/taxi_full_index --locAttrib 1 --polygonList [path to data folder]/polys/nyc-polygons.txt --polygonDataset 16384 --startTime 1230768000 --endTime 1272808000

This command runs the hybrid raster join using the synthetic polygon dataset having 16384 polygons. The resolution of the index created on the polygons is 1024. Replacing "hybrid" with "index" will execute the baseline index-based join.
    
### CPU Join
The source code for the CPU baseline (both single-core and parallel).

**Usage:** CPUJoin [--nIter number_of_iterations] [--singleCore] --backendIndexName prefix_path --polygonList path_to_file --polygonDataset name_of_polygon_collection --locAttrib offset_of_the_location_attribute_in_the_record [--indexRes number_of_cells_per_dim] --startTime start_time --endTime end_time [--outputTime path_to_file]

--nIter: Define how many times the query should be executed (for performance measurements only). Optional argument. The default value is 1.

--singleCore: Choice of single-core versus parallel execution. Optional argument. The default is parallel. 

--backendIndexName: Prefix path to the backend index containing the data records.

--polygonList: Path to the text file containing the polygon data.

--polygonDataset: Give a name for the polygon dataset.

--locAttrib: Denote which is the spatial (location) attribute of the record. For taxi records it is the first attribute (locAttrib = 1).

--indexRes: The resolution of the polygon index (in number of cells per dimension). Optional argument. The default value is 1024.

--startTime and --endTime: Define a time range and retrieve the corresponding data from the backend. For our experiments, we fixed the start time and increased the end time in order to get increasing input sizes for the aggregate query.

--outputTime: Path to output file containing the performance results. Optional argument.

 **Example:** release/CPUJoin.exe --nIter 6 --backendIndexName [path to data folder]/taxi/taxi_full_index --polygonList  [path to data folder]/polys/nyc_polys.txt --polygonDataset neigh --locAttrib 1 --indexRes 1024 --startTime 1230768000 --endTime 1272808000 

## Reproducing paper's experiments:
 The **experiments** directory provides all the required resources for reproducing the results presented in the paper.
 It consists of the following subdirectories:
 
 - **run-scripts**: This directory contains Windows powershell scripts that produce the experimental results that are presented in the paper. All scripts take the following three arguments: 1) the location of the executable, 2) the path to the data directory (which has the taxi and polys subdirectories), and, 3) the path to store the result files (the scripts will create two subdirectories, scalability and accuracy to store the respective files). The scripts that involve the GPU approaches take an additional fourth argument, 4) the GPU memory usage in Megabytes. This argument is optional and the default value is set to 3072 MB which corresponds to the configuration used in the paper.

The **executable** that the scripts take as input can be downloaded in the release section or compiled from the provided source code.
Note that the release contains a redistributable package for visual studio which only needs to be installed in case of missing DLLs.  

The following zip file (rasterjoin-data.zip) contains the data directory that should be given as input to the scripts.   
**Link to download the data:** https://drive.google.com/open?id=1v8tSVX2ktovM9XWd95fZ7P6HWvD1mBCV

The directory extracted from the above zip file contains two subdirectories: taxi and polys. The taxi subdirectory has the backend index built on 5 years of taxi data used in the paper. Note that the purpose of this index is to only provide the means of selecting data of varying input sizes for the experiments. The polys subdirectory has both the neighborhood polygons, as well as the synthetic polygons used in the experiments.

- **results-paper**: This directory contains result files with the experimental numbers which are presented in the paper. The 'scalability' subdirectory contains timing results, and the 'accuracy' subdirectory contains results about the accuracy of the raster join approach. Note that when the results are generated using the provided poweshell scripts, the scripts take care of placing the result files in the corresponding subdirectory. 

- **plot-scripts**: This directory contains python scripts that generate the figures in the paper. More specifically, all the figures can be generated using the 'plot_script_main.py' script. This script takes as input 1) the path to the directory that stores the result files to be plotted, and, 2) the paper figure to be generated.

1) the path to the result directory is optional. By default the results are retrieved from the 'results-paper' directory.
2) this argument is of the form 'figure_[8-14]'|'all', where the number corresponds to the figure number in the paper and the option 'all' generates all the figures in the paper. 

The generated figures are stored in the 'figures-paper' directory.  

*Example usage*: python plot-scripts/plot_script_main.py figure_8


This takes the required input from the 'results-paper' directory,  generates Figure 8 of the paper, and stores it in the 'figures-paper' directory. 

- **figures-paper**: This directory contains all the figures (png files) in the paper that are generated using the python plot script with the provided result files in the 'resuls-paper' directory as input (python plot-scripts/plot_script_main.py all).  
