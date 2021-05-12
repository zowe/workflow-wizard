/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Provides a 'string' class.  This is not as efficient as normal
//         char's but is able to expand strings dynamically to prevent
//         memory corruption.  
// 
//****************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <machine/atoe.h>              // EBCDIC <-> ASCII

#include "CviDefs.h"                   // include defines
#include "CviStr.h"                    // our header

#include "asasymbp.h"                  // ASASYMBM parameters

#define  CVISTR_BASE_SIZE           16 // base size of allocation
#define  CVISTR_LARGE_SIZE       65536 // large string
//#define  CVISTR_LARGE_SIZE       32768 // large string

//#define  CVISTR_BASE_SIZE            8 // base size of allocation
//#define  CVISTR_LARGE_SIZE       32768 // large string
                                     

//int CviStr_ReallocCount = 0;
          
//****************************************************************************
//
// Method       : CviStr::CviStr 
//
// Description  : Constructor
//
//****************************************************************************
CviStr::CviStr()

{                                      // begin constructor

memcpy(itsEye,                         // set eye catcher
       "CVISTR  ",
       8);

itsData = NULL;                        // no data yet

Reset();                               // reset to base size

};                                     // end constructor

//****************************************************************************
//
// Method       : CviStr::CviStr 
//
// Description  : Constructor
//
//****************************************************************************
CviStr::CviStr(const CviStr &theStr)

{                                      // begin constructor

memcpy(itsEye,                         // set eye catcher
       "CVISTR  ",
       8);

itsData = NULL;                        // no data yet

itsDataLen = 0;                        // no data yet

itsAllocLen = 0;                       // nothing allocated

itsTokOff = -1;                        // reset token offset

if (theStr.itsData != NULL)            // if valid data area

   Set(theStr.itsData);                // copy it

};                                     // end constructor

//****************************************************************************
//
// Method       : CviStr::CviStr 
//
// Description  : Constructor
//
//****************************************************************************
CviStr::CviStr(const char *theData,
               int theLen)

{                                      // begin constructor

memcpy(itsEye,                         // set eye catcher
       "CVISTR  ",
       8);

itsData = NULL;                        // no data yet

itsDataLen = 0;                        // no data yet

itsAllocLen = 0;                       // nothing allocated

itsTokOff = -1;                        // reset token offset

Reset();                               // reset

Add(theData, theLen);                  // add to buffer

};                                     // end constructor

//****************************************************************************
//
// Method       : CviStr::Set
//
// Description  : Sets buffer to specific string
//
//****************************************************************************
void CviStr::Set(const char *theData)

{                                      // begin Set

Truncate();                            // truncate
//Reset();                               // reset

if (theData != NULL)                   // if OK

    Add(theData);                      // add the data

else                                   // otherwise

    Add("");                           // use empty string

}                                      // end Set

//****************************************************************************
//
// Method       : CviStr::Add
//
// Description  : Adds data to buffer
//
//****************************************************************************
void CviStr::Add(const char *theData,
                 int theDataLen)

{                                      // begin Add

if (theData == NULL)                   // if no data

    theDataLen  0;                     // use zero length

if (theDataLen < 0)                    // if no length set

    theDataLen = strlen(theData);      // length of incoming data

Grow(theDataLen);                      // expand if necessary

if (theDataLen > 0)                    // if we have data

    memcpy(itsData + itsDataLen,       // append data
           theData,                    // to the
           theDataLen);                // buffer

itsDataLen += theDataLen;              // set new data length

*(itsData + itsDataLen) = 0;           // NULL terminate

};                                     // end Add

//****************************************************************************
//
// Method       : CviStr::Grow
//
// Description  : Grow buffer
//
//****************************************************************************
void CviStr::Grow(int theDataLen)

{                                      // begin Grow

#define ROUND_UP(itsAllocLen,x) itsAllocLen += (x - (itsAllocLen % x))

if (itsData == NULL                 || // if nothing allocated or
    (theDataLen + itsDataLen) >=       // size of our buffer
      itsAllocLen)                     // is insufficient

{                                      // begin grow

    itsAllocLen += (theDataLen+1);     // add exact data length

    if (itsAllocLen >=                 // if growing fairly large 
        CVISTR_LARGE_SIZE)

        ROUND_UP(itsAllocLen,          // round up by larger amount
                 CVISTR_LARGE_SIZE);

    else                               // otherwise

    {

        itsAllocLen = itsAllocLen * 2; // allow 2x growth

        ROUND_UP(itsAllocLen,          // round to base size
                 CVISTR_BASE_SIZE);    // chunk

    }

    itsData = (char *)                 // allocate/reallocate 
                realloc(itsData,       // to new size           
                        itsAllocLen);    

    //CviStr_ReallocCount ++;

};                                     // end grow

};                                     // end Grow

