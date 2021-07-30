#pragma pack(packed)

#ifndef ascb__
#define ascb__

struct ascb {
  union {
    double         /*  DL8 */ ascbegin;      /* -                BEGINNING OF ASCB */
    struct {
      char           /*  CL4 */ ascbascb[4];   /* -               ACRONYM IN EBCDIC -ASCB- */
      void * __ptr32 /*  AL4 */ ascbfwdp;      /* -                 ADDRESS OF NEXT ASCB ON ASCB READY */
    };
  };
  void * __ptr32 /*  AL4 */ ascbbwdp;      /* -                 ADDRESS OF PREVIOUS ASCB ON ASCB */
  void * __ptr32 /*  AL4 */ ascbltcs;      /* -                 TCB and preemptable-class SRB   @07C */
  union {
    double         /*  DL8 */ ascbr010;      /* Reserved as of z/OS 1.12        @LLA */
    double         /*  DL8 */ ascbsupc_prezos12; /* -          SUPERVISOR CELL FIELD           @LLC */
    struct {
      void * __ptr32 /*  AL4 */ ascbsvrb_prezos12; /* -          SVRB POOL ADDRESS.              @LLC */
      int            /*  FL4 */ ascbsync_prezos12; /* -           COUNT USED TO SYNCHRONIZE SVRB POOL. */
    };
  };
  void * __ptr32 /*  AL4 */ ascbiosp;      /* -                 POINTER TO IOS PURGE INTERFACE */
  union {
    char           /*  XL4 */ ascbwqlk[4];   /* WEB QUEUE LOCK WORD             @L8A */
    struct {
      char           /*  XL2 */ ascbr01c[2];   /* RESERVED, MUST BE ZERO          @L8A */
      short int      /*  HL2 */ ascbwqid;      /* LOGICAL CPU ID OF THE PROCESSOR @L8A */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ ascbr020;      /* Reserved as of z/OS 1.11        @LJA */
    void * __ptr32 /*  AL4 */ ascbsawq_prezos11; /* -           ADDRESS OF ADDRESS SPACE SRB WEB */

    #define ascburrq_prezos11 128            /* -      SYSEVENT USER READY REQUIRED    @LJC */

  };
  union {
    short int      /*  HL2 */ ascbasn;       /* -                SAME AS ASCBASID                @L8A */
    short int      /*  HL2 */ ascbasid;      /* -                 ADDRESS SPACE IDENTIFIER FOR THE */
  };
  char           /*  XL1 */ ascbr026;      /* -                 RESERVED                        @LFC */
  char           /*  XL1 */ ascbsrmflags;  /* -                SRM flags */

  #define ascbvcmoverride 128              /* -      This bit indicates that this */
  #define ascbbrokenup 64                  /* -      This bit indicates that this */
  #define ascbvcmgivepreemption 32         /* -  This bit indicates that this */
  #define ascbvcmgivesigpany 16            /* -  This bit indicates that this */
  #define ascbinelighonorpriority 8        /* When on, specialty engine eligible */

  char           /*  XL1 */ ascbll5;       /* -                 FLAGS. SERIALIZATION - LOCAL    @D2A */

  #define ascbs3s 32                       /* -             STAGE II EXIT EFECTOR HAS       @D2M */

  unsigned char  /*  FL1 */ ascbhlhi;      /* -               INDICATION OF SUSPEND LOCKS     @L4C */
  union {
    short int      /*  HL2 */ ascbdph;       /* -                HALFWORD DISPATCHING PRIORITY   @L4A */
    struct {
      unsigned char  /*  FL1 */ ascbdphi;      /* -               HIGH ORDER BYTE OF HALFWORD     @L4A */
      unsigned char  /*  FL1 */ ascbdp;        /* -               DISPATCHING PRIORITY RANGE FROM */
    };
  };
  int            /*  FL4 */ ascbtcbe;      /* -               Count of ready tcbs in the      @LCC */
  void * __ptr32 /*  AL4 */ ascblda;       /* -                 POINTER TO LOCAL DATA AREA PART OF */
  unsigned char  /*  BL1 */ ascbrsmf;      /* -                 RSM ADDRESS SPACE FLAGS */

