/**
  based upon some unburdened c code from mpeg2dec (idct_mmx.c
  written by Aaron Holtzman <aholtzma@ess.engr.uvic.ca>) 
*/

#include    "netpipe.h"

/** 32 or 64 bit detected and provided by configure. */
#ifdef 	ARCH_IS_64BIT
#define	FAST_64BIT
#else
#undef	FAST_64BIT
#endif

/* candidate for header file. */
#define W1  22725  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W2  21407  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W3  19266  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W4  16383  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W5  12873  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W6  8867   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W7  4520   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define ROW_SHIFT 11
#define COL_SHIFT 20 /* 6*/

/** @warning broken gcc 4.1.1, defined(ARCH_IS_PPC) removed.*/
#if 0 

/* signed 16x16 -> 32 multiply add accumulate */
#define MAC16(rt, ra, rb) \
    asm ("maclhw %0, %2, %3" : "=r" (rt) : "0" (rt), "r" (ra), "r" (rb));

/* signed 16x16 -> 32 multiply */
#define MUL16(rt, ra, rb) \
    asm ("mullhw %0, %1, %2" : "=r" (rt) : "r" (ra), "r" (rb));

#else

/* signed 16x16 -> 32 multiply add accumulate */
#define MAC16(rt, ra, rb) rt += (ra) * (rb)

/* signed 16x16 -> 32 multiply */
#define MUL16(rt, ra, rb) rt = (ra) * (rb)

#endif

/* candidate for header file. */
/* idct utility section */
void idctRowCondDC (int16_t * row)
{
	int64_t a0, a1, a2, a3, b0, b1, b2, b3;
#ifdef FAST_64BIT
        uint64_t temp;
#else
        uint32_t temp;
#endif

#ifdef FAST_64BIT
#ifdef WORDS_ENDIAN
#define ROW0_MASK 0xffff000000000000
#else
#define ROW0_MASK 0x000000000000ffff
#endif
	if ( ((((uint64_t *)row)[0] & ~ROW0_MASK) | 
              ((uint64_t *)row)[1]) == 0) {
            temp = (row[0] << 3) & 0xffff;
            temp += temp << 16;
            temp += temp << 32;
            ((uint64_t *)row)[0] = temp;
            ((uint64_t *)row)[1] = temp;
            return;
	}
#else
	if (!(((uint32_t*)row)[1] |
              ((uint32_t*)row)[2] |
              ((uint32_t*)row)[3] | 
              row[1])) {
            temp = (row[0] << 3) & 0xffff;
            temp += temp << 16;
            ((uint32_t*)row)[0]=((uint32_t*)row)[1] =
		((uint32_t*)row)[2]=((uint32_t*)row)[3] = temp;
		return;
	}
#endif

        a0 = (W4 * row[0]) + (1 << (ROW_SHIFT - 1));
	a1 = a0;
	a2 = a0;
	a3 = a0;

        /* no need to optimize : gcc does it */
        a0 += W2 * row[2];
        a1 += W6 * row[2];
        a2 -= W6 * row[2];
        a3 -= W2 * row[2];

        MUL16(b0, W1, row[1]);
        MAC16(b0, W3, row[3]);
        MUL16(b1, W3, row[1]);
        MAC16(b1, -W7, row[3]);
        MUL16(b2, W5, row[1]);
        MAC16(b2, -W1, row[3]);
        MUL16(b3, W7, row[1]);
        MAC16(b3, -W5, row[3]);

#ifdef FAST_64BIT
        temp = ((uint64_t*)row)[1];
#else
        temp = ((uint32_t*)row)[2] | ((uint32_t*)row)[3];
#endif
	if (temp != 0) {
            a0 += W4*row[4] + W6*row[6];
            a1 += - W4*row[4] - W2*row[6];
            a2 += - W4*row[4] + W2*row[6];
            a3 += W4*row[4] - W6*row[6];

            MAC16(b0, W5, row[5]);
            MAC16(b0, W7, row[7]);
            
            MAC16(b1, -W1, row[5]);
            MAC16(b1, -W5, row[7]);
            
            MAC16(b2, W7, row[5]);
            MAC16(b2, W3, row[7]);
            
            MAC16(b3, W3, row[5]);
            MAC16(b3, -W1, row[7]);
	}

	row[0] = (a0 + b0) >> ROW_SHIFT;
	row[7] = (a0 - b0) >> ROW_SHIFT;
	row[1] = (a1 + b1) >> ROW_SHIFT;
	row[6] = (a1 - b1) >> ROW_SHIFT;
	row[2] = (a2 + b2) >> ROW_SHIFT;
	row[5] = (a2 - b2) >> ROW_SHIFT;
	row[3] = (a3 + b3) >> ROW_SHIFT;
	row[4] = (a3 - b3) >> ROW_SHIFT;
}


