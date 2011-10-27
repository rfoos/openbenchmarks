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
/* This code was originally developed by Quinn Snell, Armin Mikler,          */
/* John Gustafson, and Guy Helmer.                                           */
/*                                                                           */
/* It is currently being developed by Dave Turner with many contributions    */
/* from ISU students (Bogdan Vasiliu, Adam Oline, Xuehua Chen, Brian Smith). */
/*****************************************************************************/

/* The default cache mode uses just one buffer per processor.  This
 * shows better performance when data gets bounced between cache on
 * SMP nodes and when the data buffer is already registered with the 
 * InfiniBand card.
 *
 * The cache invalidation mode (-I) uses separate send and recv buffers.
 * Each is a moving window through a very large memory buffer ( > cache size ).
 * This guarantees that data starts in main memory, and is not registered
 * with the InfiniBand card.
 *
 * Both these cases are needed to understand the SMP and InfiniBand performance
 * since both cases occur in applications.  Many other modules are not
 * affected by the -I mode.
 *
 * The bidirectional mode also uses 2 buffers.  Both procs send data
 * simultaneously then receive it before sending it out again from
 * the recv buffer.  This double buffering is needed to guarantee 
 * that data does not overwrite the previous message before it is read.
 */

#define _NETPIPE_C_
#include "netpipe.h"
#undef _NETPIPE_C_

#if defined(MPLITE)
#include "mplite.h" /* Included for the malloc wrapper to protect from */
#endif


extern char *optarg;
int         integCheck=0;   /* Integrity check                              */
double      *failures; 


