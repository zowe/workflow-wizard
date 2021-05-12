/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Makes z/OS calls
// 
//****************************************************************************

//****************************************************************************
// Includes
//****************************************************************************
#include <string.h>                    // standard strings
#include <unistd.h>                    // does time operations
#include <time.h>                      // time
#include <stdio.h>                     // stdio

#include "CviOs.h"                     // include ourself
#include "ihapsa.h"                    // PSA
#include "cvt.h"                       // CVT
#include "ikjtcb.h"                    // TCB
#include "ieftiot1.h"                  // TIOT

/***************************************************************************/
/*                              Begin Defines                              */
/*                                                                         */

#define CVIOS_HOBOFF(a)              \
 (((int) (a)) & 0x7fffffff)
 
#define CVIOS_HOBON(a)               \
 (((int) (a)) | 0x80000000)
 
/*                                                                         */
/*                               End Defines                               */
/***************************************************************************/


/***************************************************************************

    Function    : CviOsIsDefined

    Description : Is DD defined?

    Parameters  : 1) DD name

    Returns     : TRUE of FALSE

***************************************************************************/
char CviOsIsDefined(const char *theDD)

{

char    aRc = FALSE;                    // assume not defined


struct psa *psPsa = 0;                 // PSA
                                       
tcbfix *aTcb = (tcbfix *)              // get TCB prefix address 
    ((char *) psPsa->psatold - 32);        
                                       
tiot   *aTiot =                        // get TIOT 
    (tiot *) aTcb->tcbtio;             
                                       
while (!aRc                         && // while not found yet and
       aTiot->tioelngh != 0)           // another TIOT found
                                       
{                                      // begin walk TIOT
                                       
    if (!memcmp(aTiot->tioeddnm,       // if debug DD
                theDD,
                8))                
                                       
        aRc = TRUE;                    // indicate true
                                       
    aTiot = (struct tiot *)            // go to next TIOT
            ((char*) aTiot + aTiot->tioelngh);
                                       
};                                     // end walk TIOT


return(aRc);                           // return result

}

/***************************************************************************

    Function    : CviOsGetDDs

    Description : Get list of DDs

    Parameters  : 1) String to return comma-delimited list

    Returns     : TRUE

***************************************************************************/
char CviOsGetDDs(CviStr &theResult)

{

char    aRc = TRUE;                    // assume OK


struct psa *psPsa = 0;                 // PSA
                                       
tcbfix *aTcb = (tcbfix *)              // get TCB prefix address 
    ((char *) psPsa->psatold - 32);        
                                       
tiot   *aTiot =                        // get TIOT 
    (tiot *) aTcb->tcbtio;             
                                       
while (aTiot->tioelngh != 0)           // another TIOT found 
                                       
{                                      // begin walk TIOT
                                       
    if (strlen(theResult) > 0)         // if already have items

        theResult.Add(",");            // add comma

    if (aTiot->tioeddnm[0] != 0)       // if name looks OK

       theResult.Add(aTiot->tioeddnm, 8); // add DD

    theResult.Trim(' ');               // trim trailing spaces

    aTiot = (struct tiot *)            // go to next TIOT
            ((char*) aTiot + aTiot->tioelngh);
                                       
};                                     // end walk TIOT


return(aRc);                           // return result

}

//****************************************************************************
// CviOS - Varios OS functions
//****************************************************************************
void *CviOsLoad(const char *theMod)
{

  __register(0)  void* r0;             // entry point addr
  __register(1)  int r1;               // reason code
  __register(14) const char* r14;      // ptr to module name
  __register(15) int r15;              // return code

  void *aEp=NULL;

  r14 = theMod;                        // set module name

  __asm {
    LOAD EPLOC=(14),ERRET=LOADE000     // load
  }

  if (r15 == 0)                        // if good

    aEp = r0;                          // set EP

  if (aEp == NULL)                     // if bad

  {                                    // begin LOAD error

__asm {
LOADE000    DS  0H
}

      aEp = NULL;                      // return nothing

  };                                   // end LOAD error

  return aEp;                          // return EP

};

void CviOsDelete(const char *theMod)
{
  
  __register(0)  void* r0;             // entry point addr
  __register(1)  int r1;               // reason code
  __register(14) const char* r14;      // ptr to module name
  __register(15) int r15;              // return code

  r14 = theMod;                        // set module name

  __asm {
    DELETE EPLOC=(14)                  // delete
  }

};