//****************************************************************************
//
// Method       : CviStr::ReplaceSymbols
//
// Description  : Replace system symbols
//
//****************************************************************************
void CviStr::ReplaceSymbols()

{                                      // begin Replace

struct symbp sSymbp;                   // ASASYMBM parameters
void __register(2) *psSymbp;           // ASM register for parms
void __register(3) *psSf;              // SF area
void __register(0) *pR0;               // ASASYMBM uses R0
void __register(1) *pR1;               // ASASYMBM uses R1
void __register(14) *pR14;             // ASASYMBM uses R14
void __register(15) *pR15;             // ASASYMBM uses R15

char *aNewString = NULL;               // newstring

char sfArea[128];                      // LINKX parm area

int iTgtLen = 0;                       // target length
int iRc = 0;                           // return code for ASASYMBM

long  aSave1;                          // saved slot 1
long  aSave2;                          // saved slot 2
long  aSave3;                          // saved slot 3

__register(13) long *aSave;


iTgtLen = itsDataLen + 256;            // set length of target

aNewString = (char *) malloc(iTgtLen); // allocate target

memset(&sSymbp, 0, sizeof(sSymbp));    // clear it
sSymbp.symbppatterna = itsData;        // input string
sSymbp.symbppatternlength = strlen(itsData);
sSymbp.symbptargetaddr = aNewString;  // target string
sSymbp.symbptargetlengtha = &iTgtLen;  // target length
sSymbp.symbpsymboltablea = NULL;       // no symbole table
sSymbp.symbptimestampa = NULL;         // timestamp (none)
sSymbp.symbpreturncodeaddr = &iRc;     // return code
psSymbp = &sSymbp;                     // set address
psSf = sfArea;                         // use SF area

aSave1 = *(aSave+0);                   // these must be
aSave2 = *(aSave+1);                   // preserved
aSave3 = *(aSave+2);                   // when calling

memset(sfArea, 0, sizeof(sfArea));     // clear SF area

__asm 
{
    LINKX EP=ASASYMBM,MF=(E,(2)),SF=(E,(3))
}

*(aSave+0) = aSave1;                   // restore the
*(aSave+1) = aSave2;                   // three special words
*(aSave+2) = aSave3;                   // for Dignus

aNewString[iTgtLen] = 0;               // NULL terminate

Set(aNewString);                       // use new string

free(aNewString);                      // release new string memory

};                                     // end Replace

//****************************************************************************
//
// Method       : CviStr::Replace
//
// Description  : Replace character at offset with new string
//
//****************************************************************************
int  CviStr::Replace(const char *theOrig,
                     int theOrigLen,
                     const char *theValue)

{                                      // begin Replace

int rc = FALSE;                        // assume bad

if (theOrigLen >= 0 &&                 // if length is valid
    theOrig >= itsData &&              // if original is
    theOrig < (itsData+itsAllocLen))   // within our buffer

{                                      // begin replace

  if (itsDataLen > CVISTR_LARGE_SIZE)  // if we are reasonably large

  {                                    // begin use copy method

    CviStr aNew;                       // new string

    aNew.Grow(itsAllocLen);            // make room for data (or reasonably close)

    aNew.Add(itsData,                  // add starting data
             (int)(theOrig - itsData));

    aNew.Add(theValue);                // add new data

    aNew.Add(theOrig + theOrigLen);    // add remaining data

    Set(aNew);                         // set new buffer

  }                                    // end use copy method

  else                                 // we will shrink

  {                                    // begin move-in-place

    int aTokLen = theOrigLen;          // original length
    int aValLen = strlen(theValue);    // new value length
    int aIdx = (int) (theOrig - itsData);

    if (aValLen > aTokLen)             // if new value is larger

        Grow(aValLen - aTokLen);       // grow string

    memmove(itsData + aIdx + aValLen,
            itsData + aIdx + aTokLen,
            itsDataLen - aIdx - aTokLen + 1);

    memcpy(itsData + aIdx,         // copy new value
           theValue,               // into place
           aValLen);

    itsDataLen = itsDataLen + 
                   (aValLen - aTokLen);

  };                                   // end move-in-place

  rc = true;                           // replacement was OK

};                                     // end replace

return(rc);                            // return result

};                                     // end Replace

//****************************************************************************
//
// Method       : CviStr::Replace
//
// Description  : Replace character at offset with new string
//
//****************************************************************************
int  CviStr::Replace(int theOffset,
                     const char *theStr)

