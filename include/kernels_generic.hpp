#pragma once

#include <iostream>
#include <cassert>
#include <stdlib.h>


// simple scalar sum
template <typename T>
T scalar(T* data, size_t data_amount){
    T result = 0;
    for(size_t i = 0; i < data_amount; i++){
            result += data[i];
    }
    return result;
}

template <typename T>
T scalar_strided(T * data, size_t data_amount, size_t stride){
    T result = 0;
    for(size_t i = 0; i < stride; i ++){
        for(size_t e = 0; i + e < data_amount; e += stride){
            result += data[i + e];
        }
    }
    return result;
}