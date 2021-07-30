#pragma pack(packed)


#ifndef pds__
#define pds__


struct pds {


  #define pds2 0                           /* PDS2PTR */


  char           /*  CL8 */ pds2name[8];   /* MEMBER NAME OR ALIAS NAME */
  char           /*  CL3 */ pds2ttrp[3];   /* TTR OF FIRST BLOCK OF NAMED MEMBER */
  unsigned char  /*  BL1 */ pds2indc;      /* INDICATOR BYTE */


  #define pds2alis 128                     /* NAME IN THE FIELD PDS2NAME IS AN ALIAS */
  #define dealias 128                      /* --- ALIAS FOR PDS2ALIS */
  #define pds2nttr 96                      /* NUMBER OF TTR'S IN THE USER DATA FIELD */
  #define pds2lusr 31                      /* - LENGTH OF USER DATA FIELD */


  union {
    char           /*  CL1 */ pds2usrd;      /* START OF VARIABLE LENGTH USER DATA FIELD */
    char           /*  CL3 */ pds2ttrt[3];   /* TTR OF FIRST BLOCK OF TEXT */
  };
  char           /*  CL1 */ pds2zero;      /* ZERO */
  char           /*  CL3 */ pds2ttrn[3];   /* TTR OF NOTE LIST OR SCATTER/TRANSLATION */
  unsigned char  /*  FL1 */ pds2nl;        /* NUMBER OF ENTRIES IN NOTE LIST FOR */
  union {
    short int      /*  BL2 */ pds2atr;       /* TWO-BYTE PROGRAM ATTRIBUTE FIELD */
    struct {
      unsigned char  /*  BL1 */ pds2atr1;      /* FIRST BYTE OF PROGRAM ATTRIBUTE FIELD */


      #define pds2rent 128                     /* REENTERABLE */
      #define dereen 128                       /* --- ALIAS FOR PDS2RENT */
      #define pds2reus 64                      /* REUSABLE */
      #define pds2ovly 32                      /* IN OVERLAY STRUCTURE */
      #define deovly 32                        /* --- ALIAS FOR PDS2OVLY */
      #define pds2test 16                      /* PROGRAM TO BE TESTED - TESTRAN */
      #define pds2load 8                       /* ONLY LOADABLE */
      #define delody 8                         /* --- ALIAS FOR PDS2LOAD */
      #define pds2sctr 4                       /* SCATTER FORMAT */
      #define descat 4                         /* --- ALIAS FOR PDS2SCTR */
      #define pds2exec 2                       /* EXECUTABLE */
      #define dexcut 2                         /* --- ALIAS FOR PDS2EXEC */
      #define pds21blk 1                       /* IF ZERO, PROGRAM CONTAINS MULTIPLE */


      unsigned char  /*  BL1 */ pds2atr2;      /* SECOND BYTE OF PROGRAM ATTRIBUTE FIELD */


      #define pds2flvl 128                     /* If one, the program cannot be processed */
      #define pds2org0 64                      /* ORIGIN OF FIRST BLOCK OF TEXT IS ZERO */
      #define pds2ep0 32                       /* ENTRY POINT IS ZERO */
      #define pds2nrld 16                      /* PROGRAM CONTAINS NO RLD ITEMS */
      #define pds2nrep 8                       /* PROGRAM CANNOT BE REPROCESSED BY LINKAGE */
      #define pds2tstn 4                       /* PROGRAM CONTAINS TESTRAN SYMBOL CARDS */
      #define pds2lef 2                        /* PROGRAM CREATED BY LINKAGE EDITOR F */
      #define pds2refr 1                       /* REFRESHABLE PROGRAM */


    };
  };
  unsigned int   /*  FL3 */ pds2stor:24;   /* TOTAL CONTIGUOUS MAIN STORAGE REQUIREMENT */
  short int      /*  FL2 */ pds2ftbl;      /* LENGTH OF FIRST BLOCK OF TEXT */
  unsigned int   /*  RL3 */ pds2epa:24;    /* ENTRY POINT ADDRESS ASSOCIATED WITH */
  union {
    unsigned int   /*  BL3 */ pds2ftbo:24;   /* FLAG BYTES (MVS USE OF FIELD)        @LCC */
    struct {
      unsigned char  /*  BL1 */ pds2ftb1;      /* BYTE 1 OF PDS2FTBO */


