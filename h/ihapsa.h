#pragma pack(packed)

#ifndef psa__
#define psa__

struct psa {
  union {
    struct {

      #define flc 0

      union {
        char           /*  CL8 */ flcippsw[8];   /* -         IPL PSW */
        char           /*  XL4 */ flcrnpsw[4];   /* -RESTART NEW PSW (AFTER IPL)        MDC001 */

        #define iplpsw 0                         /* --- ALIAS */
        #define iplpsw_length 8

      };
      union {
        char           /*  CL8 */ flciccw1[8];   /* -         IPL CCW1 */
        char           /*  XL8 */ flcropsw[8];   /* -      RESTART OLD PSW (AFTER IPL) */
      };
      union {
        char           /*  CL8 */ flciccw2[8];   /* -         IPL CCW2 */
        void * __ptr32 /*  VL4 */ flccvt;        /* -    ADDRESS OF CVT (AFTER IPL).  THIS OFFSET */
      };
      char           /*  XL8 */ flceopsw[8];   /* -      EXTERNAL OLD PSW */

      #define exopsw 24                        /* --- ALIAS */
      #define exopsw_length 8

      char           /*  XL8 */ flcsopsw[8];   /* -      SVC OLD PSW.  THIS OFFSET FIXED BY */

      #define svcopsw 32                       /* --- ALIAS */
      #define svcopsw_length 8

      char           /*  XL8 */ flcpopsw[8];   /* -      PROGRAM CHECK OLD PSW */

      #define piopsw 40                        /* --- ALIAS */
      #define piopsw_length 8

      char           /*  XL8 */ flcmopsw[8];   /* -      MACHINE CHECK OLD PSW */

      #define mcopsw 48                        /* --- ALIAS */
      #define mcopsw_length 8

      char           /*  XL8 */ flciopsw[8];   /* -      INPUT/OUTPUT OLD PSW */

      #define ioopsw 56                        /* --- ALIAS */
      #define ioopsw_length 8

      char           __filler0[8];
      union {
        double         /*  DL8 */ flccvt64;      /* -      8-byte CVT address                   @H3A */
        struct {
          char           __filler1[4];
          void * __ptr32 /*  VL4 */ flccvt2;       /* -    ADDRESS OF CVT - USED BY DUMP */
        };
      };
      char           __filler2[8];
      char           /*  XL4 */ flcenpsw[4];   /* -EXTERNAL NEW PSW */

      #define exnpsw 88                        /* --- ALIAS */
      #define exnpsw_length 4

      char           __filler3[4];
      char           /*  XL4 */ flcsnpsw[4];   /* -SVC NEW PSW */

      #define svcnpsw 96                       /* --- ALIAS */
      #define svcnpsw_length 4

      char           __filler4[4];
      char           /*  XL4 */ flcpnpsw[4];   /* - PROGRAM CHECK NEW PSW, DISABLED FOR @02C */

      #define pinpsw 104                       /* --- ALIAS */
      #define pinpsw_length 4

      char           __filler5[4];
      char           /*  XL4 */ flcmnpsw[4];   /* -MACHINE CHECK NEW PSW              MDC003 */

      #define mcnpsw 112                       /* --- ALIAS */
      #define mcnpsw_length 4

      char           __filler6[4];
      char           /*  XL4 */ flcinpsw[4];   /* -INPUT/OUTPUT NEW PSW */

      #define ionpsw 120                       /* --- ALIAS */
      #define ionpsw_length 4

      char           __filler7[4];
      int            /*  FL4 */ psaeparm;      /* -         EXTERNAL INTERRUPTION PARAMETER  @G871A9A */
      union {
        int            /*  FL4 */ psaeepsw;      /* -           EXTENDED PSW DATA STORED ON EXTERNAL */
        struct {
          short int      /*  HL2 */ psaspad;       /* -         ISSUING PROCESSOR'S PHYSICAL ADDRESS */
          short int      /*  HL2 */ flceicod;      /* -         EXTERNAL INTERRUPTION CODE */

          #define excode 134                       /* --- ALIAS */
          #define excode_length 2

        };
      };
      union {
        int            /*  FL4 */ psaespsw;      /* -           EXTENDED PSW DATA STORED ON SVC */
        struct {
          char           __filler8[1];
          struct {
            unsigned char  /*  FL1 */ flcsvilc;      /* -       SVC INSTRUCTION LENGTH COUNTER - NUMBER */

            #define flcsilcb 7                       /* -        SIGNIFICANT BITS IN ILC FIELD - LAST */
            #define svcilc 137                       /* --- ALIAS */

            short int      /*  HL2 */ flcsvcn;       /* -         SVC INTERRUPTION CODE - SVC NUMBER. */

            #define svcnum 138                       /* --- ALIAS */
            #define svcnum_length 2

          };
        };
      };
      union {
        char           /*  CL8 */ psaeppsw[8];   /* -         EXTENDED PSW FOR PROGRAM INTERRUPT MDC086 */
        struct {
          char           __filler9[1];
          struct {
            unsigned char  /*  FL1 */ flcpiilc;      /* -       PROGRAM INTERRUPT LENGTH COUNTER - NUMBER */

            #define flcpilcb 7                       /* -        SIGNIFICANT BITS IN ILC FIELD - LAST */
            #define piilc 141                        /* --- ALIAS */

            short int      /*  HL2 */ flcpicod;      /* -           PROGRAM INTERRUPTION CODE */

            #define picode 142                       /* --- ALIAS */
            #define picode_length 2

          };
        };
        struct {
          char           __filler10[2];
          struct {
            unsigned char  /*  FL1 */ psaeecod;      /* -       EXCEPTION-EXTENSION CODE.            @03C */
            unsigned char  /*  FL1 */ psapicod;      /* -       8-BIT INTERRUPT CODE.  THIS OFFSET FIXED */

            #define psapiper 128                     /* -        PER INTERRUPT OCCURRED             MDC089 */
            #define psapimc 64                       /* -        MONITOR CALL INTERRUPT OCCURRED    MDC090 */
            #define psapipc 63                       /* -        AN UNSOLICITED PROGRAM CHECK HAS */

            int            /*  FL4 */ flctea;        /* -      TRANSLATION EXCEPTION ADDRESS.  THIS @L8C */

            #define flcteaxm 128                     /* -      IF 0 FLCTEA IS RELATIVE TO THE PRIMARY */

          };
        };
        struct {
          char           __filler11[7];
          char           /*  XL1 */ flcdxc;        /* -      Data exception code for PI 7         @LRA */
        };
        struct {
          char           __filler12[7];
          char           /*  XL1 */ flcteab3;      /* -      LAST BYTE OF TEA.                    @L8A */

          #define flcsopi 4                        /* -      Suppression on protection flag       @LQA */
          #define flctstdp 0                       /* -      IF 1, THE PRIMARY STD WAS USED.      @L8A */
          #define flctstda 1                       /* -      IF 1, THE STD WAS AR QUALIFIED.      @L8A */
          #define flctstds 2                       /* -      IF 1, THE SECONDARY STD WAS USED.    @L8A */
          #define flctstdh 3                       /* -      IF 1, THE HOME STD WAS USED.         @L8A */
          #define flcteacl 2147479552              /* Mask to leave only TEA address       @LSA */

        };
      };
      char           __filler13[1];
      char           /*  XL1 */ flcmcnum;      /* -      MONITOR CLASS NUMBER */
      char           /*  XL1 */ flcpercd;      /* -      PROGRAM EVENT RECORDING CODE */
      char           /*  XL1 */ flcatmid;      /* -      ATM ID                               @LSA */

      #define flcpswb4 128                     /* PSW.4 part of ATMID                  @LSA */

      void * __ptr32 /*  AL4 */ flcper;        /* -         PER ADDRESS - ESA/390            @G860PXK */
      char           __filler14[1];
      char           /*  XL3 */ flcmtrcd[3];   /* -      MONITOR CODE (ESA/390) */
      char           /*  XL1 */ flctearn;      /* -      CONTAINS THE ACCESS REGISTER NUMBER  @L8C */
      char           /*  XL1 */ flcperrn;      /* -      CONTAINS THE PER STORAGE ACCESS      @L8A */
      char           __filler15[1];
      char           /*  XL1 */ flcarch;       /* -      Architecture information             @LSA */

      #define psazarch 1                       /* -      z/Architecture                       @LSA */
      #define psaesame 1                       /* -      z/Architecture                       @LSA */

      void * __ptr32 /*  VL4 */ psampl;        /* -    MPL ADDRESS.  THIS OFFSET FIXED BY */
      char           __filler16[16];
      union {
        char           /*  XL8 */ flciocdp[8];   /* -         I/O INFORMATION CODE             @G860PVB */
        struct {
          char           /*  XL4 */ flcsid[4];     /* -      SUBSYSTEM ID                     @G860PVB */
          char           /*  XL4 */ flciofp[4];    /* -      I/O INTERRUPTION PARAMETER       @G860PVB */
        };
      };
      char           __filler17[8];
      union {
        char           /*  XL16 */ flcfacl[16];  /* -     Facilities List. See FaclBytes0To15 */
        struct {
          char           /*  XL1 */ flcfacl0;      /* Byte 0 of FLCFACL                    @LVA */

          #define flcfn3 128                       /* -     N3 installed                         @LVA */
          #define flcfzari 64                      /* -     z/Architecture installed             @LVA */
          #define flcfzara 32                      /* -     z/Architecture active                @LVA */
          #define flcfaslx 2                       /* -     ASN & LX reuse facility installed    @LVA */

          char           /*  XL1 */ flcfacl1;      /* Byte 1 of FLCFACL                    @LVA */

          #define flcfedat 128                     /* DAT features                         @0BA */
          #define flcfsrs 64                       /* Sense-running-status                 @LZA */
          #define flcfsske 32                      /* Cond. SSKE instruction installed     @0AA */
          #define flcfctop 16                      /* STSI-enhancement                     @LYA */

          char           /*  XL1 */ flcfacl2;      /* Byte 2 of FLCFACL                    @LVA */

          #define flcfetf2 128                     /* Extended Translation facility 2      @LVA */
          #define flcfcrya 64                      /* Cryptographic assist                 @LVA */
          #define flcfld 32                        /* Long Displacement facility           @LVA */
          #define flcfldhp 16                      /* Long Displacement High Performance   @LVA */
          #define flcfhmas 8                       /* HFP Multiply Add/Subtract            @LVA */
          #define flcfeimm 4                       /* Extended immediate when z/Arch       @LVA */
          #define flcfetf3 2                       /* Extended Translation Facility 3 when @LVA */
          #define flcfhun 1                        /* HFP unnormalized extension           @LVA */

          char           /*  XL1 */ flcfacl3;      /* Byte 3 of FLCFACL                    @LVA */

          #define flcfet2e 128                     /* ETF2-enhancement                   031215 */
          #define flcfstkf 64                      /* STCKF-enhancement                    @PIA */
          #define flcfet3e 2                       /* ETF3-enhancement                   040512 */
          #define flcfect 1                        /* ECT-facility                         @LXA */

          char           /*  XL1 */ flcfacl4;      /* Byte 4 of FLCFACL                    @PHA */

