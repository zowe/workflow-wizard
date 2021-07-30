#pragma pack(packed)

#ifndef cvtptrd__
#define cvtptrd__

struct cvtptrd {
  void * __ptr32 /* 0AL4 */ cvtptrd1[0];

  #define cvtptr 16                        /* -          ABSOLUTE ADDRESS OF POINTER TO CVT */

};

#endif  /* cvtptrd__ */

#ifndef cvt__
#define cvt__

struct cvt {

  #define cvtmap 0                         /* -           CVTPTR CONTENT POINTS HERE */

  void * __ptr32 /*  VL4 */ cvttcbp;       /* -  Address of PSATNEW.                   @PTC */
  void * __ptr32 /*  VL4 */ cvt0ef00;      /* - ADDRESS OF ROUTINE TO SCHEDULE */
  void * __ptr32 /*  VL4 */ cvtlink;       /* -  ADDRESS OF DCB FOR SYS1.LINKLIB DATA SET. */
  void * __ptr32 /*  VL4 */ cvtauscb;      /* - ADDRESS OF ASSIGN/UNASSIGN SERVICE    @H1A */
  void * __ptr32 /*  AL4 */ cvtbuf;        /* -        ADDRESS OF THE BUFFER OF THE RESIDENT */
  void * __ptr32 /*  VL4 */ cvtxapg;       /* -  ADDRESS OF I/O SUPERVISOR APPENDAGE VECTOR */
  void * __ptr32 /*  VL4 */ cvt0vl00;      /* - ADDRESS OF ENTRY POINT OF THE TASK */
  void * __ptr32 /*  VL4 */ cvtpcnvt;      /* - ADDRESS OF ENTRY POINT OF THE ROUTINE */
  void * __ptr32 /*  VL4 */ cvtprltv;      /* - ADDRESS OF ENTRY POINT OF THE ROUTINE */
  void * __ptr32 /*  VL4 */ cvtllcb;       /* -  ADDRESS OF THE LLCB.                  @P1M */
  void * __ptr32 /*  VL4 */ cvtlltrm;      /* - ADDRESS OF LLA'S MEMORY TERNIMATION   @P1M */
  void * __ptr32 /*  VL4 */ cvtxtler;      /* - ADDRESS OF ERROR RECOVERY PROCEDURE (ERP) */
  void * __ptr32 /*  AL4 */ cvtsysad;      /* -        UCB ADDRESS FOR THE SYSTEM RESIDENCE */
  void * __ptr32 /*  VL4 */ cvtbterm;      /* - ADDRESS OF ENTRY POINT OF THE ABTERM */
  int            /*  FL4 */ cvtdate;       /* -        CURRENT DATE IN PACKED DECIMAL */
  void * __ptr32 /*  VL4 */ cvtmslt;       /* -  ADDRESS OF THE MASTER COMMON AREA IN */
  void * __ptr32 /*  AL4 */ cvtzdtab;      /* - ADDRESS OF I/O DEVICE */
  void * __ptr32 /*  VL4 */ cvtxitp;       /* -  ADDRESS OF ERROR INTERPRETER ROUTINE */
  void * __ptr32 /*  VL4 */ cvt0ef01;      /* ENTRY POINT IN STAGE II EXIT EFFECTOR @02C */
  union {
    int            /*  FL4 */ cvtvprm;       /* VECTOR PARAMETERS                     @D5C */
    struct {
      short int      /*  HL2 */ cvtvss;        /* VECTOR SECTION SIZE                   @D5A */
      short int      /*  HL2 */ cvtvpsm;       /* VECTOR PARTIAL SUM NUMBER             @D5A */
    };
  };
  short int      /*  HL2 */ cvtexit;       /* -           AN SVC 3 INSTRUCTION.  EXIT TO        @PBA */
  short int      /*  HL2 */ cvtbret;       /* -           A BR 14 INSTRUCTION.  RETURN TO       @PBA */
  void * __ptr32 /*  VL4 */ cvtsvdcb;      /* - ADDRESS OF THE DCB FOR THE SYS1.SVCLIB */
  void * __ptr32 /*  VL4 */ cvttpc;        /* -   ADDRESS OF THE TIMER SUPERVISOR WORK AREA */
  union {
    int            /*  FL4 */ cvtflgcs;      /* -   Flags set by CS                       @0IA */
    struct {
      char           /*  XL1 */ cvtflgc0;      /* -   Flags                                 @0IA */

      #define cvtmulnf 128                     /* -   For users of IFAUSAGE, REQUEST=FUNCTIONxxx */

      char           /*  XL1 */ cvtflgc1;      /* -   Flags                                 @0IA */
      short int      /*  HL2 */ cvticpid;      /* -   IPL'ED CPU PHYSICAL ID            @ZA51978 */
    };
  };
  char           /*  CL4 */ cvtcvt[4];     /* -   CVT ACRONYM IN EBCDIC (EYE-CATCHER)   @PCA */
  void * __ptr32 /*  VL4 */ cvtcucb;       /* -  ADDRESS OF THE UNIT CONTOL MODULE (UCM) */
  void * __ptr32 /*  VL4 */ cvtqte00;      /* - ADDRESS OF THE TIMER ENQUEUE ROUTINE FOR */
  void * __ptr32 /*  VL4 */ cvtqtd00;      /* - ADDRESS OF THE TIMER DEQUEUE ROUTINE FOR */
  void * __ptr32 /*  AL4 */ cvtstb;        /* -   ADDRESS OF THE I/O DEVICE             @LDC */
  char           /*  XL1 */ cvtdcb;        /* -     OPERATING SYSTEM FOR                  @LGC */

  #define cvtmvse 128                      /* -       S/370-XA MODE EXECUTION           @G860PVB */
  #define cvt1sss 64                       /* -       OPTION 1 (PCP) SSS. ALSO, LANGUAGE    @H3C */
  #define cvt2sps 32                       /* -       OPTION 2 (MFT) SPS, OS/VS1, VSE       @PHC */
  #define cvt4ms1 16                       /* -       OPTION 4 (MVT) MS1, OS/VS2 */
  #define cvtosext 8                       /* -       INDICATOR THAT THE CVTOSLVL AREA IS   @LGA */
  #define cvt4mps 4                        /* -       MODEL 65 MULTIPROCESSING */
  #define cvt6dat 2                        /* -       DYNAMIC ADDRESS TRANSLATION BY CPU */
  #define cvtmvs2 1                        /* -       MULTIPLE MEMORY OPTION OF OS/VS2 IS */
  #define cvt8aos2 18                      /* - OS/VS2 SYSTEM */

  unsigned int   /*  RL3 */ cvtdcba:24;    /* - ADDRESS OF THE DCB FOR THE SYS1.LOGREC */
  int            /*  FL4 */ cvtsv76m;      /* -        SVC 76 MESSAGE COUNT FIELD (OS/VS2) */
  void * __ptr32 /*  VL4 */ cvtixavl;      /* - ADDRESS OF THE I/O SUPERVISOR'S FREELIST */
  void * __ptr32 /*  AL4 */ cvtnucb;       /* -        RESERVED                (MDCXXX)  @YA19285 */
  void * __ptr32 /*  VL4 */ cvtfbosv;      /* - ADDRESS OF PROGRAM FETCH ROUTINE  @G860P46 */
  void * __ptr32 /*  VL4 */ cvt0ds;        /* -   ADDRESS OF ENTRY POINT OF THE DISPATCHER */
  void * __ptr32 /*  VL4 */ cvtecvt;       /* -  POINTER TO THE EXTENDED CVT           @LJC */
  void * __ptr32 /*  AL4 */ cvtdairx;      /* -        ADDRESS OF THE 31- BIT ENTRY          @L4A */
  void * __ptr32 /*  VL4 */ cvtmser;       /* -  ADDRESS OF DATA AREA OF MASTER SCHEDULER */
  void * __ptr32 /*  VL4 */ cvt0pt01;      /* - ADDRESS OF BRANCH ENTRY POINT OF POST */
  void * __ptr32 /*  AL4 */ cvttvt;        /* -        ADDRESS OF TSO VECTOR TABLE       @ZA65725 */
  int            /*  FL4 */ cvt040id;      /* -        IFB040I WTO MESSAGE ID.               @09C */
  char           /*  XL4 */ cvtmz00[4];    /* - HIGHEST ADDRESS IN VIRTUAL STORAGE FOR */
  void * __ptr32 /*  AL4 */ cvt1ef00;      /* -        ADDRESS OF ROUTINE WHICH CREATES IRB'S */
  void * __ptr32 /*  AL4 */ cvtqocr;       /* -        GRAPHICS INTERFACE TASK (GFX) FIELD. */
  void * __ptr32 /*  VL4 */ cvtqmwr;       /* -  ADDRESS OF THE ALLOCATION COMMUNICATION */
  short int      /*  HL2 */ cvtsnctr;      /* -        SERIAL NUMBER COUNTER FOR ASSIGNING SERIAL */
  char           /*  XL1 */ cvtopta;       /* -     OPTION INDICATORS                     @LBC */

  #define cvtcch 128                       /* -       CHANNEL CHECK HANDLER (CCH) OPTION PRESENT */
  #define cvtapr 64                        /* -       ALTERNATE PATH RETRY (APR) OPTION PRESENT */
  #define cvtddr 32                        /* -       DYNAMIC DEVICE RECONFIGURATION (DDR) */
  #define cvtnip 16                        /* -       NIP IS EXECUTING */
  #define cvt121tr 4                       /* - DO NOT TRANSLATE EXCP V=R.        @ZA45816 */
  #define cvtascii 2                       /* -       ASCII TAPE PROCESSING IS GENERATED IN */
  #define cvtxpfp 1                        /* -       CPU HAS EXTENDED PRECISION FLOATING POINT */

  char           /*  XL1 */ cvtoptb;       /* -     MISCELLANEOUS FLAGS                   @LBC */