  #define ascb2lpu 128                     /* -             SECOND LEVEL PREFERRED USER.  THIS */
  #define ascb1lpu 64                      /* -             FIRST LEVEL PREFERRED USER */
  #define ascbn2lp 32                      /* -             SRM IN SYSEVENT TRANSWAP SHOULD NOT */
  #define ascbveqr 16                      /* -             V=R ADDRESS SPACE  (MDC372) @ZA17355 */

  char           /*  XL1 */ ascbflg3;      /* -               Flags needing no serialization  @LDA */

  #define ascbcnip 128                     /* -             Address space created during NIP */
  #define ascbreus 64                      /* -             This is a reusable ASID. It may be */

  union {
    short int      /*  HL2 */ ascbr036;      /* Reserved as of z/OS 1.11        @LJA */
    short int      /*  HL2 */ ascbhasi_prezos11; /* -           Local lock owning ASID.         @LJC */
  };
  void * __ptr32 /*  AL4 */ ascbcscb;      /* -                 ADDRESS OF CSCB */
  void * __ptr32 /*  AL4 */ ascbtsb;       /* -                 ADDRESS OF TSB */
  double         /*  DL8 */ ascbejst;      /* -                 ELAPSED JOB STEP TIMING  UNSIGNED */
  double         /*  DL8 */ ascbewst;      /* -                 TIME OF DAY WHENEVER I-STREAM IS */
  int            /*  FL4 */ ascbjstl;      /* -                 CPU TIME LIMIT FOR THE JOB STEP */
  int            /*  FL4 */ ascbecb;       /* -                 RCT'S WORK ECB */
  int            /*  FL4 */ ascbubet;      /* -                 TIME STAMP WHEN USER BECOMES READY */
  void * __ptr32 /*  AL4 */ ascbtlch;      /* -                 CHAIN FIELD FOR TIME LIMIT EXCEEDED */
  void * __ptr32 /*  AL4 */ ascbdump;      /* -                 SVC DUMP TASK TCB ADDRESS */
  union {
    int            /*  FL4 */ ascbfw1;       /* -                FULL-WORD LABEL TO BE USED FOR */
    struct {
      short int      /*  HL2 */ ascbaffn;      /* -                 CPU AFFINITY INDICATOR */
      unsigned char  /*  BL1 */ ascbrctf;      /* -                 FLAGS FOR RCT SERIALIZED BY COMPARE */

      #define ascbtmno 128                     /* -             MEMORY IS BEING QUIESCED, IS */
      #define ascbfrs 64                       /* -             RESTORE REQUEST */
      #define ascbfqu 32                       /* -             QUIESCE REQUEST */
      #define ascbjste 16                      /* -             JOB STEP TIME EXCEEDED. NOT USED BY */
      #define ascbwait 8                       /* -             LONG WAIT INDICATOR */
      #define ascbout 4                        /* -             ADDRESS SPACE CONSIDERED SWAPPED OUT */
      #define ascbtmlw 2                       /* -             MEMORY IS IN A LONG WAIT */
      #define ascbtoff 1                       /* -             MEMORY SHOULD NOT BE CHECKED FOR JOB */

      unsigned char  /*  BL1 */ ascbflg1;      /* -                 FLAG FIELD */

