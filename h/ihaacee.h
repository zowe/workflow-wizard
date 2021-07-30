#pragma pack(packed)

#ifndef acee__
#define acee__

struct acee {
  char           /*  CL4 */ aceeacee[4];   /* ACRONYM IN EBCDIC -ACEE- */
  union {
    int            /*  FL4 */ aceecore;      /* ACEE SUBPOOL AND LENGTH */
    struct {
      unsigned char  /*  RL1 */ aceesp;        /* ACEE SUBPOOL NUMBER */
      unsigned int   /*  RL3 */ aceelen:24;    /* LENGTH OF ACEE */
    };
  };
  unsigned char  /*  RL1 */ aceevrsn;      /* ACEE VERSION NUMBER          @L4A */

  #define aceevr01 1                       /* ACEE VERSION NUMBER = 1.     @M1A */
  #define aceevr02 2                       /* ACEE VERSION NUMBER = 2.     @M1A */
  #define aceevr03 3                       /* ACEE VERSION NUMBER = 3.     @LAA */
  #define aceecurv 3                       /* ACEE CURRENT VERSION NUMBER  @LAC */

  char           __filler0[3];
  void * __ptr32 /*  AL4 */ aceeiep;       /* RESERVED FOR INSTALLATION. */
  void * __ptr32 /*  AL4 */ aceeinst;      /* USER DATA ADDRESS:  POINTS TO  @02C */
  union {
    char           /*  CL9 */ aceeuser[9];   /* USERID INFORMATION */
    struct {
      unsigned char  /*  RL1 */ aceeusrl;      /* USERID LENGTH */
      char           /*  CL8 */ aceeusri[8];   /* CONTAINS THE VALID RACF USERID @02C */
    };
  };
  union {
    char           /*  CL9 */ aceegrp[9];    /* GROUP NAME INFORMATION */
    struct {
      unsigned char  /*  RL1 */ aceegrpl;      /* GROUP NAME LENGTH */
      char           /*  CL8 */ aceegrpn[8];   /* VALID CONNECT GROUP, UNLESS    @02C */
    };
  };
  unsigned char  /*  BL1 */ aceeflg1;      /* USER FLAGS */

  #define aceespec 128                     /* 1 - SPECIAL ATTRIBUTE */
  #define aceeadsp 64                      /* 1 - AUTOMATIC DATA SECURITY */
  #define aceeoper 32                      /* 1 - OPERATIONS ATTRIBUTE */
  #define aceeaudt 16                      /* 1 - AUDITOR ATTRIBUTE      @PC002PY */
  #define aceelogu 8                       /* 1 - USER IS TO HAVE MOST RACF */
  #define aceepriv 2                       /* 1 - USER IS A STARTED PROCEDURE */
  #define aceeracf 1                       /* 1 - RACF DEFINED USER */

  unsigned char  /*  BL1 */ aceeflg2;      /* DEFAULT UNIVERSAL ACCESS */

  #define aceealtr 128                     /* 1 - ALTER AUTORITY TO */
  #define aceecntl 64                      /* 1 - CONTROL AUTHORITY TO */
  #define aceeupdt 32                      /* 1 - UPDATE AUTHORITY TO */
  #define aceeread 16                      /* 1 - READ AUTHORITY TO */
  #define aceenone 1                       /* 1 - NO AUTHORITY TO */

  unsigned char  /*  BL1 */ aceeflg3;      /* MISCELLANEOUS FLAGS */

  #define aceegrpa 128                     /* ACCESS LIST OF GROUP DS */
  #define aceerasp 64                      /* ON- IF RACF ADDRESS SPACE    @M5C */
  #define aceeclnt 32                      /* ON- IF UN-AUTHENTICATD CLIENT  @03C */
  #define aceeaclt 16                      /* ON- IF AUTHENTICATED CLIENT    @03C */
  #define aceetskp 8                       /* ON- IF TASK LEVEL PROCESS      @D2A */
  #define aceeiusp 4                       /* ON - INITUSP has been done     @06C */
  #define aceeduid 2                       /* ON -default UID being used     @06C */
  #define aceenpwr 1                       /* ON - If Password Not Required  @M6A */

  char           /*  CL3 */ aceedate[3];   /* DATE OF RACINIT */
  char           /*  CL8 */ aceeproc[8];   /* NAME OF STARTED PROC OR BLANKS */
  void * __ptr32 /*  AL4 */ aceetrmp;      /* ADDRESS THAT POINTS TO THE     @02C */
  char           /*  XL2 */ aceeflg4[2];   /* MISCELLANEOUS FLAGS 2      @PC002PY */

  #define aceeuath 32                      /* 1 - USER IS AUTHORIZED TO DEFINE */
  #define aceedasd 8                       /* 1 - USER IS AUTHORIZED TO PROTECT */
  #define aceetape 4                       /* 1 - USER IS AUTHORIZED TO PROTECT */
  #define aceeterm 2                       /* 1 - USER IS AUTHORIZED TO PROTECT */

