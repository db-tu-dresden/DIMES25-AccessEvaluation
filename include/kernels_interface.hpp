#include <iostream>
#include <stdlib.h>

template<typename T>
struct Accessor{
    virtual T operator [] (size_t id) = 0;
    virtual void reset_stride(size_t stride) = 0;
};

size_t ggT(size_t a, size_t b){
    if(a % b == 1){
        return 1;
    }else if(a % b == 0){
        return b;
    }
    return ggT(b, a%b);
}


template<typename T>
struct Accessor_Mod : Accessor<T>{
    Accessor_Mod(T *data, size_t size, size_t stride = 1):data_m(data),size_m(size),stride_m(stride){}

    void reset_stride(size_t stride) override {
        // std::cout << "WHAT?!\t" << stride << "\t"; 
        while(ggT(stride, size_m) != 1){
            stride--;
            // std::cout << stride << "\t";
        }
        stride_m = stride;
    }

    T operator[](size_t id) override {
        return data_m[(id * stride_m) % size_m];
    }

private:
    T *data_m;
    size_t size_m;
    size_t stride_m;
};


template<typename T>
struct Accessor_Bit : Accessor<T>{
    private:
        void mask_calc(){
            // mask lowest values
            if(exponent_m > 1){
                mask_low_m = (1 << exponent_m) - 1;
            }else{
                mask_low_m = 0;
            }
            
            // where to push lowest values
            push_m = (64 - __builtin_clzll(size_m) - 1);
            if(exponent_m > push_m){
                push_m = exponent_m;
            }
            push_m -= exponent_m;
            
        }

    public:

    Accessor_Bit(T *data, size_t size, size_t exponent = 0):data_m(data),size_m(size){
        
        exponent_m = exponent;
        mask_calc();

        // to which value we want to bit shift
        highest_stride_m = 1 << (64 - __builtin_clzll(size));
    }

    void reset_stride(size_t exponent) override {
        exponent_m = exponent;
        mask_calc();
    }

    T operator[](size_t id) override {
        size_t n_id = id;
        
        if(id < highest_stride_m){
            n_id = ((id & mask_low_m) << push_m) | (id >> exponent_m);
        }
        return data_m[n_id];
    }

private:
    T *data_m;
    size_t size_m;
    size_t exponent_m;
    size_t highest_stride_m;
    size_t mask_low_m;
    size_t push_m;
};

template<typename T>
struct Accessor_Id : Accessor<T>{
    public:

    Accessor_Id(T *data, size_t size, size_t exponent = 0):data_m(data),size_m(size){}

    void reset_stride(size_t stride) override {}

    T operator[](size_t id) override {
        size_t n_id = id;
        return data_m[n_id];
    }

private:
    T *data_m;
    size_t size_m;
};

template<typename T>
int64_t interface_scalar(Accessor<T> *acc, size_t data_amount){
    int64_t result = 0;
    for(size_t i = 0; i < data_amount; i++){
        result += (*acc)[i];
    }
    return result;
}