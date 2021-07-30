#pragma pack(packed)

#ifndef symbp__
#define symbp__

struct symbp {
  union  {
    void * __ptr32 /*  AL4 */ symbppatterna; /* Address of input pattern containing symbols */
    struct {
      void * __ptr32 /*  AL4 */ symbppatternaddr; /* Same as SymbpPattern@ */
      int            /*  FL4 */ symbppatternlength; /* Length of input pattern */
      void * __ptr32 /*  AL4 */ symbptargeta;  /* Address of output target area. The target, */
    };
    struct {
      char           __filler0[8];
      void * __ptr32 /*  AL4 */ symbptargetaddr; /* Same as SymbpTarget@ */
      union {
        void * __ptr32 /*  AL4 */ symbptargetlengtha; /* Address of input output fullword field */
        struct {
          void * __ptr32 /*  AL4 */ symbptargetlengthaddr; /* Same as SymbpTargetLength */
          void * __ptr32 /*  AL4 */ symbpsymboltablea; /* Address of symbol table mapped by SYMBT */
        };
      };
    };
    struct {
      char           __filler1[16];
      void * __ptr32 /*  AL4 */ symbpsymboltableaddr; /* Same as SymbpSymbolTable */
      union {
        void * __ptr32 /*  AL4 */ symbptimestampa; /* Address of 8-character area containing the */
        struct {
          void * __ptr32 /*  AL4 */ symbptimestampaddr; /* Same as SymbpTimeStamp@ */
          union {
            int            /*  FL4 */ symbpreturncodea; /* Address of fullword which is to contain the */
            void * __ptr32 /*  AL4 */ symbpreturncodeaddr; /* Same as SymbpReturnCode */

            #define symbp_len 28

          };
        };
      };
    };
  };
};

#endif  /* symbp__ */

#ifndef symbfp__
#define symbfp__

struct symbfp {
  union {
    void * __ptr32 /*  AL4 */ symbfppatterna; /* Address of input pattern containing symbols */
    struct {
      void * __ptr32 /*  AL4 */ symbfppatternaddr; /* Same as SymbfpPattern@ */
      int            /*  FL4 */ symbfppatternlength; /* Length of input pattern */
      void * __ptr32 /*  AL4 */ symbfptargeta; /* Address of output target area. The target, */
    };
    struct {
      char           __filler0[8];
      void * __ptr32 /*  AL4 */ symbfptargetaddr; /* Same as SymbfpTarget@ */
      union {
        void * __ptr32 /*  AL4 */ symbfptargetlengtha; /* Address of input output fullword field */
        struct {
          void * __ptr32 /*  AL4 */ symbfptargetlengthaddr; /* Same as SymbfpTargetLength@ */
          void * __ptr32 /*  AL4 */ symbfpsymboltablea; /* Address of symbol table mapped by SYMBT */
        };
      };
    };
    struct {
      char           __filler1[16];
      void * __ptr32 /*  AL4 */ symbfpsymboltableaddr; /* Same as SymbfpSymbolTable */
      union {
        void * __ptr32 /*  AL4 */ symbfptimestampa; /* Address of 8-character area containing the */
        struct {
          void * __ptr32 /*  AL4 */ symbfptimestampaddr; /* Same as SymbfpTimeStamp */
          void * __ptr32 /*  AL4 */ symbfpreturncodea; /* Address of fullword which is to contain the */
        };
      };
    };
    struct {
      char           __filler2[24];
      void * __ptr32 /*  AL4 */ symbfpreturncodeaddr; /* Same as SymbfpReturnCode */
      void * __ptr32 /*  AL4 */ symbfpworkareaaddr; /* Address of 1024-byte work area on a */

      #define symbfp_len 32

    };
  };
};

#endif  /* symbfp__ */

#ifndef symbt__
#define symbt__

struct symbt {
  union {
    char           /*  CL4 */ symbtheader[4];
    short int      /*  BL2 */ symbtflags;
    struct {
      struct {
        unsigned char  /*  BL1 */ symbtflag0;    /* Byte 0 of SymbtFlags */

        #define symbtnodefaultsymbols 128        /* Avoid using the default symbol set */
        #define symbtonlystaticsymbols 64        /* Allow only static symbols */
        #define symbttimestampisgmt 32           /* The input timestamp is GMT-time, not */
        #define symbttimestampislocal 16         /* The input timestamp is Local-Time, not */
        #define symbtwarnsubstrings 8            /* When a substring problem is encountered, */
        #define symbtchecknullsubtext 4          /* The presence of null sub-text will be */
        #define symbtptrsareoffsets 2            /* The pointer fields within the */
        #define symbtonlydynamicsymbols 1        /* Allow only dynamic symbols. This */

        unsigned char  /*  BL1 */ symbtflag1;    /* Byte 1 of SymbtFlags */

        #define symbtflag1rsv1 128               /* Reserved. Must be zero. Do not use. */
        #define symbttimestampisstck 64          /* The input timestamp is from the STCK */
        #define symbtwarnnosub 32                /* When no substitution at all has occurred, */
        #define symbtindirectsymbolarea 16       /* Indicates that the symbol area is */
        #define symbtmixedcasesymbols 8          /* Indicates that the input may have */
        #define symbtflag1rsv2 6                 /* Unused. Must be zero. */
        #define symbtsymbt1 1                    /* When this bit is off, the SYMBT DSECT applies */

      };
      struct {
        short int      /*  HL2 */ symbtnumberofsymbols; /* Number of entries in symbol table. Can be 0. */
        char           /* 0CL1 */ symbttableentries[0]; /* Symbol table entries. One for each indicated */

