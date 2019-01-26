// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "RadixSort.h"

//#include "Utils.h"

#include <algorithm>
#include <string.h> // memset

using namespace nv;

static inline void FloatFlip(uint32_t & f) {
    //uint32 mask = -int32(f >> 31) | 0x80000000; // Michael Herf.
    int32_t mask = (int32_t(f) >> 31) | 0x80000000; // Warren Hunt, Manchor Ko.
    f ^= mask;
}

static inline void IFloatFlip(uint32_t & f) {
    uint32_t mask = ((f >> 31) - 1) | 0x80000000; // Michael Herf.
    //uint32 mask = (int32(f ^ 0x80000000) >> 31) | 0x80000000; // Warren Hunt, Manchor Ko. @@ Correct, but fails in release on gcc-4.2.1
    f ^= mask;
}


template<typename T> 
void createHistograms(const T * buffer, uint32_t count, uint32_t * histogram)
{
    const uint32_t bucketCount = sizeof(T); // (8 * sizeof(T)) / log2(radix)

    // Init bucket pointers.
    uint32_t* h[bucketCount];
    for (uint32_t i = 0; i < bucketCount; i++) {
#if NV_BIG_ENDIAN
        h[sizeof(T)-1-i] = histogram + 256 * i;
#else
        h[i] = histogram + 256 * i;
#endif
    }

    // Clear histograms.
    memset(histogram, 0, 256 * bucketCount * sizeof(uint32_t));

    // @@ Add support for signed integers.

    // Build histograms.
    const uint8_t* p = (const uint8_t*)buffer; // @@ Does this break aliasing rules?
    const uint8_t* pe = p + count * sizeof(T);

    while (p != pe) {
        h[0][*p++]++, h[1][*p++]++, h[2][*p++]++, h[3][*p++]++;
        if (bucketCount == 8)
            h[4][*p++]++, h[5][*p++]++, h[6][*p++]++, h[7][*p++]++;
    }
}

/*
template <>
void createHistograms<float>(const float * buffer, uint count, uint * histogram)
{
    // Init bucket pointers.
    uint32 * h[4];
    for (uint i = 0; i < 4; i++) {
#if NV_BIG_ENDIAN
        h[3-i] = histogram + 256 * i;
#else
        h[i] = histogram + 256 * i;
#endif
    }

    // Clear histograms.
    memset(histogram, 0, 256 * 4 * sizeof(uint32));

    // Build histograms.
    for (uint i = 0; i < count; i++) {
        uint32 fi = FloatFlip(buffer[i]);

        h[0][fi & 0xFF]++;
        h[1][(fi >> 8) & 0xFF]++;
        h[2][(fi >> 16) & 0xFF]++;
        h[3][fi >> 24]++;
    }
}
*/

RadixSort::RadixSort() : m_size(0), m_ranks(NULL), m_ranks2(NULL), m_validRanks(false)
{
}

RadixSort::RadixSort(uint32_t reserve_count) : m_size(0), m_ranks(NULL), m_ranks2(NULL), m_validRanks(false)
{
    checkResize(reserve_count);
}

RadixSort::~RadixSort()
{
    // Release everything
    if (m_ranks2)
        delete[] m_ranks2;
    if (m_ranks)
        delete[] m_ranks;
}


void RadixSort::resize(uint32_t count)
{
    if (m_ranks2)
        delete[] m_ranks2;
    if (m_ranks)
        delete[] m_ranks;
    m_ranks2 = new uint32_t[count];
    m_ranks = new uint32_t[count];
}

inline void RadixSort::checkResize(uint32_t count)
{
    if (count != m_size)
    {
        if (count > m_size) resize(count);
        m_size = count;
        m_validRanks = false;
    }
}

template <typename T> inline void RadixSort::insertionSort(const T * input, uint32_t count)
{
    if (!m_validRanks) {
        /*for (uint i = 0; i < count; i++) {
            m_ranks[i] = i;
        }*/

        m_ranks[0] = 0;
        for (uint32_t i = 1; i != count; ++i)
        {
            int rank = m_ranks[i] = i;

            uint32_t j = i;
            while (j != 0 && input[rank] < input[m_ranks[j-1]])
            {
                m_ranks[j] = m_ranks[j-1];
                --j;
            }
            if (i != j)
            {
                m_ranks[j] = rank;
            }
        }

        m_validRanks = true;
    }
    else {
        for (uint32_t i = 1; i != count; ++i)
        {
            int rank = m_ranks[i];

            uint32_t j = i;
            while (j != 0 && input[rank] < input[m_ranks[j-1]])
            {
                m_ranks[j] = m_ranks[j-1];
                --j;
            }
            if (i != j)
            {
                m_ranks[j] = rank;
            }
        }
    }
}

template <typename T> inline void RadixSort::radixSort(const T * input, uint32_t count)
{
    const uint32_t P = sizeof(T); // pass count

    // Allocate histograms & offsets on the stack
    uint32_t histogram[256 * P];
    uint32_t * link[256];

    createHistograms(input, count, histogram);

    // Radix sort, j is the pass number (0=LSB, P=MSB)
    for (uint32_t j = 0; j < P; j++)
    {
        // Pointer to this bucket.
        const uint32_t * h = &histogram[j * 256];

        const uint8_t * inputBytes = (const uint8_t*)input; // @@ Is this aliasing legal?

#if NV_BIG_ENDIAN
        inputBytes += P - 1 - j;
#else
        inputBytes += j;
#endif

        if (h[inputBytes[0]] == count) {
            // Skip this pass, all values are the same.
            continue;
        }

        // Create offsets
        link[0] = m_ranks2;
        for (uint32_t i = 1; i < 256; i++) link[i] = link[i - 1] + h[i - 1];

        // Perform Radix Sort
        if (!m_validRanks)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                *link[inputBytes[i*P]]++ = i;
            }
            m_validRanks = true;
        }
        else
        {
            for (uint32_t i = 0; i < count; i++)
            {
                const uint32_t idx = m_ranks[i];
                *link[inputBytes[idx*P]]++ = idx;
            }
        }

        // Swap pointers for next pass. Valid indices - the most recent ones - are in m_ranks after the swap.
        std::swap(m_ranks, m_ranks2);
    }

    // All values were equal, generate linear ranks.
    if (!m_validRanks)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            m_ranks[i] = i;
        }
        m_validRanks = true;
    }
}


RadixSort & RadixSort::sort(const uint32_t * input, uint32_t count)
{
    if (input == NULL || count == 0) return *this;

    // Resize lists if needed
    checkResize(count);

    if (count < 32) {
        insertionSort(input, count);
    }
    else {
        radixSort<uint32_t>(input, count);
    }
    return *this;
}


RadixSort & RadixSort::sort(const uint64_t * input, uint32_t count)
{
    if (input == NULL || count == 0) return *this;

    // Resize lists if needed
    checkResize(count);

    if (count < 64) {
        insertionSort(input, count);
    }
    else {
        radixSort(input, count);
    }
    return *this;
}

RadixSort& RadixSort::sort(const float * input, uint32_t count)
{
    if (input == NULL || count == 0) return *this;

    // Resize lists if needed
    checkResize(count);

    if (count < 32) {
        insertionSort(input, count);
    }
    else {
        // @@ Avoid touching the input multiple times.
        for (uint32_t i = 0; i < count; i++) {
            FloatFlip((uint32_t &)input[i]);
        }

        radixSort<uint32_t>((const uint32_t *)input, count);

        for (uint32_t i = 0; i < count; i++) {
            IFloatFlip((uint32_t &)input[i]);
        }
    }

    return *this;
}