int main(int argc, char **argv)
{
    FILE        *out;           /* Output data file                          */
    char        s[255],s2[255],delim[255],*pstr, *comma, *spc;
    char        net_type;       /* The network type (E or I)                 */
    int         *memcache;      /* used to flush cache                       */

    int         len_buf_align,  /* meaningful when args.cache is 0. buflen   */
                                /* rounded up to be divisible by 8           */
                num_buf_align,  /* meaningful when args.cache is 0. number   */
                                /* of aligned buffers in memtmp              */
                npairs,         /* nprocs/2, or nprocs for memcpy and theo   */
                memcopy=0;      /* 1 when running NPmemcpy                   */

    int         c,              /* option index                              */
                i, j, k, n, nq, /* Loop indices                              */
                asyncReceive=0, /* Pre-post a receive buffer?                */
                bufalign=16*1024,/* Boundary to align buffer to              */
                nrepeat_const=0,/* Set if we are using a constant nrepeat    */
                len,            /* Number of bytes to be transmitted         */
                inc=0,          /* Increment value                           */
                perturbation=DEFPERT, /* Perturbation value                  */
                pert,
                start= 1,       /* Starting value for signature curve        */
                flags,          
                end=MAXINT;     /* Ending value for signature curve          */
    unsigned int nrepeat;        /* Number of times to do the transmission    */
   
    ArgStruct   args;           /* Arguments for all the calls               */

    double      t, t0, t1, t2,  /* Time variables                            */
                tlast,          /* Time for the last transmission            */
                tlat,           /* The latency time                          */
                t_min, t_max,   /* Stats for multiple pairs                  */
                t_average, t_raw,
                *times,         /* Array for gathering times from other pairs*/
                latency;        /* Network message latency                   */
    double      min_time, max_time, avg_time;
    double      min_bps, avg_bps, max_bps;  /* bits per second for each trial*/
    int         bits;           /* number of bits in the message             */
    int         theo = 0;       /* Not a theoretical simulation              */
    double      cal_mflops,mflops,
                flops, cpu_load;/* Measure of the CPU workload    */
    double      tcal;
    int         cal_flops;
    int         TestCalls, TCalls;

    /* Initialize vars that may change from default due to arguments */

    strcpy(s, "np.out");   /* Default output file */

    /* Let modules initialize related vars, and possibly call a library init
       function that requires argc and argv */

    args = args_init;   /* Initialize the args structure */

    Init(&args, &argc, &argv);   /* This will set args.tr and args.rcv */

        /* Parse the arguments. See Usage for description */

    while ((c = getopt(argc, argv, "xwF:SO:rR:IiPszgfaPqQBH:p:o:l:u:b:m:n:t:c:d:D:")) != -1)
    {
        switch(c)
        {
#if defined (THEO)
            case 'F':       /* Tlat, Rmax, Rmed parameters for theo sim */
                      sscanf( optarg, "%c,%lf,%lf,%lf", &net_type, &args.prot.Tlat, 
                              &args.prot.Rmax, &args.prot.Rmed );
                      if( net_type == 'E' || net_type == 'e' ) {
                        printf("Ethernet Tlat = %lf, Rmax = %lf, Rmed = %lf\n\n",
                                args.prot.Tlat, args.prot.Rmax, args.prot.Rmed);
                        args.prot.Sp = 1500 + 26; /* 1500 B MTU + 26 B pre-amble */
                        args.prot.Sh = 26 + 20;   /* 26 B preamble + 20 B header */
                      } else if( net_type == 'I' || net_type == 'i' ) {
                        printf("InfiniBand Tlat = %lf, Rmax = %lf, Rmed = %lf\n\n",
                                args.prot.Tlat, args.prot.Rmax, args.prot.Rmed);
                        args.prot.Sp = 1024 + 30; /* Typically a 1024 B MTU */
                        args.prot.Sh = 30;   /* 8+12+4+4+2 = 30 B */
                      } else {
                        printf("ERROR: Not a recognizable network type\n");
                        exit(-1);
                      }

                      nrepeat_const = 1;
                      args.min_trials = 1;
                      theo = 1;
                      break;

            case 'R':       /* Enter a Rendezvous threshold */
                      args.prot.Rndv = atoi(optarg);
                      break;
#else
            case 'R':       /* Specify a constant number of repeats for each test */
                      nrepeat_const = atoi(optarg);
                      fprintf(stderr,"Using a constant number of %d ping-pongs\n", nrepeat_const);
                      fprintf(stderr,"NOTE: Be leary of timings that are close to the clock accuracy\n");
                      fprintf(stderr,"of the system, which can be as bad as 1-20 ms.\n");
                      break;
#endif

            case 'w':       /* Measure the CPU workload of the comm system */
                      args.workload = 1;
                      asyncReceive = 1;
                      fprintf(stderr,"Measuring the CPU workload\n");
                      fprintf(stderr,"This requires asynchronous communications\n");
                      fprintf(stderr,"Workloads for small messages have a 10%% uncertainty,\n");
                      fprintf(stderr,"and the workload measurement may affect the communication performance.\n");
                      break;

            case 'I':       /* Invalidate cache */
                      args.cache = 0;
                      break;

            case 'P':       /* Preposting all ARecv's (vendor cheating) */
                      args.preburst = 1;
                      asyncReceive = 1;
                      break;

            case 'O':       /* Set source and dest offset, sep by a comma */
                      strcpy(s2,optarg);
                      strcpy(delim,",");
                      if((pstr=strtok(s2,delim))!=NULL) {
                         args.soffset=atoi(pstr);
                         if((pstr=strtok((char *)NULL,delim))!=NULL)
                            args.roffset=atoi(pstr);
                         else /* only got one token */
                            args.roffset=args.soffset;
                      } else {
                         args.soffset=0; args.roffset=0;
                      }
                      break;

            case 'p':       /* Set the perturbation (0 is the default) */
                      perturbation = atoi(optarg);
                      break;

            case 'q':       /* Quick run */
                      args.min_trials = 3;
                      args.runtime = 0.10;
                      perturbation = 0;
                      break;

            case 'Q':       /* Quick run of 1 repeat and many tests */
                      args.min_trials = 100;
                      args.runtime = 0.10;
                      perturbation = 0;
                      if( nrepeat_const == 0 ) nrepeat_const = 1;
                      break;

            case 'o':       /* Set the output file */
                      strcpy(s,optarg);
                      break;

            case 's':       /* Stream data in one direction only */
                      args.stream = 1;
#if defined (TCP)
                      args.reset_conn = 1;
#endif
                      break;

            case 'l':       /* Set the lower bound for the message size */
                      start = atoi(optarg);
                      break;

            case 'u':       /* Set the upper bound for the message size */
                      end = atoi(optarg);
                      break;

#ifdef TCP
            case 'b': /* -b # resets the buffer size, -b 0 keeps system defs */
                      args.prot.sndbufsz = args.prot.rcvbufsz = atoi(optarg);
                      break;
#endif

            case 'B':       /* Test bi-directional communications */
                      args.bidir = 1;    /* Both procs are transmitters */
                      break;

            case 'n':       /* Set the number of processes <-n nprocs> */
                      if( optarg != 0 ) args.nprocs = atoi(optarg);
#if defined(THEO)
                      if( args.nprocs != 1 ) {
                         fprintf(stderr,"The theo.c module only supports 1 process at this time\n");
                         exit(0);
                      }

#elif defined(MEMCPY)  /* No limitations */
#elif defined (TCP) || (MPI) || (GM) || (INFINIBAND)
                      if( args.nprocs < 2 || args.nprocs%2 == 1 ) {
                         fprintf(stderr,"The number of processes must be even, nprocs = %d\n", args.nprocs);
                         exit(0);
                      }
#else                      /* All others must have 2 procs */
                      if( args.nprocs != 2 ) {
                         fprintf(stderr,"This module only supports 2 processes at this time\n");
                         exit(0);
                      }
#endif
                      fprintf(stderr,"Running on %d processors\n", args.nprocs);
                      break;

            case 'h':    /* Deprecated feature */
                      fprintf(stderr,"You must now specify the hostnames in nphosts or the file in <-H hostfile>\n");
                      PrintUsage(); 
                      break;

            case 'H':    /* This is the hostfile, possibly followed by myproc */
                      if( args.hostfile != NULL) free( args.hostfile );
                      args.hostfile = (char *)malloc(strlen(optarg)+1);
                      strcpy( args.hostfile, optarg);
                      comma = strstr( args.hostfile, ",");
                      if( comma != NULL ) {
                         args.myproc = atoi( (char *)(comma+1) );
                         *comma = '\0';
                      }
                      break;

#ifdef DISK
            case 'd':       /* I/O file name */
                      args.tr = 1;      /* -d to specify input/output file */
                      args.rcv = 0;
                      args.prot.read = 0;
                      args.prot.read_type = 'c';
                      args.prot.dfile_name = (char *)malloc(strlen(optarg)+1);
                      strcpy(args.prot.dfile_name, optarg);
                      break;

            case 'D':       /* Disk I/O mode (r == read, else write) */
                      if( optarg[0] == 'r' )
                         args.prot.read = 1;
                      else
                         args.prot.read = 0;
                      args.prot.read_type = optarg[1];
                      break;
#endif

            case 'i':       /* Do an integrity check on all messages */
                      integCheck = 1;
                      perturbation = 0;
                      /* Start with integer size, plus one for receives that
                       * poll on last byte */
                      start = sizeof(int)+1;
                      break;


#if defined(MPI)
            case 'z':       /* Receive using the ANY_SOURCE flag */
                      args.source = -1;
                      break;

            case 'a':       /* Use asynchronous receives (MPI_Irecv) */
                      asyncReceive = 1;
                      break;

            case 'S':       /* Use synchronous sends (MPI_SSend) */
                      args.syncflag=1;
                      fprintf(stderr,"Using synchronous sends\n");
                      break;
#endif
#if defined(MPI2)
            case 'g':       /* Use MPI_Get for 1-sided tests */
                      args.prot.use_get = 1;
                      fprintf(stderr,"Using MPI-2 Get instead of Put\n");
                      break;

            case 'f':       /* Do not use intervening fence calls */
                      args.prot.no_fence = 1;
                      break;
#endif /* MPI2 */

#if defined(INFINIBAND)
            case 'm': switch(atoi(optarg)) {
                        case 256: args.prot.ib_mtu = MTU256;
                          break;
                        case 512: args.prot.ib_mtu = MTU512;
                          break;
                        case 1024: args.prot.ib_mtu = MTU1024;
                          break;
                        case 2048: args.prot.ib_mtu = MTU2048;
                          break;
                        case 4096: args.prot.ib_mtu = MTU4096;
                          break;
                        default: 
                          fprintf(stderr, "Invalid MTU size, must be one of "
                                          "256, 512, 1024, 2048, 4096\n");
                          exit(-1);
                      }
                      break;

            case 't': if( !strcmp(optarg, "send_recv") ) {
                         fprintf(stderr,"Using Send/Receive communications\n");
                         args.prot.commtype = NP_COMM_SENDRECV;
                      } else if( !strcmp(optarg, "send_recv_with_imm") ) {
                         fprintf(stderr,"Using Send/Receive communications with immediate data\n");
                         args.prot.commtype = NP_COMM_SENDRECV_WITH_IMM;
                      } else if( !strcmp(optarg, "rdma_write") ) {
                         fprintf(stderr,"Using RDMA Write communications\n");
                         args.prot.commtype = NP_COMM_RDMAWRITE;
                      } else if( !strcmp(optarg, "rdma_write_with_imm") ) {
                         fprintf(stderr,"Using RDMA Write communications with immediate data\n");
                         args.prot.commtype = NP_COMM_RDMAWRITE_WITH_IMM;
                      } else {
                         fprintf(stderr, "Invalid transfer type "
                                 "specified, please choose one of:\n\n"
                                 "\tsend_recv\t\tUse Send/Receive communications\t(default)\n"
                                 "\tsend_recv_with_imm\tSame as above with immediate data\n"
                                 "\trdma_write\t\tUse RDMA Write communications\n"
                                 "\trdma_write_with_imm\tSame as above with immediate data\n\n");
                         exit(-1);
                      }
                      break;

            case 'c': if( !strcmp(optarg, "local_poll") ) {
                         fprintf(stderr,"Using local polling completion\n");
                         args.prot.comptype = NP_COMP_LOCALPOLL;
                      } else if( !strcmp(optarg, "vapi_poll") ) {
                         fprintf(stderr,"Using VAPI polling completion\n");
                         args.prot.comptype = NP_COMP_VAPIPOLL;
                      } else if( !strcmp(optarg, "event") ) {
                         fprintf(stderr,"Using VAPI event completion\n");
                         args.prot.comptype = NP_COMP_EVENT;
                      } else {
                         fprintf(stderr, "Invalid completion type specified, "
                                 "please choose one of:\n\n"
                                 "\tlocal_poll\tWait for last byte of data\t(default)\n"
                                 "\tvapi_poll\tUse VAPI polling function\n"
                                 "\tevent\t\tUse VAPI event handling function\n\n");
                         exit(-1);
                      }
                      break;
#endif

#if defined(TCP)
            case 'r':       /* Reset the TCP socket between trials */
                      args.reset_conn = 1;
                      break;
#endif

            default: 
                     PrintUsage(); 
       }
   }

      /* Setup will set many values in args, including myproc and nprocs */

   Setup(&args);

   times = malloc( args.nprocs * sizeof(double) );
   failures = malloc( (args.nprocs+1) * sizeof(double) );

   if( args.myproc == 0 ) args.master = 1;   /* Proc 0 is the master */

#ifdef MEMCPY
   memcopy = 1;
   npairs = args.nprocs;
#elif defined( THEO )
   npairs = args.nprocs;
#else
   npairs = args.nprocs/2;
#endif

#if defined(INFINIBAND)
   asyncReceive = 1;
   if( args.master ) 
      fprintf(stderr, "Preposting asynchronous receives (required for Infiniband)\n");
#endif

   if( args.stream ) {
      if( args.bidir ) {
         if( args.master ) 
             fprintf(stderr,"You can't use -s and -B together\n");
         exit(0);
      }

      if( args.master ) {
         fprintf(stderr,"Streaming in one direction only.\n\n");
#ifdef TCP
         fprintf(stderr,"Sockets are reset between trials to avoid\n");
         fprintf(stderr,"degradation from a collapsing window size.\n\n");
#endif
         fprintf(stderr,"Streaming does not necessarily provide an accurate measurement\n");
         fprintf(stderr,"of the latency since small messages may get bundled together.\n\n");
      }
   }

   if(integCheck && args.preburst ) {
      if( args.master ) 
          fprintf(stderr, "Integrity check is not supported with prepost burst\n");
      exit(0);
   }

   if ( start < 1 || start > end)
   {
       if( args.master ) 
          fprintf(stderr, "Start MUST be positive and LESS than end (%d <--> %d)\n", start, end);
       exit(0);
   }

   if( args.master ) {
#if ! defined(MEMCPY)
      if( args.nprocs > 2 && args.bidir == 0 ) {
         fprintf(stderr, "\nMultiple overlapping unidirectional communications may become out of sync,\n");
         fprintf(stderr, "so this may not tell you much.  I recommend bidirectional communications here.\n\n");
      }
#endif
      if( args.soffset != 0 || args.roffset != 0 )
         fprintf(stderr, "Transmit buffer offset: %d\nReceive buffer offset: %d\n\n",args.soffset,args.roffset);
      if( perturbation > 0 ) {
         fprintf(stderr,"Using a perturbation value of %d\n\n", perturbation);
      } else {
         fprintf(stderr,"Using no perturbations\n\n");
      }
      if( args.preburst == 1 ) {
         fprintf(stderr,"Preposting all receives before a timed run.\n");
         fprintf(stderr,"Some would consider this cheating,\n");
         fprintf(stderr,"but it is needed to match some vendor tests.\n\n");
      }
      if( args.cache == 0 )
         fprintf(stderr,"Performance measured without cache effects\n\n");
      if( strcmp(s, "np.out" ) )
         fprintf(stderr,"Sending output to %s\n\n", s);
      if( integCheck )
         fprintf(stderr,"Doing an integrity check instead of measuring performance\n\n");
      if( args.source == -1 )
         fprintf(stderr,"Receive using the ANY_SOURCE flag\n\n");
      if( asyncReceive )
         fprintf(stderr,"Preposting asynchronous receives\n\n");
      if( args.reset_conn )
         fprintf(stderr,"Resetting connection after every trial\n\n");
      if( args.bidir ) {
         fprintf(stderr,"Bi-directional mode using %d pairs of nodes.\n", args.nprocs/2);
         fprintf(stderr,"Passing data in both directions simultaneously.\n");
         fprintf(stderr,"The output shows the combined throughput.\n");
#ifdef TCP
            /* The message size will be maxed at sndbufsz+rcvbufsz */
         fprintf(stderr,"The socket buffer size may limit the maximum test size.\n\n");
#endif
         if( end > args.upper ) 
            fprintf(stderr,"The upper limit is being set to %d Bytes\n", args.upper);
         fprintf(stderr,"\n");
      }
   }

   if( args.bidir && end > args.upper ) end = args.upper;

#if defined(GM)

   if(args.stream && (!nrepeat_const || nrepeat_const > args.prot.num_stokens)) {
      if( args.master ) {
         fprintf(stderr,"\nGM is currently limited by the driver software to %d\n", 
                 args.prot.num_stokens);
         fprintf(stderr,"outstanding sends. The number of repeats will be set\n");
         fprintf(stderr,"to this limit for every trial in streaming mode.  You\n");
         fprintf(stderr,"may use the -n switch to set a smaller number of repeats\n\n");
      }
      nrepeat_const = args.prot.num_stokens;
   }

#endif

   if( args.master ) {                   /* Open the output file np.out */

       if ((out = fopen(s, "w")) == NULL) {

           fprintf(stderr,"Can't open %s for output\n", s);
           exit(1);
       }
   }

      /* Test the timing to set tlast for the first test. Since we only need
       * an estimate, this is simpler than the main benchmark code farther
       * below. For the estimate, we always do a cache mode ping-pong test. */

   i = args.cache;
   j = args.bidir;
   args.cache = 1;
   args.bidir = 0;
   
   args.bufflen = args.bytesLeft = start;
   MyMalloc(&args, args.bufflen+bufalign);

   args.r_buff_orig = args.r_buff;
   args.s_buff_orig = args.r_buff;

   args.r_buff = AlignBuffer(args.r_buff, bufalign);
   args.s_buff = args.r_buff;

      /* Some modules will require that functions like AfterAlignmentInit
       * and PrepareToReceive are called, so we still call them here. */

   InitBufferData(&args, args.bufflen);
   AfterAlignmentInit(&args);

   args.r_ptr = args.r_buff+args.roffset;
   args.s_ptr = args.s_buff+args.soffset;

   Fence(&args);

   t0 = When();
   t0 = When();
   t0 = When();
   t1 = When();
   if( args.myproc == 0 ) fprintf(stderr,"The clock accuracy is approximately %lf microseconds\n\n", (t1-t0)*1e6);

      /* This Broadcast is needed to push args.r_ptr to the destination proc */

   nrepeat = 100;
   Broadcast(&args, &nrepeat);

   if( asyncReceive ) PrepareToReceive(&args);

   Sync(&args);

   t0 = When();
      for( n=0; n<nrepeat; n++) {
         if( args.tr ) {
            SetExpected(&args);
            SendData(&args);
            RecvData(&args);
            CheckBufferData(&args, 1, n);
            if( asyncReceive && n<nrepeat-1 )
               PrepareToReceive(&args);
         } else {
            RecvData(&args);
            CheckBufferData(&args, 1, n);
            if( asyncReceive && n<nrepeat-1 )
               PrepareToReceive(&args);
            SetExpected(&args);
            SendData(&args);
         }
         UpdateExpected(&args);
      }
   tlat = tlast = (When() - t0)/(2*nrepeat);

   Fence(&args);

      /* Sync and Reset before freeing buffers */

   Sync(&args);

   Reset(&args);

      /* Free the buffers and any other module-specific resources. */

   FreeBuff(args.r_buff_orig, NULL);

      /* Reset cache and bidirectional mode to original states */

   args.cache = i;
   args.bidir = j;

      /* Calibrate the CPU workload rate if needed */

   if( args.workload ) {

         /* Pull the data into cache */

      flops = 0.0;
      DoWork( NELEMENTS, &flops );

         /* Now calibrate */

      flops = 0.0;
      t0 = When();

         for( i=0; i<10000; i++ ) DoWork( NELEMENTS, &flops );

      tcal = (When()-t0);

         /* Scale down to approximately 1 us test times */

      cal_flops = flops * 1e-6 / tcal;

      flops = 0.0;
      t0 = When();

         for( i=0; i<1000; i++ ) DoWork( 10*cal_flops, &flops );

      tcal = (When()-t0);

      cal_mflops = 1e-6 * flops / tcal;

      if( args.tr ) {
         fprintf(stderr,"\n The CPU delivers %8.2lf MFlops when the comm system is idle\n",
                 cal_mflops);
         fprintf(stderr," %lf Mflops in %lf seconds\n",1e-6*flops, tcal);
         fprintf(stderr," Each call to DoWork will do %d Flops taking %lf microseconds\n\n",
                 cal_flops, 1000*tcal);
      }

      tcal /= 1000;    /* Seconds per cal to DoWork */
   }

      /* Do setup for no-cache mode, using two distinct buffers. */

   if (!args.cache)
   {

          /* Allocate dummy pool of memory to flush cache with */

       if ( (memcache = (int *)malloc(MEMSIZE)) == NULL)
       {
           perror("malloc");
           exit(1);
       }
       mymemset(memcache, 0, MEMSIZE/sizeof(int)); 

          /* Allocate large memory pools */

       MyMalloc(&args, MEMSIZE+bufalign); 

          /* Save buffer addresses */
       
       args.s_buff_orig = args.s_buff;
       args.r_buff_orig = args.r_buff;

          /* Align buffers */

       args.s_buff = AlignBuffer(args.s_buff, bufalign);
       args.r_buff = AlignBuffer(args.r_buff, bufalign);

          /* Post alignment initialization */

       i = args.bufflen;
       args.bufflen = MEMSIZE;
       AfterAlignmentInit(&args);
       args.bufflen = i;

          /* Initialize send buffer pointer */
       
          /* Both soffset and roffset should be zero if we don't have 
           * any offset stuff, so this should be fine */

       args.s_ptr = args.s_buff+args.soffset;
       args.r_ptr = args.r_buff+args.roffset;
   }

      /* Set a starting value for the message size increment. */

   inc = (start > 1) ? start / 2 : 1;
   nq = (start > 1) ? 1 : 0;


       /**************************
        * Main loop of benchmark *
        **************************/

   if( args.master ) fprintf(stderr,"Now starting the main loop\n");

   for ( n = 0, len = start; 
        n < NSAMP - 3 && tlast < STOPTM && len <= end; 
        len = len + inc, nq++ )
   {

           /* Exponentially increase the block size.  */

      if (nq > 2) inc = ((nq % 2))? inc + inc: inc;
       
          /* This is a perturbation loop to test nearby values */

      for (pert = ((perturbation > 0) && (inc > perturbation+1)) ? -perturbation : 0;
           pert <= perturbation; 
           n++, pert += ((perturbation > 0) && (inc > perturbation+1)) ? perturbation : perturbation+1)
      {

         Sync(&args);    /* Sync to prevent race condition in armci module */

               /* Calculate how many times to repeat the experiment. */

         if( args.master ) {

            if (nrepeat_const) {
               nrepeat = nrepeat_const;
/*            } else if (len == start) {*/
/*                nrepeat = MAX( args.runtime/( 0.000020 + start/(8*1000) ), args.min_trials);*/
            } else {
               nrepeat = MAX((args.runtime / ((double)args.bufflen /
                             (args.bufflen - inc + 1.0) * tlast)),3);
            }
         }
         if( nrepeat <= 0 || nrepeat > 1000000000 ) {
            fprintf(stderr,"ERROR: nrepeat = %d is out of range (tlast = %lf)\n",
                    nrepeat, tlast);
            exit(-1);
         }

         args.bufflen = len + pert;

         /* Subtract 1 from the number of bytes if integrity check
          * because we include an extra byte in case the receive 
          * mechanism needs to poll on the last byte of the buffer */

         if( args.master )
            fprintf(stderr,"%3d: %7d bytes %6d times --> ",
                    n,args.bufflen - (integCheck ? 1 : 0), nrepeat);

         if (args.cache) { /* Allow cache effects.  We use only one buffer */

               /* Allocate the buffer with room for alignment*/

            MyMalloc(&args, args.bufflen+bufalign); 

            if( !args.bidir ) args.s_buff = args.r_buff;
            
               /* Save buffer addresses */

            args.r_buff_orig = args.r_buff;
            args.s_buff_orig = args.s_buff;

               /* Align buffers to bufalign boundary */

            args.r_buff = AlignBuffer(args.r_buff, bufalign);
            args.s_buff = AlignBuffer(args.s_buff, bufalign);
               
               /* Initialize buffer with data
                *
                * NOTE: The buffers should be initialized with some sort of
                * valid data, whether it is actually used for anything else,
                * to get accurate results.  Performance increases noticeably
                * if the buffers are left uninitialized, but this does not
                * give very useful results as realworld apps tend to actually
                * have data stored in memory.  We are not sure what causes
                * the difference in performance at this time.
                */

            InitBufferData(&args, args.bufflen);


               /* Post-alignment initialization */

            AfterAlignmentInit(&args);

               /* Initialize buffer pointers (We use r_ptr and s_ptr for
                * compatibility with no-cache mode, as this makes the code
                * simpler) 
                */
               /* offsets are zero by default so this saves an #ifdef */

            args.r_ptr  = args.r_buff  + args.roffset;
            args.s_ptr  = args.s_buff  + args.soffset;

         } else { /* Eliminate cache effects.  We use two distinct buffers */

               /* this isn't truly set up for offsets yet */
               /* Size of an aligned memory block including trailing padding */

            len_buf_align = args.bufflen;
            if(bufalign != 0)
               len_buf_align += bufalign - args.bufflen % bufalign;
 
               /* Initialize the buffers with data
                *
                * See NOTE above.
                */
            InitBufferData(&args, MEMSIZE); 
               
               /* Reset buffer pointers to beginning of pools */

            args.r_ptr  = args.r_buff  + args.roffset;
            args.s_ptr  = args.s_buff  + args.soffset;
         }

         min_time = LONGTIME;
         max_time = avg_time = 0;
         mflops = 0.0;

         failures[args.myproc] = 0.0;

            /* This Broadcast is needed to push args.r_ptr to the 
             * destination proc for kernel_copy.c */

         Broadcast(&args, &nrepeat);

            /* Finally, we get to transmit or receive and time */

            /* NOTE: If a module is running that uses only one process (e.g.
             * memcpy), we assume that it will always have the args.tr flag
             * set.  Thus we make some special allowances in the transmit 
             * section that are not in the receive section.
             */

         SaveRecvPtr(&args);  /* Needed for TCP workload tests */

         if( args.tr || args.bidir ) {

               /* This is the transmitter: send the block args.min_trials times, and
                * if we are not streaming, expect the receiver to return each
                * block.
                */

            for (i = 0; i < (integCheck ? 1 : args.min_trials); i++) {                    

               if(args.preburst && asyncReceive && !args.stream) {

                  /* We need to save the value of the recv ptr so
                   * we can reset it after we do the preposts, in case
                   * the module needs to use the same ptr values again
                   * so it can wait on the last byte to change to indicate
                   * the recv is finished.
                   */

                  SaveRecvPtr(&args);

                  for(j=0; j<nrepeat; j++) {

                     PrepareToReceive(&args);
                     if(args.bidir && args.cache)
                        SwapSendRecvPtrs(&args);
                     if(!args.cache)
                        AdvanceRecvPtr(&args, len_buf_align);
                  }

                  if( args.bidir && args.cache && nrepeat % 2 != 0 ) 
                     SwapSendRecvPtrs(&args);

                  ResetRecvPtr(&args);
               }

                  /* Flush the cache using the dummy buffer */

               if (!args.cache)
                  flushcache(memcache, MEMSIZE/sizeof(int));

               Fence(&args);

               flops = 0.0;  TCalls = 1;

               Sync(&args);

               t0 = When();

               for (j = 0; j < nrepeat; j++) {

                  args.bytesLeft = args.bufflen;

                  if (!args.preburst && asyncReceive && !args.stream)
                     PrepareToReceive(&args);

                  SetExpected(&args);

                  SendData(&args);

                  if (!args.stream) {

                        /* Do CPU workload measurement if needed */

                     if( args.workload ) {

                        while( ! TestForCompletion(&args) ) {

                           tcal = 1e6 * MAX( tlast / 100.0, tlat / 10.0);

                           DoWork( tcal * cal_flops, &flops );

                           TCalls ++;
                        }
                     }

                     RecvData(&args);
                     CheckBufferData(&args, i, j);
                  }

                  if(args.bidir && args.cache) SwapSendRecvPtrs(&args);
                   
                  if(!args.stream && !args.cache)
                     AdvanceRecvPtr(&args, len_buf_align);

                  if (!args.cache)
                     AdvanceSendPtr(&args, len_buf_align);

                  UpdateExpected(&args);
               }

/* NOTE: NetPIPE does each data point args.min_trials times, bouncing the message
 * nrepeat times for each trial, then reports the lowest of the args.min_trials
 * times.  This was chosen before me, and I haven't changed it.  I do now
 * dump the min, max, and avg to the np.out file.  -Dave Turner
 */

#if defined (THEO)
               t = Theo_Time( &args );
#else
               t = t_raw = (When() - t0);

               t /= nrepeat;

               if( !memcopy && !args.stream && !args.bidir) t /= 2;
#endif
                  /* t is now the 1-directional transmission time */

               Fence(&args);

                  /* If nrepeat is odd swap the pointers one more time */

               if( args.bidir && args.cache && nrepeat % 2 != 0 ) 
                  SwapSendRecvPtrs(&args);

               Reset(&args);
 
               min_time = MIN(min_time, t);
               max_time = MAX(max_time, t);
               avg_time += t;

               if( args.workload && t == min_time ) {
                  mflops = 1e-6 * flops / t_raw;
                  TestCalls = TCalls;
               }
            }

         } else {

            /* This is the receiver: receive the block args.min_trials times, and
               if we are not streaming, send the block back to the sender.  */

            for (i = 0; i < (integCheck ? 1 : args.min_trials); i++) {

               if (asyncReceive) {

                  if (args.preburst) {

                        /* We need to save the value of the recv ptr so
                         * we can reset it after we do the preposts, in case
                         * the module needs to use the same ptr values again
                         * so it can wait on the last byte to change to 
                         * indicate the recv is finished.
                         */

                     SaveRecvPtr(&args);

                     for (j=0; j < nrepeat; j++) {

                        PrepareToReceive(&args);
                        if (!args.cache)
                           AdvanceRecvPtr(&args, len_buf_align);
                     }
                         
                     ResetRecvPtr(&args);
                         
                  } else {

                     PrepareToReceive(&args);
                  }
                      
               }
                    
                  /* Flush the cache using the dummy buffer */

               if (!args.cache)
                  flushcache(memcache, MEMSIZE/sizeof(int));

               Fence(&args);

               flops = 0.0;  TCalls = 1;

               Sync(&args);

               t0 = When();

               for (j = 0; j < nrepeat; j++) {

                  args.bytesLeft = args.bufflen;

                     /* Do CPU workload measurement if needed */

                  if( args.workload ) {

                     while( ! TestForCompletion(&args) ) {

                        tcal = 1e6 * MAX( tlast / 100.0, tlat / 10.0);

                        DoWork( tcal * cal_flops, &flops );

                        TCalls ++;
                     }
                  }

                  RecvData(&args);

                  CheckBufferData(&args, i, j);

                  if (!args.cache) 
                     AdvanceRecvPtr(&args, len_buf_align);
 
                  if (!args.preburst && asyncReceive && (j < nrepeat-1)) 
                     PrepareToReceive(&args);

                  if (!args.stream) {

                     SetExpected(&args);
                            
                     SendData(&args);

                     if(!args.cache) 
                        AdvanceSendPtr(&args, len_buf_align);
                   
                  }

                  UpdateExpected(&args);

               }
               t = t_raw = (When() - t0);

               t /= nrepeat;

               if( !memcopy && !args.stream && !args.bidir) t /= 2;

               Fence(&args);

               Reset(&args);
 
               min_time = MIN(min_time, t);
               max_time = MAX(max_time, t);
               avg_time += t;

               if( args.workload && t == min_time ) {
                  mflops = 1e-6 * flops / t_raw;
                  TestCalls = TCalls;
               }
            }
         }

              /* Now gather the times to proc 0 if needed */

         times[args.myproc] = min_time;

         if( args.stream || npairs > 1){  /* Recv proc calcs time and sends to Trans */

            Gather(&args, times);

            if( args.stream && args.myproc < args.nprocs / 2 ) {
               times[args.myproc] = times[args.dest];
            }
         }

            /* Take the maximum time if multiple pairs are being used */

         t_min = t_average = 0;  t_max = LONGTIME;

         for( i=0; i < npairs; i++) {
            t_average += times[i] / npairs;
            if( times[i] > t_min ) t_min = times[i];
            if( times[i] < t_max ) t_max = times[i];
         }

             /* Streaming mode doesn't really calculate correct latencies.
              * Protect against a zero time. */

         if(min_time == 0.0) min_time = 0.000001;
         tlast = min_time;
  
         avg_time /= args.min_trials;

              /* Calculate the aggregate throughput */

         bits = args.bufflen * CHARSIZE * (1+args.bidir);
         min_bps = npairs * (double) bits / (min_time * 1e6);
         avg_bps = npairs * (double) bits / (avg_time * 1e6);
         max_bps = npairs * (double) bits / (max_time * 1e6);

/* NOTE:  The original authors used 1 MB = 1024*1024 Bytes.
 *        Since the network throughput is being reported in Mbps and
 *        maximum data rates across the transmission medium are factors
 *        of 10, not 2, this is being changed to 1 MB = 1,000,000 bytes.
 *                        - Dave Turner
 */

/*         bps = npairs * (double) bits / (min_time * 1024 * 1024);*/

             /* Print to the data file np.out and stdout */
 
         Gather(&args, failures);

         if (args.master) {

            for( i=0, k=0; i<args.nprocs; i++ ) k += (int) failures[i];

            if(integCheck) {

               /* Subtract 1 from the number of bytes if integrity check
                * because we include an extra byte in case the receive
                * mechanism needs to poll on the last byte of the buffer */

               fprintf(out,"%8d bytes %6d times %8d failures\n", (bits / 8)-1, nrepeat, k);
               fflush(out);

               fprintf(stderr, " %8d failures", k);

               for( i=0; i<args.nprocs; i++ ) {

                  if( (int) failures[i] ) 
                     fprintf(stderr," p%d:%4d", i, (int) failures[i]);
                  else
                     fprintf(stderr,"        ");

               }
               fprintf(stderr,"\n");

            } else {

               if( args.workload ) {

                  cpu_load = 100 * (cal_mflops - mflops) / cal_mflops;

                     /* protect against minor fluctuations */

/*                  if( cpu_load < 0 ) cpu_load = 0;*/

                  fprintf(out,"%8d %12.8lf %12.8lf %12.8lf %12.8lf %6.2lf %d\n", 
                              bits / 8, min_bps, avg_bps, max_bps, min_time,
                              cpu_load, (args.bufflen * nrepeat) / TestCalls );
                  fflush(out);

               } else {

                  fprintf(out,"%8d %lf %12.8lf %12.8lf %12.8lf\n", 
                          bits / 8, min_bps, avg_bps, max_bps, min_time);
                  fflush(out);

               }

               if( k == 0 || theo )
                  if( args.workload ) {
/*                     if( min_time / 10 < tcal ) */
/*                        fprintf(stderr," (%8.2lf) Mbps in (%10.2lf) usec (%6.2lf%%) CPU load\n",*/
/*                                min_bps, min_time*1.0e6, cpu_load);*/
/*                     else */
                        fprintf(stderr," %8.2lf Mbps in %10.2lf usec %6.2lf%% CPU load\n",
                                min_bps, min_time*1.0e6, cpu_load);
                  } else
                     fprintf(stderr," %8.2lf Mbps in %10.2lf usec\n", min_bps, min_time*1.0e6);
               else
                  fprintf(stderr," %8.2lf Mbps in %10.2lf usec (integrity problems!!!)\n", min_bps, min_time*1.0e6);

            }
            fflush(out);
         }
   
            /* Free using original buffer addresses since we may have aligned
               r_buff and s_buff */

         if (args.cache)
            FreeBuff(args.r_buff_orig, NULL);

      } /* End of the perturbation loop */

   } /* End of main loop  */
 
      /* Free using original buffer addresses since we may have aligned
         r_buff and s_buff */

   if (!args.cache) {
      FreeBuff(args.s_buff_orig, args.r_buff_orig);
   }

   CleanUp(&args);

   if (args.master) {
      fclose(out);
      fprintf(stderr,"The run has completed successfully\n");
   }

   return 0;
}


