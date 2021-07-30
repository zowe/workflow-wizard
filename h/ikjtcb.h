#pragma pack(packed)

#ifndef tcbfix__
#define tcbfix__

struct tcbfix {
  union {
    char           /*  CL32 */ tcbfrs[32];   /* - FLOATING POINT REGISTER SAVE AREA */
    struct {
      double         /*  DL8 */ tcbfrs0;       /* -     SAVE AREA FOR FLOATING POINT REGISTER 0 */
      double         /*  DL8 */ tcbfrs2;       /* -     SAVE AREA FOR FLOATING POINT REGISTER 2 */
      double         /*  DL8 */ tcbfrs4;       /* -     SAVE AREA FOR FLOATING POINT REGISTER 4 */
      double         /*  DL8 */ tcbfrs6;       /* -     SAVE AREA FOR FLOATING POINT REGISTER 6 */

      #define tcbpxlen 32                      /* LENGTH OF PREFIX SECTION */
      #define tcb 32                           /* -     TCBPTR */

    };
  };
  void * __ptr32 /*  AL4 */ tcbrbp;        /* -     ADDRESS OF THE RB FOR EXECUTING PROGRAM.  THIS */
  void * __ptr32 /*  AL4 */ tcbpie;        /* -     Address of current PIE/EPIE.  This field    @P8C */
  void * __ptr32 /*  AL4 */ tcbdeb;        /* -     ADDRESS OF THE DEB QUEUE */
  void * __ptr32 /*  AL4 */ tcbtio;        /* -     ADDRESS OF THE TASK I/O TABLE (TIOT) */
  union {
    int            /*  BL4 */ tcbcmp;        /* -  TASK COMPLETION CODE AND INDICATORS */
    struct {
      unsigned char  /*  BL1 */ tcbcmpf;       /* -     INDICATOR FLAGS */

      #define tcbcreq 128                      /* - A DUMP HAS BEEN REQUESTED */
      #define tcbcstep 64                      /* - A STEP ABEND HAS BEEN REQUESTED */
      #define tcbcpp 32                        /* - SOME PROBLEM PROGRAM STORAGE WAS OVERLAID BY THE */
      #define tcbdmpo 32                       /* - DUMP OPTIONS WERE PROVIDED ON CALLRTM OR SETRP */
      #define tcbstcc 16                       /* - COMPLETION CODE IS NOT TO BE STORED IN TCBCMPC */
      #define tcbnocc 16                       /* - A COMPLETION CODE WAS NOT PROVIDED ON CALLRTM */
      #define tcbcdbl 8                        /* - A DOUBLE ABEND HAS OCCURRED (OS/VS1) */
      #define tcbcasid 8                       /* - ABEND WAS SCHEDULED VIA CROSS MEMORY ABTERM */
      #define tcbcwto 4                        /* - A DUMP MESSAGE (WTO) IS TO BE ISSUED TO THE */
      #define tcbrv316 4                       /* - INDICATES REASON CODE (TCBARC) IS VALID */
      #define tcbcind 2                        /* - ABEND TO OUTPUT AN INDICATIVE DUMP  (OS/VS1) */
      #define tcbcmsg 1                        /* - AN ABEND MESSAGE IS PROVIDED TO BE PRINTED BY */

      unsigned int   /*  BL3 */ tcbcmpc:24;    /* -   SYSTEM COMPLETION CODE IN FIRST 12 BITS, */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ tcbtrn;        /* -    ADDRESS OF TESTRAN CONTROL CORE TABLE */
    struct {
      unsigned char  /*  BL1 */ tcbabf;        /* -     FLAG BYTE */

      #define tcbmod91 128                     /* - BOTH TESTRAN AND DECIMAL SIMULATOR ON A MOD 91 */
      #define tcbnochk 64                      /* - SUPPRESS TAKING CHECKPOINTS FOR THIS STEP */
      #define tcbgrph 32                       /* - GAM/SP ACTIVE FOR THIS TASK */
      #define tcb_refrprot_override 16         /* - Even if the REFRPROT option is */
      #define tcbtcpp 8                        /* - TCAM POST-PENDING  (RORI) */
      #define tcbtcp 4                         /* - TEST TASK - USED BY TEST SVC */
      #define tcboltep 2                       /* - OLTEP FUNCTIONS REQUIRE CLEANUP BEFORE ABNORMAL */
      #define tcbdfrbp 1                       /* - Issue SVC 61 upon fetch. Set only when      @P9A */

      unsigned int   /*  RL3 */ tcbtrnb:24;    /* -   ADDRESS OF TESTRAN CONTROL CORE TABLE */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ tcbmss;        /* -    ADDRESS OF LAST SPQE ON MSS QUEUE */
    struct {
      char           __filler0[1];
      unsigned int   /*  RL3 */ tcbmssb:24;    /* -   SAME AS TCBMSS                              @L6C */
    };
  };
  unsigned char  /*  BL1 */ tcbpkf;        /* -     STORAGE PROTECTION KEY FOR THIS TASK.  IF THERE */

  #define tcbflag 240                      /* - STORAGE PROTECTION KEY */
  #define tcbzero 15                       /* - MUST BE ZERO */

  union {
    unsigned char  /*  BL5 */ tcbflgs[5];    /* -  FLAG BYTE FIELDS */
    struct {
      unsigned char  /*  BL1 */ tcbflgs1;      /* -     FIRST TCB FLAG BYTE */

      #define tcbfa 128                        /* - May be on when a task is being abnormally   @LHC */
      #define tcbfe 64                         /* - On when the system is calling resource      @L6C */
      #define tcbfera 32                       /* - ENTER ABEND ERASE ROUTINE WHEN IN CONTROL AGAIN */
      #define tcbnonpr 16                      /* - TASK IS NON-PREEMPTABLE */
      #define tcbpdump 8                       /* - PREVENT DUMP INDICATOR */
      #define tcbft 4                          /* - TOP TASK IN TREE BEING ABTERMED */
      #define tcbfs 2                          /* - ABTERM DUMP COMPLETED */
      #define tcbfx 1                          /* - PROHIBIT QUEUEING OF ASYNCHRONOUS EXITS FOR */