      #define ascblsas 128                     /* -             ADDRESS SPACE IS LOGICALLY SWAPPED */
      #define ascbdstk 64                      /* -             SRM REQUIRES A TIME STAMP TO    @P6C */
      #define ascbdstz 64                      /* -             Bit constant for bit position  @P7A */
      #define ascbterm 16                      /* -             ADDRESS SPACE TERMINATING NORMALLY */
      #define ascbabnt 8                       /* -             ADDRESS SPACE TERMINATING ABNORMALLY */
      #define ascbmemp 4                       /* -             Memory Termination PURGEDQ flag @LBA */

    };
  };
  int            /*  FL4 */ ascbtmch;      /* -                 TERMINATION QUEUE CHAIN */
  void * __ptr32 /*  AL4 */ ascbasxb;      /* -                 POINTER TO ADDRESS SPACE EXTENSION */
  union {
    int            /*  FL4 */ ascbfw2;       /* -                FULLWORD LABEL TO ADDRESS BITS IN */
    struct {
      short int      /*  HL2 */ ascbswct;      /* -                 NUMBER OF TIMES MEMORY ENTERS SHORT */
      unsigned char  /*  BL1 */ ascbdsp1;      /* -                 NONDISPATCHABILITY FLAGS. */

      #define ascbssnd 128                     /* -             SYSTEM SET NONDISPATCHABLE AND THIS */
      #define ascbfail 64                      /* -             A FAILURE HAS OCCURRED WITHIN THE */
      #define ascbsnqs 32                      /* -             STATUS STOP NON-QUIESCABLE LEVEL */
      #define ascbssss 16                      /* -             STATUS STOP SRB SUMMARY */
      #define ascbstnd 8                       /* -             TCB'S NONDISPATCHABLE */
      #define ascbuwnd 4                       /* -             STATUS SET UNLOCKED WORKUNITS   @LAA */
      #define ascbnoq 2                        /* -             ASCB NOT ON SWAPPED IN QUEUE    @L4A */

      unsigned char  /*  BL1 */ ascbflg2;      /* -                 FLAG BYTE. */

      #define ascbxmpt 128                     /* -             ASCB EXEMPT FROM SYSTEM */
      #define ascbpxmt 64                      /* -             ASCB PERMANENTLY EXEMPT FROM SYSTEM */
      #define ascbcext 32                      /* -             CANCEL TIMER EXTENSION BECAUSE EOT */
      #define ascbs2s 16                       /* -             FOR LOCK MANAGER, ENTRY MADE TO */
      #define ascbncml 8                       /* -             ASCB NOT ELIGIBLE FOR CML LOCK */
      #define ascbnomt 4                       /* -             ADDRESS SPACE MUST NOT BE MEMTERMED */
      #define ascbnomd 2                       /* -             IF ON,ADDRESS SPACE CANNOT BE */

    };
  };
  union {
    int            /*  FL4 */ ascbscnt;      /* -                FULLWORD LABEL FOR COMPARE AND SWAP */
    struct {
      char           __filler0[2];
      short int      /*  HL2 */ ascbsrbs;      /* -                 COUNT OF SRB'S SUSPENDED IN THIS */
    };
  };
  void * __ptr32 /*  AL4 */ ascbllwq;      /* -                 ADDRESS SPACE LOCAL LOCK        @04C */
  void * __ptr32 /*  AL4 */ ascbrctp;      /* -                 POINTER TO REGION CONTROL TASK (RCT) */
  union {
    double         /*  DL8 */ ascblkgp;      /* -                LOCK GROUP  (MDC306)        @Z40FP9A */
    struct {
      int            /*  FL4 */ ascblock;      /* -                 LOCAL LOCK.  THIS OFFSET FIXED BY */
      void * __ptr32 /*  AL4 */ ascblswq;      /* -                 ADDRESS SPACE LOCAL LOCK WEB    @L8C */

      #define ascbs3nl 128                     /* -             THE LOCAL LOCK IS NEEDED BY THE @L8A */
      #define ascbltcl 1                       /* -            THE LOCAL LOCK IS NEEDED BY SOME */

    };
  };
  int            /*  FL4 */ ascbqecb;      /* -                 QUIESCE ECB */
  int            /*  FL4 */ ascbmecb;      /* -                 MEMORY CREATE/DELETE ECB */
  void * __ptr32 /*  AL4 */ ascboucb;      /* -                 SYSTEM RESOURCES MANAGER (SRM) USER */
  void * __ptr32 /*  AL4 */ ascbouxb;      /* -                 SYSTEM RESOURCES MANAGER (SRM) USER */
  union {
    int            /*  FL4 */ ascbfw2a;      /* -                FULLWORD LABEL TO ADDRESS       @DAA */
    struct {
      short int      /*  HL2 */ ascbfmct;      /* -                 RESERVED. ALLOCATED PAGE FRAME  @L6C */
      char           /*  XL1 */ ascblevl;      /* -                 LEVEL NUMBER OF ASCB            @D3A */

