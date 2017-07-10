           .ORIG       x3000
           LD          R4, #3
           LD          R5, #3
           LD          R6, #3
           BR          Fnmain
           .FILL       STACK
           .FILL       FUNCTION
           .FILL       STATIC
Fnmain     
           LD          R1, #1
           BR          LBL7
           .FILL       #-4096      ; 16-bit literal
LBL7           STR         R1, R4, #1
           LDR         R1, R4, #1
           STR         R1, R4, #-2
           LEA         R1, #1
           BR          LBL8
           .STRINGZ    "Type a name and press Enter: "      ; string literal
LBL8           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R0, R4, #1
           LDR         R7, R6, #-4
           JSRR        R7
           LD          R1, #1
           BR          LBL9
           .FILL       #0      ; 16-bit literal
LBL9           STR         R1, R4, #1
           LDR         R1, R4, #1
           STR         R1, R4, #-3
           LD          R1, #1
           BR          LBL12
           .FILL       #16384      ; 16-bit literal
LBL12           STR         R1, R4, #1
           LDR         R1, R4, #1
           STR         R1, R4, #-4
           AND         R0, R0, #0
LBL11
           ADD         R1, R4, #-4
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL13
           .FILL       #0      ; 16-bit literal
LBL13           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           NOT         R2, R2
           ADD         R2, R2, #1
           AND         R0, R0, #0
           ADD         R1, R1, R2
           BRz         LBL14
           ADD         R0, R0, #1
LBL14           STR         R0, R4, #1
; a != b
           LDR         R0, R4, #1
           BRz         LBL10
           ADD         R1, R4, #-4
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL15
           .FILL       #1      ; 16-bit literal
LBL15           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2
           STR         R1, R4, #1
; a[i]
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R1, R4, #1
           STR         R1, R4, #-5
           ADD         R1, R4, #-4
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL16
           .FILL       #3      ; 16-bit literal
LBL16           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2
           STR         R1, R4, #1
; a[i]
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R1, R4, #1
           STR         R1, R4, #-6
           ADD         R1, R4, #-4
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL17
           .FILL       #2      ; 16-bit literal
LBL17           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2
           STR         R1, R4, #1
; a[i]
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R1, R4, #1
           STR         R1, R4, #-7
           LDR         R1, R5, #0
           STR         R1, R4, #1
; Function
           ADD         R1, R4, #-2
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           ADD         R1, R4, #-6
           STR         R1, R4, #3
; Lvalue
           LDR         R7, R4, #3
           LDR         R7, R7, #0
           STR         R7, R4, #3
; To Rvalue
           LDR         R1, R4, #3
; Param1
           STR         R1, R4, #3
           LDR         R1, R4, #2
; Param0
           STR         R1, R4, #2
           AND         R1, R1, #0
           STR         R1, R4, #5
           LDR         R7, R4, #1
           AND         R1, R1, #0
           ADD         R1, R1, #-1
           STR         R1, R4, #1
           ADD         R4, R4, #1
           JSRR        R7      ; call
           STR         R0, R4, #1
; ret value
           AND         R7, R7, #0
           LDR         R1, R4, #1
           BRnp        LBL18
           ADD         R7, R7, #1
LBL18
           STR         R7, R4, #1
; !a
           LDR         R0, R4, #1
           BRnp        LBL19
           LDR         R1, R5, #0
           STR         R1, R4, #1
; Function
           ADD         R1, R4, #-2
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           ADD         R1, R4, #-7
           STR         R1, R4, #3
; Lvalue
           LDR         R7, R4, #3
           LDR         R7, R7, #0
           STR         R7, R4, #3
; To Rvalue
           LDR         R1, R4, #3
; Param1
           STR         R1, R4, #3
           LDR         R1, R4, #2
; Param0
           STR         R1, R4, #2
           AND         R1, R1, #0
           STR         R1, R4, #5
           LDR         R7, R4, #1
           AND         R1, R1, #0
           ADD         R1, R1, #-1
           STR         R1, R4, #1
           ADD         R4, R4, #1
           JSRR        R7      ; call
           STR         R0, R4, #1
; ret value
           AND         R7, R7, #0
           LDR         R1, R4, #1
           BRnp        LBL21
           ADD         R7, R7, #1
LBL21
           STR         R7, R4, #1
; !a
           LDR         R0, R4, #1
           BRnp        LBL19
           AND         R1, R1, #0
           STR         R1, R4, #1
; a || b
           BR          LBL20
LBL19
           AND         R1, R1, #0
           ADD         R1, R1, #1
           STR         R1, R4, #1
; a || b
LBL20
           LDR         R0, R4, #1
           BRz         LBL22
           ADD         R1, R4, #-6
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R0, R4, #1
           TRAP        x22
           LEA         R1, #1
           BR          LBL23
           .STRINGZ    " "      ; string literal
LBL23           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
           ADD         R1, R4, #-7
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R0, R4, #1
           TRAP        x22
           LEA         R1, #1
           BR          LBL24
           .STRINGZ    " "      ; string literal
