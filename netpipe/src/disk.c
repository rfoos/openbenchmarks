/*****************************************************************************/
/* "NetPIPE" -- Network Protocol Independent Performance Evaluator.          */
/* Copyright 1997, 1998 Iowa State University Research Foundation, Inc.      */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation.  You should have received a copy of the     */
/* GNU General Public License along with this program; if not, write to the  */
/* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   */
/*                                                                           */
/*     * disk.c           ---- reading and writing to a disk                 */
/*****************************************************************************/
#include    "netpipe.h"

void Init(ArgStruct *p, int* pargc, char*** pargv)
{
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
    int nbytes = p->bufflen, nleft;
    FILE  *diskfile;
    double doubleread;
    char letter;


    diskfile = fopen(p->prot.dfile_name,"w+");

    if( p->prot.read)  /* reading from a file*/
    {
		if(p->prot.read_type=='c')
 		{
			for( nleft=0; nleft<nbytes; nleft++)
    			{
   				fscanf(diskfile,"%c", &letter); 
   			}	
		}
		else if( p->prot.read_type == 'd' )
		{
			for( nleft=0; nleft<nbytes; nleft+=sizeof(double))
    			{
       				 fscanf(diskfile, "%lf", &doubleread);
   			}			
		}

    }

    else             /* writing to a file*/

    {
		if( p->prot.read_type == 'c' )
 		{
	   		 for(nleft=0; nleft<nbytes; nleft++)
   			 {
/*       				 fprintf(diskfile, "A");*/
       				 fputc('A',diskfile);
   			 }

		}
		else if( p->prot.read_type == 'd' )
		{
         fwrite( p->s_buff, sizeof(double), nbytes/sizeof(double), diskfile);
		}
    }

    fclose(diskfile);
}

int TestForCompletion( ArgStruct *p )
{
   return 1;
}

void RecvData(ArgStruct *p)
{
    int nbytes = p->bufflen, nleft;
    FILE  *diskfile;
    char letter;

    if ((diskfile = fopen("dataout.txt","r"))==NULL)
    {
	if ((diskfile=fopen("dataout.txt","w"))==NULL)
	{
	 printf("Fatal Error: could not create dataout.txt\n");
	 exit(1);
	}
	fclose(diskfile);
	printf("Created disk file dataout.txt\n");
	if ((diskfile = fopen("dataout.txt","r"))==NULL)
	{
	 printf("Fatal Error: could not read dataout.txt after create.\n");
	 exit(1);
	}
    }

    for(nleft=0; nleft<nbytes; nleft++);
    {
      fscanf(diskfile,"%c", &letter); 
    }

    fclose(diskfile);
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
#if 0
void FreeBuff(char *buff1, char *buff2)
{
  if(buff1 != NULL)
    free(buff1);

  if(buff2 != NULL)
    free(buff2);
}
#endif
#if 0
void MyMalloc(ArgStruct *p, int bufflen, int soffset, int roffset)
{
  if((p->r_buff = (char *)malloc(bufflen+MAX(soffset,roffset)))==(char *)NULL)
  {
      fprintf(stderr,"couldn't allocate memory\n");
      exit(-1);
  } 

  if(!p->cache)
    if((p->s_buff = (char *)malloc(bufflen+soffset))==(char *)NULL)
    {
        fprintf(stderr,"Couldn't allocate memory\n");
        exit(-1);
    } 
}
#endif

void Reset(ArgStruct *p)
{

}
#if 0
void InitBufferData(ArgStruct *p, int nbytes, int soffset, int roffset)
{
  memset(p->r_buff, 'a', nbytes);

  if(!p->cache)
    memset(p->s_buff, 'b', nbytes);
}
#endif
void AfterAlignmentInit(ArgStruct *p)
{

}
