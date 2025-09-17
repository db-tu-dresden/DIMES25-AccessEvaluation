#pragma once

#ifndef BENCHMARKNAME
#define BENCHMARKNAME "ERROR"
#endif

#include <stdlib.h>
#include <iostream>
#include <immintrin.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

using data_t = int64_t;

namespace config{

    struct run_config {
        std::string name;
        size_t stride = 0;
        size_t lane_count = 0;
        size_t sub = 0;
        size_t batch_count = 0;
        size_t run_length = 0;
        size_t real_data_amount = 0;
        size_t real_element_byte = 0;
    };

    struct benchmark_config{
        size_t min_data_amount = 32LL;
        size_t max_data_amount = 0;
        size_t data_amount = 1024 * 1024 * 128LL;
        size_t repeats = 1;
        size_t max_stride_x = 12; 
        size_t max_lane_count = 32;
        size_t max_batch_count = 512; 
        size_t max_thread_count = 12;
        double max_time_in_min = 5.;
        size_t vector_element_count = 8;
        size_t sample_count = 150;
        size_t alloc_amount;
        size_t total_allocated_data = data_amount * max_thread_count;
        bool append = false;
        bool use_64bit_elements = true;
        bool use_512bit_vectors = true;
        bool fixed_data_amount = false;
        PageType page_type = K4_Normal;
        std::string result_file;
        std::string file_and_path;
    };

    namespace hidden {
        const size_t o1 = 5, o2 = 16;
        char* getCmdOption(char ** begin, char ** end, const std::string & option)
        {
            char ** itr = std::find(begin, end, option);
            if (itr != end && ++itr != end)
            {
                return *itr;
            }
            return 0;
        }
    
        bool cmdOptionExists(char** begin, char** end, const std::string& option){
           return std::find(begin, end, option) != end;
        }
    
        void print_default_help(){
            // std::cout << "Usage of the Benchmark - " << BENCHMARKNAME << " -" << std::endl;
            std::cout << "\033[1mGENERIC OPTIONS\033[22m" << std::endl;
        
            std::cout << std::left << " " << std::setw(o1) << "-h" << " " << std::setw(o2) << "--help"
                << "Display this help" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-r" << " " << std::setw(o2) << "--repeats"
                << "Give how often the Tests should be repeated" << std::endl;
            
            std::cout << std::left << " " << std::setw(o1) << "-f" << " " << std::setw(o2) << "--file"
                << "Give a filename for the results to be written to." << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "" << " " << std::setw(o2) << ""
                << "  Result file will be written to results/" << BENCHMARKNAME << "/" << std::endl;
            
            std::cout << std::left << " " << std::setw(o1) << "-a" << " " << std::setw(o2) << "--append"
                << "Append the file if it exists instead of overriding it" << std::endl;
            // std::cout << std::left << " " << std::setw(o1) << "-b32" << " " << std::setw(o2) << "--32bit"
            //     << "Use 32 bit data and processing if available" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-b64" << " " << std::setw(o2) << "--64bit"
                << "Use 64 bit data and processing. This is the default." << std::endl; 

            std::cout << std::endl << "\033[1mHUGE PAGES FLAGS\033[22m" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-thp" << " " << std::setw(o2) << "--thp"
                << "Madvise the use of Transparent Hugepages" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "-ps" << " " << std::setw(o2) << "--page_size"
                << "Use given page size [1G; 2M]" << std::endl;
            
            std::cout << std::endl << "\033[1mDATA AMOUNT FLAGS:\033[22m Mutual Exclusive" << std::endl;
            std::cout << std::left << " " << std::setw(o1) << "-da" << " " << std::setw(o2) << "--data_amount"
                << "Give how many data Elements should be used" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-kib" << " " << std::setw(o2) << " "
                << "Give how much data should be used in KiByte" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-mib" << " " << std::setw(o2) << " "
                << "Give how much data should be used in MiByte" << std::endl; 
            std::cout << std::left << " " << std::setw(o1) << "-gib" << " " << std::setw(o2) << " "
                << "Give how much data should be used in GiByte" << std::endl; 
        }
    }

