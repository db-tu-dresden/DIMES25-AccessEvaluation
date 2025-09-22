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