  unsigned char  /*  RL1 */ aceeaplv;      /* APPLICATION LEVEL:             @02C */
  unsigned char  /*  RL1 */ aceetrlv;      /* POE LEVEL:                     @02C */
  void * __ptr32 /*  AL4 */ aceetrda;      /* POE DATA ADDRESS:  POINTS TO A @02C */
  char           /*  CL8 */ aceetrid[8];   /* AN 8-BYTE AREA CONTAINING THE  @02C */
  void * __ptr32 /*  AL4 */ aceeamp;       /* ADDRESS 1ST ANCHORED MODEL @ZA25840 */
  int            /*  BL4 */ aceeclth;      /* USER CLASS AUTHORIZATIONS - */
  void * __ptr32 /*  AL4 */ aceeclcp;      /* ANCHOR FOR INSTORAGE PROFILE */
  void * __ptr32 /*  AL4 */ aceeaptr;      /* ADDRESS FIELD RESERVED FOR */
  char           /*  CL8 */ aceeapln[8];   /* NAME OF APPLICATION TO WHICH */
  void * __ptr32 /*  AL4 */ aceeapda;      /* APPLICATION DATA ADDRESS:      @02C */
  void * __ptr32 /*  AL4 */ aceeunam;      /* ADDRESS OF USER NAME STRING.   @L4A */
  void * __ptr32 /*  AL4 */ aceemdls;      /* ADDRESS OF THE MODEL NAME      @L4A */
  void * __ptr32 /*  AL4 */ aceecgrp;      /* ADDRESS OF THE CONNECT GROUP */
  void * __ptr32 /*  AL4 */ aceegata;      /* ADDRESS OF THE GENERIC */
  void * __ptr32 /*  AL4 */ aceefcgp;      /* ADDRESS OF LIST OF GROUPS USER */
  void * __ptr32 /*  AL4 */ aceedslp;      /* ADDRESS OF THE LIST OF CATEGORIES */
  char           /*  CL4 */ aceedat4[4];   /* 4 BYTE DATE FIELD IN THE FORM OF */
  void * __ptr32 /*  AL4 */ aceepads;      /* ADDRESS OF THE LIST OF DATA SETS */
  unsigned char  /*  BL1 */ aceeslvl;      /* MAXIMUM SECURITY LEVEL ACCESSIBLE */
  char           /*  XL1 */ aceeflg5;      /* MISCELLANEOUS FLAGS            @L7A */

  #define aceemode 128                     /* AMODE OF ACEE DATA AREAS       @L7A */
  #define aceevmsk 64                      /* 0 - ACEEPLCL points to a 128   @P7C */
  #define aceed4ok 32                      /* 1 - ACEEDAT4 CONTAINS DATA     @PAC */
  #define aceexnvr 16                      /* 1 - ENVR object created on     @05A */
  #define aceestok 8                       /* 0 - SERVAUTH resource name no  @L9C */
  #define aceenste 4                       /* 1 - ENVR object in ACEENSTA should */
  #define aceedaly 2                       /* 1 - User logged on to an       @LEC */

  char           /*  CL1 */ aceeflg6;      /* More miscellaneous flags       @M7A */

  #define aceeraui 128                     /* 1 - Restricted access user ID  @M7A */
  #define aceeruaa 64                      /* 1 - Restricted access user can */
  #define aceeruav 32                      /* 1 - A check has been made to */
  #define aceemfau 16                      /* 1 - User MUST authenticate     @LIA */
  #define aceemfaa 8                       /* 1 - User authenticated with    @LIA */

  char           __filler1[1];
  void * __ptr32 /*  AL4 */ acee3pty;      /* ADDRESS OF ACEE CREATED BY */
  void * __ptr32 /*  AL4 */ aceeplcl;      /* POINTER TO EXTENDED CLASS */
  char           /*  CL8 */ aceesuid[8];   /* SURROGATE USERID (AUDIT)       @M1A */
  void * __ptr32 /*  AL4 */ aceeocox;      /* POINTER TO ACEX                @P5C */
  void * __ptr32 /*  AL4 */ aceeptds;      /* POINTER TO FIRST TDS TABLE     @M2A */
  void * __ptr32 /*  AL4 */ aceex5pr;      /* Pointer to X500 name pair      @M7C */
  void * __ptr32 /*  AL4 */ aceetokp;      /* POINTER TO UTOKEN              @P3D */
  void * __ptr32 /*  AL4 */ aceesrva;      /* Pointer to an area containing  @L9A */
  void * __ptr32 /*  AL4 */ aceesrvp;      /* Pointer to an area containing  @L9A */
  void * __ptr32 /*  AL4 */ aceensta;      /* Pointer to ENVR object for     @LBA */
  void * __ptr32 /*  AL4 */ aceeictx;      /* Pointer to the identity context@LCA */
  void * __ptr32 /*  AL4 */ aceeidid;      /* Pointer to the distributed     @LFA */
  char           /*  CL4 */ aceetime[4];   /* ACEE creation time             @LFC */
};

#endif  /* acee__ */


#pragma pack(reset)

