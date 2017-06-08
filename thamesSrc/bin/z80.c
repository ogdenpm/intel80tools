/* Emulation of the Z80 CPU with hooks into the other parts of xz80.
 * Copyright (C) 1994 Ian Collier.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include<stdio.h>
#include "thames.h"

#ifdef _WIN32
#pragma warning(disable: 4244)		// suppress conversion warnings
#endif

#define parity(a) (partable[a])

#ifdef Z80
#define _S	0
#define _Z  0
#else
#define _S	0x80
#define _Z	0x40
#endif
unsigned char partable[256]={
      4|_Z, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
      0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
      0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
      4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
      0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
      4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
      4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
      0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,

      0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S,
      4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S,
      4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S,
      0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S,
      4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S,
      0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S,
      0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S,
      4|_S, 0|_S, 0|_S, 4|_S, 0|_S, 4|_S, 4|_S, 0|_S, 0|_S, 4|_S, 4|_S, 0|_S, 4|_S, 0|_S, 0|_S, 4|_S
   };

#ifdef DEBUG
static unsigned short breakpoint=0;
static unsigned int breaks=0;

static void inline do_log(fp,name,val)
FILE *fp;
char *name;
unsigned short val;
{
   int i;
   fprintf(fp,"%s=%04X ",name,val);
   for(i=0;i<8;i++,val++)fprintf(fp," %02X",fetch(val));
   putc('\n',fp);
}
#endif

FILE *fptrace = NULL;

void mainloop(word spc, word ssp) {
	register unsigned char a, f, b, c, d, e, h, l;
	unsigned char r, a1, f1, b1, c1, d1, e1, h1, l1, i, iff1, iff2, im;
	register unsigned short pc;
	unsigned short sp;

	unsigned short ix, iy;
#ifdef Z80
	//   register unsigned long tstates;
	register unsigned int radjust;
	register unsigned char ixoriy, new_ixoriy;
#else
	const unsigned char ixoriy = 0;		/* let compiler optimize code*/
#endif
   unsigned char intsample;
   register unsigned char op;
#ifdef DEBUG
   char flags[9];
   int bit;
   register unsigned short af2=0,bc2=0,de2=0,hl2=0,ix2=0,iy2=0,sp2=0;
   register unsigned char i2=0;
   struct _next {unsigned char bytes[8];} *next;
   unsigned short BC, DE, HL, AF;

#endif
   a=f=b=c=d=e=h=l=a1=f1=b1=c1=d1=e1=h1=l1=i=r=iff1=iff2=im=0;
#ifdef Z80
   ixoriy=new_ixoriy=0;
   ix=iy=0;
#endif
   pc=spc;
   sp=ssp;
#ifdef Z80
   /*tstates=*/radjust=0;
#endif
   while(1){
#ifdef Z80
      ixoriy=new_ixoriy;
      new_ixoriy=0;
#endif
#ifdef DEBUG
      next=(struct _next *)&fetch(pc);
      BC=bc;DE=de;HL=hl;AF=(a<<8)|f;
      if(fptrace && !ixoriy){
         do_log(fptrace,"pc",pc);
         if(sp!=sp2)do_log(fptrace,"sp",sp2=sp);
#ifdef Z80
		 if(iy!=iy2)do_log(fptrace,"iy",iy2=iy);
         if(ix!=ix2)do_log(fptrace,"ix",ix2=ix);
#endif
         if(hl!=hl2)do_log(fptrace,"hl",hl2=hl);
         if(de!=de2)do_log(fptrace,"de",de2=de);
         if(bc!=bc2)do_log(fptrace,"bc",bc2=bc);
         if(((a<<8)|f)!=af2){
            af2=(a<<8)|f;
            strcpy(flags,"SZ H VNC");
            for(bit=0;bit<8;bit++)if(!(f&(1<<(7-bit))))flags[bit]=' ';
            fprintf(fptrace,"af=%04X  %s\n",af2,flags);
         }
         if(i!=i2)fprintf(fptrace,"ir=%02X%02X\n",i2=i,r);
         putc('\n',fptrace);
      }
      if(pc==breakpoint && pc)
         breaks++; /* some code at which to set a breakpoint */
      a=AF>>8; f=AF; h=HL>>8; l=HL; d=DE>>8; e=DE; b=BC>>8; c=BC;
#endif
/*

*/
      intsample=1;
      op=fetch(pc);
      pc++;
#ifdef Z80
	  radjust++;
#endif
	  switch(op){
#include "z80ops.h"
      }
   }
}
