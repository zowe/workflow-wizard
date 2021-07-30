#pragma pack(packed)


#ifndef tiot__
#define tiot__


struct tiot {


  #define tiot1 0                          /* -            TIOTPTR */


  char           /*  CL8 */ tiocnjob[8];   /* -         JOB NAME */
  union {
    char           /*  CL16 */ tiocstep[16]; /* -         STEP INFORMATION                    @P1C */
    struct {
      union {
        char           /*  CL8 */ tiocstpn[8];   /* -         8-BYTE STEP NAME FOR NON-PROCS      @P1A */
        char           /*  CL8 */ tiocpstn[8];   /* -         8-BYTE PROC STEP NAME FOR PROCS     @P1A */
      };
      char           /*  CL8 */ tiocjstn[8];   /* -         8-BYTE JOBSTEP NAME FOR PROCS       @P1A */


      #define tioentry 24                      /* -            TIODDPTR */


    };
  };
  unsigned char  /*  FL1 */ tioelngh;      /* -          LENGTH, IN BYTES, OF THIS ENTRY */
  unsigned char  /*  BL1 */ tioestta;      /* -            STATUS BYTE A */


  #define tiosltyp 128                     /* -        NONSTANDARD LABEL (TAPE) (OS/VS1) */
  #define tiospltp 64                      /* -        DURING ALLOCATION, SPLIT CYLINDER */
  #define tiosplts 32                      /* -        DURING ALLOCATION, SPLIT CYLINDER */
  #define tiosjblb 16                      /* -        JOBLIB INDICATOR */
  #define tiosdads 8                       /* -        DADSM ALLOCATION NECESSRY */
  #define tioslabl 4                       /* -        LABELED TAPE.  IF BIT 0 IS OFF, SL OR */
  #define tiosdsp1 2                       /* -        REWIND/UNLOAD THE TAPE VOLUME (TAPE) */
  #define tiosdsp2 1                       /* -        REWIND THE TAPE VOLUME (TAPE) */


  union {
    char           /*  CL2 */ tioerloc[2];   /* -         RELATIVE LOCATION OF POOL */
    struct {
      char           /*  CL1 */ tioewtct;      /* -          DURING ALLOCATION, NUMBER OF DEVICES */
      char           /*  CL1 */ tioelink;      /* -          DURING ALLOCATION, LINK TO THE */


      #define tiosyout 128                     /* -        THIS IS A SYSOUT DATA SET THAT CONTAINS */
      #define tiotrv01 64                      /* -        RESERVED                           MDC006 */
      #define tiotterm 32                      /* -        DEVICE IS A TERMINAL */
      #define tioedynm 16                      /* -        DYNAM CODED ON DD STATEMENT */
      #define tioeqnam 8                       /* -        QNAME CODED ON DD STATEMENT */
      #define tioesyin 4                       /* -        ENTRY FOR SPOOLED SYSIN DATA SET */
      #define tioesyot 2                       /* -        ENTRY FOR SPOOLED SYSOUT DATA SET */
      #define tioessds 2                       /* -        ENTRY FOR A SUBSYSTEM DATA SET */
      #define tiotrem 1                        /* -        ENTRY FOR A REMOTE DEVICE          ICB340 */


    };
  };
  char           /*  CL8 */ tioeddnm[8];   /* -          DD NAME */
  char           /*  CL3 */ tioejfcb[3];   /* -          SWA virtual address token, mapped */
  unsigned char  /*  BL1 */ tioesttc;      /* -            STATUS BYTE C.  USED DURING ALLOCATION */


  #define tiosdkcr 128                     /* -        MAIN STORAGE OR DASD ADDRESS */
  #define tiosdefr 64                      /* -        DEFERRED MOUNT */
  #define tiosaffp 32                      /* -        PRIMARY UNIT AFFINITY */
  #define tiosaffs 16                      /* -        SECONDARY UNIT AFFINITY */
  #define tiosvolp 8                       /* -        PRIMARY VOLUME AFFINITY */
  #define tiosvols 4                       /* -        SECONDARY VOLUME AFFINITY */
  #define tiosbalp 2                       /* -        PRIMARY SUBALLOCATE */
  #define tiosbals 1                       /* -        SECONDARY SUBALLOCATE */


  unsigned char  /*  BL1 */ tioesttb;      /* -            STATUS BYTE B - DURING ALLOCATION AND */


  #define tiosused 128                     /* -        DATA SET IS ON DEVICE */
  #define tiosreqd 64                      /* -        DATA SET WILL USE DEVICE */
  #define tiospvio 32                      /* -        DEVICE VIOLATES SEPARATION */
  #define tiosvlsr 16                      /* -        VOLUME SERIAL PRESENT */
  #define tiossetu 8                       /* -        SETUP MESSAGE REQUIRED */
  #define tiosmntd 4                       /* -        IF 0, DELETE UNLOADED VOLUME IF UNLOAD */
  #define tiosunld 2                       /* -        UNLOAD REQUIRED */
  #define tiosverf 1                       /* -        VERIFICATION REQUIRED */


  unsigned int   /*  RL3 */ tioefsrt:24;   /* -          DURING PROBLEM PROGRAM, ADDRESS OF UCB. */


  #define poolstar 44


  char           __filler0[1];
  unsigned char  /*  FL1 */ tiopnslt;      /* -          NUMBER OF SLOTS FOR POOL */
  char           __filler1[1];
  unsigned char  /*  FL1 */ tiopnsrt;      /* -          NUMBER OF DEVICES (FILLED SLOTS) */
  char           /*  CL8 */ tioppool[8];   /* -          POOL NAME */
  char           /*  XL1 */ tiopsttb;      /* -          STATUS OF SLOT */
  unsigned int   /*  RL3 */ tiopslot:24;   /* -          UCB ADDRESS OR EMPTY SLOT */
  char           /*  CL4 */ tiotfend[4];   /* -          FINAL END OF THE TIOT - BINARY ZEROS */
};


#endif  /* tiot__ */




#pragma pack(reset)