      unsigned char  /*  BL1 */ tcbflgs2;      /* -     SECOND FLAG BYTE */

      #define tcbfoinp 128                     /* - THE TASK IS ABENDING AND IS IN THE PROCESS OF */
      #define tcbfsti 64                       /* - SECOND JOB STEP INTERVAL HAS EXPIRED */
      #define tcbfabop 32                      /* - IF 1, THE SYSABEND DUMP DATA SET HAS BEEN OPENED */
      #define tcbfsmc 16                       /* - TASK HAS ISSUED A SYSTEM-MUST-COMPLETE AND SET */
      #define tcbfjmc 8                        /* - TASK HAS ISSUED A STEP-MUST-COMPLETE AND TURNED */
      #define tcbfdsop 4                       /* - SYSABEND OPEN FOR JOB STEP */
      #define tcbfetxr 2                       /* - ETXR TO BE SCHEDULED */
      #define tcbfts 1                         /* - THIS TASK IS A MEMBER OF A TIME-SLICED GROUP */

      unsigned char  /*  BL1 */ tcbflgs3;      /* -     THIRD FLAG BYTE.                        @G381P9A */

      #define tcbfsm 128                       /* - ALL PSW'S IN SUPERVISOR STATE */
      #define tcbrt1s 64                       /* - RTM1 HAS INVOKED SLIP FOR A TASK IN EUT MODE. */
      #define tcbabtrm 32                      /* - ABTERM BIT TO PREVENT MULTIPLE ABENDS */
      #define tcbfxset 16                      /* - TCBFX WAS SET BY STATUS MCSTEP              @04A */
      #define tcbkey9 4                        /* - TCB was attached using KEY=NINE and so is to be */
      #define tcbenqrm 2                       /* - ENQ/DEQ RESOURCE MANAGER HAS RECEIVED   @G381P9A */
      #define tcbdwsta 1                       /* - THIS TASK WAS DETACHED WITH STAE=YES OPTION */

      unsigned char  /*  BL1 */ tcbflgs4;      /* -     NONDISPATCHABILITY FLAGS */

      #define tcbndump 128                     /* - ABDUMP NONDISPATCHABILITY INDICATOR */
      #define tcbser 64                        /* - SER1 NONDISPATCHABILITY INDICATOR */
      #define tcbrqena 32                      /* - I/O RQE'S EXHAUSTED */
      #define tcbhndsp 16                      /* - TASK OR JOB STEP IS MOMENTARILY 'FROZEN' UNTIL */
      #define tcbuxndv 8                       /* - TASK IS TEMPORARILY NONDISPATCHABLE BECAUSE */
      #define tcbrbwf 4                        /* - TOP RB IS IN WAIT STATE                 @G50DP9A */
      #define tcbondsp 1                       /* - TASK TERMINATING AND NONDISPATCHABLE BECAUSE */

      unsigned char  /*  BL1 */ tcbflgs5;      /* -     MORE NONDISPATCHABILITY FLAGS.   IF ANY BIT IN */

      #define tcbfc 128                        /* - TASK TERMINATED */
      #define tcbabwf 64                       /* - ABNORMAL WAIT */
      #define tcbuxndf 64                      /* - TASK IS TEMPORARILY NONDISPATCHABLE BECAUSE */
      #define tcbpage 32                       /* - TASK IS NONDISPATCHABLE DUE TO EXCESSIVE PAGING */
      #define tcbandsp 16                      /* - TASK IS TEMPORARILY NONDISPATCHABLE BECAUSE IT */
      #define tcbsys 8                         /* - ANOTHER TASK IS IN SYSTEM-MUST-COMPLETE STATUS */
      #define tcbstp 4                         /* - ANOTHER TASK IN THIS JOB STEP IS IN */
      #define tcbfcd1 2                        /* - INITIATOR WAITING FOR REGION */
      #define tcbpndsp 1                       /* - PRIMARY NONDISPATCHABILITY BIT.   THIS BIT IS */

    };
  };
  unsigned char  /*  FL1 */ tcblmp;        /* -   TASK LIMIT PRIORITY */
  unsigned char  /*  FL1 */ tcbdsp;        /* -   DISPATCHING PRIORITY FOR THIS TASK */
  void * __ptr32 /*  AL4 */ tcblls;        /* -     ADDRESS OF LAST LOAD LIST ELEMENT (LLE) IN LOAD */
  void * __ptr32 /*  AL4 */ tcbjlb;        /* -     ADDRESS OF A JOBLIB DCB */
  union {
    void * __ptr32 /*  AL4 */ tcbjpq;        /* -    ADDRESS OF LAST CDE FOR JOB PACK AREA (JPA) */
    struct {
      unsigned char  /*  BL1 */ tcbpurge;      /* -     HIGH ORDER BYTE                         @G860PXB */

      #define tcbjpqf 128                      /* - JPQ PURGE FLAG */

