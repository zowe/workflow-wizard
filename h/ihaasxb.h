#pragma pack(packed)

#ifndef asxb__
#define asxb__


struct asxb {
  union {
    double         /*  DL8 */ asxbegin;      /* -                BEGINNING OF ASXB */
    struct {
      char           /*  CL4 */ asxbasxb[4];   /* -               ACRONYM IN EBCDIC -ASXB- */
      void * __ptr32 /*  AL4 */ asxbftcb;      /* -                 POINTER TO FIRST TCB ON TCB QUEUE */
    };
  };
  void * __ptr32 /*  AL4 */ asxbltcb;      /* -                 POINTER TO LAST TCB ON TCB QUEUE */
  short int      /*  HL2 */ asxbtcbs;      /* -                 NUMBER TCB'S IN THE MEMORY */
  unsigned char  /*  BL1 */ asxbflg1;      /* -                 Flags                           @LDA */


  #define asxbhcrm 128                     /* -             Health Check AS resmgr set      @LDA */


  unsigned char  /*  BL1 */ asxbschd;      /* -                 SCHEDULER FLAG BYTE             @LAA */


  #define asxbswup 128                     /* -             INDICATES THAT SWA SHOULD BE WRITTEN */


  void * __ptr32 /*  AL4 */ asxbmpst;      /* -                 ADDRESS OF VTAM MEMORY PROCESS */
  void * __ptr32 /*  AL4 */ asxblwa;       /* -                 ADDRESS OF LWA                MDC016 */
  void * __ptr32 /*  AL4 */ asxbvfvt;      /* -                 POINTER TO INTERNAL VIRTUAL     @L2A */
  void * __ptr32 /*  AL4 */ asxbsaf;       /* -                 ROUTER RRCB ADDRESS             @D1A */
  void * __ptr32 /*  AL4 */ asxbihsa;      /* -                 POINTER TO INTERRUPT HANDLERS SAVE */
  int            /* 18FL4 */ asxbflsa[18]; /* -               SAVE AREA FOR ANY FIRST LEVEL BRANCH */
  void * __ptr32 /*  AL4 */ asxbomcb;      /* -                 POINTER TO OBJECT ACCESS METHOD @01C */
  void * __ptr32 /*  AL4 */ asxbspsa;      /* -                 POINTER TO LOCAL WORK/SAVE AREA */
  void * __ptr32 /*  AL4 */ asxbrsmd;      /* -                 POINTER TO LOCAL RSM DATA AREA */
  void * __ptr32 /*  AL4 */ asxbrctd;      /* -                 POINTER TO LOCAL RCT DATA AREA */
  void * __ptr32 /*  AL4 */ asxbdecb;      /* -                 DUMP TASK ECB                   @02C */
  void * __ptr32 /*  AL4 */ asxbousb;      /* -                 POINTER TO SYSTEM RESOURCES MANAGER */
  void * __ptr32 /*  AL4 */ asxbcrwk;      /* -                 CHECKPOINT/RESTART WORKAREA POINTER. */
  char           /*  CL16 */ asxbprg[16];  /* -              SVC PURGE I/O PARAMETER LIST  MDC003 */
  char           /*  CL8 */ asxbpswd[8];   /* -               USER'S LOGON PASSWORD.  IF BLANK, */
  void * __ptr32 /*  AL4 */ asxbsirb;      /* -                 ADDRESS OF SIRB FOR THIS ADDRESS */
  void * __ptr32 /*  AL4 */ asxbetsk;      /* -                 ADDRESS OF ERROR TASK FOR THIS */
  union {
    char           /*  CL24 */ asxbaeq[24];  /* -             QUEUE ANCHORS FOR EXIT EFFECTOR'S */
    struct {
      void * __ptr32 /*  AL4 */ asxbfiqe;      /* -                 POINTER TO FIRST IQE          MDC006 */
      void * __ptr32 /*  AL4 */ asxbliqe;      /* -                 POINTER TO LAST IQE           MDC007 */
      void * __ptr32 /*  AL4 */ asxbfrqe;      /* -                 POINTER TO FIRST RQE          MDC008 */
      void * __ptr32 /*  AL4 */ asxblrqe;      /* -                 POINTER TO LAST RQE           MDC009 */
      void * __ptr32 /*  AL4 */ asxbfsrb;      /* -                 ADDRESS OF FIRST SRB          MDC013 */
      void * __ptr32 /*  AL4 */ asxblsrb;      /* -                 ADDRESS OF LAST SRB           MDC014 */
    };
  };
  union {
    char           /*  CL8 */ asxbusr8[8];   /* 8-byte version of ASXBUSER      @04A */
    char           /*  CL7 */ asxbuser[7];   /* -               USER ID FOR WHICH THE JOB OR SESSION */
  };
  void * __ptr32 /*  AL4 */ asxbsenv;      /* -                 ADDRESS OF ACCESS CONTROL */
  void * __ptr32 /*  AL4 */ asxbsfrs;      /* Address of SSI function request @03C */
  union {
    double         /*  DL8 */ asxbr0d0;      /* Reserved as of z/OS 1.11        @LGA */
    double         /*  DL8 */ asxbnsdw_prezos11; /* DOUBLEWORD CONTAINING THE NSSA POOL */
    struct {
      void * __ptr32 /*  AL4 */ asxbnssa_prezos11; /* NSSA POOL.                      @LGC */
      int            /*  FL4 */ asxbnsct_prezos11; /* COUNT USED TO SYNCHRONIZE THE */
    };
  };
  union {
    int            /*  FL4 */ asxbcasw;      /* -                USED BY REGION CONTROL TASK */
    struct {
      unsigned char  /*  BL1 */ asxbcrb1;      /* -                 CANCEL/RCT BYTE 1  (MDC314) @ZA05360 */


