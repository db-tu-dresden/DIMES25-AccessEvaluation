numactl -m 0 -N 0 ./experiment_fixed_stride_size -mib 127 --repeats 5 --file 127_MiB.csv;
numactl -m 0 -N 0 ./experiment_fixed_stride_size -mib 128 --repeats 5 --file 128_MiB.csv;
numactl -m 0 -N 0 ./experiment_fixed_stride_size -da 16688654 --repeats 5 --file 127b_MiB.csv;