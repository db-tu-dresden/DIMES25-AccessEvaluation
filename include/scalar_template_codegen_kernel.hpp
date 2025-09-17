#pragma once

#include <stdlib.h>
#include "kernels_generic.hpp"

template<std::size_t N>
struct Internal_Scalar_Codegen_v1{
    static int64_t load_add(int64_t* data, size_t stride){
        int64_t res = 0;
        if constexpr (N > 0){
            res += Internal_Scalar_Codegen_v1<N-1>::load_add(data, stride);
        }
        return res + data[stride * N];
    }
};

template<std::size_t N>
struct Scalar_Loop_v1{
    static int64_t run(int64_t* data, size_t stride){
        int64_t result = 0;
        for(size_t i = 0; i < stride; i++){
            result += Internal_Scalar_Codegen_v1<N-1>::load_add(&data[i], stride);
        }    
        return result;
    } 
};

int64_t scalar_lanes_codegen_v1(int64_t* data, size_t data_amount, const size_t lane_count){
    size_t stride = data_amount / lane_count;
    int64_t result = 0;
    switch(lane_count){
        case 1:
            return Scalar_Loop_v1<1>::run(data, stride);
        case 2:
            return Scalar_Loop_v1<2>::run(data, stride);
        case 3:
            return Scalar_Loop_v1<3>::run(data, stride);
        case 4:
            return Scalar_Loop_v1<4>::run(data, stride);
        case 5:
            return Scalar_Loop_v1<5>::run(data, stride);
        case 6:
            return Scalar_Loop_v1<6>::run(data, stride);
        case 7:
            return Scalar_Loop_v1<7>::run(data, stride);
        case 8:
            return Scalar_Loop_v1<8>::run(data, stride);
        case 9:
            return Scalar_Loop_v1<9>::run(data, stride);
        case 10:
            return Scalar_Loop_v1<10>::run(data, stride);
        case 11:
            return Scalar_Loop_v1<11>::run(data, stride);
        case 12:
            return Scalar_Loop_v1<12>::run(data, stride);
        case 13:
            return Scalar_Loop_v1<13>::run(data, stride);
        case 14:
            return Scalar_Loop_v1<14>::run(data, stride);
        case 15:
            return Scalar_Loop_v1<15>::run(data, stride);
        case 16:
            return Scalar_Loop_v1<16>::run(data, stride);
        case 17:
            return Scalar_Loop_v1<17>::run(data, stride);
        case 18:
            return Scalar_Loop_v1<18>::run(data, stride);
        case 19:
            return Scalar_Loop_v1<19>::run(data, stride);
        case 20:
            return Scalar_Loop_v1<20>::run(data, stride);
        case 21:
            return Scalar_Loop_v1<21>::run(data, stride);
        case 22:
            return Scalar_Loop_v1<22>::run(data, stride);
        case 23:
            return Scalar_Loop_v1<23>::run(data, stride);
        case 24:
            return Scalar_Loop_v1<24>::run(data, stride);
        case 25:
            return Scalar_Loop_v1<25>::run(data, stride);
        case 26:
            return Scalar_Loop_v1<26>::run(data, stride);
        case 27:
            return Scalar_Loop_v1<27>::run(data, stride);
        case 28:
            return Scalar_Loop_v1<28>::run(data, stride);
        case 29:
            return Scalar_Loop_v1<29>::run(data, stride);
        case 30:
            return Scalar_Loop_v1<30>::run(data, stride);
        case 31:
            return Scalar_Loop_v1<31>::run(data, stride);
        case 32:
            return Scalar_Loop_v1<32>::run(data, stride);
    };
    return result;
}


template<typename T, std::size_t N>
struct Internal_Scalar_Codegen_v2{
    static int64_t load_add(T* data, size_t stride){
        if constexpr (N > 0){
            return Internal_Scalar_Codegen_v2<T, N-1>::load_add(data, stride) + data[stride * N];
        }else{
            return data[stride * N];
        }
    }
};

template<typename T,std::size_t N>
struct Scalar_Loop_v2{
    static int64_t run(T* data, size_t stride){
        int64_t result = 0;
        for(size_t i = 0; i < stride; i++){
            result += Internal_Scalar_Codegen_v2<T ,N-1>::load_add(&data[i], stride);
        }    
        return result;
    } 
};

