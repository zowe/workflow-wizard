.*
.*
.*  This program and the accompanying materials are
.*
.*  made available under the terms of the Eclipse Public License v2.0 which accompanies
.*
.*  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
.*
.*  
.*
.*  SPDX-License-Identifier: EPL-2.0
.*
.*  
.*
.*  Copyright Contributors to the Zowe Project.
.*
.*
         TITLE 'CVIDYNA - Dynalloc Services'
***********************************************************************
*  Description:
*    Perform dynamic concatenation of an array of DD Names.
*
*
*  Input:
*    R1 -> DYNP
*
*  Register Usage:
*    R3  - DYNP    - Input parms
*    R8  - DDARRAY - Input DD array
*    R9  - TUPFX   - Text Unit Prefix
*          DDPARM  - Text unit DD parm entry
*    R11 - Code Base
*    R13 - Save area & working stg
*
*  Return Value:
*    0 = Success, 4=Warn, 8=Error, 12=Severe
*
*  Output:
*    None
*                     
***********************************************************************
CVIDYNA  $BEGIN AMODE=31,BASE=11,RMODE=ANY
*** -------------------------------------------------------------------
*** Get working storage
*** -------------------------------------------------------------------
         STORAGE OBTAIN,COND=YES,                                      >
               LENGTH=WKSTGL,                                          >
               LOC=31

         LTR   R15,R15                 Got storage?
         BNZ   DYNAE000                 No -> Exit

         ST    R13,4(,R1)              Set bwd sa pointer
*         ST    R1,8(,R13)              Set fwd sa pointer
         LR    R13,R1                  Set active sa
         USING WKSTG,R13               Local stg addressability
         $WKSTG INIT,LEN=WKSTGL        Init common wk stg header
*** -------------------------------------------------------------------
***      Get Parms - DYNP
*** -------------------------------------------------------------------
DYNA0000 DS    0H
         EREG  R1,R1                   R1 on entry -> PARMS
         LR    R3,R1
         USING DYNP,R3                 Address I/P parms
         ST    R3,DYNP@                Save I/P parms addr

***      Get function code address
*
         ICM   R1,15,DYNPCFC@          Got a function code pointer?
         BZ    DYNAE010                 No -> Error

*** -------------------------------------------------------------------
***      Index into function code vector table
*** -------------------------------------------------------------------
         ICM   R1,15,0(R1)             Valid command function code?
         BM    DYNAE010                 No ->

         LA    R0,CMDTBL#              Get max vector table ix number
         CR    R1,R0                   Valid command code?
         BH    DYNAE010                 No -> Too big

         SLL   R1,2                    Convert index to an offset
         L     R15,CMDTBL(R1)          Get function routine addr
         BR    R15                     -> Function processor
*
***      Command code vector table follows...
*
CMDTBL   DC    A(DYNAE010)             0 = Bad vector index
         DC    A(DYNA1000)             1 = Concatenate
         DC    A(DYNA2000)             2 = Deconcatenate
         DC    A(DYNA3000)             3 = Allocate SYSOUT
         DC    A(DYNA4000)             4 = Deallocate SYSOUT
CMDTBLL  EQU   *-CMDTBL                Length of vector table
CMDTBL#  EQU   (CMDTBLL/4)-1           Max vector table index #
         DROP  R3                      DYNP
         EJECT ,
*** -------------------------------------------------------------------
*** Dynamically concatenate an input array of 8-byte left justified
*** blank padded DD names.
***
*** Input:
***   R1 -> +0 = A(Function Code)
***         +4 = A(Reason Code)
***         +8 = A(Count of DDs in array)
***         +C = A(DD Array)
***        +10 = A(Permanent concatenation flag)
***        +14 = A(Error code)
***
*** Output:
***   Reason code field updated on error
***
*** Return Value:
***   R15 = Return Code
***   R0  = Reason Code
*** -------------------------------------------------------------------
DYNA1000 DS    0H
         L     R3,DYNP@                Get I/P parms addr
         USING DYNP,R3                 Address I/P parms
         MVC   DYNAREQ,=CL8'CONCAT'    Set DYNALLOC request type
