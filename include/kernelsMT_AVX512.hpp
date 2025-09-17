#pragma once

#include <stdlib.h>
#include <immintrin.h>
#include <omp.h>
#include <iostream>
#include <cassert>
#include <scalar_template_codegen_kernel.hpp>

// ##########################################
// # Function and Reduction for omp         #
// ##########################################

// 512 bit vector with 64 bit data
void helper_simd_512_add_64(__m512i &inout, __m512i &in) {
    inout = _mm512_add_epi64(inout, in);
}

#pragma omp declare reduction(                  \
        simd_512_add_64 :                       \
        __m512i :                               \
        helper_simd_512_add_64(omp_out, omp_in) \
    )                                           \
    initializer (omp_priv=omp_orig)


// 512 bit vector with 32 bit data
void helper_simd_512_add_32(__m512i &inout, __m512i &in) {
    inout = _mm512_add_epi32(inout, in);
}

#pragma omp declare reduction(                  \
        simd_512_add_32 :                       \
        __m512i :                               \
        helper_simd_512_add_32(omp_out, omp_in) \
    )                                           \
    initializer (omp_priv=omp_orig)


// 256 bit vector with 64 bit data
void helper_simd_256_add_64(__m256i &inout, __m256i &in) {
    inout = _mm256_add_epi64(inout, in);
}

#pragma omp declare reduction(                  \
        simd_256_add_64 :                       \
        __m256i :                               \
        helper_simd_256_add_64(omp_out, omp_in) \
    )                                           \
    initializer (omp_priv=omp_orig)


// 256 bit vector with 32 bit data
void helper_simd_256_add_32(__m256i &inout, __m256i &in) {
    inout = _mm256_add_epi32(inout, in);
}

#pragma omp declare reduction(                  \
        simd_256_add_32 :                       \
        __m256i :                               \
        helper_simd_256_add_32(omp_out, omp_in) \
    )                                           \
    initializer (omp_priv=omp_orig)


// ##########################################
// # KERNELS                                #
// ##########################################
// KERNELS! IMPORTANT data_amount is the data_amount for a given thread

// simple scalar sum
template <typename T> 
struct scalar_MT{
    static const size_t vector_element_count = 1;
    static int64_t run(T* data, size_t data_amount, size_t thread_count){
        T result = 0;
        #pragma omp parallel for num_threads(thread_count) reduction(+: result)    
        for(size_t i = 0; i < data_amount * thread_count; i++){
            result += data[i];
        }
        return result;
    }
};

// strided scalar sum
template <typename T> 
struct scalar_strided_MT{
    static const size_t vector_element_count = 1;
    static int64_t run(T* data, size_t data_amount, size_t thread_count, size_t partition_count){
        T result = 0;
        const size_t complete_size = data_amount * thread_count;
        size_t thread_size = data_amount;
        size_t stride = (thread_size + partition_count - 1) / partition_count;
        
        #pragma omp parallel num_threads(thread_count) reduction(+: result)   
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = thread_size * thread_id;
            size_t discard = thread_size * (thread_id + 1);
            const size_t thread_end_index = discard > complete_size? data_amount: discard;

            for(size_t i = 0; i < stride; i ++){
                for(size_t e = thread_start_index; i + e < thread_end_index; e += stride){
                    result += data[i + e];
                }
            }
        }    
        return result;
    }
};

template <typename T> 
struct unrolled_strided_MT{
    static const size_t vector_element_count = 1;
    static int64_t run(T* data, size_t data_amount, size_t thread_count, size_t partition_count){
        T result = 0;
        const size_t complete_size = data_amount * thread_count;
        size_t thread_size = data_amount;
        size_t stride = (thread_size + partition_count - 1) / partition_count;
        
        #pragma omp parallel num_threads(thread_count) reduction(+: result)   
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = thread_size * thread_id;
            result = scalar_lanes_codegen_v2<T>(&data[thread_start_index], data_amount, partition_count);
        }    
        return result;
    }
};
    
