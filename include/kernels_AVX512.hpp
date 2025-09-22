#pragma once

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <omp.h>
#include <immintrin.h>

#include "scalar_template_codegen_kernel.hpp"

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