      #define pdsaosle 128                     /* Program has been processed by OS/VS1 or */
      #define pds2big 64                       /* THE LARGE PROGRAM OBJECT EXTENSION */
      #define pds2paga 32                      /* PAGE ALIGNMENT REQUIRED FOR PROGRAM */
      #define pds2ssi 16                       /* SSI INFORMATION PRESENT */
      #define pdsapflg 8                       /* INFORMATION IN PDSAPF IS VALID */
      #define pds2pgmo 4                       /* PROGRAM OBJECT. THE PDS2FTB3 */
      #define pds2lfmt 4                       /* ALTERNATE NAME FOR PDS2PGMO          @L7A */
      #define pds2sign 2                       /* PROGRAM OBJECT IS SIGNED. VERIFIED ON */
      #define pds2xatr 1                       /* PDS2XATTR SECTION                    @LBA */


      unsigned char  /*  BL1 */ pds2ftb2;      /* BYTE 2 OF PDS2FTBO */


      #define pds2altp 128                     /* ALTERNATE PRIMARY FLAG. IF ON (FOR A @L8A */
      #define pdslrmod 16                      /* PROGRAM RESIDENCE MODE               @L6A */
      #define pdsaamod 12                      /* ALIAS ENTRY POINT ADDRESSING MODE    @L6A */
      #define pdsmamod 3                       /* MAIN ENTRY POINT ADDRESSING MODE     @L6A */


      char           /*  XL1 */ pds2rlds;      /* NUMBER OF RLD/CONTROL RECORDS WHICH  @L6A */
    };
    struct {
      char           __filler0[2];
      unsigned char  /*  BL1 */ pds2ftb3;      /* BYTE 3 OF PDS2FTBO                   @L7C */


      #define pds2nmig 128                     /* THIS PROGRAM OBJECT CANNOT BE CONVERTED */
      #define pds2prim 64                      /* FETCHOPT PRIME WAS SPECIFIED         @L7A */
      #define pds2pack 32                      /* FETCHOPT PACK WAS SPECIFIED          @L7A */
      #define pdsbcend 33                      /* END OF BASIC SECTION */
      #define pdsbcln 33                       /* - LENGTH OF BASIC SECTION */
      #define pdss01 33                        /* START OF SCATTER LOAD SECTION */


    };
  };
  short int      /*  FL2 */ pds2slsz;      /* NUMBER OF BYTES IN SCATTER LIST */
  short int      /*  FL2 */ pds2ttsz;      /* NUMBER OF BYTES IN TRANSLATION TABLE */
  char           /*  CL2 */ pds2esdt[2];   /* IDENTIFICATION OF ESD ITEM (ESDID) OF */
  char           /*  CL2 */ pds2esdc[2];   /* IDENTIFICATION OF ESD ITEM (ESDID) OF */


  #define pdss01nd 41                      /* END OF SCATTER LOAD SECTION */
  #define pdss01ln 8                       /* - LENGTH OF SCATTER LOAD SECTION */
  #define pdss02 41                        /* START OF ALIAS SECTION */


  unsigned int   /*  RL3 */ pds2epm:24;    /* ENTRY POINT FOR MEMBER NAME */


  #define deentbk 41                       /* --- ALIAS */
  #define deentbk_length 3


  char           /*  CL8 */ pds2mnm[8];    /* MEMBER NAME OF PROGRAM. WHEN THE */


  #define pdss02nd 52                      /* END OF ALIAS SECTION */
  #define pdss02ln 11                      /* - LENGTH OF ALIAS SECTION */


  union {
    struct {
      short int      /*  HL2 */ pdss03;        /* FORCE HALF-WORD ALIGNMENT FOR SSI */
      char           /*  CL2 */ pdsmbrsn[2];   /* MEMBER SERIAL NUMBER */


