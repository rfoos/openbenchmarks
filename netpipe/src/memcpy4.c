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
/*     * memcpy.c  - each process does a memcpy or optimized memory copy     */
/*        -P       - can also measure the performance through a pipe         */
/*****************************************************************************/

#include "netpipe.h"
#include <sys/shm.h>
#include <sched.h>

#ifdef USE_OPT_MEMCPY

#define memcopy(a,b,c) opt_memcpy(a,b,c)
void opt_memcpy();

#else

#define memcopy(a,b,c) memcpy(a,b,c)

#endif


void Init(ArgStruct *p, int* pargc, char*** pargv)
{
   p->tr = 1;
   p->rcv = 0;
   p->myproc = 0;
   p->nprocs = 1;
#if defined( USE_PIPE )
   int flags;
   pipe(p->prot.fd);
      /* Make the pipe non-blocking */
   flags = fcntl( p->prot.fd[0], F_GETFL, 0);
   flags |= O_NONBLOCK;
   fcntl( p->prot.fd[0], F_SETFL, flags);
   fcntl( p->prot.fd[1], F_SETFL, flags);
#endif
}

static int sync_id, *isync, sync_num = 0;
static int *ibroadcast;
static double *dgather;

void Setup(ArgStruct *p)
{
   int shm_size;

     /* Print out message about results */

#if ! defined( USE_PIPE )
   if( p->myproc == 0 ) {
      fprintf(stderr, "\n\n  *** Please see the dox/README file for notes on ***\n");
      fprintf(stderr, "  *** how to interpret the memcpy results.        ***\n\n");
   }
#endif

   if( p->nprocs == 1 ) return;

      /* Create the shared-memory segment */

   shm_size = p->nprocs * (2*sizeof(int) + sizeof(double));

   sync_id = shmget(500, shm_size, 0660 | IPC_CREAT );

   while( sync_id < 0 ) {

      sched_yield();

      sync_id = shmget(500, shm_size, 0660 | IPC_CREAT );
   }

/*fprintf(stderr, "Processor %d has sync_id %d\n", p->myproc, sync_id);*/

      /* Attach the segment to the isync array */

   if( (isync = shmat(sync_id, NULL, 0)) == (void *) -1 ) {

     fprintf(stderr, "Processor %d has shmat() error %d: %s\n",
             p->myproc, errno, strerror(errno));
     exit(0);
   }

   ibroadcast = &isync[p->nprocs];

   dgather = (double *) &isync[2 * p->nprocs];

   isync[p->myproc] = 0;

   sleep(1);

   Sync( p );
}   

void PrepareToReceive(ArgStruct *p) { }

void SendData(ArgStruct *p)
{
   int nbytes = p->bufflen, nb, mb;
   char *src = p->s_ptr, *dest = p->r_ptr;

#if defined( USE_PIPE )          /* Push the data thru a pipe */

   while( nbytes > 0 ) {

         /* non-blocking, so write as much as I can (~1 page) */

      nb = write(p->prot.fd[1], src, nbytes);

      mb = read( p->prot.fd[0], dest, nb);

      if( nb <= 0 || mb != nb ) {
         fprintf(stderr,"Pipe transfer was unsuccessful (%d/%d %d/%d)\n",
                 mb, nb, nbytes, p->bufflen);
      }
      src += nb;
      dest += nb;
      nbytes -= nb;
   }
#else
   memcopy(dest, src, nbytes);
#endif
}

int TestForCompletion( ArgStruct *p )
{
   return 1;
}

/* RecvData should do nothing.  If it copies data back, then
 * netpipe.c needs to be altered to advance the pointers to
 * prevent cache effects for -I.
 */

void RecvData(ArgStruct *p)
{
   int nbytes = p->bufflen;
   char *src = p->s_ptr, *dest = p->r_ptr;

   return;  /* Do nothing */

   memcopy(src, dest, nbytes);
}

   /* Just do a global sync since this should be fast */

void Sync(ArgStruct *p)
{
   int i, loop_count = 0;

   if( p->nprocs == 1 ) return;

/*fprintf(stderr, "%d > Sync\n", p->myproc);*/

      /* Signal that I have arrived */

   sync_num = (sync_num+1) % 10;
   isync[ p->myproc ] = (isync[ p->myproc ]+1) % 10;

   for( i=0; i<p->nprocs; i++ ) {

      while( isync[i] != sync_num && isync[i] != (sync_num+1)%10 ) {
/*         loop_count++;*/
         sched_yield();
         if( isync[i]%10 == 11 ) printf("%d",isync[i]);
/*         if( loop_count == 10000000 ) {*/
/*            fprintf(stderr,"%d stalled in Sync %d (%d %d)\n",*/
/*                    p->myproc, sync_num, isync[0], isync[1]);*/
/*            exit(0);*/
/*         }*/
      }
   }

/*fprintf(stderr, "%d < Sync\n", p->myproc);*/
}

void Broadcast(ArgStruct *p, unsigned int *buf)
{
   if( p->nprocs == 1 ) return;

   Sync( p );

   if( p->myproc == 0 ) ibroadcast[0] = *buf;

   Sync( p );

   if( p->myproc != 0 ) *buf = ibroadcast[0];
}

void Gather(ArgStruct *p, double *buf)
{
   int i;

   if( p->nprocs == 1 ) return;

   Sync( p );

   dgather[p->myproc] = buf[p->myproc];

   Sync( p );

   for( i=0; i<p->nprocs; i++ ) buf[i] = dgather[i];
}

void CleanUp(ArgStruct *p)
{
   struct shmid_ds ds;
   int err = 0;

   if( p->nprocs == 1 ) return;

   Sync( p );

      /* Detach the shared-memory segment */

   err = shmctl( sync_id, IPC_RMID, &ds);
}

void Reset(ArgStruct *p) { }
