#pragma once

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <omp.h>
#include <immintrin.h>

#include "scalar_template_codegen_kernel.hpp"
#include "load_template_codegen_kernel.hpp"

#define VECTOR_ELEMENT_COUNT 8


// simple avx512 load sum
int64_t linear(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_load_epi64(&data[i]);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

// stream load avx512 + sum
int64_t stream(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_stream_load_si512(&data[i]);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

// using the gather instruction as a load
int64_t linear_gather(int64_t* data, size_t data_amount){
    __m512i result = _mm512_set1_epi64(0);
    __m512i index = _mm512_setr_epi64(0, 1, 2, 3, 4, 5, 6, 7);

    for(size_t i = 0; i < data_amount; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_i64gather_epi64(index, &data[i], 8);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

// using a fixed stride amount and running over all the data in one stride.
// afterwards we jump to the next block where process the same way again!
// !IMPORTANT We might not process every data point, OR access illegal regions!
int64_t strided_block_gather(int64_t *data, size_t data_amount, size_t stride){
    __m512i result = _mm512_set1_epi64(0);
    __m512i index =  _mm512_setr_epi64(0, stride, 2 * stride, 3 * stride, 4 * stride, 5 * stride, 6 * stride, 7 * stride);
    for(size_t i = 0; i < data_amount; i += (stride * VECTOR_ELEMENT_COUNT)){
        for(size_t s = 0; s < stride; s++){
            __m512i load = _mm512_i64gather_epi64(index, data + i + s, 8);
            result = _mm512_add_epi64(result, load);
        }
    }
    return _mm512_reduce_add_epi64(result);
}

// similar to the block only that we process the first element of all blocks first
// afterwards we jump to the next element, ...
// PLEASE BE AWARE THIS IS A "BAD" IDEA!
int64_t strided_full_gather(int64_t *data, size_t data_amount, size_t stride){
    __m512i result = _mm512_set1_epi64(0);
    __m512i index =  _mm512_setr_epi64(0, stride, 2 * stride, 3 * stride, 4 * stride, 5 * stride, 6 * stride, 7 * stride);
    for(size_t s = 0; s < stride; s++){
        for(size_t i = 0; i + s < data_amount; i += (stride * VECTOR_ELEMENT_COUNT)){
            __m512i load = _mm512_i64gather_epi64(index, data + i + s, 8);
            result = _mm512_add_epi64(result, load);
        }
    }
    return _mm512_reduce_add_epi64(result);
}

// We select a stride size such that each lane gets one continus block of data. 
// This allows for SIMD_VECTOR_ELEMENT linear processings.
int64_t strided_dynamic_gather(int64_t *data, size_t data_amount){
    __m512i result = _mm512_set1_epi64(0);
    size_t stride = data_amount / VECTOR_ELEMENT_COUNT;
    __m512i index =  _mm512_setr_epi64(0, stride, 2 * stride, 3 * stride, 4 * stride, 5 * stride, 6 * stride, 7 * stride);
    for(size_t i = 0; i < stride; i++){
        __m512i load = _mm512_i64gather_epi64(index, data + i, 8);
            result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}


void add_simd(__m512i &inout, __m512i &in) {
    inout = _mm512_add_epi64(inout, in);
}

#pragma omp declare reduction(                          \
                              simd_add :                \
                              __m512i :                 \
                              add_simd(omp_out, omp_in) \
                             )                          \
                    initializer (omp_priv=omp_orig)


int64_t sisd_sum(int64_t* data, size_t size){
    int64_t result = 0;
    for(size_t i = 0; i < size; i ++){
        result += data[i];
    }
    return result;
}

int64_t mimd_sum(int64_t* data, size_t size, size_t thread_count){
    int64_t result = 0;
    #pragma omp parallel for num_threads(thread_count) reduction(+: result)
    for(size_t i = 0; i < size; i ++){
        result += data[i];
    }
    return result;
}

int64_t simd_sum_load(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_load_epi64(&data[i]);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

int64_t simd_mimd_sum_load(int64_t* data, size_t size, size_t thread_count){
    __m512i result = _mm512_set1_epi64(0);
    #pragma omp parallel for num_threads(thread_count) reduction(simd_add : result)
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_load_epi64(&data[i]);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}


int64_t simd_sum_gather_as_load(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    __m512i index = _mm512_set_epi64(0, 1, 2, 3, 4, 5, 6, 7);
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_i64gather_epi64(index, &data[i], 8);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

int64_t simd_sum_gather_as_load_reverse(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    __m512i index = _mm512_setr_epi64(0, 1, 2, 3, 4, 5, 6, 7);
    for(size_t i = 0; i < size; i += VECTOR_ELEMENT_COUNT){
        __m512i load = _mm512_i64gather_epi64(index, &data[i], 8);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

int64_t simd_sum_gather_total_stride(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    const size_t stride = size / VECTOR_ELEMENT_COUNT;
    __m512i index = _mm512_set_epi64(0, stride, stride * 2, stride * 3, 
        stride * 4, stride * 5, stride * 6, stride * 7);

    for(size_t i = 0; i < stride; i++){
        __m512i load = _mm512_i64gather_epi64(index, &data[i], 8);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

int64_t simd_sum_gather_total_stride_reverse(int64_t* data, size_t size){
    __m512i result = _mm512_set1_epi64(0);
    const size_t stride = size / VECTOR_ELEMENT_COUNT;
    __m512i index = _mm512_setr_epi64(0, stride, stride * 2, stride * 3, 
        stride * 4, stride * 5, stride * 6, stride * 7);

    for(size_t i = 0; i < stride; i++){
        __m512i load = _mm512_i64gather_epi64(index, &data[i], 8);
        result = _mm512_add_epi64(result, load);
    }
    return _mm512_reduce_add_epi64(result);
}

int64_t simd_sum_gather_total_stride_batched(int64_t* data, size_t size, size_t batch_count, size_t run_length){
    assert(size % VECTOR_ELEMENT_COUNT == 0);
    __m512i result = _mm512_set1_epi64(0);
    const size_t stride = size / VECTOR_ELEMENT_COUNT;
    assert(stride % batch_count == 0);
    const size_t max_run_length = (stride + batch_count - 1)/batch_count;
    __m512i stride_index = _mm512_set_epi64(0, stride, stride * 2, stride * 3, 
        stride * 4, stride * 5, stride * 6, stride * 7);


    for(size_t index = 0; index < max_run_length; index += run_length){
        for(size_t batch = 0; batch < batch_count; batch ++){
            for(size_t local_index = 0; local_index < run_length && local_index + index < max_run_length; local_index++){
                __m512i load = _mm512_i64gather_epi64(stride_index, &data[local_index + index + batch * max_run_length], 8);
                result = _mm512_add_epi64(result, load);
            }
        }
    }
    return _mm512_reduce_add_epi64(result);
}