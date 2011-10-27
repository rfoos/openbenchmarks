#include    "netpipe.h"
#ifndef NP02
#include    <limits.h>
#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif
#endif

#define W1  22725LL  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W2  21407LL  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W3  19266LL  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W4  16383LL  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W5  12873LL  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W6  8867LL   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W7  4520LL   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/

#ifndef ROW_SHIFT
#define ROW_SHIFT 22
#endif

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
  /* Print out message about results */

  printf("\n");
  printf("  *** Note about mul64b module results ***  \n");
  printf("\n");
  printf("The mul64b module is sensitive to the L1 and L2 cache sizes,\n" \
         "the size of the cache-lines, and the compiler.  The following\n" \
         "may help to interpret the results:\n" \
         "\n" \
         "* With cache effects and no perturbations (npmul64b -p 0),\n" \
         "    the plot will show 2 peaks.  The first peak is where data is\n" \
         "    copied from L1 cache to L1, peaking around half the L1 cache\n" \
         "    size.  The second peak is where data is copied from the L2 cache\n" \
         "    to L2, peaking around half the L2 cache size.  The curve then\n" \
         "    will drop off as messages are copied from RAM through the caches\n" \
         "    and back to RAM.\n" \
         "\n" \
         "* Without cache effects and no perturbations (npmul64b -I -p 0).\n" \
         "    Data always starts in RAM, and is copied through the caches\n" \
         "    up in L1, L2, or RAM depending on the message size.\n"\
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
#ifdef	NP02
void SendData(ArgStruct *p)
{
    int nint64s = p->bufflen / sizeof(uint64_t) - 1;
    uint64_t *src = (uint64_t *)p->s_ptr, *dest = (uint64_t *)p->r_ptr;

    while (nint64s--) {
        dest[0] = src[0] * W4;
        src++;dest++;
    }

}

void RecvData(ArgStruct *p)
{
    int nint64s = p->bufflen / sizeof(uint64_t) - 1;
    uint64_t *src = (uint64_t *)p->s_ptr, *dest = (uint64_t *)p->r_ptr;

    while (nint64s--) {
        dest[0] = src[0] * W4;
        src++;dest++;
    }
}
#else
/* Basop function. */
int Overflow=0;
int64_t LL_mult(int64_t var1,int64_t var2)
{
    int64_t L_var_out;

    L_var_out = (int64_t)var1 * (int64_t)var2;
    if (L_var_out != (int64_t)0x4000000000000000LL) {
        L_var_out *= 2LL;
    }
    else {
        Overflow = 1;
        L_var_out = LLONG_MAX;
    }

    return(L_var_out);
}

void SendData(ArgStruct *p)
{
    int nint64s = p->bufflen / sizeof(int64_t) - 1;
    int64_t *src = (int64_t *)p->s_ptr, *dest = (int64_t *)p->r_ptr;

    while (nint64s--) {
        dest[0] = LL_mult(src[0],W4);
        src++;dest++;
    }

}

void RecvData(ArgStruct *p)
{
    int nint64s = p->bufflen / sizeof(int64_t) - 1;
    int64_t *src = (int64_t *)p->s_ptr, *dest = (int64_t *)p->r_ptr;

    while (nint64s--) {
        dest[0] = LL_mult(src[0],W4-nint64s);
        src++;dest++;
    }
}
#endif

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