template<typename T, typename S> 
struct linear_MT{
    static const size_t vector_element_count = 1;
    static int64_t run(T* data, size_t data_amount, size_t thread_count){
        std::cout << "For the given Template Parameters exist no Kernel\t" << sizeof(T) << " " << sizeof(S) << std::endl;
        return scalar_MT<T>::run(data, data_amount, thread_count);
    };
};

template<>
struct linear_MT<int64_t, __m512i>{
    static const size_t vector_element_count = 8;
    static int64_t run(int64_t* data, size_t data_amount, size_t thread_count){
        __m512i result = _mm512_set1_epi64(0);
        size_t partition_size = data_amount;
        #pragma omp parallel num_threads(thread_count) reduction(simd_512_add_64: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + partition_size;
            
            for(size_t i = thread_start_index; i < thread_end_index; i += vector_element_count){
                __m512i load = _mm512_load_epi64(&data[i]);
                result = _mm512_add_epi64(result, load);
            }
        }
        return _mm512_reduce_add_epi64(result);
    }
};

template<>
struct linear_MT<int64_t, __m256i>{
    static const size_t vector_element_count = 4;
    static int64_t run(int64_t* data, size_t data_amount, size_t thread_count){
        __m256i result = _mm256_set1_epi64x(0);
        size_t partition_size = data_amount;
        #pragma omp parallel num_threads(thread_count) reduction(simd_256_add_64: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + partition_size;
            
            for(size_t i = thread_start_index; i < thread_end_index; i += vector_element_count){
                __m256i load = _mm256_load_epi64(&data[i]);
                result = _mm256_add_epi64(result, load);
            }
        }
        return ((int64_t*)&result)[0] 
            + ((int64_t*)&result)[1] 
            + ((int64_t*)&result)[2] 
            + ((int64_t*)&result)[3];
    }
};

template<>
struct linear_MT<int32_t, __m512i>{
    static const size_t vector_element_count = 16;
    static int64_t run(int32_t* data, size_t data_amount, size_t thread_count){
        __m512i result = _mm512_set1_epi32(0);
        size_t partition_size = data_amount;
        #pragma omp parallel num_threads(thread_count) reduction(simd_512_add_32: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + partition_size;
            
            for(size_t i = thread_start_index; i < thread_end_index; i += vector_element_count){
                __m512i load = _mm512_load_epi32(&data[i]);
                result = _mm512_add_epi32(result, load);
            }
        }
        return _mm512_reduce_add_epi32(result);
    }
};

template<>
struct linear_MT<int32_t, __m256i>{
    static const size_t vector_element_count = 8;
    static int64_t run(int32_t* data, size_t data_amount, size_t thread_count){
        __m256i result = _mm256_set1_epi32(0);
        size_t partition_size = data_amount;
        #pragma omp parallel num_threads(thread_count) reduction(simd_256_add_32: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + partition_size;
            
            for(size_t i = thread_start_index; i < thread_end_index; i += vector_element_count){
                __m256i load = _mm256_load_epi32(&data[i]);
                result = _mm256_add_epi32(result, load);
            }
        }

        result = _mm256_hadd_epi32(result, result);
        result = _mm256_hadd_epi32(result, result);

        return ((int32_t*)&result)[0]
            + ((int32_t*)&result)[5];
    }
};

template<typename T, typename S> 
struct gather_MT{
    static const size_t vector_element_count = 1;
    static int64_t run(T* data, size_t data_amount, size_t thread_count){
        std::cout << "For the given Template Parameters exist no Kernel\t" << sizeof(T) << " " << sizeof(S) << std::endl;
        return scalar_MT<T>::run(data, data_amount, thread_count);
    }
};