          #define flcfcssf 128                     /* Compare-and-swap-and-store           @LXA */
          #define flcfcsf2 64                      /* Compare-and-swap-and-store 2         @LXA */
          #define flcfgief 32                      /* General-Instructions-Extension       @M0A */
          #define flcfocm 1                        /* Obsolete CPU-measurement facility. Use */

          char           /*  XL1 */ flcfacl5;      /* Byte 5 of FLCFACL                    @PHA */

          #define flcffpse 64                      /* Floating-point-support enhancement   @PMA */
          #define flcfdfp 32                       /* Decimal-floating-point               @PMA */
          #define flcfdfph 16                      /* Decimal-floating-point high performance */
          #define flcfpfpo 8                       /* PFPO instruction                   070424 */

          char           /*  XL1 */ flcfacl6;      /* Byte 6 of FLCFACL                    @PHA */
          char           /*  XL1 */ flcfacl7;      /* Byte 7 of FLCFACL                    @PHA */
          char           /*  XL1 */ flcfacl8;      /* Byte 8 of FLCFACL                    @M4A */

          #define flcfcaai 64                      /* Crypto AP-Queue adapter interruption @M5A */
          #define flcfcmc 16                       /* CPU-measurement counter facility     @M4A */
          #define flcfcms 8                        /* CPU-measurement sampling facility    @M4A */
          #define flcfsclp 4                       /* Possible future enhancement          @M7A */
          #define flcfaisi 2                       /* AISI facility                        @PPA */
          #define flcfaen 1                        /* AEN  facility                        @PPA */

          char           /*  XL1 */ flcfacl9;      /* Byte 9 of FLCFACL                    @PPA */

          #define flcfais 128                      /* AIS  facility                        @PPA */

        };
      };
      union {
        char           /*  XL16 */ flcfacle[16]; /* -     Facilities List bytes 16-31. See     @MMA */
        struct {
          char           /*  XL8 */ flcctsa[8];    /* -      CPU-TIMER SAVE AREA (FROM STORE-     @LHA */
          char           /*  XL8 */ flcccsa[8];    /* -      CLOCK-COMPARATOR SAVE AREA (FROM     @LHA */
          char           /*  XL8 */ flcmcic[8];    /* -      MACHINE-CHECK INTERRUPTION CODE  @G860PVB */
          char           __filler18[8];
          void * __ptr32 /*  AL4 */ flcfsa;        /* -         FAILING STORAGE ADDRESS          @G860PXK */
          char           __filler19[4];
          char           /*  XL16 */ flcfla[16];   /* -     FIXED LOGOUT AREA. SIZE FIXED BY     @L9C */
          char           /*  XL16 */ flcrv110[16]; /* -     RESERVED.                            @L9A */
          int            /* 16FL4 */ flcarsav[16]; /* -     ACCESS REGISTER SAVE AREA            @L9A */
          char           /*  XL32 */ flcfpsav[32]; /* -     FLOATING POINT REGISTER SAVE AREA */
          int            /* 16FL4 */ flcgrsav[16]; /* -       GENERAL REGISTER SAVE AREA */
          int            /* 16FL4 */ flccrsav[16]; /* -       CONTROL REGISTER SAVE AREA */
          union {
            double         /*  DL8 */ flchdend;      /* -           END OF HARDWARE ASSIGNMENTS */
            struct {
              char           /*  CL4 */ psapsa[4];     /* -    CONTROL BLOCK ACRONYM IN EBCDIC */
              short int      /*  HL2 */ psacpupa;      /* -         PHYSICAL CPU ADDRESS (CHANGED DURING ACR) */
              short int      /*  HL2 */ psacpula;      /* -         LOGICAL CPU ADDRESS */
            };
          };
          void * __ptr32 /*  AL4 */ psapccav;      /* -         VIRTUAL ADDRESS OF PCCA */
          void * __ptr32 /*  AL4 */ psapccar;      /* -         REAL ADDRESS OF PCCA */
          void * __ptr32 /*  AL4 */ psalccav;      /* -         VIRTUAL ADDRESS OF LCCA */
          void * __ptr32 /*  AL4 */ psalccar;      /* -         REAL ADDRESS OF LCCA */
          void * __ptr32 /*  AL4 */ psatnew;       /* -         TCB pointer. Field maintained for code */

          #define ieatcbp 536                      /* -      ALIAS */
          #define ieatcbp_length 4

          void * __ptr32 /*  AL4 */ psatold;       /* -         Pointer to current TCB or zero if in SRB */
          void * __ptr32 /*  VL4 */ psaanew;       /* ASCB pointer.  Field maintained for code */
          void * __ptr32 /*  AL4 */ psaaold;       /* -         Pointer to the home (current) ASCB. */
          union {
            int            /*  BL4 */ psasuper;      /* -         SUPERVISOR CONTROL WORD.  THIS OFFSET */
            struct {
              char           /*  XL1 */ psasup1;       /* -      FIRST BYTE OF PSASUPER */

              #define psaio 128                        /* -        I/O FLIH */
              #define psasvc 64                        /* -        SVC FLIH */
              #define psaext 32                        /* -        EXTERNAL FLIH */
              #define psapi 16                         /* -        PROGRAM CHECK FLIH */
              #define psalock 8                        /* -        LOCK ROUTINE */
              #define psadisp 4                        /* -        DISPATCHER */
              #define psatctl 2                        /* -        TCTL RECOVERY FLAG  (MDC310)     @Z40FP9A */
              #define psatype6 1                       /* -        TYPE 6 SVC IN CONTROL  (MDC311)  @Z40FP9A */

              char           /*  XL1 */ psasup2;       /* -      SECOND BYTE OF PSASUPER */

              #define psaipcri 128                     /* -        REMOTE IMMEDIATE SIGNAL SERVICE ROUTINE */
              #define psasvcr 64                       /* -        SUPER FRR USES FOR SVC FLIH      @ZMC3227 */
              #define psasvcrr 32                      /* -        SVC RECOVERY RECURSION INDICATOR. */
              #define psaacr 4                         /* -        AUTOMATIC CPU RECONFIGURATION (ACR) IN */
              #define psartm 2                         /* -        RECOVERY TERMINATION MONITOR (RTM) IN */
              #define psalcr 1                         /* -        USED BY RTM TO SERIALIZE CALLS OF    @L5C */

              char           /*  XL1 */ psasup3;       /* -      THIRD BYTE OF PSASUPER */

              #define psaiosup 128                     /* -        IF ON, A MAINLINE IOS COMPONENT SUCH AS */
              #define psaspr 16                        /* -        SUPER FRR IS ACTIVE  (MDC305)    @ZA02995 */
              #define psaesta 8                        /* -        SVC 60 RECOVERY ROUTINE ACTIVE */
              #define psarsm 4                         /* -        REAL STORAGE MANAGER (RSM) ENTERED FOR */
              #define psaulcms 2                       /* -        LOCK MANAGER UNCONDITIONAL LOCAL OR */
              #define psaslip 1                        /* -        IEAVTSLP RECURSION CONTROL BIT */

              char           /*  XL1 */ psasup4;       /* -      FOURTH BYTE OF PSASUPER */

              #define psaldwt 128                      /* -        BLWLDWT IS IN CONTROL TO LOAD A      @LHC */
              #define psasmf 64                        /* -        SMF SUSPEND/RESET     (MDC599)   @G743PBB */
              #define psaesar 32                       /* -        SUPERVISOR ANALYSIS ROUTER IS ACTIVE @L5C */
              #define psamch 16                        /* -        Machine Check Handler is active.     @PKA */

            };
          };
          char           /*  XL9 */ psarv22c[9];   /* -     RESERVED                             @xxC */
          char           /*  XL2 */ psa_workunit_cbf_atdisp[2]; /* @MHA */
          char           /*  XL1 */ psarv237;      /* -     RESERVED                             @MKC */
          char           /*  XL2 */ psa_workunit_procclassatdisp[2]; /* -                          @MCA */
        };
      };
    };
    struct {
      char           __filler20[568];
      char           /*  XL1 */ psa_workunit_procclassatdisp_byte0; /* @MCA */
      char           /*  XL1 */ psa_workunit_procclassatdisp_byte1; /* @MCA */
      union {
        char           /*  XL2 */ psaprocclass[2]; /* -     PROCESSOR WUQ Offset. */
        char           /*  XL2 */ psa_bylpar_procclass[2]; /* - PROCESSOR WUQ Offset.            @H5A */
        struct {
          char           /*  XL1 */ psaprocclass_byte0; /* @H4A */
          char           /*  XL1 */ psaprocclass_byte1; /* @H4A */

          #define psaprocclass_cp 0                /* Standard CP. 0 is offset to SWUQ     @H4A */
          #define psaprocclass_zaap 2              /* zAAP.                                @H4A */
          #define psaprocclass_ziip 4              /* zIIP.                                @H5A */
          #define psaprocclass_sup 4               /* zIIP.                                @H4A */
          #define psaprocclassconverter 2          /* Procclass conversion factor          @0EA */
          #define psamaxprocclass 4                /* PSA Max procclass                    @0EA */
          #define psamaxprocclassindex 2

        };
        struct {
          char           /*  XL1 */ psa_bylpar_procclass_byte0; /* @H5A */
          char           /*  XL1 */ psa_bylpar_procclass_byte1; /* @H5A */
          char           /*  XL1 */ psaptype;      /* -      PROCESSOR TYPE INDICATOR             @H1A */

          #define psaifa 64                        /* Indicates Special Processor          @H3C */
          #define psa_bylpar_zaap 64               /* @H5A */
          #define psa_bylpar_ifa 64                /* @H5A */
          #define psaifads 32                      /* zAAP (IFA) that is different */
          #define psadscrp 16                      /* Discretionary Processor              @LYA */
          #define psaziip 8                        /* zIIP                                 @H4A */
          #define psa_bylpar_ziip 8                /* @H5A */
          #define psasup 8                         /* zIIP                                 @H4A */
          #define psa_bylpar_sup 8                 /* @H5A */
          #define psaziipds 4                      /* zIIP that is different speed than CP @H4A */
          #define psasupds 4                       /* zIIP that is different speed than CP @H4A */

          char           /*  XL1 */ psails;        /* -      INTERRUPT HANDLER LINKAGE STACK      @L9C */

          #define psailsio 128                     /* -      THE I/O FLIH IS USING THE            @L9A */
          #define psailsex 64                      /* -      THE EXTERNAL FLIH IS USING THE       @L9A */
          #define psailspc 32                      /* -      THE PROGRAM FLIH IS USING THE        @L9A */
          #define psailsds 16                      /* -      THE DISPATCHER IS USING THE          @L9A */
          #define psailsrs 8                       /* -      THE RESTART FLIH IS USING THE        @L9A */
          #define psailsor 4                       /* -      EXIT IS USING THE INTERRUPT HANDLER  @LAA */
          #define psailst6 2                       /* -      TYPE 6 SVC IS USING THE INTERRUPT    @D2A */
          #define psailslk 1                       /* -      THE INTERRUPT HANDLER LINKAGE STACK  @D4A */

