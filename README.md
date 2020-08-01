# A minimal RISC-V RV32I dissassembler

```
$ ./das fib.o
   0:  004000ef            jal       x1   4    
   4:  00800713            addi     x14  x0   8
   8:  80000537            lui      x10 0x80000    
   c:  00052583            lw       x11 x10   0
  10:  00450513            addi     x10 x10   4
  14:  00052603            lw       x12 x10   0
  18:  00c586b3            add      x13 x11 x12
  1c:  00450513            addi     x10 x10   4
  20:  00d52023            sw       x13 x10   0
  24:  00c005b3            add      x11  x0 x12
  28:  00d00633            add      x12  x0 x13
  2c:  fff70713            addi     x14 x14  -1
  30:  fe0714e3            bne      x14  x0 -24
```

## What's missing
- ABI register names
- proper printing for CSR and ecall/ebreak operands
- proper handling of immediate shift amounts
- selective hex/decimal printing of immediates
- handling of 16-bit compressed instructions
- handling of symbols and the .data section
