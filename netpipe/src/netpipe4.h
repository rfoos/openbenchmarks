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
/*     * netpipe.h          ---- General include file                        */
/*****************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>       /* struct timeval */
#include <sys/resource.h>   /* getrusage() */
#include <stdlib.h>         /* malloc(3) */
#include <unistd.h>         /* getopt, read, write, ... */
#include <inttypes.h>       /* uint64_t */
#include <fcntl.h>

#ifdef INFINIBAND
#include <ib_defs.h> /* ib_mtu_t */
#endif


#define  MEMSIZE            40000000
        /* MEMSIZE should be more than twice the 
         * largest message size because we need to have room for at least two 
         * distinct buffers in no-cache bi-directional mode. 
         * Otherwise we can run into race condition problems.
         * It should also be larger than the cache size (or the amount of
         * memory that an InfiniBand card can register).
         */

#define  DEFPORT            5002
#define  MAXPORT            5102
#define  NSAMP              8000
#define  DEFPERT            3
#define  LONGTIME           1e99
#define  CHARSIZE           8
#define  STOPTM             1.0
#define  MAXINT             10000000
#define MAXNSMP             16                       /* Max of 16 SMP procs */

   /* Workspace size for CPU load measurement */
#define WORKSIZE 131072       /* 132 kB will fit into any cache */
#define NELEMENTS (WORKSIZE/16)

#define     ABS(x)     (((x) < 0)?(-(x)):(x))
#define     MIN(x,y)   (((x) < (y))?(x):(y))
#define     MAX(x,y)   (((x) > (y))?(x):(y))

/* Need to include the protocol structure header file.                       */
/* Change this to reflect the protocol                                       */

typedef struct protocolstruct ProtocolStruct;

#if defined(TCP)
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  
  struct protocolstruct
  {
      struct sockaddr_in      sdin,   /* socket structure #1              */
                              sdout;  /* socket structure #2              */
      int                     nodelay;  /* Flag for TCP nodelay           */
      struct hostent          *addr;    /* Address of host                */
      int                     sndbufsz, /* Size of TCP send buffer        */
                              rcvbufsz; /* Size of TCP send buffer        */
  };

#elif defined(INFINIBAND)

     /* Header files needed for InfiniBand */

#include    "vapi.h"        /* Mellanox Verbs API */
#include    "evapi.h"       /* Mellanox Verbs API extension */
#include    "vapi_common.h" /* Mellanox VIP layer of HCA Verbs */

  struct protocolstruct
  {
      int                     commtype; /* Communications type            */
      int                     comptype; /* Completion type                */
      IB_lid_t                *lid;     /* Array of lids for each proc    */
      VAPI_qp_num_t           *qp_num,  /* Array of qp_nums for each proc */
                              *dest_qp_num; /* Array of dest qp_nums      */
      VAPI_pd_hndl_t          pd_hndl;  /* Protection domain handle       */
      VAPI_cq_hndl_t          *s_cq_hndl, /* send completion queues       */
                              *r_cq_hndl; /* recv completion queues       */
      VAPI_qp_hndl_t          *qp_hndl; /* Q pair handles                 */
      VAPI_lkey_t             l_key_sbuf; /* Local send buf access key    */
      VAPI_lkey_t             l_key_rbuf; /* Local recv buf access key    */
      VAPI_rkey_t             r_key_sbuf; /* Remote send buf access key   */
      VAPI_rkey_t             r_key_rbuf; /* Remote recv buf access key   */
      void*                   r_addr_sbuf; /* Remote send buf address     */
      void*                   r_addr_rbuf; /* Remote recv buf address     */
      VAPI_rkey_t             *r_key;   /* Remote RDMA access keys        */
      void*                   *r_addr;  /* Remote RDMA addresses          */
      IB_mtu_t                ib_mtu;   /* MTU Size for Infiniband HCA    */
  };

enum completion_types {
   NP_COMP_LOCALPOLL,  /* Poll locally on last byte of data     */
   NP_COMP_VAPIPOLL,   /* Poll using vapi function              */
   NP_COMP_EVENT       /* Don't poll, use vapi event completion */
};

