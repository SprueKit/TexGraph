// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_CORE_UTILS_H
#define NV_CORE_UTILS_H

#include <stdint.h>
#include <new> // for placement new


// Just in case. Grrr.
//#undef min
//#undef max

#define NV_INT8_MIN    (-128)
#define NV_INT8_MAX    127
#define NV_UINT8_MAX    255
#define NV_INT16_MIN    (-32767-1)
#define NV_INT16_MAX    32767
#define NV_UINT16_MAX   0xffff
#define NV_INT32_MIN    (-2147483647-1)
#define NV_INT32_MAX    2147483647
#define NV_UINT32_MAX   0xffffffff
#define NV_INT64_MAX    POSH_I64(9223372036854775807)
#define NV_INT64_MIN    (-POSH_I64(9223372036854775807)-1)
#define NV_UINT64_MAX   POSH_U64(0xffffffffffffffff)

#define NV_HALF_MAX     65504.0F
#define NV_FLOAT_MAX    3.402823466e+38F

#define NV_INTEGER_TO_FLOAT_MAX  16777217     // Largest integer such that it and all smaller integers can be stored in a 32bit float.


namespace nv
{
    // Less error prone than casting. From CB:
    // http://cbloomrants.blogspot.com/2011/06/06-17-11-c-casting-is-devil.html

    // These intentionally look like casts.

    // JRS: stripped it down quite a bit

    /** Return the next power of two. 
    * @see http://graphics.stanford.edu/~seander/bithacks.html
    * @warning Behaviour for 0 is undefined.
    * @note isPowerOfTwo(x) == true -> nextPowerOfTwo(x) == x
    * @note nextPowerOfTwo(x) = 2 << log2(x-1)
    */
    inline uint32_t nextPowerOfTwo(uint32_t x)
    {
        //nvDebugCheck( x != 0 );
#if 1	// On modern CPUs this is supposed to be as fast as using the bsr instruction.
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;	
#else
        uint p = 1;
        while( x > p ) {
            p += p;
        }
        return p;
#endif
    }

    inline uint64_t nextPowerOfTwo(uint64_t x)
    {
        //nvDebugCheck(x != 0);
        uint64_t p = 1;
        while (x > p) {
            p += p;
        }
        return p;
    }

    // @@ Should I just use a macro instead?
    template <typename T>
    inline bool isPowerOfTwo(T n)
    {
        return (n & (n-1)) == 0;
    }

} // nv namespace

#endif // NV_CORE_UTILS_H