      #define ascbvs00 0                       /* -             HBB2102 (NOT IN BASE)       @ZA68643 */
      #define ascbvs01 1                       /* -             JBB2110                         @D3A */
      #define ascbvs02 2                       /* -             JBB2133                         @H1A */
      #define ascbvs03 3                       /* -             HBB4410                         @L7A */
      #define ascbvers 3                       /* -             LEVEL OF THIS MAPPING           @L7C */

      unsigned char  /*  BL1 */ ascbfl2a;      /* -                 FLAG BYTE.                      @DAA */

      #define ascbnopr 128                     /* -             NO PREEMPTION FLAG              @DAA */

    };
  };
  union {
    void * __ptr32 /*  AL4 */ ascbr09c;      /* Reserved as of z/OS 1.11        @LJA */
    void * __ptr32 /*  AL4 */ ascbhreq_prezos11; /* -           Local lock requestor address.   @LJC */
  };
  void * __ptr32 /*  AL4 */ ascbiqea;      /* -                 POINTER TO IQE FOR ATCAM */
  void * __ptr32 /*  AL4 */ ascbrtmc;      /* -                 ANCHOR FOR SQA SDWA QUEUE */
  char           /*  CL4 */ ascbmcc[4];    /* -               USED TO HOLD A MEMORY TERMINATION */
  void * __ptr32 /*  AL4 */ ascbjbni;      /* -                 POINTER TO JOBNAME FIELD FOR */
  void * __ptr32 /*  AL4 */ ascbjbns;      /* -                 POINTER TO JOBNAME FIELD FOR */
  union {
    int            /*  FL4 */ ascbsrq;       /* -                DISPATCHER SERIALIZATION REQUIRED */
    struct {
      unsigned char  /*  BL1 */ ascbsrq1;      /* -                 FIRST BYTE OF ASCBSRQ */

      #define ascbdsg4 128                     /* -             SIGNAL WAITING PROCESSORS WHEN */
      #define ascbdflt 64                      /* -             DEFAULT LOCAL INTERSECT */

      unsigned char  /*  BL1 */ ascbsrq2;      /* -                 SECOND BYTE OF ASCBSRQ */

      #define ascbdsg3 128                     /* -             SIGNAL WAITING PROCESSORS WHEN */
      #define ascbsrm1 2                       /* -             SYSTEM RESOURCE MANAGER (SRM) */
      #define ascbqver 1                       /* -             QUEUE VERIFICATION INTERSECTING */

      unsigned char  /*  BL1 */ ascbsrq3;      /* -                 THIRD BYTE OF ASCBSRQ */

      #define ascbdsg2 128                     /* -             SIGNAL WAITING PROCESSORS WHEN */
      #define ascbrcti 64                      /* -             REGION CONTROL TASK (RCT) */
      #define ascbtcbv 32                      /* -             TCB VERIFICATION INTERSECTING */
      #define ascbacha 16                      /* -             ASCB CHAP INTERSECTING */
      #define ascbmter 4                       /* -             MEMORY TERMINATION INTERSECTING */
      #define ascbmini 2                       /* -             MEMORY INITIALIZATION INTERSECTING */
      #define ascbcbve 1                       /* -             CONTROL BLOCK VERIFICATION */

      unsigned char  /*  BL1 */ ascbsrq4;      /* -                 FOURTH BYTE OF ASCBSRQ */

