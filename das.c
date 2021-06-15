enum format {R, I, S, B, U, J};

struct {
	u8 opcode; /* 7-bit */
	enum format fmt;
} opcodefmt[] = {
	{0x37, U},
	{0x17, U},
	{0x6f, J},
	{0x67, I},
	{0x63, B},
	{0x03, I},
	{0x23, S},
	{0x13, I},
	{0x33, R},
	{0x0f, I},
	{0x73, I},
};

union encoding {
	u32 insn;
	struct { /* generic */
		u32 opcode :7;
		u32 rd     :5;
		u32 funct3 :3;
		u32 rs1    :5;
		u32 rs2    :5;
		u32 funct7 :7;
	};
	struct {
		u32 opcode :7;
		u32 rd     :5;
		u32 funct3 :3;
		u32 rs1    :5;
		u32 rs2    :5;
		u32 funct7 :7;
	} r;
	struct {
		u32 opcode :7;
		u32 rd     :5;
		u32 funct3 :3;
		u32 rs1    :5;
		s32 i11_0  :12; /* sign extension */
	} i;
	struct {
		u32 opcode :7;
		u32 i4_0   :5;
		u32 funct3 :3;
		u32 rs1    :5;
		u32 rs2    :5;
		s32 i11_5  :7; /* sign extension */
	} s;
	struct {
		u32 opcode :7;
		u32 i11    :1;
		u32 i4_1   :4;
		u32 funct3 :3;
		u32 rs1    :5;
		u32 rs2    :5;
		u32 i10_5  :6;
		s32 i12    :1; /* sign extension */
	} b;
	struct {
		u32 opcode :7;
		u32 rd     :5;
		u32 i31_12 :20;
	} u;
	struct {
		u32 opcode :7;
		u32 rd     :5;
		u32 i19_12 :8;
		u32 i11    :1;
		u32 i10_1  :10;
		s32 i20    :1; /* sign extension */
	} j;
};

int format(u8 opcode) {
	for (int i=0, n=sizeof opcodefmt/sizeof opcodefmt[0]; i<n; ++i)
		if (opcode == opcodefmt[i].opcode)
			return opcodefmt[i].fmt;
	return -1;
}

char *name(u32 insn) {
	union encoding e = {insn};
	switch (format(e.opcode)) {
	case R: switch (e.funct3) {
		case 0: return e.funct7? "sub": "add";
		case 1: return "sll";
		case 2: return "slt";
		case 3: return "sltu";
		case 4: return "xor";
		case 5: return e.funct7? "sra": "srl";
		case 6: return "or";
		case 7: return "and";
		} break;
	case I: switch(e.opcode) {
		case 0x67: return "jalr";
		case 0x03: switch (e.funct3) {
			   case 0: return "lb";
			   case 1: return "lh";
			   case 2: return "lw";
			   case 4: return "lbu";
			   case 5: return "lhu";
			   } break;
		case 0x13: switch (e.funct3) {
			   case 0: return "addi";
			   case 1: return "slli";
			   case 2: return "slti";
			   case 3: return "sltiu";
			   case 4: return "xori";
			   case 5: return e.funct7? "srai": "srli";
			   case 6: return "ori";
			   case 7: return "andi";
			   } break;
		case 0x0f: switch (e.funct3) {
			   case 0: return "fence";
			   case 1: return "fence.i";
			   } break;
		case 0x73: switch (e.funct3) {
			   case 0: return e.rs2? "ebreak": "ecall";
			   case 1: return "csrrw";
			   case 2: return "csrrs";
			   case 3: return "csrrc";
			   case 5: return "csrrwi";
			   case 6: return "csrrsi";
			   case 7: return "csrrci";
			   } break;
		} break;
	case S: switch(e.funct3) {
		case 0: return "sb";
		case 1: return "sh";
		case 2: return "sw";
		} break;
	case B: switch(e.funct3) {
		case 0: return "beq";
		case 1: return "bne";
		case 4: return "blt";
		case 5: return "bge";
		case 6: return "bltu";
		case 7: return "bgeu";
		} break;
	case U: switch(e.opcode) {
		case 0x37: return "lui";
		case 0x17: return "auipc";
		} break;
	case J: return "jal";
	}

	return NULL;
}

char *op0(u32 insn) {
	union encoding e = {insn};
	char *name = calloc(16+1, sizeof *name);
	switch (format(e.opcode)) {
	case R:
	case I: sprintf(name, "x%d", e.rd);  break;
	case S: sprintf(name, "x%d", e.rs2); break;
	case B: sprintf(name, "x%d", e.rs1); break;
	case U:
	case J: sprintf(name, "x%d", e.rd);  break;
	}
	return name;
}

char *op1(u32 insn) {
	union encoding e = {insn};
	char *name = calloc(16+1, sizeof *name);
	switch (format(e.opcode)) {
	case R:
	case I:
	case S: sprintf(name, "x%d", e.rs1);            break;
	case B: sprintf(name, "x%d", e.rs2);            break;
	case U: sprintf(name, "0x%x", e.u.i31_12);      break;
	case J: sprintf(name, "%d", (e.j.i20    <<20) |
				    (e.j.i19_12 <<12) |
				    (e.j.i11    <<11) |
				    (e.j.i10_1  << 1)); break;
	}
	return name;
}

char *op2(u32 insn) {
	union encoding e = {insn};
	char *name = calloc(16+1, sizeof *name);
	switch (format(e.opcode)) {
	case R: sprintf(name, "x%d", e.rs2);                    break;
	case I: sprintf(name, "%d", e.i.i11_0);                 break;
	case S: sprintf(name, "%d", (e.s.i11_5<<5) | e.s.i4_0); break;
	case B: sprintf(name, "%d", (e.b.i12   <<12) |
				    (e.b.i11   <<11) |
				    (e.b.i10_5 << 5) |
				    (e.b.i4_1  << 1));          break;
	case U:                                                 break;
	case J:                                                 break;
	}
	return name;
}

int main(int argc, char **argv) {
   struct progbits pb; 
   if ((argc > 1) & (!strcmp(argv[1],"-r"))) 
   {
    printf("disassembling raw binary %s\n", argv[2]);
    pb = loadbits_raw(argv[2]);
    
   }
   else 
   {
    pb = loadbits(argv[1]);
   }
   
	
	for (u32 i=0, n=pb.size, insn=pb.data[i]; i<n; ++i, insn=pb.data[i])
		printf("%4x:  %08x          %-8s %3s %3s %3s\n",
		  4*i, insn, name(insn), op0(insn), op1(insn), op2(insn));
}