{                                      // begin Replace

if (theOffset < itsDataLen)            // if offset is OK

{                                      // begin grow/replace

    if (itsDataLen > CVISTR_LARGE_SIZE) // if we are reasonably large

    {                                  // begin use copy method

      CviStr aNew;                     // new string

      aNew.Grow(itsAllocLen);          // make room for data (or reasonably close)

      aNew.Add(itsData,                // add starting data
               theOffset);             

      aNew.Add(theStr);                // add new data

      aNew.Add(itsData+theOffset+1);   // add remnants

      Set(aNew);                       // set new buffer

    }                                  // end use copy method

    else                               // we will shrink

    {                                  // begin move in-place

       Grow(strlen(theStr)-1);         // grow to accommodate new string
                                       
       if (theOffset + strlen(theStr) < // if there is room
           itsAllocLen)

       {                               // begin replace

           char *aSpot = itsData +       // setup spot to replace
                         theOffset;  

           memmove(aSpot+strlen(theStr), // shift data to leave room
                   aSpot+1,              // for value
                   strlen(aSpot + 1)+1);
                                       
           memcpy(aSpot,                 // copy new string into place
                  theStr,
                  strlen(theStr));

           itsDataLen += (strlen(theStr)-1);

        };                             // end replace

    };                                 // end move in-place

};                                     // end grow/replace


return(TRUE);                          // replaced...

};                                     // end Replace

//****************************************************************************
//
// Method       : CviStr::ReplaceQandB
//
// Description  : Replace string, quote and brace aware
//
//****************************************************************************
int CviStr::ReplaceQandB(const char *theTok,
                         const char *theValue)

{                                      // begin Replace

int aRc = FALSE;                       // assume not found
int aIdx = 0;                          // index
int aTokLen = strlen(theTok);          // token length

int aBraceCnt = 0;                     // no braces yet

bool aInQuote = false;                 // not inside a quote

if (strstr(itsData, theTok) != NULL)   // if it might be found (could be in quote/brace)

{                                      // begin search/replace

   CviStr aNew;                        // new string

   aNew.Grow(itsDataLen);              // might as well grow now

   int aStart = 0;                     // starting place

   while (aTokLen > 0 &&                  // while token length OK and
          (aIdx + aTokLen) <= itsDataLen) // while possible to find a match
   
   {                                   // begin find/replace
   
       switch (*(itsData + aIdx))      // test character
                                       
       {                               // begin cases of character
                                        
           case '\"'                 : // opening quote
                                        
               aInQuote = !aInQuote;   // flip
                                        
               break;                  // end quote
                                        
           case '{'                  : // open brace
                                        
               if (!aInQuote)          // if not inside a quote
                 aBraceCnt ++;         // bump brace counter
                                        
               break;                  // end open brace
                                        
           case '}'                  : // closing brace
                                        
               if (!aInQuote)          // if not inside a quote
                 aBraceCnt --;         // remove brace
                                        
               break;                  // end closing brace
                                        
           default                   : // default
                                        
               break;                  // nothing
                                        
       };                              // end cases of character
                                          
       if (aBraceCnt == 0 &&           // if not inside
           !aInQuote      &&           // anything special and
           !memcmp(itsData + aIdx,     // if
                   theTok,             // we found
                   aTokLen))           // a match
   
       {                               // begin replace
   
           aRc = TRUE;                 // found one to replace
   
           aNew.Add(itsData + aStart,  // add
                    aIdx - aStart);    // up to this point

           aNew.Add(theValue);         // add new value

           aStart = aIdx + strlen(theTok);

           aIdx = aStart;              // pick back up where we left off
   
       }                               // end replace
   
       else
   
          aIdx ++;                     // next character, please
   
   };                                  // end find/replace
   
   aNew.Add(itsData + aStart);         // add remnants of data

   Set(aNew);                          // use new string

};                                     // end search/replace

return(aRc);                           // return

};                                     // end Replace


//****************************************************************************
//
// Method       : CviStr::Replace
//
// Description  : Replace string at offset with new string
//
//****************************************************************************
int CviStr::Replace(const char *theTok,
                    const char *theValue,
                    bool theQuoteAndBraceAware)