/***************************************************************************

    Function    : CviAttach

    Description : Attach a new task

    Parameters  : 1) EP
                  2) Parameter block
                  3) ECB address

    Returns     : TCB address attached

***************************************************************************/

void *CviAttach

(                                      // Begin Define Function Parameters

void  *theCall,                        // call address
void  *theParms,                       // parameters
void  *theEcb

)                                      // End Define Function Parameters

{                                      // Begin Attach()

__register (8) void  *aModule;         // use R8 for task entry point
__register (7) void  *aEcb = theEcb;   // ECB
__register (6) void  *aCall = theCall;

void *aPtr;

__register (1) void  *r_c;             // return code holder
__register (0) long  r0;               // uses r0
__register (14) long  r14;             // uses r14
__register (15) long  r15;             // uses r15

char    AttachParms[1024];             // attach parms

__register (9) char *aR9 = AttachParms;

aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);

memset(AttachParms, 0, sizeof(AttachParms));

char aEp[9];

static int aCount = 0;

sprintf(aEp, "CVIEP%03d", aCount ++);

aModule = aEp;

__asm

{
    IDENTIFY EPLOC=(8),ENTRY=(6)
}

r_c = (char *) theParms;               // set parameters

__asm

{
    ATTACHX EPLOC=(8),ECB=(7),SF=(E,(9))
}

aPtr = r_c;

//printf("Attached %p at %s as %p\n", aCall, aModule, aPtr);



return((void *) aPtr);                 // return attach result

}                                      // End Attach()

/***************************************************************************

    Function    : CviAttachJs

    Description : Attach a new JS TCB

    Parameters  : 1) EP
                  2) Parameter block
                  3) ECB address
                  4) Supervisor state?

    Returns     : TCB address attached

***************************************************************************/

void *CviAttachJs

(                                      // Begin Define Function Parameters

void  *theCall,                        // call address
void  *theParms,                       // parameters
void  *theEcb

)                                      // End Define Function Parameters

{                                      // Begin Attach()

__register (8) void  *aModule;         // use R8 for task entry point
__register (7) void  *aEcb = theEcb;   // ECB
__register (6) void  *aCall = theCall;

void *aPtr;

__register (1) void  *r_c;             // return code holder
__register (0) long  r0;               // uses r0
__register (14) long  r14;             // uses r14
__register (15) long  r15;             // uses r15

char    AttachParms[1024];             // attach parms

__register (9) char *aR9 = AttachParms;

aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);

memset(AttachParms, 0, sizeof(AttachParms));

char aEp[9];

static int aCount = 0;

sprintf(aEp, "CVIEP%03d", aCount ++);

aModule = aEp;

__asm

{
    IDENTIFY EPLOC=(8),ENTRY=(6)
}

r_c = (char *) theParms;               // set parameters

__asm

{
    ATTACHX EPLOC=(8),ECB=(7),JSTCB=YES,SF=(E,(9))
}

aPtr = r_c;

//printf("Attached %p at %s as %p\n", aCall, aModule, aPtr);



return((void *) aPtr);                 // return attach result

}                                      // End Attach()

/***************************************************************************

    Function    : CviDetach

    Description : Detach a task

    Parameters  : 1) TCB

    Returns     : None

***************************************************************************/

void CviDetach

(                                      // Begin Define Function Parameters

void  *theTcb                          // TCB

)                                      // End Define Function Parameters

{                                      // Begin Detach()

__register (0) long  r0;               // uses r0
__register (1) long  r1;               // uses r1
__register (7) unsigned int   *aTcb;   
__register (14) long  r14;             // uses r14
__register (15) long  r15;             // uses r15

   unsigned int aTcbInt;               // TCB


   aTcbInt = (unsigned int) theTcb;    // set into int
   aTcbInt = aTcbInt & 0x7fffffff;     // turn off high bit

   aTcb = &aTcbInt;                    // set TCB 

   __asm

   {

        DETACH (7),STAE=NO

   }

}                                      // End Detach()


/***************************************************************************

    Function    : CviWait

    Description : Wait on ECB

    Parameters  : 1) ECB address
                  2) Timeout

    Returns     : NONE

***************************************************************************/
void CviWait

(                                      // Begin Define Function Parameters

void  *theEcb,                         // ECB to wait upon
struct timeval *theTimeout             // timeout

)                                      // End Define Function Parameters

{                                      // Begin WaitEcb()

__register (0)  long  r0;              // uses r0
__register (1)  long  r1;              // uses r1
__register (2)  void *aEcb = theEcb;
__register (14) long  r14;             // uses r14
__register (15) long  r15;             // uses r15


aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);