          char           /*  XL2 */ psalsvci[2];   /* -      LAST SVC ISSUED ON THIS PROCESSOR    @L6A */
          char           /*  XL1 */ psaflags;      /* -      SYSTEM FLAGS                         @LOA */

          #define psaaeit 128                      /* -      ADDRESSING ENVIRONMENT IS IN         @LOA */
          #define psatx 8                          /* Equivalent to CVTTX                  @MBA */
          #define psatxc 4                         /* Equivalent to CVTTXC                 @MBA */

          char           /*  XL10 */ psarv241[10]; /* RESERVED FOR FUTURE USE - SC1C5.     @LOC */
          char           /*  XL1 */ psascaff;      /* $$SCAFFOLD */

          #define psaemema 128                     /* $$SCAFFOLD: z/Architecture */

          void * __ptr32 /*  AL4 */ psalkcrf;      /* LINKAGE STACK POINTER SAVE AREA.     @D4A */
          char           /*  XL8 */ psampsw[8];    /* - SETLOCK MODEL PSW */

          #define psapiom 2                        /* INPUT/OUTPUT INTERRUPT MASK      @G860PXK */
          #define psapexm 1                        /* EXTERNAL INTERRUPT MASK          @G860PXK */

          char           /*  XL8 */ psaicnt[8];    /* -      Number of instructions executed at */
          int            /*  FL4 */ psatclin;      /* -            STOSM PSASLSA,X'00' INSTRUCTION      @P5A */
          int            /*  FL4 */ psaintin;      /* -            STOSM PSASLSA,X'00' INSTRUCTION      @P5A */
          int            /*  FL4 */ psaipcin;      /* -            STOSM PSASLSA,X'00' INSTRUCTION      @P5A */

          #define psaipcsm 617                     /* - LABEL FOR SYSTEM MASK USED IN ABOVE */

          int            /*  FL4 */ psaems2s;      /* -            STOSM PSASLSA,X'00' INSTRUCTION      @P5A */

          #define psaems2m 621                     /* - LABEL OF SYSTEM MASK USED IN ABOVE */

          int            /*  FL4 */ psastosm;      /* -            STOSM PSASLSA,X'00' INSTRUCTION.     @P5A */

          #define psastssm 625                     /* - LABEL FOR SYSTEM MASK USED IN ABOVE */

          int            /*  FL4 */ psahlhis;      /* -         SAVE AREA FOR PSAHLHI              MDC050 */
          char           /*  XL1 */ psarecur;      /* -      RESTART FLIH RECURSION INDICATOR.  IF */
          char           /*  XL1 */ psarssm;       /* -      STNSM AREA FOR IEAVERES              @L5C */
          char           /*  XL1 */ psasnsm2;      /* -      STNSM AREA FOR IEAVTRT1 (MDC470) @G65RP9A */
          char           /*  XL1 */ psartm1s;      /* -      BITS 0-7 OF THE CURRENT PSW ARE  @G383P9A */
          void * __ptr32 /*  AL4 */ psalwtsa;      /* -         REAL ADDRESS OF SAVE AREA USED WHEN  @LHC */
          union {
            char           /*  CL116 */ psaclht[116]; /* -       CPU LOCKS TABLE  (MDC314)        @G860PXH */
            struct {
              union {
                char           /*  CL80 */ psaclht1[80]; /* -        SPIN LOCKS TABLE                 @G860PXH */
                struct {
                  void * __ptr32 /*  VL4 */ psadispl;      /* -  GLOBAL DISPATCHER LOCK  (MDC315) @G50DP9A */
                  void * __ptr32 /*  AL4 */ psaasml;       /* -         AUXILIARY STORAGE MANAGEMENT (ASM) LOCK */
                  void * __ptr32 /*  VL4 */ psasalcl;      /* -  SPACE ALLOCATION LOCK  (MDC316)  @G50DP9A */
                  void * __ptr32 /*  AL4 */ psaiossl;      /* -         IOS SYNCHRONIZATION LOCK           MDC010 */
                  void * __ptr32 /*  AL4 */ psarsmdl;      /* -         ADDRESS OF THE RSM DATA SPACE LOCK   @LBC */
                  void * __ptr32 /*  AL4 */ psaiosul;      /* -         IOS UNIT CONTROL BLOCK LOCK        MDC005 */
                  void * __ptr32 /*  AL4 */ psarvlk1;      /* -         RESERVED FOR LOCK EXPANSION      @G860PXK */
                  void * __ptr32 /*  AL4 */ psarv29c;      /* -         RESERVED FOR LOCK EXPANSION          @LDC */
                  void * __ptr32 /*  AL4 */ psarv2a0;      /* -         RESERVED FOR LOCK EXPANSION          @LDC */
                  void * __ptr32 /*  AL4 */ psatpacl;      /* -         TCAM'S TPACBDEB LOCK               MDC009 */
                  void * __ptr32 /*  VL4 */ psaoptl;       /* -   OPTIMIZER LOCK  (MDC317)         @G50DP9A */
                  void * __ptr32 /*  AL4 */ psarsmgl;      /* -         RSM GLOBAL LOCK                  @G860PXH */
                  void * __ptr32 /*  VL4 */ psavfixl;      /* VSM FIXED SUBPOOLS LOCK          @G860PXH */
                  void * __ptr32 /*  AL4 */ psaasmgl;      /* -         ASM GLOBAL LOCK                  @G860PXH */
                  void * __ptr32 /*  AL4 */ psarsmsl;      /* -         RSM STEAL LOCK                   @G860PXH */
                  void * __ptr32 /*  AL4 */ psarsmxl;      /* -         RSM CROSS MEMORY LOCK            @G860PXH */
                  void * __ptr32 /*  AL4 */ psarsmal;      /* -         RSM ADDRESS SPACE LOCK           @G860PXH */
                  void * __ptr32 /*  VL4 */ psavpagl;      /* VSM PAGEABLE SUBPOOLS LOCK       @G860PXH */
                  void * __ptr32 /*  AL4 */ psarsmcl;      /* RSM COMMON LOCK                  @G860PXK */

                  #define psalks1 19                       /* COUNT OF LOCKS IN CLHT1          @G860PXK */

                  void * __ptr32 /*  AL4 */ psarvlk2;      /* RESERVED FOR LOCK EXPANSION      @G860PXH */
                };
              };
              union {
                char           /*  CL16 */ psaclht2[16]; /* SHARED EXCLUSIVE LOCKS TABLE     @G860PXH */
                struct {
                  void * __ptr32 /*  VL4 */ psarsml;       /* RSM GLOBAL FUNCTION/RECOVERY */

                  #define psarsmex 128                     /* -        BIT 0 OF PSARSML. IF ON, THE RSM */

                  void * __ptr32 /*  VL4 */ psatrcel;      /* TRACE BUFFER MANAGEMENT LOCK     @G860PXH */

                  #define psatrcex 128                     /* -        BIT 0 OF PSATRCEL. IF ON THE TRACE */

                  void * __ptr32 /*  VL4 */ psaiosl;       /* -   IOS LOCK                             @D3C */

                  #define psaiosex 128                     /* -        BIT 0 OF PSAIOSL. IF ON THE IOS      @D3A */
                  #define psalks2 3                        /* COUNT OF LOCKS IN CLHT2              @D3C */

                  void * __ptr32 /*  AL4 */ psarvlk4;      /* -         RESERVED FOR LOCK EXPANSION      @G50NP9A */
                };
              };
              union {
                char           /*  CL8 */ psaclht3[8];   /* SPECIAL LOCKS TABLE              @G860PXH */
                struct {
                  void * __ptr32 /*  AL4 */ psacpul;       /* CPU TABLE LOCKS                  @G860PXH */

                  #define psalks3 1                        /* COUNT OF LOCKS IN CLHT3          @G860PXH */

                  void * __ptr32 /*  AL4 */ psarvlk5;      /* -         RESERVED FOR LOCK EXPANSION      @G50NP9A */
                };
              };
              union {
                char           /*  CL12 */ psaclht4[12]; /* SUSPEND LOCKS TABLE              @G860PXH */
                struct {
                  void * __ptr32 /*  AL4 */ psacmsl;       /* -         CROSS MEMORY SERVICES LOCK */
                  void * __ptr32 /*  AL4 */ psalocal;      /* -         LOCAL LOCK */

                  #define psalks4 2                        /* COUNT OF LOCKS IN CLHT4          @G860PXH */

                  void * __ptr32 /*  AL4 */ psarvlk6;      /* -         RESERVED FOR LOCK EXPANSION      @G50NP9A */
                };
              };
            };
          };
          void * __ptr32 /*  AL4 */ psalcpua;      /* -         LOGICAL CPU ADDRESS FOR LOCK INSTRUCTION. */
          union {
            int            /*  FL4 */ psahlhi;       /* -           HIGHEST LOCK HELD INDICATOR.  THIS OFFSET */
            int            /*  FL4 */ psaclhs;       /* -           CPU LOCKS HELD STRING              MDC122 */
            struct {
              char           /*  XL1 */ psaclhs1;      /* -      FIRST BYTE OF PSACLHS. (MDC384)  @G860PXH */

              #define psacpuli 128                     /* -        CPU LOCK INDICATOR               @G860PXH */
              #define psasum 16                        /* -        SUMMARY BIT. IF ON, AT LEAST ONE     @LDA */
              #define psarsmli 8                       /* -        RSM LOCK INDICATOR               @G860PXH */
              #define psatrcei 4                       /* -        TRACE LOCK INDICATOR             @G860PXH */
              #define psaiosi 2                        /* -        IOS LOCK INDICATOR                   @D3A */

              char           /*  XL1 */ psaclhs2;      /* -      SECOND BYTE OF PSACLHS. (MDC385) @G860PXH */

              #define psarsmci 16                      /* -        RSM COMMON LOCK INDICATOR        @G860PXK */
              #define psarsmgi 8                       /* -        RSM GLOBAL LOCK INDICATOR        @G860PXH */
              #define psavfixi 4                       /* -        VSM FIX LOCK INDICATOR           @G860PXH */
              #define psaasmgi 2                       /* -        ASM GLOBAL LOCK INDICATOR        @G860PXH */
              #define psarsmsi 1                       /* -        RSM STEAL LOCK INDICATOR         @G860PXH */

              char           /*  XL1 */ psaclhs3;      /* -      THIRD BYTE OF PSACLHS  (MDC386)  @G50EP9A */

              #define psarsmxi 128                     /* -        RSM CROSS MEMORY LOCK INDICATOR  @G860PXH */
              #define psarsmai 64                      /* -        RSM ADDRESS SPACE LOCK INDICATOR @G860PXH */
              #define psavpagi 32                      /* -        VSM PAGE LOCK INDICATOR          @G860PXH */
              #define psadspli 16                      /* -        DISPATCHER LOCK INDICATOR */
              #define psaasmli 8                       /* -        ASM LOCK INDICATOR  (MDC388)     @G50EP9A */
              #define psasalli 4                       /* -        SPACE ALLOCATION LOCK INDICATOR */
              #define psaiosli 2                       /* -        IOS SYNCHRONIZATION LOCK INDICATOR */
              #define psarsmdi 1                       /* -        RSM DATA SPACE LOCK INDICATOR        @LBA */