*
***      Get count of DD's in the DD array...
*
         ICM   R1,15,DYNPDD#@          Got DD count pointer?
         BZ    DYNAE040                 No -> Error

         ICM   R1,15,0(R1)             Got any DD's in array?
         BZ    DYNAE040                 No -> Error

         ST    R1,DDNUM                Save DD count
*
***      Get addr of DD name array
*
         ICM   R8,15,DYNPDDA@          Got DD name array?
         BZ    DYNAE040                 No -> Error

         ST    R8,DDARRAY@             Save DD array pointer
         USING DDARRAY,R8              Address input DD array
*
***      Get permanent concatenation flag
*
         ICM   R1,15,DYNPERM@          Got perm flag pointer?
         BZ    DYNAE040                 No -> Error

         MVC   SETPERM,0(R1)           Save permanent concat flag
         MVC   ERROR@,DYNERR@          Save SVC99 error code ptr?
         DROP  R3                      DYNP
*
***      Obtain storage for variable length concatenation text unit
*
         L     R3,DDNUM                Get number of DD's in array
         USING COUNT,R3                Doc register usage!
         LR    R2,R3                   Multiple number of DD's by...
         MH    R2,=Y(DDPARML)           TU parm entry length
         AH    R2,=Y(TUPFXL)           Add on TU fixed prefix length
         ST    R2,TUSTGL               Save TU storage length
         STORAGE OBTAIN,COND=YES,                                      >
               LENGTH=(R2),                                            >
               LOC=31

         LTR   R15,R15                 Got storage ok?
         BNZ   DYNAE020                 No -> Exit

         ST    R1,TUSTG@               Save text unit stg addr
         LR    R9,R1                   Addr of DD concat TU stg
         USING TUPFX,R9                Address prefix area
         MVC   TUKEY,=AL2(DCCDDNAM)    Set text unit key
         STCM  R3,3,TUPRM#             Set number of DD parm entries
         AH    R9,=Y(TUPFXL)           Skip past TU prefix
         DROP  R9                      TUPFX
         USING DDPARM,R9               Address each DD parm entry
         L     R8,DDARRAY@             Get DD array address
         USING DDARRAY,R8              Address DD array
DYNA1100 DS    0H
         MVC   DDPLEN,=XL2'0008'       Set length of TU parm entry
         MVC   DDPDATA(8),DDARRAY      DD name value in TU parm entry
         LA    R8,8(,R8)               Next entry in DDARRAY
         LA    R9,DDPARML(,R9)         Next TU parm entry
         BCT   R3,DYNA1100             More DD's? Yes -> Loop

         DROP  R3,R8,R9                COUNT, DDARRAY, DDPARM
*
***      Set up optional permanent concatenation flag (DCCPERMC)
*
         LA    R9,PERMTU               Get TU addr
         USING TUPFX,R9                Address TU for DCCPERMC
         MVC   TUKEY,=AL2(DCCPERMC)    Set text unit key
         XC    TUPRM#,TUPRM#           Parm # is zero
         DROP  R9                      TUPFX
***      Set up the DYNALLOC(SVC 99) request block...
*
         LA    R3,Z99RB                Get SVC99 RB Addr
         USING S99RB,R3                Address SVC 99 request block
         ST    R3,Z99RBP               Set SVC99 RB pointer
         OI    Z99RBP,S99RBPND         HOB must be set
*
***      Initialize the SVC99 request block
*
         MVI   S99RBLN,S99RBL          Set request block length
         MVI   S99VERB,S99VRBCC        Set concatenation request
         OI    S99FLG11,S99NOMNT       Do not mount offline units
*         OI    S99FLG11,S99CNENQ       Issue conditional ENQ on TIOT
         OI    S99FLG11,S99MSGL0       Always use MSGLEVEL=(,0)
*         OI    S99FLG11,S99NOMIG       Do not recall migrated datasets
         OI    S99FLG12,S99DSABA       Request DSAB above 16M line
         LA    R1,Z99TUPL              Set the address to the text
         ST    R1,S99TXTPP              unit pointer list in the RB
*
***      Build SVC99 text unit pointer list
*
         L     R0,TUSTG@               Text unit stg addr
         ST    R0,Z01TUPTR             Addr of TU in TU ptr list
         LA    R14,Z01TUPTR            Set last TU ptr
         ICM   R1,15,SETPERM           Perm concat flag request?
         BZ    DYNA1200                 No ->

         LA    R0,PERMTU               Get DCCPERMC TU addr
         ST    R0,Z02TUPTR             Set in TU pointer list
         LA    R14,Z02TUPTR            Set last TU ptr