{                                      // begin Replace

if (theQuoteAndBraceAware)
    return(ReplaceQandB(theTok, theValue));

int aRc = FALSE;                       // assume not found

int aTokLen = strlen(theTok);          // token length
int aValLen = strlen(theValue);


if (itsDataLen <= 2048 &&              // small string?
    aValLen    <= aTokLen)             // no change in size

{

  char *aStart = itsData;              // start at the beginning
                                       
  char *aFound =                       // try to find
       strstr(aStart, theTok);         // it
                                       
  while (aFound != NULL)               // while possible to find a match

  {                                    // begin find/replace

    aRc = true;                        // found one to replace

    memmove(aFound + aValLen,          // shift remaining data
            aFound + aTokLen,
            itsDataLen - (aFound - itsData) - aTokLen + 1);

    memcpy(aFound,                     // copy new value
           theValue,                   // into place
           aValLen);

    itsDataLen = itsDataLen +          
                   (aValLen - aTokLen);

    aStart += aValLen;                 // jump beyond replacement

    aFound = strstr(aStart, theTok);

  }                                    // end replace

  return(aRc);                         // return

}

// Deep copy method

const char *aStart = itsData;          // start at the beginning
                                       
const char *aFound =                   // try to find
        strstr(aStart, theTok);        // it
                                       
if (aFound != NULL)                    // if found
                                       
{                                      // begin deep copy/replace
                                       
    CviStr aNew;                       // setup new string
                                       
    aNew.Grow(itsDataLen);             // might as well grow now
                                       
    aRc = true;                        // found one
                                       
    while (aFound != NULL)             // while we have more data
                                       
    {                                  // begin replace as we copy
                                       
        aNew.Add(aStart,               // add data up to this point
                 (int) (aFound - aStart));
                                       
        aNew.Add(theValue);            // add replacement value
                                       
        aStart = aFound +              // bump past
                 strlen(theTok);       // first token
                                       
        aFound = strstr(aStart,        // search for
                        theTok);       // more occurrences
                                       
    }                                  // end replace as we copy
                                       
    aNew.Add(aStart);                  // add remaining portion of original string
                                       
    Set(aNew);                         // use new value
                                       
};                                     // end deep copy/replace


return(aRc);                           // return result

};                                     // end Replace

//****************************************************************************
//
// Method       : CviStr::ReplaceSymbol
//
// Description  : Replace symbold with new string
//
//****************************************************************************
void CviStr::ReplaceSymbol(const char *theTok,
                           const char *theValue)

{                                      // begin Replace

struct symbp sSymbp;                   // ASASYMBM parameters
void __register(2) *psSymbp;           // ASM register for parms
void __register(3) *psSf;              // SF area
void __register(0) *pR0;               // ASASYMBM uses R0
void __register(1) *pR1;               // ASASYMBM uses R1
void __register(14) *pR14;             // ASASYMBM uses R14
void __register(15) *pR15;             // ASASYMBM uses R15

char *aNewString = NULL;               // newstring

char sfArea[128];                      // LINKX parm area

int iTgtLen = 0;                       // target length
int iRc = 0;                           // return code for ASASYMBM

long  aSave1;                          // saved slot 1
long  aSave2;                          // saved slot 2
long  aSave3;                          // saved slot 3

__register(13) long *aSave;

CviStr aSymb;                          // symbol
CviStr aValue(theValue);               // value

aSymb.Print("&%s.", theTok);

struct symbt   *aHdr;                  // symbol table

struct symbte  *aTable;                // table


aHdr = (symbt *) malloc(sizeof(symbt) + sizeof(symbte));

aHdr->symbtflag0 = symbtnodefaultsymbols;
aHdr->symbtflag1 = 0;
aHdr->symbtnumberofsymbols = 1;

aTable = (symbte *) &aHdr->symbttableentries;  // table start
aTable->symbtesymbolptr = aSymb;               // first symbol name
aTable->symbtesymbollength = strlen(aSymb);    // length of symbol
aTable->symbtesubtextptr = aValue;             // first symbol value
aTable->symbtesubtextlength = strlen(aValue);  // length of value

iTgtLen = itsDataLen + 256;            // set length of target

aNewString = (char *) malloc(iTgtLen); // allocate target

memset(&sSymbp, 0, sizeof(sSymbp));    // clear it
sSymbp.symbppatterna = itsData;        // input string
sSymbp.symbppatternlength = strlen(itsData);
sSymbp.symbptargetaddr = aNewString;  // target string
sSymbp.symbptargetlengtha = &iTgtLen;  // target length
sSymbp.symbpsymboltablea = aHdr;       // symbol table
sSymbp.symbptimestampa = NULL;         // timestamp (none)
sSymbp.symbpreturncodeaddr = &iRc;     // return code
psSymbp = &sSymbp;                     // set address
psSf = sfArea;                         // use SF area

aSave1 = *(aSave+0);                   // these must be
aSave2 = *(aSave+1);                   // preserved
aSave3 = *(aSave+2);                   // when calling

memset(sfArea, 0, sizeof(sfArea));     // clear SF area

__asm 
{
    LINKX EP=ASASYMBM,MF=(E,(2)),SF=(E,(3))
}

*(aSave+0) = aSave1;                   // restore the
*(aSave+1) = aSave2;                   // three special words
*(aSave+2) = aSave3;                   // for Dignus

aNewString[iTgtLen] = 0;               // NULL terminate

Set(aNewString);                       // use new string

free(aNewString);                      // release new string memory

free(aHdr);                            // release header

};                                     // end Replace

//****************************************************************************
//
// Method       : CviStr::ReplaceSymbols
//
// Description  : Replace symbold with new string
//
//****************************************************************************
void CviStr::ReplaceSymbols(char **theNames,
                            char **theValues,
                            int theCount)

