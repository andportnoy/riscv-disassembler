#define load(t,x,f)  t x; fread(&x, sizeof x, 1, f)
#define jump(x,f)    fseek(f, x, SEEK_SET);
#define skip(x,f)    fseek(f, x, SEEK_CUR);

struct progbits loadbits(char *filename) {
	FILE *f = fopen(filename, "rb");
	char elfmagic[] = {0x7f, 'E', 'L', 'F'};
	char actual[sizeof elfmagic];
	fread(actual, sizeof actual, 1, f);
	if (strncmp(elfmagic, actual, sizeof elfmagic)) {
		fprintf(stderr, "not an ELF file\n");
		exit(EXIT_FAILURE);
	}

	load(char, class, f);
	assert(class == 1); /* 32 bit */

	load(char, data, f);
	assert(data == 1); /* little endian */

	/* ISA */
	jump(0x12, f);
	load(u16, isa, f);
	assert(isa == 0xf3); /* RISC-V */

	/* section header table offset */
	jump(0x20, f);
	load(u32, shoff, f);

	jump(0x2e, f);
	load(u16, shentsize, f);
	load(u16, shnum, f);
	load(u16, shstrndx, f);

	/* find the section containing names */
	jump(shoff + shentsize*shstrndx, f);
	skip(16, f);
	load(u32, namesoff, f);
	load(u32, namessize, f);

	/* load the names array */
	char names[namessize];
	jump(namesoff, f);
	fread(names, sizeof names, 1, f);

	int texti = -1;
	jump(shoff, f);
	for (u16 i=0, n=shnum; i<n; ++i) {
		load(u32, nameoff, f);
		skip(36, f);
		if (!strcmp(".text", &names[nameoff])) {
			texti = i;
			break;
		}
	}

	if (texti < 0) {
		fprintf(stderr, ".text section not found\n");
		exit(EXIT_FAILURE);
	}

	jump(shoff + texti*shentsize, f);
	load(u32, nameoff, f);
	load(u32, type, f);
	skip(8, f);
	load(u32, offset, f);
	load(u32, size, f);

	assert(type == 1); /* PROGBITS */
	assert(size%sizeof(u32) == 0);

	jump(offset, f);
	u32 *text = malloc(size);
	fread(text, size, 1, f);
	fclose(f);

	return (struct progbits){size/sizeof(u32), text};
}