enum communication_types {
   NP_COMM_SENDRECV,           /* Communication with send/receive            */
   NP_COMM_SENDRECV_WITH_IMM,  /* Communication with send/receive & imm data */
   NP_COMM_RDMAWRITE,          /* Communication with rdma write              */
   NP_COMM_RDMAWRITE_WITH_IMM, /* Communication with rdma write & imm data   */
};

#elif defined(MPI)
  struct protocolstruct 
  { 
    int nbor, iproc;  /* still used for MPI2? */
    int use_get;
    int no_fence;
  };

#elif defined(KCOPY)
  struct protocolstruct 
  { 
    volatile int nbytes, source, tag, nleft;  /* nleft --> 0 on completion */
    int posted;
  };

#elif defined(PVM)

  struct protocolstruct
  {
    int     mytid; /* Keep track of our task id */
    int     othertid; /* Keep track of the other's task id */
  };

/*
  Choose one of the following to determine the type of data
  encoding for the PVM message passing.
  
  DataDefault means that PVM uses XDR encoding which ensures that
  the data can be packed / unpacked across non-homogeneous machines.
  
  If you know that the machines are the same, then you can use DataRaw
  and save some time (DDT - does not seem to help).
  
  DataInPlace means that the data is not copied at pack time, but is
  copied directly from memory at send time (DDT - this helps a lot).

#define PVMDATA     PvmDataDefault
#define PVMDATA     PvmDataRaw
#define PVMDATA     PvmDataInPlace
*/
#define PVMDATA     PvmDataInPlace


#elif defined(LAPI)
  struct protocolstruct { int nbor; };

#elif defined(SHMEM)
  #if defined(GPSHMEM)
    #include "gpshmem.h"
  #else
    #include <mpp/shmem.h>
  #endif
  struct protocolstruct
  {
          int nbor,ipe;
          int *flag;
  };

#elif defined(ARMCI)
    /* basically same as for GPSHMEM */
  struct protocolstruct
  {
          int nbor,ipe;
  };


#elif defined(GM)
  #include "gm.h"
  struct protocolstruct
  {
     int num_stokens;        /* The number of send tokens */
     short int *host_id;     /* Array of host id's */
  };

#elif defined(ATOLL)

  #include <atoll.h>
  
  struct protocolstruct
  {
      port_id id_self,       /* My port id */
              id_nbor;       /* My neighbor's port id */
  }

#elif defined(SISCI)

  #include "sisci_error.h"
  #include "sisci_api.h"
  #include "sisci_demolib.h"
  #include "scilib.h"

  struct protocolstruct {
    int nodeid;
    int* nodeids;
  };

#elif defined(DISK)
  struct protocolstruct {
     char *dfile_name;
     int read;
     char read_type;   /* c-char  d-double  s-stream */
  };

#elif defined(MEMCPY)
  struct protocolstruct {
     int fd[2];     /* File descriptors for pipe measurements */
  };

#elif defined(THEO)

  struct protocolstruct
  {
        /* The 3 required inputs */
     double Rmax, Rmed, Tlat;

        /* Optional inputs */
     int Sp, Sh, Rndv;
  };

#else
  struct protocolstruct { int nothing; };

#endif


typedef struct argstruct ArgStruct;
struct argstruct 
{
    /* This is the common information that is needed for all tests           */
    int      nprocs,        /* The number of processes                       */
             myproc,        /* My process number (0..nprocs-1)               */
             source,        /* Set to -1 (MPI_ANY_SOURCE) if -z specified    */
             dest;          /* The destination node number                   */
  
    int      cache,         /* Cache flag, 0 => limit cache, 1=> use cache   */
             bidir,         /* Bi-directional flag                           */
             stream,        /* Streaming mode                                */
             workload,      /* Measure the CPU workload of the comm system   */
             preburst,      /* Prepost burst mode                            */
             reset_conn,    /* Reset connection flag                         */
             syncflag;      /* flag for using sync sends in MPI mode         */

    char     **host, *hostfile; /* Name of other hosts, and the hostfile     */

    int      serv_sd,       /* File descriptor of the network socket         */
             *comm_sd;      /* Communication socket descriptors              */

    int      *port,         /* Array of ports used for connection            */
             *pid;          /* Array of PID numbers                          */

