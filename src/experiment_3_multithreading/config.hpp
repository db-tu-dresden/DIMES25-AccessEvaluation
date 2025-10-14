#pragma once

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <formatter.hpp>

namespace config{
    namespace hidden {
        void print_help(){
            std::cout << std::endl << "\033[1mBENCHMARK SPECIFIC OPTIONS\033[22m" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-p" << " " << std::setw(o2) << "--partition"
                << "Give the maximum partition count" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "-tc" << " " << std::setw(o2) << "--thread_count"
                << "Give the maximum thread count" << std::endl;
        }
    }

    benchmark_config load(int argc, char ** argv){
        benchmark_config config;
        config = load_default(argc, argv);
        
        auto check = [&](std::string option){
            return hidden::cmdOptionExists(argv, argv + argc, option);
        };
        auto option = [&](std::string option){
            return hidden::getCmdOption(argv, argv + argc, option);
        };
        
        if(check("-h") || check("--help")){
            hidden::print_help(); 
            exit(1);
        }

        if(check("--thread_count")){
            config.max_thread_count = atoll(option("--thread_count"));
        }else if(check("-tc")){
            config.max_thread_count = atoll(option("-tc"));
        }

        if(check("--partition")){
            config.max_lane_count = atoll(option("--partition"));
        }else if(check("-p")){
            config.max_lane_count = atoll(option("-p"));
        }else{
            config.max_lane_count = 50;
        }

        config.alloc_amount *= config.max_thread_count;
        return config;
    }

    void print(benchmark_config config){
        size_t element_size = print_default(config);
        size_t total_size = (element_size + 2) * 2 + 2;

        if(config.use_512bit_vectors){
            std::cout << "# " << std::string(element_size - 10, ' ')<< "Using 512 Bit Vector" << std::string(element_size - 11, ' ')  << " #" << std::endl;
        }else{
            std::cout << "# " << std::string(element_size - 10, ' ')<< "Using 256 Bit Vector" << std::string(element_size - 11, ' ')  << " #" << std::endl;
        }
        std::cout << " # " << std::right << std::setw(element_size) << "Total Allocated Data";
        std::cout << "# " << std::right << std::setw(element_size) << "max thread count" << ": " << std::left << std::setw(element_size) << config.max_thread_count << " #" << std::endl;
        std::cout << std::string(total_size, '#') << std::endl;
    }     
}