              char           /*  XL1 */ psaclhs4;      /* -      FOURTH BYTE OF PSACLHS  (MDC392) @G50EP9A */

              #define psaiouli 128                     /* -        IOS UCB LOCK INDICATOR  (MDC393) @G50EP9A */
              #define psatpali 8                       /* -        TPACBDEB LOCK INDICATOR (MDC397) @G50EP9A */
              #define psasrmli 4                       /* -        SYSTEM RESOURCE MANAGER (SRM) LOCK */
              #define psacmsli 2                       /* -        CROSS MEMORY SERVICES LOCK INDICATOR */
              #define psalclli 1                       /* -        LOCAL LOCK INDICATOR  (MDC400)   @G50EP9A */

            };
          };
          void * __ptr32 /*  VL4 */ psalita;       /* -  ADDRESS OF LOCK INTERFACE TABLE. */
          char           /*  XL8 */ psastor8[8];   /* -      8-BYTE value for master's STO        @LSA */
          int            /*  FL4 */ psacr0;        /* -         SAVE AREA FOR CONTROL REGISTER 0 */
          char           /*  XL1 */ psamchfl;      /* -      MCH RECURSION FLAGS */
          char           /*  XL1 */ psasymsk;      /* -      THIS FIELD WILL BE USED IN CONJUNCTION */
          char           /*  XL1 */ psaactcd;      /* -      ACTION CODE SUPPLIED BY OPERATOR     @LHC */
          char           /*  XL1 */ psamchic;      /* -      MCH INITIALIZATION COMPLETE FLAGS  MDC098 */
          void * __ptr32 /*  AL4 */ psawkrap;      /* -         REAL ADDRESS OF VARY CPU PARAMETER LIST */
          void * __ptr32 /*  AL4 */ psawkvap;      /* -         VIRTUAL ADDRESS OF VARY CPU PARAMETER */
          short int      /*  HL2 */ psavstap;      /* -         WORK AREA FOR VARY CPU             MDC108 */
          short int      /*  HL2 */ psacpusa;      /* -         PHYSICAL CPU ADDRESS (STATIC)  (MDC131) */
          int            /*  FL4 */ psastor;       /* -         MASTER MEMORY'S SEGMENT TABLE ORIGIN */
          char           /*  XL90 */ psaidawk[90]; /* -     WORK SAVE AREA FOR private */
          short int      /*  HL2 */ psaret;        /* -            BSM 0,14 BRANCH RETURN TO CALLER     @P5A */
          short int      /*  HL2 */ psaretcd;      /* -            BSM 0,14 BRANCH RETURN TO CALLER     @P5A */
          char           /*  XL2 */ psarv37e[2];   /* -      RESERVED                             @PJC */
          union {
            char           /*  CL64 */ psarsvt[64];  /* -        RECOVERY STACK VECTOR TABLE        MDC064 */
            char           /*  CL64 */ psarsvte[64]; /* -        RECOVERY STACK VECTOR TABLE        MDC065 */
            struct {
              void * __ptr32 /*  AL4 */ psacstk;       /* -         ADDRESS OF CURRENTLY USED FUNCTIONAL */
              void * __ptr32 /*  AL4 */ psanstk;       /* -         ADDRESS OF NORMAL FRR STACK        MDC062 */
              void * __ptr32 /*  AL4 */ psasstk;       /* -         ADDRESS OF SVC-I/O-DISPATCHER FRR STACK */
              void * __ptr32 /*  AL4 */ psassav;       /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
              void * __ptr32 /*  AL4 */ psamstk;       /* -         ADDRESS OF MCH FRR STACK           MDC067 */
              void * __ptr32 /*  AL4 */ psamsav;       /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
              void * __ptr32 /*  AL4 */ psapstk;       /* -         ADDRESS OF PROGRAM CHECK FLIH FRR STACK */
              void * __ptr32 /*  AL4 */ psapsav;       /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
              void * __ptr32 /*  AL4 */ psaestk1;      /* -         ADDRESS OF EXTERNAL FLIH FRR STACK FOR */
              void * __ptr32 /*  AL4 */ psaesav1;      /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
              void * __ptr32 /*  AL4 */ psaestk2;      /* -         ADDRESS OF EXTERNAL FLIH FRR STACK FOR */
              void * __ptr32 /*  AL4 */ psaesav2;      /* -         ADDRESS OF INTERRUPTED STACK SAVE BY */
              void * __ptr32 /*  AL4 */ psaestk3;      /* -         ADDRESS OF EXTERNAL FLIH FRR STACK FOR */
              void * __ptr32 /*  AL4 */ psaesav3;      /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
              void * __ptr32 /*  AL4 */ psarstk;       /* -         ADDRESS OF RESTART FLIH FRR STACK  MDC077 */
              void * __ptr32 /*  AL4 */ psarsav;       /* -         ADDRESS OF INTERRUPTED STACK SAVED BY */
            };
          };
          char           /*  XL8 */ psalwpsw[8];   /* -      PSW OF WORK INTERRUPTED WHEN A       @LHC */
          double         /*  DL8 */ psarv3c8;      /* Reserved                             @M8C */
          void * __ptr32 /*  AL4 */ psatstk;       /* -         ADDRESS OF RTM RECOVERY STACK. */
          void * __ptr32 /*  AL4 */ psatsav;       /* -         ADDRESS OF ERROR STACK SAVED BY RTM  @L7A */
          void * __ptr32 /*  AL4 */ psaastk;       /* -         ADDRESS OF ACR FRR STACK.            @L7A */
          void * __ptr32 /*  AL4 */ psaasav;       /* -         ADDRESS OF INTERRUPT STACK SAVED BY  @L7A */
          char           /*  XL8 */ psartpsw[8];   /* -      RESUME PSW FOR RTM SETRP RETRY       @L7A */
          char           /*  XL8 */ psarv3e8[8];   /* -      RESERVED                             @L7C */
          char           /*  XL4 */ psasfacc[4];   /* - SETFRR ABEND COMPLETION CODE USED WHEN */
          int            /*  FL4 */ psalsfcc;      /* -            L  1,PSASFACC INSTRUCTION TO LOAD    @P5A */
          short int      /*  HL2 */ psasvc13;      /* -            AN SVC 13 INSTRUCTION                @P5A */
          char           /*  XL1 */ psafpfl;       /* -      See LCCAFPFL                         @MEC */
          char           /*  XL1 */ psainte;       /* -      FLAGS FOR CPU TIMER  (MDC466)    @ZM48078 */

          #define psanuin 128                      /* -        CPU TIMER CANNOT BE USED */

          int            /*  FL4 */ psartm1r;      /* -            STOSM PSARTM1S,X'00' INSTRUCTION     @P5A */

          #define psartm1m 1021                    /* - LABEL FOR SYSTEM MASK USED IN */

          char           /*  XL8 */ psarv400[8];   /* -      Reserved                             @LVC */
          void * __ptr32 /*  AL4 */ psaatcvt;      /* -         ADDRESS OF VTAM ATCVT.  INITIALIZED BY */
          void * __ptr32 /*  AL4 */ psawtcod;      /* -         WAIT STATE CODE LOADED               @LHC */
          void * __ptr32 /*  AL4 */ psascwa;       /* -         ADDRESS OF SUPERVISOR CONTROL CPU */
          void * __ptr32 /*  AL4 */ psarsmsa;      /* -         ADDRESS OF RSM CPU RELATED WORK */
          char           /*  XL4 */ psascpsw[4];   /* - MODEL PSW */
          char           __filler21[4];
          char           /*  XL4 */ psasmpsw[4];   /* - SRB DISPATCH PSW  (MDC326)      @G50DP9A */
          char           __filler22[4];
          char           /*  XL16 */ psapcpsw[16]; /* =     TEMPORARY OLD PSW STORAGE FOR PROGRAM */
          char           /*  XL8 */ psarv438[8];   /* =     Reserved                             @M8C */
          char           /*  XL16 */ psamcx16[16]; /* -     MCH exit PSW16                       @M8A */
          char           /*  XL16 */ psarsp16[16]; /* -     Resume PSW field for restart interrupt */
          char           /*  XL16 */ psapswsv16[16]; /* -     PSW SAVE AREA FOR DISPATCHER AND ACR @M8A */
        };
      };
    };
    struct {
      char           __filler23[1128];
      struct {
        struct {
          char           /*  XL8 */ psapswsv[8];   /* -      PSW SAVE AREA FOR DISPATCHER AND ACR */
          char           /*  XL8 */ psacput[8];    /* -      SUPERVISOR CPU TIMER SAVE AREA */
          union {
            int            /*  FL4 */ psapcfun;      /* -           PROGRAM FLIH RECURSION           @G383P9A */
            struct {
              char           /*  XL1 */ psapcfb1;      /* -      FUNCTION VALUE  (MDC484)         @G383P9A */

              #define psapcmc 1                        /* -        MC INTERRUPT      (MDC605)       @G383P9A */
              #define psapcpf 2                        /* -        PAGE FAULT                       @G383P9A */
              #define psapcps 3                        /* -        PER/SPACE SWITCH INTERRUPT       @G383PXU */
              #define psapcad 4                        /* -        ADDRESSING EXCEPTION  (MDC488)   @G383P9A */
              #define psapctr 5                        /* -        TRANSLATION EXCEPTION  (MDC489)  @G383P9A */
              #define psapcpc 6                        /* -        PROGRAM CHECK  (MDC490)          @G383P9A */
              #define psapctrc 7                       /* -        TRACE INTERRUPT                  @G860PXK */
              #define psapcaf 8                        /* -        NEW VALUE FOR PROGRAM INTERRUPT      @03A */
              #define psapcls 9                        /* -        LINKAGE STACK INTERRUPT FUNCTION     @L8A */
              #define psapcart 10                      /* -        ACCESS REGISTER TRANSLATION          @L8A */
              #define psapcdpf 11                      /* -        DISABLED PAGE/SEGMENT FAULT          @LCA */
              #define psapcdar 12                      /* -        DISABLED ART PIC X'2B' FUNCTION      @LCA */
              #define psapcprt 13                      /* -        Protection exception function value  @LQA */
              #define psapcmax 13                      /* -        MAXIMUM VALID FUNCTION VALUE         @LQC */

              char           /*  XL1 */ psapcfb2;      /* -      FUNCTION FLAGS  (MDC491)         @G383P9A */

              #define psapctrr 128                     /* -        TRACE INTERRUPT RECURSION        @YA01102 */
              #define psapcmt 64                       /* -        TRACE RECURSION FLAG  (MDC493)   @G383P9A */

              char           /*  XL1 */ psapcfb3;      /* -      RECURSION FLAGS  (MDC494)        @G383P9A */