  #define cvtprot 128                      /* -       CPU HAS STORE PROTECTION FEATURE (OS/VS1) */
  #define cvtctims 64                      /* -       IF ON, HARDWARE HAS THE CLOCK COMPARATOR */
  #define cvttod 32                        /* -       CPU HAS TIME-OF-DAY CLOCK FEATURE */
  #define cvtnlog 16                       /* -       SYS1.LOGREC IS UNAVAILABLE FOR ERROR */
  #define cvtapthr 8                       /* -       NIP SETS THIS BIT TO 1 WHEN DEVICE TESTING */
  #define cvtfp 4                          /* -       CPU HAS FETCH PROTECTION FEATURE (OS/VS1) */
  #define cvtvs1a 2                        /* -       VS1 ASSIST IS AVAILABLE FOR USE (OS/VS1) */
  #define cvtvs1b 1                        /* -       VS1 ASSIST SUBSET IS AVAILABLE FOR USE */

  void * __ptr32 /*  VL4 */ cvtqcdsr;      /* - CDE SEARCH ROUTINE ADDRESS  (OS/VS2) */
  void * __ptr32 /*  VL4 */ cvtqlpaq;      /* - ADDRESS OF POINTER TO MOST RECENT ENTRY */
  void * __ptr32 /*  VL4 */ cvtenfct;      /* EVENT NOTIFICATION CONTROL TABLE */
  void * __ptr32 /*  AL4 */ cvtsmca;       /* -        ADDRESS OF THE SYSTEM MANAGEMENT CONTROL */
  void * __ptr32 /*  VL4 */ cvtabend;      /* -  ADDRESS OF SECONDARY CVT FOR ABEND IN EOT */
  void * __ptr32 /*  AL4 */ cvtuser;       /* -        A WORD AVAILABLE TO THE USER */
  void * __ptr32 /*  AL4 */ cvtmdlds;      /* -        RESERVED FOR MODEL-DEPENDENT SUPPORT */
  short int      /*  HL2 */ cvtqabst;      /* -           AN SVC 13 (ABEND) INVOCATION (OS/VS2) @PDA */
  short int      /*  HL2 */ cvtlnksc;      /* -           AN SVC  6 (LINK) INVOCATION           @PDA */
  void * __ptr32 /*  AL4 */ cvttsce;       /* -        ADDRESS OF THE FIRST TIME SLICE CONTROL */
  void * __ptr32 /*  VL4 */ cvtpatch;      /* - ADDRESS OF A 200-BYTE FE PATCH AREA */
  void * __ptr32 /*  VL4 */ cvtrms;        /* -   RECOVERY MANAGEMENT SUPPORT (RMS) */
  void * __ptr32 /*  AL4 */ cvtspdme;      /* -        SERVICE PROCESSOR DAMAGE MONITOR  @ZA57793 */
  void * __ptr32 /*  VL4 */ cvt0scr1;      /* - ADDRESS OF THE SECTOR CALCULATION ROUTINE */
  union {
    void * __ptr32 /*  AL4 */ cvtgtf;        /* -          GENERALIZED TRACE FACILITY (GTF) CONTROL */
    struct {
      unsigned char  /*  RL1 */ cvtgtfst;      /* -      GTF FLAG BYTES                      ICB312 */

      #define cvtgtfav 128                     /* -       IF ZERO, GTF NOT ACTIVE.  IF ONE, GTF */
      #define cvtspd 64                        /* -       SERVICE PROCESSOR DAMAGE.         @ZA57793 */
      #define cvtwspr 32                       /* -       WAITING FOR SERVICE PROCESSOR     @ZA57793 */
      #define cvtusr 4                         /* -       TRACE=USR SPECIFIED.  USER-REQUESTED TRACE */
      #define cvtrnio 2                        /* -       GTF IS ACTIVE AND TRACING RNIO EVENTS */

      unsigned int   /*  RL3 */ cvtgtfa:24;    /* -ADDRESS OF MAIN MONITOR CALL ROUTING */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ cvtaqavt;      /* -          ADDRESS OF THE FIRST WORD OF THE TCAM */
    struct {
      char           /*  XL1 */ cvttcmfg;      /* -       TCAM FLAGS */

      #define cvttcrdy 128                     /* -       TCAM IS READY TO ACCEPT USERS */
      #define cvtldev 64                       /* -       LOCAL DEVICE ATTACHED TO TCAM */
      #define cvtnwtcm 32                      /* -       MULTIPLE TCAM FEATURE ACTIVE.         @D1A */

      unsigned int   /*  RL3 */ cvtaqavb:24;   /* -      SAME AS CVTAQAVT ABOVE */
    };
  };
  char           /*  XL1 */ cvtflag5;      /* Flags, refreshed upon error, set */

  #define cvtvef 128                       /* Vector Extension Facility             @NUA */
  #define cvtz1 128                        /* Z1                                    @NUA */
  #define cvteec 64                        /* @0OA */

  char           /*  XL1 */ cvtflag6;      /* More flags                            @NUA */

  #define cvtsoled 128                     /* Solution Edition                      @NWA */

  char           /*  XL1 */ cvtflag7;      /* More flags                            @NUA */
  char           /*  XL1 */ cvtflag8;      /* More flags                            @NUA */
  void * __ptr32 /*  AL4 */ cvtsaf;        /* -        ADDRESS OF ROUTER VECTOR TABLE.       @L5A */
  void * __ptr32 /*  AL4 */ cvtext1;       /* - ADDRESS OF OS - OS/VS COMMON EXTENSION */
  void * __ptr32 /*  AL4 */ cvtcbsp;       /* -        ADDRESS OF ACCESS METHOD CONTROL BLOCK */
  union {
    void * __ptr32 /*  AL4 */ cvtpurg;       /* -          ADDRESS OF SUBSYSTEM PURGE ROUTINE  ICB330 */
    struct {
      char           __filler0[1];
      unsigned int   /*  RL3 */ cvtpurga:24;   /* -      ADDRESS OF SUBSYSTEM PURGE ROUTINE  ICB330 */
    };
  };
  char           /*  XL4 */ cvtamff[4];    /* -     RESERVED FOR ACCESS METHOD FLAGS    ICB436 */
  void * __ptr32 /*  VL4 */ cvtqmsg;       /* - ADDRESS OF INFORMATION TO BE      @G860P46 */
  union {
    void * __ptr32 /*  AL4 */ cvtdmsr;       /* -          SAME AS CVTDMSRA BELOW              ICB346 */
    struct {
      char           /*  XL1 */ cvtdmsrf;      /* -     OPEN/CLOSE/EOV FLAG BYTE.  SETTING BOTH */

      #define cvtsdump 128                     /* -       SET BY COREZAP.  WILL CAUSE AN SDUMP TO BE */
      #define cvtudump 64                      /* -       SET BY COREZAP.  WILL CAUSE AN ABEND DUMP */

      unsigned int   /*  RL3 */ cvtdmsra:24;   /* -      ADDRESS OF THE OPEN/CLOSE/EOV SUPERVISORY */
    };
  };
  void * __ptr32 /*  VL4 */ cvtsfr;        /* - ADDRESS OF SETFRR ROUTINE         @G383P9A */
  void * __ptr32 /*  VL4 */ cvtgxl;        /* - ADDRESS OF CONTENTS SUPERVISION   @G860P46 */
  void * __ptr32 /*  AL4 */ cvtreal;       /* -        ADDRESS OF THE VIRTUAL STORAGE    @G860P46 */
  void * __ptr32 /*  VL4 */ cvtptrv;       /* -  ADDRESS OF PAGING SUPERVISOR GENERAL  @L1C */
  void * __ptr32 /*  AL4 */ cvtihvp;       /* - POINTER TO IHV$COMM. INITIALIZED TO   @06C */
  void * __ptr32 /*  VL4 */ cvtjesct;      /* - ADDRESS OF JOB ENTRY SUBSYSTEM (JES) */
  char           /*  XL4 */ cvtrs12c[4];   /* -     RESERVED                              @LFC */
  int            /*  FL4 */ cvttz;         /* -        Difference between local time and UTC */
  void * __ptr32 /*  AL4 */ cvtmchpr;      /* -        ADDRESS OF MACHINE CHECK PARAMETER LIST */
  void * __ptr32 /*  AL4 */ cvteorm;       /* -        POTENTIAL REAL HIGH STORAGE ADDRESS.  @LBC */
  void * __ptr32 /*  VL4 */ cvtptrv3;      /* - ADDRESS OF PAGING SUPERVISOR ROUTINE  @L1A */
  void * __ptr32 /*  VL4 */ cvtlkrm;       /* ADDRESS OF CML LOCK RESOURCE      @G860P46 */
  union {
    void * __ptr32 /*  AL4 */ cvtapf;        /* -          SAME AS CVTAPFA BELOW               ICB360 */
    struct {
      char           __filler1[1];
      unsigned int   /*  RL3 */ cvtapfa:24;    /* ADDRESS OF BRANCH ENTRY POINT IN */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ cvtext2;       /* -          ADDRESS OF OS/VS1 - OS/VS2 COMMON */
    struct {
      char           __filler2[1];
      unsigned int   /*  RL3 */ cvtext2a:24;   /* SAME AS CVTEXT2 ABOVE               ICB330 */
    };
  };
  union {
    void * __ptr32 /*  AL4 */ cvthjes;       /* -          SAME AS CVTHJESA BELOW              ICB454 */
    struct {
      char           __filler3[1];
      unsigned int   /*  RL3 */ cvthjesa:24;   /* -      ADDRESS OF OPTIONAL JOB ENTRY SUBSYSTEM */
    };
  };
  union {
    char           /*  XL4 */ cvtrstw2[4];   /* -        STATUS DATA FOR RESTART FLIH      @ZA84941 */
    struct {
      char           /*  XL1 */ cvtrs150;      /* -     Reserved.  Was CVTRSTCP: */
      char           /*  XL1 */ cvtrstrs;      /* -     RESTART REASON.                   @ZA84941 */
      short int      /*  HL2 */ cvtrcp2b;      /* -        Logical CPU address of target of the */
    };
  };
  char           /*  CL8 */ cvtsname[8];   /* SYSTEM NAME FOR CURRENT SYSTEM. */
  void * __ptr32 /*  AL4 */ cvtgetl;       /* -        ADDRESS OF IKJGETL, TSO GET LINE ROUTINE */
  void * __ptr32 /*  VL4 */ cvtlpdsr;      /* - ADDRESS OF LINK PACK AREA (LPA) DIRECTORY */
  void * __ptr32 /*  VL4 */ cvtpvtp;       /* -  ADDRESS OF PAGE VECTOR TABLE */
  union {
    void * __ptr32 /*  AL4 */ cvtlpdia;      /* -          ADDRESS OF LINK PACK AREA (LPA) DIRECTORY */
    struct {
      char           /*  XL1 */ cvtdirst;      /* -       FLAG BYTE                         @ZA65734 */