LBL24           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
           ADD         R1, R4, #-5
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R0, R4, #1
           TRAP        x22
           LEA         R1, #1
           BR          LBL25
           .STRINGZ    "\n"      ; string literal
LBL25           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
           ADD         R1, R4, #-3
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           LDR         R2, R1, #0
           STR         R2, R4, #1
; a++
           ADD         R2, R2, #1
           STR         R2, R1, #0 ; lexpr++
LBL22
           ADD         R1, R4, #-4
           STR         R1, R4, #1
; Lvalue
           ADD         R1, R4, #-4
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           STR         R2, R1, #0
           STR         R2, R4, #1
; Store
           BR          LBL11
LBL10
           ADD         R1, R4, #-3
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           AND         R7, R7, #0
           LDR         R1, R4, #1
           BRnp        LBL26
           ADD         R7, R7, #1
LBL26
           STR         R7, R4, #1
; !a
           LDR         R0, R4, #1
           BRz         LBL27
           LEA         R1, #1
           BR          LBL28
           .STRINGZ    "No Entry\n"      ; string literal
LBL28           STR         R1, R4, #1
           LDR         R0, R4, #1
           TRAP        x22
LBL27
           AND         R0, R0, #0
Qmain      TRAP        x25



Fnstrcmp
           LDR         R1, R4, #0
           ADD         R1, R4, R1
           STR         R1, R4, #4
           STR         R7, R4, #3
           ADD         R4, R4, #4
           AND         R0, R0, #0
LBL1
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LD          R1, #1
           BR          LBL2
           .FILL       #0      ; 16-bit literal
LBL2           STR         R1, R4, #2
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           NOT         R2, R2
           ADD         R2, R2, #1
           AND         R0, R0, #0
           ADD         R1, R1, R2
           BRz         LBL3
           ADD         R0, R0, #1
LBL3           STR         R0, R4, #1
; a != b
           LDR         R0, R4, #1
           BRz         LBL4
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           ADD         R1, R4, #-3
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           NOT         R2, R2
           ADD         R2, R2, #1
           AND         R0, R0, #0
           ADD         R1, R1, R2
           BRnp        LBL6
           ADD         R0, R0, #1
LBL6           STR         R0, R4, #1
; a == b
           LDR         R0, R4, #1
           BRz         LBL4
           AND         R1, R1, #0
           ADD         R1, R1, #1
           STR         R1, R4, #1
; a && b
           BR          LBL5
LBL4
           AND         R1, R1, #0
           STR         R1, R4, #1
; a && b
LBL5
           LDR         R0, R4, #1
           BRz         LBL0
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           LDR         R2, R1, #0
           ADD         R2, R2, #1
           STR         R2, R4, #1
; ++a
           STR         R2, R1, #0 ; ++lexpr
           ADD         R1, R4, #-3
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           LDR         R2, R1, #0
           ADD         R2, R2, #1
           STR         R2, R4, #1
; ++a
           STR         R2, R1, #0 ; ++lexpr
           BR          LBL1
LBL0
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           LDR         R7, R4, #1
           LDR         R7, R7, #0
           STR         R7, R4, #1
; To Rvalue
           ADD         R1, R4, #-3
           STR         R1, R4, #2
; Lvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R7, R4, #2
           LDR         R7, R7, #0
           STR         R7, R4, #2
; To Rvalue
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           NOT         R2, R2
           ADD         R2, R2, #1
           ADD         R1, R1, R2
           STR         R1, R4, #1
; a - b
           LDR         R0, R4, #1
           BR          Qstrcmp
           AND         R0, R0, #0
Qstrcmp         LDR         R7, R4, #-1
           LDR         R4, R4, #0
           RET

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
CSCANS         ST          R1, CSCANSR1
               ST          R7, CSCANSR7
               LD          R1, CSCANBEGINPTR
               LD          R7, CSCANENDPTR
               NOT         R7, R7
               ADD         R7, R7, #1
               ADD         R7, R7, R1
               BRnp        CSCANSGETS
               ST          R0, CSCANSR0
               JSR         CSCAN
               LD          R0, CSCANSR0
               LD          R1, CSCANBEGINPTR
CSCANSGETS     LDR         R7, R1, #0
               STR         R7, R0, #0
               ADD         R0, R0, #1
               ADD         R1, R1, #1
               ADD         R7, R7, #0
               BRnp        CSCANSGETS
               ADD         R1, R1, #-1
               ST          R1, CSCANBEGINPTR
               LD          R1, CSCANSR1
               LD          R7, CSCANSR7
               RET
CSCANSR7        .BLKW       1
CSCANSR1        .BLKW       1
CSCANSR0        .BLKW       1
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
           .FILL       Fnstrcmp
           .FILL       Fnmain
           .FILL       CSCANS
           .FILL       CPRINTI
           .FILL       CSCANI
           .FILL       CSCANC
STATIC     .BLKW       #32
           .BLKW       x30
STACK      .BLKW       #1
           .END