      unsigned int   /*  RL3 */ tcbjpqb:24;    /* -   LOW ORDER 24-BITS OF TCBJPQ             @G860PXB */
    };
  };
  union {
    char           /*  CL64 */ tcbgrs[64];   /* - GENERAL REGISTER SAVE AREA.  THIS OFFSET FIXED */
    struct {
      int            /*  FL4 */ tcbgrs0;       /* -     SAVE AREA FOR GENERAL REGISTER 0 */
      int            /*  FL4 */ tcbgrs1;       /* -     SAVE AREA FOR GENERAL REGISTER 1 */
      int            /*  FL4 */ tcbgrs2;       /* -     SAVE AREA FOR GENERAL REGISTER 2 */
      int            /*  FL4 */ tcbgrs3;       /* -     SAVE AREA FOR GENERAL REGISTER 3 */
      int            /*  FL4 */ tcbgrs4;       /* -     SAVE AREA FOR GENERAL REGISTER 4 */
      int            /*  FL4 */ tcbgrs5;       /* -     SAVE AREA FOR GENERAL REGISTER 5 */
      int            /*  FL4 */ tcbgrs6;       /* -     SAVE AREA FOR GENERAL REGISTER 6 */
      int            /*  FL4 */ tcbgrs7;       /* -     SAVE AREA FOR GENERAL REGISTER 7 */
      int            /*  FL4 */ tcbgrs8;       /* -     SAVE AREA FOR GENERAL REGISTER 8 */
      int            /*  FL4 */ tcbgrs9;       /* -     SAVE AREA FOR GENERAL REGISTER 9 */
      int            /*  FL4 */ tcbgrs10;      /* -     SAVE AREA FOR GENERAL REGISTER 10 */
      int            /*  FL4 */ tcbgrs11;      /* -     SAVE AREA FOR GENERAL REGISTER 11 */
      int            /*  FL4 */ tcbgrs12;      /* -     SAVE AREA FOR GENERAL REGISTER 12 */
      int            /*  FL4 */ tcbgrs13;      /* -     SAVE AREA FOR GENERAL REGISTER 13 */
      int            /*  FL4 */ tcbgrs14;      /* -     SAVE AREA FOR GENERAL REGISTER 14 */
      int            /*  FL4 */ tcbgrs15;      /* -     SAVE AREA FOR GENERAL REGISTER 15 */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ tcbfsa;        /* -    ADDRESS OF THE FIRST PROBLEM PROGRAM SAVE AREA */
    struct {
      char           __filler1[1];
      unsigned int   /*  RL3 */ tcbfsab:24;    /* -   ADDRESS OF THE FIRST PROBLEM PROGRAM SAVE AREA */
    };
  };
  void * __ptr32 /*  AL4 */ tcbtcb;        /* -     Queue of TCBs in an address space.          @L5C */
  void * __ptr32 /*  AL4 */ tcbtme;        /* -     ADDRESS OF THE TIMER QUEUE ELEMENT (TQE) */

  #define tcbtqet 128                      /* - IF ZERO, TASK TYPE TQE. */

  union {
    void * __ptr32 /*  AL4 */ tcbjstcb;      /* -    Address of job step TCB for this TCB */
    struct {
      char           __filler2[1];
      unsigned int   /*  RL3 */ tcbjstca:24;   /* -   LOW ORDER 24 BITS OF TCBJSTCB           @G860PXB */
    };
  };
  void * __ptr32 /*  AL4 */ tcbntc;        /* -     ADDRESS OF THE TCB FOR THE TASK PREVIOUSLY */
  void * __ptr32 /*  AL4 */ tcbotc;        /* -     ADDRESS OF THE TCB FOR THE TASK (THE ORIGINATING */
  void * __ptr32 /*  AL4 */ tcbltc;        /* -     ADDRESS OF THE TCB FOR THE TASK LAST ATTACHED BY */
  void * __ptr32 /*  AL4 */ tcbiqe;        /* -     ADDRESS OF AN INTERRUPTION QUEUE ELEMENT (IQE) */
  void * __ptr32 /*  AL4 */ tcbecb;        /* -     ADDRESS OF THE ECB THAT WILL BE POSTED BY THE */
  unsigned char  /*  BL1 */ tcbtsflg;      /* -     TIME SHARING FLAGS */

  #define tcbtstsk 128                     /* - SWAPPED TIME SHARING TASK (OS/VS1) */
  #define tcbstppr 64                      /* - TASK SHOULD BE MADE NONDISPATCHABLE VIA TCBSTPP */
  #define tcbatt 32                        /* - TASK SHOULD NOT HAVE ATTENTION EXITS SCHEDULED */
  #define tcbtiotg 16                      /* - PURGE TGET/TPUT AFTER ATTENTION */
  #define tcbsmcp 8                        /* - STATUS MCSTEP PENDING,TASK IS SUSPENDED     @L4A */
  #define tcbdydsp 2                       /* - M195 TASK IS MEMBER OF DYNAMIC DISPATCHING */
  #define tcbcpubn 1                       /* - FOR M195, ZERO MEANS I/O BOUND AND ONE MEANS */

  unsigned char  /*  FL1 */ tcbstpct;      /* -   NUMBER OF SETTASK STARTS WHICH MUST BE */
  unsigned char  /*  FL1 */ tcbtslp;       /* -   LIMIT PRIORITY OF TIME SHARING TASK */
  unsigned char  /*  BL1 */ tcbtsdp;       /* -     DISPATCHING PRIORITY OF TIME SHARING TASK */
  void * __ptr32 /*  AL4 */ tcbrd;         /* POINTER TO DPQE MINUS 8 FOR THE JOB STEP */

  #define tcbpqe 184                       /* SAME AS TCBRD                           @G860PVB */
  #define tcbpqe_length 4

  void * __ptr32 /*  AL4 */ tcbae;         /* LIST ORIGIN OF AQE(S) FOR THIS TASK */

  #define tcbaqe 188                       /* SAME AS TCBAE                           @G860PVB */
  #define tcbaqe_length 4

  union {
    void * __ptr32 /*  AL4 */ tcbstab;       /* -    ADDRESS OF THE CURRENT STAE CONTROL BLOCK */
    struct {
      unsigned char  /*  BL1 */ tcbnstae;      /* -     FLAGS INTERNAL TO STAE ROUTINE */

      #define tcbstabe 128                     /* - ABEND ENTERED BECAUSE OF ERROR IN STAE */
      #define tcbquies 64                      /* - STAE INVOKED PURGE I/O ROUTINE WITH QUIESCE I/O */
      #define tcb33e 32                        /* - A 33E ABEND HAS OCCURRED FOR TASK */
      #define tcbppsup 16                      /* - 1=SUPERVISOR MODE,0=PROBLEM PROGRAM MODE */
      #define tcbhalt 8                        /* - PURGE I/O ROUTINE DID NOT SUCCESSFULLY QUIESCE */
      #define tcbsynch 4                       /* - SYNCH ISSUED BY ASIR TO SCHEDULE EXIT ROUTINE */
      #define tcbstcur 1                       /* - STAE RECURSION VALID                      ICB443 */