      #define cvtdicom 128                     /* -       LPA DIRECTORY HAS BEEN INITIALIZED BY NIP */

      unsigned int   /*  RL3 */ cvtlpdir:24;   /* -      ADDRESS OF LINK PACK AREA (LPA) DIRECTORY */
    };
  };
  void * __ptr32 /*  AL4 */ cvtrbcb;       /* -        ADDRESS OF THE RECORD BUFFER'S        @LEC */
  char           /*  XL4 */ cvtrs170[4];   /* -     RESERVED                              @LFC */
  union {
    char           /*  CL4 */ cvtslida[4];   /* -        IDENTITY OF TCB CAUSING SUPERVISOR LOCK */
    struct {
      char           /*  XL1 */ cvtsylk;       /* -       SUPERVISOR LOCK.  ONLY ENABLED TASKS MAY */

      #define cvtsylks 255                     /* -       SET LOCK BYTE */
      #define cvtsylkr 0                       /* -       RESET LOCK BYTE */

      unsigned int   /*  RL3 */ cvtslid:24;    /* -      SAME AS CVTSLIDA ABOVE            @ZA65734 */
    };
  };
  union {
    int            /*  FL4 */ cvtflags;      /* -          SYSTEM GLOBAL FLAGS                   @0CC */
    struct {
      char           /*  XL1 */ cvtflag1;      /* -     FLAG BYTE                             @0CC */

      #define cvtrsmwd 128                     /* -       IF ONE REAL STORAGE MANAGER WINDOW    @L1A */
      #define cvtsvprc 64                      /* -       SERVICE PROCESSOR ARCHITECTURE        @H3C */
      #define cvtcuse 32                       /* -       CUSE. SET BY NIP                      @03A */
      #define cvtmvpg 16                       /* -       IF ONE, MOVEPAGE CAPABILITY IS        @LIA */
      #define cvtover 8                        /* -       SUBPOOL OVERRIDE IS SUPPORTED.        @LRA */
      #define cvtcstr 4                        /* -       IF ONE, CSTRING FACILITY IS PRESENT   @DCA */
      #define cvtsubsp 2                       /* -       IF ONE, SUBSPACE FACILITY IS PRESENT  @LVA */
      #define cvtkpar 1                        /* -       RESERVED FOR USE BY RTM ONLY.         @M2A */

      char           /*  XL1 */ cvtflag2;      /* -     FLAG BYTE                             @0CA */

      #define cvtcmpsc 128                     /* -       IF ONE, INDICATES PRESENCE OF MVS     @0CA */
      #define cvtcmpsh 64                      /* -       IF ONE, INDICATES PRESENCE OF CMPSC   @0CA */
      #define cvtsopf 32                       /* -       IF ONE, INDICATES PRESENCE OF THE     @M9A */
      #define cvtbfph 16                       /* -       If one, indicates presence of */
      #define cvtper2 8                        /* -       If one, indicates presence of */
      #define cvtiqd 4                         /* -       If one, indicates that Internal       @MRC */
      #define cvtalr 2                         /* -       If one, indicates ASN and LX Reuse    @MXA */
      #define cvtedat 1                        /* -       If one, indicates that the Enhanced DAT */

      char           /*  XL1 */ cvtflag3;      /* -     FLAG BYTE refreshed upon error, set */

      #define cvtesame 128                     /* - */
      #define cvtzarch 128                     /* -       If one, indicates presence of */
      #define cvtprocascore 64                 /* A processor resource is viewed */
      #define cvtmulticpuspercore 32           /* When CvtProcAsCore is on, this indicates */
      #define cvtcpuasaliastocore 16           /* When CvtProcAsCore is on, the term "CPU" */

      char           /*  XL1 */ cvtflag4;      /* -     Flag byte This is an interface for */

      #define cvtp001i 128                     /* If one, indicates that P001 support */
      #define cvtp001a 64                      /* If one, indicates that the system is */
      #define cvtznalc 32                      /* zNALC                                 @0JA */
      #define cvtdcpa 16                       /* Dynamic CPU Addition is enabled       @NAA */
      #define cvttx 8                          /* TX support is enabled                 @NMA */
      #define cvtp002 8                        /* P002 support is enabled               @NMA */
      #define cvttxc 4                         /* TXC support is enabled                @NMA */
      #define cvtp002c 4                       /* P002C support is enabled              @NMA */
      #define cvtri 2                          /* RI support is enabled                 @NNA */
      #define cvtedat2 1                       /* EDAT2 is enabled                      @NMA */

    };
  };
  void * __ptr32 /*  VL4 */ cvtrt03;       /* - ADDRESS OF SRB TIMING             @G383P9A */
  char           /*  XL8 */ cvtrs180[8];   /* - RESERVED                              @LOC */
  void * __ptr32 /*  VL4 */ cvtexsnr;      /* - ADDRESS OF EXCESSIVE SPIN NOTIFICATION */
  char           /*  XL1 */ cvtexsnl;      /* -     SERIALIZATION BYTE FOR EXCESSIVE SPIN */
  unsigned char  /*  RL1 */ cvtspvlk;      /* -      NUMBER OF TASKS WHICH HAVE TERMINATED */
  char           /*  XL1 */ cvtctlfg;      /* -     SYSTEM CONTROL FLAGS                  @LBC */

  #define cvttxte 128                      /* A Transactional Execution test environment */
  #define cvttxj 128                       /* Not a programming interface           @NMA */
  #define cvtdstat 16                      /* -       DEVSTAT OPTION IN EFFECT.  DEVICE ADDRESS */
  #define cvtdrmod 8                       /* -       Set on when DRMODE=YES was specified. @MZA */
  #define cvtnomp 4                        /* -       MULTIPROCESSING CODE IS NOT IN THE SYSTEM. */
  #define cvtgtrce 2                       /* -       GENERALIZED TRACE FACILITY (GTF) HAS */
  #define cvtsdtrc 1                       /* -       SVC DUMP HAS SUPPRESSED SUPERVISOR TRACE. */

  unsigned char  /*  FL1 */ cvtapg;        /* -      DISPATCHING PRIORITY OF AUTOMATIC PRIORITY */
  char           __filler4[4];
  void * __ptr32 /*  VL4 */ cvtrscn;       /* - ADDRESS OF ROUTINE TO SCAN TCB TREE */
  void * __ptr32 /*  AL4 */ cvttas;        /* -        ADDRESS OF ROUTINE TO TRANSFER ADDRESS */
  void * __ptr32 /*  AL4 */ cvttrcrm;      /* -        ADDRESS POINTER OF THE SYSTEM     @G860P46 */
  void * __ptr32 /*  AL4 */ cvtshrvm;      /* -        LOWEST ADDRESS OF SHARED VIRTUAL STORAGE */
  void * __ptr32 /*  VL4 */ cvt0vl01;      /* - ENTRY POINT ADDRESS OF VALIDITY CHECK */
  void * __ptr32 /*  VL4 */ cvtppgmx;      /* -  ADDRESS POINTER FOR MVS/370-XA.   @G860P46 */
  char           /*  XL1 */ cvtgrsst;      /* -     GRS status. SERIALIZATION: None.      @0DA */

  #define cvte51gn 128                     /* -       When on, global resource contention */
  #define cvte51ln 64                      /* -       When on, local resource contention */