DYNA1200 DS    0H
         OI    0(R14),S99TUPLN         Set HOB (Last TU ptr in list)
*
***      Issue the dynamic concatenation request
*
         LA    R1,Z99RBP               Init R1 parameter list ptr
         DYNALLOC ,                    -> SVC99

         ST    R15,RETCODE             Save return code
*
***      Release TU storage...
*
         LM    R1,R2,TUSTGL            Get storage length & addr
         STORAGE RELEASE,LENGTH=(R1),  Release local storage           >
               ADDR=(R2)

         ICM   R15,15,RETCODE          Was DYNALLOC successful?
         BZ    DYNAEXIT                 Yes ->

         MVC   Z99ERROR,S99ERROR       DYNALLOC error code
         MVC   Z99INFO,S99INFO         DYNALLOC info reason code
         MVC   RSNCODE,Z99RSN          Set reason code
         B     DYNAE050                -> Exit

         DROP  R3                      S99RB

         EJECT ,
*** -------------------------------------------------------------------
*** Dynamically deconcatenate an DD name
***
*** Input:
***   R1 -> +0 = A(Function Code)
***         +4 = A(Reason Code)
***         +8 = A'DD name'  = 8-bytes padded with blanks
***         +C = A(Error code)
***
*** Output:
***   Reason code field updated on error
***
*** Return Value:
***   R15 = Return Code
***   R0  = Reason Code
*** -------------------------------------------------------------------
DYNA2000 DS    0H
         L     R3,DYNP@                Get I/P parms addr
         USING DYNP,R3                 Address I/P parms
         MVC   DYNAREQ,=CL8'DECONCAT'  Set DYNALLOC request type
*
***      Get addr of DD name to deconcat
*
         ICM   R8,15,DYNPDDN@          Got DD name?
         BZ    DYNAE040                 No -> Error

         USING DDN,R8                  Address DD name
         MVC   ERROR@,DYNERR2@         Save SVC99 error code ptr?
         DROP  R3                      DYNP
*
***      Set up allocation TU (DDCDDNAM)
*
         LA    R9,DCONTU               Get TU addr
         USING TUPFX,R9                Address TU for DDCDDNAM
         MVC   TUKEY,=AL2(DDCDDNAM)    Set text unit key
         MVC   TUPRM#,=AL2(1)          Parm # is always 1
         DROP  R9                      TUPFX
         LA    R9,TUPFXL(,R9)          Point past prefix area
         USING DDPARM,R9               Address DD parm
         MVC   DDPLEN,=AL2(8)          DD name length
         MVC   DDPDATA,DDN             DD name in parm field
         DROP  R8                      DDN
***      Set up the DYNALLOC(SVC 99) request block...
*
         LA    R3,Z99RB                Get SVC99 RB Addr
         USING S99RB,R3                Address SVC 99 request block
         ST    R3,Z99RBP               Set SVC99 RB pointer
         OI    Z99RBP,S99RBPND         HOB must be set
*
***      Initialize the SVC99 request block
*
         MVI   S99RBLN,S99RBL          Set request block length
         MVI   S99VERB,S99VRBDC        Set deconcatenation request
         OI    S99FLG11,S99NOMNT       Do not mount offline units
         OI    S99FLG11,S99MSGL0       Always use MSGLEVEL=(,0)
         LA    R1,Z99TUPL              Set the address to the text
         ST    R1,S99TXTPP              unit pointer list in the RB
*
***      Build SVC99 text unit pointer list
*
         LA    R0,DCONTU               Text unit stg addr
         ST    R0,Z01TUPTR             Addr of TU in TU ptr list
         OI    Z01TUPTR,S99TUPLN       Set HOB (Last TU ptr in list)
