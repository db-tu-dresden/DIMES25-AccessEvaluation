numactl -m 0 -N 0 ./experiment_fixed_partition_count --min_mib 127 --max_mib 128 --repeats 5 -p 13 --sample 35 --file 13P.csv;
numactl -m 0 -N 0 ./experiment_fixed_partition_count --min_mib 127 --max_mib 128 --repeats 5 -p 33 --sample 35 --file 33P.csv;
numactl -m 0 -N 0 ./experiment_fixed_partition_count --min_mib 127 --max_mib 128 --repeats 5 -p 42 --sample 35 --file 42P.csv;