  char           /*  XL1 */ cvtrs1ad;      /* -     Reserved                              @NJC */
  char           /*  XL2 */ cvtbsm0f[2];   /* -     Return via reg 15, BSM 0,15           @NJA */
  void * __ptr32 /*  VL4 */ cvtgvt;        /* -      ADDRESS OF THE GRS VECTOR TABLE   @G383P9A */
  void * __ptr32 /*  AL4 */ cvtascrf;      /* -        CREATED ASCB QUEUE HEADER */
  void * __ptr32 /*  AL4 */ cvtascrl;      /* -        CREATED ASCB QUEUE TRAILER */
  void * __ptr32 /*  AL4 */ cvtputl;       /* -        ADDRESS OF IKJPUTL, TSO PUT LINE ROUTINE */
  void * __ptr32 /*  VL4 */ cvtsrbrt;      /* - DISPATCHER RETURN ADDRESS FOR SRB ROUTINES */
  void * __ptr32 /*  VL4 */ cvtolt0a;      /* - BRANCH ENTRY TO OLTEP MEMORY TERMINATION */
  void * __ptr32 /*  VL4 */ cvtsmfex;      /* - BRANCH ENTRY TO SYSTEM MANAGEMENT */
  void * __ptr32 /*  AL4 */ cvtcspie;      /* -        ENTRY POINT ADDRESS OF THE SUPERVISOR */
  void * __ptr32 /*  AL4 */ cvtptgt;       /* -        ADDRESS OF IKJPTGT, TSO PUTGET ROUTINE */
  char           /*  XL1 */ cvtsigpt;      /* -     SIGP TIMEOUT VALUE.                   @07A */
  char           /*  XL1 */ cvtspdmc;      /* -     SERVICE PROCESSOR DAMAGE MACHINE  @ZA57793 */
  char           /*  XL1 */ cvtdssac;      /* -     DYNAMIC SUPPORT SYSTEM (DSS) ACTIVATED */
  char           /*  XL1 */ cvtrs1d7;      /* -     RESERVED                              @LFC */
  void * __ptr32 /*  AL4 */ cvtstck;       /* -        ADDRESS OF IKJSTCK, TSO STACK ROUTINE */
  short int      /*  HL2 */ cvtmaxmp;      /* -        Maximum CPU address available for */
  char           /*  XL2 */ cvtbsm2[2];    /* -     RETURN VIA REG 2, BSM 0,2.            @L4A */
  void * __ptr32 /*  AL4 */ cvtscan;       /* -        ADDRESS OF IKJSCAN, TSO SCAN ROUTINE */
  void * __ptr32 /*  AL4 */ cvtauthl;      /* -        POINTER TO AUTHORIZED LIBRARY TABLE.  @LWC */
  void * __ptr32 /*  VL4 */ cvtbldcp;      /* - BRANCH ENTRY TO BUILD POOL          MDC003 */
  void * __ptr32 /*  VL4 */ cvtgetcl;      /* - BRANCH ENTRY TO GET CELL            MDC004 */
  void * __ptr32 /*  VL4 */ cvtfrecl;      /* - BRANCH ENTRY TO FREE CELL           MDC005 */
  void * __ptr32 /*  VL4 */ cvtdelcp;      /* - BRANCH ENTRY TO DELETE POOL         MDC006 */
  void * __ptr32 /*  VL4 */ cvtcrmn;       /* - BRANCH ENTRY TO SVC 120 (GETMAIN/FREEMAIN */
  void * __ptr32 /*  VL4 */ cvtcras;       /* - POINTER DEFINED ADDRESS OF BRANCH  @G860PVB */
  void * __ptr32 /*  VL4 */ cvtqsas;       /* - POINTER DEFINED ADDRESS OF        @G860PVB */
  void * __ptr32 /*  VL4 */ cvtfras;       /* - POINTER DEFINED ENTRY TO TASK      @G860PVB */
  void * __ptr32 /*  VL4 */ cvts1ee;       /* - BRANCH ENTRY TO STAGE 1 EXIT EFFECTOR */
  void * __ptr32 /*  AL4 */ cvtpars;       /* -        ADDRESS OF IKJPARS, TSO PARSE ROUTINE */
  void * __ptr32 /*  VL4 */ cvtquis;       /* - BRANCH ENTRY TO QUIESCE             MDC013 */
  void * __ptr32 /*  AL4 */ cvtstxu;       /* -        BRANCH ENTRY TO ATTENTION EXIT EPILOGUE */
  void * __ptr32 /*  VL4 */ cvtopte;       /* - BRANCH ENTRY ADDRESS TO SYSEVENT    MDC015 */
  void * __ptr32 /*  AL4 */ cvtsdrm;       /* -        BRANCH ENTRY ADDRESS OF THE RESOURCE */
  void * __ptr32 /*  VL4 */ cvtcsrt;       /* - POINTER TO CALLABLE SERVICE REQUEST   @D9C */
  void * __ptr32 /*  VL4 */ cvtaqtop;      /* - POINTER TO THE ALLOCATION QUEUE   @G860P46 */
  void * __ptr32 /*  AL4 */ cvtvvmdi;      /* -        CONSTANT USED BY PAGED LINK PACK AREA */
  void * __ptr32 /*  AL4 */ cvtasvt;       /* -        POINTER TO ADDRESS SPACE VECTOR TABLE */
  void * __ptr32 /*  AL4 */ cvtgda;        /* -        POINTER TO GLOBAL DATA AREA (GDA) IN SQA */
  void * __ptr32 /*  VL4 */ cvtascbh;      /* - POINTER TO HIGHEST PRIORITY ADDRESS SPACE */
  void * __ptr32 /*  VL4 */ cvtascbl;      /* - POINTER TO LOWEST PRIORITY ASCB ON THE */
  void * __ptr32 /*  AL4 */ cvtrtmct;      /* -        POINTER TO RECOVERY/TERMINATION CONTROL */
  void * __ptr32 /*  VL4 */ cvtsv60;       /* - BRANCH ENTRY ADDRESS FOR              @D8C */
  void * __ptr32 /*  VL4 */ cvtsdmp;       /* - ADDRESS OF SVC DUMP BRANCH        @G860P46 */
  void * __ptr32 /*  VL4 */ cvtscbp;       /* - ADDRESS OF SCB PURGE RESOURCE     @G860P46 */
  char           /*  XL4 */ cvtsdbf[4];    /* - Address of 4K SQA buffer used by SVC */
  void * __ptr32 /*  AL4 */ cvtrtms;       /* -        ADDRESS OF SERVICABILITY LEVEL INDICATOR */
  void * __ptr32 /*  AL4 */ cvttpios;      /* -        ADDRESS OF THE TELEPROCESSING I/O */
  void * __ptr32 /*  AL4 */ cvtsic;        /* -        BRANCH ADDRESS OF THE ROUTINE TO SCHEDULE */
  void * __ptr32 /*  VL4 */ cvtopctp;      /* - ADDRESS OF SYSTEM RESOURCES MANAGER (SRM) */
  void * __ptr32 /*  VL4 */ cvtexpro;      /* - ADDRESS OF EXIT PROLOGUE/TYPE 1 EXIT */
  void * __ptr32 /*  VL4 */ cvtgsmq;       /* -  ADDRESS OF GLOBAL SERVICE MANAGER QUEUE */
  void * __ptr32 /*  VL4 */ cvtlsmq;       /* -  ADDRESS OF LOCAL SERVICE MANAGER QUEUE */
  char           /*  XL4 */ cvtrs26c[4];   /* -  RESERVED.                             @LZC */
  void * __ptr32 /*  VL4 */ cvtvwait;      /* - ADDRESS OF WAIT ROUTINE             MDC048 */
  void * __ptr32 /*  VL4 */ cvtparrl;      /* - ADDRESS OF PARTIALLY LOADED       @G860P46 */
  void * __ptr32 /*  AL4 */ cvtapft;       /* -        ADDRESS OF AUTHORIZED PROGRAM FACILITY */
  void * __ptr32 /*  VL4 */ cvtqcs01;      /* - BRANCH ENTRY ADDRESS TO PROGRAM MANAGER */
  int            /*  FL4 */ cvtfqcb;       /* -        FORMERLY USED BY ENQ/DEQ.         @G383P9A */
  int            /*  FL4 */ cvtlqcb;       /* -        FORMERLY USED BY ENQ/DEQ.         @G383P9A */
  void * __ptr32 /*  VL4 */ cvtrenq;       /* - RESOURCE MANAGER ADDRESS FOR ENQ  @G860P46 */
  void * __ptr32 /*  AL4 */ cvtrspie;      /* -  RESOURCE MANAGER FOR              @G860P46 */
  void * __ptr32 /*  VL4 */ cvtlkrma;      /* - RESOURCE MANAGER ADDRESS FOR LOCK @G860P46 */
  void * __ptr32 /*  AL4 */ cvtcsd;        /* -        VIRTUAL ADDRESS OF COMMON SYSTEM DATA AREA */
  void * __ptr32 /*  VL4 */ cvtdqiqe;      /* - RESOURCE MANAGER FOR EXIT         @G860P46 */
  void * __ptr32 /*  VL4 */ cvtrpost;      /* - RESOURCE MANAGER FOR POST.        @G860P46 */
  void * __ptr32 /*  VL4 */ cvt062r1;      /* - BRANCH ENTRY TO DETACH              MDC060 */
  void * __ptr32 /*  VL4 */ cvtveac0;      /* - ASCBCHAP BRANCH ENTRY               MDC061 */
  void * __ptr32 /*  VL4 */ cvtglmn;       /* - GLOBAL BRANCH ENTRY ADDRESS FOR */
  void * __ptr32 /*  VL4 */ cvtspsa;       /* - POINTER TO GLOBAL WORK/SAVE AREA VECTOR */
  void * __ptr32 /*  VL4 */ cvtwsal;       /* - ADDRESS OF TABLE OF LENGTHS OF LOCAL */
  void * __ptr32 /*  VL4 */ cvtwsag;       /* - ADDRESS OF TABLE OF LENGTHS OF GLOBAL */
  void * __ptr32 /*  VL4 */ cvtwsac;       /* - ADDRESS OF TABLE OF LENGTHS OF CPU */
  void * __ptr32 /*  VL4 */ cvtrecrq;      /* - ADDRESS OF THE RECORDING REQUEST  @G860P46 */
  void * __ptr32 /*  VL4 */ cvtasmvt;      /* -    POINTER TO AUXILIARY STORAGE MANAGEMENT */
  void * __ptr32 /*  VL4 */ cvtiobp;       /* - ADDRESS OF THE BLOCK PROCESSOR CVT */
  void * __ptr32 /*  VL4 */ cvtspost;      /* - POST RESOURCE MANAGER TERMINATION ROUTINE */
  union {
    int            /*  FL4 */ cvtrstwd;      /* -          RESTART RESOURCE MANAGEMENT WORD. @ZA84941 */
    struct {
      short int      /*  HL2 */ cvtrstci;      /* -        CPU ID OF THE CPU HOLDING THE     @ZA84941 */
      char           /*  XL2 */ cvtrstri[2];   /* -     IDENTIFIER OF OWNING ROUTINE      @ZA84941 */
    };
  };
  void * __ptr32 /*  VL4 */ cvtfetch;      /* - ADDRESS OF ENTRY POINT FOR BASIC  @G860P46 */
  void * __ptr32 /*  VL4 */ cvt044r2;      /* - ADDRESS OF IGC044R2 IN CHAP SERVICE */
  void * __ptr32 /*  AL4 */ cvtperfm;      /* -        ADDRESS OF THE PERFORMANCE WORK AREA. */
  void * __ptr32 /*  AL4 */ cvtdair;       /* -        ADDRESS OF IKJDAIR, TSO DYNAMIC */
  void * __ptr32 /*  AL4 */ cvtehdef;      /* -        ADDRESS OF IKJEHDEF, TSO DEFAULT  @G860P46 */
  void * __ptr32 /*  AL4 */ cvtehcir;      /* -        ADDRESS OF IKJEHCIR, TSO CATALOG  @G860P46 */
  void * __ptr32 /*  AL4 */ cvtssap;       /* -        ADDRESS OF SYSTEM SAVE AREA       @ZA65734 */
  void * __ptr32 /*  AL4 */ cvtaidvt;      /* -        POINTER TO APPENDAGE ID VECTOR TABLE */
  void * __ptr32 /*  VL4 */ cvtipcds;      /* - BRANCH ENTRY FOR DIRECT SIGNAL */
  void * __ptr32 /*  VL4 */ cvtipcri;      /* - BRANCH ENTRY FOR REMOTE IMMEDIATE */
  void * __ptr32 /*  VL4 */ cvtipcrp;      /* - BRANCH ENTRY FOR REMOTE PENDABLE */
  void * __ptr32 /*  AL4 */ cvtpccat;      /* -        POINTER TO PHYSICAL CCA VECTOR TABLE */
  void * __ptr32 /*  AL4 */ cvtlccat;      /* -        POINTER TO LOGICAL CCA VECTOR TABLE */
  void * __ptr32 /*  VL4 */ cvtxsft;       /* - ADDRESS OF SYSTEM FUNCTION TABLE  @G383P9A */
  void * __ptr32 /*  VL4 */ cvtxstks;      /* - ADDRESS OF PCLINK STACK           @G383P9A */
  void * __ptr32 /*  VL4 */ cvtxstkn;      /* - ADDRESS OF PCLINK STACK (SAVE=NO) @G383P9A */
  void * __ptr32 /*  VL4 */ cvtxunss;      /* - ADDRESS OF PCLINK UNSTACK         @G383P9A */
  void * __ptr32 /*  AL4 */ cvtpwi;        /* -        ADDRESS OF THE WINDOW INTERCEPT ROUTINE */
  void * __ptr32 /*  AL4 */ cvtpvbp;       /* -        ADDRESS OF THE VIRTUAL BLOCK PROCESSOR */
  void * __ptr32 /*  AL4 */ cvtmfctl;      /* -        POINTER TO MEASUREMENT FACILITY CONTROL */
  void * __ptr32 /*  AL4 */ cvtmfrtr;      /* -  IF MEASUREMENT FACILITY IS ACTIVE, */

  #define cvtmfact 128                     /* -       IF ONE, I/O SUPERVISOR AND TIMER SECOND */

  void * __ptr32 /*  VL4 */ cvtvpsib;      /* - BRANCH ENTRY TO PAGE SERVICES     @G860PVB */
  void * __ptr32 /*  VL4 */ cvtvsi;        /* - POINTER DEFINED, BRANCH ENTRY     @G860P46 */
  void * __ptr32 /*  VL4 */ cvtexcl;       /* - ADDRESS POINTER TO THE EXCP           @D3A */
  void * __ptr32 /*  VL4 */ cvtxunsn;      /* - ADDRESS OF PCLINK UNSTACK         @G383P9A */
  void * __ptr32 /*  VL4 */ cvtisnbr;      /* - ENTRY POINT ADDRESS OF DISABLED       @LLA */
  void * __ptr32 /*  VL4 */ cvtxextr;      /* - ADDRESS OF PCLINK EXTRACT ROUTINE */
  void * __ptr32 /*  VL4 */ cvtmsfrm;      /* - ADDRESS OF THE PROCESSOR          @G860P46 */
  void * __ptr32 /*  AL4 */ cvtscpin;      /* -        ADDRESS OF IPL-TIME SCPINFO DATA      @NAC */
  void * __ptr32 /*  AL4 */ cvtwsma;       /* ADDRESS OF WAIT STATE MESSAGE AREA    @LLC */
  void * __ptr32 /*  VL4 */ cvtrmbr;       /* - ADDRESS OF REGMAIN BRANCH ENTRY.    MDC123 */
  void * __ptr32 /*  VL4 */ cvtlfrm;       /* - LIST FORMAT FREEMAIN BRANCH ENTRY   MDC124 */
  void * __ptr32 /*  VL4 */ cvtgmbr;       /* - LIST FORMAT GETMAIN BRANCH ENTRY    MDC125 */
  void * __ptr32 /*  AL4 */ cvt0tc0a;      /* -        ADDRESS OF TASK CLOSE MODULE        MDC128 */
  int            /*  FL4 */ cvtrlstg;      /* -        SIZE OF ACTUAL REAL STORAGE ONLINE  @0GC */
  void * __ptr32 /*  VL4 */ cvtspfrr;      /* - 'SUPER FRR' ADDRESS (ADDRESS OF   @G860P46 */
  char           /*  XL4 */ cvtrs360[4];   /* - RESERVED.                             @LZC */
  void * __ptr32 /*  VL4 */ cvtsvt;        /* - ADDRESS POINTER FOR FETCH */
  void * __ptr32 /*  AL4 */ cvtirecm;      /* -        ADDRESS OF INITIATOR RESOURCE       MDC158 */
  void * __ptr32 /*  AL4 */ cvtdarcm;      /* -        ADDRESS OF DEVICE ALLOCATION        MDC159 */
  void * __ptr32 /*  VL4 */ cvt0pt02;      /* - ADDRESS OF POST ENTRY POINT         MDC160 */
  char           /*  XL4 */ cvtrs374[4];   /* RESERVED                              @LOC */
  void * __ptr32 /*  VL4 */ cvtwtcb;       /* -  ADDRESS OF WAIT STATE TCB.          MDC164 */
  void * __ptr32 /*  AL4 */ cvtvacr;       /* -        ACR/VARY CPU CHANNEL RECOVERY       MDC178 */
  void * __ptr32 /*  AL4 */ cvtrecon;      /* -        VARY CPU SHUTDOWN ROUTINE         @G860PVB */
  void * __ptr32 /*  VL4 */ cvtgtfr8;      /* - GENERALIZED TRACE FACILITY (GTF)    MDC180 */
  void * __ptr32 /*  VL4 */ cvtvstop;      /* - ADDRESS OF VARY CPU STOP CPU        MDC169 */
  void * __ptr32 /*  AL4 */ cvtvpsa;       /* -        ADDRESS OF COPY OF SYSGEN'ED PSA -  MDC170 */
  void * __ptr32 /*  AL4 */ cvtrmptt;      /* -        ADDRESS OF ISTRAMA1, THE VTAM     @Z30AP9A */
  void * __ptr32 /*  AL4 */ cvtrmpmt;      /* -        ADDRESS OF ISTRAMA2, THE VTAM     @Z30AP9A */
  void * __ptr32 /*  VL4 */ cvtexp1;       /* - ADDRESS OF EXIT PROLOGUE WHICH      MDC173 */
  void * __ptr32 /*  AL4 */ cvtcsdrl;      /* -        REAL ADDRESS OF COMMON SYSTEM DATA  MDC174 */
  void * __ptr32 /*  VL4 */ cvtssrb;       /* - STATUS STOP SRB ENTRY.              MDC175 */
  char           /*  XL4 */ cvtrs3a4[4];   /* -     RESERVED                              @LFC */
  void * __ptr32 /*  VL4 */ cvtqv1;        /* - ADDRESS OF QUEUE VERIFICATION FOR   MDC181 */
  void * __ptr32 /*  VL4 */ cvtqv2;        /* - ADDRESS OF QUEUE VERIFICATION FOR   MDC182 */
  void * __ptr32 /*  VL4 */ cvtqv3;        /* - ADDRESS OF QUEUE VERIFICATION FOR   MDC183 */
  void * __ptr32 /*  VL4 */ cvtgsda;       /* - ADDRESS OF GLOBAL SYSTEM DUPLEX       @L7C */

  #define cvtgsdab 128                     /* -       IF HIGH-ORDER BIT IS ONE, THERE IS  MDC185 */