*
***      Issue the dynamic concatenation request
*
         LA    R1,Z99RBP               Init R1 parameter list ptr
         DYNALLOC ,                    -> SVC99

         ST    R15,RETCODE             Save return code
         LTR   R15,R15                 Was DYNALLOC successful?
         BZ    DYNAEXIT                 Yes ->

         MVC   Z99ERROR,S99ERROR       DYNALLOC error code
         MVC   Z99INFO,S99INFO         DYNALLOC info reason code
         MVC   RSNCODE,Z99RSN          Set reason code
         B     DYNAE050                -> Exit

         EJECT ,
*** -------------------------------------------------------------------
*** Dynamically allocate a SYSOUT DD name, i.e.
***
***   //xxxPRINT DD SYSOUT=*
***
*** Input:
***   R1 -> +0 = A(Function Code)
***         +4 = A(Reason Code)
***         +8 = A'DD name'  = 8-bytes padded with blanks
***         +C = A(Error code)
***
*** Output:
***   Reason code field updated on error
***
*** Return Value:
***   R15 = Return Code
***   R0  = Reason Code
*** -------------------------------------------------------------------
DYNA3000 DS    0H
         L     R3,DYNP@                Get I/P parms addr
         USING DYNP,R3                 Address I/P parms
         MVC   DYNAREQ,=CL8'SYSOUTA'   Set DYNALLOC request type
*
***      Get addr of SYSOUT DD name...
*
         ICM   R8,15,DYNPDDN@          Got DD name?
         BZ    DYNAE040                 No -> Error

         USING DDN,R8                  Address DD name
         MVC   ERROR@,DYNERR2@         Save SVC99 error code ptr?
         DROP  R3                      DYNP
*
***      Note: We use R9 to map Text Unit prefix...
*
         USING TUPFX,R9                Map TU prefix
*
***      Set up DDNAME specification TU (DALDDNAM)
*
         LA    R9,ALDDNAM              Set TU addr
         ST    R9,Z01TUPTR             Addr of TU in TU ptr list
         MVC   TUKEY,=AL2(DALDDNAM)    Set text unit key
         MVC   TUPRM#,=AL2(1)          Parm # is always 1
         LA    R1,TUPFXL(,R9)          Point past prefix area
         USING DDPARM,R1               Address DD parm
         MVC   DDPLEN,=AL2(8)          DD name length
         MVC   DDPDATA,DDN             DD name in parm field
         DROP  R1,R8                   DDPARM, DDN
*
***      Set up SYSOUT specification TU (DALSYSOU)
*
         LA    R9,ALSYSOU              Set TU addr
         ST    R9,Z02TUPTR             Addr of TU in TU ptr list
         MVC   TUKEY,=AL2(DALSYSOU)    Set text unit key
         XC    TUPRM#,TUPRM#           Parm # is 0 (default o/p class)
         DROP  R9                      TUPFX
         OI    Z02TUPTR,S99TUPLN       Set HOB (Last TU ptr in list)
*
***      Set up the DYNALLOC(SVC 99) request block...
*
         LA    R3,Z99RB                Get SVC99 RB Addr
         USING S99RB,R3                Address SVC 99 request block
         ST    R3,Z99RBP               Set SVC99 RB pointer
         OI    Z99RBP,S99RBPND         HOB must be set
*
***      Initialize the SVC99 request block
*
         MVI   S99RBLN,S99RBL          Set request block length
         MVI   S99VERB,S99VRBAL        Allocation request
         OI    S99FLG11,S99NOMNT       Do not mount offline units
*         OI    S99FLG11,S99JBSYS       Treat as part of jobs SYSOUT
         OI    S99FLG11,S99MSGL0       Always use MSGLEVEL=(,0)
*         OI    S99FLG12,S99DSABA       DSAB above the line
*         OI    S99FLG22,S99TIOEX       Extended TIOT for S99DSABA
         LA    R1,Z99TUPL              Set the address to the text
         ST    R1,S99TXTPP              unit pointer list in the RB
*
***      Issue the dynamic SYSOUT allocation request (Note: If the
***      DD is already allocated then S99ERROR=x'0410' and RC=4,
***      which indicates the DD is unavailable)
*
         LA    R1,Z99RBP               Init R1 parameter list ptr
         DYNALLOC ,                    -> SVC99

         ST    R15,RETCODE             Save return code
         CHI   R15,4                   Was DYNALLOC successful?
         BNH   DYNAEXIT                 Yes ->

         MVC   Z99ERROR,S99ERROR       DYNALLOC error code
         MVC   Z99INFO,S99INFO         DYNALLOC info reason code
         MVC   RSNCODE,Z99RSN          Set reason code
         B     DYNAE050                -> Error and exit!

         DROP  R3                      S99RB
         EJECT ,