      unsigned int   /*  RL3 */ tcbstabb:24;   /* -   ADDRESS OF THE CURRENT STAE CONTROL BLOCK */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ tcbtct;        /* -    ADDRESS OF THE TIMING CONTROL TABLE (TCT) IF */
    struct {
      unsigned char  /*  BL1 */ tcbtctgf;      /* -     FLAG BYTE FOR TIMING CONTROL TABLE        ICB318 */

      #define tcbsmfgf 128                     /* - IF ZERO, THE TCT CORE TABLE IS NOT TO BE */

      unsigned int   /*  RL3 */ tcbtctb:24;    /* -   ADDRESS OF THE TIMING CONTROL TABLE (TCT) IF */
    };
  };
  void * __ptr32 /*  AL4 */ tcbuser;       /* -     A WORD AVAILABLE TO THE USER */
  union {
    int            /*  BL4 */ tcbscndy;      /* -  SECONDARY NONDISPATCHABILITY BITS.   IF ANY BIT */
    int            /*  BL4 */ tcbndsp;       /* -  SAME AS TCBSCNDY */
    struct {
      unsigned char  /*  BL1 */ tcbndsp0;      /* -     BYTE 0 */

      #define tcbndjl 1                        /* Join/Leave processing                       @0EA */

      unsigned char  /*  BL1 */ tcbndsp1;      /* -     BYTE 1 */

      #define tcbdartn 128                     /* - THE TASK IS TEMPORARILY NONDISPATCHABLE - */
      #define tnondisp 128                     /* . ALIAS . */
      #define tcbdarpn 64                      /* - THE TASK IS PERMANENTLY NONDISPATCHABLE - */
      #define pnondisp 64                      /* . ALIAS . */
      #define tcbrstnd 32                      /* - THE TASK IS TEMPORARILY NONDISPATCHABLE - */
      #define tcbrspnd 16                      /* - THE TASK IS PERMANENTLY NONDISPATCHABLE - */
      #define tcbddrnd 8                       /* - THE TASK IS IN DEVICE ALLOCATION AND DYNAMIC */
      #define tcbtpsp 4                        /* - DISPATCHING OF TCAM TASK MUST BE DELAYED UNTIL */
      #define tcbpiend 2                       /* - SRB IS TO BE SCHEDULED TO PERFORM PIE/PICA */
      #define tcbabtin 1                       /* - THE TASK IS TEMPORARILY NONDISPATCHABLE WHILE */

      unsigned char  /*  BL1 */ tcbndsp2;      /* -     BYTE 2 */

      #define tcbabd 128                       /* - ABDUMP IS PROCESSING  (OS/VS1) */
      #define tcbstpp 64                       /* - TASK SET NONDISPATCHABLE BY SETTASK */
      #define tcbndsvc 32                      /* - TASK IS NONDISPATCHABLE BECAUSE SVC DUMP IS */
      #define tcbndts 16                       /* - TASK IS NONDISPATCHABLE BECAUSE IT IS BEING */
      #define tcbiwait 8                       /* - TASK IS NONDISPATCHABLE DUE TO AN INPUT WAIT */
      #define tcbowait 4                       /* - TASK IS NONDISPATCHABLE DUE TO AN OUTPUT WAIT */
      #define tcbdss 2                         /* - DYNAMIC SUPPORT SYSTEM (DSS) HAS SET THIS */
      #define tcbabe 1                         /* - ABEND ROUTINE WAS ENTERED FOR THIS TASK WHILE */

      unsigned char  /*  BL1 */ tcbndsp3;      /* -     BYTE 3 */

      #define tcbljsnd 128                     /* - TASK IS ABENDING AND NONDISPATCHABLE BECAUSE IT */
      #define tcbndnyi 64                      /* - RCT TASK IS NONDISPATCHABLE BECAUSE ADDRESS @P4A */
      #define tcbsrbnd 32                      /* - TCB NONDISPATCHABLE BECAUSE SRB'S ARE STOPPED */
      #define tcbslper 16                      /* - SET NONDISPATCHABLE SO THAT SLIP/PER CAN ALTER */
      #define tcbs3mr 8                        /* - STAGE 3 EXIT EFFECTOR MUST RUN TO SYNCHRONIZE */
      #define tcbareq 4                        /* - TSO AUTHORIZED REQUEST PROCESSING ACTIVE */
      #define tcbndsmf 2                       /* - Indicates task is stopped because its CPU   @03A */
      #define tcbndint 1                       /* - INITIATOR SETS THIS BIT TO PREVENT JOB STEP */

    };
  };
  int            /*  FL4 */ tcbmdids;      /* -     RESERVED FOR MODEL-DEPENDENT SUPPORT AND FOR */
  union {
    void * __ptr32 /*  AL4 */ tcbjscb;       /* -    ADDRESS OF THE JOB STEP CONTROL BLOCK */
    struct {
      unsigned char  /*  BL1 */ tcbrecde;      /* -     ABEND RECURSION BYTE                      ICB456 */

