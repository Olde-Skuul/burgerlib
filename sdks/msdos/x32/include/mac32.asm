; Copyright Doug Huffman
; Route 1 Box 54A
; Lenore, ID 83541

; January 29,1992

comment &
        This file contains macros which are useful in writing 32 bit assmbly 
        language programs.  It also contains the .386 directive which causes 
        all segments to default to 32 bits.
&

.386

;Macro for start of a real mode code segment
start16code     macro
__X386_CODESEG_16       segment para use16 public 'CODE'

if ??version         ;If using TASM.
  assume cs:__X386_CODESEG_16,ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing
else
  if @Version GE 600
    assume ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing
  else
    assume cs:__X386_CODESEG_16,ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing
  endif
endif
endm

;Macro for end of real mode code segment
end16code       macro
__X386_CODESEG_16       ends
endm

;Macro for start of a real mode data segment
start16data     macro
__X386_DATASEG_16       segment para use16 public 'DATA16'
endm

;Macro for end of real mode data segment
end16data       macro
__X386_DATASEG_16       ends
endm


;Macro for start of protected mode code segment
start32code     macro
_TEXT   segment dword   public  'CODE'

if ??version            ;If using TASM
  assume cs:_TEXT,ds:DGROUP,es:nothing,fs:nothing,gs:nothing,ss:nothing
else
  if __WATCOMC__
    assume cs:_TEXT,ds:DGROUP,es:nothing,fs:nothing,gs:nothing,ss:nothing
  else
    if @Version GE 600
      assume ds:DGROUP,es:nothing,fs:nothing,gs:nothing,ss:nothing
    else
      assume cs:_TEXT,ds:DGROUP,es:nothing,fs:nothing,gs:nothing,ss:nothing
    endif
  endif
endif
endm

;Macro for end of protected mode code segment
end32code macro
_TEXT   ends
endm

;Macro for start of protected mode data segment
start32data     macro
_DATA   segment dword   public  'DATA'
endm

end32data       macro
_DATA   ends
endm

__X386_DATASEG_16       segment para use16 public 'DATA16'
__X386_DATASEG_16       ends

_TEXT   segment dword   public  'CODE'
_TEXT   ends

_DATA   segment dword   public  'DATA'
_DATA   ends

DGROUP  group   _DATA,_TEXT

__X386_GROUP_16  group  __X386_DATASEG_16

