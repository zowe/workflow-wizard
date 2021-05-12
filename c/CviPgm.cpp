/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Base class for all 'program' objects.
// 
//****************************************************************************

//****************************************************************************
//                              Includes

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "CviPgm.h"                    // ourself
#include "CviOs.h"                     // OS calls
#include "CviDyna.h"                   // dynamic allocation

#include "btype.h"                     // types for WTO
#include "wto.h"                       // include ARH WTO

#include "ihapsa.h"                    // PSA
#include "ihaascb.h"                   // ASCB
#include "ihaasxb.h"                   // ASXB
#include "ihaacee.h"                   // ACEE
#include "cvt.h"                       // CVT
#include "ieftiot1.h"                  // TIOT
#include "ikjtcb.h"                    // TCB
#include "iezjscb.h"                   // JSCB
#include "iefjssib.h"                  // SSIB

//
//****************************************************************************

//****************************************************************************
//                              Defines

#define FOPENMODE "a,recfm=fbm,lrecl=133,blksize=133"

struct  TraceInfo
{
    char itsEye[8];                    // eye catcher
    char itsTrcOn;                     // trace on?
    char itsTrcDisabled;               // trace disabled entirely?
    int  itsAllocTcb;                  // allocating TCB
    CviPgm *itsAllocPgm;               // allocating CviPgm object
    
    int  itsTrcLen;                    // current trace length
    int  itsTrcUse;                    // trace in-use

    char itsTrcBuf[0x40000];           // big trace buffer
};

static TraceInfo *aTrcInfo;            // trace information
             
//
//****************************************************************************

//****************************************************************************
//                           CVI-specific prodinfo

struct  CviProdInfo                    // CviProfInfo
{                                      // begin CviProdInfo
    char itsStandard[0x40];            // standard data (ProdInfo)
    short itsBundle;                   // bundle
};                                     // end CviProdInfo

//
//****************************************************************************

//****************************************************************************
//
// Method       : CviPgm Constructor
//
// Parameters   : Name, theArgc, theArgv
//
//****************************************************************************
CviPgm::CviPgm(const char *theName,
               int theArgc,
               const char **theArgv)

{

int64_t aPgmObj = (int64_t) this;      // store our token


itsMainForTask = FALSE;                // not the main pgm object for this task

if (strcmp(theName,"EAGENT") != 0   && // if not EAGENT (special pgm) and
    GetTaskCviPgm() == NULL)           // if main pgm not yet claimed

{                                      // begin claim to be main pgm object for task

    CviOsCreateNt("ZCVIPGMOBJECTTCB",  // set our pgm object for
                  &aPgmObj,            // later use
                  NAME_TASK);          // at task level

    itsMainForTask = TRUE;             // we are the main one now

};                                     // end claim to be main pgm object for task

strcpy(itsStDateStr, GetDate());       // get date we are starting
strcpy(itsStTimeStr, GetTime());       // get time we are starting

SetJobInfo();                          // setup job information

sprintf(itsEye,                        // set eye-catcher
        "CVIPGM_%-.8s",                
        theName);

itsStdOut = NULL;                      // clear stdout handle
itsStdErr = NULL;                      // clear stderr handle
itsTrcOut = NULL;                      // clear trace handle

itsCvtOff = NULL;                      // clear conversion offset

itsArgc = theArgc;                     // save argument count

itsArgv = theArgv;                     // save arguments

if (strlen(theName) >                  // if string
        sizeof(itsName)-1)             // too large  

{                                      // begin short copy

    memcpy(itsName,                    // copy program name
           theName,                    // into place
           sizeof(itsName)-1);

    itsName[sizeof(itsName)-1] = 0;    // terminate

}                                      // end short copy

else                                   // otherwise

{                                      // begin copy string

    strcpy(itsName, theName);          // copy name

};                                     // end copy string

int64_t aTrcBuf;                       // trace buffer

if (CviOsGetNt("ZCVITRCBUFFER   ",     // get trace buffer
               &aTrcBuf,
               NAME_HOME) != 0)

{                                      // begin create traceflag

    aTrcBuf = (int64_t) malloc(sizeof(TraceInfo)); // get master flag

    memset((char *) (int) aTrcBuf,     // clear it
           0,
           sizeof(TraceInfo));

    CviOsCreateNt("ZCVITRCBUFFER   ",  // set trace buffer for future use
                  &aTrcBuf,        
                  NAME_HOME);

    aTrcInfo = (TraceInfo *) (int) aTrcBuf;// grab trace information struct

    memcpy(aTrcInfo->itsEye,           // set the
           "CVITRCBF",                 // eye-catcher
           8);

    aTrcInfo->itsTrcOn = TRUE;         // assume trace should be on
                                       // it'll turn off if CVITRACE not allocated.

    aTrcInfo->itsAllocTcb = itsTcb;    // store our TCB

    aTrcInfo->itsTrcDisabled = FALSE;  // trace NOT disabled

    aTrcInfo->itsAllocPgm = this;      // we allocated it

}                                      // end create trace flag

else                                   // otherwise

   aTrcInfo =                          // grab trace information struct 
     (TraceInfo *) (int) aTrcBuf;

}

