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
            std::cout << std::left << " " << std::setw(o1) << "" << " " << std::setw(o2) << "--min_mib"
                << "Give the minimum data amount in mib" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "" << " " << std::setw(o2) << "--max_mib"
                << "Give the maximum data amount in mib" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "-p" << " " << std::setw(o2) << "--partition"
                << "Give the fixed partition count" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "" << " " << std::setw(o2) << "--samples"
                << "Give the sample amount that shall be used" << std::endl;
        }
    }

config::benchmark_config load(int argc, char ** argv){
        config::benchmark_config config;
        config = load_default(argc, argv);

        auto check = [&](std::string option){
            return hidden::cmdOptionExists(argv, argv + argc, option);
        };
        auto option = [&](std::string option){
            return hidden::getCmdOption(argv, argv + argc, option);
        };

        if(check("-h") || check("--help")){hidden::print_help(); exit(1);}

        size_t s = 4 + config.use_64bit_elements * 4;
        if(check("--min_mib")){
            size_t amount = atoll(option("--min_mib"));
            config.min_data_amount = amount * (1024 / s) * 1024LL;
        }else{
            config.min_data_amount = 127 * 1024 * 1024LL / s;
        }

        if(check("--max_mib")){
            size_t amount = atoll(option("--max_mib"));
            config.max_data_amount = amount * (1024 / s) * 1024LL;
        }else{
            config.max_data_amount = 128 * 1024 * 1024LL / s;
        }

        if(check("--partition")){
            config.max_lane_count = atoll(option("--partition"));
        }else if(check("-p")){
            config.max_lane_count = atoll(option("-p"));
        }else{
            config.max_lane_count = 33;
        }

        if(check("--sample")){
            config.sample_count = atoll(option("--sample"));
        }

        config.data_amount = 0;
        return config;
    }

    template<typename T>
    void print(benchmark_config config){
        size_t element_size = print_default(config);
        size_t total_size = (element_size + 2) * 2 + 2;
    }     
}