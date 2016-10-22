/****************** Start NSIEVE C Source Code ************************/

/****************************************************************/
/*                          NSIEVE                              */
/*                     C Program Source                         */
/*          Sieve benchmark for variable sized arrays           */
/*                Version 1.2b, 26 Sep 1992                     */
/*              Al Aburto  (aburto@nosc.mil)                    */
/*                                                              */
/*                                                              */
/* This Sieve of Eratosthenes program works with variable size  */
/* arrays. It is a straight forward extension of the original   */
/* Gilbreath version ( Gilbreath, Jim. "A High-Level Language   */
/* Benchmark." BYTE, September 1981, p. 180, and also Gilbreath,*/ 
/* Jim and Gary. "Eratosthenes Revisited: Once More Through the */
/* Sieve." BYTE January 1983, p. 283 ). Unlike the Sieve of     */
/* Gilbreath, NSIEVE uses register long variables, pointers,and */ 
/* large byte arrays via 'malloc()'.  Maximum array size is     */
/* currently set at 2.56 MBytes but this can be increased or    */
/* decreased by changing the program LIMIT constant.  NSIEVE    */
/* provides error checking to ensure correct operation.  Timer  */
/* routines are provided for several different systems. NSIEVE  */
/* results won't generally agree with the Gilbreath Sieve       */
/* results because NSIEVE specifically uses register long       */
/* variables. NSIEVE, and Sieve, are programs designed          */
/* specifically to generate and printout prime numbers (positive*/ 
/* integers which have no other integral factor other than      */
/* itself and unity, as 2,3,5,7,11, ... ). NSIEVE does not      */
/* conduct the 'typical' instructions one might expect from the */
/* mythical 'typical program'. NSIEVE results can be used to    */
/* gain a perspective into the relative performance of different*/
/* computer systems, but they can not be used in isolation to   */
/* categorize the general performance capabilities of any       */
/* computer system (no single benchmark program currently can do*/
/* this).                                                       */
/*                                                              */
/* The program uses a maximum array size of 2.56 MBytes. You can*/
/* increase or lower this value by changing the 'LIMIT' define  */
/* from 9 to a higher or lower value.  Some systems (IBM PC's   */
/* and clones) will be unable to work beyond 'LIMIT = 3' which  */
/* corresponds to an array size of only 40,000 bytes. Be careful*/
/* when specifying LIMIT > 3 for these systems as the system may*/ 
/* crash or hang-up. Normally NSIEVE will stop program execution*/  
/* when 'malloc()' fails.                                       */
/*                                                              */
/* The maximum array size is given by:                          */
/*              size = 5000 * ( 2 ** LIMIT ).                   */
/*                                                              */
/* The array 'Number_Of_Primes[LIMIT]' is intended to hold the  */
/* correct number of primes found for each array size up to     */
/* LIMIT = 20, but the array is only currently defined up to    */
/* LIMIT = 13.                                                  */
/*                                                              */
/* Program outputs to check for correct operation:              */
/*    Array Size  LIMIT    Primes Found      Last Prime         */
/*     (Bytes)                                                  */
/*         8191       0            1899           16381         */
/*        10000       1            2261           19997         */
/*        20000       2            4202           39989         */
/*        40000       3            7836           79999         */
/*        80000       4           14683          160001         */
/*       160000       5           27607          319993         */
/*       320000       6           52073          639997         */
/*       640000       7           98609         1279997         */
/*      1280000       8          187133         2559989         */
/*      2560000       9          356243         5119997         */
/*      5120000      10          679460        10239989         */
/*     10240000      11         1299068        20479999         */
/*     20480000      12         2488465        40960001         */
/*     40960000      13         4774994        81919993         */
/*     81920000      14         -------       ---------         */
/****************************************************************/

/****************************************************/
/* Example Compilation:                             */
/* (1) UNIX Systems:                                */
/*     cc -O -DUNIX nsieve.c -o nsieve              */
/*     cc -DUNIX nsieve.c -o nsieve                 */
/****************************************************/

/***************************************************************/
/* Timer options. You MUST uncomment one of the options below  */
/* or compile, for example, with the '-DUNIX' option.          */
/***************************************************************/
/* #define Amiga       */
/* #define UNIX        */
/* #define UNIX_Old    */
/* #define VMS         */
/* #define BORLAND_C   */
/* #define MSC         */
/* #define MAC         */
/* #define IPSC        */
/* #define FORTRAN_SEC */
/* #define GTODay      */
/* #define CTimer      */
/* #define UXPM        */
/* #define MAC_TMgr    */
/* #define PARIX       */
/* #define POSIX       */
/* #define WIN32       */
/* #define POSIX1      */
/***********************/