if (theTimeout == NULL)                // if no timeout

{                                      // begin WAIT

__asm

{

   WAIT ECB=(2)

}

}                                      // end WAIT

else                                   // otherwise

{                                      // begin SELECTEX

    void  *aParms[16];                 // parms
    int    aZero = 0;                  // zero parm
    int    aErr = 0;                   // error code
    int    aErrNo = 0;                 // errno
    int    aErrRsn = 0;                // error reason

    aParms[0] = &aZero;                // no high socket
    aParms[1] = &aZero;                // no read list
    aParms[2] = &aZero;                // no read list
    aParms[3] = &aZero;                // no write list
    aParms[4] = &aZero;                // no write list
    aParms[5] = &aZero;                // no exception list
    aParms[6] = &aZero;                // no exceotion list
    aParms[7] = (void *) CVIOS_HOBOFF(&theTimeout); // timeout
    aParms[8] = &theEcb;               // ECB
    aParms[9] = &aZero;                // format N/A
    aParms[10] = &aErr;                // error
    aParms[11] = &aErrNo;              // errno
    aParms[12] = (void *) CVIOS_HOBON(&aErrRsn);

    void  __register(1)  **aParmList = aParms;
    int   __register(14)  aR14;
    int   __register(15)  aRc;

__asm 
{

    L 15,16(0)                         CVT
    L 15,544(15)                       CSRTABLE
    L 15,24(15)                        SS CSR slots
    L 15,552(15)                       BPX1SEL
    CALL (15)                          CALL

}

};                                     // end SELECTEX

}                                      // End WaitEcb()

/***************************************************************************

    Function    : CviPost

    Description : Post ECB

    Parameters  : 1) ECB address

    Returns     : NONE

***************************************************************************/
void CviPost

(                                      // Begin Define Function Parameters

void  *theEcb

)                                      // End Define Function Parameters

{                                      // Begin CviPost()

__register (0)  long  r0;              // uses r0
__register (1)  long  r1;              // uses r1
__register (2)  void *aEcb = theEcb;
__register (14) long  r14;             // uses r14
__register (15) long  r15;             // uses r15


aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);


__asm

{

   POST (2)

}

}                                      // End CviPost()

/***************************************************************************

    Function    : CviSleep

    Description : Wait for x seconds

    Parameters  : 1) Time to wait in seconds
                  2) Optional ECB to wait upon
                  
    Return      : 0 - Time expired, no ECB
                  1 - ECB posted

***************************************************************************/
int CviSleep(int theTime, int *theEcb)
{
  __register(0) unsigned int r0;       // used by macro 
  __register(1) unsigned int r1;       // used by macro 
  __register(14) int* r14;             // used by macro 
  __register(15) unsigned int r15;     // used by macro 

  struct timeval aTimeout;             // timeout

  void  *aParms[16];                   // parms
  int   theZero = 0;                   // zero parm
  int   iTime = *(int *)(theTime);     // get actual time
  void *theTimeout = &aTimeout;        // timeout
  int   theErr = 0;                    // error code
  int   theErrNo = 0;                  // errno
  int   theErrRsn = 0;                 // error reason
  int   aRc = 0;                       // return code

  if (theEcb == NULL)                  // if no ECB

  {                                    // begin use stimer

      r14 = &iTime;                    // set timer addr 

      iTime = iTime * 100;             // convert into hundredths

      __asm                            // begin call STIMER
      {
        STIMER WAIT,BINTVL=(14)        
      }                                // end call STIMER

      aRc = 0;                         // time expired

  }

  else                                 // otherwise

  {                                    // begin SELECTEX

    theEcb = (int *)CVIOS_HOBOFF(theEcb);

    aTimeout.tv_sec = iTime;           // set timeout in seconds
    aTimeout.tv_usec = 0;              // no microseconds

    aParms[0] = &theZero;              // no sockets
    aParms[1] = &theZero;              // no read
    aParms[2] = &theZero;              // list
    aParms[3] = &theZero;              // no write
    aParms[4] = &theZero;              // list
    aParms[5] = &theZero;              // no state
    aParms[6] = &theZero;              // list
    aParms[7] = &theTimeout;           // timeout
    aParms[8] = &theEcb;               // ECB
    aParms[9] = &theZero;              // format doesn't matter
    
    aParms[10] = &theErr;                  // parm 11
    aParms[11] = &theErrNo;                // parm 12
    aParms[12] = (void *) CVIOS_HOBON(&theErrRsn);

    void  __register(1)  **aParmList = aParms;

    __asm {

        L 15,16(0)                     
        L 15,544(15)                   
        L 15,24(15)                    
        L 15,552(15)                   

        CALL (15)                      

    }                                  

    if (*theEcb != 0)                  // if ECB posted

        aRc = 1;                       // say so

    else                               // otherwise

        aRc = 0;                       // timeout

  };                                   // end SELECTEX

  return(aRc);                         // return result

}

