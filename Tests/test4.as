macr validMacro
cmp r3, #-6
endmacr

; invalid macro ending
macr someMacro
LIST: .data 7898,0,-1
    .data -100
endmacr          hello:)
; ---------------------------------

; extra text in macro initialization
macr macrname  *
cmp r3, #-6
bne END
endmacr
; ---------------------------------

; reserved word 'macr'
macr macr
cmp r3, #-6
endmacr

; ---------------------------------

; invalid macr name - reserved word
macr mov
LIST: .data 7898,0,-1
    .data -100
endmacr
; ---------------------------------

; invalid macro name - existing macro
macr validMacro
LIST: .data 7898,0,-1
    .data -100
endmacr
; ---------------------------------

; Line is too long but it will be checked in first pass, but first pass will not be reached so the error will not be reported.
A: .string "ab "  ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddcd"
; ---------------------------------








