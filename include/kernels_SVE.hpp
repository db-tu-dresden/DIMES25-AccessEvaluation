#pragma once

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <omp.h>
#include <arm_sve.h>

#include "scalar_template_codegen_kernel.hpp"
#include "load_template_codegen_kernel.hpp"

#define VECTOR_ELEMENT_COUNT 2


int64_t linear(int64_t* data, size_t data_amount) {
    svint64_t sum = svdup_s64(0);
    auto vec_elem = svcntd();
    for (size_t i = 0; i < data_amount; i += vec_elem) {
        svbool_t pg = svwhilelt_b64(i, data_amount);
        svint64_t vec = svld1_s64(pg, &data[i]);
        sum = svadd_s64_x(pg, sum, vec);
    }
    return svaddv_s64(svptrue_b64(), sum);
}


int64_t linear_gather(int64_t* data, size_t data_amount) {
    svint64_t sum = svdup_s64(0);
    auto vec_elem = svcntd();
    for (size_t i = 0; i < data_amount; i+=vec_elem) {
        svbool_t pg = svwhilelt_b64(i, data_amount);
        svuint64_t offsets = svindex_u64(i, 1);
        svint64_t vec = svld1_gather_u64index_s64(pg, data, offsets);
        sum = svadd_s64_m(pg, sum, vec);
    }
    return svaddv_s64(svptrue_b64(), sum);
}


int64_t strided_block_gather(int64_t* data, size_t data_amount, size_t stride) {
    svint64_t sum = svdup_s64(0);
    auto vec_elem = svcntd();
    for (size_t i = 0U; i < data_amount; i += (stride * vec_elem)) {
        for (size_t j = i; j < i + stride; j++) {
            svbool_t pg = svwhilelt_b64(j, data_amount);
            svuint64_t offsets = svindex_u64(j, stride);
            svint64_t vec = svld1_gather_u64index_s64(pg, data, offsets);
            sum = svadd_s64_m(pg, sum, vec);
        }
    }
    return svaddv_s64(svptrue_b64(), sum);
}

// WARNING: untested!!!
// int64_t strided_full_gather(int64_t* data, size_t data_amount, size_t stride) {
//     svint64_t sum = svdup_s64(0);
//     auto vec_elem = svcntd();
//     for (size_t j = 0U; j < stride; j++) {
//         for (size_t i = 0U; i + j < data_amount; i += (stride * vec_elem)) {
//             svbool_t pg = svwhilelt_b64(i, data_amount);
//             svuint64_t offsets = svindex_u64(i, stride);
//             svint64_t vec = svld1_gather_u64index_s64(pg, data, offsets);
//             sum = svadd_s64_m(pg, sum, vec);
//         }
//     }
//     return svaddv_s64(svptrue_b64(), sum);
// }

int64_t strided_dynamic_gather(int64_t *data, size_t data_amount){
    svint64_t sum = svdup_s64(0);
    size_t stride = data_amount / svcntd();
    for(size_t i = 0; i < stride; i++){
        svuint64_t offsets = svindex_u64(i, stride);
        svint64_t load = svld1_gather_u64index_s64(svptrue_b64(), data, offsets);
        sum = svadd_s64_x(svptrue_b64(), sum, load);
    }
    return svaddv_s64(svptrue_b64(), sum);
}

void add_sve(svint64_t &a, svint64_t &b) {
    a = svadd_s64_x(svptrue_b64(), a, b);
}

#pragma omp declare reduction(simd_add : svint64_t : add_sve(omp_out, omp_in)) initializer(omp_priv = omp_orig)