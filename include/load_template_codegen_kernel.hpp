#pragma once

#include <stdlib.h>
#ifdef ARM
#include <arm_sve.h>
#else
#include <immintrin.h>
#endif

#ifdef ARM

template<std::size_t N>
struct Internal_Load_Codegen {
    static svint64_t load_add(int64_t* data, std::size_t stride) {
        int64_t* current_ptr = data + stride * N;
        svint64_t current = svld1_s64(svptrue_b64(), current_ptr);
        svint64_t prev = Internal_Load_Codegen<N - 1>::load_add(data, stride);
        return svadd_s64_x(svptrue_b64(), prev, current);
    }
};

template<>
struct Internal_Load_Codegen<0> {
    static svint64_t load_add(int64_t* data, std::size_t stride) {
        return svld1_s64(svptrue_b64(), data);
    }
};

template<std::size_t N>
struct Load_Loop {
    static int64_t run(int64_t* data, std::size_t stride) {
        svint64_t result = svdup_s64(0);
        std::size_t vec_len = svcntd();
        for (std::size_t i = 0; i < stride; i += vec_len) {
            result = svadd_s64_x(svptrue_b64(), result, Internal_Load_Codegen<N - 1>::load_add(data + i, stride)
            );
        }
        return svaddv_s64(svptrue_b64(), result);
    }
};

#else

template<std::size_t N>
struct Internal_Load_Codegen{
    static __m512i load_add(int64_t* data, size_t stride){
        if constexpr (N > 0){
            return _mm512_add_epi64(Internal_Load_Codegen<N-1>::load_add(data, stride), _mm512_load_epi64( data + stride * N));
        }
        return _mm512_load_epi64( data + stride * N);
    }

    static __m512i load_add(int32_t* data, size_t stride){
        if constexpr (N > 0){
            return _mm512_add_epi32(Internal_Load_Codegen<N-1>::load_add(data, stride), _mm512_load_epi32( data + stride * N));
        }
        return _mm512_load_epi32( data + stride * N);
    }
};

template<std::size_t N>
struct Load_Loop{
    static int64_t run(int64_t* data, size_t stride){
        __m512i result = _mm512_set1_epi64(0);
        for(size_t i = 0; i < stride; i += 8){
            result = _mm512_add_epi64(Internal_Load_Codegen<N-1>::load_add(data + i, stride), result);
        }    
        return _mm512_reduce_add_epi64(result);
    } 

    static int64_t run(int32_t* data, size_t stride){
        __m512i result = _mm512_set1_epi32(0);
        for(size_t i = 0; i < stride; i += 16){
            result = _mm512_add_epi32(Internal_Load_Codegen<N-1>::load_add(data + i, stride), result);
        }    
        return _mm512_reduce_add_epi32(result);
    } 
};

#endif

int64_t load_lanes_codegen(int64_t* data, size_t data_amount, const size_t lane_count){
    size_t stride = data_amount / lane_count;
    int64_t result = 0;
    switch(lane_count){
        case 1:
            return Load_Loop<1>::run(data, stride);
        case 2:
            return Load_Loop<2>::run(data, stride);
        case 3:
            return Load_Loop<3>::run(data, stride);
        case 4:
            return Load_Loop<4>::run(data, stride);
        case 5:
            return Load_Loop<5>::run(data, stride);
        case 6:
            return Load_Loop<6>::run(data, stride);
        case 7:
            return Load_Loop<7>::run(data, stride);
        case 8:
            return Load_Loop<8>::run(data, stride);
        case 9:
            return Load_Loop<9>::run(data, stride);
        case 10:
            return Load_Loop<10>::run(data, stride);
        case 11:
            return Load_Loop<11>::run(data, stride);
        case 12:
            return Load_Loop<12>::run(data, stride);
        case 13:
            return Load_Loop<13>::run(data, stride);
        case 14:
            return Load_Loop<14>::run(data, stride);
        case 15:
            return Load_Loop<15>::run(data, stride);
        case 16:
            return Load_Loop<16>::run(data, stride);
        case 17:
            return Load_Loop<17>::run(data, stride);
        case 18:
            return Load_Loop<18>::run(data, stride);
        case 19:
            return Load_Loop<19>::run(data, stride);
        case 20:
            return Load_Loop<20>::run(data, stride);
        case 21:
            return Load_Loop<21>::run(data, stride);
        case 22:
            return Load_Loop<22>::run(data, stride);
        case 23:
            return Load_Loop<23>::run(data, stride);
        case 24:
            return Load_Loop<24>::run(data, stride);
        case 25:
            return Load_Loop<25>::run(data, stride);
        case 26:
            return Load_Loop<26>::run(data, stride);
        case 27:
            return Load_Loop<27>::run(data, stride);
        case 28:
            return Load_Loop<28>::run(data, stride);
        case 29:
            return Load_Loop<29>::run(data, stride);
        case 30:
            return Load_Loop<30>::run(data, stride);
        case 31:
            return Load_Loop<31>::run(data, stride);
        case 32:
            return Load_Loop<32>::run(data, stride);
    };
    return result;
}

int64_t load_lanes_codegen(int32_t* data, size_t data_amount, const size_t lane_count){
    size_t stride = data_amount / lane_count;
    int64_t result = 0;
    switch(lane_count){
        case 1:
            return Load_Loop<1>::run(data, stride);
        case 2:
            return Load_Loop<2>::run(data, stride);
        case 3:
            return Load_Loop<3>::run(data, stride);
        case 4:
            return Load_Loop<4>::run(data, stride);
        case 5:
            return Load_Loop<5>::run(data, stride);
        case 6:
            return Load_Loop<6>::run(data, stride);
        case 7:
            return Load_Loop<7>::run(data, stride);
        case 8:
            return Load_Loop<8>::run(data, stride);
        case 9:
            return Load_Loop<9>::run(data, stride);
        case 10:
            return Load_Loop<10>::run(data, stride);
        case 11:
            return Load_Loop<11>::run(data, stride);
        case 12:
            return Load_Loop<12>::run(data, stride);
        case 13:
            return Load_Loop<13>::run(data, stride);
        case 14:
            return Load_Loop<14>::run(data, stride);
        case 15:
            return Load_Loop<15>::run(data, stride);
        case 16:
            return Load_Loop<16>::run(data, stride);
        case 17:
            return Load_Loop<17>::run(data, stride);
        case 18:
            return Load_Loop<18>::run(data, stride);
        case 19:
            return Load_Loop<19>::run(data, stride);
        case 20:
            return Load_Loop<20>::run(data, stride);
        case 21:
            return Load_Loop<21>::run(data, stride);
        case 22:
            return Load_Loop<22>::run(data, stride);
        case 23:
            return Load_Loop<23>::run(data, stride);
        case 24:
            return Load_Loop<24>::run(data, stride);
        case 25:
            return Load_Loop<25>::run(data, stride);
        case 26:
            return Load_Loop<26>::run(data, stride);
        case 27:
            return Load_Loop<27>::run(data, stride);
        case 28:
            return Load_Loop<28>::run(data, stride);
        case 29:
            return Load_Loop<29>::run(data, stride);
        case 30:
            return Load_Loop<30>::run(data, stride);
        case 31:
            return Load_Loop<31>::run(data, stride);
        case 32:
            return Load_Loop<32>::run(data, stride);
    };
    return result;
}