{                                      // begin Replace

struct symbp sSymbp;                   // ASASYMBM parameters
void __register(2) *psSymbp;           // ASM register for parms
void __register(3) *psSf;              // SF area
void __register(0) *pR0;               // ASASYMBM uses R0
void __register(1) *pR1;               // ASASYMBM uses R1
void __register(14) *pR14;             // ASASYMBM uses R14
void __register(15) *pR15;             // ASASYMBM uses R15

char *aNewString = NULL;               // newstring

char sfArea[128];                      // LINKX parm area

int iTgtLen = 0;                       // target length
int iRc = 0;                           // return code for ASASYMBM

long  aSave1;                          // saved slot 1
long  aSave2;                          // saved slot 2
long  aSave3;                          // saved slot 3

__register(13) long *aSave;

struct symbt   *aHdr;                  // symbol table

struct symbte  *aTable;                // table


aHdr = (symbt *) malloc(sizeof(symbt) + sizeof(symbte) * theCount);

aHdr->symbtflag0 = symbtnodefaultsymbols;
aHdr->symbtflag1 = 0;
aHdr->symbtnumberofsymbols = theCount;

aTable = (symbte *) &aHdr->symbttableentries;  // table start

int aIdx = 0;                          // index

while (aIdx < theCount)                // for all items

{                                      // begin add name/value pairs

    char *aName = theNames[aIdx];
    char *aValue = theValues[aIdx];

    aTable->symbtesymbolptr = aName;               // first symbol name
    aTable->symbtesymbollength = strlen(aName);    // length of symbol
    aTable->symbtesubtextptr = aValue;             // first symbol value
    aTable->symbtesubtextlength = strlen(aValue);  // length of value

    aTable++;                          // next table entry

    aIdx ++;                           // next index

};                                     // end add name/value pairs

iTgtLen = itsDataLen + 256;            // set length of target

aNewString = (char *) malloc(iTgtLen); // allocate target

memset(&sSymbp, 0, sizeof(sSymbp));    // clear it
sSymbp.symbppatterna = itsData;        // input string
sSymbp.symbppatternlength = strlen(itsData);
sSymbp.symbptargetaddr = aNewString;  // target string
sSymbp.symbptargetlengtha = &iTgtLen;  // target length
sSymbp.symbpsymboltablea = aHdr;       // symbol table
sSymbp.symbptimestampa = NULL;         // timestamp (none)
sSymbp.symbpreturncodeaddr = &iRc;     // return code
psSymbp = &sSymbp;                     // set address
psSf = sfArea;                         // use SF area

aSave1 = *(aSave+0);                   // these must be
aSave2 = *(aSave+1);                   // preserved
aSave3 = *(aSave+2);                   // when calling

memset(sfArea, 0, sizeof(sfArea));     // clear SF area

__asm 
{
    LINKX EP=ASASYMBM,MF=(E,(2)),SF=(E,(3))
}

*(aSave+0) = aSave1;                   // restore the
*(aSave+1) = aSave2;                   // three special words
*(aSave+2) = aSave3;                   // for Dignus

aNewString[iTgtLen] = 0;               // NULL terminate

Set(aNewString);                       // use new string

free(aNewString);                      // release new string memory

free(aHdr);                            // release header

};                                     // end Replace


//****************************************************************************
//
// Method       : CviStr::Print
//
// Description  : Adds data to buffer via sprintf mechanism
//
//****************************************************************************
void CviStr::Print(const char *theStr, ...)

{                                      // begin Print

va_list aArgList;                      // variable argument list

va_start(aArgList, theStr);            // get arguments

VsnPrint(theStr, aArgList);            // print with arguments

va_end(aArgList);                      // no longer need list

};                                     // end Print

//****************************************************************************
//
// Method       : CviStr::Print
//
// Description  : Adds data to buffer via sprintf mechanism
//
//****************************************************************************
void CviStr::VsnPrint(const char *theStr, va_list theArgList)

{                                      // begin Print

va_list aTempList;
 
va_copy(aTempList, theArgList);        // copy arguments

int theDataLen =                       // get length
    vsnprintf(NULL,                    // of
              0,                       // our
              theStr,                  // resulting
              aTempList);              // string

va_end(aTempList);                     // terminate 

if (itsData == NULL                 || // if nothing allocated or
    (theDataLen + itsDataLen) >=       // size of our buffer
      itsAllocLen)                     // is insufficient

    Grow(theDataLen + 1);              // grow buffer

vsnprintf(itsData + itsDataLen,        // now we can
          theDataLen+1,                // build the
          theStr,                      // actual
          theArgList);                 // string

itsDataLen += theDataLen;              // set new data length

};                                     // end Print

