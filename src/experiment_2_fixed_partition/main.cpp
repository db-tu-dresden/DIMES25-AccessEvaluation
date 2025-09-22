#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>

#include "config.hpp"
#include <timeing.hpp>
#include <io.hpp>
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
    
    res_printer.add_columns({"algorithm", "stride", "lane_count","real_data_amount"});
    res_printer.add_metrics({"gibs"});
    
    AlignedAllocator<data_t> allocator(conf.page_type);
    data_t *data = allocator.allocate(conf.alloc_amount * conf.max_lane_count);
    allocator.fill(data, conf.alloc_amount * conf.max_lane_count);
    
    auto counter_definitions = perf::CounterDefinition{"perf-profiles/available_counters.csv"};
    auto event_counter = perf::EventCounter{ counter_definitions };
    event_counter.add({ "instructions", "cycles", "nanoseconds"});
    
    auto run_benchmark = [&](){            
        rc.name = "strided";
        for(auto run_id = 0U; run_id < conf.repeats; run_id++){
            event_counter.start();
            auto res = scalar_strided<data_t>(data, rc.real_data_amount, rc.stride);
            event_counter.stop();
            res_printer.add_run_results(rc, event_counter.result(), res);
        }
        
        rc.name = "strided unrolled";
        for(auto run_id = 0U; run_id < conf.repeats; run_id++){
            event_counter.start();
            auto res = scalar_unrolled_codegen<data_t>(data, rc.real_data_amount, rc.lane_count);
            event_counter.stop();
            res_printer.add_run_results(rc, event_counter.result(), res);
        }
    };
    
    rc.lane_count = conf.max_lane_count;
    size_t increase = conf.max_data_amount - conf.min_data_amount;
    increase /= conf.sample_count;
    
    for(rc.stride = conf.min_data_amount; rc.stride < conf.max_data_amount; rc.stride += increase){
        rc.real_data_amount = rc.stride * rc.lane_count;        
        run_benchmark();
    }
    
    rc.stride = conf.max_data_amount;
    rc.real_data_amount = rc.stride * rc.lane_count;
    run_benchmark();

    allocator.deallocate(data, conf.alloc_amount);
}