/* Return the current time in seconds, using a double precision number.
 * This has been changed to subtract off the 0-time to increase the
 * precision of the routine at the suggestion of Bob Felderman.
 * This should also protect against roll-over of the 64-bit cycle counter.
 */

#ifdef USE_TSC

static uint64_t last_ticks = 0, nflips = 0;
static double MHz = 0;

double When()
{
   double mytime, get_mhz();
   volatile uint64_t ticks, t64;

   if( MHz == 0 ) {
      MHz = get_mhz( );   /* Get the CPU frequency */
/*      fprintf(stderr,"Using the cycle counter with the CPU MHz = %lf\n", MHz);*/
   }

   __asm__ __volatile__ (
      "rdtsc            \n"
      : "=A" (ticks) );

/*      "movl %%eax, 0(%0)\n"*/
/*      "movl %%edx, 4(%0)\n"*/
/*      :*/
/*      : "r" (ticks)*/
/*      : "eax", "edx" );*/

/*fprintf(stderr,"ticks = %.0lf\n", (double) ticks);*/

      /* Has the cycle counter flipped since the last call.
       * Note:  For 32-bit cycle counters (AMD), this will only work
       * if this routine is called at least every 2 seconds.
       */

   if( ticks < last_ticks ) { /* Assume it is a 32-bit counter that flipped */
      nflips++;
/*      fprintf(stderr,"The cycle counter has flipped %d times\n", nflips);*/
   } 

   t64 = ticks + (nflips << 32);

   mytime = (double) t64 / (double) (MHz * 1e6) ;

/*fprintf(stderr,"time = %lf\n", mytime );*/

      /* Reset the starting time after the last of each pair of calls */

   last_ticks = ticks;

   return mytime;
} 