              #define psapcp1 128                      /* -        FIRST LEVEL PROGRAM CHECK        @G383P9A */
              #define psapcp2 64                       /* -        SECOND LEVEL PROGRAM CHECK       @G383P9A */
              #define psapcde 32                       /* -        DAT ERROR CONDITION  (MDC497)    @G383P9A */
              #define psapclv 16                       /* -        0=REGISTERS IN LCCA, 1=REGISTERS @G383P9A */
              #define psapcp3 8                        /* -        THIRD LEVEL PROGRAM CHECK        @G383P9A */
              #define psapcp4 4                        /* -        FOURTH LEVEL PROGRAM CHECK       @G383P9A */
              #define psapcpfr 2                       /* -        RECURSIVE PAGE FAULT INDICATOR       @LAA */
              #define psapcavr 1                       /* -        RECURSIVE ASTE VALIDITY INDICATOR    @LCA */

              char           /*  XL1 */ psapcfb4;      /* -      RECURSION FLAGS */

              #define psapcdnv 128                     /* -        DUCT validity indicator              @PBA */
              #define psapclsr 64                      /* -        IEAVLSIH has invoked IARPTEPR and    @PEA */

            };
          };
          short int      /*  HL2 */ psapcps2;      /* -         PASID AT TIME OF SECOND LEVEL    @G383P9A */
          char           /*  XL2 */ psarv47e[2];   /* -      RESERVED                         @G860PXK */
          char           /*  XL24 */ psapcwka[24]; /* -     Work area for PC FLIH. Must be */
          short int      /*  HL2 */ psapcps3;      /* -         PASID AT TIME OF THIRD LEVEL     @G383P9A */
          short int      /*  HL2 */ psapcps4;      /* -         PASID AT TIME OF FOURTH LEVEL */
          union {
            int            /*  FL4 */ psamodew;      /* -           Word label to address PSAMODE.       @LPC */
            struct {
              char           __filler24[1];
              struct {
                char           /*  XL1 */ psamflgs;      /* -      SECOND BYTE OF PSAMODEW (MDC604) @G383P9A */

                #define psanss 128                       /* -        ENABLED UNLOCKED TASK WITH FRR   @G383P9A */
                #define psaprsrb 64                      /* -        Preemptable-class SRB                @LPA */

                char           /*  XL1 */ psamodeh;      /* -      SECOND HALFWORD OF PSAMODEW.     @G383P9A */
                char           /*  XL1 */ psamode;       /* -      SYSTEM MODE INDICATOR AND DISPLACEMENT */

                #define psataskm 0                       /* -        TASK MODE VALUE  (MDC338)        @G50DP9A */
                #define psasrbm 4                        /* -        SRB MODE VALUE  (MDC339)         @G50DP9A */
                #define psawaitm 8                       /* -        WAIT MODE VALUE  (MDC340)        @G50DP9A */
                #define psadispm 16                      /* -        DISPATCHER MODE VALUE  (MDC342)  @G50DP9A */
                #define psapsrbm 32                      /* -        PSEUDO SRB MODE FLAG BIT.  THIS BIT MAY */

              };
            };
          };
          char           __filler25[3];
          char           /*  XL1 */ psastnsm;      /* -      STNSM TARGET USED BY EXIT PROLOGUE */
          int            /*  FL4 */ psalkjw;       /* -         LOCAL LOCK RELEASE SRB JOURNAL   @G383P9A */
          union {
            double         /*  DL8 */ psadzero;      /* -           DOUBLEWORD OF ZERO   (MDC612)    @G383P9A */
            int            /*  FL4 */ psafzero;      /* -         FULLWORD OF ZERO     (MDC612)    @G383P9A */
          };
          int            /*  FL4 */ psalkjw2;      /* -         CMS LOCK RELEASE JOURNAL WORD.   @G383P9A */
          void * __ptr32 /*  VL4 */ psalkpt;       /* -   SETLOCK TEST,TYPE=HIER */
          void * __ptr32 /*  AL4 */ psalaa;        /* -      LE Anchor Area. Owner: LE            @LVA */
          void * __ptr32 /*  VL4 */ psalit2;       /* -  POINTER TO THE EXTENDED LOCK         @LDA */
          void * __ptr32 /*  VL4 */ psaecltp;      /* -   POINTER TO THE EXTENDED CURRENT      @LDA */
          union {
            int            /*  FL4 */ psaclhse;      /* -           CURRENT LOCKS HELD STRING EXTENSION  @LDA */
            struct {
              char           /*  XL1 */ psalheb0;      /* -      BYTE 0 OF THE CURRENT LOCK HELD      @LDA */

              #define psablsdi 128                     /* -        BMFLSD LOCK INDICATOR.               @LGA */
              #define psaxdsi 64                       /* -        XCFDS LOCK INDICATOR.                @LEA */
              #define psaxresi 32                      /* -        XCFRES LOCK INDICATOR.               @LEA */
              #define psaxqi 16                        /* -        XCFQ LOCK INDICATOR.                 @LEA */
              #define psaeseti 8                       /* -        ETRSET LOCK INDICATOR.               @LFA */
              #define psaixsci 4                       /* -        IXLSCH LOCK INDICATOR.               @LMC */
              #define psaixshi 2                       /* -        IXLSHR LOCK INDICATOR.               @LMC */
              #define psaixdsi 1                       /* -        IXLDS LOCK INDICATOR.                @LLA */

              char           /*  XL1 */ psalheb1;      /* -      BYTE 1 OF THE CURRENT LOCK HELD      @LDA */

              #define psaixlli 128                     /* -        IXLSHELL LOCK INDICATOR.             @LMC */
              #define psauluti 64                      /* -        IOSULUT LOCK INDICATOR.              @LJA */
              #define psaixlri 32                      /* -        IXLREQST LOCK INDICATOR.             @05A */
              #define psawlmri 16                      /* -        WLMRES LOCK INDICATOR                @LRA */
              #define psawlmqi 8                       /* -        WLMQ LOCK INDICATOR.                 @LRA */
              #define psacntxi 4                       /* -        CONTEXT LOCK INDICATOR               @LRA */
              #define psaregsi 2                       /* -        REGSRV LOCK INDICATOR.               @LRA */
              #define psassdli 1                       /* -        SSD LOCK INDICATOR.                  @LTA */

              char           /*  XL1 */ psalheb2;      /* -      BYTE 2 OF THE CURRENT LOCK HELD      @LDA */

              #define psagrsli 128                     /* -        GRSINT lock indicator                @M1A */
              #define psapslk1 64                      /* -        HCWPSLK1 lock indicator              @M6A */
              #define psapnlk1 32                      /* -        HCWPNLK1 lock indicator              @M6A */
              #define psaiolk1 16                      /* -        HCWIOLK1 lock indicator              @M6A */
              #define psapxlk1 8                       /* -        HCWPXLK1 lock indicator              @M6A */
              #define psadrlk3 4                       /* -        HCWDRLK3 lock indicator              @M6A */
              #define psadrlk2 2                       /* -        HCWDRLK2 lock indicator              @M6A */
              #define psadrlk1 1                       /* -        HCWDRLK1 lock indicator              @M6A */

              char           /*  XL1 */ psalheb3;      /* -      BYTE 3 OF THE CURRENT LOCK HELD      @LDA */

              #define psasrmei 128                     /* -        SRMENQ lock indicator                @M9A */

            };
          };
          char           /*  XL8 */ psarv4c8[8];   /* -    RESERVED FOR FUTURE LOCK EXPANSION.  @LDA */
          char           /*  XL184 */ psarv4d0[184]; /* -    RESERVED.                            @LDA */
          char           /*  XL1 */ psahwfb;       /* -      HARDWARE FLAG BYTE.                  @L3A */
          char           /*  XL1 */ psacr0cb;      /* -      CR0 CONTROL BYTE USED BY PROTPSA MACRO */

          #define psaenabl 16                      /* -        TO ENABLE PSA PROTECTION */
          #define psadsabl 0                       /* -        TO DISABLE PSA PROTECTION */

          char           /*  XL2 */ psarv58a[2];   /* -      RESERVED                             @PJC */
          int            /*  FL4 */ psacr0sv;      /* -         CR0 SAVE AREA USED BY PROTPSA MACRO */

          #define psacr0en 16                      /* -        IF 0, PSA PROTECT DISABLED.  IF 1, PSA */
          #define psacr0ed 128                     /* DAT features. Bit is in PSACR0SV+1   @0BA */
          #define psacr0al 8                       /* -        IF 1, ASN & LX Reuse facility is */
          #define psacr0fp 4                       /* -        IF 1, extended floating point is */
          #define psacr0vi 2                       /* -        IF 1, vector instructions are */

          int            /*  FL4 */ psapccr0;      /* -         PROGRAM CHECK FLIH CR0 SAVE AREA */
          int            /*  FL4 */ psarcr0;       /* -         RESTART FLIH CR0 SAVE AREA */

          #define psarpen 16                       /* -        IF 0, PSA PROTECT DISABLED.  IF 1, PSA */

          union {
            double         /*  DL8 */ psastke;       /* -           CURRENT STACK CONTROL WORD FOR SRB'S AND */
            struct {
              short int      /*  HL2 */ psatkn;        /* -         CURRENT STACK TOKEN     (MDC610) @G383P9A */
              short int      /*  HL2 */ psaasd;        /* -         CURRENT STACK ADDRESS SPACE */
              int            /*  FL4 */ psasel;        /* -         CURRENT STACK ELEMENTS ADDRESS */
            };
          };
          char           /*  XL4 */ psaskpsw[4];   /* PCLINK STACK/UNSTACK MODEL PSW */
          void * __ptr32 /*  AL4 */ psaskps2;      /* -         PCLINK PSW ADDRESS      (MDC604) @G383P9A */
          void * __ptr32 /*  AL4 */ psacpcls;      /* -      PCLINK WORKAREA - CURRENT STACK      @L9C */
          char           /*  XL4 */ psarv5ac[4];   /* -      RESERVED.                            @L9A */
          void * __ptr32 /*  AL4 */ psascfs;       /* -      ADDRESS OF THE SUPERVISOR CONTROL    @L8C */
          void * __ptr32 /*  AL4 */ psapawa;       /* -      ADDRESS OF PC/AUTH WORK AREA.        @L8A */
          char           /*  XL1 */ psascfb;       /* -      SUPERVISOR CONTROL FLAG BYTE.        @L1A */

          #define psaiopr 128                      /* -        INDICATES IF INTERRUPTED TASK SHOULD @L1A */
          #define psaiorty 64                      /* -        I/O FLIH RECOVERY FLAG. IF 1,        @L5A */

