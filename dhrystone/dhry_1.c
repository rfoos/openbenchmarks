/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *                                                                            
 *  Version:    C, Version 2.1
 *                                                                            
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#include "dhry.h"

/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

extern char     *malloc ();
Enumeration     Func_1 ();
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

/* variables for time measurement: */

#ifdef TIMES
struct tms      time_info;
extern  int     times ();
                /* see library function "times" */
#define Too_Small_Time 120
                /* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
extern long     time();
                /* see library function "time"  */
#define Too_Small_Time 2
                /* Measurements should last at least 2 seconds */
#endif
#ifdef CLOCK
extern clock_t    clock();
                /* see library function "time"  */
#define Too_Small_Time 2
                /* Measurements should last at least 2 seconds */
#endif

long            Begin_Time,
                End_Time,
                User_Time;
float           Microseconds,
                Dhrystones_Per_Second;

/* end of variables for time measurement */

#ifdef MYTIME
// Clocks per Second / Microseconds per Second
// Return Clocks in Microseconds (1E+6)
#define CLOCKS_PER_MICROSEC 1000
long time(int x)
{
	return clock()/(CLOCKS_PER_MICROSEC);//GetTickCount();
}
#endif

main ()
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
        One_Fifty       Int_1_Loc;
  REG   One_Fifty       Int_2_Loc;
        One_Fifty       Int_3_Loc;
  REG   char            Ch_Index;
        Enumeration     Enum_Loc;
        Str_30          Str_1_Loc;
        Str_30          Str_2_Loc;
  REG   int             Run_Index;
  REG   int             Number_Of_Runs;
  FILE  *fstdout;

 // if ((fstdout=fopen("dhrystone.txt","w"))==NULL)
 // {
	  fstdout=stdout;
 // }

  /* Initializations */

  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;
  strcpy (Ptr_Glob->variant.var_1.Str_Comp, 
          "DHRYSTONE PROGRAM, SOME STRING");
  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */

  fprintf (fstdout,"\n");
  fprintf (fstdout,"Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  fprintf (fstdout,"\n");
  if (Reg)
  {
    fprintf (fstdout,"Program compiled with 'register' attribute\n");
    fprintf (fstdout,"\n");
  }
  else
  {
    fprintf (fstdout,"Program compiled without 'register' attribute\n");
    fprintf (fstdout,"\n");
  }
#ifdef DRUNS
  Number_Of_Runs = DRUNS;
#else
  fprintf (fstdout,"Please give the number of runs through the benchmark: ");
  {
    int n;
    scanf ("%d", &n);
    Number_Of_Runs = n;
  }
#endif
  fprintf (fstdout,"\n");
  fprintf (fstdout,"Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

  /***************/
  /* Start timer */
  /***************/
 
#ifdef TIMES
  times (&time_info);
  Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  Begin_Time = time ( /*(long *)*/ 0);
#endif
#ifdef CLOCK
  Begin_Time = clock();
#endif

  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {

    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/
  
#ifdef TIMES
  times (&time_info);
  End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  End_Time = time ( /*(long *)*/ 0);
#endif
#ifdef CLOCK
  End_Time = clock();
#endif
  fprintf (fstdout,"Execution ends\n");
  fprintf (fstdout,"\n");
  fprintf (fstdout,"Final values of the variables used in the benchmark:\n");
  fprintf (fstdout,"\n");
  fprintf (fstdout,"Int_Glob:            %d\n", Int_Glob);
  fprintf (fstdout,"        should be:   %d\n", 5);
  fprintf (fstdout,"Bool_Glob:           %d\n", Bool_Glob);
  fprintf (fstdout,"        should be:   %d\n", 1);
  fprintf (fstdout,"Ch_1_Glob:           %c\n", Ch_1_Glob);
  fprintf (fstdout,"        should be:   %c\n", 'A');
  fprintf (fstdout,"Ch_2_Glob:           %c\n", Ch_2_Glob);
  fprintf (fstdout,"        should be:   %c\n", 'B');
  fprintf (fstdout,"Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  fprintf (fstdout,"        should be:   %d\n", 7);
  fprintf (fstdout,"Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  fprintf (fstdout,"        should be:   Number_Of_Runs + 10\n");
  fprintf (fstdout,"Ptr_Glob->\n");
  fprintf (fstdout,"  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
  fprintf (fstdout,"        should be:   (implementation-dependent)\n");
  fprintf (fstdout,"  Discr:             %d\n", Ptr_Glob->Discr);
  fprintf (fstdout,"        should be:   %d\n", 0);
  fprintf (fstdout,"  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
  fprintf (fstdout,"        should be:   %d\n", 2);
  fprintf (fstdout,"  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
  fprintf (fstdout,"        should be:   %d\n", 17);
  fprintf (fstdout,"  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
  fprintf (fstdout,"        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  fprintf (fstdout,"Next_Ptr_Glob->\n");
  fprintf (fstdout,"  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
  fprintf (fstdout,"        should be:   (implementation-dependent), same as above\n");
  fprintf (fstdout,"  Discr:             %d\n", Next_Ptr_Glob->Discr);
  fprintf (fstdout,"        should be:   %d\n", 0);
  fprintf (fstdout,"  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
  fprintf (fstdout,"        should be:   %d\n", 1);
  fprintf (fstdout,"  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
  fprintf (fstdout,"        should be:   %d\n", 18);
  fprintf (fstdout,"  Str_Comp:          %s\n",
                                Next_Ptr_Glob->variant.var_1.Str_Comp);
  fprintf (fstdout,"        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  fprintf (fstdout,"Int_1_Loc:           %d\n", Int_1_Loc);
  fprintf (fstdout,"        should be:   %d\n", 5);
  fprintf (fstdout,"Int_2_Loc:           %d\n", Int_2_Loc);
  fprintf (fstdout,"        should be:   %d\n", 13);
  fprintf (fstdout,"Int_3_Loc:           %d\n", Int_3_Loc);
  fprintf (fstdout,"        should be:   %d\n", 7);
  fprintf (fstdout,"Enum_Loc:            %d\n", Enum_Loc);
  fprintf (fstdout,"        should be:   %d\n", 1);
  fprintf (fstdout,"Str_1_Loc:           %s\n", Str_1_Loc);
  fprintf (fstdout,"        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  fprintf (fstdout,"Str_2_Loc:           %s\n", Str_2_Loc);
  fprintf (fstdout,"        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  fprintf (fstdout,"\n");

  User_Time = End_Time - Begin_Time;

  /* Addition: False Positives negative.*/
  if (End_Time < 0 || Begin_Time < 0)
  {
    fprintf (fstdout,"Measured time invalid, returned a negative result\n");
    fprintf (fstdout,"Please check timer function.\n");
    fprintf (fstdout,"\n");
  }
  if (User_Time < Too_Small_Time)
  {
    fprintf (fstdout,"Measured time too small to obtain meaningful results\n");
    fprintf (fstdout,"Please increase number of runs\n");
    fprintf (fstdout,"\n");
  }
  else
  {
#if defined(TIME) || defined(CLOCK) // || defined(MYTIME)
// For this to work, User_Time must be in seconds.
    Microseconds = (float) User_Time * Mic_secs_Per_Second 
                        / (float) Number_Of_Runs;
    Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#else
    Microseconds = (float) User_Time * Mic_secs_Per_Second 
                        / ((float) HZ * ((float) Number_Of_Runs));
    Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
                        / (float) User_Time;
#endif
    fprintf (fstdout,"Microseconds for one run through Dhrystone: ");
    fprintf (fstdout,"%6.1f \n", Microseconds);
    fprintf (fstdout,"Dhrystones per Second:                      ");
    fprintf (fstdout,"%6.1f \n", Dhrystones_Per_Second);
    fprintf (fstdout,"\n");
  }
  fclose(fstdout);
}


Proc_1 (Ptr_Val_Par)
/******************/

REG Rec_Pointer Ptr_Val_Par;
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */
  
  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


Proc_2 (Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */

One_Fifty   *Int_Par_Ref;
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


Proc_3 (Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

Rec_Pointer *Ptr_Ref_Par;

{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif


