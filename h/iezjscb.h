#pragma pack(packed)

#ifndef iezjscb__
#define iezjscb__

struct iezjscb {

  #define jscbsec1 188                     /* -            START OF JSCB SECTION 1 */

  char           __filler0[188];
  int            /*  FL4 */ jscrsv01;      /* -            RESERVED */
  union {
    void * __ptr32 /*  AL4 */ jschpce;       /* -           ADDRESS OF OPTIONAL JOB ENTRY SUBSYSTEM */
    struct {
      char           /*  XL1 */ jscrsv32;      /* -            RESERVED                           ICB459 */
      unsigned int   /*  RL3 */ jschpcea:24;   /* -          ADDRESS OF OPTIONAL JOB ENTRY SUBSYSTEM */
    };
  };
  void * __ptr32 /*  AL4 */ jscbshr;       /* -            ADDRESS OF ASSEMBLY CHAIN (VSAM)   ICB434 */
  void * __ptr32 /*  AL4 */ jscbtcp;       /* -            ADDRESS OF TIOT CHAINING ELEMENT CHAIN */
  void * __ptr32 /*  AL4 */ jscbpcc;       /* -            ADDRESS OF PRIVATE CATALOG CONTROL BLOCK */
  void * __ptr32 /*  AL4 */ jscbtcbp;      /* -            ADDRESS OF INITIATOR'S TCB (VSAM)  ICB434 */
  void * __ptr32 /*  AL4 */ jscbijsc;      /* -            ADDRESS OF JSCB OF THE INITIATOR THAT */
  void * __ptr32 /*  AL4 */ jscbdbtb;      /* -            ADDRESS OF THE DEB TABLE FOR THIS JOB */
  char           /*  CL4 */ jscbid[4];     /* -          JOB SERIAL NUMBER (OS/VS1) */
  union {
    void * __ptr32 /*  AL4 */ jscbdcb;       /* -           ADDRESS OF DCB FOR DATA SET CONTAINING */
    struct {
      char           /*  XL1 */ jscrsv02;      /* -            RESERVED */
      unsigned int   /*  RL3 */ jscbdcba:24;   /* -          ADDRESS OF DCB FOR DATA SET CONTAINING */
    };
  };
  unsigned char  /*  FL1 */ jscbstep;      /* -          CURRENT STEP NUMBER.  THE FIRST STEP IS */
  char           /*  XL3 */ jscrsv03[3];   /* -          RESERVED */
  void * __ptr32 /*  AL4 */ jscbsecb;      /* -            ECB FOR COMMUNICATION BETWEEN    @ZMC1264 */
  unsigned char  /*  BL1 */ jscbopts;      /* -            OPTION SWITCHES */

  #define jscrsv04 128                     /* -  RESERVED */
  #define jscrsv05 64                      /* -  RESERVED */
  #define jscblong 32                      /* -        THE PARTITION CANNOT BE REDEFINED BECAUSE */
  #define jscrsv06 16                      /* -  RESERVED */
  #define jscrsv07 8                       /* -  RESERVED */
  #define jscbtiod 4                       /* -        WHEN SET BY PROGRAM, EXCLUSIVE ENQS FOR */
  #define jscsiots 2                       /* -        CHECKPOINT MUST SCAN SIOT          MDC018 */
  #define jscbauth 1                       /* -        The step represented by this */

  char           /*  CL6 */ jscbcrb6[6];   /* -          LOW ORDER 6 BYTES OF THE SHR RBA USED */
  unsigned char  /*  BL1 */ jscbswt1;      /* -            STATUS SWITCHES  (OS/VS2)          ICB351 */

  #define jscbpass 128                     /* -        WHEN THIS BIT IS SET TO ONE AND A */
  #define jscbunin 64                      /* -        When ON, indicates that Allocation */
  #define jscrsv12 32                      /* -  RESERVED */
  #define jscrsv13 16                      /* -  RESERVED */
  #define jscrsv14 8                       /* -  RESERVED */
  #define jscrsv15 4                       /* -  RESERVED */
  #define jscrsv16 2                       /* -  RESERVED                             @01C */
  #define jscbpmsg 1                       /* -        A MESSAGE HAS BEEN ISSUED BECAUSE THE */

  void * __ptr32 /*  AL4 */ jscbqmpi;      /* -            ADDRESS OF THE QUEUE MANAGER PARAMETER */
  void * __ptr32 /*  AL4 */ jscbjesw;      /* -            ADDRESS OF THE JES WORKAREA      @YA01530 */
  union {
    char           /*  CL4 */ jscbwtp[4];    /* -         WRITE-TO-PROGRAMMER (WTP) DATA */
    struct {
      unsigned char  /*  BL1 */ jscbwtfg;      /* -            FLAGS USED BY WTP SUPPORT */

      #define jscbiofg 128                     /* -        THE PREVIOUS WTP I/O OPERATION HAD AN */
      #define jscbret 64                       /* -        TEXT BREAKING INDICATOR, ADDITIONAL */
      #define jscbbmo 32                       /* -              Buffer Messages Only flag.  Set */
      #define jscrsv19 16                      /* -  RESERVED */
      #define jscrsv20 8                       /* -  RESERVED */
      #define jscrsv21 4                       /* -  RESERVED */
      #define jscrsv22 2                       /* -  RESERVED */
      #define jscrsv23 1                       /* -  RESERVED */

      unsigned char  /*  FL1 */ jscbwtsp;      /* -          NUMBER OF THE LAST JOB STEP TO ISSUE WTP */
      short int      /*  HL2 */ jscbpmg;       /* -            NUMBER OF WTP OPERATIONS ISSUED FOR THE */
    };
  };
  void * __ptr32 /*  AL4 */ jscbcscb;      /* -            ADDRESS OF COMMAND SCHEDULING CONTROL */

  #define jscbs1ln 72                      /* - LENGTH OF SECTION 1 */
  #define jscbsec2 260                     /* -            START OF JSCB SECTION 2            ICB351 */
  #define jscbs2ln 0                       /* - LENGTH OF SECTION 2                ICB351 */
  #define jscbsec3 260                     /* -            START OF JSCB SECTION 3            ICB351 */

  union {
    int            /*  FL4 */ jscbjct;       /* -           Structure containing SVA of JCT      @P2C */
    struct {
      char           /*  XL1 */ jscrsv24;      /* -            RESERVED                           ICB351 */
      char           /*  CL3 */ jscjctp[3];    /* -         ALIAS FOR JSCBJCTA                 MDC025 */
    };
    struct {
      char           __filler1[1];
      char           /*  CL3 */ jscbjcta[3];   /* -          SVA of JCT, use SWAREQ to convert    @P2C */
    };
  };
  void * __ptr32 /*  AL4 */ jscbpscb;      /* -            ADDRESS OF TSO PROTECTED STEP CONTROL */
  union {
    short int      /*  HL2 */ jscbasid;      /* -           ADDRESS SPACE IDENTIFIER (MDC028)  YM0446 */
    short int      /*  HL2 */ jscbtjid;      /* -            TSO TERMINAL JOB IDENTIFIER */
  };
  unsigned char  /*  BL1 */ jscbfbyt;      /* -            FLAG BYTE  (MDC300)              @Z40RP9A */

  #define jscbrv01 128                     /* -  RESERVED */
  #define jscbadsp 64                      /* -        AUTOMATIC DATA SET PROTECTION FOR THIS */
  #define jscbrv02 32                      /* -  RESERVED */
  #define jscbrv03 16                      /* -  RESERVED */
  #define jscbsjfy 8                       /* -  Used by BB131                        @P4C */
  #define jscbsjfn 4                       /* -  Used by BB131                        @P4C */
  #define jscbrv06 2                       /* -  RESERVED */
  #define jscbrv07 1                       /* -  RESERVED */

  char           /*  XL1 */ jscbrv08;      /* -            RESERVED */
  int            /*  FL4 */ jscbiecb;      /* -            ECB USED FOR COMMUNICATION BETWEEN */
  char           /*  CL8 */ jscbjrba[8];   /* -          JOB JOURNAL RELATIVE BYTE ADDRESS (RBA) */
  void * __ptr32 /*  AL4 */ jscbaloc;      /* -            ADDRESS OF THE ALLOCATION WORK   @ZMC1264 */
  union {
    void * __ptr32 /*  AL4 */ jscbjnl;       /* -           INITIATOR JSCB ONLY - ADDRESS OF JSCB */
    struct {
      unsigned char  /*  BL1 */ jscbjjsb;      /* -            JOB JOURNAL STATUS INDICATORS      ICB332 */

      #define jscbjnln 128                     /* -        NOTHING SHOULD BE WRITTEN IN */
      #define jscbjnlf 64                      /* -        NO JOB JOURNAL                     MDC017 */
      #define jscbjnle 32                      /* -        ERROR IN JOURNAL, DO NOT WRITE     ICB332 */
      #define jscbjsbi 8                       /* -        JOB HAS NOT ENTERED ALLOCATION FOR THE */
      #define jscbjsba 4                       /* -        JOB HAS ENTERED ALLOCATION         ICB332 */
      #define jscbjsbx 2                       /* -        JOB HAS COMPLETED ALLOCATION       ICB332 */
      #define jscbjsbt 1                       /* -        JOB HAS ENTERED TERMINATION        ICB332 */