  void * __ptr32 /*  VL4 */ cvtadv;        /* - ADDRESS OF ADDRESS VERIFICATION     MDC186 */
  void * __ptr32 /*  VL4 */ cvttpio;       /* -   ADDRESS OF VTAM TPIO (SVC 124)      MDC193 */
  char           /*  XL4 */ cvtrs3c0[4];   /* -     RESERVED                              @LFC */
  void * __ptr32 /*  VL4 */ cvtevent;      /* - BRANCH ENTRY ADDRESS TO EVENTS    @G860P46 */
  void * __ptr32 /*  AL4 */ cvtsscr;       /* -        ADDRESS OF STORAGE SYSTEM         @Z30LP9A */
  void * __ptr32 /*  VL4 */ cvtcbbr;       /* - BRANCH ENTRY ADDRESS TO           @Z30EN9A */
  void * __ptr32 /*  AL4 */ cvteff02;      /* -        ADDRESS OF IKJEFF02, TSO MESSAGE  @ZM30972 */
  void * __ptr32 /*  VL4 */ cvtlsch;       /* - ADDRESS OF LOCAL SCHEDULE.        @G50EP9A */
  void * __ptr32 /*  AL4 */ cvtcdeq;       /* -        ADDRESS OF PROGRAM MANAGER        @G50EP9A */
  void * __ptr32 /*  AL4 */ cvthsm;        /* -        POINTER TO HIERARCHICAL STORAGE   @ZA27771 */
  void * __ptr32 /*  AL4 */ cvtrac;        /* -        ADDRESS OF ACCESS CONTROL CVT.    @Z40RP9A */
  void * __ptr32 /*  VL4 */ cvtcgk;        /* -  ADDRESS OF ROUTINE USED TO CHANGE @G860P46 */
  void * __ptr32 /*  VL4 */ cvtsrm;        /* - ADDRESS OF ENTRY TABLE FOR SRM,   @G50IP9A */
  void * __ptr32 /*  VL4 */ cvt0pt0e;      /* - ENTRY POINT TO IDENTIFY POST EXIT @Z40EP9A */
  void * __ptr32 /*  VL4 */ cvt0pt03;      /* - POST REINVOCATION ENTRY POINT     @Z40EP9A */
  void * __ptr32 /*  AL4 */ cvttcasp;      /* -        POINTER TO THE TSO/VTAM TERMINAL  @Z40DQ9A */
  void * __ptr32 /*  AL4 */ cvtcttvt;      /* -        CTT VT                                @PVC */
  void * __ptr32 /*  VL4 */ cvtjterm;      /* - POINTER DEFINED ADDRESS OF        @G860PVB */
  void * __ptr32 /*  VL4 */ cvtrsume;      /* - ADDRESS OF RESUME FUNCTION.       @G383P9A */
  void * __ptr32 /*  VL4 */ cvttctl;       /* - ADDRESS OF TRANSFER CONTROL       @Z40FP9A */
  void * __ptr32 /*  AL4 */ cvtrmt;        /* -        ADDRESS OF RESOURCE MANAGER           @LGC */
  void * __ptr32 /*  VL4 */ cvtt6svc;      /* - ENTRY POINT ADDRESS FOR TYPE 6    @Z40FP9A */
  void * __ptr32 /*  VL4 */ cvtsusp;       /* - ADDRESS OF SUSPEND ROUTINE.       @Z40FP9A */
  void * __ptr32 /*  VL4 */ cvtihasu;      /* - ADDRESS OF BIT STRING.  (MDC355)  @Z404P9A */
  void * __ptr32 /*  VL4 */ cvtsfv;        /* - ADDRESS OF SETFRR ROUTINE ABOVE 16M   @N3A */
  void * __ptr32 /*  VL4 */ cvtidevn;      /* - ADDRESS OF DEVICE NUMBER CONVERSION   @LGC */
  void * __ptr32 /*  AL4 */ cvtsmf83;      /* -        ADDRESS OF BRANCH ENTRY TO        @G741P9A */
  void * __ptr32 /*  VL4 */ cvtsmfsp;      /* - ADDRESS OF SMF SUSPEND HANDLER.   @G860P46 */
  void * __ptr32 /*  AL4 */ cvtmsfcb;      /* -        ADDRESS OF MAINTENANCE AND        @G871A9A */
  void * __ptr32 /*  VL4 */ cvthid;        /* -  ADDRESS OF SCP HOST ID. @(DCR819) @G860P46 */
  void * __ptr32 /*  VL4 */ cvtpsxm;       /* - ADDRESS OF CROSS MEMORY PAGE      @G860PVB */
  void * __ptr32 /*  VL4 */ cvtucbsc;      /* - ADDRESS OF UCB SCAN               @G860P46 */
  void * __ptr32 /*  AL4 */ cvttpur;       /* -        DDR QUEUE OF TAPE UNIT-RECORD         @L2A */
  void * __ptr32 /*  AL4 */ cvtdpur;       /* -        DDR QUEUE OF DASD SWAP REQUESTS.      @L2A */
  void * __ptr32 /*  AL4 */ cvttrpos;      /* -        DDR QUEUE OF TAPES TO BE              @L2A */
  void * __ptr32 /*  AL4 */ cvtrs444;      /* -        Reserved, must always be 0. Was CVTRESTX, */
  short int      /*  HL2 */ cvtxcpct;      /* -      MAXIMUM EXCP COUNT PER ADDRESS        @L2A */
  short int      /*  HL2 */ cvtcall;       /* -           A BASSM 14,15 INSTRUCTION.  POINTER   @PDA */
  void * __ptr32 /*  VL4 */ cvtvfind;      /* - THE POINTER TO VIRTUAL FETCH BUILD    @L8A */
  void * __ptr32 /*  VL4 */ cvtvfget;      /* - THE POINTER TO VIRTUAL FETCH GET      @L8A */
  void * __ptr32 /*  AL4 */ cvtvfmem;      /* RESERVED. THIS FIELD IS NO LONGER    @MLC */
  void * __ptr32 /*  AL4 */ cvtvfcb;       /* -        THE POINTER TO VIRTUAL FETCH INTERNAL @L8A */
  void * __ptr32 /*  VL4 */ cvtpgser;      /* - POINTER DEFINED ADDRESS OF ENTRY   @G860P46 */
  void * __ptr32 /*  VL4 */ cvttski;       /* - POINTER DEFINED ADDRESS OF TASK   @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpgub;      /* - POINTER DEFINED ADDRESS OF CPOOL  @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpgup;      /* - POINTER DEFINED ADDRESS OF CPOOL  @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpgtc;      /* - POINTER DEFINED ADDRESS OF GET    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpfre;      /* - POINTER DEFINED ADDRESS OF CPOOL  @G860PVB */
  void * __ptr32 /*  VL4 */ cvtslist;      /* - POINTER DEFINED ADDRESS OF VSM    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtsregn;      /* - POINTER DEFINED ADDRESS OF VSM    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtsloc;       /* - POINTER DEFINED ADDRESS OF VSM     @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpbdb;      /* - POINTER DEFINED ADDRESS OF CPOOL  @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpdlb;      /* - POINTER DEFINED ADDRESS OF CPOOL  @G860PVB */
  void * __ptr32 /*  AL4 */ cvtdoffs;      /* -        STARTING REAL ADDRESS OF DAT-OFF  @G860PVB */
  void * __ptr32 /*  AL4 */ cvtdoffe;      /* -        ENDING REAL ADDRESS OF DAT-OFF    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtrcep;       /* - ADDRESS OF THE RSM CONTROL AND    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtcpgus;      /* - ADDRESS OF CPOOL GET              @G860P46 */
  void * __ptr32 /*  VL4 */ cvtgrrgn;      /* - POINTER DEFINED ADDRESS OF GET    @G860PVB */
  void * __ptr32 /*  VL4 */ cvtgvrgn;      /* - POINTER DEFINED ADDRESS OF GET    @G860PVB */
  char           /*  XL1 */ cvtionlv;      /* -     DEFAULT VALUE OF                  @G860P46 */
  char           /*  XL3 */ cvtrs4a1[3];   /* -     RESERVED                              @LFC */
  char           __filler5[4];
  char           /*  XL4 */ cvtfunc[4];    /* -     Reserved for solution/offering use.   @M6A */

  #define cvtsoln 128                      /* -       If high order bit is on, this is not  @M6A */

  void * __ptr32 /*  AL4 */ cvtsmext;      /* - ADDRESS OF STORAGE MAP EXTENSION. @G860PVB */
  void * __ptr32 /*  AL4 */ cvtnucmp;      /* -        ADDRESS OF NUCLEUS MAP.           @G860PVB */
  char           /*  XL1 */ cvtxafl;       /* -     FLAG BYTE FOR MVS/XA PROCESSING.  @G860P46 */

  #define cvtcsrim 128                     /* -       EXPLICIT LOAD PROCESSING REQUIRED @G860PVB */

  char           /*  XL3 */ cvtrs4b5[3];   /* -     RESERVED                              @LFC */
  void * __ptr32 /*  AL4 */ cvtvtam;       /* -        ADDRESS OF VTAM COMMAND PROCESSOR @G860P46 */
  void * __ptr32 /*  AL4 */ cvtspip;       /* -        ADDRESS OF RTM INTERFACE TO       @ZA65734 */
  union {
    void * __ptr32 /*  AL4 */ cvtckras;      /* -        OLD NAME FOR CVTDFA FIELD.            @P8C */
    void * __ptr32 /*  VL4 */ cvtdfa;        /* - ADDRESS OF DFP ID TABLE, MAPPED BY    @P8A */
  };
  void * __ptr32 /*  VL4 */ cvtnvt0;       /* - ADDRESS OF DATA IN DAT-ON NUCLEUS @G860P46 */
  void * __ptr32 /*  AL4 */ cvtcsomf;      /* -        OWNER OF CHANNEL MEASUREMENT      @G860P46 */
  void * __ptr32 /*  AL4 */ cvtcsoal;      /* -        OWNER OF ADDRESS LIMIT FACILITY.  @G860P46 */
  void * __ptr32 /*  AL4 */ cvtichpt;      /* -        ADDRESS OF THE INSTALLED CHANNEL  @G860P46 */
  void * __ptr32 /*  AL4 */ cvtcsocr;      /* -        CHANNEL SUBSYSTEM OWNER - CHANNEL @G860P46 */
  void * __ptr32 /*  AL4 */ cvtcsocs;      /* -        CHANNEL SUBSYSTEM OWNER - CHANNEL @G860P46 */
  void * __ptr32 /*  AL4 */ cvtllta;       /* -        LINK LIST TABLE ADDRESS.          @G860P46 */
  void * __ptr32 /*  AL4 */ cvtdcqa;       /* -   ADDRESS OF DEVICE CLASS QUEUE         @LDC */
  void * __ptr32 /*  AL4 */ cvtucba;       /* -   ADDRESS OF THE FIRST UCB IN THE       @LDC */
  void * __ptr32 /*  VL4 */ cvtvestu;      /* - ADDRESS OF THE ENTRY POINT OF THE @G860P46 */
  void * __ptr32 /*  VL4 */ cvtnuclu;      /* - ADDRESS TO SUPPORT THE NUCLEUS    @G860P46 */
  union {
    char           /*  XL16 */ cvtoslvl[16]; /* SYSTEM LEVEL INDICATORS               @LGA */
    struct {
      char           /*  XL1 */ cvtoslv0;      /* BYTE 0 OF CVTOSLVL                    @LRC */

