#pragma once

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include "kernels_interface.hpp"


template <typename T>
T scalar_strided_mod(T * data, size_t data_amount, size_t stride){
    while(ggT(data_amount, stride) != 1){
        stride --;
    }
    T result = 0;
    for(size_t i = 0; i < data_amount; i ++){
        result += data[(i * stride) % data_amount];
    }
    return result;
}

// simple scalar sum
template <typename T>
T scalar(T* data, size_t data_amount){
    T result = 0;
    for(size_t i = 0; i < data_amount; i++){
            result += data[i];
    }
    return result;
}

// First try at a new scalar variant as described in Issue #12
// Our hope is that we achieve a higher throughput with this
// manually "unrolled" loop. The prefetcher (?) hopefully loads
// now 8 streams for us.
template<typename T>
T scalar_8_lanes(T* data, size_t data_amount){
    size_t stride = data_amount / 8;
    T result = 0;
    for(size_t i = 0; i < stride; i++){
        result += data[i];
        result += data[i + stride];
        result += data[i + 2 * stride];
        result += data[i + 3 * stride];
        result += data[i + 4 * stride];
        result += data[i + 5 * stride];
        result += data[i + 6 * stride];
        result += data[i + 7 * stride];
    }
    return result;
}

template <typename T>
T scalar_2_lanes(T* data, size_t data_amount){
    size_t stride = data_amount / 2;
    T result = 0;
    for(size_t i = 0; i < stride; i++){
        result += data[i];
        result += data[i + stride];
    }
    return result;
}

template <typename T>
T scalar_front_and_back(T * data, size_t data_amount){
    size_t stride = data_amount / 2;
    size_t start_back = data_amount -1;
    T result = 0;
    for(size_t i = 0; i < stride; i++){
        result += data[i] + data[start_back - i];
    }
    return result;
}

template <typename T>
T scalar_lanes(T* data, size_t data_amount, size_t lane_count){
    size_t stride = data_amount / lane_count;
    T result = 0;
    size_t lane_stride[lane_count];
    for(size_t i = 0; i < lane_count; i++){
        lane_stride[i] = i * stride;
    }
    for(size_t i = 0; i < stride; i++){
        for(size_t lane = 0; lane < lane_count; lane++){
            result += data[i + lane_stride[lane]];   
        }
    }
    return result;
}

template <typename T>
T scalar_strided(T * data, size_t data_amount, size_t stride){
    T result = 0;
    for(size_t i = 0; i < stride; i ++){
        for(size_t e = i; e < data_amount; e += stride){
            result += data[e];
        }
    }
    return result;
}

template <typename T>
T scalar_strided_v2(T * data, size_t data_amount, size_t stride){
    T result = 0;
    for(size_t i = 0; i < stride; i ++){
        for(size_t e = 0; i + e < data_amount; e += stride){
            result += data[i + e];
        }
    }
    return result;
}

template <typename T>
T scalar_strided_v3(T * data, size_t data_amount, size_t stride){
    T result = 0;
    for(size_t i = 0; i < stride; i ++){
        for(size_t e = 0; e < data_amount; e += stride){
            result += data[i + e];
        }
    }
    return result;
}
