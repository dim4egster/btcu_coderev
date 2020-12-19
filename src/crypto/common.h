// Copyright (c) 2014-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_COMMON_H
#define BITCOIN_CRYPTO_COMMON_H

#if defined(HAVE_CONFIG_H)
#include <config/btcu-config.h>
#endif
#ifdef WIN32 || WIN64
#include <winsock2.h>
#endif // WIN32 || WIN64

#include <stdint.h>
#include <string.h>

#include <compat/endian.h>
#include <compat/support.h>

#ifdef _WIN64
#define DSSIZE_T
typedef int64_t ssize_t;
#else
#define DSSIZE_T
typedef int32_t ssize_t;
#endif

uint16_t static inline ReadLE16(const unsigned char* ptr)
{
    uint16_t x;
    memcpy((char*)&x, ptr, 2);
    return le16toh(x);
}

uint32_t static inline ReadLE32(const unsigned char* ptr)
{
    uint32_t x;
    memcpy((char*)&x, ptr, 4);
    return le32toh(x);
}

uint64_t static inline ReadLE64(const unsigned char* ptr)
{
    uint64_t x;
    memcpy((char*)&x, ptr, 8);
    return le64toh(x);
}

void static inline WriteLE16(unsigned char* ptr, uint16_t x)
{
    uint16_t v = htole16(x);
    memcpy(ptr, (char*)&v, 2);
}

void static inline WriteLE32(unsigned char* ptr, uint32_t x)
{
    uint32_t v = htole32(x);
    memcpy(ptr, (char*)&v, 4);
}

void static inline WriteLE64(unsigned char* ptr, uint64_t x)
{
    uint64_t v = htole64(x);
    memcpy(ptr, (char*)&v, 8);
}

uint32_t static inline ReadBE32(const unsigned char* ptr)
{
    uint32_t x;
    memcpy((char*)&x, ptr, 4);
    return be32toh(x);
}

uint64_t static inline ReadBE64(const unsigned char* ptr)
{
    uint64_t x;
    memcpy((char*)&x, ptr, 8);
    return be64toh(x);
}

void static inline WriteBE32(unsigned char* ptr, uint32_t x)
{
    uint32_t v = htobe32(x);
    memcpy(ptr, (char*)&v, 4);
}

void static inline WriteBE64(unsigned char* ptr, uint64_t x)
{
    uint64_t v = htobe64(x);
    memcpy(ptr, (char*)&v, 8);
}

#ifdef WIN32
#include <intrin.h>
static uint32_t __inline __builtin_clz(uint32_t x)
{
    unsigned long r = 0;
    _BitScanReverse(&r, x);
    return (31 - r);
}
#endif

inline int __builtin_clzll(unsigned long long mask)
{
    unsigned long where;
// BitScanReverse scans from MSB to LSB for first set bit.
// Returns 0 if no set bit is found.
#if defined(_WIN64)
    if (_BitScanReverse64(&where, mask))
        return static_cast<int>(63 - where);
#elif defined(_WIN32)
    // Scan the high 32 bits.
    if (_BitScanReverse(&where, static_cast<unsigned long>(mask >> 32)))
        return static_cast<int>(63 -
                                (where + 32)); // Create a bit offset from the MSB.
    // Scan the low 32 bits.
    if (_BitScanReverse(&where, static_cast<unsigned long>(mask)))
        return static_cast<int>(63 - where);
#else
#error "Implementation of __builtin_clzll required"
#endif
    return 64; // Undefined Behavior.
}

inline int __builtin_clzl(unsigned long mask)
{
    unsigned long where;
    // Search from LSB to MSB for first set bit.
    // Returns zero if no set bit is found.
    if (_BitScanReverse(&where, mask))
        return static_cast<int>(31 - where);
    return 32; // Undefined Behavior.
}

/*
uint32_t clz64(const uint64_t x)
{
    uint32_t u32 = (x >> 32);
    uint32_t result = u32 ? __builtin_clz(u32) : 32;
    if (result == 32) {
        u32 = x & 0xFFFFFFFFUL;
        result += (u32 ? __builtin_clz(u32) : 32);
    }
    return result;
}*/
/** Return the smallest number n such that (x >> n) == 0 (or 64 if the highest bit in x is set. */
uint64_t static inline CountBits(uint64_t x)
{
#if HAVE_DECL___BUILTIN_CLZL
    if (sizeof(unsigned long) >= sizeof(uint64_t)) {
        return x ? 8 * sizeof(unsigned long) - __builtin_clzl(x) : 0;
    }
#endif
#if HAVE_DECL___BUILTIN_CLZLL
    if (sizeof(unsigned long long) >= sizeof(uint64_t)) {
        return x ? 8 * sizeof(unsigned long long) - __builtin_clzll(x) : 0;
    }
#endif
    int ret = 0;
    while (x) {
        x >>= 1;
        ++ret;
    }
    return ret;
}

#endif // BITCOIN_CRYPTO_COMMON_H