/***************************************************************************

    Function    : CviHSleep

    Description : Wait for x hundreths of a second

    Parameters  : 1) Time to wait in hundredeths
                  2) Optional ECB to wait upon
                  
    Return      : 0 - Time expired, no ECB
                  1 - ECB posted

***************************************************************************/
int CviHSleep(int theTime)
{
  __register(0) unsigned int r0;       // used by macro 
  __register(1) unsigned int r1;       // used by macro 
  __register(14) int* r14;             // used by macro 
  __register(15) unsigned int r15;     // used by macro 

  int   iTime = *(int *)(theTime);     // get actual time
  int   aRc = 0;                       // return code

  r14 = &iTime;                        // set timer addr 
                                       
  __asm                                // begin call STIMER
  {                                    
    STIMER WAIT,BINTVL=(14)            
  }                                    // end call STIMER
                                       
  aRc = 0;                             // time expired


  return(aRc);                         // return result

}

/***************************************************************************

    Function    : CviGetMvsName

    Description : Get MVS name
    
    Parameters  : 9-bytes for name

***************************************************************************/
void CviGetMvsName(char *theName)
{

struct psa *psPsa = 0;                 // PSA

struct cvt *psCvt =                    // get CVT 
(struct cvt *)psPsa->flccvt;   

memcpy(theName,                        // copy MVS name into place 
       &psCvt->cvtsname,
       8);           

theName[8] = 0;                        // terminate

char    *thePtr = theName + 7;         // grab last character

while (thePtr != theName &&
       *thePtr == ' ')                 // while space

{                                      // begin remove space

    *thePtr = 0;                       // clear it

    thePtr --;                         // keep going

};                                     // end remove space

}

/***************************************************************************

    Function    : CviOsCreateNT

    Description : Set name token

    Parameters  : 1) Token name
                  2) Token value
                  3) Level (default - SYSTEM)

    Returns     : NONE

***************************************************************************/

int CviOsCreateNt

(                                      // Begin Define Function Parameters

const char  * theName,                 // token name
int64_t *theValue,                     // token value
int     theLevel                       // level

)                                      // End Define Function Parameters

{                                      // Begin CviOsCreateNT()

long  r_c = 0;                         // OK so far


typedef int (* __ptr31 IeaNtCr) (void);
__register(7) void * aOldR1;
__register(1) void * aR1 = (void *) &theName;
__register (15) IeaNtCr aSvc = 0;

#pragma pack(4)
struct SvcParms
{
   const int  * __ptr31 itsLevel;
         char * __ptr31 itsName;
    int64_t   * __ptr31 itsValue;
         int  * __ptr31 itsOptions;
         int  * __ptr31 itsRc;

    int  theLevel;
    char theName[16];
    int64_t theValue;
    int  theOptions;
    int  theRc;
};

struct SvcParms aParm;

aParm.theLevel = *(int *) theLevel;    // level to use
aParm.theValue = *theValue;            // value to set
aParm.theOptions = 0;                  

memcpy(aParm.theName, theName, 16);
aParm.itsLevel = (const int * __ptr31) &aParm.theLevel;
aParm.itsName = (char * __ptr31) &aParm.theName;
aParm.itsOptions = (int * __ptr31) &aParm.theOptions;
aParm.itsValue = (long long * __ptr31) &aParm.theValue;
aParm.itsRc = (int * __ptr31) &aParm.theRc;


aOldR1 = aR1;

aR1 = (void *) &aParm;

#ifdef _LP64
__asm
{
   sam31
}
#endif

__asm
{
   L     15,16(0,0)                    // Get the CVT
   L     15,544(0,15)                  // then
   L     15,20(0,15)                   // get the routine
   L     15,4(0,15)                    // address

   BALR 14,15

}

//aSvc();


#ifdef _LP64
__asm
{
   sam64
}
#endif

aR1 = aOldR1;

r_c = aParm.theRc;

#pragma pack(reset)



return(r_c);

}                                      // CviOsCreateNT()