      #define tcbrec 128                       /* - VALID REENTRY TO ABEND IF NON-ZERO VALUE IN */
      #define tcbopen 1                        /* - OPEN DUMP DATA SET                        ICB456 */
      #define tcbclosd 2                       /* - CLOSE DIRECT SYSOUT ON TAPE               ICB456 */
      #define tcbclose 3                       /* - CLOSE OPEN DATA SETS                      ICB456 */
      #define tcbclosf 4                       /* - RESERVED.                                 ICB456 */
      #define tcbgrec 5                        /* - GRAPHICS                                  ICB456 */
      #define tcbadump 7                       /* - ABDUMP                                    ICB456 */
      #define tcbptaxe 8                       /* - PURGE TAXE                                ICB456 */
      #define tcbmesg 9                        /* - MESSAGE RECURSION                         ICB456 */
      #define tcbdynam 10                      /* - DD-DYNAM TIOT CLEANUP                     ICB456 */
      #define tcbdamsg 11                      /* - ABEND IS ISSUING A WTOR ASKING WHETHER THE JOB */
      #define tcbqtip 12                       /* - PURGE TSO INTERPARTITION POSTS            ICB456 */
      #define tcbtcamp 13                      /* - PURGE TCAM INTERPARTITION POSTS           ICB456 */
      #define tcbindrc 14                      /* - INDICATIVE DUMP (LOAD 8 OF ABEND) HAS ABENDED. */
      #define tcbsavcd 15                      /* - ASIR RECURSION.  SAVE OLD COMPLETION CODE ICB456 */
      #define tcbtyp1w 16                      /* - TYPE 1 MESSAGE WRITE TO PROGRAMMER        ICB456 */
      #define tcbwtpse 32                      /* - WRITE-TO-PROGRAMMER (WTP) FAILED.  JOB STEP */
      #define tcbvtam1 33                      /* - ABEND IS ENTERING FIRST VTAM INTERFACE, */
      #define tcbvtam2 34                      /* - ABEND IS ENTERING SECOND VTAM INTERFACE, */
      #define tcbvtam3 35                      /* - ABEND IS ENTERING FIRST VTAM INTERFACE, */
      #define tcbvtam4 36                      /* - ABEND IS ENTERING SECOND VTAM INTERFACE, */
      #define tcbnosta 48                      /* - STAE/STAI NOT TO BE HONORED               ICB456 */
      #define tcbstret 49                      /* - RETURN FROM DUMP PROCESSING               ICB456 */
      #define tcbconvr 50                      /* - CONVERT TO STEP ABEND                     ICB456 */
      #define tcbdaret 51                      /* - RETURN FROM DAMAGE ASSESSMENT ROUTINES    ICB456 */
      #define tcbtyp1r 52                      /* - RETURN FROM TYPE 1 MESSAGE MODULE         ICB456 */
      #define tcbnewrb 53                      /* - ABEND ISSUED SVC 13 TO TRANSFER CONTROL (XCTL) */
      #define tcbmccns 64                      /* - A MUST COMPLETE TASK HAS ABNORMALLY TERMINATED */

      unsigned int   /*  RL3 */ tcbjscbb:24;   /* -   ADDRESS OF THE JOB STEP CONTROL BLOCK */
    };
  };
  void * __ptr32 /*  AL4 */ tcbssat;       /* -     ADDRESS OF THE SUBSYSTEM AFFINITY TABLE @G381P9A */
  void * __ptr32 /*  AL4 */ tcbiobrc;      /* -     ADDRESS OF IOB RESTORE CHAIN FOR I/O QUIESCED */
  void * __ptr32 /*  AL4 */ tcbexcpd;      /* -     ADDRESS OF EXCP DEBUG AREA                YM4297 */
  union {
    void * __ptr32 /*  AL4 */ tcbext1;       /* -    ADDRESS OF OS-OS/VS COMMON TCB EXTENSION  ICB311 */
    struct {
      char           __filler3[1];
      unsigned int   /*  RL3 */ tcbext1a:24;   /* -   ADDRESS OF OS-OS/VS COMMON TCB EXTENSION  ICB311 */
    };
  };
  union {
    int            /*  BL4 */ tcbbits;       /* -  FLAG BYTES.  IF A BIT IN THE FOLLOWING TWO BYTES */
    struct {
      unsigned char  /*  BL1 */ tcbndsp4;      /* -     SECONDARY NONDISPATCHABILITY FLAGS COMMON TO */
      unsigned char  /*  BL1 */ tcbndsp5;      /* -     SECONDARY NONDISPATCHABILITY FLAGS UNIQUE TO */
      unsigned char  /*  BL1 */ tcbflgs6;      /* -     TASK-RELATED FLAGS */

      #define tcbrv 128                        /* - THE PARTITION IS FIXED IN REAL STORAGE.  VIRTUAL */
      #define tcbpie17 64                      /* - PAGE FAULT INTERRUPT IS TO BE PASSED TO THE */
      #define tcbcpu 32                        /* - TASK IS CPU-BOUND MEMBER OF AUTOMATIC PRIORITY */
      #define tcbspvlk 16                      /* - TASK SCHEDULED FOR ABTERM WHILE OWNING */
      #define tcbhcrm 8                        /* - Health Checker has established a task term */
      #define tcbmigr 4                        /* - REGION SELECTED FOR MIGRATION FROM PRIMARY */
      #define tcbapg 2                         /* - TASK IS IN AUTOMATIC PRIORITY GROUP (APG) */
      #define tcbntjs 1                        /* - JOB STEP TASK BUT NOT HIGHEST IN FAILING TREE */

      unsigned char  /*  BL1 */ tcbflgs7;      /* -     TASK-RELATED FLAGS */

      #define tcbgpecb 128                     /* - TASK IS IN AN ECB WAIT FOR A GETPART */
      #define tcbsenvp 64                      /* -                                             @LCA */
      #define tcbsvcsp 32                      /* - IF 1, SVC SCREENING IS TO BE PROPAGATED TO */
      #define tcbstack 16                      /* - SET IN JOB STEP TCB TO INDICATE THAT A TASK IN */
      #define tcbsvcs 8                        /* - IF 1, SVC SCREENING IS REQUIRED FOR THE TASK. */
      #define tcbrstsk 4                       /* - RESIDENT SYSTEM TASK */
      #define tcbadmp 2                        /* - ALL OTHER TASKS IN JOB STEP HAVE BEEN SET */
      #define tcbgtofm 1                       /* - GENERALIZED TRACE FACILITY (GTF) TRACING HAS */

    };
  };
  unsigned char  /*  BL1 */ tcbdar;        /* -     DAMAGE ASSESSMENT ROUTINE (DAR) FLAGS */