          char           /*  XL3 */ psarv5b9[3];   /* -      RESERVED                             @PJC */
          char           /*  XL4 */ psacr0m1[4];   /* MASK OF CR0 WITH EXTERNAL MASK BITS  @0HC */
          char           /*  XL4 */ psacr0m2[4];   /* MASK OF CR0 WITH ONLY EXTERNAL MASK  @0HC */
          char           /*  XL4 */ psarv5c4[4];   /* -      RESERVED                             @MAA */
          char           /*  XL8 */ psa_cr0emaskoffextint[8]; /* Mask of bits to turn */
          char           /*  XL8 */ psa_cr0emaskonextint[8]; /* Mask of bits to turn */
          union {
            char           /*  XL8 */ psa_cr0esavearea[8]; /* Save area for grande CR0         @MAA */
            struct {
              char           /*  XL4 */ psa_cr0esavearea_hw[4]; /* High word save area for high word of */
              char           /*  XL4 */ psa_cr0esavearea_lw[4]; /* Low word save area for low word of */
            };
          };
          union {
            char           /*  XL16 */ psa_windowworkarea[16]; /* WorkArea for IEAMWIN                @0IA */
            char           /*  XL8 */ psa_windowtoddelta[8]; /* Difference in TOD values - used in */
            struct {
              char           /*  XL4 */ psa_windowtoddelta_hw[4]; /* High word area for difference in TOD */
              char           /*  XL4 */ psa_windowtoddelta_lw[4]; /* Low word area for difference in TOD */
            };
          };
        };
        struct {
          char           /*  XL8 */ psa_windowlastopentod[8]; /* TOD when IEAMWIN last opened a window */
          char           /*  XL8 */ psa_windowcurrenttod[8]; /* TOD when IEAMWIN last checked to open */
          char           /*  XL80 */ psarv600[80]; /* -     RESERVED                             @0IC */
          double         /*  DL8 */ psa_time_on_cp; /* -      Current SRB's accumulated CPU time   @0CA */
          double         /*  DL8 */ psatime;       /* -         CURRENT SRB'S ACCUMULATED CPU TIME   @01C */
          int            /*  FL4 */ psasrsav;      /* -        ADDRESS OF CURRENT FRR STACK     @G383P9A */
          char           /*  XL12 */ psaesc8[12];  /* -     Save area for IEAVESC8               @LPA */
          char           /*  XL8 */ psadexmw[8];   /* -         Work area for dispatcher CR3/4       @LVC */
          char           /*  XL64 */ psadsars[64]; /* -     DISPATCHER ACCESS REGISTER SAVE AREA @L9C */
          double         /*  DL8 */ psa_pcflih_trace_interrupt_cput; /* - Trace interrupt CPU timer saved */
          union {
            double         /*  DL8 */ psadtsav;      /* -            CPU TIMER VALUE AT LAST DISPATCH,    @01C */
            struct {
              char           /* 8XL1 */ psaff6c0[8];   /* INITIALIZE FIELD PSADTSAV     @ZMC3284 */
              union {
                char           /*  XL16 */ psadexms[16]; /* -         DISPATCHER CONTROL REGISTER 3 AND 4 */
                struct {
                  union {
                    double         /*  DL8 */ psadcr3;       /* -           DISPATCHER CONTROL REGISTER 3 SAVE AREA */
                    struct {
                      int            /*  FL4 */ psadsins;      /* -        DISPATCHER Secondary ASTE Inst# S/A  @LVA */
                      union {
                        int            /*  FL4 */ psadpksa;      /* -        PKM and SASID                        @LVA */
                        struct {
                          short int      /*  HL2 */ psadpkm;       /* -        DISPATCHER PROGRAM KEY MASK SAVE AREA */
                          short int      /*  HL2 */ psadsas;       /* -        DISPATCHER SECONDARY ASID SAVE AREA */
                        };
                      };
                    };
                  };
                  union {
                    double         /*  DL8 */ psadcr4;       /* -           DISPATCHER CONTROL REGISTER 4 SAVE AREA */
                    struct {
                      int            /*  FL4 */ psadpins;      /* -        DISPATCHER Primary ASTE Inst# S/A    @LVA */
                      int            /*  FL4 */ psadaxpa;      /* -        AX and PASID                         @LVA */
                    };
                    struct {
                      char           __filler26[4];
                      struct {
                        short int      /*  HL2 */ psadax;        /* -        DISPATCHER  AUTHORIZATION        @G383P9A */
                        short int      /*  HL2 */ psadpas;       /* -        DISPATCHER PRIMARY ASID SAVE     @G383P9A */
                      };
                    };
                  };
                };
              };
              union {
                double         /*  DL8 */ psausend;      /* END FIRST SET OF ASSIGNED FIELDS @G383PXU */
                char           /*  XL200 */ psarv6d8[200]; /* -    RESERVED                             @M3C */
              };
              double         /*  DL8 */ psaecvt;       /* Address of ECVT                      @M3A */
              double         /*  DL8 */ psaxcvt;       /* Address of XCVT                      @M3A */
              char           /* 48XL1 */ psadatlk[48]; /* -    AREA FOR DAT-OFF ASSIST LINKAGE CODE */
              void * __ptr32 /*  AL4 */ psadatof;      /* -         REAL STORAGE ADDRESS OF THE DAT-OFF */
              int            /*  FL4 */ psadatln;      /* -        LENGTH OF THE DAT-OFF INDEX TABLE */
              char           /*  XL4 */ psarv7e8[4];   /* -          RESERVED FOR SYSTEM TRACE.       @G860PXK */
            };
          };
        };
      };
    };
    struct {
      char           __filler27[2024];
      char           /* 4XL1 */ psaff7e8[4];   /* INITIALIZE FIELD PSARV7E8     @ZMC3284 */
      char           /*  XL1 */ psatrace;      /* -      SYSTEM TRACE FLAGS.              @G860PXK */

      #define psatroff 128                     /* -        IF ON, SYSTEM TRACE SUSPENDED ON THIS */

      char           /*  XL3 */ psarv7ed[3];   /* -      RESERVED FOR SYSTEM TRACE.           @PJC */
      void * __ptr32 /*  AL4 */ psatbvtr;      /* -         REAL ADDRESS OF SYSTEM TRACE BUFFER */
      void * __ptr32 /*  AL4 */ psatbvtv;      /* -         VIRTUAL ADDRESS CORRESPONDING TO */
      void * __ptr32 /*  VL4 */ psatrvt;       /* -  ADDRESS OF SYSTEM TRACE VECTOR */
      void * __ptr32 /*  VL4 */ psatot;        /* -  ADDRESS OF SYSTEM TRACE OPERAND */
      union {
        struct {
          double         /*  DL8 */ psaus2st;      /* START SECOND SET OF ASSIGNED     @G383PXU */
          int            /*  FL4 */ psacdsa0;      /* CALLDISP REGISTER 0  SAVE AREA   @G383PXU */
          int            /*  FL4 */ psacdsa1;      /* CALLDISP REGISTER 1  SAVE AREA   @G383PXU */
        };
        char           /*  XL16 */ psacdsav[16]; /* CALLDISP REGISTER SAVE AREA FOR  @G860PXK */
        struct {
          int            /*  FL4 */ psacdsae;      /* CALLDISP REGISTER 14 SAVE AREA   @G383PXU */
          int            /*  FL4 */ psacdsaf;      /* CALLDISP REGISTER 15 SAVE AREA   @G383PXU */
        };
      };
      int            /*  FL4 */ psagspsw;      /* GLOBAL SCHEDULE SYSTEM MASK SAVE @ZA63674 */
      int            /*  FL4 */ psagsrgs;      /* GLOBAL SCHEDULE REGISTER SAVE    @ZA63674 */
      char           /*  XL4 */ psarv818[4];   /* -     RESERVED                             @PAC */
      int            /*  FL4 */ psasv01r;      /* IEAVTRG1 register 1 save area.       @PAA */
      int            /*  FL4 */ psasv14r;      /* IEAVTRG1 register 14 save area.      @PAA */
      int            /*  FL4 */ psaems2r;      /* -        REGISTER SAVE AREA */
      union {
        char           /*  XL64 */ psatrsav[64]; /* -       TRACE REGISTER SAVE AREA.        @G860PXH */
        struct {
          int            /*  FL4 */ psatrgr0;      /* -        TRACE REGISTER 0 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr1;      /* -        TRACE REGISTER 1 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr2;      /* -        TRACE REGISTER 2 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr3;      /* -        TRACE REGISTER 3 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr4;      /* -        TRACE REGISTER 4 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr5;      /* -        TRACE REGISTER 5 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr6;      /* -        TRACE REGISTER 6 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr7;      /* -        TRACE REGISTER 7 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr8;      /* -        TRACE REGISTER 8 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgr9;      /* -        TRACE REGISTER 9 SAVE AREA.      @G860PXH */
          int            /*  FL4 */ psatrgra;      /* -        TRACE REGISTER 10 SAVE AREA.     @G860PXH */
          int            /*  FL4 */ psatrgrb;      /* -        TRACE REGISTER 11 SAVE AREA.     @G860PXH */
          int            /*  FL4 */ psatrgrc;      /* -        TRACE REGISTER 12 SAVE AREA.     @G860PXH */
          int            /*  FL4 */ psatrgrd;      /* -        TRACE REGISTER 13 SAVE AREA.     @G860PXH */
          int            /*  FL4 */ psatrgre;      /* -        TRACE REGISTER 14 SAVE AREA.     @G860PXH */
          int            /*  FL4 */ psatrgrf;      /* -        TRACE REGISTER 15 SAVE AREA.     @G860PXH */
        };
      };
      char           /*  XL4 */ psatrsv1[4];   /* -     Trace Save 1                         @M8A */
      char           /*  XL4 */ psatrsvs[4];   /* -     Trace Save for SLIP/PER              @M8A */
      char           /*  XL8 */ psatrsv2[8];   /* -     Trace Save 2                         @M8A */
      char           /*  XL40 */ psarv878[40]; /* -     RESERVED.                            @M8A */
      char           /*  XL8 */ psagsavh[8];   /* -     Register save area used by           @09C */
      union {
        char           /*  XL64 */ psagsav[64];  /* -         REGISTER SAVE AREA USED BY */
        struct {
          char           /* 64XL1 */ psaff8a8[64]; /* INITIALIZE FIELD PSAGSAV      @ZMC3284 */
          int            /*  FL4 */ psascrg1;      /* -        GLOBAL SCHEDULE REGISTER SAVE AREA */
          int            /*  FL4 */ psascrg2;      /* -        GLOBAL SCHEDULE REGISTER SAVE AREA */
          int            /* 3FL4 */ psagpreg[3];   /* -       REGISTER SAVE AREA FOR SVC FLIH */
          int            /*  FL4 */ psarsreg;      /* -        RESTART FLIH REGISTER SAVE       @G860PXK */
          int            /*  FL4 */ psapcgr8;      /* -        PROGRAM FLIH REGISTER 8 SAVE AREA */
          int            /*  FL4 */ psapcgr9;      /* -        PROGRAM FLIH REGISTER 9 SAVE AREA */
          union {
            double         /*  DL8 */ psapcgab;      /* PROGRAM FLIH REG 10-11 SAVE AREA     @LSA */
            struct {
              int            /*  FL4 */ psapcgra;      /* -        PROGRAM FLIH REGISTER 10 SAVE AREA */
              int            /*  FL4 */ psapcgrb;      /* -        PROGRAM FLIH REGISTER 11 SAVE AREA */
            };
          };
          union {
            char           /*  XL64 */ psalksa[64];  /* -        IEAVELK REGISTER SAVE AREA       @G860PXK */
            struct {
              int            /*  FL4 */ psalkr0;       /* -        IEAVELK REGISTER 0 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr1;       /* -        IEAVELK REGISTER 1 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr2;       /* -        IEAVELK REGISTER 2 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr3;       /* -        IEAVELK REGISTER 3 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr4;       /* -        IEAVELK REGISTER 4 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr5;       /* -        IEAVELK REGISTER 5 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr6;       /* -        IEAVELK REGISTER 6 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr7;       /* -        IEAVELK REGISTER 7 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr8;       /* -        IEAVELK REGISTER 8 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr9;       /* -        IEAVELK REGISTER 9 SAVE AREA     @G860PXK */
              int            /*  FL4 */ psalkr10;      /* -        IEAVELK REGISTER 10 SAVE AREA    @G860PXK */
              int            /*  FL4 */ psalkr11;      /* -        IEAVELK REGISTER 11 SAVE AREA    @G860PXK */
              int            /*  FL4 */ psalkr12;      /* -        IEAVELK REGISTER 12 SAVE AREA    @G860PXK */
              int            /*  FL4 */ psalkr13;      /* -        IEAVELK REGISTER 13 SAVE AREA    @G860PXK */
              int            /*  FL4 */ psalkr14;      /* -        IEAVELK REGISTER 14 SAVE AREA    @G860PXK */
              int            /*  FL4 */ psalkr15;      /* -        IEAVELK REGISTER 15 SAVE AREA    @G860PXK */
            };
          };
          char           /*  XL72 */ psaslsa[72];  /* -         SINGLE LEVEL SAVE AREA USED BY DISABLED */
        };
      };
    };
    struct {
      char           __filler28[2384];
      char           /* 72XL1 */ psaff950[72]; /* INITIALIZE FIELD PSASLSA       @ZMC3284 */
      union {
        char           /*  XL64 */ psajstsa[64]; /* -     SAVE AREA FOR JOB STEP TIMING        @H1A */
        struct {
          char           /* 64XL1 */ psaff998[64]; /* INITIALIZE FIELD PSAJSTSA         @H1A */
          union {
            struct {
              double         /*  DL8 */ psaus2nd;      /* END SECOND SET OF ASSIGNED           @H1M */
              int            /*  FL4 */ psaslkr2;      /* -        IEAVESLK REGISTER 2 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr3;      /* -        IEAVESLK REGISTER 3 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr4;      /* -        IEAVESLK REGISTER 4 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr5;      /* -        IEAVESLK REGISTER 5 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr6;      /* -        IEAVESLK REGISTER 6 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr7;      /* -        IEAVESLK REGISTER 7 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr8;      /* -        IEAVESLK REGISTER 8 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr9;      /* -        IEAVESLK REGISTER 9 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkra;      /* -        IEAVESLK REGISTER 10 SAVE AREA       @P4A */
              int            /*  FL4 */ psaslkrb;      /* -        IEAVESLK REGISTER 11 SAVE AREA       @P4A */
              int            /*  FL4 */ psaslkrc;      /* -        IEAVESLK REGISTER 12 SAVE AREA       @P4A */
              int            /*  FL4 */ psaslkrd;      /* -        IEAVESLK REGISTER 13 SAVE AREA       @P4A */
              int            /*  FL4 */ psaslkre;      /* -        IEAVESLK REGISTER 14 SAVE AREA       @P4A */
              int            /*  FL4 */ psaslkrf;      /* -        IEAVESLK REGISTER 15 SAVE AREA       @P4A */
            };
            char           /*  XL64 */ psaslksa[64]; /* -        IEAVESLK REGISTER SAVE AREA          @P4A */
            struct {
              int            /*  FL4 */ psaslkr0;      /* -        IEAVESLK REGISTER 0 SAVE AREA        @P4A */
              int            /*  FL4 */ psaslkr1;      /* -        IEAVESLK REGISTER 1 SAVE AREA        @P4A */
            };
          };
          char           /*  XL36 */ psarva18[36]; /* -    RESERVED                             @P4A */
          char           /*  XL64 */ psascsav[64]; /* IEAVESC0 save area                   @P7A */
          char           /*  XL1 */ psasflgs;      /* Schedule flags                       @P8A */