double get_mhz( )
{
   FILE *fd;
   double mhz = -1;
   char line[100], garb[100];
   int i;

   fd = fopen( "/proc/cpuinfo", "r");

   for( i=0; i<20; i++ ) {

      fscanf( fd, "%[^\n]\n", line);

      if( strstr( line, "cpu MHz" ) != NULL ) {
         sscanf( line, "%[^0-9]%lf", garb, &mhz);
         break;
      }
   }

   fclose( fd );

   if( mhz < 0 ) {
      fprintf(stderr, "ERROR: Could not get the CPU MHz rate from /proc/cpuinfo\n");
      fprintf(stderr, "MHz = %lf\n", MHz);
      exit(0);
   }

   return mhz;
}
#else

static struct timeval start_time = {0, 0};

double When()
{
   struct timeval tp;
   double mytime;

   gettimeofday(&tp, NULL);

   if( start_time.tv_sec == 0 ) {  /* Set the initial time */
      start_time.tv_sec  = tp.tv_sec;
      start_time.tv_usec = tp.tv_usec;
   }

   mytime = tp.tv_sec - start_time.tv_sec;
   mytime += 1e-6 * (tp.tv_usec - start_time.tv_usec);

   return (mytime);
}
#endif

/* 
 * The mymemset() function fills the first n integers of the memory area 
 * pointed to by ptr with the constant integer c. 
 */