//****************************************************************************
//
// Method       : CviStr::Reset
//
// Description  : Reset buffer
//
//****************************************************************************
void CviStr::Reset()                   // reset buffer

{                                      // begin Reset

itsDataLen = 0;                        // clear data length

itsTokOff = -1;                        // reset token offset

//if (itsData == NULL                 || // if nothing allocated yet or
//    itsAllocLen >= CVISTR_LARGE_SIZE)  // we are large

if (itsData == NULL)                   // if nothing allocated yet 

{                                      // begin reset to base size

    itsAllocLen = CVISTR_BASE_SIZE;    // set to base size

    if (itsData == NULL)               // if not allocate

        free(itsData);                 // release

    itsData = (char *)                 // allocate
                malloc(itsAllocLen);

};                                     // end reset to base size

*itsData = 0;                          // NULL terminate

}                                      // end Reset

//****************************************************************************
//
// Method       : CviStr::Truncate
//
// Description  : Truncate, but don't reset allocation.
//                Caller anticipates the string will grow back to around
//                its original size.
//
//****************************************************************************
void CviStr::Truncate()                // reset buffer

{                                      // begin Reset

itsDataLen = 0;                        // clear data length

itsTokOff = -1;                        // reset token offset

if (itsData != NULL)                   // if we have data

    *itsData = 0;                      // NULL terminate

}                                      // end Reset

//****************************************************************************
//
// Method       : CviStr::Trim
//
// Description  : Trim trailing character
//
//****************************************************************************
void CviStr::Trim(char theChar)        // trim character

{                                      // begin Trim 

int anOffset = itsDataLen - 1;         // start with last character

while (anOffset >= 0 &&                // while in range and
       *(itsData+anOffset) == theChar) // character matches

    anOffset --;                       // back up

anOffset ++;                           // bump up one
itsDataLen = anOffset;                 // set length to include NULL terminator

*(itsData + itsDataLen) = 0;           // NULL terminate

}                                      // end Trim  

//****************************************************************************
//
// Method       : CviStr::TrimWhitespace
//
// Description  : Trim trailing whitespace
//
//****************************************************************************
void CviStr::TrimWhitespace()          // trim whitespace from end

{                                      // begin TrimWhitespace 

int anOffset = itsDataLen - 1;         // start with last character

while (anOffset >= 0 &&                // while in range and
       (*(itsData+anOffset) == ' ' ||  // it is a space
        *(itsData+anOffset) == '\n'))  // newline

    anOffset --;                       // back up

anOffset ++;                           // bump up one
itsDataLen = anOffset;                 // set length to include NULL terminator

*(itsData + itsDataLen) = 0;           // NULL terminate

}                                      // end TrimWhitespace  


//****************************************************************************
//
// Method       : CviStr::Tok
//
// Description  : Gets next token, skipping X tokens
//
//****************************************************************************
const char *CviStr::Tok(const char *theDelim,
                        int theSkipCnt)

{                                      // begin tok with skip
                                       
const char *aTok = Tok(theDelim);      // get first token

while (aTok != NULL                 && // while we have a token and
       theSkipCnt > 0)                 // have items to skip

{                                      // begin get next token

    theSkipCnt --;                     // decrement skip count

    aTok = Tok(theDelim);              // get next token

};                                     // end get next token


return(aTok);                          // return token

}                                      // end tok with skip

//****************************************************************************
//
// Method       : CviStr::ResetTok
//
// Description  : Reset tokenizer, optionally passing offset to tokenize
//
//****************************************************************************
void CviStr::ResetTok(int theOffset)

{                                      // begin tok with skip
                                       
itsTokOff = theOffset;                 // reset token offset

if (itsTokOff < 0) itsTokOff = -1;     // anything negative means start over

}                                      // end tok with skip

//****************************************************************************
//
// Method       : CviStr::Tok
//
// Description  : Gets next token
//
//****************************************************************************
const char *CviStr::Tok(const char *theDelim)

{                                      // begin Tok

const char *aTok = NULL;               // no token yet

if (itsTokOff == -1                 && // if first time
    itsDataLen > 0)                    // and OK so far

{                                      // begin first token call

    aTok = itsData;                    // use data
    itsTokOff = 0;                     // start at beginning

}                                      // end first token call

else                                   // otherwise

if (itsTokOff >= 0)                    // if OK

{                                      // begin restore string

    itsTokOff ++;                      // advance past delimiter

    if (itsTokOff < itsDataLen)        // if not at the end

      aTok = itsData + itsTokOff;      // get next token start

};                                     // end restore string

if (aTok != NULL)                      // if we have token to give back

{                                      // begin find end of token
                                       
   itsTokOff +=                        // find next delimiter
        strcspn(itsData + itsTokOff,   // in our data
                theDelim);             // string

   *(itsData + itsTokOff) = 0;         // put in NULL

}                                      // end find end of token

else                                   // otherwise

   itsTokOff = -1;                     // reset


return(aTok);                          // return token

}                                      // end Tok