    benchmark_config load_default(int argc, char ** argv){
        auto check = [&](std::string option){
            return hidden::cmdOptionExists(argv, argv + argc, option);
        };
        auto option = [&](std::string option){
            return hidden::getCmdOption(argv, argv + argc, option);
        };
        
        benchmark_config config;
        
        if(check("-h") || check("--help")){hidden::print_default_help(); return config;}

        if(check("--repeats")){
            config.repeats = atoll(option("--repeats"));
        }else if(check("-r")){
            config.repeats = atoll(option("-r"));
        }

        std::stringstream result_file_stream;

        if(check("--file")){
            config.result_file = option("--file");
        }else if(check("-f")){
            config.result_file = option("-f");
        }else{
            config.result_file = "latest.csv";
        }
        result_file_stream << "results/" << BENCHMARKNAME << "/";
        result_file_stream << config.result_file; 
        config.file_and_path = result_file_stream.str();

        config.append = check("-a") || check("--append");
        
        if(check("-b32") || check("--32bit")){
            config.use_64bit_elements = false;
            std::cout << "\033[33;1mWARNING: \033[22mAT THE MOMENT ONLY 64 BIT PROCESSING IS POSSIBLE\033[0m" << std::endl;
            config.use_64bit_elements = true; // TODO 
        }
        if((check("-b64") || check("--64bit")) && !config.use_64bit_elements){
            std::cout << "\033[33;1mWARNING: \033[22mYou can't specify both 32 Bit and 64 Bit Processing at once. \033[31;1mUsing 64 Bit Processing\033[0m" << std::endl;
            config.use_64bit_elements = true;
        }
        size_t s = 4 + config.use_64bit_elements * 4;

        if(check("-kib")){
            size_t amount = atoll(option("-kib"));
            config.data_amount = amount * (1024 / s);
        }else if(check("-mib")){
            size_t amount = atoll(option("-mib"));
            config.data_amount = amount * (1024 / s) * 1024LL;
        }else if(check("-gib")){
            size_t amount = atoll(option("-gib"));
            config.data_amount = amount * (1024 / s) * 1024 * 1024LL;
        }else if(check("--data_amount")){
            config.data_amount = atoll(option("--data_amount"));
        }else if(check("-da")){
            config.data_amount = atoll(option("-da"));
        }

        if(check("-thp") || check("--thp")){
            config.page_type = Transparent_HugePages;
        }
        
        size_t hugepage = check("--page_size") * 2 + check("-ps");  
        if(hugepage >= 1){
            std::string ptype;
            if(hugepage == 1){
                ptype = option("-ps");
            }else if(hugepage == 2){
                ptype = option("--page_size");
            }else{
                std::cout << "\033[31;1mERROR:\033[22m use either -ps or --page_size\033[0m" << std::endl;
                exit(-1);
            }
            if (ptype == "1G") {
                config.page_type = G1_HugePages;
            } else if (ptype == "2M") {
                config.page_type = M2_HugePages;
            }
        }

        config.alloc_amount = config.data_amount;
        return config;
    }

    //returns the size of the output
    size_t print_default(benchmark_config config){
        size_t element_size = config.result_file.length() * 1.1;
        if(element_size < 15){
            element_size = 15;
        }
        size_t total_size = (element_size) * 2 + 2; 

        size_t value_size = 4 + 4 * config.use_64bit_elements;

        double data = config.data_amount * value_size;
        int8_t k = 0;
        while(data >= 1024){
            data /= 1024;
            ++k;
        }
        std::stringstream res;
        res <<  std::setprecision(4) << data;
        switch(k){
            case 0: res << " B  "; break;
            case 1: res << " KiB"; break;
            case 2: res << " MiB"; break;
            case 3: res << " GiB"; break;
            case 4: res << " TiB"; break;
        }

        std::cout << std::string(total_size + 4, '#') << std::endl;
        std::cout << "# " << std::right << std::setw(element_size) << "data amount" << ": " << std::left << std::setw(element_size) << config.data_amount << " #" << std::endl;
        std::cout << "# " << std::right << std::setw(element_size) << "data size" << ": " << std::left << std::setw(element_size) << res.str() << " #" << std::endl;
        std::cout << "# " << std::right << std::setw(element_size) << "repeats" << ": " << std::left << std::setw(element_size) << config.repeats << " #" << std::endl;
        std::cout << "# " << std::right << std::setw(element_size) << "result file" << ": " << std::left << std::setw(element_size) << config.result_file << " #" << std::endl;
        if(config.use_64bit_elements){
            std::cout << "# " << std::string(element_size - 11, ' ')<< "Using 64 Bit Processing " << std::string(element_size - 11, ' ')  << " #" << std::endl;
        }else{
            std::cout << "# " << std::string(element_size - 11, ' ')<< "Using 32 Bit Processing " << std::string(element_size - 11, ' ')  << " #" << std::endl;
        }
        std::cout << "# " << std::string(total_size, '-') << " #" << std::endl;
        return element_size;
    }
}