        #define symbtmaxstaticsymbollengthzosv2r2 16 /* The max length of a static */
        #define symbtmaxstaticsymbollength 8     /* The max length of a static symbol, */
        #define symbtmaxstaticentriesprezosr4 103 /* The max number of full-sized */
        #define symbtmaxstaticentrieszosv2r2_8 1631 /* The max number of 8-byte-name */
        #define symbtmaxstaticentrieszosv2r2_16 1119 /* The max number of */
        #define symbtmaxstaticentrieszosv2r2_44 731 /* The max number of 16-byte-name */
        #define symbtmaxstaticentrieszosr4 928   /* The max number of full-sized */
        #define symbtmaxstaticentrydatalengthzosv2r2 62 /* Name with "&" and "." */
        #define symbtmaxstaticentries 928        /* The max number of pre-z/OS2.2 full-sized */
        #define symbtmaxstaticsubtextlengthzosv2r2 17 /* The max length of */
        #define symbtmaxstaticlongsubtextlengthzosv2r2 44 /* The max length of */
        #define symbtmaxstaticsubtextlength 9    /* The max length of substitution text */
        #define symbtmaxstatictablesizeprezosr4 3609 /* Name with "&" and "." The max */
        #define symbtmaxstatictablesizezosv2r2 57088 /* The max table size, taking */
        #define symbtmaxstatictablesizeprezosv2r2 32512 /* The max table size, taking */
        #define symbtmaxstatictablesizezosr4 32512 /* The max table size, taking into */
        #define symbtmaxstatictablesize 32512
        #define symbt_len 4

      };
    };
  };
};

#endif  /* symbt__ */

#ifndef symbt1__
#define symbt1__

struct symbt1 {
  union {
    char           /*  CL16 */ symbt1header[16];
    short int      /*  BL2 */ symbt1flags;
    struct {
      struct {
        unsigned char  /*  BL1 */ symbt1flag0;   /* Byte 0 of Symbt1Flags */

        #define symbt1nodefaultsymbols 128       /* Avoid using the default symbol set. */
        #define symbt1onlystaticsymbols 64       /* Allow only static symbols. If there */
        #define symbt1timestampisgmt 32          /* The input timestamp is GMT-time, not */
        #define symbt1timestampislocal 16        /* The input timestamp is Local-Time, */
        #define symbt1warnsubstrings 8           /* When a substring problem is */
        #define symbt1checknullsubtext 4         /* The presence of null sub-text will be */
        #define symbt1ptrsareoffsets 2           /* The pointer fields within the */
        #define symbt1onlydynamicsymbols 1       /* Allow only dynamic symbols. This */

        unsigned char  /*  BL1 */ symbt1flag1;   /* Byte 1 of Symbt1Flags */

        #define symbt1flag1rsv1 128              /* Reserved. Must be zero. Do not use. */
        #define symbt1timestampisstck 64         /* The input timestamp is from the STCK */
        #define symbt1warnnosub 32               /* When no substitution at all has occurred, */
        #define symbt1indirectsymbolarea 16      /* Indicates that the symbol area is */
        #define symbt1mixedcasesymbols 8         /* Indicates that the input may have */
        #define symbt1flag1rsv2 6                /* Unused. Must be zero. */
        #define symbt1symbt1 1                   /* When this bit is on, the SYMBT1 DSECT applies */

      };
      struct {
        unsigned char  /*  BL1 */ symbt1flag2;

        #define symbt1preservealignment 128      /* Indicates that an attempt is to be */
        #define symbt1nodoubleampersand 64       /* Indicates that &&symbol in the */
        #define symbt1iefsjsym 32                /* The symbol table area is an area returned by */
        #define symbt1continueafterfull 16       /* If the target buffer does not have */

        unsigned char  /*  BL1 */ symbt1flag3;

        #define symbt1jessymbols 1               /* This bit is intended for IBM use only. When */

        void * __ptr32 /*  AL4 */ symbt1nextsymbtaddr; /* Address of next SYMBT1 or SYMBT so that the */
        char           __filler0[6];
        short int      /*  HL2 */ symbt1numberofsymbols; /* Number of entries in symbol table. Can be */
        char           /* 0CL1 */ symbt1tableentries[0]; /* Symbol table entries. One for each indicated */

        #define symbt1_len 16

      };
    };
  };
};

#endif  /* symbt1__ */

#ifndef symbte__
#define symbte__

struct symbte {
  union {
    char           /*  CL16 */ symbtetableentries[16]; /* Symbol table entry. One such entry for each */
    void * __ptr32 /*  AL4 */ symbtesymbolptr; /* Address of symbol. Do not use when bit */
    int            /*  FL4 */ symbtesymboloffset; /* Offset to symbol from start of symbol area. */
    struct {
      void * __ptr32 /*  AL4 */ symbtesymbolareaaddr; /* Address of symbol area when */
      int            /*  FL4 */ symbtesymbollength; /* Length of symbol (includes preceding "&" and */
      void * __ptr32 /*  AL4 */ symbtesubtextptr; /* Address of substitution text. Do not use when */
    };
    struct {
      char           __filler0[8];
      int            /*  FL4 */ symbtesubtextoffset; /* Offset to substitution text from start of */
      int            /*  FL4 */ symbtesubtextlength; /* Length of substitution text */

      #define symbte_len 16

    };
  };
};

#endif  /* symbte__ */

#ifndef symbth__
#define symbth__

struct symbth {
  char           /*  CL4 */ symbthnotinterface[4];
  int            /*  FL4 */ symbthsymboltablelen; /* The length of the symbol table (not */

  #define symbth_len 8

};

#endif  /* symbth__ */


#pragma pack(reset)

