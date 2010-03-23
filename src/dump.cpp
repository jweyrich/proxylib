#include <proxylib/dump.h>
#include <ctype.h>

namespace proxylib {

void print_bits(FILE* stream, uint64 value, size_t size) {
	register int32 i;
	size *= 8;
	for (i = size - 1; i >= 0; --i)
		fputc((value & (1 << i)) == 0 ? '0' : '1', stream);
}

void dump_hex(FILE* stream, const byte* data, size_t size, uint32 offset) {
	int ch;
	register uint32 i, j, cols;

	for (i = 0; i < size; i += 16) {
		fprintf(stream, "%04x: ", i + offset);
		cols = size - i;
		cols = cols > 16 ? 16 : cols;

		for (j = 0; j < cols; ++j) {
			if ((j % 2) == 0)
				fprintf(stream, "%02x", (uint32)data[i+j]);
			else
				fprintf(stream, "%02x ", (uint32)data[i+j]);
		}
		for (; j < 16; ++j) {
			fprintf(stream, "%*s", (j % 2) == 0 ? 2 : 3, " ");
		}
		fprintf(stream, " ");

		for (j = 0; j < cols; ++j) {
			ch = data[i+j];
			ch = isprint(ch) ? ch : '.';
			fprintf(stream, "%c", ch);
		}
		fprintf(stream, "\n");
	}
}

} // namespace proxylib