void mymemset(int *ptr, int c, int n)  
{
   int i;

   for (i = 0; i < n; i++) *(ptr + i) = c;
}

/* Read the first n integers of the memmory area pointed to by ptr, to flush  
 * out the cache   
 */
void flushcache(int *ptr, int n)
{
   static int flag = 0;
   int    i; 

   flag = (flag + 1) % 2; 
   if ( flag == 0) 
       for (i = 0; i < n; i++)
           *(ptr + i) = *(ptr + i) + 1;
   else
       for (i = 0; i < n; i++) 
           *(ptr + i) = *(ptr + i) - 1; 
    
}

static unsigned int random_seed = 0x45;

void set_seed( unsigned int seed )
{
   random_seed = seed;
}

static int random_num()  /* not used at this time */
{
   unsigned int i, j;

   i = random_seed;
   j = ((i >> 31)& 1) ^ ((i >> 21) & 1) ^ ((i >> 1) & 1) ^ (i & 1);
   i = (j << 31) | (i >> 1);
   random_seed = i;

   return (i);
}

/* For integrity check, set each integer-sized block to the next consecutive
 * integer, starting with the value 0 in the first block, and so on.  Earlier
 * we made sure the memory allocated for the buffer is of size i*sizeof(int) +
 * 1 so there is an extra byte that can be used as a flag to detect the end
 * of a receive.
 */