      #define cvth3310 128                     /* HBB3310 FUNCTIONS ARE PRESENT         @DBA */
      #define cvtesa 128                       /* ESA/370 IS SUPPORTED                  @LQA */
      #define cvtxax 128                       /* ESA/370 IS SUPPORTED (XAX - OLD NAME) @LQC */
      #define cvth4420 64                      /* HBB4420 FUNCTIONS ARE PRESENT.        @LPA */
      #define cvtj3313 32                      /* JBB3313 FUNCTIONS ARE PRESENT         @LHA */
      #define cvtj3311 16                      /* JBB3311 FUNCTIONS ARE PRESENT         @DBA */
      #define cvthiper 16                      /* HIPERSPACES ARE SUPPORTED             @DAA */
      #define cvth4410 8                       /* HBB4410 FUNCTIONS ARE PRESENT.        @LKA */
      #define cvtlkr 8                         /* SPIN LOCK RESTRUCTURE INDICATOR.      @LJA */
      #define cvtucbsv 8                       /* UCB SERVICES INSTALLED.               @LMA */
      #define cvtcads 4                        /* SCOPE=COMMON DATA SPACES SUPPORTED    @04A */
      #define cvtcrptl 2                       /* ENCRYPTION ASYMMETRIC FEATURE IS      @05A */
      #define cvtj4422 1                       /* JBB4422 FUNCTIONS ARE PRESENT         @LTA */

      char           /*  XL1 */ cvtoslv1;      /* BYTE 1 OF CVTOSLVL                    @MBC */

      #define cvth4430 128                     /* HBB4430 FUNCTIONS ARE PRESENT         @LQA */
      #define cvtdyapf 128                     /* DYNAMIC APF, THROUGH CSVAPF, PRESENT  @LQA */
      #define cvtwlm 64                        /* WORKLOAD MANAGER IS INSTALLED         @LYA */
      #define cvth5510 32                      /* HBB5510 FUNCTIONS ARE PRESENT         @M1A */
      #define cvtdynex 32                      /* CSVDYNEX FOR DYNAMIC EXITS IS PRESENT @LXA */
      #define cvth5520 16                      /* HBB5520 FUNCTIONS ARE PRESENT         @M3A */
      #define cvtenclv 16                      /* ENCLAVES FUNCTION IS PRESENT          @M4A */
      #define cvtj5522 8                       /* JBB5522 FUNCTIONS ARE PRESENT         @M8A */
      #define cvth5530 4                       /* HBB6603 FUNCTIONS ARE PRESENT         @MBA */
      #define cvth6603 4                       /* HBB6603 FUNCTIONS ARE PRESENT         @MDA */
      #define cvtos390_010300 4                /* OS/390 R3                             @MEA */
      #define cvtos390_r3 4                    /* OS/390 R3                             @MEA */
      #define cvtdynl 4                        /* Dynamic LNKLST, via CSVDYNL, is present */
      #define cvth6601 2                       /* OS/390 release 1                      @M9A */
      #define cvtos390 2                       /* OS/390 release 1                      @M9A */
      #define cvtos390_010100 2                /* OS/390 R1                             @MEA */
      #define cvtos390_r1 2                    /* OS/390 R1                             @MEA */
      #define cvtprded 2                       /* Product enable/disable (IFAEDxxx)     @M9A */
      #define cvtj6602 1                       /* OS/390 release 2                      @MAA */
      #define cvtos390_010200 1                /* OS/390 R2                             @MEA */
      #define cvtos390_r2 1                    /* OS/390 R2                             @MEA */
      #define cvtparmc 1                       /* Logical Parmlib Service is available */

      char           /*  XL1 */ cvtoslv2;      /* BYTE 2 OF CVTOSLVL                    @PTC */

      #define cvtos390_010400 128              /* OS/390 R4                             @MEA */
      #define cvtos390_020400 128              /* OS/390 R4                             @PPA */
      #define cvtos390_r4 128                  /* OS/390 R4                             @MEA */
      #define cvtj6604 128                     /* OS/390 R4                             @MEA */
      #define cvtdylpa 128                     /* Dynamic LPA (CSVDYLPA) available      @MEA */
      #define cvtrtls 128                      /* Runtime Library Services (CSVRTLS)    @MEA */
      #define cvtos390_020500 64               /* OS/390 R5                             @MFA */
      #define cvtos390_r5 64                   /* OS/390 R5                             @MFA */
      #define cvth6605 64                      /* OS/390 R5                             @MFA */
      #define cvtos390_020600 32               /* OS/390 R6                             @MGA */
      #define cvtos390_r6 32                   /* OS/390 R6                             @MGA */
      #define cvth6606 32                      /* OS/390 R6                             @MGA */
      #define cvtbfp 16                        /* Binary Floating Point support         @MGA */
      #define cvtos390_020700 8                /* OS/390 R7                             @MHA */
      #define cvtos390_r7 8                    /* OS/390 R7                             @MHA */
      #define cvtj6607 8                       /* OS/390 R7                             @MHA */
      #define cvtos390_020800 4                /* OS/390 R8                             @MIA */
      #define cvtos390_r8 4                    /* OS/390 R8                             @MIA */
      #define cvth6608 4                       /* OS/390 R8                             @MIA */
      #define cvtos390_020900 2                /* OS/390 R9                             @MJA */
      #define cvtos390_r9 2                    /* OS/390 R9                             @MJA */
      #define cvtj6609 2                       /* OS/390 R9                             @MJA */
      #define cvth6609 2                       /* OS/390 R9                             @MJA */
      #define cvtos390_021000 1                /* OS/390 R10                            @PUC */
      #define cvtos390_r10 1                   /* OS/390 R10                            @MLA */
      #define cvth7703 1                       /* OS/390 R10                            @MLA */

      char           /*  XL1 */ cvtoslv3;      /* BYTE 3 OF CVTOSLVL                    @NDC */

      #define cvtpause 128                     /* Pause/Release services are present    @MKA */
      #define cvtpaus2 64                      /* IEAVAPE2 and related services, and    @N5A */
      #define cvtzos 32                        /* z/OS V1R1                             @MMA */
      #define cvtzos_010100 32                 /* z/OS V1R1                             @MMA */
      #define cvtzos_v1r1 32                   /* z/OS V1R1                             @MMA */
      #define cvtj7713 32                      /* JBB7713                               @MMA */
      #define cvtlparc 32                      /* LPAR Clustering is present.           @PWA */
      #define cvtzos_010200 16                 /* z/OS V1R2                             @MQA */
      #define cvtzos_v1r2 16                   /* z/OS V1R2                             @MQA */
      #define cvth7705 16                      /* HBB7705                               @MQA */
      #define cvtv64 16                        /* 64-bit virtual services are present.  @MQA */
      #define cvtzos_010300 8                  /* z/OS V1R3                             @MSA */
      #define cvtzos_v1r3 8                    /* z/OS V1R3                             @MSA */
      #define cvth7706 8                       /* HBB7706                               @MSA */
      #define cvtzos_010400 4                  /* z/OS V1R4                             @MUA */
      #define cvtzos_v1r4 4                    /* z/OS V1R4                             @MUA */
      #define cvth7707 4                       /* HBB7707                               @MUA */
      #define cvtzos_010500 2                  /* z/OS V1R5                             @MVA */
      #define cvtzos_v1r5 2                    /* z/OS V1R5                             @MVA */
      #define cvth7708 2                       /* HBB7708                               @MVA */
      #define cvtzos_010600 1                  /* z/OS V1R6                             @MWA */
      #define cvtzos_v1r6 1                    /* z/OS V1R6                             @MWA */
      #define cvth7709 1                       /* HBB7709                               @MWA */

      char           /*  XL1 */ cvtoslv4;      /* BYTE 4 OF CVTOSLVL                    @0KC */

      #define cvtcsrsi 128                     /* CSRSI service is available            @0EA */
      #define cvtunics 64                      /* Unicode callable services available   @MLA */
      #define cvtcsrun 32                      /* CSRUNIC callable service available    @MLA */
      #define cvtilm 16                        /* IBM License Manager functions are     @PYM */
      #define cvtalrs 8                        /* ASN-and-LX-Reuse architecture is      @MXA */
      #define cvttocp 4                        /* TIMEUSED TIME_ON_CP                   @0KA */
      #define cvtziip 2                        /* zIIP support is present               @H5A */
      #define cvtsup 2                         /* zIIP support is present               @H5A */
      #define cvtifar 1                        /* IFA routine is present                @H4A */

      char           /*  XL1 */ cvtoslv5;      /* BYTE 5 OF CVTOSLVL                    @NIC */

      #define cvtzose 128                      /* z/OS.e                                @MTA */
      #define cvtzosas 128                     /* z/OS.e                                @MTA */
      #define cvtpuma 128                      /* z/OS.e                                @MTA */
      #define cvtzos_010700 64                 /* z/OS V1R7                             @Q3C */
      #define cvtzos_v1r7 64                   /* z/OS V1R7                             @Q3C */
      #define cvth7720 64                      /* HBB7720                               @MYA */
      #define cvtzos_010800 32                 /* z/OS V1R8                             @N2A */
      #define cvtzos_v1r8 32                   /* z/OS V1R8                             @N2A */
      #define cvth7730 32                      /* HBB7730                               @N2A */
      #define cvtzos_010900 16                 /* z/OS V1R9                             @N6A */
      #define cvtzos_v1r9 16                   /* z/OS V1R9                             @N6A */
      #define cvth7740 16                      /* HBB7740                               @N6A */
      #define cvtzos_011000 8                  /* z/OS V1R10                            @N8A */
      #define cvtzos_v1r10 8                   /* z/OS V1R10                            @N8A */
      #define cvth7750 8                       /* HBB7750                               @N8A */
      #define cvtzos_011100 4                  /* z/OS V1R11                            @NBA */
      #define cvtzos_v1r11 4                   /* z/OS V1R11                            @NBA */
      #define cvt_g64cpu_infrastructure 4      /* G64CPU Infrastructure present  @NCA */
      #define cvth7760 4                       /* HBB7760                               @NBA */
      #define cvtzos_011200 2                  /* z/OS V1R12                            @NFA */
      #define cvtzos_v1r12 2                   /* z/OS V1R12                            @NFA */
      #define cvth7770 2                       /* HBB7770                               @NFA */
      #define cvtzos_011300 1                  /* z/OS V1R13                            @NIA */
      #define cvtzos_v1r13 1                   /* z/OS V1R13                            @NIA */
      #define cvth7780 1                       /* HBB7780                               @NIA */