/***************************************************************************

    Function    : CviOsGetNT

    Description : Get token. 

    Parameters  : 1) Token name
                  2) Token value
                  3) Level

    Returns     : NONE

***************************************************************************/

int CviOsGetNt

(                                      // Begin Define Function Parameters

const char  * theName,                 // token name
int64_t *theValue,                     // token value
int     theLevel                       // level

)                                      // End Define Function Parameters

{                                      // Begin CviOsGetNT()

long  r_c = 0;                         // OK so far

typedef int (* __ptr31 IeaNtRt) (void);

__register(8) void * aOldR1;
__register(1) void * aR1;
__register(15) IeaNtRt aSvc = 0;

#pragma pack(4)
struct SvcParms
{
   const int  * __ptr31 itsLevel;
         char * __ptr31 itsName;
    int64_t   * __ptr31 itsValue;
         int  * __ptr31 itsRc;

    int  theLevel;
    char theName[16];
    int64_t theValue;
    int  theRc;
};

typedef struct SvcParms SvcParms;

SvcParms *aParm;                       // parameter pointer
SvcParms aParmBlk;                     // parameter block
#ifdef _LP64
if ((unsigned long) &aParmBlk >        // if
      0xffffffff)                      // memory is above bar
   aParm = (SvcParms *) __malloc31(sizeof(SvcParms));
else                                   // otherwise
   aParm = &aParmBlk;
#else
   aParm = &aParmBlk;
#endif

aParm->theLevel = *(int *) theLevel;    // level to use

memcpy(aParm->theName, theName, 16);
aParm->itsLevel = (const int * __ptr31) &aParm->theLevel;
aParm->itsName = (char * __ptr31) &aParm->theName;
aParm->itsValue = (long long * __ptr31) &aParm->theValue;
aParm->itsRc = (int * __ptr31) &aParm->theRc;

aR1 = (void *) &theName;

aOldR1 = aR1;
aR1 = (void *) aParm;

aSvc = 0;

#ifdef _LP64
__asm
{
   sam31
}
#endif

__asm
{
   L     15,16(0,0)                    // Get the CVT
   L     15,544(0,15)                  // then
   L     15,20(0,15)                   // get the routine
   L     15,8(0,15)                    // address
   BALR 14,15
}

//aSvc();

#ifdef _LP64
__asm
{
   sam64
}
#endif

aR1 = aOldR1;

*theValue = aParm->theValue;

r_c = aParm->theRc;

#pragma pack(reset)


#ifdef _LP64
if ((unsigned long) &aParmBlk >        // if
      0xffffffff)                      // memory is above bar
   __free31((void *) aParm);
#endif


return(r_c);                           // return our result

}                                      // End CviOsGetNT()

/***************************************************************************

    Function    : CviOsDeleteNT

    Description : Delete token.  This is NOT intended for public consumption.

    Parameters  : 1) Token name
                  2) Level

    Returns     : NONE

***************************************************************************/
int  CviOsDeleteNt

(                                      // Begin Define Function Parameters

const char  *theName,                  // token name
int     theLevel                       // level

)                                      // End Define Function Parameters

{                                      // Begin CviOsDeleteNT()

long  r_c = 0;                         // OK so far

typedef int (* __ptr31 IeaNtRt) (void);

__register(8) void * aOldR1;
__register(1) void * aR1;
__register(15) IeaNtRt aSvc = 0;


#pragma pack(4)
struct SvcParms
{
   const int  * __ptr31 itsLevel;
         char * __ptr31 itsName;
         int  * __ptr31 itsRc;

    int  theLevel;
    char theName[16];
    int  theRc;
};

struct SvcParms aParm;

aParm.theLevel = *(int *) theLevel;    // level to use

memcpy(aParm.theName, theName, 16);

aParm.itsLevel = (const int * __ptr31) &aParm.theLevel;
aParm.itsName = (char * __ptr31) &aParm.theName;
aParm.itsRc = (int * __ptr31) &aParm.theRc;

aR1 = (void *) &theName;

aOldR1 = aR1;
aR1 = (void *) &aParm;

#ifdef _LP64
__asm
{
   sam31
}
#endif

__asm
{
   L     15,16(0,0)                    // Get the CVT
   L     15,544(0,15)                  // then
   L     15,20(0,15)                   // get the routine
   L     15,12(0,15)                   // address
   BALR 14,15
}

//aSvc();


#ifdef _LP64
__asm
{
   sam64
}
#endif

aR1 = aOldR1;

r_c = aParm.theRc;

#pragma pack(reset)

return(r_c);                           // return our result

}                                      // End CviOsDeleteNT()