/* Modifications for embedded target. */
/* Allow printf removal for power measurement. */
#ifdef NOPRINTF
#define PRINTF(x)
#else
#define PRINTF(x) printf x
#endif
/* Clear warnings. */
int dtime(double *);
int SIEVE(long,long,long);

#include <stdio.h>
#ifndef vax
#include <stdlib.h>
#endif
#include <math.h>
		    /***********************************************/
#define LIMIT 9      /* You may need to change this to '3' for PC's */
		    /* and Clones or you can experiment with higher*/
		    /* values, but '13' is currently the max.      */
		    /***********************************************/

#ifdef Amiga
#include <exec/types.h>
#include <ctype.h>
#endif

#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#endif

#ifdef MSC
#include <ctype.h>
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

double nulltime,runtime,TimeArray[4];
double reftime,adj_time,emips;
double hmips,lmips,smips[21];

long L_Prime,N_Prime;      /* Last Prime and Number of Primes Found */
long ErrorFlag;

long Number_Of_Primes[21]; /* List of Correct Number of Primes for */
			  /* each sieve array size.               */

long NLoops[21];


int main()
{

long  i,j,k,p;
double sumtime;


PRINTF(("\n   Sieve of Eratosthenes (Scaled to 10 Iterations)\n"));
PRINTF(("   Version 1.2b, 26 Sep 1992\n\n"));
PRINTF(("   Array Size   Number   Last Prime     Linear"));
PRINTF(("    RunTime    MIPS\n"));
PRINTF(("    (Bytes)   of Primes               Time(sec)"));
PRINTF(("    (Sec)\n"));

       
		   /*******************************/
		   /* Number of        Array Size */
		   /* Primes Found      (Bytes)   */
Number_Of_Primes[0] =     1899;      /*       8191 */
Number_Of_Primes[1] =     2261;      /*      10000 */
Number_Of_Primes[2] =     4202;      /*      20000 */
Number_Of_Primes[3] =     7836;      /*      40000 */
Number_Of_Primes[4] =    14683;      /*      80000 */
Number_Of_Primes[5] =    27607;      /*     160000 */
Number_Of_Primes[6] =    52073;      /*     320000 */
Number_Of_Primes[7] =    98609;      /*     640000 */
Number_Of_Primes[8] =   187133;      /*    1280000 */
Number_Of_Primes[9] =   356243;      /*    2560000 */
Number_Of_Primes[10]=   679460;      /*    5120000 */
Number_Of_Primes[11]=  1299068;      /*   10240000 */
Number_Of_Primes[12]=  2488465;      /*   20480000 */
Number_Of_Primes[13]=  4774994;      /*   40960000 */
Number_Of_Primes[14]=        0;      /*   81920000 */
Number_Of_Primes[15]=        0;      /*  163840000 */

j = 8191;
k = 256;
p = 0;
SIEVE(j,k,p);

for( i=1 ; i<= 20 ; i++)
{
 NLoops[i] = 1;
}

p = 8;
if ( runtime > 0.125 ) p = 1;

NLoops[0] = 256 * p; 
NLoops[1] = 256 * p; 
NLoops[2] = 128 * p;
NLoops[3] =  64 * p;
NLoops[4] =  32 * p;
NLoops[5] =  16 * p;
NLoops[6] =   8 * p;
NLoops[7] =   4 * p;
NLoops[8] =   2 * p;
NLoops[9] =       p;
NLoops[10] =  p / 2;
NLoops[11] =  p / 4;

if ( p == 1 )
{
NLoops[10] = 1;
NLoops[11] = 1;
}

sumtime = 0.0;
i = 0;
j = 8191;
k = NLoops[0];
SIEVE(j,k,p);
sumtime = sumtime + runtime;
smips[i] = emips;

j = 5000;
ErrorFlag = 0;

for( i=1 ; i<= LIMIT ; i++)
{
   j = 2 * j;

   k = NLoops[i];

   SIEVE(j,k,p);
   smips[i] = emips;

   if( ErrorFlag == 0L )
   {
   if( N_Prime != Number_Of_Primes[i] )
   {
   PRINTF(("\n   Error --- Incorrect Number of Primes for Array: %ld\n",j));
   PRINTF(("   Number of  Primes  Found is: %ld\n",N_Prime));
   PRINTF(("   Correct Number of Primes is: %ld\n",Number_Of_Primes[i]));
   ErrorFlag = 1L;
   }
   }

   if( ErrorFlag > 0L ) break;

   sumtime = sumtime + runtime * ( 8191.0 / (double)j );

}

if( ErrorFlag == 2L )
{
PRINTF(("\n   Could Not Allocate Memory for Array Size: %ld\n",j));
}

sumtime = sumtime / (double)i;

hmips = 0.0;
lmips = 1.0e+06;
for( k=0 ; k < i ; k++)
{
if( smips[k] > hmips ) hmips = smips[k];
if( smips[k] < lmips ) lmips = smips[k];
}

PRINTF(("\n   Relative to 10 Iterations and the 8191 Array Size:\n"));
PRINTF(("   Average RunTime = %8.3f (sec)\n",sumtime));
PRINTF(("   High  MIPS      = %8.1f\n",hmips));
PRINTF(("   Low   MIPS      = %8.1f\n\n",lmips));
return ErrorFlag;
}