      char           /*  XL1 */ cvtoslv6;      /* BYTE 6 OF CVTOSLVL                    @NKC */

      #define cvtzos_020100 128                /* z/OS V2R1                             @NLC */
      #define cvtzos_v2r1 128                  /* z/OS V2R1                             @NLC */
      #define cvth7790 128                     /* HBB7790                               @NKA */
      #define cvtj778h 32                      /* JBB778H                               @NTA */
      #define cvtzos_v1r13_jbb778h 32          /* JBB778H                             @NTA */
      #define cvtzos_011300_jbb778h 32         /* JBB778H                            @NTA */

      char           /*  XL1 */ cvtoslv7;      /* BYTE 7 OF CVTOSLVL                    @LGA */
      char           /*  XL1 */ cvtoslv8;      /* BYTE 8 OF CVTOSLVL                    @N9C */

      #define cvtpaus3 128                     /* IEA4xxxx                              @N9A */
      #define cvtpaus4 64                      /* Pause with checkpoint-OK              @0MA */
      #define cvtect1 32                       /* TIMEUSED ECT=YES with TIME_ON_CP, */
      #define cvtoocp 16                       /* TIMEUSED with TIME_ON_CP and */

      char           /*  XL1 */ cvtoslv9;      /* BYTE 9 OF CVTOSLVL                    @LGA */
      char           /*  XL1 */ cvtoslva;      /* BYTE 10 OF CVTOSLVL                   @LGA */
      char           /*  XL1 */ cvtoslvb;      /* BYTE 11 OF CVTOSLVL                   @LGA */
      char           /*  XL1 */ cvtoslvc;      /* BYTE 12 OF CVTOSLVL                   @LGA */
      char           /*  XL1 */ cvtoslvd;      /* BYTE 13 OF CVTOSLVL                   @LGA */
      char           /*  XL1 */ cvtoslve;      /* BYTE 14 OF CVTOSLVL                   @LGA */
      char           /*  XL1 */ cvtoslvf;      /* BYTE 15 OF CVTOSLVL                   @LGA */
    };
  };
};

#endif  /* cvt__ */

#ifndef cvtvstgx__
#define cvtvstgx__

struct cvtvstgx {
  int            /*  FL4 */ cvtbldls;      /* -        RESERVED - WAS STARTING ADDRESS OF    @P3C */
  int            /*  FL4 */ cvtbldle;      /* -        RESERVED - WAS ENDING ADDRESS OF      @P3C */
  void * __ptr32 /*  AL4 */ cvtmlpas;      /* -        STARTING VIRTUAL ADDRESS OF MLPA. @G860PVB */
  void * __ptr32 /*  AL4 */ cvtmlpae;      /* -        ENDING VIRTUAL ADDRESS OF MLPA.   @G860PVB */
  void * __ptr32 /*  AL4 */ cvtflpas;      /* -        STARTING VIRTUAL ADDRESS OF FLPA. @G860PVB */
  void * __ptr32 /*  AL4 */ cvtflpae;      /* -        ENDING VIRTUAL ADDRESS OF FLPA.   @G860PVB */
  void * __ptr32 /*  AL4 */ cvtplpas;      /* -        STARTING VIRTUAL ADDRESS OF PLPA. @G860PVB */
  void * __ptr32 /*  AL4 */ cvtplpae;      /* -        ENDING VIRTUAL ADDRESS OF PLPA.   @G860PVB */
  void * __ptr32 /*  AL4 */ cvtrwns;       /* -        STARTING VIRTUAL ADDRESS OF       @G860PVB */
  void * __ptr32 /*  AL4 */ cvtrwne;       /* -        ENDING VIRTUAL ADDRESS OF         @G860PVB */
  void * __ptr32 /*  AL4 */ cvtrons;       /* -        STARTING VIRTUAL ADDRESS OF       @G860PVB */
  void * __ptr32 /*  AL4 */ cvtrone;       /* -        ENDING VIRTUAL ADDRESS OF         @G860PVB */
  void * __ptr32 /*  AL4 */ cvterwns;      /* -        STARTING EXTENDED ADDRESS         @G860P46 */
  void * __ptr32 /*  AL4 */ cvterwne;      /* -        ENDING EXTENDED ADDRESS           @G860P46 */
  void * __ptr32 /*  AL4 */ cvteplps;      /* -        STARTING VIRTUAL ADDRESS OF       @G860PVB */
  void * __ptr32 /*  AL4 */ cvteplpe;      /* -        ENDING VIRTUAL ADDRESS OF         @G860PVB */
  void * __ptr32 /*  AL4 */ cvteflps;      /* -        STARTING VIRTUAL ADDRESS OF       @G860PVB */
  void * __ptr32 /*  AL4 */ cvteflpe;      /* -        ENDING VIRTUAL ADDRESS OF         @G860PVB */
  void * __ptr32 /*  AL4 */ cvtemlps;      /* -        STARTING VIRTUAL ADDRESS OF       @G860PVB */
  void * __ptr32 /*  AL4 */ cvtemlpe;      /* -        ENDING VIRTUAL ADDRESS OF         @G860PVB */
};

#endif  /* cvtvstgx__ */

#ifndef cvtxtnt1__
#define cvtxtnt1__

struct cvtxtnt1 {
  void * __ptr32 /*  AL4 */ cvtfachn;      /* -        ADDRESS OF CHAIN OF DCB FIELD AREAS */
  char           /*  XL8 */ cvt1r004[8];   /* RESERVED                              @LFC */
};

#endif  /* cvtxtnt1__ */

#ifndef cvtxtnt2__
#define cvtxtnt2__

struct cvtxtnt2 {
  char           /*  XL4 */ cvt2r000[4];   /* RESERVED                              @LFC */
  char           /*  CL1 */ cvtnucls;      /* -      IDENTIFICATION OF THE NUCLEUS MEMBER */
  char           /*  XL1 */ cvtflgbt;      /* -     Flag byte. This byte is an interface */

  #define cvtnpe 128                       /* -       INDICATES NON-PAGING ENVIRONMENT (VM */
  #define cvtvme 64                        /* -       INDICATES MACHINE IS OPERATING IN VM */
  #define cvtbah 32                        /* -       INDICATES THAT THE VM/370 - OS/VS1 BTAM */
  #define cvtundvm 16                      /* -       Running under VM (this is not the same */

  char           /*  XL2 */ cvtiocid[2];   /* -     EBCDIC IDENTIFIER OF THE ACTIVE I/O   @LBA */
  void * __ptr32 /*  VL4 */ cvtdebvr;      /* - ADDRESS OF BRANCH ENTRY POINT OF DEB */
  void * __ptr32 /*  AL4 */ cvtcvaf;       /* -        POINTER TO THE CVAF TABLE, WHICH CONTAINS */
  void * __ptr32 /*  VL4 */ cvtmmvt;       /* ADDRESS OF THE MEDIA MANAGER VECTOR TABLE */
  void * __ptr32 /*  AL4 */ cvtncvp;       /* ADDRESS OF CSA BUFFER POOL - USED BY */
  union {
    void * __ptr32 /*  AL4 */ cvtqid;        /* -          SAME AS CVTQIDA BELOW               ICB381 */
    struct {
      char           __filler0[1];
      unsigned int   /*  RL3 */ cvtqida:24;    /* -      ADDRESS OF QUEUE IDENTIFICATION (QID) */
    };
  };
  void * __ptr32 /*  AL4 */ cvtoltep;      /* -        POINTER TO CONTROL BLOCK CREATED BY SVC 59 */
  char           /*  XL4 */ cvt2r020[4];   /* -     RESERVED                              @LFC */
  void * __ptr32 /*  AL4 */ cvtavvt;       /* ADDRESS OF AVM CONTROL BLOCK          @LAA */

  #define cvtavin 128                      /* INDICATES AVM INSTALLED               @LAA */

  void * __ptr32 /*  AL4 */ cvtccvt;       /* -        ADDRESS OF CRYPTOGRAPHIC FACILITY CVT */
  void * __ptr32 /*  AL4 */ cvtskta;       /* -        ADDRESS OF STORAGE KEY TABLE (VM */
  void * __ptr32 /*  AL4 */ cvticb;        /* -        ADDRESS OF MASS STORAGE SYSTEM (MSS) */
  char           /*  XL1 */ cvtfbyt1;      /* -     FLAG BYTE */

  #define cvtrde 128                       /* -       RELIABILITY DATA EXTRACTOR INDICATOR  @D7A */

  char           /*  XL3 */ cvt2r035[3];   /* -     RESERVED                              @LFC */
  union {
    double         /*  DL8 */ cvtldto;       /* LOCAL TIME/DATE OFFSET                @08C */
    struct {
      int            /*  FL4 */ cvtldtol;      /* HIGH WORD                             @08A */
      int            /*  FL4 */ cvtldtor;      /* LOW WORD                              @08A */
    };
  };
  void * __ptr32 /*  AL4 */ cvtatcvt;      /* -        POINTER TO VTAM'S CVT             @ZA65734 */

  #define cvtatact 128                     /* IF ON, VTAM IS ACTIVE               MDC081 */

  char           /*  XL4 */ cvt2r044[4];   /* -     RESERVED                              @LFC */
  int            /*  FL4 */ cvtbclmt;      /* -        NUMBER OF 130-BYTE RECORDS SET ASIDE  @LBA */
  int            /*  FL4 */ cvt2r04c;      /* RESERVED                              @0BA */
  union {
    double         /*  DL8 */ cvtlso;        /* LEAP SECOND OFFSET IN TOD FORMAT      @0BA */
    struct {
      int            /*  FL4 */ cvtlsoh;       /* HIGH WORD                             @0BA */
      int            /*  FL4 */ cvtlsol;       /* LOW WORD                              @0BA */
    };
  };
  char           /*  XL44 */ cvt2r058[44]; /* RESERVED                              @0BC */
};

#endif  /* cvtxtnt2__ */


#pragma pack(reset)