      #define ascbdsg1 128                     /* -             SIGNAL WAITING PROCESSORS WHEN */
      #define ascbdeta 64                      /* -             DETACH INTERSECTING */
      #define ascbatta 32                      /* -             ATTACH INTERSECTING */
      #define ascbrtm2 16                      /* -             RTM2 INTERSECTING  (MDC351) @G50DP9A */
      #define ascbrtm1 8                       /* -             RTM1 INTERSECTING  (MDC352) @G50DP9A */
      #define ascbchap 4                       /* -             CHAP INTERSECTING  (MDC353) @G50DP9A */
      #define ascbstat 2                       /* -             STATUS INTERSECTING */
      #define ascbpurd 1                       /* -             PURGEDQ INTERSECTING */

    };
  };
  void * __ptr32 /*  AL4 */ ascbvgtt;      /* -                 ADDRESS OF VSAM GLOBAL TERMINATION */
  void * __ptr32 /*  AL4 */ ascbpctt;      /* -                 ADDRESS OF PRIVATE CATALOG */
  short int      /*  HL2 */ ascbssrb;      /* -                 COUNT OF STATUS STOP SRB'S */
  unsigned char  /*  FL1 */ ascbsmct;      /* -               NUMBER OF OUTSTANDING STEP MUST */
  unsigned char  /*  BL1 */ ascbsrbm;      /* -                 MODEL PSW BYTE 0 USED BY SRB */

  #define ascbper 64                       /* -             PER BIT IN ASCBSRBM - ALSO USED TO */

  int            /*  FL4 */ ascbswtl;      /* -                 STEP WAIT TIME LIMIT          MDC029 */
  double         /*  DL8 */ ascbsrbt;      /* -                 ACCUMULATED SRB TIME          MDC030 */
  void * __ptr32 /*  AL4 */ ascbltcb;      /* -                 TCB and preemptable-class SRB   @07C */
  int            /*  FL4 */ ascbltcn;      /* -                 Count of TCB and preemptable-   @07A */
  int            /*  FL4 */ ascbtcbs;      /* -                 NUMBER OF READY TCB'S.          @L8A */
  int            /*  FL4 */ ascblsqt;      /* -                 NUMBER OF TCBS ON A LOCAL LOCK  @L8A */
  void * __ptr32 /*  AL4 */ ascbwprb;      /* -                 ADDRESS OF WAIT POST REQUEST BLOCK */
  union {
    int            /*  FL4 */ ascbsrdp;      /* -                SYSTEM RESOURCE MANAGER (SRM) */
    struct {
      unsigned char  /*  FL1 */ ascbndp;       /* -               NEW DISPATCHING PRIORITY */
      unsigned char  /*  FL1 */ ascbtndp;      /* -               NEW TIME SLICE DISPATCHING PRIORITY */
      unsigned char  /*  FL1 */ ascbntsg;      /* -               NEW TIME SLICE GROUP */
      unsigned char  /*  FL1 */ ascbiodp;      /* -               I/O PRIORITY (MDC374)       @G50IP9A */
    };
  };
  void * __ptr32 /*  AL4 */ ascbloci;      /* -                 LOCK IMAGE, ADDRESS OF ASCB */
  void * __ptr32 /*  AL4 */ ascbcmlw;      /* -                 ADDRESS OF THE WEB REPRESENTING @L8C */
  union {
    int            /*  FL4 */ ascbr0f0;      /* Reserved as of z/OS 1.12        @LLA */
    int            /*  FL4 */ ascbcmlc_prezos12; /* -           COUNT OF CML LOCKS HELD BY      @LLC */
  };
  union {
    int            /*  FL4 */ ascbssom;      /* -                SPACE SWITCH EVENT OWNER    @ZA58694 */
    struct {
      char           /*  XL3 */ ascbsso1[3];   /* -               SPACE SWITCH EVENT OWNER    @G381P9A */
      unsigned char  /*  BL1 */ ascbsso4;      /* -                 SPACE SWITCH EVENT OWNER    @G381P9A */