  #define tcbdarp 128                      /* - PRIMARY DAR RECURSION.  DAR HAS BEEN ENTERED FOR */
  #define tcbdars 64                       /* - SECONDARY DAR RECURSION.  IF DAR IS REENTERED, */
  #define tcbdard 32                       /* - A DUMP HAS BEEN REQUESTED FOR A WRITER OR */
  #define tcbdarc 16                       /* - RECURSION PERMITTED IN CLOSE - */
  #define tcbdarmc 16                      /* - DAR HAS BEEN ENTERED TO HANDLE A VALID RECURSION */
  #define tcbdaro 8                        /* - SYSTEM ERROR TASK IS FAILING.  DAR DUMP SHOULD */
  #define tcbdarwt 4                       /* - A WTO OPERATION WITH A 'REINSTATEMENT FAILURE' */
  #define tcbdarms 2                       /* - WTO OPERATION WITH A 'DAR IN PROGRESS' MESSAGE */
  #define tcbexsvc 1                       /* - THE DUMP SVC ROUTINE IS EXECUTING FOR THIS TASK */

  char           /*  XL1 */ tcbrsv37;      /* -     RESERVED FOR USER */
  unsigned char  /*  FL1 */ tcbsysct;      /* -   NUMBER OF OUTSTANDING SYSTEM-MUST-COMPLETE */
  unsigned char  /*  FL1 */ tcbstmct;      /* -   NUMBER OF OUTSTANDING STEP-MUST-COMPLETE */
  union {
    void * __ptr32 /*  AL4 */ tcbext2;       /* -    ADDRESS OF OS/VS1 - OS/VS2 COMMON EXTENSION */
    struct {
      char           __filler4[1];
      unsigned int   /*  RL3 */ tcbext2a:24;   /* -   ADDRESS OF OS/VS1 - OS/VS2 COMMON EXTENSION */
    };
  };
  int            /*  FL4 */ tcbaecb;       /* -     ABEND ECB.  POSTED BY A MOTHER TASK IN RTM2 */
  void * __ptr32 /*  AL4 */ tcbxsb;        /* -     ADDRESS OF CURRENT XSB FOR TASK.        @G381P9A */
  void * __ptr32 /*  AL4 */ tcbback;       /* -     ADDRESS OF PREVIOUS TCB ON READY QUEUE.  ZERO */
  void * __ptr32 /*  AL4 */ tcbrtwa;       /* -     POINTER TO CURRENT RTM2 WORK AREA */
  void * __ptr32 /*  AL4 */ tcbnssp;       /* -     NORMAL STACK SAVE AREA POINTER. */

  #define tcbnssqa 128                     /* - NORMAL STACK SAVED IN SQA INDICATOR.    @G38CPXB */

  void * __ptr32 /*  AL4 */ tcbxlas;       /* -     ASCB ADDRESS OF THE CML LOCK HELD WHILE @G381PXU */
  char           /*  CL1 */ tcbabcur;      /* -     ABEND RECURSION BYTE */
  unsigned char  /*  FL1 */ tcbfjmct;      /* -   NUMBER OF OUTSTANDING STEP-MUST-COMPLETE@YA02725 */
  char           /*  CL1 */ tcbtid;        /* -     The task identifier as specified on the     @P7C */

  #define tcbpagid 255                     /* -   ID FOR PAGING SUPERVISOR TASK             ICB403 */
  #define tcbsyerr 254                     /* -   ID FOR SYSTEM ERROR TASK                  ICB403 */
  #define tcbcomm 253                      /* -   ID FOR COMMUNCIATIONS TASK                ICB403 */
  #define tcbiorms 252                     /* -   ID FOR I/O RMS TASK                       ICB403 */
  #define tcbmastr 251                     /* -   ID FOR MASTER SCHEDULER TASK              ICB403 */
  #define tcbjes 250                       /* -   ID FOR JOB ENTRY SUBSYSTEM (JES) MONITOR TASK */
  #define tcbdssid 249                     /* -   ID FOR DYNAMIC SUPPORT SYSTEM (DSS) TASK  ICB403 */
  #define tcblogid 248                     /* -   ID FOR SYSTEM LOG TASK */

  unsigned char  /*  BL1 */ tcbflgs8;      /* -     GUPI FLAG BYTE FIELD                        @P1A */

  #define tcbdying 128                     /* - If on, indicates that this TCB will be      @0CC */
  #define tcbnoirb 64                      /* - If on, IRBs will not be queued to this TCB. @08A */
  #define tcbjtcba 32                      /* - If on, this is the attach of the JSTCB by the */
  #define tcbsdndx 16                      /* - If on, this task is exempt from being set */
  #define tcbendng 8                       /* - If on, indicates that this TCB will be      @0CA */
  #define tcbendingabnormally 4            /* - If On, indicates that this TCB is @LHA */
  #define tcbsiqeo 1                       /* - If on, during task termination, only IQEs set */

  union {
    int            /*  FL4 */ tcbxsct;       /* -    DISPATCHER INTERSECT CONTROL WORD       @Z40FP9A */
    struct {
      unsigned char  /*  BL1 */ tcbxsct1;      /* -     FLAG BYTE                               @Z40FP9A */

      #define tcbactiv 128                     /* - BIT ON MEANS THIS TCB IS CURRENTLY ACTIVE ON A */
      #define tcbs3a 64                        /* - STAGE 3 EXIT EFFECTOR/RESUME/TCTL INTERSECT FLAG */
      #define tcbllreq 32                      /* - TASK REQUESTED LOCAL LOCK               @G50DP9A */
      #define tcbtdorm 8                       /* - Turned on to detect dormant Tasks by        @06A */

      unsigned char  /*  BL1 */ tcbxsct2;      /* -     FLAG BYTE                               @G381P9A */

