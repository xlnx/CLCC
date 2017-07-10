           .ORIG       x3000
           LD          R4, #3
           LD          R5, #3
           LD          R6, #3
           BR          Fnmain
           .FILL       STACK
           .FILL       FUNCTION
           .FILL       STATIC
Fnmain
           ADD         R1, R6, #1
           STR         R1, R4, #1
; Lvalue
           LD          R1, #1
           BR          LBL0
           .FILL       #0      ; 16-bit literal
LBL0           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2           STR         R1, R4, #1
; a[i]
           LD          R1, #1
           BR          LBL1
           .FILL       #0      ; 16-bit literal
LBL1           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           STR         R2, R1, #0
           STR         R2, R4, #1
; Store
           ADD         R1, R6, #1
           STR         R1, R4, #1
; Lvalue
           LD          R1, #1
           BR          LBL2
           .FILL       #1      ; 16-bit literal
LBL2           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2           STR         R1, R4, #1
; a[i]
           LD          R1, #1
           BR          LBL3
           .FILL       #1      ; 16-bit literal
LBL3           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           STR         R2, R1, #0
           STR         R2, R4, #1
; Store
           LD          R1, #1
           BR          LBL6
           .FILL       #2      ; 16-bit literal
LBL6           STR         R1, R4, #1
           LDR         R1, R4, #1
           STR         R1, R4, #-2
           AND         R0, R0, #0
LBL5
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL7
           .FILL       #10      ; 16-bit literal
LBL7           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           NOT         R2, R2
           ADD         R2, R2, #1
           AND         R0, R0, #0
           ADD         R1, R1, R2
           BRz         LBL8
           ADD         R0, R0, #1
LBL8           STR         R0, R4, #1
; a != b
           LDR         R0, R4, #1
           BRz         LBL4
           ADD         R1, R6, #1
           STR         R1, R4, #1
; Lvalue
           ADD         R1, R4, #-2
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2           STR         R1, R4, #1
; a[i]
           ADD         R1, R6, #1
           STR         R1, R4, #2
; Lvalue
           ADD         R1, R4, #-2
           STR         R1, R4, #3
; Lvalue
           LDR         R7, R4, #3
           LDR         R7, R7, #0
           STR         R7, R4, #3
; To Rvalue
           LD          R1, #1
           BR          LBL9
           .FILL       #1      ; 16-bit literal
LBL9           STR         R1, R4, #4
           LDR         R2, R4, #4
           LDR         R1, R4, #3
           NOT         R2, R2
           ADD         R2, R2, #1
           ADD         R1, R1, R2
           STR         R1, R4, #3
; a - b
           LDR         R2, R4, #3
           LDR         R1, R4, #2
           ADD         R1, R1, R2           STR         R1, R4, #2
; a[i]
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           ADD         R1, R6, #1
           STR         R1, R4, #3
; Lvalue
           ADD         R1, R4, #-2
           STR         R1, R4, #4
; Lvalue
           LDR         R7, R4, #4
           LDR         R7, R7, #0
           STR         R7, R4, #4
; To Rvalue
           LD          R1, #1
           BR          LBL10
           .FILL       #2      ; 16-bit literal
LBL10           STR         R1, R4, #5
           LDR         R2, R4, #5
           LDR         R1, R4, #4
           NOT         R2, R2
           ADD         R2, R2, #1
           ADD         R1, R1, R2
           STR         R1, R4, #4
; a - b
           LDR         R2, R4, #4
           LDR         R1, R4, #3
           ADD         R1, R1, R2           STR         R1, R4, #3
; a[i]
           LDR         R7, R4, #3
           LDR         R7, R7, #0
           STR         R7, R4, #3
; To Rvalue
           LDR         R2, R4, #3
           LDR         R1, R4, #2
           ADD         R1, R1, R2
           STR         R1, R4, #2
; a + b
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           STR         R2, R1, #0
           STR         R2, R4, #1
; Store
           LDR         R0, R4, #1
           LDR         R7, R6, #-3
           JSRR        R7
           LEA         R1, #1
           BR          LBL11
           .STRINGZ    "\n"      ; string literal
LBL11           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           LDR         R2, R1, #0
           ADD         R2, R2, #1
           STR         R2, R4, #1
; ++a
           STR         R2, R1, #0 ; ++lexpr
           BR          LBL5
LBL4
           AND         R0, R0, #0
Qmain      TRAP        x25

CSCANI         ST          R7, CIOR7
               ST          R1, CIOR1
               ST          R2, CIOR2
               ST          R3, CIOR3
               LD          R1, CSCANBEGINPTR
               LD          R7, CSCANENDPTR
               NOT         R7, R7
               ADD         R7, R7, #1
               ADD         R7, R7, R1
               BRnp        CSCANIGETI
               JSR         CSCAN
               LD          R1, CSCANBEGINPTR
CSCANIGETI     AND         R0, R0, #0
               LD          R2, CLRANGE
CGETILOOP      LDR         R7, R1, #0
               ADD         R7, R7, R2
               BRn         CGETIFINISH
               ADD         R3, R7, #-9
               BRp         CGETIFINISH
               ADD         R7, R7, R0
               ADD         R7, R7, R0
               ADD         R0, R0, R0
               ADD         R0, R0, R0
               ADD         R0, R0, R0
               ADD         R0, R0, R7    ;   x10
               ADD         R1, R1, #1
               BR          CGETILOOP
CGETIFINISH    ST          R1, CSCANBEGINPTR
               LD          R7, CIOR7
               LD          R1, CIOR1
               LD          R2, CIOR2
               LD          R3, CIOR3
               RET