          #define psaschda 128                     /* Schedule is active                   @LPA */
          #define psamcha 64                       /* Machine Check is active              @06A */
          #define psarsta 32                       /* Restart is active                    @06A */
          #define psaegra 16                       /* Global Recovery is active            @06A */
          #define psartma 8                        /* Selected RTM functions are active    @06A */
          #define psadontgetweb 4                  /* A WEB or WEBQLOCK is held. IEAVESC0 */

          char           /*  XL1 */ psamiscf;      /* Miscellaneous flags                  @LVA */

          #define psaalr 128                       /* Equivalent to CVTALR                 @LVA */

          char           /*  XL2 */ psarva7e[2];   /* Reserved for future use - SC1C5      @LVC */
          char           /*  XL188 */ psarva80[188]; /* -    RESERVED                             @P8C */
          void * __ptr32 /*  VL4 */ psagsch7;      /* -  ENABLED GLOBAL SCHEDULE ENTRY */
          void * __ptr32 /*  VL4 */ psagsch8;      /* -  DISABLED GLOBAL SCHEDULE ENTRY */
          void * __ptr32 /*  VL4 */ psalsch1;      /* -  ENABLED SCHEDULE ENTRY POINT */
          void * __ptr32 /*  VL4 */ psalsch2;      /* -  DISABLED SCHEDULE ENTRY POINT */
          void * __ptr32 /*  VL4 */ psasvt;        /* -  ADDRESS OF SUPERVISOR VECTOR TABLE */
          void * __ptr32 /*  VL4 */ psasvtx;       /* Address of Supervisor Vector Table   @LNC */
          union {
            char           /*  XL8 */ psafafrr[8];   /* Fast FRR fields.  These fields are for */
            struct {
              void * __ptr32 /*  AL4 */ psaffrr;       /* Fast FRR address.  This field is     @PSC */
              void * __ptr32 /*  AL4 */ psaffrrs;      /* Fast FRR stack.  This field is       @PSA */
            };
          };
          char           /*  XL36 */ psarvb5c[36]; /* -     Reserved                             @PSC */
          char           /* 1112XL1 */ psastak[1112]; /* -   NORMAL FRR STACK                     @LVC */
          char           /* 40XL1 */ psarvfd8[40]; /* -    RESERVED FOR EXPANSION OF            @PJC */
          double         /*  DL8 */ psaend;        /* -           END OF PSA            (MDC612)   @G383P9A */
        };
      };
    };
    struct {
      struct {
        union {
          double         /*  DL8 */ flcesame;      /* FLCE 0x: defined by architecture */
          char           /*  CL8 */ flceippsw[8];  /* FLCE 0x: IPL PSW */
        };
        char           /*  CL8 */ flceiccw1[8];  /* FLCE 8x: IPL CCW1 */
        char           /*  CL8 */ flceiccw2[8];  /* FLCE 10x: IPL CCW1 */
        char           /*  CL104 */ flcer018[104]; /* FLCE 18x: reserved */
        char           /*  CL4 */ flceeparm[4];  /* FLCE 80x: External interruption parameter */
        char           /*  CL2 */ flcecpuad[2];  /* FLCE 84x: CPU address */
        char           /*  CL2 */ flceeicode[2]; /* FLCE 86x: External interruption code */
        union {
          char           /*  CL4 */ flcesdata[4];  /* FLCE 88x: Additional SVC interruption data */
          char           /*  CL2 */ flcesdatabyte0[2]; /* FLCE 88x: */
          struct {
            char           __filler29[1];
            struct {
              char           /*  XL1 */ flcesilc;      /* FLCE 89x: SVC interruption length code */

              #define flcesilcb 7                      /* FLCE 89x: Significant bits in ILC. Last bit */

              struct {
                char           /*  CL2 */ flcesicode[2]; /* FLCE 8Ax: SVC interruption code */
                union {
                  char           /*  CL4 */ flcepdata[4];  /* FLCE 8Cx: Additional Program interruption */
                  char           /*  CL2 */ flcepdatabyte0[2]; /* FLCE 8Cx: */
                  struct {
                    char           __filler30[1];
                    struct {
                      char           /*  XL1 */ flcepilc;      /* FLCE 8Dx: Program interruption length code */

                      #define flcepilcb 7                      /* FLCE 8Dx: Significant bits in ILC. Last bit */

                      char           /*  CL2 */ flcepicode[2]; /* FLCE 8Ex: Program interruption code */
                    };
                  };
                  struct {
                    char           __filler31[2];
                    char           /*  XL1 */ flcepicode0;   /* FLCE 8Ex: Exception extension code */
                    char           /*  XL1 */ flcepicode1;   /* FLCE 8Fx: 8-bit interruption code */

                    #define flcepiper 128                    /* FLCE 8Fx: PER interruption code */
                    #define flcepimc 64                      /* FLCE 8Fx: Monitor Call interruption code */
                    #define flcepipc 63                      /* FLCE 8Fx: An unsolicited program interruption */

                  };
                };
              };
            };
          };
        };
      };
      char           /*  CL4 */ flcepiinformation[4]; /* FLCE 90x: */
    };
    struct {
      char           __filler32[147];
      struct {
        struct {
          char           /*  XL1 */ flcedxc;       /* FLCE 93x: Data exception code for PI 7 */
          char           /*  CL2 */ flcemcnum[2];  /* FLCE 94x: Monitor class number */
          union {
            char           /*  CL2 */ flcepercode[2]; /* FLCE 96x: PER code */
            struct {
              unsigned char  /*  BL1 */ flcepercode0;  /* FLCE 96x: Byte 0 */

              #define flcepersb 128                    /* FLCE 96x: PER successful branch event */
              #define flceperif 64                     /* FLCE 96x: PER instruction fetch event */
              #define flcepersa 32                     /* FLCE 96x: PER storage alteration event */
              #define flcepersar 8                     /* FLCE 96x: PER storage alteration using real */
              #define flceperzad 4                     /* FLCE 96x: PER zero address detection */
              #define flcepertransactionend 2

              unsigned char  /*  BL1 */ flceperatmid;  /* FLCE 97x: PER addressing and translation mode */

              #define flceperpsw4 128                  /* FLCE 97x: PER PSW bit 4 */
              #define flceperatmidvalid 64             /* FLCE 97x: When 1, the ATMID bits are valid */
              #define flceperpsw32 32                  /* FLCE 97x: PER PSW bit 32 */
              #define flceperpsw5 16                   /* FLCE 97x: PER PSW bit 5 */
              #define flceperpsw16 8                   /* FLCE 97x: PER PSW bit 16 */
              #define flceperpsw17 4                   /* FLCE 97x: PER PSW bit 17 */
              #define flceperasceid 3                  /* FLCE 97x: PER ASCE identification. If a */

            };
          };
        };
        char           /*  CL8 */ flceper[8];    /* FLCE 98x: PER address */
      };
    };
    struct {
      char           __filler33[152];
      char           /*  CL4 */ flceperw0[4];  /* FLCE 98x: PER address word 0 */
      void * __ptr32 /*  AL4 */ flceperw1;     /* FLCE 9Cx: PER address word 1 */
      char           /*  XL1 */ flceeaid;      /* FLCE A0x: Exception access ID (The AR number */

