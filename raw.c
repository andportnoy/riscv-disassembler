struct progbits loadbits_raw(char *filename) {
	FILE *f = fopen(filename, "rb");
	unsigned int size;
	fseek(f, 0, SEEK_END); // seek to end of file
    size = ftell(f); // get current file pointer
    fseek(f, 0, SEEK_SET); // seek back to beginning of file
	u32 *text = malloc(size);
	fread(text, size, 1, f);
	fclose(f);

	return (struct progbits){size/sizeof(u32), text};
    
}