*** -------------------------------------------------------------------
*** Dynamically unallocate a SYSOUT DD name
***
*** Input:
***   R1 -> +0 = A(Function Code)
***         +4 = A(Reason Code)
***         +8 = A'DD name'  = 8-bytes padded with blanks
***         +C = A(Error code)
***
*** Output:
***   Reason code field updated on error
***
*** Return Value:
***   R15 = Return Code
***   R0  = Reason Code
*** -------------------------------------------------------------------
DYNA4000 DS    0H
         L     R3,DYNP@                Get I/P parms addr
         USING DYNP,R3                 Address I/P parms
         MVC   DYNAREQ,=CL8'SYSOUTD'   Set DYNALLOC request type
*
***      Get addr of SYSOUT DD name...
*
         ICM   R8,15,DYNPDDN@          Got DD name?
         BZ    DYNAE040                 No -> Error

         USING DDN,R8                  Address DD name
         MVC   ERROR@,DYNERR2@         Save SVC99 error code ptr?
         DROP  R3                      DYNP
*
***      Note: We use R9 to map Text Unit prefix...
*
         USING TUPFX,R9                Map TU prefix
*
***      Set up DDNAME specification TU (DUNUNALC)
*
         LA    R9,UNUNALC              Get TU addr
         ST    R9,Z01TUPTR             Addr of TU in TU ptr list
         MVC   TUKEY,=AL2(DUNUNALC)    Set text unit key
         XC    TUPRM#,TUPRM#           Parm # is always 0
*
***      Set up DDNAME specification TU (DUNDDNAM)
*
         LA    R9,UNDDNAM              Get TU addr
         ST    R9,Z02TUPTR             Addr of TU in TU ptr list
         MVC   TUKEY,=AL2(DUNDDNAM)    Set text unit key
         MVC   TUPRM#,=AL2(1)          Parm # is always 1
         LA    R1,TUPFXL(,R9)          Point past prefix area
         USING DDPARM,R1               Address DD parm
         MVC   DDPLEN,=AL2(8)          DD name length
         MVC   DDPDATA,DDN             DD name in parm field
         DROP  R1,R8                   DDPARM, DDN
         OI    Z02TUPTR,S99TUPLN       Set HOB (Last TU ptr in list)
         DROP  R9                      TUPFX
*
***      Set up the DYNALLOC(SVC 99) request block...
*
         LA    R3,Z99RB                Get SVC99 RB Addr
         USING S99RB,R3                Address SVC 99 request block
         ST    R3,Z99RBP               Set SVC99 RB pointer
         OI    Z99RBP,S99RBPND         HOB must be set
*
***      Initialize the SVC99 request block
*
         MVI   S99RBLN,S99RBL          Set request block length
         MVI   S99VERB,S99VRBUN        Set unallocation request
         OI    S99FLG11,S99NOMNT       Do not mount offline units
         OI    S99FLG11,S99JBSYS       Treat as part of jobs SYSOUT
         OI    S99FLG11,S99MSGL0       Always use MSGLEVEL=(,0)
*         OI    S99FLG12,S99DSABA       DSAB above the line
*         OI    S99FLG22,S99TIOEX       Extended TIOT for S99DSABA
         LA    R1,Z99TUPL              Set the address to the text
         ST    R1,S99TXTPP              unit pointer list in the RB
*
***      Issue the dynamic unallocation request
*
         LA    R1,Z99RBP               Init R1 parameter list ptr
         DYNALLOC ,                    -> SVC99

         ST    R15,RETCODE             Save return code
         LTR   R15,R15                 Was DYNALLOC successful?
         BZ    DYNAEXIT                 Yes ->

         MVC   Z99ERROR,S99ERROR       DYNALLOC error code
         MVC   Z99INFO,S99INFO         DYNALLOC info reason code
         MVC   RSNCODE,Z99RSN          Set reason code
         B     DYNAE050                -> Exit

         EJECT ,