    char     *r_buff,       /* Aligned receive buffer                        */
             *r_buff_orig,  /* Original unaligned receive buffer             */
             *r_ptr,        /* Pointer to current location in recv buffer    */
             *r_ptr_saved,  /* Pointer for saving value of r_ptr             */

             *ds_buff,      /* Send pointer base on dest proc for kernel_copy.c */
             *dr_buff,      /* Recv pointer base on dest proc for kernel_copy.c */
             *ds_ptr,       /* Send pointer on dest proc for kernel_copy.c   */
             *dr_ptr,       /* Recv pointer on dest proc for kernel_copy.c   */

             *s_buff,       /* Aligned send buffer                           */
             *s_buff_orig,  /* Original unaligned send buffer                */
             *s_ptr;        /* Pointer to current location in send buffer    */

    int      soffset,       /* Send and recv buffer offsets                  */
             roffset;

    unsigned char expected; /* Expected value of first and last bytes        */             

    int      bufflen,       /* Length of transmitted buffer                  */
             bytesLeft,     /* Number of bytes left to xmit/recv             */
             upper,         /* Upper limit to bufflen                        */
             tr,rcv,        /* Transmit and Recv flags, or maybe neither     */
             master,        /* Proc 0 is the master                          */
             min_trials;    /* Min number of trials to make per data point   */

    double   runtime;       /* Runtime for each data point                   */

    /* Now we work with a union of information for protocol dependent stuff  */
    ProtocolStruct prot;
};

#if defined( _NETPIPE_C_ )
   /* Initial parameters for the args structure */

struct argstruct args_init = {
   2, -1, 0, 0,
   1, 0, 0, 0, 0, 0, 0,
   NULL, NULL,
   0, NULL,
   NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   0, 0,
   0,
   0, 0, MAXINT, 0, 0, 0, 7,
   0.25,
   0 
};
#endif

double When();

#if defined (THEO)
  double Theo_Time( ArgStruct *p );
#endif

void Init(ArgStruct *p, int* argc, char*** argv);

void Setup(ArgStruct *p);

void establish(ArgStruct *p);

void Sync(ArgStruct *p);

void PrepareToReceive(ArgStruct *p);

void SendData(ArgStruct *p);

int TestForCompletion( ArgStruct *p );

void DoWork( int requested_flops, double *mflops );

void RecvData(ArgStruct *p);

void Gather(ArgStruct *p, double *t);

void Broadcast(ArgStruct *p, unsigned int *nrepeat);

void FreeBuff(char *buff1, char *buff2);

void CleanUp(ArgStruct *p);

void InitBufferData(ArgStruct *p, int nbytes);

void SetExpected(ArgStruct *p);

void CheckBufferData(ArgStruct *p, int i, int j);

void UpdateExpected(ArgStruct *p);

void SwapSendRecvPtrs(ArgStruct *p);

void MyMalloc(ArgStruct *p, int bufflen);

void Reset(ArgStruct *p);

void mymemset(int *ptr, int c, int n);

void flushcache(int *ptr, int n);

void SetIntegrityData(ArgStruct *p);

void VerifyIntegrity(ArgStruct *p);

void* AlignBuffer(void* buff, int boundary);

void AdvanceSendPtr(ArgStruct* p, int blocksize);

void AdvanceRecvPtr(ArgStruct* p, int blocksize);

void SaveRecvPtr(ArgStruct* p);

void ResetRecvPtr(ArgStruct* p);

void PrintUsage();

int getopt( int argc, char * const argv[], const char *optstring);

void AfterAlignmentInit(ArgStruct *p);

   /* If the module does not need the following functions, then use
    * these empty definitions */
    
#if defined (_NETPIPE_C_)

#if ! defined (INFINIBAND) && ! defined (MPI2) && ! defined (ARMCI) && ! defined (LAPI)
void AfterAlignmentInit(ArgStruct *p) { }
#endif

#if ! defined (INFINIBAND) && ! defined (ARMCI) && ! defined (MPI2) && ! defined (LAPI) && ! defined(KCOPY)
void ModuleSwapPtrs( ArgStruct *p ) { }
#endif

#if ! defined (MPI2)
void Fence( ArgStruct *p ) { }
#endif

#endif
