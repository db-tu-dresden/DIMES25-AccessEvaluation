#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <set>
#include <unordered_set>
#include <allocator.hpp>

#include <perfcpp/event_counter.h>
#include <default_config.hpp>
#include <limits>

struct run_data {
    config::run_config run_conf;
    perf::CounterResult couter_res;
    data_t res;
};


template<typename T>
class OrderedSet {
public:
    bool insert(const T& value) {
        if (_set.count(value)) return false;
        _set.insert(value);
        _order.push_back(value);
        return true;
    }

    bool contains(const T& value) const {
        return _set.count(value);
    }

    const std::vector<T>& ordered() const {
        return _order;
    }

private:
    std::unordered_set<T> _set;
    std::vector<T> _order;
};


class ResultPrinter {
public:
    ResultPrinter(config::benchmark_config bc, bool print = true) : _benchmark_config(bc), _print(print) {
        std::filesystem::create_directories(std::filesystem::path(_benchmark_config.file_and_path).parent_path());

        _csv_file.open(_benchmark_config.file_and_path);
        if (!_csv_file) {
            throw std::runtime_error("Could not open CSV file: " + _benchmark_config.file_and_path);
        }
        _supported_metrics.insert("gibs");
    }

    ~ResultPrinter(){
        _csv_file.close();
    }

    void add_columns(const std::vector<std::string> &columns){
        _custom_headline = true;
        for (auto &column : columns){
            if (!(column == "checksum" || column == "algorithm")){
                _columns.insert(column);
            }
        }
    }

    void add_metrics(const std::vector<std::string> &metrics){
        _custom_headline = true;
        for (auto &metric : metrics){
            if (_supported_metrics.contains(metric)){
                _columns.insert(metric);
            }
        }
    }

    double add_run_results(config::run_config &rc, const perf::CounterResult &cr, data_t &res) {
        if (!_header) {
            print_header();
            if (!_custom_headline){
                print_headline(rc, cr);
            } else {
                print_headline(cr);
            }
            _header = true;
        }
        print_static_data(rc, res);
        print_run_config(rc);
        double gibytes_per_sec = print_metrics(cr, rc);
        print_counter(cr);
        _csv_file.flush();
        return gibytes_per_sec;
    }

private:

    void print_header(size_t column_size = 0) {
        std::string line(150, '#');
        std::cout << line << '\n';
        std::cout << "#  " << std::setw(15) << "Elements : "  << std::setw(50) << std::left << std::to_string(_benchmark_config.data_amount)
                  << "#  " << std::setw(15) << std::right << "Repetitions: " << std::setw(63) << std::left << std::to_string(_benchmark_config.repeats) << "#\n";
        std::cout << "#  " << std::setw(15) << std::right << "Result File: " << std::setw(50) << std::left << _benchmark_config.file_and_path
                  << "#  " << std::setw(15) << std::right << "Page Size: " << std::setw(63) << std::left <<  get_page_size() << "#\n";
        std::cout  << std::left << line << '\n';
    }

    void print_headline(const config::run_config &rc, const perf::CounterResult &cr){
        std::cout << std::left << std::setw(30) << "algorithm";
        _csv_file << "algorithm" << ",";
        _csv_file << "data_amount" << ",";
        _csv_file << "byte_count" << ",";
        std::cout << std::left << std::setw(20) << "checksum";
        _csv_file << "checksum" << ",";
        if (rc.stride != 0){
            std::cout << std::left << std::setw(20) << "stride";
            _csv_file << "stride" << ",";
        }
        if (rc.sub != 0){
            std::cout << std::left << std::setw(20) << "sub";
            _csv_file << "sub" << ",";
        }
        if (rc.lane_count != 0){
            std::cout << std::left << std::setw(20) << "lane_count";
            _csv_file << "lane_count" << ",";
        }
        if (rc.batch_count != 0){
            std::cout << std::left << std::setw(20) << "batch_count";
            _csv_file << "batch_count" << ",";
        }
        if (rc.run_length != 0){
            std::cout << std::left << std::setw(20) << "run_length";
            _csv_file << "run_length" << ",";
        }
        if (rc.real_data_amount != 0){
            std::cout << std::left << std::setw(20) << "real_data_amount";
            _csv_file << "real_data_amount" << ",";
        }
        for (const auto &res : cr){
            std::cout << std::left << std::setw(20) << res.first;
            _csv_file << "," << res.first;
        }
        std::cout << "\n";
        _csv_file << "\n";
    }

    void print_headline(const perf::CounterResult &cr){
        std::cout << std::left << std::setw(30) << "algorithm";
        _csv_file << "algorithm" << ",";
        _csv_file << "data_amount" << ",";
        _csv_file << "byte_count" << ",";
        std::cout << std::left << std::setw(20) << "checksum";
        _csv_file << "checksum";
        for (auto &col : _columns.ordered()){
            std::cout << std::setw(20) << col;
            _csv_file  << "," << col;
        }
        for (const auto &res : cr){
            if (res.first == "nanoseconds"){
                std::cout << std::left << std::setw(20) << "time";
                _csv_file  << ","<< "time_ns";
            } else {
                std::cout << std::left << std::setw(20) << res.first;
                _csv_file  << ","<< res.first;
            }
        }
        std::cout << "\n";
        _csv_file << "\n";
    }

    void print_static_data(config::run_config &rc, data_t &result){
        std::cout << std::left << std::setw(30) << rc.name;
        _csv_file << rc.name;
        _csv_file << "," << _benchmark_config.data_amount;
        if(rc.real_element_byte == 0){
            _csv_file << "," << sizeof(data_t);
        }else{
            _csv_file << "," << rc.real_element_byte;
        }
        std::cout << std::left << std::setw(20) << compute_checksum(result);
        _csv_file << "," << compute_checksum(result);
    }

