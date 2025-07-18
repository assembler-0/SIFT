/*Added cache testing ang logging
 * PCG Random Number Generation for C++
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcAdded cache testing ang loggingg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

/*
 * This code provides a a C++ class that can provide 128-bit (or higher)
 * integers.  To produce 2K-bit integers, it uses two K-bit integers,
 * placed in a union that allowes the code to also see them as four K/2 bit
 * integers (and access them either directly name, or by index).
 *
 * It may seem like we're reinventing the wheel here, because several
 * libraries already exist that support large integers, but most existing
 * libraries provide a very generic multiprecision code, but here we're
 * operating at a fixed size.  Also, most other libraries are fairly
 * heavyweight.  So we use a direct implementation.  Sadly, it's much slower
 * than hand-coded assembly or direct CPU support.
 */

#ifndef PCG_UINT128_HPP_INCLUDED
#define PCG_UINT128_HPP_INCLUDED 1

#include <cstdint>
#include <cstdio>
#include <cassert>
#include <climits>
#include <utility>
#include <initializer_list>
#include <type_traits>

/*
 * We want to lay the type out the same way that a native type would be laid
 * out, which means we must know the machine's endian, at compile time.
 * This ugliness attempts to do so.
 */

#ifndef PCG_LITTLE_ENDIAN
    #if defined(__BYTE_ORDER__)
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            #define PCG_LITTLE_ENDIAN 1
        #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            #define PCG_LITTLE_ENDIAN 0
        #else
            #error __BYTE_ORDER__ does not match a standard endian, pick a side
        #endif
    #elif __LITTLE_ENDIAN__ || _LITTLE_ENDIAN
        #define PCG_LITTLE_ENDIAN 1
    #elif __BIG_ENDIAN__ || _BIG_ENDIAN
        #define PCG_LITTLE_ENDIAN 0
    #elif __x86_64 || __x86_64__ || __i386 || __i386__
        #define PCG_LITTLE_ENDIAN 1
    #elif __powerpc__ || __POWERPC__ || __ppc__ || __PPC__ \
          || __m68k__ || __mc68000__
        #define PCG_LITTLE_ENDIAN 0
    #else
        #error Unable to determine target endianness
    #endif
#endif