void SetIntegrityData(ArgStruct *p)
{
  int i;
  int num_segments;

  num_segments = p->bufflen / sizeof(int);

  for(i=0; i<num_segments; i++) {

    *( (int*)p->s_ptr + i ) = i;

  }
}

void VerifyIntegrity(ArgStruct *p)
{
   int i;
   int num_segments;

   if( !p->stream || p->myproc > p->nprocs / 2 ) {

      num_segments = p->bufflen / sizeof(int);

      for(i=1; i<num_segments-1; i++) {

        if( *( (int*)p->r_ptr + i )  != i ) {

          failures[p->myproc] += 1.0;
/*
          fprintf(stderr, "Proc %d - Integrity check failed: Expecting %d but received %d\n",
                  p->myproc, i, *( (int*)p->r_ptr + i ) );
*/
        }
      }
   }
}  


void PrintUsage()
{
    fprintf(stderr,"\n NETPIPE USAGE \n\n");
#if defined(THEO)
    fprintf(stderr,"-F N,Tlat,Rmax,Rmedium\n");
    fprintf(stderr,"   N is E for Ethernet or I for InfiniBand\n");
    fprintf(stderr,"   Tlat is the measured small-message latency in us\n");
    fprintf(stderr,"   Rmax is the measured maximum bandwidth in Mbps\n");
    fprintf(stderr,"   Rmed is the theoretical bandwidth of the medium in Mbps\n");
    fprintf(stderr,"        (1000 Mbps for GigE, 8000 Mbps for 4x InfiniBand)\n");
    fprintf(stderr,"-R nbytes : nbytes = the rendezvous threshold in Bytes\n");
#else
    fprintf(stderr,"a: asynchronous receive (a.k.a. preposted receive)\n");

#if defined(TCP)
    fprintf(stderr,"b: specify TCP send/receive socket buffer sizes\n");
#endif

    fprintf(stderr,"B: Send data in both directions at the same time.\n");
#if defined(TCP)
    fprintf(stderr,"   The maximum test size is limited by the TCP buffer size/n");
#endif

#if defined(INFINIBAND)
    fprintf(stderr,"c: specify type of completion <-c type> for InfiniBand\n"
           "   valid types: local_poll, vapi_poll, event\n"
           "   default: local_poll\n");
#endif
    
#if defined(MPI2)
    fprintf(stderr,"g: use get instead of put\n");
    fprintf(stderr,"f: do not use fence during timing segment; may not work with\n");
    fprintf(stderr,"   all MPI-2 implementations\n");
#endif

#if defined(TCP)
/*    fprintf(stderr,"h: specify the hostname of the receiver <-h host>\n");*/
    fprintf(stderr,"H: specify the hostfile <-H hostfile> which must contain at least to hosts.\n");
    fprintf(stderr,"   For multiple overlapping pairwise tests, pairs are set from the outside in\n");
    fprintf(stderr,"   (0 & np-1, 1 & np-2, ...)\n");
    fprintf(stderr,"   For SMP nodes, you must also specify the proc# <-H hostfile,proc#>\n");
    fprintf(stderr,"   The nplaunch script does this automatically.\n");
#endif

    fprintf(stderr,"I: Invalidate cache (measure performance without cache effects).\n"
           "   This simulates data coming from main memory instead of cache.\n");
    fprintf(stderr,"i: Do an integrity check instead of measuring performance\n");
    fprintf(stderr,"l: lower bound start value e.g. <-l 1>\n");

#if defined(INFINIBAND)
    fprintf(stderr,"m: set MTU for Infiniband adapter <-m mtu_size>\n");
    fprintf(stderr,"   valid sizes: 256, 512, 1024, 2048, 4096 (default 1024)\n");
#endif

    fprintf(stderr,"n: specify the even number of processes <-n nprocs>\n");
    fprintf(stderr,"   nprocs=2 is default (1 proc only for memcpy.c)\n");
    fprintf(stderr,"   (nprocs != default) only works for TCP and MPI currently.\n");

    fprintf(stderr,"o: specify output filename <-o filename>\n");
    fprintf(stderr,"O: specify transmit and optionally receive buffer offsets <-O 1,3>\n");
    fprintf(stderr,"p: set the perturbation number <-p 1>\n"
           "   (default = 3 Bytes, set to 0 for no perturbations)\n");
    fprintf(stderr,"P: Prepost all receives before measuring performance\n");
    fprintf(stderr,"q: Do a quick run (min_trials 7-->3, runtime 0.25-->0.10\n");

#if defined(TCP)
    fprintf(stderr,"r: reset sockets for every trial\n");
#endif
    fprintf(stderr,"R: Set a constant value for number of repeats <-R 50>\n");

    fprintf(stderr,"s: stream data in one direction only.\n");
#if defined(MPI)
    fprintf(stderr,"S: Use synchronous sends.\n");
#endif

#if defined(INFINIBAND)
    fprintf(stderr,"t: specify type of communications <-t type> for InfiniBand\n"
           "   valid types: send_recv, send_recv_with_imm,\n"
           "                rdma_write, rdma_write_with_imm\n"
           "   defaul: send_recv\n");
#endif
    
    fprintf(stderr,"u: upper bound stop value e.g. <-u 1048576>\n");
 
#if defined(MEMCPY)
    fprintf(stderr,"\n    The following commands must use nplaunch\n");
    fprintf(stderr,"    They only work on NUMA systems with homenode support\n");
    fprintf(stderr,"    (Try 'numactl --show' to test your system)\n");
    fprintf(stderr,"ml: For NUMA SMPs, do memcpy using local memory\n");
    fprintf(stderr,"mi: For NUMA SMPs, do memcpy using interleaved memory\n");
    fprintf(stderr,"mx: For NUMA SMPs, do memcpy in your neighbor's memory\n");
#endif
 
#if defined(MPI)
    fprintf(stderr,"z: receive messages using the MPI_ANY_SOURCE flag\n");
#endif

#if defined(MPI)
    fprintf(stderr,"   May need to use -a to choose asynchronous communications for MPI/n");
#endif
#endif   /* ifdef THEO / else */
    fprintf(stderr,"\n");
    exit(0);
}

