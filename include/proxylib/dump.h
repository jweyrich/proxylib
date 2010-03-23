#pragma once

#include <cstdio>
#include <cstddef>
#include <proxylib/compat/types.h>

namespace proxylib {

void print_bits(FILE* stream, uint64 value, size_t size);
void dump_hex(FILE* stream, const byte* data, size_t size, uint32 offset);

} // namespace proxylib