namespace pcg_extras {

// Recent versions of GCC have intrinsics we can use to quickly calculate
// the number of leading and trailing zeros in a number.  If possible, we
// use them, otherwise we fall back to old-fashioned bit twiddling to figure
// them out.

#ifndef PCG_BITCOUNT_T
    typedef uint8_t bitcount_t;
#else
    typedef PCG_BITCOUNT_T bitcount_t;
#endif

/*
 * Provide some useful helper functions
 *      * flog2                 floor(log2(x))
 *      * trailingzeros         number of trailing zero bits
 */

#ifdef __GNUC__         // Any GNU-compatible compiler supporting C++11 has
                        // some useful intrinsics we can use.

inline bitcount_t flog2(uint32_t v)
{
    return 31 - __builtin_clz(v);
}

inline bitcount_t trailingzeros(uint32_t v)
{
    return __builtin_ctz(v);
}

inline bitcount_t flog2(uint64_t v)
{
#if UINT64_MAX == ULONG_MAX
    return 63 - __builtin_clzl(v);
#elif UINT64_MAX == ULLONG_MAX
    return 63 - __builtin_clzll(v);
#else
    #error Cannot find a function for uint64_t
#endif
}

inline bitcount_t trailingzeros(uint64_t v)
{
#if UINT64_MAX == ULONG_MAX
    return __builtin_ctzl(v);
#elif UINT64_MAX == ULLONG_MAX
    return __builtin_ctzll(v);
#else
    #error Cannot find a function for uint64_t
#endif
}

#else                   // Otherwise, we fall back to bit twiddling
                        // implementations

inline bitcount_t flog2(uint32_t v)
{
    // Based on code by Eric Cole and Mark Dickinson, which appears at
    // https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn

    static const uint8_t multiplyDeBruijnBitPos[32] = {
      0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
      8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
    };

    v |= v >> 1; // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return multiplyDeBruijnBitPos[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

inline bitcount_t trailingzeros(uint32_t v)
{
    static const uint8_t multiplyDeBruijnBitPos[32] = {
      0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
      31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };

    return multiplyDeBruijnBitPos[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}

inline bitcount_t flog2(uint64_t v)
{
    uint32_t high = v >> 32;
    uint32_t low  = uint32_t(v);

    return high ? 32+flog2(high) : flog2(low);
}

inline bitcount_t trailingzeros(uint64_t v)
{
    uint32_t high = v >> 32;
    uint32_t low  = uint32_t(v);

    return low ? trailingzeros(low) : trailingzeros(high)+32;
}

#endif

template <typename UInt>
inline bitcount_t clog2(UInt v)
{
    return flog2(v) + ((v & (-v)) != v);
}

template <typename UInt>
inline UInt addwithcarry(UInt x, UInt y, bool carryin, bool* carryout)
{
    UInt half_result = y + carryin;
    UInt result = x + half_result;
    *carryout = (half_result < y) || (result < x);
    return result;
}

template <typename UInt>
inline UInt subwithcarry(UInt x, UInt y, bool carryin, bool* carryout)
{
    UInt half_result = y + carryin;
    UInt result = x - half_result;
    *carryout = (half_result < y) || (result > x);
    return result;
}


template <typename UInt, typename UIntX2>
class uint_x4 {
// private:
public:
    union {
#if PCG_LITTLE_ENDIAN
        struct {
            UInt v0, v1, v2, v3;
        } w;
        struct {
            UIntX2 v01, v23;
        } d;
#else
        struct {
            UInt v3, v2, v1, v0;
        } w;
        struct {
            UIntX2 v23, v01;
        } d;
#endif
        // For the array access versions, the code that uses the array
        // must handle endian itself.  Yuck.
        UInt wa[4];
        UIntX2 da[2];
    };

public:
    uint_x4() = default;

    constexpr uint_x4(UInt v3, UInt v2, UInt v1, UInt v0)
#if PCG_LITTLE_ENDIAN
       : w{v0, v1, v2, v3}
#else
       : w{v3, v2, v1, v0}
#endif
    {
        // Nothing (else) to do
    }

    constexpr uint_x4(UIntX2 v23, UIntX2 v01)
#if PCG_LITTLE_ENDIAN
       : d{v01,v23}
#else
       : d{v23,v01}
#endif
    {
        // Nothing (else) to do
    }

    template<class Integral,
             typename std::enable_if<(std::is_integral<Integral>::value
                                      && sizeof(Integral) <= sizeof(UIntX2))
                                    >::type* = nullptr>
    constexpr uint_x4(Integral v01)
#if PCG_LITTLE_ENDIAN
       : d{UIntX2(v01),0UL}
#else
       : d{0UL,UIntX2(v01)}
#endif
    {
        // Nothing (else) to do
    }

    explicit constexpr operator uint64_t() const
    {
        return d.v01;
    }

    explicit constexpr operator uint32_t() const
    {
        return w.v0;
    }

    explicit constexpr operator int() const
    {
        return w.v0;
    }

    explicit constexpr operator uint16_t() const
    {
        return w.v0;
    }

    explicit constexpr operator uint8_t() const
    {
        return w.v0;
    }

    typedef typename std::conditional<std::is_same<uint64_t,
                                                   unsigned long>::value,
                                      unsigned long long,
                                      unsigned long>::type
            uint_missing_t;

    explicit constexpr operator uint_missing_t() const
    {
        return d.v01;
    }

    explicit constexpr operator bool() const
    {
        return d.v01 || d.v23;
    }

    template<typename U, typename V>
    friend uint_x4<U,V> operator*(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend std::pair< uint_x4<U,V>,uint_x4<U,V> >
        divmod(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator+(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator-(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator<<(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator>>(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator&(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator|(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator^(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator==(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator!=(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator<(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator<=(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator>(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bool operator>=(const uint_x4<U,V>&, const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator~(const uint_x4<U,V>&);

    template<typename U, typename V>
    friend uint_x4<U,V> operator-(const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bitcount_t flog2(const uint_x4<U,V>&);

    template<typename U, typename V>
    friend bitcount_t trailingzeros(const uint_x4<U,V>&);

    uint_x4& operator*=(const uint_x4& rhs)
    {
        uint_x4 result = *this * rhs;
        return *this = result;
    }

    uint_x4& operator/=(const uint_x4& rhs)
    {
        uint_x4 result = *this / rhs;
        return *this = result;
    }

    uint_x4& operator%=(const uint_x4& rhs)
    {
        uint_x4 result = *this % rhs;
        return *this = result;
    }

    uint_x4& operator+=(const uint_x4& rhs)
    {
        uint_x4 result = *this + rhs;
        return *this = result;
    }

    uint_x4& operator-=(const uint_x4& rhs)
    {
        uint_x4 result = *this - rhs;
        return *this = result;
    }

    uint_x4& operator&=(const uint_x4& rhs)
    {
        uint_x4 result = *this & rhs;
        return *this = result;
    }

    uint_x4& operator|=(const uint_x4& rhs)
    {
        uint_x4 result = *this | rhs;
        return *this = result;
    }

    uint_x4& operator^=(const uint_x4& rhs)
    {
        uint_x4 result = *this ^ rhs;
        return *this = result;
    }

    uint_x4& operator>>=(bitcount_t shift)
    {
        uint_x4 result = *this >> shift;
        return *this = result;
    }

    uint_x4& operator<<=(bitcount_t shift)
    {
        uint_x4 result = *this << shift;
        return *this = result;
    }

};

template<typename U, typename V>
bitcount_t flog2(const uint_x4<U,V>& v)
{
#if PCG_LITTLE_ENDIAN
    for (uint8_t i = 4; i !=0; /* dec in loop */) {
        --i;
#else
    for (uint8_t i = 0; i < 4; ++i) {
#endif
        if (v.wa[i] == 0)
             continue;
        return flog2(v.wa[i]) + (sizeof(U)*CHAR_BIT)*i;
    }
    abort();
}

template<typename U, typename V>
bitcount_t trailingzeros(const uint_x4<U,V>& v)
{
#if PCG_LITTLE_ENDIAN
    for (uint8_t i = 0; i < 4; ++i) {
#else
    for (uint8_t i = 4; i !=0; /* dec in loop */) {
        --i;
#endif
        if (v.wa[i] != 0)
            return trailingzeros(v.wa[i]) + (sizeof(U)*CHAR_BIT)*i;
    }
    return (sizeof(U)*CHAR_BIT)*4;
}

template <typename UInt, typename UIntX2>
std::pair< uint_x4<UInt,UIntX2>, uint_x4<UInt,UIntX2> >
    divmod(const uint_x4<UInt,UIntX2>& orig_dividend,
           const uint_x4<UInt,UIntX2>& divisor)
{
    // If the dividend is less than the divisor, the answer is always zero.
    // This takes care of boundary cases like 0/x (which would otherwise be
    // problematic because we can't take the log of zero.  (The boundary case
    // of division by zero is undefined.)
    if (orig_dividend < divisor)
        return { uint_x4<UInt,UIntX2>(0UL), orig_dividend };

    auto dividend = orig_dividend;

    auto log2_divisor  = flog2(divisor);
    auto log2_dividend = flog2(dividend);
    // assert(log2_dividend >= log2_divisor);
    bitcount_t logdiff = log2_dividend - log2_divisor;

    constexpr uint_x4<UInt,UIntX2> ONE(1UL);
    if (logdiff == 0)
        return { ONE, dividend - divisor };

    // Now we change the log difference to
    //  floor(log2(divisor)) - ceil(log2(dividend))
    // to ensure that we *underestimate* the result.
    logdiff -= 1;

    uint_x4<UInt,UIntX2> quotient(0UL);

    auto qfactor = ONE << logdiff;
    auto factor  = divisor << logdiff;

    do {
        dividend -= factor;
        quotient += qfactor;
        while (dividend < factor) {
            factor  >>= 1;
            qfactor >>= 1;
        }
    } while (dividend >= divisor);

    return { quotient, dividend };
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator/(const uint_x4<UInt,UIntX2>& dividend,
                               const uint_x4<UInt,UIntX2>& divisor)
{
    return divmod(dividend, divisor).first;
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator%(const uint_x4<UInt,UIntX2>& dividend,
                               const uint_x4<UInt,UIntX2>& divisor)
{
    return divmod(dividend, divisor).second;
}


template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator*(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    uint_x4<UInt,UIntX2> r = {0U, 0U, 0U, 0U};
    bool carryin = false;
    bool carryout;
    UIntX2 a0b0 = UIntX2(a.w.v0) * UIntX2(b.w.v0);
    r.w.v0 = UInt(a0b0);
    r.w.v1 = UInt(a0b0 >> 32);

    UIntX2 a1b0 = UIntX2(a.w.v1) * UIntX2(b.w.v0);
    r.w.v2 = UInt(a1b0 >> 32);
    r.w.v1 = addwithcarry(r.w.v1, UInt(a1b0), carryin, &carryout);
    carryin = carryout;
    r.w.v2 = addwithcarry(r.w.v2, UInt(0U), carryin, &carryout);
    carryin = carryout;
    r.w.v3 = addwithcarry(r.w.v3, UInt(0U), carryin, &carryout);

    UIntX2 a0b1 = UIntX2(a.w.v0) * UIntX2(b.w.v1);
    carryin = false;
    r.w.v2 = addwithcarry(r.w.v2, UInt(a0b1 >> 32), carryin, &carryout);
    carryin = carryout;
    r.w.v3 = addwithcarry(r.w.v3, UInt(0U), carryin, &carryout);

    carryin = false;
    r.w.v1 = addwithcarry(r.w.v1, UInt(a0b1), carryin, &carryout);
    carryin = carryout;
    r.w.v2 = addwithcarry(r.w.v2, UInt(0U), carryin, &carryout);
    carryin = carryout;
    r.w.v3 = addwithcarry(r.w.v3, UInt(0U), carryin, &carryout);

    UIntX2 a1b1 = UIntX2(a.w.v1) * UIntX2(b.w.v1);
    carryin = false;
    r.w.v2 = addwithcarry(r.w.v2, UInt(a1b1), carryin, &carryout);
    carryin = carryout;
    r.w.v3 = addwithcarry(r.w.v3, UInt(a1b1 >> 32), carryin, &carryout);

    r.d.v23 += a.d.v01 * b.d.v23 + a.d.v23 * b.d.v01;

    return r;
}


template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator+(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    uint_x4<UInt,UIntX2> r = {0U, 0U, 0U, 0U};

    bool carryin = false;
    bool carryout;
    r.w.v0 = addwithcarry(a.w.v0, b.w.v0, carryin, &carryout);
    carryin = carryout;
    r.w.v1 = addwithcarry(a.w.v1, b.w.v1, carryin, &carryout);
    carryin = carryout;
    r.w.v2 = addwithcarry(a.w.v2, b.w.v2, carryin, &carryout);
    carryin = carryout;
    r.w.v3 = addwithcarry(a.w.v3, b.w.v3, carryin, &carryout);

    return r;
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator-(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    uint_x4<UInt,UIntX2> r = {0U, 0U, 0U, 0U};

    bool carryin = false;
    bool carryout;
    r.w.v0 = subwithcarry(a.w.v0, b.w.v0, carryin, &carryout);
    carryin = carryout;
    r.w.v1 = subwithcarry(a.w.v1, b.w.v1, carryin, &carryout);
    carryin = carryout;
    r.w.v2 = subwithcarry(a.w.v2, b.w.v2, carryin, &carryout);
    carryin = carryout;
    r.w.v3 = subwithcarry(a.w.v3, b.w.v3, carryin, &carryout);

    return r;
}


template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator&(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    return uint_x4<UInt,UIntX2>(a.d.v23 & b.d.v23, a.d.v01 & b.d.v01);
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator|(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    return uint_x4<UInt,UIntX2>(a.d.v23 | b.d.v23, a.d.v01 | b.d.v01);
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator^(const uint_x4<UInt,UIntX2>& a,
                               const uint_x4<UInt,UIntX2>& b)
{
    return uint_x4<UInt,UIntX2>(a.d.v23 ^ b.d.v23, a.d.v01 ^ b.d.v01);
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator~(const uint_x4<UInt,UIntX2>& v)
{
    return uint_x4<UInt,UIntX2>(~v.d.v23, ~v.d.v01);
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator-(const uint_x4<UInt,UIntX2>& v)
{
    return uint_x4<UInt,UIntX2>(0UL,0UL) - v;
}

template <typename UInt, typename UIntX2>
bool operator==(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return (a.d.v01 == b.d.v01) && (a.d.v23 == b.d.v23);
}

template <typename UInt, typename UIntX2>
bool operator!=(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return !operator==(a,b);
}


template <typename UInt, typename UIntX2>
bool operator<(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return (a.d.v23 < b.d.v23)
           || ((a.d.v23 == b.d.v23) && (a.d.v01 < b.d.v01));
}

template <typename UInt, typename UIntX2>
bool operator>(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return operator<(b,a);
}

template <typename UInt, typename UIntX2>
bool operator<=(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return !(operator<(b,a));
}

template <typename UInt, typename UIntX2>
bool operator>=(const uint_x4<UInt,UIntX2>& a, const uint_x4<UInt,UIntX2>& b)
{
    return !(operator<(a,b));
}



template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator<<(const uint_x4<UInt,UIntX2>& v,
                                const bitcount_t shift)
{
    uint_x4<UInt,UIntX2> r = {0U, 0U, 0U, 0U};
    const bitcount_t bits    = sizeof(UInt) * CHAR_BIT;
    const bitcount_t bitmask = bits - 1;
    const bitcount_t shiftdiv = shift / bits;
    const bitcount_t shiftmod = shift & bitmask;

    if (shiftmod) {
        UInt carryover = 0;
#if PCG_LITTLE_ENDIAN
        for (uint8_t out = shiftdiv, in = 0; out < 4; ++out, ++in) {
#else
        for (uint8_t out = 4-shiftdiv, in = 4; out != 0; /* dec in loop */) {
            --out, --in;
#endif
            r.wa[out] = (v.wa[in] << shiftmod) | carryover;
            carryover = (v.wa[in] >> (bits - shiftmod));
        }
    } else {
#if PCG_LITTLE_ENDIAN
        for (uint8_t out = shiftdiv, in = 0; out < 4; ++out, ++in) {
#else
        for (uint8_t out = 4-shiftdiv, in = 4; out != 0; /* dec in loop */) {
            --out, --in;
#endif
            r.wa[out] = v.wa[in];
        }
    }

    return r;
}

template <typename UInt, typename UIntX2>
uint_x4<UInt,UIntX2> operator>>(const uint_x4<UInt,UIntX2>& v,
                                const bitcount_t shift)
{
    uint_x4<UInt,UIntX2> r = {0U, 0U, 0U, 0U};
    const bitcount_t bits    = sizeof(UInt) * CHAR_BIT;
    const bitcount_t bitmask = bits - 1;
    const bitcount_t shiftdiv = shift / bits;
    const bitcount_t shiftmod = shift & bitmask;

    if (shiftmod) {
        UInt carryover = 0;
#if PCG_LITTLE_ENDIAN
        for (uint8_t out = 4-shiftdiv, in = 4; out != 0; /* dec in loop */) {
            --out, --in;
#else
        for (uint8_t out = shiftdiv, in = 0; out < 4; ++out, ++in) {
#endif
            r.wa[out] = (v.wa[in] >> shiftmod) | carryover;
            carryover = (v.wa[in] << (bits - shiftmod));
        }
    } else {
#if PCG_LITTLE_ENDIAN
        for (uint8_t out = 4-shiftdiv, in = 4; out != 0; /* dec in loop */) {
            --out, --in;
#else
        for (uint8_t out = shiftdiv, in = 0; out < 4; ++out, ++in) {
#endif
            r.wa[out] = v.wa[in];
        }
    }

    return r;
}

} // namespace pcg_extras

#endif // PCG_UINT128_HPP_INCLUDED
