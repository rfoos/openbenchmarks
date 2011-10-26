/* AMD/Linux ONLY: gcc -O2 -o test_strcpy.c mmx.c */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>


#define LEN 0x1000


static char source[LEN];
static char destin[LEN];
int trg;
long count;


void stop(int unused)
{
printf("Count = %ld\n", count);
fflush(stdout);
trg = 0;
}
main()
{
int i;
memset(source, 0x20, LEN-1);
i = LEN - 1;
for(;;)
{
source[i] = (char) 0x00;
printf("Length = %d\n", i);
i -= 64;
if(i <=0 ) exit(0);
printf("Normal strcpy() ");
(void)signal(SIGALRM, stop);
trg = 1;
count = 0L;
alarm(1);
while(trg) 
{
count++;
strcpy(destin, source);
}
printf("Strange strcpy() ");
(void)signal(SIGALRM, stop);
trg = 1;
count = 0L;
alarm(1);
while(trg) 
{
count++;
memcpy(destin, source, strlen(source) + 1);
}
printf("MMX strcpy() ");
(void)signal(SIGALRM, stop);
trg = 1;
count = 0L;
alarm(1);
while(trg) 
{
count++;
//memcpy(destin, source, strlen(source) + 1);
_mmx_memcpy(destin, source, strlen(source) + 1);
}
}
return 0; 
}

