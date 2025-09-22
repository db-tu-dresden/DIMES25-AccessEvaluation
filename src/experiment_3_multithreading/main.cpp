#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>

#include "config.hpp"
#include <timeing.hpp>
#include <io.hpp>

#ifdef ARM
// TODO
#else
#include <kernelsMT_AVX512.hpp>
#endif

#define MAX_VECTOR_ELEMENT_COUNT 16

template<typename data_t, typename simd_t>
void benchmark(config::benchmark_config conf);

int main(int argc, char ** argv){
    config::benchmark_config conf = config::load(argc, argv);
    conf.data_amount = (conf.data_amount / MAX_VECTOR_ELEMENT_COUNT) * MAX_VECTOR_ELEMENT_COUNT;
    config::print(conf);

    std::string header = stringify(
        "algorithm", 
        "run_id", 
        "data_amount", 
        "byte_count", 
        "vector_element_count",
        "uses_SIMD",
        "thread_count",
        "time_ns", 
        "gibs"
    );

    if(!conf.append){
        write_to_file(conf.file_and_path, header, !conf.append, true);
    }

    if(conf.use_64bit_elements && conf.use_512bit_vectors){
        std::cout << "using 512 bit vector with 64 bit elements" << std::endl;
        benchmark<int64_t, __m512i>(conf);
    }else if(conf.use_64bit_elements && !conf.use_512bit_vectors){
        std::cout << "using 256 bit vector with 64 bit elements" << std::endl;
        benchmark<int64_t, __m256i>(conf);
    }else if(!conf.use_64bit_elements && conf.use_512bit_vectors){
        std::cout << "using 512 bit vector with 32 bit elements" << std::endl;
        benchmark<int32_t, __m512i>(conf);
    }else if(!conf.use_64bit_elements && !conf.use_512bit_vectors){
        std::cout << "using 256 bit vector with 32 bit elements" << std::endl;
        benchmark<int32_t, __m256i>(conf);    
    }
}

template<typename data_t, typename simd_t>
void benchmark(config::benchmark_config conf){
    AlignedAllocator<data_t> allocator(conf.page_type);
    data_t *data = allocator.allocate(conf.alloc_amount);
    allocator.fill(data, conf.alloc_amount);
    
    print_page_info<data_t>(data, conf.alloc_amount);
    timeing::time_stamp a, b;
    int64_t res = 0;
    std::string name;
    size_t run_id = 0;
    size_t tc = 0;
    size_t stride = 0;
    size_t time_nano[conf.repeats];
    size_t check_sum = 0;

    auto get_time = [&](){
        time_nano[run_id] = timeing::time_microseconds(a, b);

        check_sum +=  res;
    };

    auto write_benchmark_result = [&](size_t vector_element_count, bool uses_simd = false){
        uint64_t total_time = 0;
        size_t data_amount = conf.data_amount;
        for(size_t i = 0; i < conf.repeats; i++){
            write_to_file(
                conf.file_and_path,
                stringify(
                    name, 
                    i,
                    data_amount * tc,
                    sizeof(data_t),
                    vector_element_count,
                    vector_element_count * sizeof(data_t) * uses_simd,
                    tc,
                    time_nano[i],
                    timeing::gibs<data_t>(time_nano[i], -2, data_amount * tc)
                )
            );
            total_time += time_nano[i];
        }
        total_time /= conf.repeats;
        std::cout << " " ;
        if(total_time != 0){
            std::cout   << std::right << std::setw(5) << vector_element_count << " "
                        << std::left << std::setw(23) << name
                        << std::right << std::setw(10) << "check: "
                        << std::left << std::setw(10) << check_sum 
                        << std::right << std::setw(8) << timeing::time_string(total_time, -2) 
                        << std::setw(15) << timeing::throughput_string(timeing::bs<data_t>(total_time, -2, data_amount * tc));    
        }else{
            std::cout   << std::left << std::setw(23) << name
                        << std::right << std::setw(10) << "check: "
                        << std::left << std::setw(10) << check_sum 
                        << std::right << std::setw(8) << timeing::time_string(total_time, -2) 
                        << std::setw(15) << "inf";
        }
        if(stride != 0){
            std::cout << std::right << std::setw(11) << "stride: ";
            std::cout << std::right << std::setw(8) << stride;
        }
        std::cout << std::endl;
        check_sum = 0;
    };

    size_t partitions = 1;

    for(tc = 1; tc <= conf.max_thread_count; tc++){
        std::cout << "\n### Thread Count: " << std::right << std::setw(3) << tc << " " << std::string(47, '#') << std::endl;

        name = "sequential";
        for(run_id = 0; run_id < conf.repeats; run_id++){
            a = timeing::now();
            res = scalar_MT<data_t>::run(data, conf.data_amount, tc);
            b = timeing::now();
            get_time();
        }
        write_benchmark_result(scalar_MT<data_t>::vector_element_count);
        
        name = "simd sequential";
        for(run_id = 0; run_id < conf.repeats; run_id++){
            a = timeing::now();
            res = linear_MT<data_t, simd_t>::run(data, conf.data_amount, tc);
            b = timeing::now();
            get_time();
        }
        write_benchmark_result(linear_MT<data_t, simd_t>::vector_element_count, true);
        
        name = "strided";
        size_t max_partition = conf.max_lane_count; 
        for(size_t partitions = 1; partitions <= max_partition; partitions++){
            for(run_id = 0; run_id < conf.repeats; run_id++){
                a = timeing::now();
                res = scalar_strided_MT<data_t>::run(data, conf.data_amount, tc, partitions);
                b = timeing::now();
                get_time();
            }
            write_benchmark_result(partitions);
        }

        for(run_id = 0; run_id < conf.repeats; run_id++){
            a = timeing::now();
            res = scalar_strided_MT<data_t>::run(data, conf.data_amount, tc, conf.data_amount); //stride size 1
            b = timeing::now();
            get_time();
        }
        write_benchmark_result(conf.data_amount);


        name = "strided unrolled";
        for(size_t partitions = 1; partitions <= max_partition; partitions++){
            size_t sub = conf.data_amount % partitions;
            for(run_id = 0; run_id < conf.repeats; run_id++){
                a = timeing::now();
                res = unrolled_strided_MT<data_t>::run(data, conf.data_amount - sub, tc, partitions);
                b = timeing::now();
                get_time();
            }
            write_benchmark_result(partitions);
        }
    }
    allocator.deallocate(data, conf.total_allocated_data);
}