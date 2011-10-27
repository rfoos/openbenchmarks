/**

 mask64b           ---- single process bit manipulation
*/
#include    "netpipe.h"

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
  /* Print out message about results */

  printf("\n");
  printf("  *** Note about mask64b module results ***  \n");
  printf("\n");
  printf("The mask64b module is sensitive to the L1 and L2 cache sizes,\n" \
         "the size of the cache-lines, and the compiler.  The following\n" \
         "may help to interpret the results:\n" \
         "\n" \
         "* With cache effects and no perturbations (npmask64b -p 0),\n" \
         "    the plot will show 2 peaks.  The first peak is where data is\n" \
         "    copied from L1 cache to L1, peaking around half the L1 cache\n" \
         "    size.  The second peak is where data is copied from the L2 cache\n" \
         "    to L2, peaking around half the L2 cache size.  The curve then\n" \
         "    will drop off as messages are copied from RAM through the caches\n" \
         "    and back to RAM.\n" \
         "\n" \
         "* Without cache effects and no perturbations (npmask64b -I -p 0).\n" \
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
    int nint64s = p->bufflen / sizeof(uint64_t) - 1;
    uint64_t *src = (uint64_t *)p->s_ptr, *dest = (uint64_t *)p->r_ptr, mask[2];

    while (nint64s--) {
      mask[0]=src[0];mask[1]=dest[0];
      mask[0]<<=1;
      while (mask[1]=mask[0] && mask[1])
       mask[0] <<= 1;
      src++;dest++;
    }
      

}

void RecvData(ArgStruct *p)
{
    int nint64s = p->bufflen / sizeof(uint64_t), nleft;
    uint64_t *src = (uint64_t *)p->s_ptr, *dest = (uint64_t *)p->r_ptr, mask[2];

    while (nint64s) {
      mask[0]=src[0] ^ -1;mask[1]=dest[0] ^ -1;
      mask[0]<<=1;
      while (mask[1]=mask[0] && mask[1])
       mask[0] <<= 1;
      nint64s--; src++;dest++;
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