      #define tcbcmlf 128                      /* - CML RESOURCE MANAGER PROCESSING         @G381P9A */
      #define tcbllnew 64                      /* - Lock Manager has given this task the local  @L5A */

      short int      /*  HL2 */ tcbccpvi;      /* -     ID OF THE CURRENT CPU RUNNING THIS TASK.  USED */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ tcbfoe;        /* -    ADDRESS OF FIRST FIX OWNERSHIP ELEMENT (FOE) */
    struct {
      char           __filler5[1];
      unsigned int   /*  RL3 */ tcbfoea:24;    /* -   ADDRESS OF FIRST FIX OWNERSHIP ELEMENT (FOE) */
    };
  };
  void * __ptr32 /*  AL4 */ tcbswa;        /* -     ADDRESS OF FIRST SCHEDULER WORK AREA (SWA) SPQE */
  void * __ptr32 /*  AL4 */ tcbstawa;      /* -     ESTAE ROUTINE WORK AREA POINTER         @Z40FP9A */
  char           /*  CL4 */ tcbtcbid[4];   /* -   CONTAINS BLOCK ID - 'TCB ' */
  void * __ptr32 /*  AL4 */ tcbrtm12;      /* -     POINTER TO PARAMETER AREAS PASSED FROM RTM1 TO */
  union {
    char           /*  XL4 */ tcbestae[4];   /* -  AREA TO CONTAIN RECOVERY DATA FOR RTM */
    struct {
      char           /*  CL1 */ tcbscbky;      /* -     KEY IN WHICH SYNCH IS TO PASS CONTROL */
      unsigned char  /*  BL1 */ tcbestrm;      /* -     ESTAE TERM OPTIONS */

      #define tcbeterm 128                     /* - ESTAE EXIT ENTERED WITH TERM OPTION */
      #define tcbstafx 64                      /* - SERIALIZED BY TCB ACTIVE                @ZA30005 */

      unsigned char  /*  FL1 */ tcbertyp;      /* -   TYPE OF ERROR CAUSING ENTRY TO THE RTM.  SET */
      unsigned char  /*  FL1 */ tcbmode;       /* -   MASK INDICATING MODE OF SYSTEM AT TIME OF ERROR. */
    };
  };
  void * __ptr32 /*  AL4 */ tcbukysp;      /* -     ADDRESS OF SPQE'S FOR SUBPOOLS 229 AND 230 */
  union {
    char           /*  XL2 */ tcbpropf[2];   /* -  Flags propagated                           @L9C */
    struct {
      char           /*  XL1 */ tcbprop0;      /* -  Byte 0 of TCBPROPF                         @L9A */

      #define tcbbitcb 128                     /* - This task is "below" the initiator TCB     @L9A */

      char           /*  XL1 */ tcbprop1;      /* -  Byte 1 of TCBPROPF                         @L9A */
    };
  };
  short int      /*  BL2 */ tcbaffn;       /* -   CPU AFFINITY INDICATOR */
  unsigned char  /*  BL1 */ tcbfbyt1;      /* -     FLAG BYTE.                              @G381P9A */

  #define tcbeotfm 128                     /* - END OF TASK FLAG FOR FREEMAIN.  SET TO 1 BY TASK */
  #define tcbrtm1e 64                      /* - RTM1 IS CURRENTLY PROCESSING EUT FRR'S  @G381PXU */
  #define tcbndios 32                      /* - TASK HAS BEEN SET NONDISPATCHABLE VIA STATUSND */
  #define tcbpgnly 16                      /* - SET BY RTM2 TO INDICATE ONLY PURGE PHASE TO BE */
  #define tcbrtm2 8                        /* - SET BY RTM2 TO INDICATE RTM2 HAS BEEN ENTERED */
  #define tcbeot 4                         /* - SET BY RTM2 TO INDICATE TO EXIT THAT END OF TASK */
  #define tcbsattn 2                       /* - SYNCHRONIZATION OF ATTENTION INTERRUPT REQUIRED */
  #define tcbllh 1                         /* - Task was interrupted holding the local lock. */

  unsigned char  /*  BL1 */ tcbfbyt2;      /* -     FLAG BYTE.                              @G381P9A */

  #define tcbcncb 128                      /* - SET BY RTM2 IN THE JOB STEP TCB WHEN IT HAS BEEN */
  #define tcbfmw 64                        /* - MOTHER WAITING FLAG.  TURNED ON IN A SUBTASK IN */
  #define tcbfdw 32                        /* - DAUGHTER WAITING FLAG.  TURNED ON IN A MOTHER */
  #define tcbfprap 16                      /* - SET BY RTM2 TO PREVENT PERCOLATION TO THE TASK */
  #define tcbrt1nr 8                       /* - IF 1, ERROR PROPAGATED FROM RTM1 IS         @D1A */
  #define tcbecbnv 4                       /* - IF 1, ECB POINTED TO BY TCBECB IS NOT TO BE */
  #define tcbsspc 2                        /* - STATUS STOP PENDING,TASK HOLDS A CML LOCK */
  #define tcbrtm1c 1                       /* - A TASK WITH EUT FRRS HAS BEEN CANCELLED. THIS */

  unsigned char  /*  BL1 */ tcbfbyt3;      /* -     FLAG BYTE                               @ZA72701 */

  #define tcbexp 128                       /* - EXPANDED VERSION OF THE TCB             @ZA72701 */
  #define tcbnctl 64                       /* - IF 1, INDICATES USER PROGRAM IS LOADED  @ZA84893 */
  #define tcbrtmde 32                      /* - DETACH HAS BEEN CALLED BY RTM           @ZA93650 */
  #define tcbmtdp 16                       /* - MEMTERM SDUMP                               @LAA */

  unsigned char  /*  BL1 */ tcbfbyt4;      /* -     FLAG BYTE.                                  @L3A */