void* AlignBuffer(void* buff, int boundary)
{
  if(boundary == 0)
    return buff;
  else
    /* char* typecast required for cc on IRIX */
    return ((char*)buff) + (boundary - ((unsigned long)buff % boundary) );
}

void AdvanceSendPtr(ArgStruct* p, int blocksize)
{
     /* Move the send buffer pointer forward if there is room.
      * The ds_ptr is also advanced for the kernel_copy.c module
      * which does a put from s_ptr to ds_ptr on the destination proc.
      */

  if(p->s_ptr + blocksize < p->s_buff + MEMSIZE - blocksize) {
    
    p->s_ptr  += blocksize;
    p->dr_ptr += blocksize;

  } else { /* Otherwise wrap around to the beginning of the aligned buffer */

    p->s_ptr  = p->s_buff;
    p->dr_ptr = p->dr_buff;
  }
}

void AdvanceRecvPtr(ArgStruct* p, int blocksize)
{
  /* Move the send buffer pointer forward if there is room */

  if(p->r_ptr + blocksize < p->r_buff + MEMSIZE - blocksize) {
    
    p->r_ptr  += blocksize;
    p->ds_ptr += blocksize;

  } else { /* Otherwise wrap around to the beginning of the aligned buffer */

    p->r_ptr  = p->r_buff  + p->roffset;
    p->ds_ptr = p->ds_buff + p->roffset;
  }
}

