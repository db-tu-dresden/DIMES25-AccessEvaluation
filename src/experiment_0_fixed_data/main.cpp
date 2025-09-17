#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>

#include "config.hpp"
#include <timeing.hpp>
#include <io.hpp>
// // #include <perfcpp/event_counter.h>
#include <kernels_generic.hpp>

#ifdef ARM
#include <kernels_SVE.hpp>
#else
#include <kernels_AVX512.hpp>
#endif


int main(int argc, char ** argv){
    
    config::benchmark_config conf = config::load(argc, argv);
    ResultPrinter res_printer(conf);
    config::run_config rc;
    
    res_printer.add_columns({"algorithm", "stride", "real_data_amount"});
    res_printer.add_metrics({"gibs"});
    
    AlignedAllocator<data_t> allocator(conf.page_type);
    data_t *data = allocator.allocate(conf.alloc_amount);
    allocator.fill(data, conf.alloc_amount);
    
    auto counter_definitions = perf::CounterDefinition{"perf-profiles/available_counters.csv"};
    auto event_counter = perf::EventCounter{ counter_definitions };
    event_counter.add({ "instructions", "cycles", "nanoseconds"});
    
    rc.stride = 1;
    rc.real_data_amount = conf.data_amount - (conf.data_amount % VECTOR_ELEMENT_COUNT);

    auto run_benchmark = [&](){
        rc.name = "sequential";
        for(auto run_id = 0U; run_id < conf.repeats; run_id++){
            event_counter.start();
            auto res = scalar<data_t>(data, rc.real_data_amount);
            event_counter.stop();
            res_printer.add_run_results(rc, event_counter.result(), res);
        }
        
        rc.name = "strided";
        for(auto run_id = 0U; run_id < conf.repeats; run_id++){
            event_counter.start();
            auto res = scalar_strided_v2<data_t>(data, rc.real_data_amount, rc.stride);
            event_counter.stop();
            res_printer.add_run_results(rc, event_counter.result(), res);
        }
        
        rc.name = "simd sequential";
        for(auto run_id = 0U; run_id < conf.repeats; run_id++){
            event_counter.start();
            auto res = linear(data, rc.real_data_amount);
            event_counter.stop();
            res_printer.add_run_results(rc, event_counter.result(), res);
        }
            
        rc.name = "strided unrolled";
        size_t partition_count = rc.real_data_amount / rc.stride;
        rc.real_data_amount = partition_count * rc.stride; 
        if(partition_count <= 512){
            for(auto run_id = 0U; run_id < conf.repeats; run_id++){
                event_counter.start();
                auto res = scalar_lanes_codegen_v2<data_t>(data, rc.real_data_amount, partition_count);
                event_counter.stop();
                res_printer.add_run_results(rc, event_counter.result(), res);
            }
        }
        rc.real_data_amount = conf.data_amount - (conf.data_amount % VECTOR_ELEMENT_COUNT);
    };

    const size_t min_partition_count = 512; 
    size_t step_size = 1;
    for(size_t i = 1; i < rc.real_data_amount && (rc.real_data_amount / i) > min_partition_count; i += step_size){
        rc.stride = i;
        switch(step_size){
            case 1:
                if(i % 10 == 0){
                    step_size = 10;
                }
                break;
            case 10:
                if(i % 100 == 0){
                    step_size = 100;
                }
                break;
            case 100:
                if(i % 1000 == 0){
                    step_size = 1000;
                }
            case 1000:
                if(i % 10000 == 0){
                    step_size = 2500;
                }
                break;
        }
        run_benchmark();
    }

    for(size_t i = min_partition_count; i > 0; i--){
        rc.stride = rc.real_data_amount / i;
        run_benchmark();
    }
    allocator.deallocate(data, conf.alloc_amount);
}