      #define ascbsssp 2                       /* -             SLIP/PER REQUESTED          @G381P9A */
      #define ascbssjs 1                       /* -             JOB STEP TERMINATION        @G381P9A */

    };
  };
  void * __ptr32 /*  AL4 */ ascbaste;      /* -                 VIRTUAL ADDRESS OF ADDRESS  @G381P9A */
  void * __ptr32 /*  AL4 */ ascbltov;      /* -                 VIRTUAL ADDRESS OF THE      @G381P9A */
  void * __ptr32 /*  AL4 */ ascbatov;      /* -                 VIRTUAL ADDRESS OF          @G381P9A */
  short int      /*  HL2 */ ascbetc;       /* -                 NUMBER OF ENTRY TABLES      @G381P9A */
  short int      /*  HL2 */ ascbetcn;      /* -                 NUMBER OF CONNECTIONS TO    @G381P9A */
  short int      /*  HL2 */ ascblxr;       /* -                 NUMBER OF LINKAGE INDEXES   @G381P9A */
  short int      /*  HL2 */ ascbaxr;       /* -                 NUMBER OF AUTHORIZATION     @G381P9A */
  void * __ptr32 /*  AL4 */ ascbstkh;      /* -                 ADDRESS OF LOCAL STACK POOL @G381P9A */
  char           /*  XL8 */ ascbr110[8];   /* Reserved.                       @LQC */
  void * __ptr32 /*  AL4 */ ascbjafbaddr;  /* -                Address of the JAFB             @LQA */
  void * __ptr32 /*  AL4 */ ascbxtcb;      /* -                 ADDRESS OF THE JOB STEP @G381P9A */
  union {
    int            /*  FL4 */ ascbfw3;       /* -                Fullword label to address bits in */
    struct {
      unsigned char  /*  BL1 */ ascbcs1;       /* -                 FIRST BYTE OF COMPARE AND */

      #define ascbxmet 128                     /* -             IF ONE, THE ADDRESS SPACE IS    @L3C */
      #define ascbxmec 64                      /* -             CROSS MEMORY ENTRY TABLES   @G381P9A */
      #define ascbxmpa 32                      /* -             IF ONE, THE ADDRESS SPACE IS    @L3A */
      #define ascbxmlk 16                      /* -             IF ONE, THE ADDRESS SPACE IS    @L3A */
      #define ascbpers 8                       /* -             COMMUNICATION BIT FOR       @G381P9A */
      #define ascbdter 4                       /* -             A DAT ERROR HAS OCCURRED    @G381P9A */
      #define ascbpero 2                       /* -             PER PROCESSING NEEDS TO BE      @L5A */
      #define ascbswop 1                       /* -             ADDRESS SPACE IS SWAPPED OUT    @L5A */

      unsigned char  /*  BL1 */ ascbcs2;       /* -                 SECOND BYTE OF COMPARE AND SWAP @P5A */

      #define ascbsas 128                      /* -             INDICATES THAT STORAGE          @P5M */
      #define ascbsmgr 64                      /* -             This space is or has been associated */
      #define ascbdtin 32                      /* -             This space is or has been associated */
      #define ascbxmnr 16                      /* -             The address space is */
      #define ascbsdbf 8                       /* -             A work unit in this address space */
      #define ascbnoft 4                       /* -  Set this to exempt all tasks in this address */
      #define ascbpo1m 2                       /* -             Set this to indicate that */
      #define ascbp1m0 1                       /* -             Set this to indicate that */

      char           /*  XL2 */ ascbr122[2];   /* -               RESERVED.                       @P5C */
    };
  };
  void * __ptr32 /*  AL4 */ ascbgxl;       /* -                 ADDRESS OF GLOBALLY LOADED MODULE */
  double         /*  DL8 */ ascbeatt;      /* -                 EXPENDED AND ACCOUNTED TASK TIME. */
  double         /*  DL8 */ ascbints;      /* -                 JOB SELECTION TIME STAMP. */
  union {
    int            /*  FL4 */ ascbfw4;       /* -                FULLWORD LABEL TO ADDRESS   @G381P9A */
    struct {
      unsigned char  /*  BL1 */ ascbll1;       /* -                 FIRST BYTE OF FLAGS.        @G381P9A */