/***************************************************************************

    Function    : CviOsSetCdeName

    Description : Sets CDENAME

    Parameters  : 1) Name to use

***************************************************************************/
void CviOsSetCdeName(const char *theName)

{


struct psa *psPsa = 0;                 // PSA

int ourTcb  = (int) psPsa->psatold;    // get TCB address

tcbfix *aTcb = (tcbfix *)              // get TCB prefix address 
    ((char *) psPsa->psatold - 32);    

if ((int) ourTcb !=                    // if we are NOT
     (int) aTcb->tcbjstcb)             // the JSTCB

    return;                            // get out of here!


__register(0)  void* r0;               // entry point addr
__register(1)  int r1;                 // reason code
__register(14) const char* r14;        // ptr to module name
__register(15) int r15;                // return code

__register(5)  const char *r5;         // R5 - name

char    aTaskName[9];                  // task name    
                                       
sprintf(aTaskName,                     // set task name
        "%-8.8s",                      // to   
        CVIOS_HOBOFF(theName));        // command name

r5 = aTaskName;                        // grab task name
                                       

__register(6)  int r6;                 // R6 will be used

__asm {

*
* FIND THE PRB FOR OUR TASK, THEN FIND OUR CDE
*
         USING PSA,0                   MAP PSA
         L     6,PSATOLD               GET CURRENT TCB
         USING TCB,6                   MAP TCB
         L     6,TCBRBP                GET ADDRESS OF LAST RB
         DROP  6                       NO MORE NEED OF TCB
         SH    6,=Y(RBBASIC-RBPRFX)    POINT TO PREFIX
         USING RBPRFX,6                MAP RB PREFIX
         SPACE 1
RBLOOP1  DS    0H
         SPACE 1
         CLC   RBLINKB,PSATOLD+1       TOP RB?
         BE    TOPRB1                  YES - BRANCH
         L     6,RBLINK                NO - GET PREVIOUS RB
         SLL   6,8                     CLEAR HIGH ORDER BYTE
         SRL   6,8
         SH    6,=Y(RBBASIC-RBPRFX)    POINT TO PREFIX
         B     RBLOOP1                 FIND TOP RB
         SPACE 1
TOPRB1   DS    0H
         SPACE 1
         L     6,RBCDE                 GET ADDRESS OF DBC2MAIN CDE
         DROP  6                       NO MORE NEED OF RB
*
* REPLACE THE MODULE NAME IN OUR CDE WITH THE NAME OF THE FUNCTION
* OR MODULE TO BE GIVEN CONTROL
*
         USING CDENTRY,6               MAP CDE
         SPACE 1
         MODESET KEY=ZERO              TO MODIFY CDE
         SPACE 1
         MVC   CDNAME,0(5)             SET TARGET NAME
         DROP  6                       NO MORE NEED OF CDE
         SPACE 1
         MODESET KEY=NZERO             BACK TO NORMAL
         SPACE 1
                                       
}

}

//****************************************************************************
// CviOsStckTime - Convert STCK to microseconds
//****************************************************************************
uint64_t CviOsStckTime(uint64_t *theTime)
{

  __register(0)  void* r0;             // entry point addr
  __register(1)  int r1;               // reason code
  __register(2)  uint64_t *aTime;      // STCK address
  __register(3)  unsigned char *aStr;  // string output
  __register(10) char *r10;            // parms
  __register(14) const char* r14;      // ptr to module name
  __register(15) int r15;              // return code


  char  ParmBlk[64];                   // parm block

  unsigned char aBuf[16];              // output buffer

  aTime = theTime;                     // set time into register
  aStr = aBuf;                         // set microsecond return to register
  r10 = ParmBlk;                       // parm block

  __asm {
    STCKCONV STCKVAL=(2),CONVVAL=(3),TIMETYPE=BIN,                     x
               DATETYPE=YYYYMMDD,MF=(E,(10))
  }

  unsigned int *aVal = (unsigned int *) aStr;

  return((uint64_t) *aVal);

}


/***************************************************************************

    Function    : CviOsTime

    Description : Gets current time in seconds

    Parameters  : NONE

    Returns     : Current time in seconds

***************************************************************************/
int64_t  CviOsTime()
{

return(time(NULL));                    // return current time

}

__asm {

    SPACE 1
    IHAPSA
    SPACE 1
    IKJTCB
    SPACE 1
    IHACDE
    SPACE 1
    IHARB

}
