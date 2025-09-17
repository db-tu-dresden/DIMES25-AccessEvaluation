#pragma once

#include <cstdint>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <page-info.h>

#define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)
#define MAP_HUGE_1GB    (30 << MAP_HUGE_SHIFT)

enum PageType {
    K4_Normal,
    M2_HugePages,
    G1_HugePages,
    Transparent_HugePages
};

enum FillType{
    ONE,
    INCREASE
};

template<typename T> 
void print_page_info(T *array, size_t length) {
    constexpr int KPF_THP = 22;
    page_info_array pinfo = get_info_for_range(array, array + length);
    flag_count thp_count = get_flag_count(pinfo, KPF_THP);
    if (thp_count.pages_available) {
        std::cout << "\033[32m";
        std::cout << "Source pages allocated with transparent hugepages: " 
            << 100.0 * thp_count.pages_set / thp_count.pages_total 
            << "% (" << thp_count.pages_total 
            << " pages, " << 100.0 * thp_count.pages_available / thp_count.pages_total
            << "% flagged)\n" << std::endl;
        std::cout << "\033[0m" << std::flush;
    } else {
        std::cout << "\033[31mCouldn't determine hugepage info \033[31;1m(you are probably not running as root)\033[0m" << std::endl;
    }
}
template<typename T>
class AlignedAllocator {
public:
    using value_type = T;

    explicit AlignedAllocator(const PageType ptype) : _ptype(ptype) {}

    AlignedAllocator(const AlignedAllocator&) = default;
    AlignedAllocator(AlignedAllocator&&) = default;

    AlignedAllocator& operator=(const AlignedAllocator&) noexcept = default;
    AlignedAllocator& operator=(AlignedAllocator&&) noexcept = default;

    T* allocate(const std::size_t count) {
        const auto size = sizeof(T) * count;

        if (_ptype == M2_HugePages) {
            return static_cast<T*> (::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB, -1, 0));
        }
        else if(_ptype == G1_HugePages) {
            return static_cast<T*> (::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_1GB, -1, 0));
        }
        T* data = static_cast<T*>(std::aligned_alloc(4096U, size));
        if(_ptype == Transparent_HugePages){
            madvise(data, size, MADV_HUGEPAGE);
        }

        return data;
    }

    void fill(T* data, const std::size_t count, FillType ftype = INCREASE, bool inform = true){
        if(ftype == ONE){
            #pragma omp parallel for
            for(size_t i = 0; i < count; i++){
                data[i] = 1;
            }
        }else if(ftype == INCREASE){
            #pragma omp parallel for
            for(size_t i = 0; i < count; i++){
                data[i] = i;
            }
        }

        if(inform){
            print_page_info<T>(data, count);
        }
    }

    void deallocate(T* ptr, const std::size_t count) {
        if (_ptype == M2_HugePages || _ptype == G1_HugePages) {
            ::munmap(ptr, sizeof(T) * count);
        }
        else {
            std::free(ptr);
        }
    }

private:
    PageType _ptype = K4_Normal;
};