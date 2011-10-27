
#include    "netpipe.h"

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
  /* Print out message about results */

  printf("\n");
  printf("  *** Note about strlen module results ***  \n");
  printf("\n");
  printf("The strlen module is sensitive to strong optimization.\n" \
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

    volatile int nleft;
void SendData(ArgStruct *p)
{
    int nbytes = p->bufflen;
    char *src = p->s_ptr, *dest = p->r_ptr;

    nleft=strlen(src);
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

    nleft -= strlen(src);

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