*** -------------------------------------------------------------------
***      Exit Processing
*** -------------------------------------------------------------------
DYNAEXIT DS    0H
         LM    R15,R0,RETCODE          Restore return & reason code
         LTR   R15,R15                 Good return code?
         BZ    DYNAEX99                 Yes -> Skip reason code sets

         ICM   R1,15,ERROR@            Got an SVC99 error code ptr
         BZ    DYNAEX99                 No ->

         LH    R14,Z99ERROR            Get SVC99 error code
         ST    R14,0(,R1)              Return to caller
*
***      Release working storage and return to caller
*
DYNAEX99 DS    0H
         L     R3,4(,R13)              Get prev sa
         USING PREVSA,R3               Indicate reg usage
         STM   R15,R0,16(R3)           Save return and reason code
         L     R1,$WKLEN               Get stg length
         STORAGE RELEASE,LENGTH=(R1),  Release local storage           >
               ADDR=(R13)

         LM    R15,R0,16(R3)           Get return and reason code
         DROP  R3                      PREVSA
DYNAQUIT DS    0H
         PR    ,                       -> Return to caller

         EJECT ,
*** -------------------------------------------------------------------
***      Convert hex to displayable characters
*** -------------------------------------------------------------------
CNVTHEX  DS    0H
         ST    R14,HEXRETN@
         $HEX  TYPE=DISPLAY,DATA=(R2)
         L     R14,HEXRETN@
         BR    R14

*** -------------------------------------------------------------------
***      Message Processing Subroutines
*** -------------------------------------------------------------------
*
***      Main working stg OBTAIN failed
*
DYNAE000 DS    0H
         B     DYNAQUIT                -> Quit
*
***      Invalid command/function code
*
DYNAE010 DS    0H
         B     DYNAE911                -> Exit
*
***      Failed to obtain storage
*
DYNAE020 DS    0H
         B     DYNAE911                -> Exit
*
***      No reason code pointer supplied
*
DYNAE030 DS    0H
         B     DYNAE911                -> Exit
*
***      No DD names supplied or NULL DD name array addr
*
DYNAE040 DS    0H
         B     DYNAE911                -> Exit

*
***      Display DYNALLOC return, error & reason codes
*
DYNAE050 DS    0H
         B     DYNAEXIT                -> Exit
*
***      911 Emergency
*
DYNAE911 DS    0H
         STM   R15,R0,RETCODE          Save return & reason code
*
***      Issue internal error msg
         B     DYNAEXIT                -> Exit

         EJECT ,
***********************************************************************
***      Constant definition area
***********************************************************************
         LTORG ,
*
***      Local Messages
*
MSG010   DC    0C' '
MSG0101  DC    C'CVIDYNA - Internal error: rc='
MSG0102  DC    C'nn, reason='
MSG010L  EQU   *-MSG010
*
MSG020   DC    0C' '
MSG0201  DC    C'CVIDYNA - DYNALLOC '
MSG0202  DC    C'aaaaaaaa request failed, rc='
MSG0202L EQU   *-MSG020
MSG0203  DC    C'nn error='
MSG0203L EQU   *-MSG020
MSG0204  DC    C'xxxx reason='
MSG020L  EQU   *-MSG020
*
         EJECT ,
***********************************************************************
***      Local Program Storage
***********************************************************************
         $WKSTG DEFINE
*
DBLWORD  DS    D                       Double word work area
         ORG   DBLWORD
FULWORD  DS    F                       Single word work area
         ORG   ,
*
HEXRETN@ DS    A                       Hex conversion rtn return addr
TERMRTN@ DS    A                       Term cleanup return addr
***      Do NOT move positional dependency follows...
RETCODE  DS    F                       Return code
RSNCODE  DS    F                       Reason code
*
TUSTGL   DS    F                       Text unit storage length
TUSTG@   DS    A                       Text unit storage addr
***      End of positional dependency!
ERROR@   DS    A                       Addr of callers SVC99 error code
DYNP@    DS    A                       Addr of i/p parms
DDNUM    DS    F                       Number of entries in DD array
DDARRAY@ DS    A                       Addr of 8-byte DD name array
SETPERM  DS    F                       Permanent concat flag
*
Z99RBP   DS    0F                      See S99RBP DSECT
Z99RBPTR DS    F                       REQUEST BLOCK POINTER
*
Z99RB    DS    CL(S99RBL)              See S99RB DSECT
*
Z99TUPL  DS    0F                      TEXT UNIT POINTER LIST
Z01TUPTR DS    F                       1st TU pointer
Z02TUPTR DS    F                       2nd TU pointer
*Z03TUPTR DS    F                       3rd TU pointer
*
TUAREA   DS    0F
PERMTU   DS    CL(TUPFXL)              Permananent concatenation TU
         ORG   TUAREA
