# DIMES25-AccessEvaluation
To stride or not to stride the memory access?

## 📄 Abstract
Due to the increasing gap between CPU performance and memory bandwidth, memory access patterns play more and more a significant role for efficient data processing.
The current core assumption is that a sequential access pattern delivers the best performance, especially when the data to be processed is stored in adjacent memory locations (contiguous memory).
Given the continuous advances in memory technologies, it is of course questionable whether this assumption still holds true.
To answer this question, we present a comprehensive experimental comparison of the sequential and the strided access pattern for data stored in contiguous memory on modern disruptive memory systems in this paper. 
As we are going to show, the core assumption must be revised, as the strided access pattern with a well-chosen stride size clearly outperforms the sequential access pattern.
Even a SIMD-accelerated sequential access is considerably slower than the best-performing scalar strided access.
In particular, we explain the differences, highlight further advantages, and present open challenges of the strided access pattern on disruptive memory systems.

## ℹ️ Overview

This project contains:
- source code to all the experiment present in the paper
- R code to generate the plots like the one in the paper
- additional plots and discussions that were not present in the paper 

## ⚙️ Benchmarking

To build the benchmark suit use 
```bash
cmake . 
make
```

This will generate the following benchmarks:
| Executalble Name | Description | Figure in Paper  |
| --- | --- | :---: |
| experiment_fixed_data_size | Given a data amount, this benchmark systematically samples different stride and partition counts | 1, 2, 3 |
| experiment_fixed_stride_size | Given a stride size, this benchmark test the different partition counts from in a range of 2 to 50.  | 5a |
| experiment_fixed_partition_count | Given a partition count and a data range, the benchmark samples different stride sizes in the given range  | 5b, 5c |
| experiment_multithreading | Given a data amount, this benchmark evaluates the MIMD execution of our strided access. For each thread count the partition count of 1 to 72 gets evaluated | 6 |

To run the benchmarks use ```sudo ./<benchmark_name>``` 

It is adviced to use numactl to pin the execution and the memory to a NUMA-node.

All the available options for the benchmarks can be read out with:
```./<benchmark_name> --help``` 

## 📈 Additional Paper Results




## Third-Party Licenses

This project includes or uses code from the following third-party projects:

- perf-cpp (version v0.11-dev) — LGPLv3+
https://github.com/jmuehlig/perf-cpp
- page-info — MIT License
https://github.com/travisdowns/page-info
A copy of the full license texts and attributions is included in the `LICENSES/` folder:


## Using a Modified perf-cpp Library

This project depends on the perf-cpp library (version v0.11-dev), licensed under the GNU Lesser General Public License, version 3 (LGPLv3). By default, CMake will automatically download and build perf-cpp from its official repository:

https://github.com/jmuehlig/perf-cpp 

### Rebuilding with a Modified perf-cpp

Under the LGPLv3, you have the right to replace perf-cpp with your
own modified version and rebuild this program against it. To do so:

1. Fork or download your modified version of perf-cpp.
2. In your local checkout of this project, edit the CMake configuration such that it uses your modified perf-cpp source instead of fetching the official repository.
3. Reconfigure and rebuild this project with CMake such that it will link against your modified perf-cpp library.

### License Information
- This program includes and links against perf-cpp under the terms of the GNU Lesser General Public License, version 3 or later.
- A copy of the LGPLv3 license is provided in this distribution (`COPYING.LESSER`).



## 💭 Feedback
For feedback, questions or discussions please feel free to contact us. 