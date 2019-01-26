#pragma once
#ifndef NV_CORE_RADIXSORT_H
#define NV_CORE_RADIXSORT_H

// Based on Pierre Terdiman's and Michael Herf's source code.
// http://www.codercorner.com/RadixSortRevisited.htm
// http://www.stereopsis.com/radix.html

#include <stdint.h>
#include <vector>

namespace nv
{

    class RadixSort
    {
        RadixSort(const RadixSort&) = delete;
    public:
        // Constructor/Destructor
        RadixSort();
        RadixSort(uint32_t reserve_count);
        ~RadixSort();

        // Invalidate ranks.
        RadixSort & reset() { m_validRanks = false; return *this; }

        // Sorting methods.
        RadixSort & sort(const uint32_t * input, uint32_t count);
        RadixSort & sort(const uint64_t * input, uint32_t count);
        RadixSort & sort(const float * input, uint32_t count);

        // Helpers.
        RadixSort & sort(const std::vector<uint32_t> & input);
        RadixSort & sort(const std::vector<uint64_t> & input);
        RadixSort & sort(const std::vector<float> & input);

        // Access to results. m_ranks is a list of indices in sorted order, i.e. in the order you may further process your data
        inline const uint32_t * ranks() const { /*nvDebugCheck(m_validRanks); */return m_ranks; }
        inline uint32_t * ranks() { /*nvDebugCheck(m_validRanks); */return m_ranks; }
        inline uint32_t rank(uint32_t i) const { /*nvDebugCheck(m_validRanks); */return m_ranks[i]; }

        // query whether the sort has been performed
        inline bool valid() const { return m_validRanks; }

    private:
        uint32_t m_size;
        uint32_t * m_ranks;
        uint32_t * m_ranks2;
        bool m_validRanks;

        // Internal methods
        template <typename T> void insertionSort(const T * input, uint32_t count);
        template <typename T> void radixSort(const T * input, uint32_t count);

        void checkResize(uint32_t nb);
        void resize(uint32_t nb);
    };

    inline RadixSort & RadixSort::sort(const std::vector<uint32_t> & input) {
        return sort(input.data(), input.size());
    }

    inline RadixSort & RadixSort::sort(const std::vector<uint64_t> & input) {
        return sort(input.data(), input.size());
    }

    inline RadixSort & RadixSort::sort(const std::vector<float> & input) {
        return sort(input.data(), input.size());
    }

} // nv namespace



#endif // NV_CORE_RADIXSORT_H