void SaveRecvPtr(ArgStruct* p)
{
  p->r_ptr_saved = p->r_ptr;
}

void ResetRecvPtr(ArgStruct* p)
{
  p->r_ptr = p->r_ptr_saved;
}

/* Set p->expected to 1 - p->tr
 * The first, last, and expected will be incremented by 2 % 128 for
 * each subsequent message by SetExpected and UpdateExpected.
 */

void InitBufferData(ArgStruct *p, int nbytes)
{
   memset(p->s_buff, -2, nbytes+p->soffset);

   memset(p->r_buff, -3, nbytes+MAX(p->soffset,p->roffset));

  /* If using cache mode, then we need to initialize the last byte
   * to the proper value since the transmitter and receiver are waiting
   * on different values to determine when the message has completely
   * arrive.
   */   

/*  if(!p->cache || p->bidir)*/

   p->expected = 1 - p->tr;
}

void SetExpected(ArgStruct *p)
{
   int set_expected;

   if( integCheck ) {

      SetIntegrityData(p);

   }

      /* Now set first and last bytes */

   set_expected = p->tr ? p->expected + 1 : p->expected - 1;

   p->s_ptr[0] = set_expected;

   p->s_ptr[p->bufflen-1] = set_expected;

   /* XXX Special case if streaming? Probably need to inc expected here */
}

void CheckBufferData(ArgStruct *p, int trial, int repeat)
{
#if defined (MEMCPY)
   unsigned char* buf = (unsigned char *) p->s_ptr;
   unsigned char val = (unsigned char) p->expected + 1;
#else
   unsigned char* buf = (unsigned char *) p->r_ptr;
   unsigned char val = (unsigned char) p->expected;
#endif

   if( integCheck ) {

      VerifyIntegrity( p );

   } else {

      if(buf[0] != val || buf[p->bufflen-1] != val) failures[p->myproc] += 1.0;

/*      if(buf[0] != val ) {*/
/*         fprintf(stderr, "First byte is %u, expected %u\n", buf[0], val);*/
/*         fprintf(stderr, "Trial %d, repeat %d\n", trial, repeat);*/
/*         exit(-1);*/
/*      } else if(buf[p->bufflen-1] != val) {*/
/*         fprintf(stderr, "Last byte is %u, expected %u\n", buf[p->bufflen-1], val);*/
/*         fprintf(stderr, "Trial %d, repeat %d\n", trial, repeat);*/
/*         exit(-1);*/
/*      }*/

   }

      /* Reset 1st and last to -1 to prevent the spread of garbage */

   buf[0] = buf[p->bufflen-1] = -1;

}

void UpdateExpected(ArgStruct *p)
{
   p->expected = (p->expected + 2) % 128;
}

void SwapSendRecvPtrs(ArgStruct *p)
{
      /* This function should only be called when we're doing bi-directional
       * cache mode, i.e. distinct send and receive buffers on each node, of
       * size equal to the message size (plus some for alignment and offsets) */

   void* tmp_ptr;
   
      /* Remove offsets before flip-flop */
   
   p->s_ptr -= p->soffset;
   p->r_ptr -= p->roffset;

      /* Flip-flop of buffer pointers */
   
   tmp_ptr = p->s_ptr;
   p->s_ptr = p->r_ptr;
   p->r_ptr = tmp_ptr;

      /* Add on offsets */
   
   p->s_ptr += p->soffset;
   p->r_ptr += p->roffset;

      /* Flip-flop aligned buffer pointers */
   
   tmp_ptr = p->s_buff;
   p->s_buff = p->r_buff;
   p->r_buff = tmp_ptr;

      /* Flip-flop original buffer pointers */

   tmp_ptr = p->s_buff_orig;
   p->s_buff_orig = p->r_buff_orig;
   p->r_buff_orig = tmp_ptr;

      /* Let the module do any other housekeeping it needs */

   ModuleSwapPtrs(p);
}

#if !defined(INFINIBAND) && !defined(ARMCI) && !defined(LAPI) && !defined(GPSHMEM) && !defined(SHMEM) && !defined(GM) && !defined(MPI2)

void MyMalloc(ArgStruct *p, int bufflen)
{
    if((p->r_buff=(char *)malloc(bufflen+MAX(p->soffset,p->roffset)))==(char *)NULL)
    {
        fprintf(stderr,"couldn't allocate memory for receive buffer\n");
        exit(-1);
    }

       /* Allocate second buffer if flushing cache or doing bidirectional mode */
    
    if(!p->cache || p->bidir) 
      if((p->s_buff=(char *)malloc(bufflen+MAX(p->soffset,p->roffset)))==(char *)NULL)
      {
          fprintf(stderr,"couldn't allocate memory for send buffer\n");
          exit(-1);
      }
}

void FreeBuff(char *buff1, char *buff2)
{
  if(buff1 != NULL)

   free(buff1);


  if(buff2 != NULL)

   free(buff2);
}

#endif

/* DoWork repeatedly does a daxpy on data that should be in cache.
 * It should be called once to pull the data into cache before any
 * timings are done.
 */
/*
void DoWork( int requested_flops, double *flops )
{
   static double x[NELEMENTS], y[NELEMENTS], a = 3.1415926;
   int i, j, repeats, nel;

   if( requested_flops/2 <= NELEMENTS ) {
      nel = requested_flops/2;
      repeats = 1;
   } else {
      repeats = (requested_flops/2 + NELEMENTS - 1) / NELEMENTS;
      nel = (requested_flops / 2) / repeats;
   }

   for( i=0; i<repeats; i++ ) {

      for( j=0; j<nel; j++ ) {

         x[j] = a * x[j] + y[j];

   }  }

   *flops += 2 * repeats * NELEMENTS;
}
*/

/* Try a counter instead of a daxpy. This tests CPU load, but not the
 * load on the memory bandwidth. */

void DoWork( int requested_flops, double *flops )
{
   int i, sum = 0;;

   for( i=0; i<requested_flops; i++ ) {

      sum ++;

         /* prevent compilers from optimizing the loop away */

      if( sum % 2 == 3 ) printf("%d", sum);
   }

   *flops += 2 * requested_flops;
}