    void print_run_config(config::run_config &rc){
        if (rc.stride != 0 || _columns.contains("stride")){
            std::cout << std::left << std::setw(20) << (rc.stride == 0 ? "-" : std::to_string(rc.stride));
            _csv_file << "," << rc.stride;
        }
        if (rc.sub != 0 || _columns.contains("sub")){
            std::cout << std::left << std::setw(20) << rc.sub;
            _csv_file << "," << rc.sub;
        }
        if (rc.lane_count != 0 || _columns.contains("lane_count")){
            std::cout << std::left << std::setw(20) << rc.lane_count;
            _csv_file << "," << rc.lane_count;
        }
        if (rc.batch_count != 0 || _columns.contains("batch_count")){
            std::cout << std::left << std::setw(20) <<  (rc.batch_count == 0 ? "-" : std::to_string(rc.batch_count));
            _csv_file << "," << rc.batch_count;
        }
        if (rc.run_length != 0 || _columns.contains("run_length")){
            std::cout << std::left << std::setw(20) <<  (rc.run_length == 0 ? "-" : std::to_string(rc.run_length));
            _csv_file << "," << rc.run_length;
        }
        if (rc.real_data_amount != 0 || _columns.contains("real_data_amount")){
            std::cout << std::left << std::setw(20) << rc.real_data_amount;
            _csv_file << "," << rc.real_data_amount;
        }
    }

    double print_metrics(const perf::CounterResult &cr, config::run_config &rc){
        if (_columns.contains("gibs")){
            auto time = cr.get("nanoseconds");
            size_t element_size = rc.real_element_byte == 0? sizeof(data_t) : rc.real_element_byte;
            auto data_amount = rc.real_data_amount == 0 ? _benchmark_config.data_amount * element_size : rc.real_data_amount * element_size;
            std::string throughput_string = get_throughput_string(time.value(), data_amount);
            double res = get_throughput_in_gibs(time.value(), data_amount);
            std::cout << std::left << std::setw(20) << throughput_string;
            _csv_file << "," << res;
            return res;
        }
        return 0;
    }

    void print_counter(const perf::CounterResult &cr) {
        for (const auto &res : cr) {
            if (res.first == "nanoseconds"){
                std::cout << std::left << std::setw(20) << get_time_string(res.second);
            } else {
                std::cout << std::left << std::setw(20) << res.second;
            }
            _csv_file << "," << res.second;
        }
        std::cout << "\n";
        _csv_file << "\n";
    }

    size_t compute_checksum(const data_t &result){
        size_t check_sum = 0;
        check_sum = (check_sum << 13) ^ result;
        check_sum = (check_sum >> 17) ^ check_sum;
        check_sum = (check_sum << 5) ^ check_sum;
        return check_sum & 0x3FFFFFF;
    }

    std::string get_page_size(){
        if (_benchmark_config.page_type == M2_HugePages){
            return "2M";
        } else if (_benchmark_config.page_type == G1_HugePages){
            return "1G";
        }
        return "4K";
    }

    std::string get_throughput_string(double nanoseconds, size_t data_in_bytes){
        if (nanoseconds <= 0.0) {
            return "inv";
        }

        double seconds = nanoseconds * 1e-9;
        double bytes_per_sec = static_cast<double>(data_in_bytes) / seconds;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        
        if (bytes_per_sec >= 1024.0 * 1024 * 1024) {
            oss << (bytes_per_sec / (1024.0 * 1024 * 1024)) << " GiB/s";
        } else if (bytes_per_sec >= 1024.0 * 1024) {
            oss << (bytes_per_sec / (1024.0 * 1024)) << " MiB/s";
        } else if (bytes_per_sec >= 1024.0 * 1024) {
            oss << (bytes_per_sec / 1024.0) << " KiB/s";
        } else {
            oss << bytes_per_sec << " B/s  ";
        }
        return oss.str();
    }

    double get_throughput_in_gibs(double nanoseconds, size_t data_in_bytes){
        if (nanoseconds <= 0.0) {
            return std::numeric_limits<double>::max();
        }

        double seconds = nanoseconds * 1e-9;
        double bytes_per_sec = static_cast<double>(data_in_bytes) / seconds;
        double gibytes_per_sec = bytes_per_sec / (1024.0 * 1024.0 * 1024.0);
        return gibytes_per_sec;
    }

    std::string get_time_string(double nanoseconds){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);

        if (nanoseconds >= 1e9) {
            oss << nanoseconds / 1e9 << " s";
        } else if (nanoseconds >= 1e6) {
            oss << nanoseconds / 1e6 << " ms";
        } else if (nanoseconds >= 1e3) {
            oss << nanoseconds / 1e3 << " µs";
        } else {
            oss << nanoseconds << " ns";
        }

        return oss.str();
    }

    static std::string size_to_GiB(size_t bytes) {
        double gib = static_cast<double>(bytes) / (1024 * 1024 * 1024);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << gib << " GiB";
        return oss.str();
    }

    static std::string size_to_MiB(size_t bytes) {
        double mib = static_cast<double>(bytes) / (1024 * 1024);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << mib << "MiB";
        return oss.str();
    }

    static std::string size_to_KiB(size_t bytes) {
        double kib = static_cast<double>(bytes) / 1024;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << kib << "KiB";
        return oss.str();
    }

    bool _print = true;
    bool _header = false;
    bool _custom_headline = false;
    std::ofstream _csv_file;
    config::benchmark_config _benchmark_config;
    OrderedSet<std::string> _columns;
    std::set<std::string> _supported_metrics;
    std::vector<run_data> _benchmark_results;
};