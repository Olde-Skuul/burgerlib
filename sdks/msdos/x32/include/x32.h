/*
 Copyright FlashTek, Inc. 1992 1993
 121 Sweet Ave.
 Moscow, Idaho  83843

 Joe Huffman 
 October 12, 1992
 FAX:   208-882-7275
 email: joe@proto.com

 Include file for the X-32VM DOS extender functions and global variables.
*/

#ifndef _X32_H_INCLUDED
#define _X32_H_INCLUDED

#if __cplusplus
extern "C" {


#endif

extern int _cdecl _x32_mouse_init(void (* _cdecl func)(int mask,int state,
                                  int x,int y),int mask);
extern int _cdecl _x386_mouse_init(void (* _cdecl func)(int mask,int state,
                                  int x,int y),int mask);
extern int _cdecl _x32_mouse_term();
extern int _cdecl _x386_mouse_term();

#ifdef __WATCOMC__
unsigned int _x32_coreleft(void);
unsigned int _x386_coreleft(void);
unsigned int  _x32_allcoreleft(void);
unsigned int _x386_allcoreleft(void);
#else
unsigned int _cdecl _x32_coreleft(void);
unsigned int _cdecl _x386_coreleft(void);
unsigned int _cdecl _x32_allcoreleft(void);
unsigned int _cdecl _x386_allcoreleft(void);
#endif

unsigned int _cdecl _x32_get_abs_address(void _far *p);
int _cdecl _x32_free_protected_ptr(void _far *p);
void *_cdecl _x32_map_physical_address(void *abs_addr, unsigned int size);
void *_cdecl _x386_map_physical_address(void *abs_addr, unsigned int size);
int _cdecl _x32_memlock(void _far *p, unsigned int size);
int _cdecl _x386_memlock(void _far *p, unsigned int size);
int _cdecl _x32_memunlock(void _far *p, unsigned int length);
int _cdecl _x386_memunlock(void _far *p, unsigned int length);
void _far *_cdecl _x32_mk_protected_ptr(void *p);

void _cdecl _x32_click(void); /* Clicks the speaker, no registers affected. */
void _cdecl _x32_reg_dump(void);/* Does a register dump and aborts. */

extern unsigned short _x32_zero_base_selector;
extern unsigned short _x386_zero_base_selector;
extern void * _x32_zero_base_ptr;
extern void * _x386_zero_base_ptr;

extern void _cdecl _x32_mmfio_flush(void *ptr);
extern void *_cdecl _x32_mmfio_open(int fd,int size);

unsigned int _cdecl _x32_real_alloc(unsigned int size);
void _cdecl _x32_real_free(unsigned int rptr);
unsigned short _cdecl _x32_real_offset(unsigned int real_ptr);
void * _cdecl _x32_real_to_protected(unsigned int real_ptr);
unsigned int _cdecl _x32_dosmemleft();
unsigned int _cdecl _x32_real_coreleft();
unsigned short _cdecl _x32_real_segment(unsigned int real_ptr);

struct FAULT_STRUC
{
  unsigned eax,ebx,ecx,edx,esi,edi,ebp;
  unsigned short ds,es,fs,gs,
  fault_num,                    /* fault number                              */
  mode;                         /* 1 indicates real mode, 0 protected mode   */
  unsigned eip;
  unsigned short cs,space;      /* space maintains dword alignment           */
  unsigned eflags,esp;
  unsigned short ss;
};

extern void _cdecl _x32_fault_intercept(void (* _cdecl func) ( struct FAULT_STRUC *fault));

#if __WATCOMC__
#pragma aux _x32_zero_base_ptr "_*";
#pragma aux _x386_zero_base_ptr "_*";
#pragma aux _x32_zero_base_selector "_*";
#pragma aux _x386_zero_base_selector "_*";

extern int _cdecl controlc_open(void);
extern int _cdecl controlc_close(void);
extern void (*_controlc_handler)(void);
#pragma aux _controlc_handler "_*";
#endif


#if __cplusplus
}
#endif

#endif

