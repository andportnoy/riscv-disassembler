#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t s32;

/* stream of 32 bit fixed length instructions */
struct progbits {
	u32 size;
	u32 *data;
};

struct progbits loadbits(char *filename);