      #define asxbpip 128                      /* -             SET BY RCT TO INDICATE PURGE (SVC */
      #define asxbtfd 64                       /* -             SET BY CANCEL TO INDICATE THAT ALL */


      unsigned char  /*  BL1 */ asxbcrb2;      /* -                 CANCEL/RCT BYTE 2  (MDC317) @ZA05360 */
      unsigned char  /*  BL1 */ asxbcrb3;      /* -                 CANCEL/RCT BYTE 3  (MDC318) @ZA05360 */
      unsigned char  /*  BL1 */ asxbcrb4;      /* -                 CANCEL/RCT BYTE 4  (MDC319) @ZA05360 */
    };
  };
  void * __ptr32 /*  AL4 */ asxbpt0e;      /* -                 POST EXIT QUEUE HEADER */
  void * __ptr32 /*  AL4 */ asxbcapc;      /* -                 Count of task mode UCB capture  @L7A */
  void * __ptr32 /*  AL4 */ asxbjsvt;      /* -                 JES COMMUNICATION AREA POINTER. @D2A */
  void * __ptr32 /*  AL4 */ asxbdivw;      /* -                 ADDRESS OF THE DIV WORK/SAVE    @L4A */
  void * __ptr32 /*  AL4 */ asxbcapt;      /* -                 Pointer to IOS captured UCB     @L7A */
  void * __ptr32 /*  AL4 */ asxblinf;      /* -                 Latch information area          @L8A */
  void * __ptr32 /*  AL4 */ asxbpirl;      /* Pointer to queue of PIRLs. */
  void * __ptr32 /*  AL4 */ asxbitcb;      /* -                 Initial jobstep TCB address */
  void * __ptr32 /*  AL4 */ asxbrzvp;      /* -                 Address of RZV Control Table         */
  void * __ptr32 /*  AL4 */ asxbgrsp;      /* -                 Address of GRS control          @P4A */
  void * __ptr32 /*  AL4 */ asxbvasb;      /* Address of VASB.                     */
  double         /*  DL8 */ asxbalec;      /* AuthorizedLE Anchor             @LBA */
  union {
    double         /*  DL8 */ asxbifar;      /* Range                           @H1A */
    struct {
      void * __ptr32 /*  AL4 */ asxbfxrs;      /* Range Start                     @H1A */
      void * __ptr32 /*  AL4 */ asxbfxre;      /* Range End                       @H1A */
    };
  };
  void * __ptr32 /*  AL4 */ asxbexta;      /* Local exits                     @LCA */
  void * __ptr32 /*  AL4 */ asxbaxrl;      /* AXR local area */
  double         /*  DL8 */ asxb_mapreq_addr; /* MAPMVS tracking area address    @LHA */
  char           /*  XL216 */ asxbr128[216]; /* Reserved                        @06C */
  union {
    double         /*  DL8 */ asxbnsdw;      /* DOUBLEWORD CONTAINING THE NSSA POOL */
    struct {
      void * __ptr32 /*  AL4 */ asxbnssa;      /* NSSA POOL.                      @LGM */
      int            /*  FL4 */ asxbnsct;      /* COUNT USED TO SYNCHRONIZE THE */
    };
  };
  char           /*  XL248 */ asxbr208[248]; /* Reserved                        @LGA */
  double         /* 0DL8 */ asxbend[0];    /* -                END OF ASXB */
};


#endif  /* asxb__ */


#pragma pack(reset)