      #define ascbsspc 128                     /* -             STATUS STOP TASKS PENDING   @G381P9A */

      unsigned char  /*  BL1 */ ascbll2;       /* -                 SECOND BYTE OF FLAGS.       @G381P9A */
      unsigned char  /*  BL1 */ ascbll3;       /* -                 THIRD BYTE OF FLAGS.        @G381P9A */
      unsigned char  /*  BL1 */ ascbll4;       /* -                 FOURTH BYTE OF FLAGS.       @G381P9A */

      #define ascbtyp1 2                       /* -             TYPE 1 SVC HAS CONTROL.  THIS OFFSET */

    };
  };
  void * __ptr32 /*  AL4 */ ascbrcms;      /* ADDRESS OF THE REQUESTED    @G381PXU */
  int            /*  FL4 */ ascbiosc;      /* -                 I/O SERVICE MEASURE.        @G381PXU */
  short int      /*  HL2 */ ascbpkml;      /* -              PKM OF LAST TASK DISPATCHED */
  short int      /*  HL2 */ ascbxcnt;      /* -                 EXCP COUNT FIELD.               @L1A */
  void * __ptr32 /*  AL4 */ ascbnsqa;      /* -                 ADDRESS OF THE SQA RESIDENT */
  void * __ptr32 /*  AL4 */ ascbasm;       /* -                 ADDRESS OF THE ASM HEADER.      @L2A */
  void * __ptr32 /*  AL4 */ ascbassb;      /* -                 POINTER TO ADDRESS SPACE        @D6C */
  void * __ptr32 /*  AL4 */ ascbtcme;      /* -                 POINTER TO TCXTB.               @D1A */
  union {
    void * __ptr32 /*  AL4 */ ascbgqir;      /* -                ISGQSCAN INFORMATION            @O3A */

    #define ascbgqab 128                     /* -             ISGQSCAN INFORMATION            @O3A */

    struct {
      char           __filler1[3];
      char           /*  XL1 */ ascbgqi3;      /* -               BYTE 3 OF ASCBGQIR              @O3A */

      #define ascbgqds 1                       /* -             ISGQSCAN INFORMATION            @O3A */

    };
  };
  int            /*  FL4 */ ascblsqe;      /* -                 Number of Enclave TCBs that are on */
  double         /*  DL8 */ ascbiosx;      /* -                 I/O service measure extended.   @0AC */
  char           /*  XL2 */ ascbr168[2];   /* -              RESERVED.                       @0AC */
  char           /*  XL2 */ ascbsvcn[2];   /* -              SVC Number for type-1 SVC */
  void * __ptr32 /*  AL4 */ ascbrsme;      /* -               POINTER TO RSM ADDRESS SPACE    @D5A */
  union {
    int            /*  FL4 */ ascbavm;       /* -                AVAILABILITY MANAGER ADDRESS    @01C */
    struct {
      unsigned char  /*  BL1 */ ascbavm1;      /* -                 FIRST BYTE OF ASCBAVM.          @01A */
      unsigned char  /*  BL1 */ ascbavm2;      /* -                 SECOND BYTE OF ASCBAVM.         @01A */
      short int      /*  HL2 */ ascbagen;      /* -                 AVM ASID REUSE GENERATION       @01A */
    };
  };
  int            /*  FL4 */ ascbarc;       /* -                 REASON CODE ON MEMTERM. */
  union {
    void * __ptr32 /*  AL4 */ ascbrsm;       /* -                ADDRESS OF RSM'S CONTROL BLOCK */
    void * __ptr32 /*  RL4 */ ascbrsma;      /* -               ADDRESS OF RSM'S CONTROL BLOCK */
  };
  int            /*  FL4 */ ascbdcti;      /* -              ACCUMULATED CHANNEL CONNECT TIME */
  double         /* 0DL8 */ ascbend[0];    /* -                END OF ASCB                     @L7C */
};

#endif  /* ascb__ */


#pragma pack(reset)