      unsigned int   /*  RL3 */ jscbjnla:24;   /* -          INITIATOR JSCB ONLY - ADDRESS OF JSCB */
    };
  };
  void * __ptr32 /*  AL4 */ jscbjnlr;      /* -            POINTER TO JOB JOURNAL RPL         MDC023 */
  void * __ptr32 /*  AL4 */ jscbsmlr;      /* -            ADDRESS OF SYSTEM MESSAGE DATA */
  union {
    void * __ptr32 /*  AL4 */ jscbsub;       /* -           ADDRESS OF JES-SUBTL FOR THIS JOB */
    struct {
      char           /*  XL1 */ jscrsv31;      /* -            RESERVED                           ICB333 */
      unsigned int   /*  RL3 */ jscbsuba:24;   /* -          ADDRESS OF JES-SUBTL FOR THIS JOB */
    };
  };
  short int      /*  HL2 */ jscbsono;      /* -            THE NUMBER OF SYSOUT DATA SETS PLUS */
  char           /*  CL2 */ jscbcrb2[2];   /* -          HIGH ORDER 2 BYTES OF THE SHR RBA USED */
  char           /*  CL8 */ jscbfrba[8];   /* -          RELATIVE BYTE ADDRESS (RBA) OF THE FIRST */
  void * __ptr32 /*  AL4 */ jscbssib;      /* -            ADDRESS OF THE SUBSYSTEM IDENTIFICATION */
  void * __ptr32 /*  AL4 */ jscdsabq;      /* -            ADDRESS OF QDB FOR DSAB CHAIN      MDC007 */
  void * __ptr32 /*  AL4 */ jscrsv35;      /* -            Reserved - was JSCBASW2              @L7C */
  union {
    int            /*  FL4 */ jscsct;        /* -           Structure containing SVA of SCT      @P2C */
    struct {
      char           /*  XL1 */ jscrsv55;      /* -          RESERVED */
      char           /*  CL3 */ jscsctp[3];    /* -          SVA of SCT, use SWAREQ to convert    @P2C */
    };
  };
  void * __ptr32 /*  AL4 */ jsctmcor;      /* -            ADDRESS OF TIOT MAIN STORAGE MANAGEMENT */
  void * __ptr32 /*  AL4 */ jscbvata;      /* -            ADDRESS OF VAT USED DURING SYSTEM RESTART */
  short int      /*  HL2 */ jscddnno;      /* -            COUNTER USED BY DYNAMIC ALLOCATION TO */
  short int      /*  HL2 */ jscbodno;      /* -            COUNTER USED BY DYNAMIC OUTPUT TO    @D1C */
  short int      /*  HL2 */ jscddnum;      /* -            NUMBER OF DD ENTRIES CURRENTLY ALLOCATED */
  char           /*  XL1 */ jscrsv33;      /* -            RESERVED                           MDC019 */
  unsigned char  /*  FL1 */ jscbswsp;      /* -          SWA SUBPOOL                        MDC015 */
  void * __ptr32 /*  AL4 */ jscbact;       /* -            POINTER TO ACTIVE JSCB             MDC014 */
  void * __ptr32 /*  AL4 */ jscbufpt;      /* -            ADDRESS OF ALLOCATION/UNALLOCATION */
  void * __ptr32 /*  AL4 */ jscrsv34;      /* -            Reserved - was JSCBASWA              @L7C */
  union {
    char           /*  CL8 */ jscbpgmn[8];   /* -         JOB STEP PROGRAM NAME  (MDC304)  @G64RP9A */
    struct {
      void * __ptr32 /*  AL4 */ jscbecb1;      /* -            ADDR OF CANCEL ECB WHILE         @ZMC1510 */
      void * __ptr32 /*  AL4 */ jscbecb2;      /* -            ADDR OF WAIT FOR REGION ECB      @ZMC1510 */
    };
  };
  void * __ptr32 /*  AL4 */ jscdsnqp;      /* -            Pointer to the first DSENQ Table     @L5C */
  void * __ptr32 /*  AL4 */ jscbcscx;      /* -            ADDRESS OF CSCX EXTENSION TO CSCB    @L2C */
  int            /*  FL4 */ jscamcpl;      /* -            ALLOCATION MESSAGE CELLPOOL ID   @YC19251 */

  #define jscbs3ln 120                     /* - LENGTH OF SECTION 3                ICB351 */
  #define jscbdisp 188                     /* -    DISPLACEMENT OF FIRST JSCB DATA BYTE */
  #define jscbaos1 72                      /* - OS/VS1 JSCB LENGTH            ICB351 */
  #define jscbaos2 192                     /* - OS/VS2 JSCB LENGTH            ICB332 */

};

#endif  /* iezjscb__ */


#pragma pack(reset)