      #define flceeaid0 128                    /* Bit 0 of EAID. Zero */
      #define flceeaid1 64                     /* Bit 1 of EAID. Zero */
      #define flceeaid2 32                     /* Bit 2 of EAID. Set only when PIC 2C for PTI */
      #define flceeaid3 16                     /* Bit 3 of EAID. Set only when PIC 2C for SSAIR */
      #define flceeaid_arnum 15                /* AR number. Zero when Bit 1 or Bit 2 is set */

      char           /*  XL1 */ flceperaid;    /* FLCE A1x: PER access ID (the access register */
      char           /*  XL1 */ flceopacid;    /* FLCE A2x: */
      char           /*  CL1 */ flceamdid;     /* FLCE A3x: Architecture mode ID (See FLCARCH */

      #define flceloeme 1                      /* Logout is Z/Architecture */

      void * __ptr32 /*  AL4 */ flcempl;       /* FLCE A4x: MPL address */
      union {
        char           /*  CL8 */ flceteid[8];   /* FLCE A8x: Translation exception */
        char           /*  CL8 */ flcetea[8];    /* FLCE A8x: Translation exception address */
        struct {
          char           __filler34[6];
          unsigned char  /*  BL1 */ flcetea6;      /* FLCE AEx: Byte 6 of FlceTEA */

          #define flceaefsi 12                     /* Access-exception Fetch/Store indicator: 00 -- */

          unsigned char  /*  BL1 */ flcetea7;      /* FLCE AFx: Byte 7 of FlceTEA */

          #define flcepealc 8                      /* FLCE AFx: Protection exception due to */
          #define flcesopi 4                       /* FLCE AFx: Suppress on protection indication */
          #define flceteastd 3                     /* FLCE AFx: Segment table designation for TEA: */

        };
        char           /*  CL8 */ flceteasninfo[8]; /* FLCE A8x: ASN Info */
        struct {
          char           __filler35[6];
          short int      /*  HL2 */ flceteasn;     /* FLCE AEx: ASN */
        };
        char           /*  CL8 */ flcetepcinfo[8]; /* FLCE A8x: PC Info */
      };
    };
    struct {
      char           __filler36[172];
      int            /*  FL4 */ flcepcnum;     /* FLCE ACx: PC#. Bits 0-10 are 0, bit 11 is 1, */
      char           /*  CL8 */ flcemonitorcode[8]; /* FLCE B0x: Monitor Code */
      char           /*  CL4 */ flcessid[4];   /* FLCE B8x: Subsystem ID word */
      char           /*  CL4 */ flceiointparm[4]; /* FLCE BCx: I/O interruption parameter */
      char           /*  CL4 */ flceiointid[4]; /* FLCE C0x: I/O interruption ID */
      char           /*  CL4 */ flcer0c4[4];   /* FLCE C4x: Reserved */
      union {
        char           /*  CL16 */ flcefacilitieslist[16]; /* FLCE C8x: Facilities list stored by STFLE. */
        struct {
          unsigned char  /*  BL1 */ flcefacilitieslistbyte0; /* FLCE C8x */

          #define flcezarchn3 128                  /* Instructions marked "N3" in the instruction */
          #define flceesamen3 128                  /* Instructions marked "N3" in the instruction */
          #define flcezarchinstalled 64            /* The z/Architecture mode is installed on */
          #define flceesameinstalled 64            /* The z/Architecture mode is installed on */
          #define flcezarch 32                     /* The z/Architecture mode is active on the CPU */
          #define flceesame 32                     /* The z/Architecture mode is active on the CPU */
          #define flceidteinstalled 16             /* IDTE is installed */
          #define flceidteclearingcombinedsegment 8 /* IDTE does clearing of */
          #define flceidteclearingcombinedregion 4 /* IDTE does clearing of */
          #define flceasnandlxreuseinstalled 2     /* The ASN and LX reuse facility is */
          #define flcestfle 1                      /* STFLE instruction is available */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte1; /* FLCE C9x */

          #define flceedatfeat 128                 /* DAT features */
          #define flcesenserunningstatus 64        /* sense-running-status facility */
          #define flcecondsskeinstalled 32         /* The conditional SSKE instruction is */
          #define flceconfigurationtopology 16     /* STSI-enhancement for configuration */
          #define flcecqcif 8                      /* 110524 */
          #define flceipterange 4                  /* IPTE-range facility is installed */
          #define flcenonqkeysetting 2             /* Nonquiescing key-setting facility is */
          #define flceapft 1                       /* The APFT facility is installed / 091111 */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte2; /* FLCE CAx */

          #define flceetf2 128                     /* Extended translation facility 2 is present */
          #define flcecryptoassist 64              /* The cryptographic assist is present */
          #define flcemessagesecurityassist 64     /* The message security assist is */
          #define flcelongdisplacement 32          /* The long displacement facility is */
          #define flcelongdisplacementhp 16        /* The long displacement facility has */
          #define flcehfpmas 8                     /* The HFP Multiply add/subtract facility is */
          #define flceextendedimmediate 4          /* The extended immediate facility is */
          #define flceetf3 2                       /* The extended translaction facility 3 is */
          #define flcehfpunnormextension 1         /* The HFP unnormalized extension */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte3; /* FLCE CBx */

          #define flceetf2e 128                    /* ETF2 enhancement is present 031215 */
          #define flcestckf 64                     /* STCKF enhancement is present */
          #define flceparse 32                     /* Parsing enhancement facility is present */
          #define flcetcsf 8                       /* TOD clock steering facility */
          #define flceetf3e 2                      /* ETF3 enhancement is present 040512 */
          #define flceectf 1                       /* Extract Cpu Time facility */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte4; /* FLCE CCx */

          #define flcecssf 128                     /* Compare-and-swap-and-store facility */
          #define flcecssf2 64                     /* Compare-and-swap-and-store facility 2 */
          #define flcegeneralinstextension 32      /* General-Instructions- Extension */
          #define flceenhancedmonitor 8            /* The Enhanced Monitor facility is */
          #define flceobsoletecpumeasurement 1     /* Obsolete. Meant CPU-measurement */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte5; /* FLCE CDx */

          #define flcesetprogramparm 128           /* Set-Program-Parameter facility is */
          #define flcefpsef 64                     /* Floating-point-support enhancement facility */
          #define flcedfpf 32                      /* Decimal-floating-point facility */
          #define flcedfpfhp 16                    /* Decimal-floating-point facility high */
          #define flcepfpo 8                       /* PFPO instruction 070424 */
          #define flcedistinctoperands 4           /* z196 is the first machine with this */
          #define flcehighword 4
          #define flceloadstoreoncondition 4
          #define flcepopulationcount 4
          #define flcecmpef 1                      /* Possible future enhancement */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte6; /* FLCE CEx */

          #define flcemiscinstext 64               /* Bit 49 - Miscellaneous instruction */
          #define flceexecutionhint 64             /* Bit 49 - Execution hint facility. */
          #define flceloadandtrap 64               /* Bit 49 - Load and trap facility. */
          #define flceconstrainedtx 32             /* Bit 50 - Constrained Transactional */
          #define flceloadstoreoncond2 4           /* Bit 53 - */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte7; /* FLCE CFx */

          #define flcemie2 32                      /* Bit 58 */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte8; /* FLCE D0x bits 64-71 */

          #define flceri 128                       /* FlceRI */
          #define flcecryptoapqai 64               /* Crypto AP-Queue adapter interruption */
          #define flcecpumeasurementcounter 16     /* CPU-measurement counter facility */
          #define flcecpumeasurementsampling 8     /* CPU-measurement sampling facility */
          #define flcesclp 4                       /* Possible future enhancement */
          #define flceaisi 2                       /* AISI facility, bit 70 */
          #define flceaen 1                        /* AEN facility, bit 71 */

          unsigned char  /*  BL1 */ flcefacilitieslistbyte9; /* FLCE D1x bits 72-79 */

          #define flceais 128                      /* AIS facility, bit 72 */
          #define flcetransactionalexecution 64    /* Bit 73 - Transactional execution */
          #define flcemsa4 4                       /* MSA4 facility, bit 77 */
          #define flceedat2 2                      /* Bit 78 - Enhanced Dat-2 */

          unsigned char  /*  BL1 */ flcefacilitieslistbytea; /* FLCE D2x */
          unsigned char  /*  BL1 */ flcefacilitieslistbyteb; /* FLCE D3x */
          unsigned char  /*  BL1 */ flcefacilitieslistbytec; /* FLCE D4x */
          unsigned char  /*  BL1 */ flcefacilitieslistbyted; /* FLCE D5x */
          unsigned char  /*  BL1 */ flcefacilitieslistbytee; /* FLCE D6x */
          unsigned char  /*  BL1 */ flcefacilitieslistbytef; /* FLCE D7x */
          char           /*  CL16 */ flcefacilitieslist1[16]; /* FLCE D8x: Facilities list stored by STFLE. */
          char           /*  CL8 */ flcemcic[8];   /* FLCE E8x: Machine check interruption code */
          char           /*  CL4 */ flcemcice[4];  /* FLCE F0x: Machine check interruption code */
          char           /*  CL4 */ flceedcode[4]; /* FLCE F4x: External damage code */
          char           /*  CL8 */ flcefsa[8];    /* FLCE F8x: Failing storage address */
          void * __ptr64 /*  AL8 */ flceemfctrarrayaddr; /* FLCE 100x: The enhanced monitor facility */
          int            /*  FL4 */ flceemfctrarraysize; /* FLCE 108x: The enhanced monitor facility */
          int            /*  FL4 */ flceemfexceptioncnt; /* FLCE 10Cx: The enhanced monitor facility */
          char           /*  CL8 */ flcebea[8];    /* FLCE 110x: Breaking event address */
          char           /*  CL8 */ flcer118[8];   /* FLCE 118x: Reserved */
          char           /*  CL16 */ flceropsw[16]; /* FLCE 120x: Restart old PSW */
          char           /*  CL16 */ flceeopsw[16]; /* FLCE 130x: External old PSW */
          char           /*  CL16 */ flcesopsw[16]; /* FLCE 140x: SVC old PSW */
          char           /*  CL16 */ flcepopsw[16]; /* FLCE 150x: Program old PSW */
          char           /*  CL16 */ flcemopsw[16]; /* FLCE 160x: Machine check old PSW */
          char           /*  CL16 */ flceiopsw[16]; /* FLCE 170x: I/O old PSW */
          char           /*  CL32 */ flcer180[32]; /* FLCE 180x: reserved */
          char           /*  CL16 */ flcernpsw[16]; /* FLCE 1A0x: Restart new PSW */
          char           /*  CL16 */ flceenpsw[16]; /* FLCE 1B0x: External new PSW */
          char           /*  CL16 */ flcesnpsw[16]; /* FLCE 1C0x: SVC new PSW */
          char           /*  CL16 */ flcepnpsw[16]; /* FLCE 1D0x: Program new PSW */
          char           /*  CL16 */ flcemnpsw[16]; /* FLCE 1E0x: Machine check new PSW */
          char           /*  CL16 */ flceinpsw[16]; /* FLCE 1F0x: I/O new PSW */

          #define flcesame_len 512

        };
      };
    };
  };
};

#endif  /* psa__ */


#pragma pack(reset)

