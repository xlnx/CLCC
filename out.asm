           .ORIG       x3000
           LD          R4, #3
           LD          R5, #3
           LD          R6, #3
           BR          Fnmain
           .FILL       STACK
           .FILL       FUNCTION
           .FILL       STATIC
Fnmain     
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           TRAP        x23
           STR         R0, R1, #0
           ADD         R1, R4, #-3
           STR         R1, R4, #1
; Lvalue
           LDR         R1, R4, #1
           TRAP        x23
           STR         R0, R1, #0
           ADD         R1, R4, #-2
           STR         R1, R4, #1
; Lvalue
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
           LDR         R2, R4, #2
           LDR         R1, R4, #1
           ADD         R1, R1, R2
           STR         R1, R4, #1
; a + b
           LDR         R0, R4, #1
           TRAP        x21
           AND         R0, R0, #0
Qmain      TRAP        x25

FUNCTION               
           .FILL       Fnmain
           .BLKW       x20
STATIC     .BLKW       x20
           .BLKW       x30
STACK      .BLKW       #1
           .END