  #define tcbpmc 128                       /* - IF 1, INDICATES TASK IS IN PROCESS MUST     @D2C */
  #define tcbnojlb 64                      /* - If 1, indicates that TCBJLB is not to be used */
  #define tcbatskl 32                      /* - If 1, this task's TASKLIB was provided by   @LDA */
  #define tcb_keep_ls_extent_valid 8       /* - If 1, do not unchain and page @0DA */
  #define tcb_pthreadtermabnormal 4        /* - If 1, some z/OS Unix thread    @0FA */

  void * __ptr32 /*  AL4 */ tcbrpt;        /* -     ADDRESS OF RADIX PARTITION TREE FOR LOCAL */
  void * __ptr32 /*  AL4 */ tcbvat;        /* -     ADDRESS OF THE VAT (VSAM). THERE IS ONE @ZA63673 */
  void * __ptr32 /*  AL4 */ tcbswasa;      /* -     ADDRESS OF SAVE AREA USED BY SWA MANAGER */
  void * __ptr32 /*  AL4 */ tcbsvca2;      /* -     ADDRESS OF SVC SCREENING TABLE          @G50EP9A */
  void * __ptr32 /*  AL4 */ tcberd;        /* EXTENDED REGION DESCRIPTOR.             @G860PXU */
  void * __ptr32 /*  AL4 */ tcbeae;        /* EXTENDED ALLOCATED ELEMENT QUEUE        @G860PXU */
  void * __ptr32 /*  AL4 */ tcbarc;        /* REASON CODE FOR ABEND                   @G860PXB */
  int            /*  FL4 */ tcbgres;       /* -     TASK GLOBAL RESOURCE COUNT - NUMBER OF GLOBAL */
  void * __ptr32 /*  AL4 */ tcbstcb;       /* ADDRESS OF STCB                             @H2C */
  char           /*  CL8 */ tcbttime[8];   /* -  TCB'S ACCUMULATED CPU TIME                  @01A */
  void * __ptr32 /*  RL4 */ tcbcelap;      /* -  COMMON EXECUTION LIBRARY ANCHOR POINTER     @02A */
  char           /*  XL2 */ tcbr148[2];    /* -  RESERVED                                    @05C */
  unsigned char  /*  BL1 */ tcbrbyt1;      /* -     RTM Flag byte                               @05A */

  #define tcbpvict 128                     /* - If 1, indicates that the associated task    @05M */
  #define tcbpking 64                      /* - If 1, indicates that the associated Task has */
  #define tcbpcand 32                      /* - If 1, indicates that this Task has requested */
  #define tcbptop 16                       /* - If 1, indicates that this Task is at the top */

  unsigned char  /*  BL1 */ tcblevel;      /* -     LEVEL NUMBER OF TCB                     @ZA72701 */

  #define tcbvs02a 2                       /* - JBB2110 (NOT IN BASE)                   @ZA72701 */
  #define tcbvs02b 2                       /* - JBB2125                                 @ZA72701 */
  #define tcbvs03 3                        /* - JBB2133                                     @H1A */
  #define tcbvers 3                        /* - LEVEL OF THIS MAPPING                       @H1C */

  void * __ptr32 /*  AL4 */ tcbbdt;        /* -     ADDRESS OF BDT'S GSD LINKAGE            @ZA72701 */
  int            /*  FL4 */ tcbndaxp;      /* -     COUNT OF NUMBER OF CONSECUTIVE DISPATCHES   @H1A */
  void * __ptr32 /*  AL4 */ tcbsenv;       /* -     ADDRESS OF ACEE FOR THE TASK.               @H1A */

  #define tcbmnlen 344                     /* - LENGTH OF MAIN SECTION OF TCB */

};

#endif  /* tcbfix__ */

#ifndef tcbxtnt2__
#define tcbxtnt2__

struct tcbxtnt2 {
  union {
    void * __ptr32 /*  AL4 */ tcbgtf;        /* -    ADDRESS OF GENERALIZED TRACE FACILITY (GTF) */
    struct {
      unsigned char  /*  BL1 */ tcbtflg;       /* -     GTF FLAG BYTE                             ICB312 */

      #define tcbasync 128                     /* - GTF ASYNCHRONOUS GATHER ROUTINE IS IN CONTROL */
      #define tcberrtn 64                      /* - GTF ASYNCHRONOUS GATHER ERROR ROUTINE IS IN */
      #define tcbdspit 32                      /* - MACHINE CHECK INTERRUPTION HANDLER SHOULD */

      unsigned int   /*  RL3 */ tcbgtfa:24;    /* -   ADDRESS OF GTF TEMPORARY TRACE BUFFER     ICB312 */
    };
  };
  char           __filler0[1];
  unsigned int   /*  BL3 */ tcbrcmp:24;    /* -   MOST RECENT ABEND COMPLETION CODE (INCLUDING */
  void * __ptr32 /*  AL4 */ tcbevent;      /* -     ADDRESS OF EVENT TABLES QUEUE           @X31ED9A */
  int            /*  FL4 */ tcbrtmct;      /* -     COUNT OF TOKENS USED FOR ESTAE.         @G381PXU */
  void * __ptr32 /*  AL4 */ tcbtqe;        /* -     ADDRESS OF A REUSABLE TASK-RELATED TQE  @G50EP9A */
  void * __ptr32 /*  AL4 */ tcbcauf;       /* -     ADDRESS OF SUBSYSTEM FACILITY CONTROL BLOCK */
  void * __ptr32 /*  AL4 */ tcbpercp;      /* -     POINTER TO A QUEUE OF SPIS. AN SPI      @G381P9A */

  #define tcbrcvry 128                     /* - TASK IS IN RECOVERY.                    @G381P9A */

  int            /*  FL4 */ tcbperct;      /* -     COUNT OF SRB MODE FRRS WAITING TO       @G381P9A */

  #define tcbx2len 32                      /* LENGTH OF COMMON EXTENSION */
  #define tcblen 408                       /* - TCB LENGTH INCLUDING */

};

#endif  /* tcbxtnt2__ */


#pragma pack(reset)

