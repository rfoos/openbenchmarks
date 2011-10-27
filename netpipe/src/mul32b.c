#include    "netpipe.h"

#define W1  22725  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W2  21407  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W3  19266  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W4  16383  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W5  12873  /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W6  8867   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/
#define W7  4520   /*cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5*/

#ifndef ROW_SHIFT
#define ROW_SHIFT 22
#endif

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
  /* Print out message about results */

  printf("\n");
  printf("  *** Note about mul32b module results ***  \n");
  printf("\n");
  printf("The mul32b module is sensitive to the L1 and L2 cache sizes,\n" \
         "the size of the cache-lines, and the compiler.  The following\n" \
         "may help to interpret the results:\n" \
         "\n" \
         "* With cache effects and no perturbations (npmul32b -p 0),\n" \
         "    the plot will show 2 peaks.  The first peak is where data is\n" \
         "    copied from L1 cache to L1, peaking around half the L1 cache\n" \
         "    size.  The second peak is where data is copied from the L2 cache\n" \
         "    to L2, peaking around half the L2 cache size.  The curve then\n" \
         "    will drop off as messages are copied from RAM through the caches\n" \
         "    and back to RAM.\n" \
         "\n" \
         "* Without cache effects and no perturbations (npmul32b -I -p 0).\n" \
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

void SendData(ArgStruct *p)
{
    int nint32s = p->bufflen / sizeof(uint32_t) - 1;
    uint32_t *src = (uint32_t *)p->s_ptr, *dest = (uint32_t *)p->r_ptr;

    while (nint32s--) {
        dest[0] = src[0] * W4;
        src++;dest++;
    }

}

void RecvData(ArgStruct *p)
{
    int nint32s = p->bufflen / sizeof(uint32_t) - 1;
    uint32_t *src = (uint32_t *)p->s_ptr, *dest = (uint32_t *)p->r_ptr;

    while (nint32s--) {
        dest[0] = src[0] * W4;
        src++;dest++;
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
