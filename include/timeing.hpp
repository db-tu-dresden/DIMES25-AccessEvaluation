#ifndef TUD_DDRG_TIMEING_HPP
#define TUD_DDRG_TIMEING_HPP
#include <chrono>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <iomanip> 

namespace timeing{
    using time_stamp = std::chrono::steady_clock::time_point;

    time_stamp now(){
        return std::chrono::steady_clock::now();
    }

    // s * 10^0 = texp = 0
    uint64_t time_seconds(time_stamp begin, time_stamp end){
        return std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
    }

    // s * 10^-3 -> texp = -1
    uint64_t time_milliseconds(time_stamp begin, time_stamp end){
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

    // s * 10^-6 -> texp = -2
    uint64_t time_microseconds(time_stamp begin, time_stamp end){
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }

    // s * 10^-9 -> texp = -3
    uint64_t time_nanoseconds(time_stamp begin, time_stamp end){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    }

    namespace internal{
        uint64_t get_time(time_stamp begin, time_stamp end, int8_t &exp){
            switch(exp){
                case -4:    exp ++;
                case -3:    return time_nanoseconds(begin, end);    // nanoseconds
                case -2:    return time_microseconds(begin, end);   // microseconds
                case -1:    return time_milliseconds(begin, end);   // milliseconds
                case 0:     return time_seconds(begin, end);        // seconds
            }
            return 0;
        }
        
        // gives the time in <time> * 1000^<exp> second 
        void helper_time(time_stamp begin, time_stamp end, uint64_t &time, int8_t &exp){
            exp = -4;
            do{
                exp ++;
                time = get_time(begin, end, exp);
            }while(time > 1000 && exp != 0);
        }
    }
    

    template <typename T>
    double throughput(uint64_t time, int8_t t_exp, size_t element_count, int8_t goal_exp){
        double exp = pow(1024., goal_exp) / pow(1000., t_exp);
        double b_per_s = (sizeof(T) * element_count) / time;
        return b_per_s * exp;
    }

    template <typename T>
    double throughput(time_stamp begin, time_stamp end, size_t element_count, int8_t goal_exp){
        uint64_t time;
        int8_t t_exp;
        internal::helper_time(begin, end, time, t_exp);
        --t_exp;
        time = internal::get_time(begin, end, t_exp);
        return throughput<T>(time, t_exp, element_count, goal_exp);;
    }

    template<typename T>
    double gibs(time_stamp begin, time_stamp end, size_t element_count){
        return throughput<T>(begin, end, element_count, -3);
    }

    template<typename T>
    double gibs(uint64_t time, int8_t t_exp, size_t element_count){
        return throughput<T>(time, t_exp, element_count, -3);
    }

    template<typename T>
    double mibs(time_stamp begin, time_stamp end, size_t element_count){
        return throughput<T>(begin, end, element_count, -2);
    }

    template<typename T>
    double mibs(uint64_t time, int8_t t_exp, size_t element_count){
        return throughput<T>(time, t_exp, element_count, -2);
    }

    template<typename T>
    double kibs(time_stamp begin, time_stamp end, size_t element_count){
        return throughput<T>(begin, end, element_count, -1);
    }

    template<typename T>
    double kibs(uint64_t time, int8_t t_exp, size_t element_count){
        return throughput<T>(time, t_exp, element_count, -1);
    }

    template<typename T>
    double bs(time_stamp begin, time_stamp end, size_t element_count){
        return throughput<T>(begin, end, element_count, 0);
    }

    template<typename T>
    double bs(uint64_t time, int8_t t_exp, size_t element_count){
        return throughput<T>(time, t_exp, element_count, 0);
    }


    std::string throughput_string(double throughput){
        int8_t d_exp = 0;
        while(throughput >= 1024.){
            throughput /= 1024.;
            d_exp ++;
        }
        
        std::stringstream res;
        res << std::fixed << std::setprecision(2) << throughput << " ";
        switch(d_exp){
            case 0: res << "B/s  "; break;
            case 1: res << "KiB/s"; break;
            case 2: res << "MiB/s"; break;
            case 3: res << "GiB/s"; break;
            case 4: res << "TiB/s"; break;
            case 5: res << "PiB/s"; break;    
            case 6: res << "EiB/s"; break;
            default: return "error";
        }
        return res.str();
    }

    template<typename T>
    std::string throughput_string(time_stamp begin, time_stamp end, size_t element_count){
        double throughput = bs<T>(begin, end, element_count);
        return throughput_string(throughput);
    }

    template<typename T>
    std::string data_string(uint64_t element_count){
        double byte = element_count * sizeof(T);
        uint8_t k = 0;
        while(byte >= 1024){
            k++;
            byte /= 1024;
        }
        std::stringstream res;
        res << byte << " ";

        switch(k){
            case 0: res << "B  "; break;
            case 1: res << "KiB"; break;
            case 2: res << "MiB"; break;
            case 3: res << "GiB"; break;
            case 4: res << "TiB"; break;
            case 5: res << "PiB"; break;    
            case 6: res << "EiB"; break;
            default: return "error";
        }
        return res.str();
    }

    std::string time_string(uint64_t time, int8_t t_exp){
        while(time > 1000){
            time /= 1000;
            t_exp++;
        }

        std::stringstream res;
        res << time << " ";
        switch(t_exp){
            case  0: res << "s "; break;
            case -1: res << "ms"; break;
            case -2: res << "µs"; break;
            case -3: res << "ns"; break;
            default: return "error";
        }
        return res.str();
    }

    std::string time_string(time_stamp begin, time_stamp end){
        int8_t t_exp = 0;
        uint64_t time = 0;
        internal::helper_time(begin, end, time, t_exp);

        std::stringstream res;
        res << time << " ";
        switch(t_exp){
            case  0: res << "s "; break;
            case -1: res << "ms"; break;
            case -2: res << "µs"; break;
            case -3: res << "ns"; break;
            default: return "error";
        }
        return res.str();
    }

    /**
    *   exp { 0=seconds, -1=milliseconds, -2=microseconds, -3=nanoseconds} 
    **/
    std::string time_string(time_stamp begin, time_stamp end, int8_t exp){
        uint64_t time = internal::get_time(begin, end, exp);
        std::stringstream res;
        res << time << " ";
        switch(exp){
            case  0: res << "s "; break;
            case -1: res << "ms"; break;
            case -2: res << "µs"; break;
            case -3: res << "ns"; break;
            default: return "error";
        }
        return res.str();
    }
}
#endif