      #define pdss03nd 56                      /* END OF SSI SECTION */
      #define pdss03ln 4                       /* LENGTH OF SSI SECTION */
      #define pdss04 56                        /* START OF APF SECTION */


    };
    char           /*  CL4 */ pdsssiwd[4];   /* SSI INFORMATION WORD */
    struct {
      unsigned char  /*  FL1 */ pdschlvl;      /* CHANGE LEVEL OF MEMBER */
      unsigned char  /*  BL1 */ pdsssifb;      /* SSI FLAG BYTE */


      #define pdsforce 64                      /* A FORCE CONTROL CARD WAS USED WHEN */
      #define pdsusrch 32                      /* A CHANGE WAS MADE TO MEMBER BY THE */
      #define pdsemfix 16                      /* SET WHEN AN EMERGENCY IBM-AUTHORIZED */
      #define pdsdepch 8                       /* A CHANGE MADE TO THE MEMBER IS DEPENDENT */
      #define pdssysgn 6                       /* FLAGS THAT INDICATE WHETHER A */
      #define pdsnosgn 0                       /* NOT CRITICAL FOR SYSTEM GENERATION */
      #define pdscmsgn 2                       /* MAY REQUIRE COMPLETE REGENERATION */
      #define pdsptsgn 4                       /* MAY REQUIRE PARTIAL REGENERATION */
      #define pdsibmmb 1                       /* MEMBER IS SUPPLIED BY IBM */


    };
  };
  union {
    char           /*  CL2 */ pdsapf[2];     /* PROGRAM AUTHORIZATION FACILITY (APF) */
    struct {
      unsigned char  /*  FL1 */ pdsapfct;      /* LENGTH OF PROGRAM AUTHORIZATION CODE */
      char           /*  CL1 */ pdsapfac;      /* PROGRAM AUTHORIZATION CODE */


      #define pdss04nd 58                      /* END OF APF SECTION */
      #define pdss04ln 2                       /* LENGTH OF APF SECTION */
      #define pdslpo 58                        /* START OF LARGE PROGRAM OBJECT SECTION@L7A */
      #define pdsllm 58                        /* ALTERNATE NAME FOR PDSLPO            @L7A */


    };
  };
  union {
    unsigned char  /*  FL1 */ pds2lpol;      /* LARGE PROGRAM OBJECT SECTION LENGTH  @L7A */
    struct {
      unsigned char  /*  FL1 */ pds2llml;      /* ALTERNATE NAME FOR PDS2LLML          @L7A */
      int            /*  FL4 */ pds2vstr;      /* VIRTUAL STORAGE REQUIREMENT FOR THIS */
      int            /*  FL4 */ pds2mepa;      /* MAIN ENTRY POINT OFFSET */
      int            /*  FL4 */ pds2aepa;      /* ALIAS ENTRY POINT OFFSET. ONLY VALID */


      #define pdslpond 71                      /* END OF LARGE PROGRAM OBJECT SECTION */
      #define pdsllmnd 71                      /* ALTERNATE NAME FOR PDSLPOND */
      #define pdslpoln 13                      /* LENGTH OF LLM SECTION              @L7A */
      #define pdsllmln 13                      /* ALTERNATE NAME FOR PDSLPOLN          @L7A */
      #define pds2xattr 71                     /* Start of extended attributes         @LBA */


      unsigned char  /*  FL1 */ pds2xattrbyte0; /* Extended attribute byte 0            @LBA */


      #define pds2xattr_optn_mask 15           /* Bits 4-7 of PDS2XATTRBYTE0 identify the   */


      unsigned char  /*  FL1 */ pds2xattrbyte1; /* Extended attribute byte 1            @LBA */


      #define pds2longparm 128                 /* PARM > 100 chars allowed             @LBA */
      #define pds2xattr_opt 74                 /* Start of optional fields. Number of       */


    };
  };
  char           __filler1[1];
};


#endif  /* pds__ */


#pragma pack(reset)