//****************************************************************************
//
// Method       : CviStr::Parm
//
// Description  : Gets next parameter. This differs from Tok() only in that
//                it is aware of quotation marks and will not tokenize within
//                quotations.
//
//****************************************************************************
inline int CharIn(const char *theHaystack, char theNeedle)
{
char aTest[2];
aTest[0] = theNeedle;
aTest[1] = 0;
if (strstr(theHaystack, aTest) != NULL) return 1;
return 0;
}

const char *CviStr::Parm(const char *theDelim,
                         bool theStripQuotes)

{                                      // begin Parm

const char *aTok = NULL;               // no token yet

if (itsTokOff == -1                 && // if first time
    itsDataLen > 0)                    // and OK so far

{                                      // begin first token call

    aTok = itsData;                    // use data
    itsTokOff = 0;                     // start at beginning

}                                      // end first token call

else                                   // otherwise

if (itsTokOff >= 0)                    // if OK

{                                      // begin restore string

    itsTokOff ++;                      // advance past delimiter

    if (itsTokOff < itsDataLen)        // if not at the end

      aTok = itsData + itsTokOff;      // get next token start

};                                     // end restore string

while (*aTok == ' ' || *aTok == '\n')  // while it starts with a space or newline

{                                      // begin bypass starting white space

    aTok ++;                           // advance beyond white space

    itsTokOff ++;                      // advance beyond white space

};                                     // end bypass starting white space

if (aTok != NULL)                      // if we have token to give back

{                                      // begin find end of token
                                       
   int aTokOff = itsTokOff;            // offset

   while (*(itsData + aTokOff) != 0 &&       // while not end of string and
          !CharIn(theDelim, *(itsData + aTokOff))) // no delimiter found yet

   {                                   // begin keep looking

       if (*(itsData + aTokOff) == '"') // if quoted string

       {                               // begin find end of string

           aTokOff ++;                 // skip past start of quote

           while (*(itsData + aTokOff) != 0 &&
                  *(itsData + aTokOff) != '"')

                  aTokOff ++;          // advance beyond 
                                       
           if (*(itsData + aTokOff) != 0) // if not end of string

               aTokOff ++;                // go to next character

       }                               // end find end of string

       else                            // otherwise

           aTokOff ++;                 // advance

   };                                  // end keep looking

   if (theStripQuotes &&               // if we should strip quotes and
       *aTok == '"' &&                 // if it starts with a quote
       aTokOff > itsTokOff          && // and
       *(itsData + aTokOff - 1) == '"' )// it ends with one as well

   {                                   // begin bypass quotes

       aTok ++;                        // bypass token

       *(itsData + aTokOff - 1) = 0;   // put in NULL

   };                                  // end bypass quotes

   itsTokOff = aTokOff;                // set token end offset

   *(itsData + itsTokOff) = 0;         // put in NULL

}                                      // end find end of token

else                                   // otherwise

   itsTokOff = -1;                     // reset


return(aTok);                          // return token

}                                      // end Parm

//****************************************************************************
//
// Method       : CviStr::VelocityEscape
//
// Description  : Adds Velocity escape sequencing
//
//****************************************************************************
void CviStr::VelocityEscape()          

{                                      // begin const

VelocityUnescape();                    // remove existing escape sequences

Add("]]#");                            // append closing escape
Replace(itsData, 0, "#[[");            // prepend opening escape

const char *aOpen = strstr(itsData, "${_");

while (aOpen != NULL)                   // for all of these

{                                      // begin escape variable name
                                       
    const char *aClose = strstr(aOpen, "}");  // find close

    if (aClose != NULL)                // if closed

    {                                  // begin escape this part
                                       
        int aOpenOffset = (int) (aOpen - itsData);

        Replace(aClose, 1, "}#[[");    // add escape after variable
        Replace(itsData+aOpenOffset,
                1,
                "]]#$");

        aOpen = itsData + aOpenOffset + 4;

//        printf("%s\n", itsData);

    }                                  // end escape this part

    aOpen = strstr(aOpen+1, "${_");    // find next one

}                                      // end escape variable name

}                                      // end const

//****************************************************************************
//
// Method       : CviStr::VelocityUnescape
//
// Description  : Removes Velocity escape sequencing
//
//****************************************************************************
void CviStr::VelocityUnescape()          