/**************************************/
/*  Sieve of Erathosthenes Program    */
/**************************************/
int
SIEVE(m,n,p)
long m,n,p;
{

register char *flags;
register long i,prime,k,ci;
register long count,size;

long  iter,j;

char *ptr;

#ifdef vax
char *malloc();
int   free(); 
#endif

size  = m - 1;
ptr   = malloc(m);

   ErrorFlag = 0L;
   N_Prime   = 0L;
   L_Prime   = 0L;

   if( !ptr )
     {
     ErrorFlag = 2L;
     return 0;
     }

   flags = ptr;

   dtime(TimeArray);
   dtime(TimeArray);
   nulltime = TimeArray[1];
   if ( nulltime < 0.0 ) nulltime = 0.0;

   j = 0;
						  /****************/
						  /* Instructions */
						  /*    *iter     */
						  /****************/
   dtime(TimeArray);
   for(iter=1 ; iter<=n ; iter++)                    
   {
   count = 0;                                       

   for(i=0 ; i<=size ; i++)                      
   {
   *(flags+i) = TRUE;                                /* 1*size  */
   }                                                 /* 3*size  */
						   
   ci = 0;                                         
     for(i=0 ; i<=size ; i++)                       
     {
	if(*(flags+i))                                /* 2*size  */
	{                                             /* 1*count */
	count++;                                      /* 1*count */
	prime = i + i + 3;                            /* 3*count */
	for(k = i + prime ; k<=size ; k+=prime)     /* 3*count */
	{
	ci++;                                       /* 1*ci    */
	*(flags+k)=FALSE;                           /* 1*ci    */
	}                                           /* 3*ci    */
						    /* 1*count */
	}
     }                                               /* 3*size  */
						   
   j = j + count;                                   
   }                                               
						   
   dtime(TimeArray);

   free(ptr);

   runtime = (TimeArray[1] - nulltime) * 10.0 / (double)n;

   if ( m == 8191 ) reftime = runtime;

   adj_time = reftime * ( (double)m / 8191.0 );

   emips = 9.0*(double)size+9.0*(double)count;
   emips = emips+5.0*(double)ci;
   emips = 1.0e-05*(emips/runtime);

   N_Prime = j / n;
   L_Prime = prime;

   if ( p != 0L )
   {
   PRINTF(("  %9ld   %8ld     %8ld  ",m,N_Prime,L_Prime));
   PRINTF(("%9.3f  %9.3f  %6.1f\n",adj_time,runtime,emips));
   }

return 0;
}

/*****************************************************/
/* Various timer routines.                           */
/* Al Aburto, aburto@nosc.mil, 18 Feb 1997           */
/*                                                   */
/* dtime(p) outputs the elapsed time seconds in p[1] */
/* from a call of dtime(p) to the next call of       */
/* dtime(p).  Use CAUTION as some of these routines  */
/* will mess up when timing across the hour mark!!!  */
/*                                                   */
/* For timing I use the 'user' time whenever         */
/* possible. Using 'user+sys' time is a separate     */
/* issue.                                            */
/*                                                   */
/* Example Usage:                                    */
/* [Timer options added here]                        */
/* double RunTime, TimeArray[3];                     */
/* main()                                            */
/* {                                                 */
/* dtime(TimeArray);                                 */ 
/* [routine to time]                                 */
/* dtime(TimeArray);                                 */
/* RunTime = TimeArray[1];                           */
/* }                                                 */
/* [Timer code added here]                           */
/*****************************************************/