void idctSparseCol (int16_t * const col)
{
        int64_t a0, a1, a2, a3, b0, b1, b2, b3;

        /* XXX: I did that only to give same values as previous code */
        a0 = W4 * (col[8*0] + ((1<<(COL_SHIFT-1))/W4));
        a1 = a0;
        a2 = a0;
        a3 = a0;

        a0 +=  + W2*col[8*2];
        a1 +=  + W6*col[8*2];
        a2 +=  - W6*col[8*2];
        a3 +=  - W2*col[8*2];

        MUL16(b0, W1, col[8*1]);
        MUL16(b1, W3, col[8*1]);
        MUL16(b2, W5, col[8*1]);
        MUL16(b3, W7, col[8*1]);

        MAC16(b0, + W3, col[8*3]);
        MAC16(b1, - W7, col[8*3]);
        MAC16(b2, - W1, col[8*3]);
        MAC16(b3, - W5, col[8*3]);

        if(col[8*4]){
            a0 += + W4*col[8*4];
            a1 += - W4*col[8*4];
            a2 += - W4*col[8*4];
            a3 += + W4*col[8*4];
        }

        if (col[8*5]) {
            MAC16(b0, + W5, col[8*5]);
            MAC16(b1, - W1, col[8*5]);
            MAC16(b2, + W7, col[8*5]);
            MAC16(b3, + W3, col[8*5]);
        }

        if(col[8*6]){
            a0 += + W6*col[8*6];
            a1 += - W2*col[8*6];
            a2 += + W2*col[8*6];
            a3 += - W6*col[8*6];
        }

        if (col[8*7]) {
            MAC16(b0, + W7, col[8*7]);
            MAC16(b1, - W5, col[8*7]);
            MAC16(b2, + W3, col[8*7]);
            MAC16(b3, - W1, col[8*7]);
        }

        col[0 ] = ((a0 + b0) >> COL_SHIFT);
        col[8 ] = ((a1 + b1) >> COL_SHIFT);
        col[16] = ((a2 + b2) >> COL_SHIFT);
        col[24] = ((a3 + b3) >> COL_SHIFT);
        col[32] = ((a3 - b3) >> COL_SHIFT);
        col[40] = ((a2 - b2) >> COL_SHIFT);
        col[48] = ((a1 - b1) >> COL_SHIFT);
        col[56] = ((a0 - b0) >> COL_SHIFT);
}

void simple_idct_c(int16_t * block)
{
    int i;
    for(i=0; i<8; i++)
        idctRowCondDC(block + i*8);
/* Forcing the mask operations on all data. */
#if 1    
    for(i=0; i<8; i++)
        idctSparseCol(block + i);
#endif
}
/* idct utility section */

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
  /* Print out message about results */

  printf("\n");
  printf("  *** Note about idct module results ***  \n");
  printf("\n");
  printf("The idct module implements a simple idct.\n" \
         "The result is currently stored into a volatile, if the compiler\n" \
         "is too smart, you may need to make a global value, referenced in\n" \
         "another location outside the timing loop.\n" \
         "\n" \
         );
  printf("\n");
  p->tr = 1;
  p->rcv = 0;
}

void Setup(ArgStruct *p)
{
}   

void Sync(ArgStruct *p)
{
}

void PrepareToReceive(ArgStruct *p)
{
}
/** 
Attempt to avoid strong optimization by not letting the compiler
know that the result is unused elsewhere.
*/
    volatile int nleft;
void SendData(ArgStruct *p)
{
    int nbytes = p->bufflen;
    char *src = p->s_ptr, *dest = p->r_ptr;

	for(;src<p->s_ptr+p->bufflen;src+=sizeof(int16_t)*64)
	{
	  simple_idct_c((int16_t *)src);
	}
#if 0
    if (nleft != p->bufflen-1) 
    {
      printf("\nMismatch nleft: %d, bufflen: %d\n",
             nleft,p->bufflen);
    }
#endif
}

void RecvData(ArgStruct *p)
{
    int nbytes = p->bufflen;
    char *src = p->s_ptr, *dest = p->r_ptr;

	for(;dest<p->r_ptr+p->bufflen;dest+=sizeof(int16_t)*64)
	{
	  simple_idct_c((int16_t *)dest);
	}

}

void SendTime(ArgStruct *p, double *t)
{
}

void RecvTime(ArgStruct *p, double *t)
{
}

void SendRepeat(ArgStruct *p, int rpt)
{
}

void RecvRepeat(ArgStruct *p, int *rpt)
{
}

void CleanUp(ArgStruct *p)
{
}



void Reset(ArgStruct *p)
{

}


void AfterAlignmentInit(ArgStruct *p)
{

}

