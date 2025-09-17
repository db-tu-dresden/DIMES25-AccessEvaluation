numactl -m 0 -N 0 ./experiment_fixed_data_size -mib 256 --repeats 5 --file 1_gib.csv;
numactl -m 0 -N 0 ./experiment_fixed_data_size -mib 256 --repeats 5 --file 1_gib_thp.csv -thp;