//****************************************************************************
//
// Method       : CviPgm Constructor
//
// Parameters   : Name
//
//****************************************************************************
CviPgm::CviPgm(const char *theName)

{

int64_t aPgmObj = (int64_t) this;      // store our token

itsMainForTask = FALSE;                // not the main pgm object for this task

if (CviOsGetNt("ZCVIPGMOBJEC2TCB",     // if no program object
               &aPgmObj,               // defined for
               NAME_TASK) != 0)        // "type 2" program object

{                                      // begin claim to be main pgm object for task

    aPgmObj = (int64_t) this;          // store our token

    CviOsCreateNt("ZCVIPGMOBJEC2TCB",  // set our pgm object for
                  &aPgmObj,            // later use
                  NAME_TASK);          // at task level

    itsMainForTask = TRUE;             // we are the main one now

};                                     // end claim to be main pgm object for task

strcpy(itsStDateStr, GetDate());       // get date we are starting
strcpy(itsStTimeStr, GetTime());       // get time we are starting

SetJobInfo();                          // setup job information

sprintf(itsEye,                        // set eye-catcher
        "CVIPGM_%-.8s",                
        theName);

itsStdOut = NULL;                      // clear stdout handle
itsStdErr = NULL;                      // clear stderr handle
itsTrcOut = NULL;                      // clear trace handle

itsCvtOff = NULL;                      // clear conversion offset

itsArgc = 0;                           // no arguments

itsArgv = NULL;                        // no arguments

if (strlen(theName) >                  // if string
        sizeof(itsName)-1)             // too large  

{                                      // begin short copy

    memcpy(itsName,                    // copy program name
           theName,                    // into place
           sizeof(itsName)-1);

    itsName[sizeof(itsName)-1] = 0;    // terminate

}                                      // end short copy

else                                   // otherwise

{                                      // begin copy string

    strcpy(itsName, theName);          // copy name

};                                     // end copy string

aTrcInfo = (TraceInfo *)               // allocate
            malloc(sizeof(TraceInfo)); // trace buffer

memset((char *) aTrcInfo,              // clear it
       0,
       sizeof(TraceInfo));

memcpy(aTrcInfo->itsEye,               // set the
       "CVITRCBF",                     // eye-catcher
       8);                             

aTrcInfo->itsTrcOn = TRUE;             // assume trace should be on
                                       // it'll turn off if CVITRACE not allocated.

aTrcInfo->itsTrcDisabled = FALSE;      // trace NOT disabled

aTrcInfo->itsAllocTcb = itsTcb;        // store our TCB

aTrcInfo->itsAllocPgm = this;          // we allocated it

}

//****************************************************************************
//
// Method       : GetTaskCviPgm
//
// Description  : Gets main CviPgm object for this task
//
//****************************************************************************
CviPgm *GetTaskCviPgm()

{

static CviPgm *aCviPgm = NULL;         // initialize to NULL at C environment start

if (aCviPgm != NULL) return aCviPgm;


int64_t aPgmObj = 0;                   // no object yet

if (CviOsGetNt("ZCVIPGMOBJEC2TCB",     // set our pgm object for
               &aPgmObj,               // later use
               NAME_TASK) != 0)        // at task level

    aPgmObj = 0;                       // no object if failed

aCviPgm = (CviPgm *) (int32_t) aPgmObj;

if (aCviPgm == NULL)                   // if CVIAPI PGM not found

{                                      // begin normal CviPgm

    if (CviOsGetNt("ZCVIPGMOBJECTTCB", // set our pgm object for
                   &aPgmObj,           // later use
                   NAME_TASK) == 0)    // at task level

        aCviPgm = (CviPgm *) (int32_t) aPgmObj;

};                                     // end normal CviPgm

return(aCviPgm);                       // return object

}

//****************************************************************************
//
// Method       : SetJobInfo
//
// Description  : Sets information about the job environment
//
//****************************************************************************
void CviPgm::SetJobInfo()

{

struct psa *psPsa = 0;                 // PSA

struct ascb *psAscb =                  // set ASCB address 
   (struct ascb *) psPsa->psaaold;  

struct asxb *psAsxb =                  // grab ASXB
   (struct asxb *) psAscb->ascbasxb;   

struct acee *psAcee =                  // grab ACEE
   (struct acee *) psAsxb->asxbsenv;

CviGetMvsName(itsMvsName);             // get LPAR name

itsTcb  = (int) psPsa->psatold;        // get TCB address
itsAsid = (int) psAscb->ascbasid;      // get ASID

tcbfix *aTcb = (tcbfix *)              // get TCB prefix address 
    ((char *) psPsa->psatold - 32);    

tiot   *aTiot = (tiot *) aTcb->tcbtio; // get TIOT

sprintf(itsJobName,                    // store
        "%-.8s",                       // job
        aTiot->tiocnjob);              // name

sprintf(itsJobStep,                    // set job step
        "%-.8s",
        aTiot->tiocstpn);

if (itsJobStep[0] == ' ')              // if nothing there

    sprintf(itsJobStep,                // set job step with proc jobstep
            "%-.8s",
            aTiot->tiocjstn);          

iezjscb *aScb = (iezjscb *) aTcb->tcbjscb; // get SCB
ssib    *aSsib = (ssib *) aScb->jscbssib;  // get SSIB

sprintf(itsJobId,                      // store
        "%-.8s",                       // job
        aSsib->ssibjbid);              // ID

sprintf(itsUser,                       // store user
        "%-.8s",
        psAcee->aceeusri);

Replace(itsUser, " ", "");             // remove space
Replace(itsJobName, " ", "");          // remove spaces
Replace(itsJobId, " ", "");            // remove spaces
Replace(itsJobStep, " ", "");          // remove spaces

}