{                                      // begin const

const char *aOpen = strstr(itsData,    // locate
                           "#[[");     // opening


if (aOpen != NULL)                     // if found

{                                      // begin unescape

    CviStr aNew;                       // new string

    aNew.Grow(itsDataLen);             // allocate space

    const char *aStart = itsData;      // starting point

    while (aOpen != NULL)              // for all opening sequences

    {                                  // begin find close/remove
                                       
        char *aClose = strstr(aOpen, "]]#");

        if (aClose != NULL)                // found close?
        {                                  // begin remove open/close
                                       
            aNew.Add(aStart, (int) (aOpen - aStart));

            aNew.Add(aOpen+3, (int) (aClose - aOpen - 3));

            aStart = aClose + 3;       // skip beyond close

            aOpen = strstr(aStart, "#[[");

        }                              // end remove open/close
        else
            break;                     // otherwise we are done

    };                                 // find close/remove

    aNew.Add(aStart);                  // add remnants

    Set(aNew);                         // use new result

};                                     // end unescape

}                                      // end const

//****************************************************************************
//
// Method       : CviStr::operator char *
//
// Description  : Reset buffer
//
//****************************************************************************
CviStr::operator char *() const        // convert to char *

{                                      // begin const

return(itsData);                       // return data

}                                      // end const

//****************************************************************************
//
// Method       : CviStr::ToEBCDIC
//
// Description  : Convert to EBCDIC
//
//****************************************************************************
void CviStr::ToEBCDIC()                // convert to EBCDIC

{                                      // begin convert

__stratoe((unsigned char *) itsData);  // convert ASCII to EBCDIC

}                                      // end convert

//****************************************************************************
//
// Method       : CviStr::ToAscii
//
// Description  : Convert to ASCII
//
//****************************************************************************
void CviStr::ToAscii()                 // convert to ASCII

{                                      // begin convert

__stretoa((unsigned char *) itsData);  // convert EBCDIC to ASCII

}                                      // end convert

//****************************************************************************
//
// Method       : CviStr::operator= const CviStr &
//
// Description  : Set from another CviStr
//
//****************************************************************************
CviStr &CviStr::operator=(const CviStr &theStr)  // convert from const char *

{                                      // begin equals

Set(theStr);                           // call Set

return(*this);                         // return ourself

}                                      // end equals

//****************************************************************************
//
// Method       : CviStr::operator= const char *
//
// Description  : Set from const char *
//
//****************************************************************************
CviStr &CviStr::operator=(const char *theStr)  // convert from const char *

{                                      // begin equals

Set(theStr);                           // call Set

return(*this);                         // return ourself

}                                      // end equals

//****************************************************************************
//
// Method       : CviStr::operator += char *
//
// Description  : Append
//
//****************************************************************************
CviStr &CviStr::operator+=(const char *theStr)  // add const char *

{                                      // begin append

Add(theStr);                           // call Add

return(*this);                         // return ourself

}                                      // end append

//****************************************************************************
//
// Method       : CviStr::operator += char
//
// Description  : Append
//
//****************************************************************************
CviStr &CviStr::operator+=(const char theChar)  // add const char

{                                      // begin append

Add(&theChar, 1);                      // call Add

return(*this);                         // return ourself

}                                      // end append

//****************************************************************************
//
// Method       : CviStr::operator -= char *
//
// Description  : Remove string
//
//****************************************************************************
CviStr &CviStr::operator-=(const char *theStr)  // remove const char *

{                                      // begin append

Replace(theStr, "");                   // remove the string

return(*this);                         // return ourself

}                                      // end append

//****************************************************************************
//
// Method       : CviStr::operator == char *
//
// Description  : Compare
//
//****************************************************************************
int CviStr::operator==(const char *theStr)  // compare const char *

{                                      // begin compare

return(!strcmp(theStr, itsData));      // return comparison

}                                      // end compare

//****************************************************************************
//
// Method       : CviStr::operator == const CviStr
//
// Description  : Compare
//
//****************************************************************************
int CviStr::operator==(const CviStr &theStr)  // compare 

{                                      // begin compare

return(!strcmp(theStr.itsData,         // return comparison 
               itsData));      

}                                      // end compare

//****************************************************************************
//
// Method       : CviStr::operator != char *
//
// Description  : Compare
//
//****************************************************************************
int CviStr::operator!=(const char *theStr)  // compare const char *

{                                      // begin compare

return(strcmp(theStr, itsData));       // return comparison

}                                      // end compare

//****************************************************************************
//
// Method       : CviStr::operator != const CviStr
//
// Description  : Compare
//
//****************************************************************************
int CviStr::operator!=(const CviStr &theStr)  // compare 

{                                      // begin compare

return(strcmp(theStr.itsData,          // return comparison 
              itsData));               

}                                      // end compare

//****************************************************************************
//
// Method       : CviStr::~CviStr
//
// Description  : Destructor
//
//****************************************************************************
CviStr::~CviStr()                      // destructor

{

if (itsData != NULL)                   // if allocated

    free(itsData);                     // release data

}