DCONTU   DS    CL(TUPFXL+DDPARML)      Deconcatenation TU
         ORG   TUAREA
*ALCLOSE  DS    CL(TUPFXL)              For DALCLOSE text unit
ALDDNAM  DS    CL(TUPFXL+DDPARML)      For DALDDNAM text unit
ALSYSOU  DS    CL(TUPFXL+CLPARML)      For DALSYSOU text unit
         ORG   TUAREA
*
UNUNALC  DS    CL(TUPFXL)              For DUNUNALC text unit
UNDDNAM  DS    CL(TUPFXL+DDPARML)      For DUNDDNAM text unit
         ORG   ,
*
Z99RSN   DS    0F                      Reason code from DYNALLOC
Z99ERROR DS    H                       DYNALLOC error code
Z99INFO  DS    H                       DYNALLOC information reason code
*
DYNAREQ  DS    CL8                     DYNALLOC request
*
         DS    0F                      FULLWORD ALIGN
REPT     DS    CL130
         ORG   ,
*
WKSTGL   EQU   *-WKSTG                 Working stg len
WKVAL    EQU   *-$WKVA                 Variable stg len
         EJECT ,
***********************************************************************
***      Local DSECT's...
***********************************************************************
*
***      Our input parameters...
*
DYNP     DSECT ,
DYNPCFC@ DS    A                       Command Function Code pointer
DYNPAREA DS    0C                      Start of common area
***      CONCATENATE parms only...
DYNPDD#@ DS    A                       Ptr to number of DD's in array
DYNPDDA@ DS    A                       Ptr to DD name array
DYNPERM@ DS    A                       Ptr to perm concat flag (1=Set)
DYNERR@  DS    A                       Ptr to error code area
         ORG   DYNPAREA
***      DECONCATENATE, ALLOC, DEALLOC parms...
DYNPDDN@ DS    A                       Ptr to DD name
DYNERR2@ DS    A                       Ptr to error code area
         ORG   ,
***********************************************************************
***      Dynamic Allocation Mapping Macros
***********************************************************************
         IEFZB4D0 ,                    SVC99 Parameter List
S99RBL   EQU   S99RBEND-S99RB          SVC99 Request Block Length
         IEFZB4D2 ,                    SVC99 text unit mnemonics

*
TUPFX    DSECT ,                       Text unit prefix
TUKEY    DC    XL2'0000'               Text unit key
TUPRM#   DC    XL2'0000'               Text unit parm count
TUPFXL   EQU   *-TUPFX                 Text unit prefix length
TUPRML   DC    XL2'0000'               Text unit parm length
TUPRMV   DC    0X'00'                  Text unit parm value
*
DDPARM   DSECT ,                       TU DD parm
DDPLEN   DS    XL2'0008'               DD parm length
DDPDATA  DS    CL8' '                  DD parm data = 8-byte DDNAME
DDPARML  EQU   *-DDPARM                Length of CONCAT DD parm entry
*
CLPARM   DSECT ,                       TU Class parm
CLPLEN   DS    XL2'0001'               Class parm length
CLPDATA  DS    C' '                    1-byte SYSOUT class
CLPARML  EQU   *-CLPARM                Length of CONCAT DD parm entry
*
         EJECT ,
***********************************************************************
***      Local DSECT's used to document register usage
***********************************************************************
PREVSA   DSECT ,                       Previous save area
COUNT    DSECT ,                       Count register
DDARRAY  DSECT ,                       Array of 8-byte DD names
DDN      DSECT ,                       DD name
*
         EJECT ,
***********************************************************************
***      z/OS DSECT's
***********************************************************************
         CVT   DSECT=YES               CVT
         IHAPSA ,                      PSA
         END
