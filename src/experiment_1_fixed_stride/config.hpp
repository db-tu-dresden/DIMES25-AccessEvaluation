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

        if(check("--partition")){
            config.max_lane_count = atoll(option("--partition"));
        }else if(check("-p")){
            config.max_lane_count = atoll(option("-p"));
        }else{
            config.max_lane_count = 50;
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