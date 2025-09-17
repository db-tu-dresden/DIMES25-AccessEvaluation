#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

bool file_exists(std::string filename){
    std::ifstream f(filename.c_str());
    return f.good();
}

void write_to_file(std::string filename, std::string content, bool override = false, bool header=false){
    std::ofstream myfile;
    bool planed_not_open = false;
    if(override){
        myfile.open(filename, std::ios::out);
    }else{
        if(!(header && file_exists(filename))){ // print header only if the file doesn't exist
            myfile.open(filename, std::ios::app);
        }else{
            planed_not_open = true;
        }
    }

    if(myfile.is_open()){
        myfile << content << "\n";
        myfile.close();
    }else if(!planed_not_open){
        std::cout << filename << std::endl;
        throw std::runtime_error("Unable to open the file to write the results!\n");
    }
}

template<typename T>
std::string vector_to_string(std::vector<T> arg, char split = ','){
    std::stringstream res;
    for(size_t i = 0; i < arg.size(); i++){
        if(i != 0){
            res << split;
        }
        res << arg[i];
    }
    return res.str();
}

template<typename Arg>
std::string stringify(Arg&& arg){
    return (std::ostringstream() << arg).str();
}

template<typename Arg>
std::string stringify(char split, Arg&& arg){
    return (std::ostringstream() << arg).str();
}

template<typename T>
std::string stringify(std::vector<T> &&arg){
    std::stringstream stream;
    for(size_t i = 0; i < arg.size(); i++){
        if(i != 0){
            stream << ",";
        }
        stream << arg[i];
    }
    return stream.str();
}

template<typename T>
std::string stringify(char split, std::vector<T> &&arg){
    std::stringstream stream;
    for(size_t i = 0; i < arg.size(); i++){
        if(i != 0){
            stream << split;
        }
        stream << arg[i];
    }
    return stream.str();
}

template<typename First, typename ...Rest>
std::string stringify(First&& first, Rest&& ...rest){
    return (std::ostringstream() << stringify(first) << ',' << stringify(rest...)).str();
}

template<typename First, typename ...Rest>
std::string stringify(char split, First&& first, Rest&& ...rest){
    return (std::ostringstream() << stringify(first) << split << stringify(split, rest...)).str();
}