/******************************/
/* Timer code.                */
/******************************/

/*******************/
/*  Amiga dtime()  */
/*******************/
#ifdef Amiga
#include <ctype.h>
#define HZ 50

int dtime(p)
double p[];
{
 double q;

 struct tt {
	long  days;
	long  minutes;
	long  ticks;
 } tt;

 q = p[2];

 DateStamp(&tt);

 p[2] = ( (double)(tt.ticks + (tt.minutes * 60L * 50L)) ) / (double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/*****************************************************/
/*  UNIX dtime(). This is the preferred UNIX timer.  */
/*  Provided by: Markku Kolkka, mk59200@cc.tut.fi    */
/*  HP-UX Addition by: Bo Thide', bt@irfu.se         */
/*****************************************************/
#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

#ifdef hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 getrusage(RUSAGE_SELF,&rusage);

 p[2] = (double)(rusage.ru_utime.tv_sec);
 p[2] = p[2] + (double)(rusage.ru_utime.tv_usec) * 1.0e-06;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/***************************************************/
/*  UNIX_Old dtime(). This is the old UNIX timer.  */
/*  Use only if absolutely necessary as HZ may be  */
/*  ill defined on your system.                    */
/***************************************************/
#ifdef UNIX_Old
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

struct tms tms;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 times(&tms);

 p[2] = (double)(tms.tms_utime) / (double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/*********************************************************/
/*  VMS dtime() for VMS systems.                         */
/*  Provided by: RAMO@uvphys.phys.UVic.CA                */
/*  Some people have run into problems with this timer.  */
/*********************************************************/
#ifdef VMS
#include time

#ifndef HZ
#define HZ 100
#endif

struct tbuffer_t
      {
       int proc_user_time;
       int proc_system_time;
       int child_user_time;
       int child_system_time;
      };

struct tbuffer_t tms;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 times(&tms);

 p[2] = (double)(tms.proc_user_time) / (double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/******************************/
/*  BORLAND C dtime() for DOS */
/******************************/
#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#include <time.h>

#define HZ 100
struct time tnow;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 gettime(&tnow);

 p[2] = 60.0 * (double)(tnow.ti_min);
 p[2] = p[2] + (double)(tnow.ti_sec);
 p[2] = p[2] + (double)(tnow.ti_hund)/(double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/**************************************/
/*  Microsoft C (MSC) dtime() for DOS */
/**************************************/
#ifdef MSC
#include <time.h>
#include <ctype.h>

#define HZ CLOCKS_PER_SEC
clock_t tnow;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 tnow = clock();

 p[2] = (double)tnow / (double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/*************************************/
/*  Macintosh (MAC) Think C dtime()  */
/*************************************/
#ifdef MAC
#include <time.h>

#define HZ 60

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 p[2] = (double)clock() / (double)HZ;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/************************************************************/
/*  iPSC/860 (IPSC) dtime() for i860.                       */
/*  Provided by: Dan Yergeau, yergeau@gloworm.Stanford.EDU  */
/************************************************************/
#ifdef IPSC
extern double dclock();

int dtime(p)
double p[];
{
  double q;

  q = p[2];

  p[2] = dclock();
  p[1] = p[2] - q;
	
  return 0;
}
#endif

/**************************************************/
/*  FORTRAN dtime() for Cray type systems.        */
/*  This is the preferred timer for Cray systems. */
/**************************************************/
#ifdef FORTRAN_SEC

fortran double second();

int dtime(p)
double p[];
{
 double q,v;

 q = p[2];

 second(&v);
 p[2] = v;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/***********************************************************/
/*  UNICOS C dtime() for Cray UNICOS systems.  Don't use   */
/*  unless absolutely necessary as returned time includes  */
/*  'user+system' time.  Provided by: R. Mike Dority,      */
/*  dority@craysea.cray.com                                */
/***********************************************************/
#ifdef CTimer
#include <time.h>

int dtime(p)
double p[];
{
 double    q;
 clock_t   clock(void);

 q = p[2];

 p[2] = (double)clock() / (double)CLOCKS_PER_SEC;
 p[1] = p[2] - q;

 return 0;
}
#endif

/********************************************/
/* Another UNIX timer using gettimeofday(). */
/* However, getrusage() is preferred.       */
/********************************************/
#ifdef GTODay
#include <sys/time.h>

struct timeval tnow;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 gettimeofday(&tnow,NULL);
 p[2] = (double)tnow.tv_sec + (double)tnow.tv_usec * 1.0e-6;
 p[1] = p[2] - q;

 return 0;
}
#endif

/*****************************************************/
/*  Fujitsu UXP/M timer.                             */
/*  Provided by: Mathew Lim, ANUSF, M.Lim@anu.edu.au */
/*****************************************************/
#ifdef UXPM
#include <sys/types.h>
#include <sys/timesu.h>
struct tmsu rusage;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 timesu(&rusage);

 p[2] = (double)(rusage.tms_utime) * 1.0e-06;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/**********************************************/
/*    Macintosh (MAC_TMgr) Think C dtime()    */
/*   requires Think C Language Extensions or  */
/*    #include <MacHeaders> in the prefix     */
/*  provided by Francis H Schiffer 3rd (fhs)  */
/*         skipschiffer@genie.geis.com        */
/**********************************************/
#ifdef MAC_TMgr
#include <Time.h>
#include <stdlib.h>

static TMTask   mgrTimer;
static Boolean  mgrInited = FALSE;
static double   mgrClock;

#define RMV_TIMER RmvTime( (QElemPtr)&mgrTimer )
#define MAX_TIME  1800000000L
/* MAX_TIME limits time between calls to */
/* dtime( ) to no more than 30 minutes   */
/* this limitation could be removed by   */
/* creating a completion routine to sum  */
/* 30 minute segments (fhs 1994 feb 9)   */

static void     Remove_timer( )
{
 RMV_TIMER;
 mgrInited = FALSE;
}

int     dtime( p )
double p[];
{
 if ( mgrInited ) {
    RMV_TIMER;
    mgrClock += (MAX_TIME + mgrTimer.tmCount)*1.0e-6;
    } else {
    if ( _atexit( &Remove_timer ) == 0 ) mgrInited = TRUE;
    mgrClock = 0.0;
   }
	
 p[1] = mgrClock - p[2];
 p[2] = mgrClock;
 if ( mgrInited ) {
    mgrTimer.tmAddr = NULL;
    mgrTimer.tmCount = 0;
    mgrTimer.tmWakeUp = 0;
    mgrTimer.tmReserved = 0;
    InsTime( (QElemPtr)&mgrTimer );
    PrimeTime( (QElemPtr)&mgrTimer, -MAX_TIME );
   }
 return( 0 );
}
#endif

/***********************************************************/
/*  Parsytec GCel timer.                                   */
/*  Provided by: Georg Wambach, gw@informatik.uni-koeln.de */
/***********************************************************/
#ifdef PARIX
#include <sys/time.h>

int dtime(p)
double p[];
{
 double q;

 q = p[2];
 p[2] = (double) (TimeNowHigh()) / (double) CLK_TCK_HIGH;
 p[1] = p[2] - q;

 return 0;
}
#endif

/************************************************/
/*  Sun Solaris POSIX dtime() routine           */
/*  Provided by: Case Larsen, CTLarsen@lbl.gov  */
/************************************************/
#ifdef POSIX
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/rusage.h>

#ifdef __hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 getrusage(RUSAGE_SELF,&rusage);

 p[2] = (double)(rusage.ru_utime.tv_sec);
 p[2] = p[2] + (double)(rusage.ru_utime.tv_nsec) * 1.0e-09;
 p[1] = p[2] - q;
	
 return 0;
}
#endif

/****************************************************/
/*  Windows NT (32 bit) dtime() routine             */
/*  Provided by: Piers Haken, piersh@microsoft.com  */
/****************************************************/
#ifdef WIN32
#include <windows.h>

int dtime(p)
double p[];
{
 double q;

 q = p[2];

 p[2] = (double)GetTickCount() * 1.0e-03;
 p[1] = p[2] - q;

 return 0;
}
#endif

/*****************************************************/
/* Time according to POSIX.1  -  <J.Pelan@qub.ac.uk> */
/* Ref: "POSIX Programmer's Guide"  O'Reilly & Assoc.*/
/*****************************************************/
#ifdef POSIX1
#define _POSIX_SOURCE 1
#include <unistd.h>
#include <limits.h>
#include <sys/times.h>

struct tms tms;

int dtime(p)
double p[];
{
 double q;
 times(&tms);
 q = p[2];
 p[2] = (double)tms.tms_utime / (double)CLK_TCK;
 p[1] = p[2] - q;
 return 0;
}
#endif

/*------ End of nsieve.c, say goodnight Liz! (Sep 1992) ------*/