template<typename T>
int64_t scalar_lanes_codegen_v2(T* data, size_t data_amount, const size_t lane_count){
    size_t stride = data_amount / lane_count;
    int64_t result = 0;
    switch(lane_count){
        case 1: return Scalar_Loop_v2<T, 1>::run(data, stride);
        case 2: return Scalar_Loop_v2<T, 2>::run(data, stride);
        case 3: return Scalar_Loop_v2<T, 3>::run(data, stride);
        case 4: return Scalar_Loop_v2<T, 4>::run(data, stride);
        case 5: return Scalar_Loop_v2<T, 5>::run(data, stride);
        case 6: return Scalar_Loop_v2<T, 6>::run(data, stride);
        case 7: return Scalar_Loop_v2<T, 7>::run(data, stride);
        case 8: return Scalar_Loop_v2<T, 8>::run(data, stride);
        case 9: return Scalar_Loop_v2<T, 9>::run(data, stride);
        case 10: return Scalar_Loop_v2<T, 10>::run(data, stride);
        case 11: return Scalar_Loop_v2<T, 11>::run(data, stride);
        case 12: return Scalar_Loop_v2<T, 12>::run(data, stride);
        case 13: return Scalar_Loop_v2<T, 13>::run(data, stride);
        case 14: return Scalar_Loop_v2<T, 14>::run(data, stride);
        case 15: return Scalar_Loop_v2<T, 15>::run(data, stride);
        case 16: return Scalar_Loop_v2<T, 16>::run(data, stride);
        case 17: return Scalar_Loop_v2<T, 17>::run(data, stride);
        case 18: return Scalar_Loop_v2<T, 18>::run(data, stride);
        case 19: return Scalar_Loop_v2<T, 19>::run(data, stride);
        case 20: return Scalar_Loop_v2<T, 20>::run(data, stride);
        case 21: return Scalar_Loop_v2<T, 21>::run(data, stride);
        case 22: return Scalar_Loop_v2<T, 22>::run(data, stride);
        case 23: return Scalar_Loop_v2<T, 23>::run(data, stride);
        case 24: return Scalar_Loop_v2<T, 24>::run(data, stride);
        case 25: return Scalar_Loop_v2<T, 25>::run(data, stride);
        case 26: return Scalar_Loop_v2<T, 26>::run(data, stride);
        case 27: return Scalar_Loop_v2<T, 27>::run(data, stride);
        case 28: return Scalar_Loop_v2<T, 28>::run(data, stride);
        case 29: return Scalar_Loop_v2<T, 29>::run(data, stride);
        case 30: return Scalar_Loop_v2<T, 30>::run(data, stride);
        case 31: return Scalar_Loop_v2<T, 31>::run(data, stride);
        case 32: return Scalar_Loop_v2<T, 32>::run(data, stride);
        case 33: return Scalar_Loop_v2<T, 33>::run(data, stride);
        case 34: return Scalar_Loop_v2<T, 34>::run(data, stride);
        case 35: return Scalar_Loop_v2<T, 35>::run(data, stride);
        case 36: return Scalar_Loop_v2<T, 36>::run(data, stride);
        case 37: return Scalar_Loop_v2<T, 37>::run(data, stride);
        case 38: return Scalar_Loop_v2<T, 38>::run(data, stride);
        case 39: return Scalar_Loop_v2<T, 39>::run(data, stride);
        case 40: return Scalar_Loop_v2<T, 40>::run(data, stride);
        case 41: return Scalar_Loop_v2<T, 41>::run(data, stride);
        case 42: return Scalar_Loop_v2<T, 42>::run(data, stride);
        case 43: return Scalar_Loop_v2<T, 43>::run(data, stride);
        case 44: return Scalar_Loop_v2<T, 44>::run(data, stride);
        case 45: return Scalar_Loop_v2<T, 45>::run(data, stride);
        case 46: return Scalar_Loop_v2<T, 46>::run(data, stride);
        case 47: return Scalar_Loop_v2<T, 47>::run(data, stride);
        case 48: return Scalar_Loop_v2<T, 48>::run(data, stride);
        case 49: return Scalar_Loop_v2<T, 49>::run(data, stride);
        case 50: return Scalar_Loop_v2<T, 50>::run(data, stride);
        case 51: return Scalar_Loop_v2<T, 51>::run(data, stride);
        case 52: return Scalar_Loop_v2<T, 52>::run(data, stride);
        case 53: return Scalar_Loop_v2<T, 53>::run(data, stride);
        case 54: return Scalar_Loop_v2<T, 54>::run(data, stride);
        case 55: return Scalar_Loop_v2<T, 55>::run(data, stride);
        case 56: return Scalar_Loop_v2<T, 56>::run(data, stride);
        case 57: return Scalar_Loop_v2<T, 57>::run(data, stride);
        case 58: return Scalar_Loop_v2<T, 58>::run(data, stride);
        case 59: return Scalar_Loop_v2<T, 59>::run(data, stride);
        case 60: return Scalar_Loop_v2<T, 60>::run(data, stride);
        case 61: return Scalar_Loop_v2<T, 61>::run(data, stride);
        case 62: return Scalar_Loop_v2<T, 62>::run(data, stride);
        case 63: return Scalar_Loop_v2<T, 63>::run(data, stride);
        case 64: return Scalar_Loop_v2<T, 64>::run(data, stride);
        case 65: return Scalar_Loop_v2<T, 65>::run(data, stride);
        case 66: return Scalar_Loop_v2<T, 66>::run(data, stride);
        case 67: return Scalar_Loop_v2<T, 67>::run(data, stride);
        case 68: return Scalar_Loop_v2<T, 68>::run(data, stride);
        case 69: return Scalar_Loop_v2<T, 69>::run(data, stride);
        case 70: return Scalar_Loop_v2<T, 70>::run(data, stride);
        case 71: return Scalar_Loop_v2<T, 71>::run(data, stride);
        case 72: return Scalar_Loop_v2<T, 72>::run(data, stride);
        case 73: return Scalar_Loop_v2<T, 73>::run(data, stride);
        case 74: return Scalar_Loop_v2<T, 74>::run(data, stride);
        case 75: return Scalar_Loop_v2<T, 75>::run(data, stride);
        case 76: return Scalar_Loop_v2<T, 76>::run(data, stride);
        case 77: return Scalar_Loop_v2<T, 77>::run(data, stride);
        case 78: return Scalar_Loop_v2<T, 78>::run(data, stride);
        case 79: return Scalar_Loop_v2<T, 79>::run(data, stride);
        case 80: return Scalar_Loop_v2<T, 80>::run(data, stride);
        case 81: return Scalar_Loop_v2<T, 81>::run(data, stride);
        case 82: return Scalar_Loop_v2<T, 82>::run(data, stride);
        case 83: return Scalar_Loop_v2<T, 83>::run(data, stride);
        case 84: return Scalar_Loop_v2<T, 84>::run(data, stride);
        case 85: return Scalar_Loop_v2<T, 85>::run(data, stride);
        case 86: return Scalar_Loop_v2<T, 86>::run(data, stride);
        case 87: return Scalar_Loop_v2<T, 87>::run(data, stride);
        case 88: return Scalar_Loop_v2<T, 88>::run(data, stride);
        case 89: return Scalar_Loop_v2<T, 89>::run(data, stride);
        case 90: return Scalar_Loop_v2<T, 90>::run(data, stride);
        case 91: return Scalar_Loop_v2<T, 91>::run(data, stride);
        case 92: return Scalar_Loop_v2<T, 92>::run(data, stride);
        case 93: return Scalar_Loop_v2<T, 93>::run(data, stride);
        case 94: return Scalar_Loop_v2<T, 94>::run(data, stride);
        case 95: return Scalar_Loop_v2<T, 95>::run(data, stride);
        case 96: return Scalar_Loop_v2<T, 96>::run(data, stride);
        case 97: return Scalar_Loop_v2<T, 97>::run(data, stride);
        case 98: return Scalar_Loop_v2<T, 98>::run(data, stride);
        case 99: return Scalar_Loop_v2<T, 99>::run(data, stride);
        case 100: return Scalar_Loop_v2<T, 100>::run(data, stride);
        case 101: return Scalar_Loop_v2<T, 101>::run(data, stride);
        case 102: return Scalar_Loop_v2<T, 102>::run(data, stride);
        case 103: return Scalar_Loop_v2<T, 103>::run(data, stride);
        case 104: return Scalar_Loop_v2<T, 104>::run(data, stride);
        case 105: return Scalar_Loop_v2<T, 105>::run(data, stride);
        case 106: return Scalar_Loop_v2<T, 106>::run(data, stride);
        case 107: return Scalar_Loop_v2<T, 107>::run(data, stride);
        case 108: return Scalar_Loop_v2<T, 108>::run(data, stride);
        case 109: return Scalar_Loop_v2<T, 109>::run(data, stride);
        case 110: return Scalar_Loop_v2<T, 110>::run(data, stride);
        case 111: return Scalar_Loop_v2<T, 111>::run(data, stride);
        case 112: return Scalar_Loop_v2<T, 112>::run(data, stride);
        case 113: return Scalar_Loop_v2<T, 113>::run(data, stride);
        case 114: return Scalar_Loop_v2<T, 114>::run(data, stride);
        case 115: return Scalar_Loop_v2<T, 115>::run(data, stride);
        case 116: return Scalar_Loop_v2<T, 116>::run(data, stride);
        case 117: return Scalar_Loop_v2<T, 117>::run(data, stride);
        case 118: return Scalar_Loop_v2<T, 118>::run(data, stride);
        case 119: return Scalar_Loop_v2<T, 119>::run(data, stride);
        case 120: return Scalar_Loop_v2<T, 120>::run(data, stride);
        case 121: return Scalar_Loop_v2<T, 121>::run(data, stride);
        case 122: return Scalar_Loop_v2<T, 122>::run(data, stride);
        case 123: return Scalar_Loop_v2<T, 123>::run(data, stride);
        case 124: return Scalar_Loop_v2<T, 124>::run(data, stride);
        case 125: return Scalar_Loop_v2<T, 125>::run(data, stride);
        case 126: return Scalar_Loop_v2<T, 126>::run(data, stride);
        case 127: return Scalar_Loop_v2<T, 127>::run(data, stride);
        case 128: return Scalar_Loop_v2<T, 128>::run(data, stride);
        case 129: return Scalar_Loop_v2<T, 129>::run(data, stride);
        case 130: return Scalar_Loop_v2<T, 130>::run(data, stride);
        case 131: return Scalar_Loop_v2<T, 131>::run(data, stride);
        case 132: return Scalar_Loop_v2<T, 132>::run(data, stride);
        case 133: return Scalar_Loop_v2<T, 133>::run(data, stride);
        case 134: return Scalar_Loop_v2<T, 134>::run(data, stride);
        case 135: return Scalar_Loop_v2<T, 135>::run(data, stride);
        case 136: return Scalar_Loop_v2<T, 136>::run(data, stride);
        case 137: return Scalar_Loop_v2<T, 137>::run(data, stride);
        case 138: return Scalar_Loop_v2<T, 138>::run(data, stride);
        case 139: return Scalar_Loop_v2<T, 139>::run(data, stride);
        case 140: return Scalar_Loop_v2<T, 140>::run(data, stride);
        case 141: return Scalar_Loop_v2<T, 141>::run(data, stride);
        case 142: return Scalar_Loop_v2<T, 142>::run(data, stride);
        case 143: return Scalar_Loop_v2<T, 143>::run(data, stride);
        case 144: return Scalar_Loop_v2<T, 144>::run(data, stride);
        case 145: return Scalar_Loop_v2<T, 145>::run(data, stride);
        case 146: return Scalar_Loop_v2<T, 146>::run(data, stride);
        case 147: return Scalar_Loop_v2<T, 147>::run(data, stride);
        case 148: return Scalar_Loop_v2<T, 148>::run(data, stride);
        case 149: return Scalar_Loop_v2<T, 149>::run(data, stride);
        case 150: return Scalar_Loop_v2<T, 150>::run(data, stride);
        case 151: return Scalar_Loop_v2<T, 151>::run(data, stride);
        case 152: return Scalar_Loop_v2<T, 152>::run(data, stride);
        case 153: return Scalar_Loop_v2<T, 153>::run(data, stride);
        case 154: return Scalar_Loop_v2<T, 154>::run(data, stride);
        case 155: return Scalar_Loop_v2<T, 155>::run(data, stride);
        case 156: return Scalar_Loop_v2<T, 156>::run(data, stride);
        case 157: return Scalar_Loop_v2<T, 157>::run(data, stride);
        case 158: return Scalar_Loop_v2<T, 158>::run(data, stride);
        case 159: return Scalar_Loop_v2<T, 159>::run(data, stride);
        case 160: return Scalar_Loop_v2<T, 160>::run(data, stride);
        case 161: return Scalar_Loop_v2<T, 161>::run(data, stride);
        case 162: return Scalar_Loop_v2<T, 162>::run(data, stride);
        case 163: return Scalar_Loop_v2<T, 163>::run(data, stride);
        case 164: return Scalar_Loop_v2<T, 164>::run(data, stride);
        case 165: return Scalar_Loop_v2<T, 165>::run(data, stride);
        case 166: return Scalar_Loop_v2<T, 166>::run(data, stride);
        case 167: return Scalar_Loop_v2<T, 167>::run(data, stride);
        case 168: return Scalar_Loop_v2<T, 168>::run(data, stride);
        case 169: return Scalar_Loop_v2<T, 169>::run(data, stride);
        case 170: return Scalar_Loop_v2<T, 170>::run(data, stride);
        case 171: return Scalar_Loop_v2<T, 171>::run(data, stride);
        case 172: return Scalar_Loop_v2<T, 172>::run(data, stride);
        case 173: return Scalar_Loop_v2<T, 173>::run(data, stride);
        case 174: return Scalar_Loop_v2<T, 174>::run(data, stride);
        case 175: return Scalar_Loop_v2<T, 175>::run(data, stride);
        case 176: return Scalar_Loop_v2<T, 176>::run(data, stride);
        case 177: return Scalar_Loop_v2<T, 177>::run(data, stride);
        case 178: return Scalar_Loop_v2<T, 178>::run(data, stride);
        case 179: return Scalar_Loop_v2<T, 179>::run(data, stride);
        case 180: return Scalar_Loop_v2<T, 180>::run(data, stride);
        case 181: return Scalar_Loop_v2<T, 181>::run(data, stride);
        case 182: return Scalar_Loop_v2<T, 182>::run(data, stride);
        case 183: return Scalar_Loop_v2<T, 183>::run(data, stride);
        case 184: return Scalar_Loop_v2<T, 184>::run(data, stride);
        case 185: return Scalar_Loop_v2<T, 185>::run(data, stride);
        case 186: return Scalar_Loop_v2<T, 186>::run(data, stride);
        case 187: return Scalar_Loop_v2<T, 187>::run(data, stride);
        case 188: return Scalar_Loop_v2<T, 188>::run(data, stride);
        case 189: return Scalar_Loop_v2<T, 189>::run(data, stride);
        case 190: return Scalar_Loop_v2<T, 190>::run(data, stride);
        case 191: return Scalar_Loop_v2<T, 191>::run(data, stride);
        case 192: return Scalar_Loop_v2<T, 192>::run(data, stride);
        case 193: return Scalar_Loop_v2<T, 193>::run(data, stride);
        case 194: return Scalar_Loop_v2<T, 194>::run(data, stride);
        case 195: return Scalar_Loop_v2<T, 195>::run(data, stride);
        case 196: return Scalar_Loop_v2<T, 196>::run(data, stride);
        case 197: return Scalar_Loop_v2<T, 197>::run(data, stride);
        case 198: return Scalar_Loop_v2<T, 198>::run(data, stride);
        case 199: return Scalar_Loop_v2<T, 199>::run(data, stride);
        case 200: return Scalar_Loop_v2<T, 200>::run(data, stride);
        case 201: return Scalar_Loop_v2<T, 201>::run(data, stride);
        case 202: return Scalar_Loop_v2<T, 202>::run(data, stride);
        case 203: return Scalar_Loop_v2<T, 203>::run(data, stride);
        case 204: return Scalar_Loop_v2<T, 204>::run(data, stride);
        case 205: return Scalar_Loop_v2<T, 205>::run(data, stride);
        case 206: return Scalar_Loop_v2<T, 206>::run(data, stride);
        case 207: return Scalar_Loop_v2<T, 207>::run(data, stride);
        case 208: return Scalar_Loop_v2<T, 208>::run(data, stride);
        case 209: return Scalar_Loop_v2<T, 209>::run(data, stride);
        case 210: return Scalar_Loop_v2<T, 210>::run(data, stride);
        case 211: return Scalar_Loop_v2<T, 211>::run(data, stride);
        case 212: return Scalar_Loop_v2<T, 212>::run(data, stride);
        case 213: return Scalar_Loop_v2<T, 213>::run(data, stride);
        case 214: return Scalar_Loop_v2<T, 214>::run(data, stride);
        case 215: return Scalar_Loop_v2<T, 215>::run(data, stride);
        case 216: return Scalar_Loop_v2<T, 216>::run(data, stride);
        case 217: return Scalar_Loop_v2<T, 217>::run(data, stride);
        case 218: return Scalar_Loop_v2<T, 218>::run(data, stride);
        case 219: return Scalar_Loop_v2<T, 219>::run(data, stride);
        case 220: return Scalar_Loop_v2<T, 220>::run(data, stride);
        case 221: return Scalar_Loop_v2<T, 221>::run(data, stride);
        case 222: return Scalar_Loop_v2<T, 222>::run(data, stride);
        case 223: return Scalar_Loop_v2<T, 223>::run(data, stride);
        case 224: return Scalar_Loop_v2<T, 224>::run(data, stride);
        case 225: return Scalar_Loop_v2<T, 225>::run(data, stride);
        case 226: return Scalar_Loop_v2<T, 226>::run(data, stride);
        case 227: return Scalar_Loop_v2<T, 227>::run(data, stride);
        case 228: return Scalar_Loop_v2<T, 228>::run(data, stride);
        case 229: return Scalar_Loop_v2<T, 229>::run(data, stride);
        case 230: return Scalar_Loop_v2<T, 230>::run(data, stride);
        case 231: return Scalar_Loop_v2<T, 231>::run(data, stride);
        case 232: return Scalar_Loop_v2<T, 232>::run(data, stride);
        case 233: return Scalar_Loop_v2<T, 233>::run(data, stride);
        case 234: return Scalar_Loop_v2<T, 234>::run(data, stride);
        case 235: return Scalar_Loop_v2<T, 235>::run(data, stride);
        case 236: return Scalar_Loop_v2<T, 236>::run(data, stride);
        case 237: return Scalar_Loop_v2<T, 237>::run(data, stride);
        case 238: return Scalar_Loop_v2<T, 238>::run(data, stride);
        case 239: return Scalar_Loop_v2<T, 239>::run(data, stride);
        case 240: return Scalar_Loop_v2<T, 240>::run(data, stride);
        case 241: return Scalar_Loop_v2<T, 241>::run(data, stride);
        case 242: return Scalar_Loop_v2<T, 242>::run(data, stride);
        case 243: return Scalar_Loop_v2<T, 243>::run(data, stride);
        case 244: return Scalar_Loop_v2<T, 244>::run(data, stride);
        case 245: return Scalar_Loop_v2<T, 245>::run(data, stride);
        case 246: return Scalar_Loop_v2<T, 246>::run(data, stride);
        case 247: return Scalar_Loop_v2<T, 247>::run(data, stride);
        case 248: return Scalar_Loop_v2<T, 248>::run(data, stride);
        case 249: return Scalar_Loop_v2<T, 249>::run(data, stride);
        case 250: return Scalar_Loop_v2<T, 250>::run(data, stride);
        case 251: return Scalar_Loop_v2<T, 251>::run(data, stride);
        case 252: return Scalar_Loop_v2<T, 252>::run(data, stride);
        case 253: return Scalar_Loop_v2<T, 253>::run(data, stride);
        case 254: return Scalar_Loop_v2<T, 254>::run(data, stride);
        case 255: return Scalar_Loop_v2<T, 255>::run(data, stride);
        case 256: return Scalar_Loop_v2<T, 256>::run(data, stride);
        case 257: return Scalar_Loop_v2<T, 257>::run(data, stride);
        case 258: return Scalar_Loop_v2<T, 258>::run(data, stride);
        case 259: return Scalar_Loop_v2<T, 259>::run(data, stride);
        case 260: return Scalar_Loop_v2<T, 260>::run(data, stride);
        case 261: return Scalar_Loop_v2<T, 261>::run(data, stride);
        case 262: return Scalar_Loop_v2<T, 262>::run(data, stride);
        case 263: return Scalar_Loop_v2<T, 263>::run(data, stride);
        case 264: return Scalar_Loop_v2<T, 264>::run(data, stride);
        case 265: return Scalar_Loop_v2<T, 265>::run(data, stride);
        case 266: return Scalar_Loop_v2<T, 266>::run(data, stride);
        case 267: return Scalar_Loop_v2<T, 267>::run(data, stride);
        case 268: return Scalar_Loop_v2<T, 268>::run(data, stride);
        case 269: return Scalar_Loop_v2<T, 269>::run(data, stride);
        case 270: return Scalar_Loop_v2<T, 270>::run(data, stride);
        case 271: return Scalar_Loop_v2<T, 271>::run(data, stride);
        case 272: return Scalar_Loop_v2<T, 272>::run(data, stride);
        case 273: return Scalar_Loop_v2<T, 273>::run(data, stride);
        case 274: return Scalar_Loop_v2<T, 274>::run(data, stride);
        case 275: return Scalar_Loop_v2<T, 275>::run(data, stride);
        case 276: return Scalar_Loop_v2<T, 276>::run(data, stride);
        case 277: return Scalar_Loop_v2<T, 277>::run(data, stride);
        case 278: return Scalar_Loop_v2<T, 278>::run(data, stride);
        case 279: return Scalar_Loop_v2<T, 279>::run(data, stride);
        case 280: return Scalar_Loop_v2<T, 280>::run(data, stride);
        case 281: return Scalar_Loop_v2<T, 281>::run(data, stride);
        case 282: return Scalar_Loop_v2<T, 282>::run(data, stride);
        case 283: return Scalar_Loop_v2<T, 283>::run(data, stride);
        case 284: return Scalar_Loop_v2<T, 284>::run(data, stride);
        case 285: return Scalar_Loop_v2<T, 285>::run(data, stride);
        case 286: return Scalar_Loop_v2<T, 286>::run(data, stride);
        case 287: return Scalar_Loop_v2<T, 287>::run(data, stride);
        case 288: return Scalar_Loop_v2<T, 288>::run(data, stride);
        case 289: return Scalar_Loop_v2<T, 289>::run(data, stride);
        case 290: return Scalar_Loop_v2<T, 290>::run(data, stride);
        case 291: return Scalar_Loop_v2<T, 291>::run(data, stride);
        case 292: return Scalar_Loop_v2<T, 292>::run(data, stride);
        case 293: return Scalar_Loop_v2<T, 293>::run(data, stride);
        case 294: return Scalar_Loop_v2<T, 294>::run(data, stride);
        case 295: return Scalar_Loop_v2<T, 295>::run(data, stride);
        case 296: return Scalar_Loop_v2<T, 296>::run(data, stride);
        case 297: return Scalar_Loop_v2<T, 297>::run(data, stride);
        case 298: return Scalar_Loop_v2<T, 298>::run(data, stride);
        case 299: return Scalar_Loop_v2<T, 299>::run(data, stride);
        case 300: return Scalar_Loop_v2<T, 300>::run(data, stride);
        case 301: return Scalar_Loop_v2<T, 301>::run(data, stride);
        case 302: return Scalar_Loop_v2<T, 302>::run(data, stride);
        case 303: return Scalar_Loop_v2<T, 303>::run(data, stride);
        case 304: return Scalar_Loop_v2<T, 304>::run(data, stride);
        case 305: return Scalar_Loop_v2<T, 305>::run(data, stride);
        case 306: return Scalar_Loop_v2<T, 306>::run(data, stride);
        case 307: return Scalar_Loop_v2<T, 307>::run(data, stride);
        case 308: return Scalar_Loop_v2<T, 308>::run(data, stride);
        case 309: return Scalar_Loop_v2<T, 309>::run(data, stride);
        case 310: return Scalar_Loop_v2<T, 310>::run(data, stride);
        case 311: return Scalar_Loop_v2<T, 311>::run(data, stride);
        case 312: return Scalar_Loop_v2<T, 312>::run(data, stride);
        case 313: return Scalar_Loop_v2<T, 313>::run(data, stride);
        case 314: return Scalar_Loop_v2<T, 314>::run(data, stride);
        case 315: return Scalar_Loop_v2<T, 315>::run(data, stride);
        case 316: return Scalar_Loop_v2<T, 316>::run(data, stride);
        case 317: return Scalar_Loop_v2<T, 317>::run(data, stride);
        case 318: return Scalar_Loop_v2<T, 318>::run(data, stride);
        case 319: return Scalar_Loop_v2<T, 319>::run(data, stride);
        case 320: return Scalar_Loop_v2<T, 320>::run(data, stride);
        case 321: return Scalar_Loop_v2<T, 321>::run(data, stride);
        case 322: return Scalar_Loop_v2<T, 322>::run(data, stride);
        case 323: return Scalar_Loop_v2<T, 323>::run(data, stride);
        case 324: return Scalar_Loop_v2<T, 324>::run(data, stride);
        case 325: return Scalar_Loop_v2<T, 325>::run(data, stride);
        case 326: return Scalar_Loop_v2<T, 326>::run(data, stride);
        case 327: return Scalar_Loop_v2<T, 327>::run(data, stride);
        case 328: return Scalar_Loop_v2<T, 328>::run(data, stride);
        case 329: return Scalar_Loop_v2<T, 329>::run(data, stride);
        case 330: return Scalar_Loop_v2<T, 330>::run(data, stride);
        case 331: return Scalar_Loop_v2<T, 331>::run(data, stride);
        case 332: return Scalar_Loop_v2<T, 332>::run(data, stride);
        case 333: return Scalar_Loop_v2<T, 333>::run(data, stride);
        case 334: return Scalar_Loop_v2<T, 334>::run(data, stride);
        case 335: return Scalar_Loop_v2<T, 335>::run(data, stride);
        case 336: return Scalar_Loop_v2<T, 336>::run(data, stride);
        case 337: return Scalar_Loop_v2<T, 337>::run(data, stride);
        case 338: return Scalar_Loop_v2<T, 338>::run(data, stride);
        case 339: return Scalar_Loop_v2<T, 339>::run(data, stride);
        case 340: return Scalar_Loop_v2<T, 340>::run(data, stride);
        case 341: return Scalar_Loop_v2<T, 341>::run(data, stride);
        case 342: return Scalar_Loop_v2<T, 342>::run(data, stride);
        case 343: return Scalar_Loop_v2<T, 343>::run(data, stride);
        case 344: return Scalar_Loop_v2<T, 344>::run(data, stride);
        case 345: return Scalar_Loop_v2<T, 345>::run(data, stride);
        case 346: return Scalar_Loop_v2<T, 346>::run(data, stride);
        case 347: return Scalar_Loop_v2<T, 347>::run(data, stride);
        case 348: return Scalar_Loop_v2<T, 348>::run(data, stride);
        case 349: return Scalar_Loop_v2<T, 349>::run(data, stride);
        case 350: return Scalar_Loop_v2<T, 350>::run(data, stride);
        case 351: return Scalar_Loop_v2<T, 351>::run(data, stride);
        case 352: return Scalar_Loop_v2<T, 352>::run(data, stride);
        case 353: return Scalar_Loop_v2<T, 353>::run(data, stride);
        case 354: return Scalar_Loop_v2<T, 354>::run(data, stride);
        case 355: return Scalar_Loop_v2<T, 355>::run(data, stride);
        case 356: return Scalar_Loop_v2<T, 356>::run(data, stride);
        case 357: return Scalar_Loop_v2<T, 357>::run(data, stride);
        case 358: return Scalar_Loop_v2<T, 358>::run(data, stride);
        case 359: return Scalar_Loop_v2<T, 359>::run(data, stride);
        case 360: return Scalar_Loop_v2<T, 360>::run(data, stride);
        case 361: return Scalar_Loop_v2<T, 361>::run(data, stride);
        case 362: return Scalar_Loop_v2<T, 362>::run(data, stride);
        case 363: return Scalar_Loop_v2<T, 363>::run(data, stride);
        case 364: return Scalar_Loop_v2<T, 364>::run(data, stride);
        case 365: return Scalar_Loop_v2<T, 365>::run(data, stride);
        case 366: return Scalar_Loop_v2<T, 366>::run(data, stride);
        case 367: return Scalar_Loop_v2<T, 367>::run(data, stride);
        case 368: return Scalar_Loop_v2<T, 368>::run(data, stride);
        case 369: return Scalar_Loop_v2<T, 369>::run(data, stride);
        case 370: return Scalar_Loop_v2<T, 370>::run(data, stride);
        case 371: return Scalar_Loop_v2<T, 371>::run(data, stride);
        case 372: return Scalar_Loop_v2<T, 372>::run(data, stride);
        case 373: return Scalar_Loop_v2<T, 373>::run(data, stride);
        case 374: return Scalar_Loop_v2<T, 374>::run(data, stride);
        case 375: return Scalar_Loop_v2<T, 375>::run(data, stride);
        case 376: return Scalar_Loop_v2<T, 376>::run(data, stride);
        case 377: return Scalar_Loop_v2<T, 377>::run(data, stride);
        case 378: return Scalar_Loop_v2<T, 378>::run(data, stride);
        case 379: return Scalar_Loop_v2<T, 379>::run(data, stride);
        case 380: return Scalar_Loop_v2<T, 380>::run(data, stride);
        case 381: return Scalar_Loop_v2<T, 381>::run(data, stride);
        case 382: return Scalar_Loop_v2<T, 382>::run(data, stride);
        case 383: return Scalar_Loop_v2<T, 383>::run(data, stride);
        case 384: return Scalar_Loop_v2<T, 384>::run(data, stride);
        case 385: return Scalar_Loop_v2<T, 385>::run(data, stride);
        case 386: return Scalar_Loop_v2<T, 386>::run(data, stride);
        case 387: return Scalar_Loop_v2<T, 387>::run(data, stride);
        case 388: return Scalar_Loop_v2<T, 388>::run(data, stride);
        case 389: return Scalar_Loop_v2<T, 389>::run(data, stride);
        case 390: return Scalar_Loop_v2<T, 390>::run(data, stride);
        case 391: return Scalar_Loop_v2<T, 391>::run(data, stride);
        case 392: return Scalar_Loop_v2<T, 392>::run(data, stride);
        case 393: return Scalar_Loop_v2<T, 393>::run(data, stride);
        case 394: return Scalar_Loop_v2<T, 394>::run(data, stride);
        case 395: return Scalar_Loop_v2<T, 395>::run(data, stride);
        case 396: return Scalar_Loop_v2<T, 396>::run(data, stride);
        case 397: return Scalar_Loop_v2<T, 397>::run(data, stride);
        case 398: return Scalar_Loop_v2<T, 398>::run(data, stride);
        case 399: return Scalar_Loop_v2<T, 399>::run(data, stride);
        case 400: return Scalar_Loop_v2<T, 400>::run(data, stride);
        case 401: return Scalar_Loop_v2<T, 401>::run(data, stride);
        case 402: return Scalar_Loop_v2<T, 402>::run(data, stride);
        case 403: return Scalar_Loop_v2<T, 403>::run(data, stride);
        case 404: return Scalar_Loop_v2<T, 404>::run(data, stride);
        case 405: return Scalar_Loop_v2<T, 405>::run(data, stride);
        case 406: return Scalar_Loop_v2<T, 406>::run(data, stride);
        case 407: return Scalar_Loop_v2<T, 407>::run(data, stride);
        case 408: return Scalar_Loop_v2<T, 408>::run(data, stride);
        case 409: return Scalar_Loop_v2<T, 409>::run(data, stride);
        case 410: return Scalar_Loop_v2<T, 410>::run(data, stride);
        case 411: return Scalar_Loop_v2<T, 411>::run(data, stride);
        case 412: return Scalar_Loop_v2<T, 412>::run(data, stride);
        case 413: return Scalar_Loop_v2<T, 413>::run(data, stride);
        case 414: return Scalar_Loop_v2<T, 414>::run(data, stride);
        case 415: return Scalar_Loop_v2<T, 415>::run(data, stride);
        case 416: return Scalar_Loop_v2<T, 416>::run(data, stride);
        case 417: return Scalar_Loop_v2<T, 417>::run(data, stride);
        case 418: return Scalar_Loop_v2<T, 418>::run(data, stride);
        case 419: return Scalar_Loop_v2<T, 419>::run(data, stride);
        case 420: return Scalar_Loop_v2<T, 420>::run(data, stride);
        case 421: return Scalar_Loop_v2<T, 421>::run(data, stride);
        case 422: return Scalar_Loop_v2<T, 422>::run(data, stride);
        case 423: return Scalar_Loop_v2<T, 423>::run(data, stride);
        case 424: return Scalar_Loop_v2<T, 424>::run(data, stride);
        case 425: return Scalar_Loop_v2<T, 425>::run(data, stride);
        case 426: return Scalar_Loop_v2<T, 426>::run(data, stride);
        case 427: return Scalar_Loop_v2<T, 427>::run(data, stride);
        case 428: return Scalar_Loop_v2<T, 428>::run(data, stride);
        case 429: return Scalar_Loop_v2<T, 429>::run(data, stride);
        case 430: return Scalar_Loop_v2<T, 430>::run(data, stride);
        case 431: return Scalar_Loop_v2<T, 431>::run(data, stride);
        case 432: return Scalar_Loop_v2<T, 432>::run(data, stride);
        case 433: return Scalar_Loop_v2<T, 433>::run(data, stride);
        case 434: return Scalar_Loop_v2<T, 434>::run(data, stride);
        case 435: return Scalar_Loop_v2<T, 435>::run(data, stride);
        case 436: return Scalar_Loop_v2<T, 436>::run(data, stride);
        case 437: return Scalar_Loop_v2<T, 437>::run(data, stride);
        case 438: return Scalar_Loop_v2<T, 438>::run(data, stride);
        case 439: return Scalar_Loop_v2<T, 439>::run(data, stride);
        case 440: return Scalar_Loop_v2<T, 440>::run(data, stride);
        case 441: return Scalar_Loop_v2<T, 441>::run(data, stride);
        case 442: return Scalar_Loop_v2<T, 442>::run(data, stride);
        case 443: return Scalar_Loop_v2<T, 443>::run(data, stride);
        case 444: return Scalar_Loop_v2<T, 444>::run(data, stride);
        case 445: return Scalar_Loop_v2<T, 445>::run(data, stride);
        case 446: return Scalar_Loop_v2<T, 446>::run(data, stride);
        case 447: return Scalar_Loop_v2<T, 447>::run(data, stride);
        case 448: return Scalar_Loop_v2<T, 448>::run(data, stride);
        case 449: return Scalar_Loop_v2<T, 449>::run(data, stride);
        case 450: return Scalar_Loop_v2<T, 450>::run(data, stride);
        case 451: return Scalar_Loop_v2<T, 451>::run(data, stride);
        case 452: return Scalar_Loop_v2<T, 452>::run(data, stride);
        case 453: return Scalar_Loop_v2<T, 453>::run(data, stride);
        case 454: return Scalar_Loop_v2<T, 454>::run(data, stride);
        case 455: return Scalar_Loop_v2<T, 455>::run(data, stride);
        case 456: return Scalar_Loop_v2<T, 456>::run(data, stride);
        case 457: return Scalar_Loop_v2<T, 457>::run(data, stride);
        case 458: return Scalar_Loop_v2<T, 458>::run(data, stride);
        case 459: return Scalar_Loop_v2<T, 459>::run(data, stride);
        case 460: return Scalar_Loop_v2<T, 460>::run(data, stride);
        case 461: return Scalar_Loop_v2<T, 461>::run(data, stride);
        case 462: return Scalar_Loop_v2<T, 462>::run(data, stride);
        case 463: return Scalar_Loop_v2<T, 463>::run(data, stride);
        case 464: return Scalar_Loop_v2<T, 464>::run(data, stride);
        case 465: return Scalar_Loop_v2<T, 465>::run(data, stride);
        case 466: return Scalar_Loop_v2<T, 466>::run(data, stride);
        case 467: return Scalar_Loop_v2<T, 467>::run(data, stride);
        case 468: return Scalar_Loop_v2<T, 468>::run(data, stride);
        case 469: return Scalar_Loop_v2<T, 469>::run(data, stride);
        case 470: return Scalar_Loop_v2<T, 470>::run(data, stride);
        case 471: return Scalar_Loop_v2<T, 471>::run(data, stride);
        case 472: return Scalar_Loop_v2<T, 472>::run(data, stride);
        case 473: return Scalar_Loop_v2<T, 473>::run(data, stride);
        case 474: return Scalar_Loop_v2<T, 474>::run(data, stride);
        case 475: return Scalar_Loop_v2<T, 475>::run(data, stride);
        case 476: return Scalar_Loop_v2<T, 476>::run(data, stride);
        case 477: return Scalar_Loop_v2<T, 477>::run(data, stride);
        case 478: return Scalar_Loop_v2<T, 478>::run(data, stride);
        case 479: return Scalar_Loop_v2<T, 479>::run(data, stride);
        case 480: return Scalar_Loop_v2<T, 480>::run(data, stride);
        case 481: return Scalar_Loop_v2<T, 481>::run(data, stride);
        case 482: return Scalar_Loop_v2<T, 482>::run(data, stride);
        case 483: return Scalar_Loop_v2<T, 483>::run(data, stride);
        case 484: return Scalar_Loop_v2<T, 484>::run(data, stride);
        case 485: return Scalar_Loop_v2<T, 485>::run(data, stride);
        case 486: return Scalar_Loop_v2<T, 486>::run(data, stride);
        case 487: return Scalar_Loop_v2<T, 487>::run(data, stride);
        case 488: return Scalar_Loop_v2<T, 488>::run(data, stride);
        case 489: return Scalar_Loop_v2<T, 489>::run(data, stride);
        case 490: return Scalar_Loop_v2<T, 490>::run(data, stride);
        case 491: return Scalar_Loop_v2<T, 491>::run(data, stride);
        case 492: return Scalar_Loop_v2<T, 492>::run(data, stride);
        case 493: return Scalar_Loop_v2<T, 493>::run(data, stride);
        case 494: return Scalar_Loop_v2<T, 494>::run(data, stride);
        case 495: return Scalar_Loop_v2<T, 495>::run(data, stride);
        case 496: return Scalar_Loop_v2<T, 496>::run(data, stride);
        case 497: return Scalar_Loop_v2<T, 497>::run(data, stride);
        case 498: return Scalar_Loop_v2<T, 498>::run(data, stride);
        case 499: return Scalar_Loop_v2<T, 499>::run(data, stride);
        case 500: return Scalar_Loop_v2<T, 500>::run(data, stride);
        case 501: return Scalar_Loop_v2<T, 501>::run(data, stride);
        case 502: return Scalar_Loop_v2<T, 502>::run(data, stride);
        case 503: return Scalar_Loop_v2<T, 503>::run(data, stride);
        case 504: return Scalar_Loop_v2<T, 504>::run(data, stride);
        case 505: return Scalar_Loop_v2<T, 505>::run(data, stride);
        case 506: return Scalar_Loop_v2<T, 506>::run(data, stride);
        case 507: return Scalar_Loop_v2<T, 507>::run(data, stride);
        case 508: return Scalar_Loop_v2<T, 508>::run(data, stride);
        case 509: return Scalar_Loop_v2<T, 509>::run(data, stride);
        case 510: return Scalar_Loop_v2<T, 510>::run(data, stride);
        case 511: return Scalar_Loop_v2<T, 511>::run(data, stride);
        case 512: return Scalar_Loop_v2<T, 512>::run(data, stride);
        case 513: return Scalar_Loop_v2<T, 513>::run(data, stride);
        default: return scalar(data, data_amount);
    };
    return result;
}


template<typename T>
int64_t scalar_merged(T* data, size_t data_amount, const size_t stride_size){
    const size_t partitions = data_amount / stride_size;
    if(partitions <= 512){
        int64_t res = scalar_lanes_codegen_v2<T>(data, data_amount, partitions);
        for(size_t i = partitions * stride_size; i < data_amount; i++){
            res += data[i];
        }
        return res;
    }
    return scalar_strided(data, data_amount, stride_size);
}