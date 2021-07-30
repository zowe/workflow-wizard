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
                                       
    if (!memcmp(aTiot->tioeddnm,       // if DD name matches
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

void  *aModule;                        // use R8 for task entry point
void  *aEcb = theEcb;                  // ECB

void *aPtr = NULL;                     // returned TCB address
void **aPtrReg = &aPtr;                // pointer for register access

char    AttachParms[1024];             // attach parms

char *aAP = AttachParms;

aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);

memset(AttachParms, 0, sizeof(AttachParms));

char aEp[9];

static int aCount = 0;

sprintf(aEp, "CVIEP%03d", aCount ++);

aModule = aEp;

__asm__("   IDENTIFY EPLOC=(%1),ENTRY=(%0)\n"
        "   LR %0,15\n"
        :
        :"r"(theCall),
         "r"(aModule)
        :"r0","r1","r14","r15");

__asm__("   LR 1,%4\n"
        "   ATTACHX EPLOC=(%2),ECB=(%1),SF=(E,(%3))\n"
        "   LR %0,1\n"
        :"+r"(aPtr)
        :"r"(aEcb),
         "r"(aModule),
         "r"(aAP),
         "r"(theParms)
        :"r0","r1","r14","r15");

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

void  *aModule;                        // use R8 for task entry point
void  *aEcb = theEcb;                  // ECB

void *aPtr = NULL;                     // returned TCB address
void **aPtrReg = &aPtr;                // pointer for register access

char    AttachParms[1024];             // attach parms

char *aAP = AttachParms;

aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);

memset(AttachParms, 0, sizeof(AttachParms));

char aEp[9];

static int aCount = 0;

sprintf(aEp, "CVIEP%03d", aCount ++);

aModule = aEp;

__asm__("   IDENTIFY EPLOC=(%1),ENTRY=(%0)\n"
        "   LR %0,15\n"
        :
        :"r"(theCall),
         "r"(aModule)
        :"r0","r1","r14","r15");

__asm__("   LR 1,%4\n"
        "   ATTACHX EPLOC=(%2),ECB=(%1),JSTCB=YES,SF=(E,(%3))\n"
        "   LR %0,1\n"
        :"+r"(aPtr)
        :"r"(aEcb),
         "r"(aModule),
         "r"(aAP),
         "r"(theParms)
        :"r0","r1","r14","r15");

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

   unsigned int   *aTcb;               // TCB address

   unsigned int aTcbInt;               // TCB


   aTcbInt = (unsigned int) theTcb;    // set into int
   aTcbInt = aTcbInt & 0x7fffffff;     // turn off high bit

   aTcb = &aTcbInt;                    // set TCB 


   __asm__("   DETACH (%0),STAE=NO\n"
        :
        :"r"(aTcb)
        :"r0","r1","r14","r15");

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

void *aEcb = theEcb;


aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);


if (theTimeout == NULL)                // if no timeout

{                                      // begin WAIT


   __asm__("   WAIT ECB=(%0)\n"
           :
           :"r"(aEcb)
           :"r0","r1","r14","r15");

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

    void **aParmList = aParms;

    __asm__(
"    LR 1,%0\n"
"    L 15,16(0)                         CVT\n"
"    L 15,544(15)                       CSRTABLE\n"
"    L 15,24(15)                        SS CSR slots\n"
"    L 15,552(15)                       BPX1SEL\n"
"    CALL (15)                          CALL\n"
    :
    :"r"(aParmList)
    :"r0","r1","r14","r15");

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

void *aEcb = theEcb;                   


aEcb = (void *)(((unsigned int )theEcb) & 0x7fffffff);


  __asm__("   POST (%0)\n"
          :
          :"r"(aEcb)
          :"r0","r1","r14","r15");

}                                      // End CviPost()

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

    Function    : CviHSleep

    Description : Wait for x hundreths of a second

    Parameters  : 1) Time to wait in hundredeths
                  2) Optional ECB to wait upon
                  
    Return      : 0 - Time expired, no ECB
                  1 - ECB posted

***************************************************************************/
int CviHSleep(int theTime)
{

  int   iTime = *(int *)(theTime);     // get actual time
  int   aRc = 0;                       // return code

  int   *aTimePtr = &iTime;            // set timer addr 
                                       
  __asm__("  STIMER WAIT,BINTVL=(14)\n"
          :
          :"r"(aTimePtr)
          :"r0","r1","r14","r15");
            
  aRc = 0;                             // time expired


  return(aRc);                         // return result

}