//****************************************************************************
//
// Method       : ToDecStr
//
// Description  : Convert integer to decimal string
//
// Parameters   : 1) Integer
//
// NOTES        : String is 'guaranteed' good until To*Str calls
//                have had to wrap back around in the temp buffer.
//                This should be for results that are short-term
//                in scope only!  About 8 calls will cause it to
//                wrap.
//
//****************************************************************************
const char *CviPgm::ToDecStr(int theNum)
{

char *aBuf;                            // buffer

if (itsCvtOff + 16 > sizeof(itsCvtBuf))// if too large

    itsCvtOff = 0;                     // loop back to start

aBuf = itsCvtBuf + itsCvtOff;          // set buffer

sprintf(aBuf,                          // convert
        "%d",                          // to 
        theNum);                       // string

itsCvtOff += strlen(aBuf) + 1;         // advance offset into cvt buffer

return(aBuf);                          // return string

}

//****************************************************************************
//
// Method       : ToHexStr
//
// Description  : Convert integer to hex string
//
// Parameters   : 1) Integer
//
// NOTES        : String is 'guaranteed' good until To*Str calls
//                have had to wrap back around in the temp buffer.
//                This should be for results that are short-term
//                in scope only!  About 8 calls will cause it to
//                wrap.
//
//****************************************************************************
const char *CviPgm::ToHexStr(int theNum)
{

char *aBuf;                            // buffer

if (itsCvtOff + 16 > sizeof(itsCvtBuf))// if too large

    itsCvtOff = 0;                     // loop back to start

aBuf = itsCvtBuf + itsCvtOff;          // set buffer

sprintf(aBuf,                          // convert
        "%p",                          // to 
        theNum);                       // string

itsCvtOff += strlen(aBuf) + 1;         // advance offset into cvt buffer

return(aBuf);                          // return string

}

//****************************************************************************
//
// Method       : Init
//
// Description  : Initialize program
//
// Parameters   : None
//
//****************************************************************************
int CviPgm::Init(void)
{


return(0);                             // we are OK

}

//****************************************************************************
//
// Method       : Work
//
// Description  : Perform work
//
// Parameters   : None
//
//****************************************************************************
int CviPgm::Work(void)
{

return(0);                             // we are OK

}