CLRANGE        .FILL       #-48
CSCANC         ST          R7, CIOR7
               ST          R1, CIOR1
               LD          R1, CSCANBEGINPTR
               LD          R7, CSCANENDPTR
               NOT         R7, R7
               ADD         R7, R7, #1
               ADD         R7, R7, R1
               BRnp        CSCANCGETC
               JSR         CSCAN
               LD          R1, CSCANBEGINPTR
CSCANCGETC     LDR         R0, R1, #0
               ADD         R1, R1, #1
               ST          R1, CSCANBEGINPTR
               LD          R7, CIOR7
               LD          R1, CIOR1
               RET
CIOR7          .BLKW       1
CIOR1          .BLKW       1
CIOR2          .BLKW       1
CIOR3          .BLKW       1
CSCANPTR       .FILL       CSCANBUF
CSCANBEGINPTR  .FILL       CSCANBUF
CSCANENDPTR    .FILL       CSCANBUF
CSCAN          ST          R7, CSCANR7
               ST          R1, CSCANR1
               LD          R1, CSCANPTR
CWAIT          LDI         R0, CCANSCAN   ; Wait until input
               BRzp        CWAIT
               LDI         R0, CSRCPTR
               ADD         R7, R0, #-10
               BRz         CSCANGET
               TRAP        x21
               STR         R0, R1, #0
               ADD         R1, R1, #1
               BR          CWAIT
CSCANGET       AND         R7, R7, #0
               STR         R7, R1, #0
               ST          R1, CSCANENDPTR
               LD          R1, CSCANPTR
               ST          R1, CSCANBEGINPTR
               AND         R0, R0, #0
               ADD         R0, R0, #10
               TRAP        x21
               LD          R7, CSCANR7
               LD          R1, CSCANR1
               RET
CCANSCAN       .FILL       xFE00
CSRCPTR        .FILL       xFE02
CSCANR7        .BLKW       1
CSCANR1        .BLKW       1
CSCANBUF       .BLKW       x301
CPRINTI        ST          R7, CPRR7
               ST          R1, CPRR1
               ST          R2, CPRR2
               AND         R2, R2, #0
               ADD         R1, R0, #0
               BRp         CPPRINT4
               BRn         CPPRINTREV
               LD          R0, CIBASCII
               TRAP        x21
               BR          CPPRINTFINISH
CPPRINTREV     NOT         R1, R1
               LD          R0, CIBSGN
               TRAP        x21
               ADD         R1, R1, #1
CPPRINT4       AND         R0, R0, #0
               LD          R7, CIBIT4
CPPRINT4N      ADD         R1, R7, R1
               BRn         CPPRINT3
               ADD         R0, R0, #1
               BR          CPPRINT4N
CPPRINT3       LD          R7, CIRBIT4
               ADD         R1, R1, R7
               ADD         R2, R0, #0
               BRnz        CPPRINT3Y
               LD          R7, CIBASCII
               ADD         R0, R0, R7
               TRAP        x21
CPPRINT3Y      LD          R7, CIBIT3
               AND         R0, R0, #0
CPPRINT3N      ADD         R1, R7, R1
               BRn         CPPRINT2
               ADD         R0, R0, #1
               BR          CPPRINT3N
CPPRINT2       LD          R7, CIRBIT3
               ADD         R1, R1, R7
               ADD         R2, R0, #0
               BRnz        CPPRINT2Y
               LD          R7, CIBASCII
               ADD         R0, R0, R7
               TRAP        x21
CPPRINT2Y      LD          R7, CIBIT2
               AND         R0, R0, #0
CPPRINT2N      ADD         R1, R7, R1
               BRn         CPPRINT1
               ADD         R0, R0, #1
               BR          CPPRINT2N
CPPRINT1       LD          R7, CIRBIT2
               ADD         R1, R1, R7
               ADD         R2, R0, #0
               BRnz        CPPRINT1Y
               LD          R7, CIBASCII
               ADD         R0, R0, R7
               TRAP        x21
CPPRINT1Y      LD          R7, CIBIT1
               AND         R0, R0, #0
CPPRINT1N      ADD         R1, R7, R1
               BRn         CPPRINT0
               ADD         R0, R0, #1
               BR          CPPRINT1N
CPPRINT0       ADD         R1, R1, #10
               ADD         R2, R0, #0
               BRnz        CPP0
               LD          R7, CIBASCII
               ADD         R0, R0, R7
               TRAP        x21
CPP0           LD          R7, CIBASCII
               ADD         R0, R1, R7
               TRAP        x21
CPPRINTFINISH  LD          R7, CPRR7
               LD          R1, CPRR1
               LD          R2, CPRR2
               RET
CPRR7          .BLKW       #1
CPRR1          .BLKW       #1
CPRR2          .BLKW       #1
CIBASCII       .FILL       #48
CIBSGN         .FILL       #45
CIBIT4         .FILL       #-10000
CIBIT3         .FILL       #-1000
CIBIT2         .FILL       #-100
CIBIT1         .FILL       #-10
CIRBIT4        .FILL       #10000
CIRBIT3        .FILL       #1000
CIRBIT2        .FILL       #100
CIRBIT1        .FILL       #10
FUNCTION
           .FILL       Fnmain
           .FILL       CPRINTI
           .FILL       CSCANI
           .FILL       CSCANC
STATIC     .BLKW       x20
           .BLKW       x30
STACK      .BLKW       #1
           .END