template<> 
struct gather_MT<int64_t, __m512i>{
    static const size_t vector_element_count = 8;
    static int64_t run(int64_t* data, size_t data_amount, size_t thread_count){
        __m512i result = _mm512_set1_epi64(0);
        size_t partition_size = data_amount;
        size_t stride = partition_size / vector_element_count;
        __m512i index =  _mm512_setr_epi64(0, stride, 2 * stride, 3 * stride, 4 * stride, 5 * stride, 6 * stride, 7 * stride);
        
        #pragma omp parallel num_threads(thread_count) reduction(simd_512_add_64: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + stride;

            for(size_t i = thread_start_index; i < thread_end_index; i++){
                __m512i load = _mm512_i64gather_epi64(index, data + i, 8);
                result = _mm512_add_epi64(result, load);
            }
        }    

        return _mm512_reduce_add_epi64(result); 
    }
};

template<> 
struct gather_MT<int64_t, __m256i>{
    static const size_t vector_element_count = 4;
    static int64_t run(int64_t* data, size_t data_amount, size_t thread_count){
        
        __m256i result = _mm256_set1_epi64x(0);
        size_t partition_size = data_amount;
        size_t stride = partition_size / vector_element_count;
        __m256i index =  _mm256_set_epi64x(0, stride, 2 * stride, 3 * stride);
        
        #pragma omp parallel num_threads(thread_count) reduction(simd_256_add_64: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + stride;

            for(size_t i = thread_start_index; i < thread_end_index; i++){
                __m256i load = _mm256_i64gather_epi64(reinterpret_cast<const long long int*>(data + i), index, 8);
                result = _mm256_add_epi64(result, load);
            }
        }    

        return ((int64_t*)&result)[0] 
            + ((int64_t*)&result)[1] 
            + ((int64_t*)&result)[2] 
            + ((int64_t*)&result)[3];
    }
};

template<> 
struct gather_MT<int32_t, __m512i>{
    static const size_t vector_element_count = 16;
    static int64_t run(int32_t* data, size_t data_amount, size_t thread_count){
        __m512i result = _mm512_set1_epi32(0);
        size_t partition_size = data_amount;
        size_t stride = partition_size / vector_element_count;
        __m512i index =  _mm512_setr_epi32(0, stride, 2 * stride, 3 * stride, 
                                            4 * stride, 5 * stride, 6 * stride, 7 * stride, 
                                            8 * stride, 9 * stride, 10 * stride, 11 * stride, 
                                            12 * stride, 13 * stride, 14 * stride, 15 * stride);
        #pragma omp parallel num_threads(thread_count) reduction(simd_512_add_32: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + stride;

            for(size_t i = thread_start_index; i < thread_end_index; i++){
                __m512i load = _mm512_i32gather_epi32(index, data + i, 4);
                result = _mm512_add_epi32(result, load);
            }
        }    

        return _mm512_reduce_add_epi32(result); 
    }
};

template<> 
struct gather_MT<int32_t, __m256i>{
    static const size_t vector_element_count = 8;
    static int64_t run(int32_t* data, size_t data_amount, size_t thread_count){
        
        __m256i result = _mm256_set1_epi32(0);
        size_t partition_size = data_amount;
        size_t stride = partition_size / vector_element_count;
        __m256i index =  _mm256_setr_epi32(0, stride, 2 * stride, 3 * stride, 
                                        4 * stride, 5 * stride, 6 * stride, 7 * stride);
        #pragma omp parallel num_threads(thread_count) reduction(simd_256_add_32: result)    
        {
            const size_t thread_id = omp_get_thread_num();
            const size_t thread_start_index = partition_size * thread_id;
            const size_t thread_end_index = thread_start_index + stride;

            for(size_t i = thread_start_index; i < thread_end_index; i++){
                __m256i load = _mm256_i32gather_epi32(data + i, index, 4);
                result = _mm256_add_epi32(result, load);
            }
        }
        result = _mm256_hadd_epi32(result, result);
        result = _mm256_hadd_epi32(result, result);

        return ((int32_t*)&result)[0]
            + ((int32_t*)&result)[5];
    }
};