//****************************************************************************
//
// Method       : Run
//
// Description  : Runs the program.  Performs init, then performs work.
//
// Parameters   : None
//
//****************************************************************************
int CviPgm::Run(void)
{

int aRc = 0;                           // return code

aRc = Init();                          // initialize

if (aRc == 0)                          // if initialized OK

    aRc = Work();                      // do the work

return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : GetName
//
// Description  : Get task name
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetName()
{

return(itsName);                       // return task name

}

//****************************************************************************
//
// Method       : GetJobName
//
// Description  : Get job name
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetJobName()
{

return(itsJobName);                    // return job name

}

//****************************************************************************
//
// Method       : GetUser
//
// Description  : Get user ID
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetUser()
{

return(itsUser);                       // return user ID

}

//****************************************************************************
//
// Method       : GetStartDate
//
// Description  : Get start date as string
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetStartDate()
{

return(itsStDateStr);                  // return start date string

}

//****************************************************************************
//
// Method       : GetStartTime
//
// Description  : Get start time as string
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetStartTime()
{

return(itsStTimeStr);                  // return start time string

}

//****************************************************************************
//
// Method       : GetTime
//
// Description  : Get current time
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetTime()
{

__register(0)  int aR0;                // time
__register(1)  int aR1;                // R1
__register(3)  char *aR3;              // R3
__register(4)  char *aR4;              // R4
__register(14) int aR14;               // R14
__register(15) int aR15;               // R15

char    aTime[16];                     // 16-byte return
char    aParms[32];                    // parm block

aR4 = aParms;                          // set parm area

aR3 = aTime;                           // set return address

__asm {                                // get
     TIME DEC,(3),ZONE=LT,LINKAGE=SYSTEM,DATETYPE=MMDDYYYY,MF=(E,(4))
}                                      // time

sprintf(itsTimeStr,                    // set time into place
        "%02.2x:%02.2x:%02.2x.%02.2x",
        (unsigned int) aTime[0],
        (unsigned int) aTime[1],
        (unsigned int) aTime[2],
        (unsigned int) aTime[3]);

return(itsTimeStr);                    // return timestamp string

}

//****************************************************************************
//
// Method       : GetDate
//
// Description  : Get current date
//
// Parameters   : None
//
//****************************************************************************
const char *CviPgm::GetDate()
{

__register(0)  int aR0;                // time
__register(1)  int aR1;                // R1
__register(3)  char *aR3;              // R3
__register(4)  char *aR4;              // R4
__register(14) int aR14;               // R14
__register(15) int aR15;               // R15

char    aDate[16];                     // 16-byte return
char    aParms[32];                    // parm block

aR4 = aParms;                          // set parm area

aR3 = aDate;                           // set return address

__asm {                                // get
     TIME DEC,(3),ZONE=LT,LINKAGE=SYSTEM,DATETYPE=MMDDYYYY,MF=(E,(4))
}                                      // time

sprintf(itsDateStr,                    // set date into place
        "%02.2x/%02.2x/%02.2x%02.2x",
        (unsigned int) aDate[8],
        (unsigned int) aDate[9],
        (unsigned int) aDate[10],
        (unsigned int) aDate[11]);

return(itsDateStr);                    // return date string

}

//****************************************************************************
//
// Method       : PrintOutput
//
// Description  : Actually print the output
//
// Parameters   : None
//
//****************************************************************************
int CviPgm::PrintOutput(const char *theStr)
{

int     aRc = 0;                       // return code


CviPgm *aPgm = this;                   // assume we will use ourself

struct psa *psPsa = 0;                 // PSA
int    aTcb  = (int) psPsa->psatold;   // get TCB address

if (aPgm == NULL ||                    // if no pgm object is known or
    aTcb != itsTcb)                    // or if this is the wrong program object...

{                                      // begin grab correct CviPgm

    aPgm = GetTaskCviPgm();            // get correct pgm object

    if (aPgm == NULL)                  // if not good

        return(16);                    // fail

    else                               // otherwise

        return(aPgm->PrintOutput(theStr));

};                                     // end grab correct CviPgm

if (itsStdOut == NULL)                 // if not open and print
                                       
{                                      // begin open

    AllocDd("CVIPRINT");               // allocate CVIPRINT
    
    itsStdOut = fopen("//DDN:CVIPRINT",// attempt to open
                      FOPENMODE);

    if (itsStdOut != NULL)             // if OK

        setlinebuf(itsStdOut);         // line buffer
                                       
};                                     // end open

if (itsStdOut != NULL)                 // if open

{                                      // begin print

    if (theStr[strlen(theStr)-1] != '\n')

       fprintf(itsStdOut,
               "%s\n",
               theStr);

    else

       fprintf(itsStdOut,              // write out
               "%s",                   // line     
               theStr);                

}                                      // end print

else                                   // otherwise

    aRc = 8;                           // failure


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : PrintLines
//
// Description  : Dump lines out
//
//****************************************************************************
void CviPgm::PrintLines(const char *theMsg)

{

CviStr  aTmpMsg(theMsg);               // message


const char *aPtr = aTmpMsg.Tok("\n");  // first line 

while (aPtr != NULL)                   // for all lines

{                                      // begin display line

    Print("%s\n", aPtr);               // display line

    aPtr = aTmpMsg.Tok("\n");          // get next line

};                                     // end display line

}

//****************************************************************************
//
// Method       : Replace
//
// Description  : Replace all tokens in string with value
//
// Parameters   : 1) String
//                2) Token
//                3) Value
//
//****************************************************************************
void CviPgm::Replace(char *theMsg,
                     const char *theTok,
                     const char *theValue,
                     int theMaxLen)
{

char *aPtr = strstr(theMsg,            // find
                    theTok);           // token

while (aPtr != NULL                 && // while token found
       (theMaxLen < 0               || // and max length passed and
        strlen(theMsg) + strlen(theValue) - strlen(theTok) + 1 <=
        theMaxLen))

{                                      // begin replace token

    memmove(aPtr + strlen(theValue),   // shift data to leave room
            aPtr + strlen(theTok),     // for value
            strlen(aPtr + strlen(theTok))+1);

    memcpy(aPtr,                       // copy value into place
           theValue,
           strlen(theValue));

    aPtr += strlen(theValue);          // advance past token

    aPtr = strstr(aPtr, theTok);       // find next token

};                                     // end replace token

}

//****************************************************************************
//
// Method       : Print
//
// Description  : Print output
//
// Parameters   : printf parms
//
//****************************************************************************
void CviPgm::Print(const char *theStr, ...)
{

CviStr  aFmtStr;                       // output string
CviStr  aOutStr;                       // output string

char    aPrefix[64];                   // string prefix

va_list aArgList;                      // variable argument list

int     aPrefixLen = 0;                // prefix length


sprintf(aPrefix,                       // set time into place
        "%-8.8s --> ",         
        GetTime());

aPrefixLen = strlen(aPrefix);          // get length of prefix

va_start(aArgList, theStr);            // get arguments

aFmtStr.VsnPrint                       // add
         (theStr,                      // desired data
          aArgList);

Beautify(aFmtStr, 132-aPrefixLen);     // beautify message

va_end(aArgList);                      // no longer need list

const char *aTok = aFmtStr.Tok("\n");  // search for newlines

while (aTok != NULL)                   // for all lines

{                                      // begin write line

    aOutStr.Reset();                   // reset output

    aOutStr.Print("%s%s\n",            // setup
                 aPrefix,              // output
                 aTok);                // line

    PrintOutput((const char *) aOutStr);// display output
                                       
    Trace("%s",
          (const char *) aOutStr +     // trace 
          aPrefixLen);        

    aTok = aFmtStr.Tok("\n");          // get next line

};                                     // end write line

}

//****************************************************************************
//
// Method       : Error
//
// Description  : Print error output
//
// Parameters   : printf parms
//
//****************************************************************************
void CviPgm::Error(const char *theStr, ...)
{

CviStr  aFmtStr;                       // output string
CviStr  aOutStr;                       // output string

char    aPrefix[64];                   // string prefix

va_list aArgList;                      // variable argument list

int     aPrefixLen = 0;                // prefix length


sprintf(aPrefix,                       // set time into place
        "%-8.8s !!! ERROR: ",
        GetTime());

aPrefixLen = strlen(aPrefix);          // get length of prefix

va_start(aArgList, theStr);            // get arguments

aFmtStr.VsnPrint                       // add
         (theStr,                      // desired data
          aArgList);

Beautify(aFmtStr, 132-aPrefixLen);     // beautify message

va_end(aArgList);                      // no longer need list

const char *aTok = aFmtStr.Tok("\n");  // search for newlines

while (aTok != NULL)                   // for all lines

{                                      // begin write line

    aOutStr.Reset();                   // reset output

    aOutStr.Print("%s%s\n",            // setup
                 aPrefix,              // output
                 aTok);                // line

    PrintOutput((const char *) aOutStr);// display output
                                       
    Trace("%s",
          (const char *) aOutStr +     // trace 
          aPrefixLen);        

    aTok = aFmtStr.Tok("\n");          // get next line

};                                     // end write line

}

//****************************************************************************
//
// Method       : DevPrint
//
// Description  : Print output to dev output
//
// Parameters   : printf parms
//
//****************************************************************************
void CviPgm::DevPrint(const char *theStr, ...)
{

CviStr  aFmtStr;                       // output string
CviStr  aOutStr;                       // output string

char    aPrefix[64];                   // string prefix

va_list aArgList;                      // variable argument list

int     aPrefixLen = 0;                // prefix length


sprintf(aPrefix,                       // set time into place
        "%-8.8s DEV ",          
        GetTime());

aPrefixLen = strlen(aPrefix);          // get length of prefix

va_start(aArgList, theStr);            // get arguments

aFmtStr.VsnPrint                       // add
         (theStr,                      // desired data
          aArgList);

Beautify(aFmtStr, 132-aPrefixLen);     // beautify message

va_end(aArgList);                      // no longer need list

const char *aTok = aFmtStr.Tok("\n");  // search for newlines

while (aTok != NULL)                   // for all lines

{                                      // begin write line

    aOutStr.Reset();                   // reset output

    aOutStr.Print("%s%s\n",            // setup
                 aPrefix,              // output
                 aTok);                // line

    PrintOutput((const char *) aOutStr);
                                       
    Trace("%s",
          (const char *) aOutStr +     // trace 
          aPrefixLen);        

    aTok = aFmtStr.Tok("\n");          // get next line

};                                     // end write line

}

//****************************************************************************
//
// Method       : Warn
//
// Description  : Print warning output
//
// Parameters   : printf parms
//
//****************************************************************************
void CviPgm::Warn(const char *theStr, ...)
{

CviStr  aFmtStr;                       // output string
CviStr  aOutStr;                       // output string

char    aPrefix[64];                   // string prefix

va_list aArgList;                      // variable argument list

int     aPrefixLen = 0;                // prefix length


sprintf(aPrefix,                       // set time into place
        "%-8.8s ### WARNING: ",
        GetTime());

aPrefixLen = strlen(aPrefix);          // get length of prefix

va_start(aArgList, theStr);            // get arguments

aFmtStr.VsnPrint                       // add
         (theStr,                      // desired data
          aArgList);

Beautify(aFmtStr, 132-aPrefixLen);     // beautify message

va_end(aArgList);                      // no longer need list

const char *aTok = aFmtStr.Tok("\n");  // search for newlines

while (aTok != NULL)                   // for all lines

{                                      // begin write line

    aOutStr.Reset();                   // reset output

    aOutStr.Print("%s%s\n",            // setup
                 aPrefix,              // output
                 aTok);                // line

    PrintOutput((const char *) aOutStr);// display output
                                       
    Trace("%s",
          (const char *) aOutStr +     // trace 
          aPrefixLen);        

    aTok = aFmtStr.Tok("\n");          // get next line

};                                     // end write line

}

//****************************************************************************
//
// Method       : Beautify
//
// Description  : Intelligently split lines so they 'fit' in 132 length output
//
// Parameters   : 1) Destination string
//
//****************************************************************************
void CviPgm::Beautify(CviStr &theDestStr,
                      int theLen)
{

if (strlen(theDestStr) > theLen)       // if large

{                                      // begin add newlines, if necessary

    CviStr aNewStr;                    // new string to build

    const char *aTok =                 // grab new string
        theDestStr.Tok("\n");

    while (aTok != NULL)               // for all tokens

    {                                  // begin process line

        if (strlen(aNewStr) > 0)       // if had prior data

            aNewStr.Add("\n");         // add newline...

        if (strlen(aTok) <= theLen)    // if length OK

           aNewStr.Add(aTok);          // add it as-is

        else                           // otherwise

        {                              // begin locate space


            CviStr aTmpStr;            // temp string

            aTmpStr.Set(aTok);         // add 

            int aIdx = theLen;         // start at column 132

            while (aIdx > 0         && // while index range OK and
                   *(aTok+aIdx) != ' ')// not a space

                aIdx --;               // keep backing up

            aTmpStr.Replace(aIdx,      // add newline
                            "\n            ");

            aNewStr.Add(aTmpStr);      // add to final string

        };                             // end locate space

        aTok = theDestStr.Tok("\n");   // find next token

    };                                 // end process line

    theDestStr.Set(aNewStr);           // use the new string

};                                     // end add newlines, if necessary

}

//****************************************************************************
//
// Method       : Trace
//
// Description  : Trace output
//
// Parameters   : None
//
//****************************************************************************
int CviPgm::Trace(const char *theStr, ...)
{

if (aTrcInfo == NULL                || // if not allocated
    aTrcInfo->itsTrcDisabled)          // or it is disabled
{
    return(0);                         // return...now...really...get outta here...
}

CviPgm *aPgm = this;                   // assume we will use ourself

struct psa *psPsa = 0;                 // PSA
int    aTcb  = (int) psPsa->psatold;   // get TCB address

if (aPgm == NULL ||                    // if no pgm object is known or
    aTcb != itsTcb)                    // or if this is the wrong program object...

{                                      // begin grab correct CviPgm

    aPgm = GetTaskCviPgm();            // get correct pgm object

    if (aPgm == NULL)                  // if not good

        return(16);                    // fail

};                                     // end grab correct CviPgm

char *aWriteOff = NULL;                // write offset

va_list aArgList;                      // variable argument list

int     aRetry = 500;                  // retry count - 5 full seconds...
int     aRc = 0;                       // return code
int     aWrote = 0;                    // bytes written
int     aLen = 0;                      // length of trace data


va_start(aArgList, theStr);            // get arguments
                                       
aLen = vsnprintf(NULL,                 // save sprintf, get length
                 0,
                 theStr,                      
                 aArgList);                   
                                       
va_end(aArgList);                      // no longer need list

va_start(aArgList, theStr);            // reset to start of arguments
                                       
while (CS(0, aTcb,                     // make sure we are the only
          &aTrcInfo->itsTrcUse,        
          NULL) != 0                && // task using this
       aRetry > 0)                     // and have retries left

{                                      // begin wait

    CviHSleep(1);                      // rest a moment

    aRetry --;                         // decrement retries

};                                     // end wait

if (aTrcInfo->itsTrcLen + aLen + 100 > // if we would write off
     sizeof(aTrcInfo->itsTrcBuf))      // trace buffer

{                                      // begin roll trace

    memset(aTrcInfo->itsTrcBuf +       // clear the rest of the
             aTrcInfo->itsTrcLen,      // trace buffer...
           0,
           sizeof(aTrcInfo->itsTrcBuf) - 
                  aTrcInfo->itsTrcLen);

    aTrcInfo->itsTrcLen = 0;           // start over...

};                                     // end roll trace

aWriteOff = aTrcInfo->itsTrcBuf +      // set point to  
                aTrcInfo->itsTrcLen;   // trace

aWrote =     
   sprintf(aWriteOff,                  // setup write data
           "%s %-4.8s %-8.8s(%p): ",     
           GetTime(),
           itsMvsName,                 
           itsName,                    // with name
           aTcb);                      // and TCB

aWrote += vsnprintf(aWriteOff + aWrote,// add trace data
                    aLen+1,
                    theStr,                      
                    aArgList);                   
                                       

if (aWrote > 0)                        // if good

    aTrcInfo->itsTrcLen += aWrote;     // add bytes written to trc len

va_end(aArgList);                      // no longer need list
    
// NOTE: We are now about to write in a potentially "dangerous" situation.
//       It shouldn't be a 'crash' danger, however. In worst case if the
//       trace write is taking a REALLY long time we might end up getting
//       wrapped trace data rather than what we intended. So be it.

if (aTrcInfo->itsTrcOn)                // if tracing is enabled

{                                      // begin trace

    if (aPgm->itsTrcOut == NULL)       // if not open
                                       
    {                                  // begin open
    
        aPgm->itsTrcOut =              // attempt to open 
            fopen("//DDN:CVITRACE",
                  FOPENMODE);
    
        if (aPgm->itsTrcOut != NULL)   // if OK
    
            setlinebuf(aPgm->itsTrcOut); // line buffer
    
        else                           // otherwise
    
            aTrcInfo->itsTrcOn = FALSE;// trace is now off
                                       
    };                                 // end open

    CS(aTrcInfo->itsTrcUse, 0,         // clear trace flag 
       &aTrcInfo->itsTrcUse, NULL);        

    if (aPgm->itsTrcOut != NULL)       // if open
    
        fwrite(aWriteOff,              // write it out
               aWrote,                 
               1,
               aPgm->itsTrcOut);

    else                               // otherwise

        aRc = 8;                       // failure
    
}                                      // end trace

else

   CS(aTrcInfo->itsTrcUse, 0,          // clear trace flag 
      &aTrcInfo->itsTrcUse, NULL);        

return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : AllocDd
//
// Description  : Allocate SYSOUT DD
//
// Parameters   : DD name
//
//****************************************************************************
int CviPgm::AllocDd(const char *theDd)
{

int iFunc = CVIDYNA_ALLOC_SYSOUT;      // allocate SYSOUT
int iError = 0;                        // returned error
int iRc = CVIDYNA(&iFunc,              // allocate
                  theDd,               
                  &iError);            

return(iRc);                           // return result

}

//****************************************************************************
//
// Method       : Dump
//
// Description  : Dump out a block of data
//
// Parameters   : Title
//                Block
//
//****************************************************************************
int CviPgm::Dump(const char *theTitle,
                 const char *theBlock,
                       int   theLen)
{

if (aTrcInfo == NULL)                  // if not allocated

    return(0);                         // return...now...really...get outta here...

const char *aHexTab = "0123456789ABCDEF";

unsigned char c;                       // character
int  i,j,k,l;                          // iterators
int  aOffset=0;                        // offset
char aWorkStr[63+1];                   // output line
char aOffStr[5+1];                     // offset string

int  aRc = 0;                          // return code


if (aTrcInfo->itsTrcOn)                // if tracing is enabled

{                                      // begin dump block

    Trace("Dumping %p: %s\n",          // dump title 
          theBlock,
          theTitle); 

    while (aOffset < theLen)           // for entire block

    {                                  // begin for entire block

      memset(aWorkStr, ' ', sizeof(aWorkStr)-1);
      aWorkStr[sizeof(aWorkStr)-1] = 0;       
      sprintf(aOffStr, "+%04X", aOffset);     
      memcpy(aWorkStr, aOffStr, sizeof(aOffStr)-1);

      aWorkStr[45] = '*';              
      aWorkStr[62] = '*';              
      k = 46;                          // offset where string data goes

#ifdef _SRC_
      aWorkSr[81] = '*';              
      aWorkStr[114] = '*';              
      k = 82;                          // offset where string data goes
#endif

      /* Convert to hex and move into line... */
      l = 8;
//      for (i=0; i < 8; ++i)            // 8 words at a time
      for (i=0; i < 4; ++i)            // 4 words at a time
      {        
        for (j=0; j < 4; j++)
        {      
          if (aOffset >= theLen)
            break;                   
          c = theBlock[aOffset++];        
          aWorkStr[l++] = aHexTab[c/16];
          aWorkStr[l++] = aHexTab[c&15];
          if (!isprint(c))
            c = '.';                    
          aWorkStr[k++] = c;            
        }
        l += 1;                        // 1 byte inbetween words

       }

       Trace("%s\n", aWorkStr);        // write out dump line

    };                                 // end for entire block

}                                      // end dump block

else                                   // otherwise

    aRc = 8;                           // failure


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : Dump
//
// Description  : Dump out a block of data
//
// Parameters   : Title
//                Block
//
//****************************************************************************
int CviPgm::PrintDump(const char *theTitle,
                      const char *theBlock,
                            int   theLen,
                            char  theError)
{

unsigned char c;                       
int  i,j,k,l;
int  aOffset=0;
char aHexTab[16];
char aWorkStr[80+1];                   // 80 byte line
char aOffStr[5+1];

int     aRc = 0;                       // return code


if (theError)                          // if error

    Error("Dumping %p: %s\n",          // dump title 
          theBlock,                        
          theTitle);                       

else

    Print("Dumping %p: %s\n",          // dump title 
          theBlock,                        
          theTitle);                       
                                       
memcpy(aHexTab, "0123456789ABCDEF", 16);
                                       
while (aOffset < theLen)               // for entire block
                                       
{                                      // begin for entire block
                                       
  memset(aWorkStr, ' ', sizeof(aWorkStr)-1);
  aWorkStr[sizeof(aWorkStr)-1] = 0;       
  sprintf(aOffStr, "+%04X", aOffset);     
  memcpy(aWorkStr, aOffStr, sizeof(aOffStr)-1);
  aWorkStr[48] = '*';              
  aWorkStr[65] = '*';              
                                   
  /* Convert to hex and move into line... */
  k = 49;                          
  l = 8;                           
  for (i=0; i < 4; ++i)            
  {                                
    for (j=0; j < 4; j++)          
    {                              
      if (aOffset >= theLen)       
        break;                     
      c = theBlock[aOffset++];        
      aWorkStr[l++] = aHexTab[c/16];
      aWorkStr[l++] = aHexTab[c&15];
      if (!isprint(c))                 
        c = '.';                        
      aWorkStr[k++] = c;                
    }                                  
    l += 2;                            
   }                                   
                                       
   if (theError)

       Error("%s\n",                   // write out dump line 
             aWorkStr);            

   else

       Print("%s\n",                   // write out dump line 
             aWorkStr);            
                                       
};                                     // end for entire block


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : CviPgm::ToggleTrace
//
// Description  : Toggle trace on/off
//
// Parameters   : TRUE or FALSE
//
//****************************************************************************
void CviPgm::ToggleTrace(char theFlag)
{

if (theFlag == 2)                      // if disabling trace entirely

{                                      // begin disable trace

    if (aTrcInfo != NULL)              // if have trace info

        aTrcInfo->itsTrcDisabled = TRUE; // disable trace

}                                      // end disable trace

else                                   // otherwise

if (aTrcInfo != NULL)                  // if trace allocated

{                                      // begin set trace

    if (!theFlag)                      // if turning off

        Trace("Trace: Disabled\n");

    if (theFlag)                       // if turning on
        
        AllocDd("CVITRACE");           // allocate trace DD

    aTrcInfo->itsTrcOn = theFlag;      // set trace flag

    if (theFlag)                       // if turning on trace

        Trace("Trace: Enabled\n");     // trace enabled

}                                      // end set trace

}

//****************************************************************************
//
// Method       : CviPgm::TraceEnabled?
//
// Description  : Return if trace on/off
//
// Return       : TRUE or FALSE
//
//****************************************************************************
int CviPgm::TraceEnabled()
{

if (aTrcInfo != NULL)                  // if trace allocated

    return(aTrcInfo->itsTrcOn);        // return flag

return(FALSE);                         // FALSE by default

}

//****************************************************************************
//
// Method       : CviPgm::DumpTrace
//
// Description  : Dump in-memory trace
//
// Parameters   : None
//
//****************************************************************************
void CviPgm::DumpTrace()

{

struct psa *psPsa = 0;                 // PSA
int    aTcb  = (int) psPsa->psatold;   // get TCB address


if (aTrcInfo != NULL                && // if trace allocated
    !aTrcInfo->itsTrcOn)               // and trace isn't on...

{                                      // begin set trace

    AllocDd("CVITRACE");               // allocate trace DD, if not already

    FILE *aFp =                        // open up
        fopen("//DDN:CVITRACE",        // trace
              FOPENMODE);

    if (aFp != NULL)                   // if opened

    {                                  // begin write out in-memory trace data

        fprintf(aFp, "\n\n -- Starting In-Memory Trace Flush --\n\n");

        int aRetry = 100;              // retry count

        while (CS(0, aTcb,             // make sure we are the only
                  &aTrcInfo->itsTrcUse,        
                  NULL) != 0        && // task using this
               aRetry > 0)             // and have retries left

        {                              // begin wait

            CviHSleep(1);              // rest a moment

            aRetry --;                 // decrement retries

        };                             // end wait

        const char *aEnd =
            aTrcInfo->itsTrcBuf + aTrcInfo->itsTrcLen + 1;

        aEnd = strstr(aEnd, "\n");     // search for next newline

        if (aEnd != NULL)              // if found old rolled trace data

           fwrite(aEnd+1,              // write remaining data
                  strlen(aEnd+1),
                  1,
                  aFp);

        fwrite(aTrcInfo->itsTrcBuf,    // write
               aTrcInfo->itsTrcLen,    // entries
               1,
               aFp);

        CS(aTrcInfo->itsTrcUse, 0,     // clear trace flag 
           &aTrcInfo->itsTrcUse, NULL);        

        fprintf(aFp, "\n\n -- Completed In-Memory Trace Flush --\n\n");

        fclose(aFp);                   // close trace output

    };                                 // end write out in-memory trace data

}                                      // end set trace

}

//****************************************************************************
//
// Method       : CviPgm::ParseFailure
//
// Description  : Handle parse failure
//
// Parameters   : None
//
//****************************************************************************
void CviPgm::ParseFailure()

{

                                       // do nothing

}

//****************************************************************************
//
// Method       : ~CviPgm
//
// Description  : Destructor
//
// Parameters   : None
//
//****************************************************************************
CviPgm::~CviPgm()
{

if (itsStdOut != NULL)                 // if open

{                                      // begin close

    fclose(itsStdOut);                 // close

    itsStdOut = NULL;                  // mark as closed

};                                     // end close

if (itsStdErr != NULL)                 // if open

{                                      // begin close

    fclose(itsStdErr);                 // close

    itsStdErr = NULL;                  // mark as closed

};                                     // end close

if (itsTrcOut != NULL)                 // if trace is open

{                                      // begin close trace

    fclose(itsTrcOut);                 // close trace

    itsTrcOut = NULL;                  // mark as closed

};                                     // end close trace

if (aTrcInfo->itsAllocPgm == this)     // if we allocated it

{                                      // begin remove NT

    CviOsDeleteNt("ZCVITRCBUFFER   ",  // remove global trace flag
                  NAME_HOME);

    free(aTrcInfo);                    // remove trace info

    aTrcInfo = NULL;                   // clear trace info

};                                     // end remove NT

if (itsMainForTask)                    // if we were the main pgm object

{                                      // begin no longer main pgm object

    if (CviOsDeleteNt("ZCVIPGMOBJEC2TCB",  // if secondary not removed
                      NAME_TASK) != 0)

       CviOsDeleteNt("ZCVIPGMOBJECTTCB",  // remove our pgm object for
                     NAME_TASK);          // at task level

};                                     // end no longer main pgm object

}


