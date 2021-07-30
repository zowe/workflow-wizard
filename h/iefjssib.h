#pragma pack(packed)


#ifndef ssib__
#define ssib__


struct ssib {


  #define ssibegin 0


  char           /*  CL4 */ ssibid[4];     /* CONTROL BLOCK IDENTIFIER */
  short int      /*  RL2 */ ssiblen;       /* SSIB LENGTH */
  unsigned char  /*  BL1 */ ssibflg1;      /* FLAGS */


  #define ssibpjes 128                     /* THIS SSIB IS USED TO START THE */
  #define ssibnsvc 64                      /* NO SVC INDICATOR             @G38RP2Q */


  char           /*  XL1 */ ssibssid;      /* SUBSYSTEM IDENTIFIER. SET    @YC01974 */


  #define ssibunkn 0                       /* UNKNOWN SUBSYSTEM ID         @YA01974 */
  #define ssibjes2 2                       /* JES2 SUBSYSTEM ID            @YA01974 */
  #define ssibjes3 3                       /* JES3 SUBSYSTEM ID            @YA01974 */


  char           /*  CL4 */ ssibssnm[4];   /* Subsystem name to which a        @P1C */
  char           /*  CL8 */ ssibjbid[8];   /* Job Identifier or Subsystem name @P1C */
  char           /*  CL8 */ ssibdest[8];   /* DEFAULT USERID FOR SYSOUT DESTINATION */
  int            /*  FL4 */ ssibrsv1;      /* RESERVED */
  int            /*  FL4 */ ssibsuse;      /* RESERVED FOR SUBSYSTEM USAGE */


  #define ssibsize 36                      /* SSIB LENGTH */


};


#endif  /* ssib__ */




#pragma pack(reset)


