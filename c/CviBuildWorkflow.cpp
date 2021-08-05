//****************************************************************************
// DESCRIPTION
//         Utility to build a workflow based upon a template
// 
//****************************************************************************

//****************************************************************************
// Changes desired
//****************************************************************************
//
// 1) Track which templates were involved in each step produced for 
//    diagnostic purposes.
//
// 2) Add support for adding options/questions...
//
//****************************************************************************

#define _EXT
#include <dynit.h>                     // dynamic allocation
#include <stdio.h>                     // stdio
#include <string.h>                    // string
#include <stdlib.h>                    // stdlib
#include <ctype.h>                     // c types
#include <dirent.h>                    // list directory

#include "CviBuildWorkflow.h"          // workflow program class
#include "CviVariables.h"              // variables
#include "CviExpression.h"             // exprssion analyzer

#include "CviOs.h"                     // OS calls
#include "CviDefs.h"                   // include defines
#include "CviStr.h"                    // include string

#include "CviMemberList.h"             // include member list

//****************************************************************************
// 
// Useful macro
//
//****************************************************************************
#define SETRC(v,c) { if (v < c) v = c; }

#define SET_NL "#set ( $NL = \"\" )\n" // set newline


//****************************************************************************
// Trace macros to improve efficiency
//****************************************************************************
CviPgm *aTrcPgm = NULL;
#define TRACE(s,...) \
  if (aTrcPgm != NULL) \
   { aTrcPgm->Trace(s, ##__VA_ARGS__); } else 

//****************************************************************************
// I/O Stuff
//****************************************************************************
#define READ_BUF_SIZE 65536
#define READ_LOW_SIZE 256

#define FOPEN_RMODE "r,recfm=FB,lrecl=133,blksize=133"

//****************************************************************************
// TCBs for getting member info on datasets
//****************************************************************************
#define COMPUTE_TCB_MAX            100 // maximum TCBs for computing members
#define COMPUTE_TCB_WM              50 // watermark to start more TCBs  

//****************************************************************************
// Target types
//****************************************************************************
#define TARGET_CONFIGURATION         0 // full configuration workflow
#define TARGET_DEPLOYMENT            1 // deployment-time workflow

//****************************************************************************
// Shameless globals
//****************************************************************************
char itsDevPrint = false;       // assume devprint not on
char itsGenJobname = true;      // generate job names
char itsGenOnly = false;        // generate, rather than submit, jobs?
char itsCombineSteps = true;    // assume combining steps is OK

WorkflowStep *aPrev = NULL;     // previous step holder


//****************************************************************************
// Define stuff for dev print
//****************************************************************************
#define DPRINT(...) { if (itsDevPrint) { \
                      CviPgm *aPgm = GetTaskCviPgm(); \
                      aPgm->DevPrint(##__VA_ARGS__); }; }
                      

//****************************************************************************
// ToBase - converts string in base 10 to string in base X 
//****************************************************************************
static void ToBase(CviStr &theInOut,int theBase)   // convert a number as a string to base X string
{

const char *aCvt = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned long aNum = strtoul(theInOut, NULL, 10);  // convert to really long integer format
unsigned long aRem = 0;                // holds remainder

char aOutBuf[256];                     // output buffer

if (theBase > 36 || theBase < 2) return;

theInOut.Reset();                      // reset

char *aOut = aOutBuf + sizeof(aOutBuf)-1; // setup to write at the end of string

*aOut = 0;                             // output empty to start

while (aOut > aOutBuf &&
       aNum > 0)                       // while we have more of the number left

{                                      // begin convert to base X
                                       
    aRem = (aNum % theBase);           // get remainder for base X digit
    aNum /= theBase;                   // divide for next digit

    aOut --;                           // decrement

    *aOut = aCvt[aRem];                // drop character into place

//    memmove(aOut+1,aOut,strlen(aOut)+1); // shift output over by 1 character, including EOS
//
//    *aOut = aCvt[aRem];                // set output character from translation table

};                                     // end convert to base X

theInOut.Set(aOut);                    // setup answer

}

//****************************************************************************
//
// Function     : ReadFile
//
// Description  : Reads content of a file and append to data string
//                
//
//****************************************************************************
int ReadFile(const char *theName,      // filename
             CviStr &theData)          // location to store data

{

int aRc = 0;                           // OK so far

FILE *aFp;
 

if (!strcmp(theName, "DD:SYSIN"))      // if trying to read SYSIN info

    aFp = stdin;                       // use stdin

else                                   // otherwise

    aFp = fopen(theName, "r");         // open command dataset

if (aFp != NULL)                       // if OK

{                                      // begin read lines

    char aBuffer[8192];

    int aRecLen = fread(aBuffer,
                        1,
                        sizeof(aBuffer),
                        aFp);

    GetTaskCviPgm()->Print("Read %d bytes\n", aRecLen);

    while (aRecLen > 0)                // for all data

    {                                  // begin read in data
                                       
        theData.Add(aBuffer, aRecLen); // append new data

        aRecLen = fread(aBuffer,
                        1,
                        sizeof(aBuffer),
                        aFp);

    };                                 // end read in data

    fclose(aFp);                       // close it

}                                      // end read lines

else                                   // otherwise

{                                      // begin fail

    aRc = 8;                           // failed

    GetTaskCviPgm()->Error("Failed opening SYSIN DD\n");

};                                     // end fail

return(aRc);                           // return result

}                                      // end ProcessInput

//****************************************************************************
//
// Method       : IsNumber
//
// Description  : Is string a number?
//
// Parameters   : 1) String to check
//
// Returns      : 0/1
//
//****************************************************************************
static int IsNumber(const char *theStr)

{

int rc = 0;                            // result

int aIdx = 0;                          // string index

while (theStr[aIdx] != 0 &&            // if string
       isdigit(theStr[aIdx]))          // is digit
  aIdx ++;                             // keep going

if (theStr[aIdx] == 0               && // if got to end of string
    aIdx > 0)                          // and it wasn't empty

    rc = 1;                            // we were successful

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : LocateEndParm
//
// Description  : Locate end of parameter block
//
// Parameters   : 1) Parameter string - after opening (
//                2) Call name to use for error message
//                3) Call data to dump for error message
//                4) Workflow step to use for error message
//
// Returns      : NULL if end not found... (error)
//                Address of closing parenthesis
//
//****************************************************************************
static const char *LocateEndParm(const char *theOpen,
                                 const char *theCallName,
                                 const char *theCallData,
                                 WorkflowStep *theStep)

{

const char *aEnd = theOpen;            // initialize ending character candidate

int aParenCnt = 1;                     // we are in parenthesis processing
char aInQuote = 0;                     // in quote setting
char aInBrace = 0;                     // not inside brace

while (aParenCnt > 0 &&                // while in parenthesis
       *aEnd != 0)                     // and not at the end

{                                      // begin find end
                           
    aEnd = strpbrk(aEnd, "\"[]()");    // find next character

    if (aEnd == NULL) break;           // break out if not found...

    if (aInQuote)                      // inside quote?

    {                                  // begin check for end quote

        if (*aEnd == '"')              // found end of quote?
            aInQuote = 0;              // no longer inside quote

    }                                  // end check for end quote

    else                               // otherwise

    if (aInBrace)                      // inside brace?

    {                                  // begin check for end brace
        if (*aEnd == ']')              // found end of brace?
            aInBrace = 0;              // no longer inside brace

    }                                  // end check for end brace

    else                               // otherwise

    {                                  // begin handle nested (), start of quote/brackets
        if (*aEnd == '[')              // start of brace
            aInBrace = 1;
        if (*aEnd == '(')              // start?
            aParenCnt ++;              // bump nested count
        if (*aEnd == ')')              // end?
            aParenCnt --;              // decrement nested count
        if (*aEnd == '"')              // start of quote?
            aInQuote = 1;              // inside a quote

    };                                 // end handle nested (), start of quote/brackets

    if (aParenCnt != 0)                // not finished?
        aEnd ++;                       // bump

};                                     // end find end

if (aParenCnt > 0       ||             // if end not found
    aInBrace  > 0)                     // or brace count not OK

{                                      // begin error
                           
    if (aInBrace  > 0)

        GetTaskCviPgm()->Print("ERROR: No closing ] found for %s in template member %s, name %s\nFirst 512 bytes of statement text:\n%.512s\n",
                               (const char *) theCallName,
                               (const char *) theStep->GetMember(),
                               (const char *) theStep->GetName(),
                               (const char *) theCallData);

    if (aParenCnt > 0)

        GetTaskCviPgm()->Print("ERROR: No closing ) found for %s in template member %s, name %s\nFirst 512 bytes of statement text:\n%.512s\n",
                               (const char *) theCallName,
                               (const char *) theStep->GetMember(),
                               (const char *) theStep->GetName(),
                               (const char *) theCallData);

    aEnd = NULL;                       // this is a failure

};                                     // end error


return(aEnd);                          // return ending ) location or NULL

}

//****************************************************************************
// 
// Class        : ConditionalContent
//
// Description  : Holds conditional content
// 
//****************************************************************************
class ConditionalContent
{
public:
    CviStr itsCondition;
    CviStr itsContent;
    ConditionalContent *itsNext;

public:

    ConditionalContent(const char *theCondition,
                       const char *theContent = NULL)
    {
        itsCondition.Set(theCondition);
        if (theContent != NULL)
           itsContent.Set(theContent);
        itsNext = NULL;
    }

    void Append(const char *theContent)
    {
        itsContent.Add(theContent);
    }
    void Append(ConditionalContent *theItem)
    {
       if (itsNext != NULL)
           itsNext->Append(theItem);
       else
           itsNext = theItem;
    }
    ~ConditionalContent()
    {
        if (itsNext != NULL)
            delete itsNext;
    }

};

//****************************************************************************
// 
// Class        : MacroCache
//
// Description  : Holds macro result caches
// 
//****************************************************************************
class MacroCache
{
public:
    void *itsMacro;
    CviStr itsParms;
    CviStr itsResult;
    MacroCache *itsNext;
    MacroCache *itsNextBucket;

    int itsHits;
    int itsAnswers;

public:

    MacroCache()
    {
        itsMacro = NULL;
        itsNext = NULL;
        itsNextBucket = NULL;
        itsHits = 0;
        itsAnswers = 0;
    }

    void AddAnswer(void *theMacro,
                   CviStr &theParms,
                   CviStr &theResult)
    {

        if (theMacro == NULL) return;

        MacroCache *aCache = new MacroCache;

        aCache->itsMacro = theMacro;
        aCache->itsParms = theParms;
        aCache->itsResult = theResult;

        MacroCache *aBucket = this;
        while (aBucket != NULL &&
               aBucket->itsMacro != theMacro)
            aBucket = aBucket->itsNextBucket;

        if (aBucket != NULL)
        {
            aCache->itsNextBucket = NULL;
            aCache->itsNext = aBucket->itsNext;
            aBucket->itsNext = aCache;
        }
        else
        {
            itsNext = NULL;
            aCache->itsNextBucket = itsNextBucket;
            itsNextBucket = aCache;
        };

        itsAnswers ++;

    }

    int GetAnswer(void *theMacro,
                  CviStr &theParms,
                  CviStr &theResult)

    {
        int aRc = 1;

        MacroCache *aCache = this;

        while (aCache != NULL &&
               aCache->itsMacro != theMacro)
            aCache = aCache->itsNextBucket;

        while (aRc == 1 && aCache != NULL)
        {
            if (aCache->itsMacro == theMacro &&
                aCache->itsParms == theParms)
            {
                theResult = aCache->itsResult;
                aRc = 0;
                itsHits ++;
            }
            aCache = aCache->itsNext;
        }

        return(aRc);
    }

    void Clear()
    {
        if (itsNext != NULL)
           delete itsNext;
        if (itsNextBucket != NULL)
            delete itsNextBucket;
        itsMacro = NULL;
        itsParms.Reset();
        itsResult.Reset();
    }

    ~MacroCache()
    {
        if (itsNext != NULL)
            delete itsNext;
        if (itsNextBucket != NULL)
            delete itsNextBucket;
    }

};

MacroCache aMacroCache;

//****************************************************************************
// 
// Class        : MacroParms
//
// Description  : Holds macro parameters
// 
//****************************************************************************
class MacroParms
{
public:
    CviStr itsData;
    MacroParms *itsNext;

public:

    MacroParms(const char *theData)
    {
        if (strlen(theData) > 1 &&
            *theData == '"' &&
            *(theData+strlen(theData)-1) == '"')
            itsData.Add(theData+1,
                        strlen(theData)-2);
        else
            itsData.Set(theData);
        itsNext = NULL;
    }
    void Append(MacroParms *theParm)
    {
       if (itsNext != NULL)
           itsNext->Append(theParm);
       else
           itsNext = theParm;
    }
    ~MacroParms()
    {
        if (itsNext != NULL)
            delete itsNext;
    }

};

static MacroParms *GenParmList(CviStr &theParms)
{

    int aParenCnt = 0;                 // paranthesis count
    char aInQuote = 0;                 // inside quote (0 if not, else quote char)
    char aInBrace = 0;

    const char *aParm = theParms;      // grab start of parm

    CviStr aParmData;                  // parameter data

    MacroParms *aParmList = NULL;

    while (*aParm != 0)                // for all parm characters

    {

        if (aInQuote)                  // inside quote?

        {                              // begin check for end quote

            if (*aParm == '"')         // found end of quote?
                aInQuote = 0;          // no longer inside quote

        }                              // end check for end quote

        else                           // otherwise

        if (aInBrace)                  // inside brace?

        {                              // begin check for end brace
            if (*aParm == ']')         // found end of brace?
                aInBrace = 0;          // no longer inside brace

        }                              // end check for end brace

        else                           // otherwise

        {                              // begin handle nested (), start of quote/brackets
            if (*aParm == '[')         // start of brace
                aInBrace = 1;
            if (*aParm == '(')         // start?
                aParenCnt ++;          // bump nested count
            if (*aParm == ')')         // end?
                aParenCnt --;          // decrement nested count
            if (*aParm == '"')         // start of quote?
                aInQuote = 1;          // inside a quote

        };                             // end handle nested (), start of quote/brackets

        if (*(aParm+1) == 0 ||         // if end of parms or
            (aInQuote == 0 &&          // not inside a quote
             aInBrace == 0 &&          // not inside a brace
             aParenCnt == 0 &&         // and not nested
             *aParm == ','))           // and we have a comma

        {                              // begin end of parm

            if (*(aParm+1) == 0 &&     // if this is last
                !aInQuote &&           // and not inside
                aParenCnt == 0)        // quotes or parens
                aParmData += *aParm;   // add last character to parm data

            aParmData.TrimWhitespace(); // remove whitespace

            MacroParms *aParmItem =
                new MacroParms(aParmData);

            if (aParmList != NULL)
                aParmList->Append(aParmItem);
            else
                aParmList = aParmItem;

            TRACE("Parameter %s\n",
                  (const char *) aParmItem->itsData);

            aParmData.Reset();         // reset data

        }                              // end end of parm 

        else                           // otherwise
        if (*aParmData != 0 ||
            (*aParm != '\n' &&         // if not
             *aParm != ' '))           // whitespace

            aParmData += *aParm;       // add parm character

        aParm ++;                      // next!
    }
    return(aParmList);
}

//****************************************************************************
// 
// Populate
// 
//****************************************************************************
class  Populate                         // Populate
{

private:
    CviStr  itsId;                     // ID
    CviStr  itsSourceDSN;              // source DSN

    int     itsMemberCnt;              // member counter

    Populate *itsNext;                  // next dataset

public:                                

    Populate(const char *theID,
             const char *theSourceDSN) // constructor
    {
        itsSourceDSN.Set(theSourceDSN); // set DSN
        itsId.Set(theID);
        itsMemberCnt = 0;              // no members known
        itsNext = NULL;                // no NEXT
    }

    ~Populate()                        // destructor

    {

        if (itsNext != NULL)           // if next
            delete itsNext;            // remove it

    }

    const char *GetSourceDSN() { return((const char *) itsSourceDSN); }
    const char *GetID() { return((const char *) itsId); }

    void SetNext(Populate *theNext) { itsNext = theNext; }

    Populate *GetNext() { return(itsNext); }

    void SetMemberCnt(int theCount) { itsMemberCnt = theCount; }
    int GetMemberCnt() { return(itsMemberCnt); }

};                                     
             
static int RewriteExpression(CviStr &theExpStr,
                             WorkflowStep *theStep,
                             VariableList *theVars,
                             CviStr &theRewritten);

//****************************************************************************
// 
// Datasets
// 
//****************************************************************************
class  Dataset                         // dataset
{

private:
    CviStr  itsID;                     // identifier
    CviStr  itsDSN;                    // DSN
    CviStr  itsJCL;                    // JCL components
    CviStr  itsSize;                   // size string
    CviStr  itsTag;                    // tag 
    CviStr  itsTempMember;             // template member where it was allocated
    CviStr  itsCondition;              // condition under which it should be used (new for prompts)

    VariableList itsAttributes;        // attributes

    Dataset *itsNext;                  // next dataset
    Populate *itsPopulate;             // populatioon entries

    int     itsNeeded;                 // is it needed?
    int     itsAlloc;                  // use for allocation?
    int     itsListed;                 // listed?

    char    itsUnitSize;               // unit size for ReqSize

    void   *itsParms[2];               // parameters for thread

    pid_t   itsThread;                 // thread

    int       itsThreadRc;             // thread return code

    unsigned long    itsReqSize;       // required size
    unsigned long    itsMemberCnt;     // anticipated # of members

    FILE  *itsMemCntFP;                // member count file pointer


public:                                

    Dataset(const char *theID,
            const char *theDSN,        // dataset constructor
            const char *theTemplateMember)
    {
        itsDSN.Set(theDSN);            // set DSN
        itsID.Set(theID);              // set ID
        itsTempMember.Set(theTemplateMember); // set template member
        itsNeeded = false;             // not needed
        itsNext = NULL;                // no NEXT
        itsPopulate = NULL;            // no population entries
        itsReqSize = 0;                // no size set yet
        itsMemberCnt = 128;            // assume 128 by default
        itsUnitSize = 'B';             // assume blocks
        itsAlloc = itsNeeded;          // don't use for allocation yet
        itsListed = false;             // not yet listed in DSN_LIST
        itsMemCntFP = NULL;            // no count yet
    }

    ~Dataset()                         // destructor

    {                                  // begin destroy

        if (itsNext != NULL)           // if next

            delete itsNext;            // destroy it

        if (itsPopulate != NULL)       // if population entry

           delete itsPopulate;         // remove

    }                                  // end destroy

    void GetMemberCount(VariableList *theProps)

    {

        int aTotMembers = 0;           // total members


        Populate *aPop = GetPopulate(); // get first population entry

        while (aPop != NULL)           // for all population entries

        {                              // begin process population entry

            CviStr aSrcDSN(aPop->GetSourceDSN());

            if (strlen(aSrcDSN) > 0)   // while good

            {                          // begin process

                MemberList  memberList;        // DSN member list

                TRACE("GETTING MEMBERS FOR %s\n",
                      (const char *) aSrcDSN);

                memberList.GetMembers(aSrcDSN);

                int aMems = (int) memberList;

                aPop->SetMemberCnt(aMems);     // save for later trace

                aTotMembers += aMems;          // add member count
                                       
            };                                 // end process

            aPop = aPop->GetNext();            // next!

        };                                     // end process population entry

        SetMemberCount(aTotMembers);     // set member count

    }

    void AddAttribute(const char *theName,
                      const char *theValue)
    {
        itsAttributes.Add(theName, theValue);
    }

    void CreateAttrCols(Variable *theRow,
                        VariableList *theProps)
    {

        theRow->AddSubVariable         // add 
            (new Variable("TAG",       // TAG
                          GetTag()));
                                       
        theRow->AddSubVariable         // add 
            (new Variable("DSN",       // DSN
                          GetDSN()));

        CviStr aResolvDSN(GetDSN());

        theProps->TranslateJCLValues(aResolvDSN);

        theRow->AddSubVariable         // add 
            (new Variable("RDSN",      // DSN
                          aResolvDSN));

        theRow->AddSubVariable         // add 
            (new Variable("DSNID",     // DSNID
                          GetID()));
                                       
        theRow->AddSubVariable         // add 
            (new Variable("JCL",       // JCL
                          GetJCL()));  

        if (strlen(itsCondition) > 0)
            theRow->AddSubVariable
                (new Variable("VELOCITY_CONDITION",
                          (const char *) itsCondition));

        Variable *aVar = itsAttributes.GetFirst();
        while (aVar != NULL)
        {
            theRow->AddSubVariable
                 (new Variable(aVar->GetName(),
                               aVar->Value()));
            aVar = itsAttributes.GetNext();
        }
    }

    int  GetRC()
    {
        return(itsThreadRc);
    }

    void AddJCL(const char *theJCL)    // add to JCL
    {
        itsJCL.Print("%s\n", theJCL);
    }

    void Tag(const char *theTag)
    {
        itsTag.Set(theTag);            // set tag
    }

    void SetCondition(const char *theCondition)
    {

        CviStr aCond(theCondition);    // temporary condition line

        int aCnt = 0;                  // item counter

        const char *aLinePtr = aCond.Tok("\n"); // get each condition

        while (aLinePtr != NULL)       // process all lines
        {                
            if (strlen(aLinePtr) > 0)

            {

                CviStr aLine(aLinePtr);    // save line value
                CviStr aVelocity;          // velocity format of expression

                RewriteExpression(aLine,
                                  NULL,
                                  &itsAttributes,
                                  aVelocity);

                if (aCnt == 0)

                {

                    if (strlen(itsCondition) != 0)

                        itsCondition.Add(" || ");  // OR other sets of conditions

                    itsCondition.Print("(( %s )",
                                       (const char *) aVelocity);

                }

                else

                    itsCondition.Print(" == false &&\n( %s )",
                                       (const char *) aVelocity);

                aCnt ++;

            }

            aLinePtr = aCond.Tok("\n"); // next!
        }

        if (aCnt > 0)
            itsCondition.Add(" )");

//        itsCondition.Set(theCondition);
    }

    const char *GetCondition()
    {
        return((const char *) itsCondition);
    }

    const char *GetTemplateMember()    // get template member name where this was defined
    {
        return((const char *) itsTempMember);
    }

    const char *GetTag()               // get tag

    {
        return((const char *) itsTag); // return the tag
    }

    void MarkNeeded()                  // mark entry as needed
    {
        itsNeeded = true;              // it is needed

    }

    void MarkAllocate()                // mark allocation needed

    {
        itsNeeded = true;              // must be needed
        itsAlloc = true;               // allocate

    }

    void AddPopulate(const char *theID,
                     const char *theDSN)

    {

        itsNeeded = true;              // needed

        itsAlloc = true;               // assume we want to allocate it

        if (strlen(theDSN) > 0)        // if we have a source name

        {                              // begin add population entry

            Populate *aPop = itsPopulate;

            while (aPop != NULL &&     // while we have entries and they don't match our new DSN
                   strcmp(aPop->GetSourceDSN(),
                          theDSN))

                aPop = aPop->GetNext();// next!

            if (aPop != NULL)          // if found

                theDSN = "";           // use empty DSN

            aPop = new Populate        // allocate object
                        (theID, theDSN);

            aPop->SetNext(itsPopulate);// add it to the
            itsPopulate = aPop;        // list

        }                              // end add population entry

    }

    const char *GenSize()              // generate size

    {                                  // begin GenSize

        itsSize.Reset();               // clear size

        unsigned long aBlkSize = 0;    // no block size yet

        if (aBlkSize == 0)             // if still not set

        {                              // begin locate LRECL in JCL
                                       
            const char *aLRECL =       // get block size
                strstr(itsJCL, "LRECL=");

            if (aLRECL != NULL)        // if OK

                aBlkSize = atol(aLRECL + 6);


        };                             // end locate LRECL in JCL

        if (aBlkSize == 0)             // if not found or not set

        {
            const char *aBSIZE =
                strstr(itsJCL, "BLKSIZE=");// use BLKSIZE

            if (aBSIZE != NULL)        // if found

            {                          // begin use BLKSIZE

                aBlkSize = atol(aBSIZE+8); // grab value

                if (itsUnitSize == 'B')

                   itsReqSize = itsReqSize / aBlkSize;

            };                         // end use BLKSIZE

        }

        if (itsReqSize > 0          && // if size is OK and
            aBlkSize > 0            && // block size is OK
            itsUnitSize == 'B')        // and we have block sizes

        {                              // begin compute size

            itsReqSize = itsReqSize / 100 * 125;

            if (itsReqSize < 100)       // don't let it

                itsReqSize = 100;       // go too low

            int aDirBlks =             // compute
                    itsMemberCnt / 6;  // blocks needed

            if (aDirBlks < 10)         // if low

                 aDirBlks = 10;        // minimum size

            aDirBlks = aDirBlks * 3;   // allocate 3x to allow growth

            itsSize.Print("%d,(%d,%d,%d),,,ROUND",
                          aBlkSize,
                          itsReqSize * 2,
                          itsReqSize * 2,
                          aDirBlks
                          );

        }                              // end compute size

        else                           // otherwise

        if (itsUnitSize == 'T')        // if we have tracks

        {                              // begin use tracks

            int aCyls = itsReqSize / 15; // get # of cylinders needed

            if (aCyls < 1)             // if too low

                aCyls = 1;             // use minimum

            if (aCyls > 3000)          // if too high

                aCyls = 3000;          // cap it

            int aNext = aCyls / 4;     // use appropriate 'next' size of 25%
                                       // (OZI used 25% for secondary)

            if (aNext < 2)             // if too low

                aNext = 2;             // have at least 2

            int aDirBlks =             // compute
                    itsMemberCnt / 6;  // blocks needed

            if (aDirBlks < 20)         // if low

                 aDirBlks = 20;        // minimum size

            aDirBlks = aDirBlks * 3;   // allocate 3x to allow growth

            itsSize.Print("CYL,(%d,%d,%d)",
                          aCyls,
                          aNext,
                          aDirBlks);

        }                              // end use tracks

        else                           // otherwise

        {                              // begin use default

            int aDirBlks =             // compute
                    itsMemberCnt / 6;  // blocks needed

            if (aDirBlks < 20)         // if low

                 aDirBlks = 20;        // minimum size

            itsSize.Print("CYL,(1,2,%d)",
                          aDirBlks);

        };                             // end use default

        itsJCL.Replace("#COMPUTE_SPACE()", // replace with appropriate size
                       itsSize);

        return((const char *) itsSize);

    }                                  // end GenSize

    void Add(Dataset *theDataset)
    {
        if (itsNext != NULL)
            itsNext->Add(theDataset);
        else
            itsNext = theDataset;
    }

    const char *GetID()  { return((const char *) itsID);  }
    const char *GetDSN() { return((const char *) itsDSN); }
    const char *GetJCL() { return((const char *) itsJCL); }

    void SetID(const char *theID) { itsID.Set(theID); }
    void SetDSN(const char *theDSN) { itsDSN.Set(theDSN); }

    void SetAllocate(int theAlloc) { itsAlloc = theAlloc; }

    void MarkListed() { itsListed = true; }

    int IsNeeded() { return(itsNeeded); }

    int IsListed() { return(itsListed); }

    int Allocate() { return(itsAlloc); }

    int TotSize() { return(itsReqSize); }

    int TotMembers() { return(itsMemberCnt); }

    void SetReqSize(unsigned long theSize,
                    char theUnit = 'B') { itsReqSize = theSize; itsUnitSize = theUnit; };

    void SetMemberCount(unsigned long theCount) { itsMemberCnt = theCount; }

    void SetNext(Dataset *theNext) { itsNext = theNext; }

    Dataset *GetNext() { return(itsNext); }

    Populate *GetPopulate() { return(itsPopulate); }

};                                     

Dataset      *itsDatasets = NULL; // dataset list

//****************************************************************************
// 
// Declare some static functions
// 
//****************************************************************************
static void ProcessInternMacros(CviStr &theData,
                                WorkflowStep *theStep,
                                VariableList *thePropVars,
                                char theEscape = false);

//****************************************************************************
// 
// Expression handler w/variable substitution
// 
//****************************************************************************
class  ExpressionWithVars : public Expression
{

public:

    VariableList *itsVars;             // variables

    CviStr itsToken;                   // most recent token

    bool    itsUnknownVar;             // unknown variable

protected:

    virtual
        int ReplaceVariable(const char **theTok)  // process token with RPN engine
    {                                    

        int aRc = 0;                   // assume OK

        CviStr aOrig(*theTok);         // original token value

        itsToken = *theTok;            // copy token

        itsVars->ReplaceVariable(itsToken); // do replacement of variables

        //DPRINT("Replaced %s with %s\n", (const char *) aOrig, (const char *) itsToken);

        itsToken.VelocityUnescape();   // remove Velocity escape sequence

        char *aPtr = (char *) itsToken;     // grab token address for convenience

        if (strlen(aPtr) >= 2       &&      // if it is long enough to be checked
            *aPtr == '\"'           &&      // if it starts with "
            *(aPtr+strlen(aPtr)-1) == '\"') // and ends with"

        {                              // begin remove quotes

            aPtr ++;                   // skip past quote

            *(aPtr+strlen(aPtr)-1) = 0;// remove end quote

            CviStr aTemp = aPtr;       // set temp token

            itsToken = aTemp;          // set new token

        }                              // end remove quotes

        if (!strcmp(aOrig, itsToken) && // if not replaced and
            strlen(aOrig) > 7        && // could be XXX_INSTALL
            (!strcmp(((const char *) aOrig) + 3,
                     "_INSTALL")    ||
             !strcmp(((const char *) aOrig) + 5,
                     "_INSTALL}")))

        {                              // begin replace with N

            itsToken.Set("N");         // set to N

        };                             // end replace with N

        if (!strcmp(aOrig, itsToken) &&
            ( strstr(aOrig, "${") == (const char *) aOrig ||
              strstr(aOrig, "$!{") == (const char *) aOrig) &&
            *(((const char *) aOrig) + strlen(aOrig) - 1) == '}')

        {                              // begin use empty value

               itsToken.Reset();       // use empty value

        };                             // end use empty value

        *theTok = (const char *) itsToken;

        return(aRc);                   // return result

    };

public:

    ExpressionWithVars(const CviStr &theExpression,
                       VariableList *theVars)

    :
    Expression(theExpression)
    {
        itsVars = theVars;

        itsUnknownVar = false;

    }

    void GetVelocity(CviStr &theExpression,
                     bool theAlways = false)
    {
        if (itsUnknownVar || theAlways)
        {
            theExpression.Set(itsRewritten);
            if (!strcmp(itsRewritten, " 1"))
                theExpression.Set("true");
        }
        else
            theExpression.Reset();
    }

    int Process(CviStr &theResult)
    {

        itsUnknownVar = false;         // no unknown variables yet

        int aRc = Expression::Process(theResult);

        if (itsUnknownVar)             // if we have unknown variables

        {                              // begin handle unknowns

            TRACE("Expression %s has unknown variables...\n",
                  (const char *) itsRewritten);

        };                             // end handle unknowns

        return(aRc);                   // return result

    }

};

//****************************************************************************
//
// Function     : GenRowVarName
//
// Description  : Generate row variable name
//
// Parameters   : 1) Target string
//                2) Base variable
//                3) Sub variable
//                4) Row number
//                5) Add braces
//
// Returns      : TRUE if all expressions are true, else FALSE
//
//****************************************************************************
void GenRowVarName(CviStr &theTarget,
                   Variable *theVar,
                   Variable *theColVar,
                   int theRowNum = 0,
                   char theVelocity = false)

{

theTarget.Reset();                     // clear target

if (theVelocity  &&                    // if special rownum variable
    !strcmp(theColVar->GetName(), "rownum"))
    theTarget.Print("%d", theRowNum);  
else
if (theVelocity)                       // if adding braces, set braces u
    theTarget.Print("${ROWVAR_%p}",
                    (void *) theColVar);
else
    theTarget.Print("ROWVAR_%p",       // otherwise, no braces
                    (void *) theColVar);

}

//****************************************************************************
//
// Method       : AddRowVars
//
// Description  : Add row/column variables
//
//****************************************************************************
static void AddColVars(VariableList *theList,
                       Variable *theVar)

{

Variable *aColVar =                    // get "column" variable
    theVar->GetSubVar();      
                               
while (aColVar != NULL)                // for all of them
                               
{                                      // begin add new variable
                               
    CviStr aName;                      // set new name
                               
    GenRowVarName(aName, theVar, aColVar);

    theList->Add(aName,                // add "column" variable
                 aColVar->Value());
                           
    aColVar = aColVar->GetNext();
                               
};                                     // end add new variable
                                       
}

//****************************************************************************
//
// Function     : ValidateValue
//
// Description  : Validate a value
//
// Parameters   : 1) String to validate
//                2) Special characters, if any, to allow. 
//                   Allows all upper/lower case characters by default plus
//                   any special characters
//
// Returns      : TRUE if valid...
//
//****************************************************************************
int ValidateValue(const char *theValue,
                  const char *theExtra = "")

{

int aRc = true;                        // valid so far

const char *aChar = theValue;          // start with first character

char aSrch[2];                         // search string

aSrch[1] = 0;                          // null terminate

while (aRc && *aChar != 0)             // for all characters

{                                      // begin test characters

    *aSrch = *aChar;                   // use character in search

    if (!((*aChar >= 'A' && *aChar <= 'Z') ||
          (*aChar >= 'a' && *aChar <= 'z') ||
          (aChar != theValue && (strstr(theExtra, aSrch) != NULL)))) // cannot start with specials

        aRc = false;                   // no good

    else
        aChar ++;                      // next!

};                                     // end test characters

return(aRc);                           // return result

}


//****************************************************************************
//
// Function     : EvaluateExpression
//
// Description  : Evaluates a list of expressions
//
// Parameters   : 1) Express string (comma-separated expressions)
//                2) Variable list  
//                3) Validation failure reasons
//
// Returns      : TRUE if all expressions are true, else FALSE
//
//****************************************************************************
int EvaluateExpression(CviStr &theExpStr,
                       WorkflowStep *theStep,
                       VariableList *theVars,
                       CviStr &theReason)

{

if (strstr(theExpStr, "#") != NULL)    // if we might have macros

    ProcessInternMacros(theExpStr, theStep, theVars, false); // process any macros


// Performance: Possibly change to static?

ExpressionWithVars anExpr(theExpStr,
                          theVars);    // expression engine

CviStr anAnswer;                       // answer


theReason.Reset();                     // clear out reason

int aRc = anExpr.Process(anAnswer);    // evaluate expression

if (aRc == 0)                          // if valid

    aRc = atoi(anAnswer);              // setup answer

else

    theReason.Print("Error evaluating expression %s",
                     (const char *) theExpStr);


return(aRc);                           // return result

}

//****************************************************************************
//
// Function     : EvaluateExpression
//
// Description  : Evaluates a list of expressions
//
// Parameters   : 1) Express string (comma-separated expressions)
//                2) Variable list  
//                3) Validation failure reasons
//
// Returns      : TRUE if all expressions are true, else FALSE
//
//****************************************************************************
int EvaluateExpression(CviStr &theExpStr,
                       WorkflowStep *theStep,
                       VariableList *theVars,
                       CviStr &theReason,
                       CviStr &theRewritten)

{

if (strstr(theExpStr, "#") != NULL)    // if we might have macros

    ProcessInternMacros(theExpStr, theStep, theVars, false); // process any macros


// Performance: Possibly change to static?

ExpressionWithVars anExpr(theExpStr,
                          theVars);    // expression engine

CviStr anAnswer;                       // answer


theReason.Reset();                     // clear out reason

int aRc = anExpr.Process(anAnswer);    // evaluate expression

if (aRc == 0)                          // if valid

    aRc = atoi(anAnswer);              // setup answer

else

    theReason.Print("Error evaluating expression %s",
                     (const char *) theExpStr);

anExpr.GetVelocity(theRewritten);      // get re-written expression


return(aRc);                           // return result

}

//****************************************************************************
//
// Function     : RewriteExpression
//
// Description  : Rewrites a list of expressions
//
// Parameters   : 1) Express string (comma-separated expressions)
//                2) Variable list  
//                3) Validation failure reasons
//
// Returns      : TRUE if all expressions are true, else FALSE
//
//****************************************************************************
static int RewriteExpression(CviStr &theExpStr,
                             WorkflowStep *theStep,
                             VariableList *theVars,
                             CviStr &theRewritten)

{

if (strstr(theExpStr, "#") != NULL)    // if we might have macros

    ProcessInternMacros(theExpStr, theStep, theVars, false); // process any macros


// Performance: Possibly change to static?

ExpressionWithVars anExpr(theExpStr,
                          theVars);    // expression engine

CviStr anAnswer;                       // answer


int aRc = anExpr.Process(anAnswer);    // evaluate expression

if (aRc == 0)                          // if valid

{

    anExpr.GetVelocity(theRewritten, true);  // get re-written expression

//    if (strlen(theRewritten) == 0)     // if couldn't be rewritten
//
//        theRewritten.Set(theExpStr);   // use original

}


else

{

    theRewritten.Print("Error evaluating expression %s",
                       (const char *) theExpStr);

    aRc = 8;                           // failure

}




return(aRc);                           // return result

}

//****************************************************************************
//
// Function     : ReplaceConditionalVars
//
// Description  : Replace all $!{variable} with empty - assumes var sub has
//                already happened so any remaining $!{var} usage are empty
//                strings
//
// Parameters   : 1) Content to search/replace
//
//****************************************************************************
static void ReplaceConditionalVars(CviStr &theContent)

{

const char *aVar =                     // find 
    strstr(theContent, "$!{");         // variable
                       
while (aVar != NULL)                   // for all variables

{                                      // begin find/replace

    const char *aEnd = strstr(aVar, "}");

    if (aEnd != NULL)                  // begin check value

    {                                  // begin add 

        CviStr aVarStr;                // variable string

        aVarStr.Print("%*.*s",
                      (int) (aEnd - aVar)+1,
                      (int) (aEnd - aVar)+1,
                      aVar);

//        TRACE("Replacing %s for empty string\n",
//                               (const char *) aVarStr);
                              
        long aPos =                // adjust content may change address, so save our position
            (long) (aVar - (const char *) theContent);      

        char aRepl = true;             // assume will do replace

        const char *aClose =           // find closing
            strstr(aVar, "]]#");       // escape sequence

        if (aClose != NULL)            // if close found
        {                              // begin locate open
            const char *aOpen = 
                strstr(aVar, "#[[");   // locate open

            if (aOpen == NULL       || // if not found or
                aOpen > aClose)        // it is after close

            {                          // begin must be inside escape sequence
                                       
                TRACE("Refusing to replace inside escape sequence.%*.*s\n",
                                       (int) (aClose - aVar) + 3,
                                       (int) (aClose - aVar) + 3,
                                       aVar);

                aRepl = false;         // do not replace

            }                          // end must be inside escape sequence

        }                              // end locate open

        if (aRepl)                     // if it should be replaced
        {                              // begin replace
           theContent.Replace(aVarStr, "");
           aVar = (const char *) theContent + aPos;  // set new position
        }                              // end replace
        else
        if (aClose != NULL)            // jump to close
           aVar = aClose;              // if available

    }                                  // end add

    aVar = strstr(aVar+1, "$!{");        // find next one

};                                     // end find/replace

}

//****************************************************************************
//
// Method       : AddList
//
// Description  : Add to comma-delimited list
//
// Parameters   : 1) String list
//                2) List to add
//
//****************************************************************************
static int AddList(      CviStr &theList,
                   const CviStr &theItem)

{                                      // begin AddList

int aAdded = false;                    // assume it hasn't been added yet

CviStr aList = theList;                // copy list

const char *aItem = aList.Tok(",");    // get item from list

while (aItem != NULL)                  // for all tokens

{                                      // begin check item

    if (!strcmp(aItem, theItem))       // if it matches

        break;                         // stop

    aItem = aList.Tok(",");            // end check item

};                                     // end merge dependency

if (aItem == NULL)                     // if not found

{                                      // begin add

    aAdded = true;                     // we added it

    if (strlen(theList) == 0)          // if empty

        theList.Set(theItem);          // set as first
    
    else                               // otherwise

        theList.Print(",%s",           // add as-is
                      (const char *) theItem);

};                                     // end add


return(aAdded);                        // added it

};                                     // end AddList

//****************************************************************************
//
// Method       : InList
//
// Description  : Is item in list?
//
// Parameters   : 1) String list
//                2) Item to check
//
//****************************************************************************
static int InList(      CviStr &theList,
                  const CviStr &theItem,
                        bool   thePrefix = false)

{                                      // begin AddList

int aIn = false;                       // not in yet

CviStr aList = theList;                // copy list

const char *aItem = aList.Tok(",");    // get item from list

while (aItem != NULL)                  // for all tokens

{                                      // begin check item

    if (thePrefix &&                   // if prefix mode and
        strlen(aItem) >= strlen(theItem) &&
        !strcmp(theItem, aItem + strlen(aItem) - strlen(theItem)))

        break;                         // we found it

    else                               // otherwise

    if (!strcmp(aItem, theItem))       // if it perfectly matches

        break;                         // stop

    aItem = aList.Tok(",");            // end check item

};                                     // end merge dependency

if (aItem != NULL)                     // if found

    aIn = true;                        // it is in the list

return(aIn);                           // added it

};                                     // end AddList

//****************************************************************************
//
// Function     : RemoveUnknownColumns
//
// Description  : Remove unknown columns in FOR_EACH iteration
//                Rather than remove, replace with ${EMPTY_STR} so
//                it doesn't break conditional expressions that might exist.
//
// Parameters   : 1) String to check
//                2) Row variable name/alias
//
//****************************************************************************
static void RemoveUnknownColumns(CviStr &theStr,
                                 const CviStr &theVar)


{                                      // begin RemoveUnknownColumns

const char *aLeftover =                // check if we have leftover vars
    strstr(theStr, theVar);

while (aLeftover != NULL)              // leftovers available?
{
    const char *aName = aLeftover;     // setup to find end of name

    int anOffset =                     // preserve offset in string
        (int)(aName - (const char *) theStr);   
                                       
    while (*aName != 0 &&              // while not
           *aName != ' ' &&            // at the end
           *aName != '\n' &&           // of the
           *aName != '[')              // list
       aName ++;

    if (*aName == '[')                 // if found column name

        while (*aName != 0 &&          // find close of
               *aName != ']')          // column
            aName++;

    if (*aName == ']')                 // if end column name found

    {                                  // begin replace

        TRACE("Replacing unknown row column: %.*s\n",
                               (int) (aName-aLeftover) +1,
                               aLeftover);

        theStr.Replace(aLeftover,
                       (int)(aName - aLeftover) + 1,
                       "${EMPTY_STR}");

        aLeftover = ((const char *) theStr) + anOffset;

    }                                  // end replace

    aLeftover = strstr(aLeftover + 1, theVar);   

}

};                                     // end RemoveUnknownColumns

//****************************************************************************
// 
// Class        : WorkflowIntMacro
//
// Description  : Does standard processing required for internal macros
// 
//****************************************************************************
class WorkflowIntMacro
{

public:

    CviStr  itsName;                   // macro name
    CviStr  itsPrefix;                 // macro prefix
    VariableList *itsPropVars;         // property variables
    WorkflowStep *itsStep;             // step this is acting upon

    bool    itsEscape;                 // escape replacement values?
    bool    itsProcParms;              // process parameters first?
    bool    itsCache;                  // cache results?

    int     itsCol;                    // column number where macro starts

    static  WorkflowIntMacro *itsTop;  // top macro list
    WorkflowIntMacro *itsNext;         // next item

public:

    WorkflowIntMacro(const char *theName,
                     bool theLink = true) // link to main list?
    {
      itsStep = NULL;                  // no step yet
      itsCache = false;                // do not cache results
      itsEscape = true;                // assume we will escape results with #[[ ]]#
      itsName.Set(theName);            // set macro name
      itsPrefix.Print("#%s(",          // set macro prefix
                      theName);
      itsPropVars = NULL;              // no variables yet
      itsNext = NULL;                  // clear
      itsCol = 0;                      // no column known
      itsProcParms = true;             // assume we will process macros on parms first

      if (theLink)                     // if it should be linked
      {                                // begin link together with others
          if (itsTop == NULL)          // if no top
              itsTop = this;           // we are top
          else                         // otherwise
          {                            // add next
              itsNext = itsTop;        // our next is current list's second item
              itsTop = this;           // we are now second
          }                            // end add next
      }                                // end link together with others

    };

    virtual void AddCacheParms(CviStr &theParms)
    {

    }

    int Process(CviStr &theContent,    // process macro
                WorkflowStep *theStep, // workflow step
                VariableList *thePropVars,
                char theEscape = false)
                
    {

        itsPropVars = thePropVars;     // save variable list

        int aRc = 0;                   // assume all is well

        if (aRc <= 4 &&                // if OK so far and
            itsNext != NULL)           // have another item?
        {
            aRc = itsNext->Process(theContent, theStep, thePropVars, theEscape);

        }

        itsCol = 0;                    // no column known

        const char *aData =            // find first macro
            strstr(theContent,         // prefix
                   itsPrefix);

        while (aRc == 0 &&             // while OK and
               aData != NULL)          // for all prefixes

        {                              // end process prefix

            const char *aChar = aData; // set first character for column search

            itsCol = 0;                // start with nothing

            while (aChar > (const char *) theContent && // find column
                   *aChar != '\n')     // where this
            {                          // macro
               itsCol ++;              // starts
               aChar --;               // to pad
            };                         // JCL

            if (itsCol > 0)            // if positive column

                itsCol --;             // make relative to 0

            int aIdx = (int) (aData - (const char *) theContent);

            const char *aEnd =            // get
                aData + strlen(itsPrefix);// to start of parameters

            const char *aStParms = aEnd; // start of parms

            char aAlone = 0;           // assume not alone on a line by itself

            aEnd = LocateEndParm(aStParms,
                                 itsPrefix,
                                 aData,
                                 theStep);

            if (aEnd == NULL)          // did it fail?

            {                          // begin error
                                       
                aRc = 8;               // fail

                break;                 // end

            };                         // end error

            if (*aEnd != 0)            // if we found the end

            {                          // begin process macro

                if (*aEnd != 0 &&      // if call is all alone on the line
                    *(aEnd+1) == '\n' &&
                    aData > (const char *) theContent &&
                    (*(aData-1) == '\n' ||
                     aData-1 == (const char *) theContent))

                    aAlone = 1;        // it is by itself on this line

                CviStr aFullCall;      // full call
                CviStr aParms;         // parameters

                aFullCall.Print        // setup full call
                          ("%*.*s",    
                           (int) (aEnd-aData)+1,
                           (int) (aEnd-aData)+1,
                           aData);

                TRACE("Found %s\n", (const char *) aFullCall);

                aParms.Print("%*.*s",  // get all parameters
                             (int) (aEnd-aStParms),
                             (int) (aEnd-aStParms),
                             aStParms);

                MacroParms *aParmList = NULL;

                CviStr aReplaceData;   // replacement data

                aParmList =            // generate parameter list
                    GenParmList(aParms);

                aParms.Reset();        // reset parms

                MacroParms *aParmItem = aParmList;

                if (!itsProcParms) aParmItem = NULL; // do not process parms if false

                while (aParmItem != NULL)    // handle nested parms and velocity values
                {  
                   //Nested macros
                   if (strstr(aParmItem->itsData, "#") != NULL)
                        itsTop->Process(aParmItem->itsData, theStep, thePropVars, false);

                   if (thePropVars != NULL)
                       thePropVars->ReplaceWithValues(aParmItem->itsData, false);

                   aParmItem->itsData.VelocityUnescape(); 

                   TRACE("Parm: %s\n", (const char *) aParmItem->itsData);

                   if (itsCache)
                   {
                       aParms.Add("-:-");  // add unlkely separators between parms
                       aParms.Add(aParmItem->itsData);
                   };

                   aParmItem = aParmItem->itsNext;
                };

                if (itsCache)          // possibly add other cached parm data
                    AddCacheParms(aParms);

                itsStep = theStep;     // make step we are working against accessible

                if (!itsCache  ||      // if caching is allow and
                    aMacroCache.GetAnswer(this,   
                                          aParms,
                                          aReplaceData) != 0)

                {                      // begin cache miss

                    //if (itsCache)
                    //fprintf(stderr, "Cache miss for %s (%s)\n",
                    //        (const char *) itsName,
                    //        (const char *) aParms);

                    // Call to process parameters
                    aRc = Evaluate(aParmList, aReplaceData);

                    if (aRc == 0 && itsCache)

                    {                  // begin cache result

                        aMacroCache.AddAnswer(this,
                                              aParms,
                                              aReplaceData);

                    }                  // end cache result

                }                      // end cache miss
                //else
                //{
                //    fprintf(stderr, "Cache hit for %s (%s)\n",
                //            (const char *) itsName,
                //            (const char *) aParms);
                //
                //};

                if (aParmList != NULL) // if OK
                    delete aParmList;  // delete

                //
                // ADD ESCAPE SEQUENCE
                //
                if (itsEscape && theEscape && strlen(aReplaceData) > 0)
                {
                    aReplaceData.VelocityEscape(); // escape the string
                }
                    
                TRACE("ReplaceData: %s\n", (const char *) aReplaceData);
                TRACE("Replacing into %*.*s\n", strlen(aFullCall), strlen(aFullCall), aData);

                int aCallLen = strlen(aFullCall);
                if (aAlone && strlen(aReplaceData) == 0)
                    aCallLen ++;

                if (*(aData + aCallLen) == '\n')
                    aReplaceData += "$!{NL}";

                aIdx = ReplaceContent(theContent,
                                      aData,
                                      aCallLen,
                                      aReplaceData);

                if (aIdx < 0) aIdx = 0; // start over if offset is not known

                aData = strstr((const char *) theContent + aIdx, itsPrefix);

            }                          // end process macro

            else                       // otherwise

            {                          // begin failure

                aRc = 8;               // failure

                aData = strstr((const char *) theContent + aIdx + 1, itsPrefix);

            };                         // end failure


        };                             // begin process prefix

        return(aRc);                   // return the result

    }                                  // end process tag

    virtual int Evaluate(MacroParms *theParmList,
                         CviStr &theNewData)
    {
        return(0);
    }

    virtual int ReplaceContent(CviStr &theContent,
                               const char * theOffset,
                               int theLen,
                               const char *theNewData)
    {
        int anOffset = (theOffset - (const char *) theContent);
        theContent.Replace(theOffset, theLen, theNewData);
        return(anOffset + strlen(theNewData)); 
    }

};

WorkflowIntMacro *WorkflowIntMacro::itsTop = NULL;

//****************************************************************************
// 
// Internal Macro : Unique()
//
// Description    : Determine if parameters passed are unique, meaning
//                  the parameters have not been observed before.
//                  This is NOT useful for main macro processing but IS
//                  useful for #FOR_EACH processing and will be instantiated
//                  for it. 
// 
//****************************************************************************
class MacroUnique : public WorkflowIntMacro
{
private:
    CviStr itsUniqueList;              // unique string list

    CviStr itsColList;                 // column list

public:
    MacroUnique() : WorkflowIntMacro("UNIQUE", false) {};  // do NOT put on macro list
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        CviStr aStr;                   // check string

        while (theParmList != NULL)    // while parms OK
        {                              // begin build string with all parms
            aStr.Print("%s%c",         // add to string, separating by character 0x01
                       (const char *) theParmList->itsData,
                       (char) 1);

            theParmList = theParmList->itsNext;
        }                              // end build string with all parms

        //GetTaskCviPgm()->Print("adding %s\n", (const char *) aStr);
        //GetTaskCviPgm()->Print("Checking against %s\n", (const char *) itsUniqueList);

        if (AddList(itsUniqueList,     // if it hasn't been seen
                    aStr))             // before
            theAnswer.Set("1");        // set true
        else                           // otherwise
            theAnswer.Set("0");        // set false

        return(aRc);                   // return if evaluation was successful
    }
    void Reset() {
        itsUniqueList.Reset();         // clear list
    }

};

//****************************************************************************
// 
// Internal Macro : RowCnt()
//
// Description    : Returns number of rows in a table
// 
//****************************************************************************
class MacroRowCnt : public WorkflowIntMacro
{
public:
    MacroRowCnt() : WorkflowIntMacro("ROWCNT") 
        { itsProcParms = false; };     // we must run before our parameters are translated

    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;                   // OK so far

        int aCnt = 0;                  // counter

        theAnswer.Set("0");            // default answer

        Variable *aVar = NULL;         // variable

        CviStr aCond("1");             // condition
        CviStr aVarAlias;              // alias

        if (theParmList == NULL)       // any parms?
        {                              // begin error
            theAnswer.Set("#ROWCNT has too few parameters.");
            return 8;                  // failure
        }                              // end error

        if (theParmList != NULL)       // parm 1 - table
        {

            CviStr aVarName(theParmList->itsData);
            CviStr aTmp;               // tmp


            const char *aTok = aVarName.Tok(":");

            aVarAlias.Set(aTok);       // assume no alias

            aVar = itsPropVars->GetVariable(aTok);


            aTok = aVarName.Tok("");   // get remainder

            if (aTok != NULL)          // if found

                aVarAlias.Set(aTok);   // use alias

            aTmp.Print("${%s}", (const char *) aVarAlias);

            aVarAlias.Set(aTmp);       // set in variable form

            theParmList = theParmList->itsNext;
        }

        if (theParmList != NULL)       // parm 2 - condition
        {
            aCond.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        }

        if (aVar != NULL)              // if variable is OK

        {                              // begin process rows

            int aRowNum = 1;           // row counter

            MacroUnique aUnique;       // uniqueness macro

            Variable *aRow = aVar->GetSubVar();

            while (aRow != NULL)       // for all rows

            {                          // begin process row

                CviStr aRowCond(aCond);// condition

                Variable *aCol = NULL; // column iteration

                if (strlen(aCond) > 0) // if we need to iterate columns

                    aCol = aRow->GetSubVar();

                while (aCol != NULL)   // for all columns

                {                      // begin process columns

                    CviStr aFind;      // find
                    CviStr aRepl;      // replace

                    aFind.Print("%s[%s]",
                                (const char *) aVarAlias,
                                (const char *) aCol->GetName());

                    GenRowVarName(aRepl, aVar, aCol, aRowNum, true);

                    aRowCond.Replace(aFind, aRepl);

                    aCol = aCol->GetNext(); // next!

                };                     // end process columns

                CviStr aReason;        // reason

                aUnique.Process(aRowCond, itsStep, itsPropVars, false);

                if (strlen(aRowCond) == 0 ||
                    EvaluateExpression(aRowCond,
                                       itsStep,
                                       itsPropVars,
                                       aReason))

                    aCnt ++;           // bump count

                aRowNum ++;            // bump row count

                aRow = aRow->GetNext();

            };                         // end process row

        };                             // end process rows

        theAnswer.Print("%d", aCnt);   // setup answer

        return(aRc);                   // we were OK

    }
} MacroRowCnt;

//****************************************************************************
// 
// Internal Macro : IsIn()
//
// Description    : Checks if string 1 is equal to any other strings passed.
// 
//****************************************************************************
class MacroIsIn : public WorkflowIntMacro
{
public:
    MacroIsIn() : WorkflowIntMacro("ISIN") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;                   // OK so far
        theAnswer.Set("0");
        CviStr aStr;                   // string to check
        if (theParmList == NULL)       // any parms?
        {                              // begin error
            theAnswer.Set("#ISIN has too few parameters.");
            return 8;                  // failure
        }                              // end error
        if (theParmList != NULL)
        {
            aStr.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        }

        while (!strcmp(theAnswer, "0") && // while false and
               theParmList != NULL)    // have more parms

        {                              // begin set value

            TRACE("Macro %s: checking %s and %s\n",
                                   (const char *) itsName,
                                   (const char *) aStr,
                                   (const char *) theParmList->itsData);

            if (!strcmp(aStr, theParmList->itsData))   // if they match

               theAnswer.Set("1");    // indicate we match

            theParmList = theParmList->itsNext;

        }                              // end set value
        return(aRc);                   // we were OK

    }
} MacroInstIsIn;

//****************************************************************************
// 
// Internal Macro : GenDD()
//
// Description    : Generates DD concatenation list
// 
//****************************************************************************
class GenDD : public WorkflowIntMacro
{                           
public:
    GenDD(const char *theName = "GEN_DD") : WorkflowIntMacro(theName)
    { 
       itsCache = true; 
       itsEscape=false;
    };

    virtual void AddCacheParms(CviStr &theParms)
    {
        theParms.Print("\n%d", itsCol);
    }

    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;                   // OK so far
        theAnswer.Set("");             // clear out answer

        CviStr aScope;                 // scope to use - default is product only (no scope)
        CviStr aReplaceList;           // replace string list
        CviStr aDupe;                  // duplicate check string
        CviStr aDD;                    // DD name
        CviStr aEnclose;               // enclose
        CviStr aTemplate;              // template
        CviStr aPad;                   // padding
        CviStr aExclude;               // exclude items
        CviStr aPrefix;                // prefix
        CviStr aSource("DSNID");       // default DSN source
        CviStr aSourceList("DSN");     // source list

        bool   aDDSet = false;         // not yet set


        if (strstr(itsName, "CLIST") != NULL)
            aDDSet = true;             // DD not needed (pretend it has been done)

        if (theParmList == NULL)       // any parms?
        {                              // begin error
            theAnswer.Print("#%s has too few parameters.", (const char *) itsName);
            return 8;                  // failure
        }                              // end error

        while (theParmList != NULL)    // for all parameters

        {                              // begin process parms

            CviStr aOpt(theParmList->itsData);

            if (strstr(aOpt, "="))     // if it contains a value
            {                          // begin process flags
                CviStr aName;          // option name
                CviStr aValue;         // option value


                aName.Set(aOpt.Parm("=")); // get name
                aValue.Set(aOpt.Parm("="));// get value


                aValue.Replace("&amp;", "&");    // translate
                aValue.Replace("&gt;", ">");     // special
                aValue.Replace("&lt;", "<");     // words
                aValue.Replace("&quot;", "\"");  // that escape
                aValue.Replace("&apos;", "'");   // stuff

                if (!strcmp(aName,     // got a template?
                            "TEMPLATE"))

                {                      // begin grab template


                    aTemplate = aValue;

                }                      // end grab template

                else

                if (!strcmp(aName,     // if ENCLOSE
                            "ENCLOSE"))

                {                      // begin ENCLOSE

                    aEnclose = aValue; // use value for ENCLOSE

                }                      // end ENCLOSE

                else                   // otherwise

                if (!strcmp(aName,     // if PREFIX
                            "PREFIX"))

                {                      // begin PREFIX

                   aPrefix = aValue;

                }                      // end PREFIX

                else                   // otherwise

                if (!strcmp(aName,     // if SCOPE
                            "SCOPE"))  

                {                      // begin set scope

                    aScope.Set(aValue); // set scope

                    if (strcmp(aScope, "ALL") &&
                        strlen(aScope) > 0)

                        aScope += "-"; // prefix scope

                    TRACE("Default Scope: %s\n",
                                           (const char *) aScope);

                }                      // end set scope

                else                   // otherwise

                if (!strcmp(aName,     // if POPID
                            "SOURCE"))  

                {                      // begin set to use population IDs

                    if (!strcmp(aValue, "POP"))
                    {
                        aSource.Set("POPID");   // set to use population IDs
                        aSourceList.Set("POP"); // set to use population list
                    }
                    else
                    if (!strcmp(aValue, "POPTAG"))
                    {
                        aSource.Set("TAGS");    // set to use tags
                        aSourceList.Set("POP"); // set to use population list
                    }
                    else
                    if (!strcmp(aValue, "DSN"))
                    {
                        aSource.Set("DSNID");   // set to use DSN ID
                        aSourceList.Set("DSN"); // set to use dataset list
                    }
                    else
                    if (!strcmp(aValue, "DSNTAG"))
                    {
                        aSource.Set("TAG");     // set to use DSN tags
                        aSourceList.Set("DSN"); // set to use dataset list
                    }
                    else
                    {
                        GetTaskCviPgm()->Print("#%s: Bad SOURCE value %s\n",
                                               (const char *) itsName,
                                               (const char *) aValue);
                        if (itsStep != NULL)
                            GetTaskCviPgm()->Print("Template %s\n",
                                                   (const char *) itsStep->GetName());
                        aRc = 8;
                    }
                    TRACE("Setting source list %s, col %s\n",
                                           (const char *) aSourceList,
                                           (const char *) aSource);


                }                      // end set scope

                else                   // otherwise

                if (!strcmp(aName,     // if REPLACE
                            "REPLACE"))

                {                      // begin REPLACE

                    CviStr aParm(aValue);

                    if (*((const char *) aParm) == '[')
                    {
                        aParm.Replace("[", "");
                        aParm.Replace("]", "");
                    }

                    aReplaceList.Set(aParm);

                }                      // end REPLACE

                else                   // otherwise

                {                      // begin error

                    GetTaskCviPgm()->Print("#%s: Bad option %s\n",
                                           (const char *) itsName,
                                           (const char *) aOpt);

                    if (itsStep != NULL)

                        GetTaskCviPgm()->Print("Template %s\n",
                                               (const char *) itsStep->GetName());

                    GetTaskCviPgm()->Print("Content:\n%s\n",
                                           (const char *) aName);

                    aRc = 8;           // error!

                };                     // end error

            }                          // end process flags

            else                       // if not any of the above

            if (*(const char *) aOpt == '!')

            {                          // begin exclude

                AddList(aExclude, ((const char *) aOpt)+1);

            }                          // end exclude

            else

            if (!aDDSet)               // DD set?

            {                          // begin set DD

                aDD.Set(aOpt);         // set DD

                aDDSet = true;         // mark as set

            }                          // end set DD

            else                       // otherwise

            {                          // begin process datasets

                if (strlen(aOpt) > 0 && // if doesn't look like it is in DSN form yet...
                    strstr(aOpt, ".") == NULL)

                {                       // begin locate LLQ

                    CviStr aLLQ(aOpt); // set LLQ to find

                    aOpt.Reset();      // clear out

                    TRACE("Searching for DDs with ID ending in %s\n", (const char *) aLLQ);

                    Variable *aListVar = NULL;

                    CviStr aListName;
                    CviStr aIdName;
                    CviStr aScopeTest(aScope);


                    if (strlen(aScope) != 0 &&
                        strcmp(aScope, "ALL"))

                        aListName.Print("%s%s_LIST",
                                        (const char *) aScope,
                                        (const char *) aSourceList);

                    else

                    if (!strcmp(aScope, "ALL"))

                        aListName.Print("INFR-%s_LIST",
                                        (const char *) aSourceList);

                    else

                        aListName.Print("%s_LIST",
                                        (const char *) aSourceList);

                    aIdName.Set(aSource);

                    aListVar = itsPropVars->GetVariable(aListName, true);

                    TRACE("Processing list %s (%p)\n",
                                           (const char *) aListName,
                                           aListVar);

                    Variable *aRow = NULL;
                    
                    if (aListVar != NULL)
                    {
                        aRow = aListVar->GetSubVar();
                        TRACE("Row %p\n", aRow);
                    }

                    if (aRow == NULL && // if nothing found in this scope
                        strstr(aListName, "-") != NULL) 

                    {                  // begin unscope

                        aListName.Reset();

                        aListName.Print("%s_LIST",
                                        (const char *) aSourceList);

                        aListVar = itsPropVars->GetVariable(aListName);

                        TRACE("Processing list %s (%p)\n",
                                               (const char *) aListName,
                                               aListVar);

                        aScopeTest.Reset();

                        if (aListVar != NULL)

                            aRow = aListVar->GetSubVar();

                    };                 // end unscope

                    bool aFound = false;   // assume not found

                    while (aRow != NULL)  // for all rows

                    {                     // being process row

                        const char *aId = NULL;

                        Variable *aIdCol = aRow->FindSubVar(aIdName);
                        Variable *aDSNCol = aRow->FindSubVar("DSN");

                        if (aDSNCol == NULL)
                            aIdCol = NULL;      // don't process if things aren't right

                        if (aIdCol != NULL)
                            aId = aIdCol->Value();

                        if (aId != NULL)
                            TRACE("Checking ID %s\n",
                                   (const char *) aId);

                        CviStr aIdStr(aId);

                        if (aId != NULL &&
                            InList(aIdStr, aLLQ, true))

                        {                       // begin match


                            CviStr aDupeItem;

                            aDupeItem.Print(",%s,",
                                            (const char *) aDSNCol->Value());

                            if (strlen(aExclude) != 0)

                            {

                                CviStr aList(aExclude);
                                const char *aTok = aList.Tok(",");

                                while (aTok != NULL &&
                                       aId != NULL)
                                {

                                    if (strlen(aTok) > 0 &&
                                        strlen(aId) >= strlen(aTok) &&
                                        !strcmp(aTok,
                                                aId+strlen(aId)-strlen(aTok)))

                                    {

                                        TRACE("Excluding %s\n",
                                                               (const char *) aId);

                                        aId = NULL;

                                    }

                                    aTok = aList.Tok(",");
                                }

                            }

                            if (aId != NULL &&
                                aDSNCol != NULL)

                            {

                                aFound = true;

                                TRACE("Using %s\n",
                                                       (const char *) aDSNCol->Value());

                                if (strlen(aOpt) != 0)
                                    aOpt.Print(",%s", (const char *) aDSNCol->Value());
                                else
                                    aOpt.Print("%s", (const char *) aDSNCol->Value());

                            }

                        }              // end match


                        aRow = aRow->GetNext(); // next!

                        if (aRow == NULL     &&      // not
                            (!aFound || !strcmp(aScope, "ALL")) && // not found or want all
                            strstr(aListVar->GetName(), "-") != NULL &&
                            strlen(aScopeTest) > 0)  // using scope

                        {                  // begin search again without scope

                            aListName.Reset();

                            aListName.Print("%s_LIST",
                                            (const char *) aSourceList);

                            TRACE("Processing list %s\n",
                                                   (const char *) aListName);

                            aListVar = itsPropVars->GetVariable(aListName);

                            aScopeTest.Reset();

                            if (aListVar != NULL)

                                aRow = aListVar->GetSubVar();

                        };                 // end search again without scope

                    };                    // end process row

                };                 // end locate LLQ

                const char *aDSN = aOpt.Tok(",");  // get datasets

                while (aDSN != NULL)    // while OK

                {                      // begin process dataset

                    CviStr aDupeItem;  // dupe item check

                    aDupeItem.Print(",%s,", // setup check string
                                    (const char *) aDSN);

                    if (strstr(aDupe,  // if not duplicated
                               aDupeItem) == NULL)

                    {                  // begin use 

                        CviStr aDDLine;// output line

                        aDupe.Add(aDupeItem); // add to dupe list

                        CviStr aDSNSub(aDSN);

                        CviStr aReplacements(aReplaceList);


                        const char *anEntry = aReplacements.Tok(",");

                        while (anEntry != NULL) // process all replacements

                        {                       // begin process replacement

                            CviStr anItem(anEntry);

                            const char *aFindPtr = anItem.Tok(":");

                            if (aFindPtr != NULL)
                            {

                                while (*aFindPtr != 0 &&
                                       (*aFindPtr == ' ' ||
                                        *aFindPtr == '\n'))
                                    aFindPtr ++;

                                CviStr aFind(aFindPtr);

                                const char *aReplacePtr = anItem.Tok("");

                                if (aReplacePtr != NULL)
                                {

                                    while (*aReplacePtr != 0 &&
                                           (*aReplacePtr == ' ' ||
                                            *aReplacePtr == '\n'))
                                        aReplacePtr ++;

                                    CviStr aRepl(aReplacePtr);

                                    aFind.TrimWhitespace();
                                    aRepl.TrimWhitespace();

//                                    TRACE("REPL %s with %s\n",
//                                                           (const char *) aFind,
//                                                           (const char *) aRepl);

                                    aDSNSub.Replace(aFind,
                                                    aRepl);
                                }


                            }

                            anEntry = aReplacements.Tok(",");

                        };                      // end process replacement

                        if (strlen(aTemplate) == 0)

                            GenLine(aDDLine, aDD, aDSNSub, aPad, aEnclose, aPrefix);

                        else

                            GenFromTemp(aDDLine, aTemplate, aDD, aDSNSub, aPad, aEnclose, aPrefix);

                        if (strlen(aPad) == 0) // setup padding for line 2 on

                            aPad.Print("%*.*s",
                                       itsCol, itsCol, " ");

                        if (strlen(theAnswer) > 0)
                            theAnswer.Add("\n"); // add newline only if needed

                        theAnswer.Add(aDDLine);

                        aDD.Reset();   // reset

                    };                 // end use

                    aDSN = aOpt.Tok(",");// next!

                };                     // end process dataset

            };                         // end process datasets

            theParmList = theParmList->itsNext; // next!

        };                             // end process parms


//        if (itsStep != NULL)
//            TRACE("Template %s\n", (const char *) itsStep->GetName());
//
//        TRACE("%s Column %d Result:\n%s\n",
//                               (const char *) itsName,
//                               itsCol,
//                               (const char *) theAnswer);

        return(aRc);                   // we were OK

    }

    virtual void GenLine(CviStr &theLine,
                         const char *theDD,
                         const char *theDSN,
                         const char *thePad,
                         const char *theEnclose,
                         const char *thePrefix)

    {

        theLine.Print("%s//%-8.8s DD DSN=%s,\n",
                      (const char *) thePrefix,
                      (const char *) theDD,
                      (const char *) theDSN);

        theLine.Print("%s//            DISP=SHR",
                      (const char *) thePrefix);


    }

    virtual void GenFromTemp(CviStr &theLine,
                             CviStr &theTemplate,
                             const char *theDD,
                             const char *theDSN,
                             const char *thePad,
                             const char *theEnclose,
                             const char *thePrefix)

    {

        theLine.Print("%s%s",
                      (const char *) thePrefix,
                      (const char *) thePad);

        CviStr aLine(theTemplate);

        aLine.Replace("${DSN}", theDSN);
        aLine.Replace("${DD}", theDD);

        theLine += aLine;

    }

} MacroGenDD;

//****************************************************************************
// 
// Internal Macro : GenCListDD()
//
// Description    : Generates DSN concatenation list for CLIST
// 
//****************************************************************************
class GenCListDD : public GenDD
{
public:
    GenCListDD() : GenDD("GEN_CLIST_DD")
    {
    }

    virtual void GenLine(CviStr &theLine,
                         const char *theDD,
                         const char *theDSN,
                         const char *thePad,
                         const char *theEnclose,
                         const char *thePrefix)
    {

        CviStr aRevEnc(theEnclose);


        char *aRev = aRevEnc + strlen(aRevEnc) - 1;
        const char *aFwd = theEnclose;

        while (*aFwd != 0)
        {
            *aRev = *aFwd;
            aFwd++;
            aRev--;
        }

        CviStr aLineEnd(" +");

        theLine.Print("%s%s%s%s%s%s",
                      (const char *) thePrefix,
                      (const char *) thePad,
                      (const char *) theEnclose,
                      (const char *) theDSN,
                      (const char *) aRevEnc,
                      (const char *) aLineEnd);

    }

} MacroGenCLISTDD;

//****************************************************************************
// 
// Internal Macro : Compute()
//
// Description    : Evaluate expressions
// 
//****************************************************************************
class MacroCompute : public WorkflowIntMacro
{
public:
    MacroCompute() : WorkflowIntMacro("COMPUTE")
    {
       itsEscape=false;
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;
        while (theParmList != NULL)
        {
            theParmList->itsData.Replace("\n",""); // remove newlines

            ExpressionWithVars anEval      // expression evaluator
                (theParmList->itsData,     // expression
                 itsPropVars);             // variables

            if (anEval.Process(theAnswer) != 0) // get answer
            {
                theAnswer.Set("!ERROR EVALUATING EXPRESSION!");
                aRc = 8;
            }

            theParmList = theParmList->itsNext;

            if (theParmList != NULL)
                theAnswer.Add(",");        // add comma if more to evaluate

        }
        return(aRc);
    }
} MacroInstCompute;

//****************************************************************************
// 
// Internal Macro : Max()
//
// Description    : Return maximum of passed parameters
// 
//****************************************************************************
class MacroMax : public WorkflowIntMacro
{
public:
    MacroMax() : WorkflowIntMacro("MAX")
    {
       itsEscape=false;
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        if (theParmList != NULL)
        {
            theAnswer.Print("%s",
                            (const char *) theParmList->itsData);
            theParmList = theParmList->itsNext;
        }

        while (theParmList != NULL)
        {
            
            if (IsNumber(theAnswer) &&
                IsNumber(theParmList->itsData) &&
                atol(theParmList->itsData) > 
                         atol(theAnswer))

                theAnswer.Set(theParmList->itsData);

            else

                if (strcmp(theParmList->itsData,
                           theAnswer) > 0)
                    theAnswer.Set(theParmList->itsData);

            theParmList = theParmList->itsNext;

        }
        return(aRc);
    }
} MacroInstMax;

//****************************************************************************
// 
// Internal Macro : Min()
//
// Description    : Return minimum of passed parameters
// 
//****************************************************************************
class MacroMin : public WorkflowIntMacro
{
public:
    MacroMin() : WorkflowIntMacro("MIN")
    {
       itsEscape=false;
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        if (theParmList != NULL)
        {
            theAnswer.Print("%s",
                            (const char *) theParmList->itsData);
            theParmList = theParmList->itsNext;
        }

        while (theParmList != NULL)
        {
            
            if (IsNumber(theAnswer) &&
                IsNumber(theParmList->itsData) &&
                atol(theParmList->itsData) <
                         atol(theAnswer))

                theAnswer.Set(theParmList->itsData);

            else

                if (strcmp(theParmList->itsData,
                           theAnswer) < 0)
                    theAnswer.Set(theParmList->itsData);

            theParmList = theParmList->itsNext;

        }
        return(aRc);
    }
} MacroInstMin;

//****************************************************************************
// 
// Internal Macro : Base()
//
// Description    : Return Base X representation of number
// 
//****************************************************************************
class MacroBase : public WorkflowIntMacro
{
public:
    MacroBase() : WorkflowIntMacro("TO_BASE")
    {
       itsEscape=false;
    };             
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        int aBase = 16;

        if (theParmList != NULL)
        {
            theAnswer.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        }

        if (theParmList != NULL)
            aBase = atol(theParmList->itsData);

        if (aBase >= 2 && aBase <= 36)

            ToBase(theAnswer, aBase);

        else
        {

            if (itsStep != NULL)
                GetTaskCviPgm()->Print("ERROR: BASE of %s in template member %s, name is not valid. Base must be between 2 and 36.\n",
                                       (const char *) theParmList->itsData,
                                       (const char *) itsStep->GetMember(),
                                       (const char *) itsStep->GetName());
            aRc = 8;
        }

        return(aRc);
    }
} MacroInstBase;

//****************************************************************************
// 
// Internal Macro : EndsWith()
//
// Description    : Checks if a string ends with a given string
// 
//****************************************************************************
class MacroEndsWith : public WorkflowIntMacro
{
public:
    MacroEndsWith() : WorkflowIntMacro("ENDSWITH") { itsCache = true; };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        CviStr aStr;                   // string to find
        theAnswer.Set("0");            // assume false
        int aRc = 0;

        if (theParmList != NULL)       // got the 'find' parameter?
        {                              // begin grab find
            aStr.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab find

        while (!strcmp(theAnswer, "0") && // while false and
               theParmList != NULL)    // have more parameters to check
        {

            CviStr aVal(theParmList->itsData);

            const char *aCheck = aStr; // string to check

            if (strlen(aVal) <= strlen(aStr))

            {                          // begin value small enough

                aCheck += strlen(aCheck);
                aCheck -= strlen(aVal);

                if (!strcmp(aCheck, aVal))

                    theAnswer.Set("1");// we match

            }                          // end value small enount

            theParmList = theParmList->itsNext;

        }
        return(aRc);
    }
} MacroInstEndsWith;

//****************************************************************************
// 
// Internal Macro : LLQ()
//
// Description    : Get LLQ from dataset name
// 
//****************************************************************************
class MacroLLQ : public WorkflowIntMacro
{
public:
    MacroLLQ() : WorkflowIntMacro("LLQ") { itsCache = true; };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        CviStr aStr;                   // string to find
        int aRc = 0;

        if (theParmList != NULL)       // got the 'find' parameter?
        {                              // begin grab find
            const char *aPtr =             // set pointer
                (const char *) theParmList->itsData +
                strlen(theParmList->itsData);

            while (aPtr > (const char *) theParmList->itsData &&
                   *aPtr != '.')

                aPtr --;               // go backward to find .

            if (*aPtr == '.')          // if have a .

               theAnswer.Set(aPtr + 1);// set LLQ

            else                       // otherwise

               theAnswer.Set(aPtr);    // use as-is with no dot

        };                             // end grab find
        return(aRc);
    }
} MacroInstLLQ;

//****************************************************************************
// 
// Internal Macro : TargetFields()
//
// Description    : Locate DSNs
// 
//****************************************************************************
class MacroTargetFields : public WorkflowIntMacro
{
protected:

    char itsMulti;
    char itsHasField;

public:
    MacroTargetFields(const char *theName = "TARGET_FIELDS") : WorkflowIntMacro(theName)
    {
        itsMulti = true;
        itsHasField = true;            // has a field parm
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        CviStr aOrigLLQ;               // Original LLQ to locate
        CviStr aField("DSN");          // field to use for replacement data


        itsEscape = true;              // assume we will want to escape answer

        if (theParmList != NULL)       // got LLQ?
        {                              // begin grab LLQ

            aOrigLLQ.Set(theParmList->itsData); // BMC-ORIG-LLQ value to search

            theParmList = theParmList->itsNext;

        };                             // end grab LLQ

        if (itsHasField &&             // takes a field parm
            theParmList != NULL)       // expecting field to be passed?

        {                              // begin add field

            aField.Set(theParmList->itsData);

            theParmList = theParmList->itsNext;

        };                             // end add field

        Variable *aVar =               // does shortcut exist?
               itsPropVars->GetVariable("TARGET");

        if (aVar != NULL)              // if OK
                                       
        {                              // begin locate from set

            TRACE("Found target shortcut\n");

            aVar = aVar->GetSubVar();  // get subvariables

            while (aVar != NULL)       // for all rows

            {                          // begin locate LLQ in row

// Use BMC-ORIG-LLQ attribute that config tool writes from the SI.
// This will allow us to locate targets even if customer renamed them.

                Variable *aLLQVar =
                    aVar->FindSubVar("BMC-ORIG-LLQ");
                                       
                if (aLLQVar != NULL && // if OK and matches
                    !strcmp(aLLQVar->Value(),
                            aOrigLLQ))

                {                      // begin found!

                    Variable *aFieldVar = aVar->FindSubVar(aField);

                    if (aFieldVar != NULL) // if OK

                    {

                        TRACE("FOUND LLQ %s FROM TARGET BMC-ORIG-LLQ SET, FIELD %s %s\n",
                                               (const char *) aOrigLLQ,
                                               (const char *) aField,
                                               (const char *) aFieldVar->Value());

                        CviStr anAnswer(aFieldVar->Value());

                        if (itsMulti)  // multiples are OK

                        {

                            AddList(theAnswer,
                                    anAnswer);

                        }

                        else           // otherwise

                        {              // begin single only

                            theAnswer.Set(anAnswer);

#ifdef _SRC_
// This was a temporary thing until z/OSMF could provide a defininitive SMP/E target
// backing dataset for UNIX filesystems when the filesystem wasn't mounted.
// However, we are going to avoid having such filesystems for SMP/E targets and will
// move toward a tar/archive based solution instead.
// This prompt disrupts the automated flow up-front so this step will be disabled.
                            if (!strcmp(aField, "DSN"))

                            {          // begin check for suspect DSN

                                Variable *aSuspect = aVar->FindSubVar("MOUNT_STATUS");

                                if (aSuspect != NULL &&
                                    !strcmp(aSuspect->Value(),
                                            "SUSPECT"))

                                {      // begin suspect mount point - ask user

                                    itsEscape = false; // Do NOT add Velocity escape sequence

                                    GetTaskCviPgm()->Print("WORKFLOW STEP %s NEEDS TO PROMPT FOR SMP/E TARGET ASSOCIATED WITH %s\n",
                                                           (const char *) itsStep->GetName(),
                                                           (const char *) aVar->FindSubVar("MOUNT")->Value());

                                    theAnswer.Set(itsStep->AddUNIXDSNPrompt(itsPropVars,
                                                                            aVar)); // add prompt

                                };     // end suspect mount point - ask user

                            };         // end check for suspect DSN
#endif

                            break;     // stop the search

                        }              // single only

                    };

                };                     // end found!

                aVar = aVar->GetNext();// next!

            };                         // end locate LLQ in row

        };                             // end locate from set

        return(aRc);                   // return result
    }
} MacroInstTargetFields;

//****************************************************************************
// 
// Internal Macro : TargetField()
//
// Description    : Locate DSNs
// 
//****************************************************************************
class MacroTargetField : public MacroTargetFields
{

public:
    MacroTargetField() : MacroTargetFields("TARGET_FIELD")
    {
        itsMulti = false;
    };

} MacroIntTargetField;


//****************************************************************************
// 
// Internal Macro : TargetDSN()
//
// Description    : Locate DSNs
// 
//****************************************************************************
class MacroTargetDSN : public MacroTargetFields
{

public:
    MacroTargetDSN() : MacroTargetFields("TARGET_DSN")
    {
        itsMulti = false;
        itsHasField = false;
    };

} MacroIntTargetDSN;


//****************************************************************************
// 
// Internal Macro : TargetDSNS()
//
// Description    : Locate DSNs
// 
//****************************************************************************
class MacroTargetDSNS : public MacroTargetFields
{

public:
    MacroTargetDSNS() : MacroTargetFields("TARGET_DSNS")
    {
        itsMulti = true;
        itsHasField = false;
    };

} MacroIntTargetDSNS;


//****************************************************************************
// 
// Internal Macro : ResolveSymbols()
//
// Description    : Resolve JCL-style symbols in resulting string
// 
//****************************************************************************
class MacroResolveSymbols : public WorkflowIntMacro
{
public:
    MacroResolveSymbols() : WorkflowIntMacro("RSYM") { };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        if (theParmList != NULL)       // got the 'find' parameter?
        {                              // begin grab find
            theAnswer.Set(theParmList->itsData);
            itsPropVars->ReplaceWithValues(theAnswer);
            itsPropVars->TranslateJCLValues(theAnswer);

        };                             // end grab find
        return(aRc);
    }
    virtual MacroParms *GenParmList(CviStr &theParms)
    {
        return(new MacroParms(theParms));
    }

} MacroInstResolveSymbols;

//****************************************************************************
// 
// Internal Macro : FmtVersion()
//
// Description    : Format version number
// 
//****************************************************************************
class MacroFmtVersion : public WorkflowIntMacro
{
public:
    MacroFmtVersion() : WorkflowIntMacro("FMT_VERSION") { itsCache = true; };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        CviStr aVersion;               // version
        int aRc = 0;

        if (theParmList != NULL)       // got version number?
        {                              // begin grab version number
            aVersion.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab version number

        if (theParmList != NULL)       // got the format?
        {                              // begin grab fromat

            CviStr aFmt(theParmList->itsData);

            theParmList = theParmList->itsNext;

            int aVer = 0;              // version
            int aRel = 0;              // release
            int aMod = 0;              // mod

            const char *aParm;         // parameter

            aParm = aVersion.Tok("."); // get first token

            if (aParm != NULL)         // if OK

            {                          // begin get version

                aVer = atoi(aParm);    // set version

                aParm = aVersion.Tok("."); // get next part

            };                         // end get version

            if (aParm != NULL)         // if OK

            {                          // begin get release

                aRel = atoi(aParm);    // set release

                aParm = aVersion.Tok("."); // get next part

            };                         // end get release

            if (aParm != NULL)         // if OK

            {                          // begin get release

                aMod = atoi(aParm);    // set mode

            };                         // end get release

            CviStr aV;                 // V
            CviStr aR;                 // R
            CviStr aM;                 // M
            CviStr aVV;                // VV
            CviStr aRR;                // RR
            CviStr aMM;                // MM

            aV.Print("%d", aVer);      // version
            aVV.Print("%02d", aVer);   // version
            aR.Print("%d", aRel);      // release
            aRR.Print("%02d", aRel);   // release
            aM.Print("%d", aMod);      // mod
            aMM.Print("%02d", aMod);   // mod

            theAnswer.Set(aFmt);       // setup new string


            theAnswer.Replace("VV", aVV);
            theAnswer.Replace("RR", aRR);
            theAnswer.Replace("MM", aMM);
            theAnswer.Replace("V", aV);
            theAnswer.Replace("R", aR);
            theAnswer.Replace("M", aM);

        };                             // end grab format

        return(aRc);
    }
} MacroInstFmtVersion;


//****************************************************************************
// 
// Internal Macro : StrTok()
//
// Description    : Grabs token
// 
//****************************************************************************
class MacroStrTok : public WorkflowIntMacro
{
public:
    MacroStrTok() : WorkflowIntMacro("STRTOK") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {

        int    aTokNum = 1;            // token number
        CviStr aDelimit(",");          // delimiter
        CviStr aFind;                  // string to locate tokens

        int aRc = 0;

        if (theParmList != NULL)       // got the string to parse?
        {                              // begin grab find string
            aFind.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab find string

        if (theParmList != NULL)       // got the delimiter?
        {                              // begin grab delimiter
            aDelimit.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab token number

        if (theParmList != NULL)       // got the token number?
        {                              // begin grab token number
            aTokNum =                  // grab token number
                atoi(theParmList->itsData);
            
            theParmList = theParmList->itsNext;
        };                             // end grab token number

        const char *aTok =             // get token
            aFind.Tok(aDelimit,        // from string
                      aTokNum - 1);    // (# of tokens to skip before getting token)

        if (aTok != NULL)              // if OK
            theAnswer.Set(aTok);       // use it

        return(aRc);
    }
} MacroInstStrTok;

//****************************************************************************
// 
// Internal Macro : #PREFIX
//
// Description    : Gets prefix of string
// 
//****************************************************************************
class MacroPrefix : public WorkflowIntMacro
{
public:
    MacroPrefix() : WorkflowIntMacro("PREFIX") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {

        const char *aStr;              // string to process
        int    aLen;                   // length to process

        int aRc = 0;

        if (theParmList != NULL)       // got the token number?
        {                              // begin grab token number
            aStr = theParmList->itsData;
            theParmList = theParmList->itsNext;
        };                             // end grab token number

        if (theParmList != NULL)       // got the string to parse?
        {                              // begin grab find string
            aLen = atoi(theParmList->itsData);

            theAnswer.Reset();         // reset answer
            theAnswer.Print("%.*s",
                            aLen,
                            aStr);

        };                             // end grab find string

        return(aRc);
    }
} MacroPrefix;

//****************************************************************************
// 
// Internal Macro : BuildDSNList()
//
// Description    : Builds DSN list
// 
//****************************************************************************
class MacroBuildDSNList : public WorkflowIntMacro
{
public:
    MacroBuildDSNList() : WorkflowIntMacro("BUILD_DSN_LIST") 
    {
       itsEscape = false;              // do NOT escape the whole content
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {

        CviStr aListName;
        int aRc = 0;

        if (theParmList != NULL)       // got the list name?
        {                              // begin grab list name
            aListName.Set(theParmList->itsData);
            theAnswer.Print("#set($%s = [])\n", (const char *) aListName);
            theParmList = theParmList->itsNext;
        };                             // end grab list name

        while (theParmList != NULL)    // get correct token
        {

            CviStr aName(theParmList->itsData);

            Dataset *aDSN = itsDatasets; // process datasets

            while (aDSN != NULL)       // for all DSNs

            {                          // begin find matching DSN

                const char *aCheck = aDSN->GetID();

                TRACE("Comparing %s to %s\n",
                                       (const char *) aName,
                                       (const char *) aCheck);

                if (strlen(aName) <= strlen(aCheck) &&
                    aDSN->IsNeeded() &&
                    aDSN->Allocate())

                {                      // begin safe to check

                    aCheck += strlen(aCheck);

                    aCheck -= strlen(aName);

                    if (!strcmp(aCheck,// if it
                                aName)) // matches the LLQ

                    {                  // begin matches

                        CviStr aDSNName(aDSN->GetDSN());

                        if (itsPropVars != NULL)

                            itsPropVars-> // translate JCL-style names to values
                                TranslateJCLValues(aDSNName);

                        aDSNName.VelocityEscape(); 

                        theAnswer.Print("#set( $TMP = $%s.add(\"%s\") )",
                                        (const char *) aListName,
                                        (const char *) aDSNName); 

                    }                  // end matches

                };                     // end safe to check

                aDSN = aDSN->GetNext(); // next!


            };                         // end find matching DSN

            theParmList = theParmList->itsNext;
        };

        return(aRc);
    }
} MacroInstBuildDSNList;

//****************************************************************************
// 
// Internal Macro : Replace()
//
// Description    : Grabs token
// 
//****************************************************************************
class MacroReplace : public WorkflowIntMacro
{
public:
    MacroReplace() : WorkflowIntMacro("REPLACE") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {

        CviStr aFind;                  // string to locate tokens

        int aRc = 0;

        if (theParmList != NULL)       // got the original string
        {                              // begin grab string
            theAnswer.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab strint

        if (theParmList != NULL)       // got the find string?
        {                              // begin grab find
            aFind.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab find

        if (theParmList != NULL)       // got the replace string?
        {                              // begin find/replace
            theAnswer.Replace(aFind, theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end find/replace
        return(aRc);
    }
} MacroInstReplace;


//****************************************************************************
// 
// Internal Macro : ReplTok()
//
// Description    : Replace token with a value
// 
//****************************************************************************
class MacroReplTok : public WorkflowIntMacro
{
public:
    MacroReplTok() : WorkflowIntMacro("REPLACE_TOK") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {

        int    aTokNum = 1;            // token number
        CviStr aDelimit(",");          // delimiter
        CviStr aNewValue;              // new value
        CviStr aFind;                  // string to locate tokens

        int aRc = 0;

        if (theParmList != NULL)       // got the string to parse?
        {                              // begin grab find string
            aFind.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab find string

        if (theParmList != NULL)       // got the delimiter?
        {                              // begin grab delimiter
            aDelimit.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab token number

        if (theParmList != NULL)       // got the token number?
        {                              // begin grab token number
            aTokNum =                  // grab token number
                atoi(theParmList->itsData);
            
            theParmList = theParmList->itsNext;
        };                             // end grab token number

        if (theParmList != NULL)       // got the new value?
        {                              // begin grab new value
            aNewValue.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab new value

        const char *aTok =             // get token
            aFind.Tok(aDelimit);       // from string

        while (aTok != NULL)           // for all tokens

        {                              // begin process token

            if (strlen(theAnswer) != 0)

                theAnswer.Add(aDelimit);

            aTokNum --;                // decrement token count

            if (aTokNum == 0)          // if found

                theAnswer.Add(aNewValue);  // use new token

            else                       // otherwise

                theAnswer.Add(aTok);   // use original token

            aTok = aFind.Tok(aDelimit); // next!

        };                             // end process token

        return(aRc);
    }
} MacroInstReplTok;

//****************************************************************************
// 
// Internal Macro : Pad()
//
// Description    : Pad variable to desired length
// 
//****************************************************************************
class MacroPad : public WorkflowIntMacro
{
public:
    MacroPad() : WorkflowIntMacro("PAD") {};
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        CviStr aValue;                 // value of variable
        int    aLen = 0;               // length
        int aRc = 0;

        if (theParmList != NULL)       // got the variable name parameter?
        {                              // begin grab variable name
            aValue.Set(theParmList->itsData);
            theParmList = theParmList->itsNext;
        };                             // end grab variable name

        if (theParmList != NULL)       // got the length?
        {                              // begin grab length

            aLen = atoi(theParmList->itsData);
            theParmList = theParmList->itsNext;

            const char *aPtr = strstr(theParmList->itsData, "#[[");
            while (aPtr != NULL)
            {
                aLen -= 3;
                aPtr = strstr(aPtr+3, "##[");
            }
            aPtr = strstr(theParmList->itsData, "]]#");
            while (aPtr != NULL)
            {
                aLen -= 3;
                aPtr = strstr(aPtr+3, "]]#");
            }

        };                             // end grab length

        TRACE("PADDING %s length %d\n",
                               (const char *) aValue,
                               aLen);

        theAnswer.Print("%-*.*s",       // pad
                        aLen,
                        aLen,
                        (const char *) aValue);
        return(aRc);
    }
} MacroInstPad;

//****************************************************************************
// 
// Internal Macro : Append()
//
// Description    : Append to previous line
// 
//****************************************************************************
class MacroAppend : public WorkflowIntMacro
{
public:
    MacroAppend() : WorkflowIntMacro("APPEND") { };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;
        if (theParmList != NULL)
            theAnswer.Set(theParmList->itsData);
        return(aRc);
    }
    virtual MacroParms *GenParmList(CviStr &theParms)
    {
        return(new MacroParms(theParms));
    }
    virtual int ReplaceContent(CviStr &theContent,
                               const char *theOffset,
                               int theLen,
                               const char *theNewData)
    {
        const char *aPrev = theOffset; // setup to find previous line

        while (aPrev > (const char *) theContent &&
               *aPrev != '\n')

            aPrev --;                  // go backwards to previous line

        theContent.Replace(aPrev,
                           (int) (theOffset - aPrev + theLen),
                           theNewData);
        return((int) (theOffset - aPrev + strlen(theNewData)));

    }
} MacroInstAppend;

//****************************************************************************
// 
// Internal Macro : MERGE()
//
// Description    : Merges two or more tables together.
//                  Valid only during table iteration operations like
//                  #FOR_EACH
// 
//****************************************************************************
class MacroMerge : public WorkflowIntMacro
{
private:
    int itsMergeCnt;
public:
    MacroMerge() : WorkflowIntMacro("MERGE", false)
    {
       itsEscape=false;
       itsMergeCnt = 0;
    };
    virtual int Evaluate(MacroParms *theParmList, CviStr &theAnswer) {
        int aRc = 0;

        int aRowCnt = 0;               // row counter

        itsMergeCnt ++;                // bump merge counter

        CviStr aMergeName;
        CviStr aBaseCols;              // base columns
        CviStr aUsedList;              // used row list
        CviStr aBaseName;              // base name
                                       


        aMergeName.Print("MERGED_%d",  // setup merged table name
                         itsMergeCnt);

        theAnswer.Set(aMergeName);     // setup answer

        itsPropVars->Add(aMergeName, "", 2); // add table

        Variable *aMergeVar = 
            itsPropVars->GetVariable(aMergeName);

        Variable *aBaseTb = NULL;      // base table variable
        Variable *aBaseSort = NULL;    // sorted rows

        if (theParmList != NULL &&            // if parm found and
            strlen(theParmList->itsData) > 0) // OK
        {

            const char *aPtr = theParmList->itsData.Parm(":");

            if (aPtr != NULL)          // grab name and columns
            {
                aBaseName.Set(aPtr);
                aPtr = theParmList->itsData.Parm("");
                if (aPtr != NULL)
                    aBaseCols.Set(aPtr);
            }

            aBaseTb = itsPropVars->GetVariable(aBaseName);

            if (aBaseTb == NULL)       // if not found

            {                          // begin complain

                DPRINT("WARNING: Base table %s not found during #MERGE for step %s\n",
                       (const char *) aBaseName,
                       (const char *) itsStep->GetName());

                aRc = 4;               // warning

            }                          // end complaint

            else

                aBaseSort = aBaseTb->SortRows();

            theParmList = theParmList->itsNext;
        }

        while (aRc <= 4 &&             // while just warning and
               aBaseTb != NULL &&      // base table OK and
               theParmList != NULL)    // have parms left

        {                              // begin process parm/table
            
            CviStr aSrcCols;           // source columns
            CviStr aSrcName;           // source name

            const char *aPtr = theParmList->itsData.Parm(":");

            if (aPtr != NULL)          // grab name and columns
            {
                aSrcName.Set(aPtr);
                aPtr = theParmList->itsData.Parm("");
                if (aPtr != NULL)
                    aSrcCols.Set(aPtr);
            }

            TRACE("Base %s  Source %s\n",
                                   (const char *) aBaseName,
                                   (const char *) aSrcName);

            TRACE("Base Columns: %s\n\nSrc Columns: %s\n\n",
                                   (const char *) aBaseCols,
                                   (const char *) aSrcCols);

            Variable *aSrcTb = itsPropVars->GetVariable(aSrcName);

            if (aSrcTb != NULL)        // if OK

            {                          // begin process to merge

                Variable *aSrcSort = aSrcTb->SortRows();
                Variable *aSrcRow = aSrcSort;

                while (aSrcRow != NULL)

                {

                    Variable *aBaseRow = aBaseSort;

                    while (aBaseRow != NULL)

                    {                  // begin iterate over base row to find matches

                        CviStr aSrcIterCols(aSrcCols);
                        CviStr aBaseIterCols(aBaseCols);

                        const char *aSrcCol = aSrcIterCols.Tok(":");
                        const char *aBaseCol = aBaseIterCols.Tok(":");

                        bool    aMatch = true;

                        while (aMatch &&
                               aSrcCol != NULL &&
                               aBaseCol != NULL)

                        {

                            Variable *aSrcVar = aSrcRow->FindSubVar(aSrcCol);
                            Variable *aBaseVar = aBaseRow->FindSubVar(aBaseCol);

                            aMatch = false;

                            if (aSrcVar != NULL && aBaseVar != NULL)
                            {
                                TRACE("Comparing %s:%s and %s:%s\n",
                                                       (const char *) aSrcCol,
                                                       (const char *) aSrcVar->Value(),
                                                       (const char *) aBaseCol,
                                                       (const char *) aBaseVar->Value());
                            }

                            if (aSrcVar != NULL &&
                                aBaseVar != NULL &&
                                !strcmp(aSrcVar->Value(),
                                        aBaseVar->Value()))

                            {

                                aMatch = true;

                            }


                            if (aMatch)
                            {
                                aSrcCol = aSrcIterCols.Tok(":");
                                aBaseCol = aBaseIterCols.Tok(":");
                            }

                        };

                        if (aMatch)    // match?

                        {              // begin merge

                            TRACE("Matched row...\n");

                            CviStr aUsed;

                            aUsed.Print("#%p#", (void *) aBaseRow);

                            AddList(aUsedList, aUsed);

                            aUsed.Reset();
                            aUsed.Print("#%p#", (void *) aSrcRow);

                            AddList(aUsedList, aUsed);

                            Variable *aRowVar = new Variable(aBaseRow);

                            aMergeVar->AddSubVariable(aRowVar);

                            aRowCnt ++;

                            CviStr aRowStr;

                            aRowStr.Print("%d", aRowCnt);

                            Variable *aCol = aSrcRow->GetSubVar();

                            while (aCol != NULL)
                            {
                                if (strcmp(aCol->GetName(), "rownum"))
                                {
                                    TRACE("Adding col %s\n", (const char *) aCol->GetName());
                                    aRowVar->AddSubVariable
                                        (new Variable(aCol->GetName(),
                                                      aCol->Value()));
                                }
                                aCol = aCol->GetNext();
                            }

                            aRowVar->AddSubVariable        // add column
                                (new Variable("rownum",    // called rownum
                                              aRowStr));

                            AddColVars(itsPropVars,
                                       aRowVar);


                        };             // end merge

                        aBaseRow = aBaseRow->GetNext();

                    };                 // end iterate over base row to find matches

                    aSrcRow = aSrcRow->GetNext();

                }

                Variable *aRow = aSrcSort; // setup to copy unused rows

                while (aRow != NULL)       // for all rows

                {                          // begin search/copy unused rows

                    CviStr aUsed;
                    aUsed.Print("#%p#", (void *) aRow);

                    if (!InList(aUsedList, aUsed))

                    {

                        aRowCnt ++;

                        CviStr aRowStr;

                        aRowStr.Print("%d", aRowCnt);

                        Variable *aNewRow = new Variable(aRow);

                        CviStr aSrcIterCols(aSrcCols);
                        CviStr aBaseIterCols(aBaseCols);

                        const char *aSrcCol = aSrcIterCols.Tok(":");
                        const char *aBaseCol = aBaseIterCols.Tok(":");

                        while (aSrcCol != NULL &&
                               aBaseCol != NULL)

                        {

                            Variable *aCol = 
                                aRow->FindSubVar(aSrcCol);

                            if (aCol != NULL)

                            {

                                TRACE("Adding missing base column %s\n",
                                                       (const char *) aBaseCol);

                                aNewRow->AddSubVariable
                                    (new Variable(aBaseCol,
                                                  aCol->Value()));

                            };

                            aSrcCol = aSrcIterCols.Tok(":");
                            aBaseCol = aBaseIterCols.Tok(":");

                        }

                        aMergeVar->AddSubVariable(aNewRow);

                        aNewRow->AddSubVariable        // add column
                            (new Variable("rownum",    // called rownum
                                          aRowStr));

                        AddColVars(itsPropVars,
                                   aNewRow);

                    }

                    aRow = aRow->GetNext();

                }                          // end search/copy unused rows

                aSrcTb->DeleteSorted(aSrcSort);

            }                          // end process to merge

            else                       // otherwise

            {                          // begin error

                DPRINT("WARNING: Table %s not found during #MERGE for step %s\n",
                       (const char *) aSrcName,
                       (const char *) itsStep->GetName());

                aRc = 4;               // warning

            };                         // end error

            theParmList = theParmList->itsNext;

        };                             // end process parm/table

        if (aBaseSort != NULL)         // if sorted

        {                              // begin 

            Variable *aRow = aBaseSort;// setup to copy unused rows

            while (aRow != NULL)       // for all rows

            {                          // begin search/copy unused rows

                CviStr aUsed;
                aUsed.Print("#%p#", (void *) aRow);

                if (!InList(aUsedList, aUsed))

                {

                    aRowCnt ++;

                    CviStr aRowStr;

                    aRowStr.Print("%d", aRowCnt);

                    Variable *aNewRow = new Variable(aRow);

                    aMergeVar->AddSubVariable(aNewRow);

                    aNewRow->AddSubVariable        // add column
                        (new Variable("rownum",    // called rownum
                                      aRowStr));

                    AddColVars(itsPropVars,
                               aNewRow);

                }

                aRow = aRow->GetNext();

            }                          // end search/copy unused rows

            aBaseTb->DeleteSorted(aBaseSort);

        }

        if (aRc <= 4) aRc = 0;         // we're OK with warnings

        return(aRc);
    }

} MacroMerge;


//****************************************************************************
// 
// Class        : ContentBlock
//
// Description  : Does standard processing for content blocks, such as FOR,
//                IF, ITERATE
// 
//****************************************************************************

class ContentBlock                     // begin ContentBlock
{

protected:

    CviStr  itsType;                   // content block type
    CviStr  itsCall;                   // call format
    CviStr  itsMid;                    // middle processor (such as #ELSE)
    CviStr  itsEnd;                    // end indicator

    CviStr  itsContent;                // content
    CviStr  itsParms;                  // parameters

    int     itsCallCnt;                // call counter

    const char *itsReplStart;          // replace content starting point
    int     itsReplLen;                // replacement length

    WorkflowStep *itsStep;             // step being processed
    WorkflowStep *itsIncludes;         // include list

    VariableList *itsVariables;        // variables

    ContentBlock *itsNext;             // next block processor

public:

    static  ContentBlock *itsTop;      // top-most processor

public:

    ContentBlock(const char *theType)  // content block, starting at address

    {                                  // begin process start of processing block

        itsType.Set(theType);          // save type

        itsNext = NULL;                // no next yet

        if (itsTop == NULL)            // if no top
            itsTop = this;             // we are top
        else                           // otherwise
        {                              // add next
            itsNext = itsTop;          // our next is current list's second item
            itsTop = this;             // we are now second
        };                             // end add next

        itsCall.Print("#%s(",          // setup starting
                      theType);        // call

        itsEnd.Print("#END_%s",        // default ending
                     theType);

        itsVariables = NULL;           // no variables

        itsIncludes = NULL;            // no includes

        itsStep = NULL;                // no step

        itsCallCnt = 0;                // clear call count

    }                                  // end process start of processing block

    const char *GetType()
    {
        return((const char *) itsType);
    }

    int GetCount()
    {
        return(itsCallCnt);
    }

    ContentBlock *GetNext()
    {
        return(itsNext);
    }

    virtual void Reset()               // reset processing for new content 
    
    {                                  // begin reset
        
        if (itsNext != NULL)           // if have next

            itsNext->Reset();          // reset it, too

    };                                 // end reset

    int ProcessBlock(CviStr &theContent, // process
                     int     theOffset,  // content at offset
                     WorkflowStep *theStep,
                     VariableList *theVariables,
                     WorkflowStep *theIncludes)

    {                                  // begin grab standard information

        int aRc = 0;                   // return code

        const char *aContent =         // get address
            (const char *) theContent + theOffset;

        while (*aContent == ' ') aContent ++;

        if (memcmp(itsCall,            // if this is
                   aContent,           // for us
                   strlen(itsCall)) == 0)

        {                              // begin process

            itsCallCnt ++;             // bump call counter

            itsStep = theStep;         // set step being processed

            itsVariables = theVariables; // set variables

            itsIncludes = theIncludes; // set includes

            aRc = GetParms(aContent + strlen(itsCall));  // grab parameters

            if (aRc == 0)              // if OK

                aRc = EvalParms();     // evaluate parameters

            if (aRc == 0)              // if still OK

                aRc = GetContent(theContent,
                                 aContent);  // get block content

            if (aRc == 0)              // if still OK

                aRc = ProcessContent();// proces content

            if (aRc == 0)              // if OK

                aRc = ReplaceContent(theContent);

        }                              // end process

        else                           // otherwise

        if (itsNext != NULL)           // if we have a next item

            aRc = itsNext->ProcessBlock // try another processor
                    (theContent,
                     theOffset,
                     theStep,
                     theVariables,
                     theIncludes);

        else                           // otherwise

            aRc = -1;                  // mark as not processed

        return(aRc);                   // return result

    };                                 // end grab standard information

    virtual int GetParms(const char *theStart)  // get content from parm location

    {                                  // grab parameters

        int aRc = 0;                   // OK so far

        itsParms.Reset();              // reset parameters

        const char *aEnd = theStart;   // initialize ending character candidate

        int aParenCnt = 1;             // we are in parenthesis processing
        char aInQuote = 0;             // in quote setting
        char aInBrace = 0;             // not inside brace

        while (aParenCnt > 0 &&        // while in parenthesis
               *aEnd != 0)             // and not at the end

        {                              // begin find end
                           
            aEnd = strpbrk(aEnd,       // find
                           "\"[]()");  // next character

            if (aEnd == NULL) break;   // break out if not found...

            if (aInQuote)              // inside quote?

            {                          // begin check for end quote

                if (*aEnd == '"')      // found end of quote?
                    aInQuote = 0;      // no longer inside quote

            }                          // end check for end quote

            else                       // otherwise

            if (aInBrace)              // inside brace?

            {                          // begin check for end brace
                if (*aEnd == ']')      // found end of brace?
                    aInBrace = 0;      // no longer inside brace

            }                          // end check for end brace

            else                       // otherwise

            {                          // begin handle nested (), start of quote/brackets
                if (*aEnd == '[')      // start of brace
                    aInBrace = 1;     
                if (*aEnd == '(')      // start?
                    aParenCnt ++;      // bump nested count
                if (*aEnd == ')')      // end?
                    aParenCnt --;      // decrement nested count
                if (*aEnd == '"')      // start of quote?
                    aInQuote = 1;      // inside a quote

            };                         // end handle nested (), start of quote/brackets
                                       
            if (aParenCnt != 0)        // not finished?
                aEnd ++;               // bump

        };                             // end find end

        if (aParenCnt > 0       ||     // if end not found
            aInBrace  > 0)             // or brace count not OK

        {                              // begin error
                           
            if (aInBrace  > 0)

                GetTaskCviPgm()->Print("ERROR: No closing ] found for %s in template member %s, name %s\nFirst 512 bytes of statement text:\n%.512s\n",
                                       (const char *) itsType,
                                       (const char *) itsStep->GetMember(),
                                       (const char *) itsStep->GetName(),
                                       (const char *) theStart);

            if (aParenCnt > 0)

                GetTaskCviPgm()->Print("ERROR: No closing ) found for %s in template member %s, name %s\nFirst 512 bytes of statement text:\n%.512s\n",
                                       (const char *) itsType,
                                       (const char *) itsStep->GetMember(),
                                       (const char *) itsStep->GetName(),
                                       (const char *) theStart);

            aRc = 8;                   // we failed

         };                            // end error

         if (aEnd != NULL && aRc == 0) // if OK

             itsParms.Add(theStart, (int)(aEnd - theStart));

         return(aRc);                  // return result

    };                                 // end grab parameters

    virtual int GetContent(CviStr &theContent,
                           const char *theCall)
    {
                                       
        int aRc = 0;                   // OK so far

        bool aEvalOk = false;          // no evaluation done yet

        itsContent.Reset();            // reset content block

        int aDepth = 1;                // depth of block so far - default looking for close

        itsReplStart = theCall;        // set replacement start point for later

        const char *aStart = theCall + strlen(itsCall) + strlen(itsParms);

        if (strlen(itsEnd) > 0)        // if we have an end tag

        {                              // begin skip first line

            aStart = strstr(aStart, "\n"); 

            if (aStart != NULL)        // if OK
                                       
                aStart ++;             // begin start beyond call line
                                       
        };                             // end skip first line

        const char *aEndPos = aStart;  // setup a potential ending position

        if (strlen(itsEnd) == 0)       // if no end tag

        {                              // begin handle no end 

            while (*aEndPos != '\n' && // while not at end of
                   *aEndPos != 0)      // the line

                aEndPos ++;            // bump forward

            aDepth = 0;                // no search should be performed

        };                             // end handle no end

        while (aDepth > 0)             // while depth is OK and

        {                              // begin locate data

            const char *aNext = strstr(aEndPos, itsCall);
            const char *aEnd = strstr(aEndPos, itsEnd);
            const char *aMid = NULL;

            if (aDepth == 1 &&         // if outer statement
                !aEvalOk    &&         // no evaluation copmleted yet
                strlen(itsMid) != 0)   // if we should process middle statement

                aMid = strstr(aEndPos, itsMid);

            if (aEnd == NULL)          // if no ending found

            {                          // begin error

                GetTaskCviPgm()->Print("ERROR: No closing %s found for %s in template member %s, name %s\n%s\n",
                                       (const char *) itsEnd,
                                       (const char *) itsCall,
                                       (const char *) itsStep->GetMember(),
                                       (const char *) itsStep->GetName(),
                                       (const char *) theCall);

                aRc = 8;               // fail

                break;                 // break out of loop

            };                         // end error

            if (aMid != NULL        && // if found middle statement and
                aMid < aEnd         && // it is before the end
                (aNext == NULL ||      // and has to come before
                 aMid < aNext))        // any nested IF

            {                          // begin process middle

                if (EvalMid(aRc))      // evaluate at mid-way point

                {                      // begin bypass any other midway points

                    itsContent.Add(aStart, (int) (aMid - aStart));

                    itsContent.Trim(' '); // trim trailing spaces

                    aEvalOk = true;    // evaluation complete - just find the end now

                    aEndPos = aMid + strlen(itsMid); // set new ending position to continue search

                }                      // end bypass any other midway points

                else                   // otherwise

                {                      // begin get new parms

                    itsParms.Reset();  // clear out parms

                    aRc = GetParms(aMid + strlen(itsMid)); // get new parms

                    aStart = aMid + strlen(itsMid) + strlen(itsParms);

                    aStart = strstr(aStart, "\n"); 

                    if (aStart != NULL) // if OK

                        aStart ++;     // begin start beyond call line

                    aEndPos = aStart;  // start end search in new spot

                };                     // end get new parms

            }                          // end process middle

            else                       // otherwise

            if (aNext != NULL       && // if we have another call
                aNext < aEnd)          // and it comes before our end point
                                       
            {                          // begin nested

                aDepth ++;             // bump depth

                aEndPos = aNext + strlen(itsCall); // set new ending position to continue search

            }                          // end nested

            else                       // otherwise

            {                          // begin end of a block

                aDepth --;             // decrement depth

                aEndPos = aEnd + strlen(itsEnd); // set new ending position to continue search

            };                         // end end of a block

        };                             // end locate data

        if (aRc == 0)                  // if OK
                                       
        {                              // begin setup content

            itsReplLen = (int) (aEndPos - itsReplStart);

            if (*aEndPos == '\n')      // if ends on a newline

                itsReplLen ++;         // replace it, too

            aEndPos -= strlen(itsEnd); // backup to end string

            if (!aEvalOk            && // if not evaluated OK already
                (strlen(itsMid) == 0 || // if not mid-operation type or
                 EvalMid(aRc)))         // evaluation passed

            {                          // begin use content

                itsContent.Add(aStart,
                               (int) (aEndPos - aStart));

                itsContent.Trim(' '); // trim trailing spaces

            };                         // end use content

        };                             // end setup content

        return(aRc);                   // return result

    };

    virtual int EvalParms() { return 0; }; // evaluate parameters

    virtual bool EvalMid(int &theRc) { return false; };// evaluate mid-content statement
         
    virtual int ProcessContent()
    { 
        return 0;
    };

    virtual int ReplaceContent(CviStr &theContent)
    {

//        TRACE("Replacing block content with %s\n",
//                               (const char *) itsContent);

        theContent.Replace(itsReplStart, // replace
                           itsReplLen,   // original
                           itsContent);  // content

//        TRACE("New block content: %s\n",
//                               (const char *) theContent);

        return(0);                     // OK

    };

};                                     // end ContentBlock

ContentBlock *ContentBlock::itsTop = NULL;

//****************************************************************************
// 
// Class        : ForEach
//
// Description  : ForEach processing
// 
//****************************************************************************
class ForEach : public ContentBlock

{

public:

    int itsIterationCount;

    ForEach() : ContentBlock("FOR_EACH") { itsIterationCount = 0; };

    virtual int EvalParms()
    {

        int aRc = 0;

        TRACE("FOR_EACH PARAMETERS: %s\n",
                               (const char *) itsParms);

        aRc = MacroMerge.Process(itsParms,
                                 itsStep,
                                 itsVariables,
                                 false);

        return(aRc);

    }

    virtual int ProcessContent()
    {

        int aRc = 0;

        TRACE("FOR_EACH CONTENT BLOCK:\n%s\n",
                               (const char *) itsContent);

        const char *aParm = itsParms.Parm(",");

        if (aParm != NULL)             // if found

        {                              // begin use parm

            CviStr anUnrolled;         // unrolled loop
            CviStr aCond;              // condition
            CviStr aVarName(aParm);    // variable name
            CviStr aVarAlias;          // variable alias name

            if (strstr(aVarName, ":") != NULL) // if has a rename of variable
            {                          // begin get real and alias
                CviStr aTmp;
                aTmp.Set(aVarName.Tok(":"));
                aVarAlias.Print("${%s}", aVarName.Tok(":"));
                aVarName.Set(aTmp);
            }                          // end get real ans alias

            TRACE("Will loop using var %s alias %s for: \n%s\n",
                                   (const char *) aVarName,
                                   (const char *) aVarAlias,
                                   (const char *) itsContent);

            aParm = itsParms.Parm("");  // get additional parms

            if (aParm != NULL)         // if set

                aCond = aParm;         // set conditional

            Variable *aVar = itsVariables->GetVariable(aVarName);

            TRACE("Variable %s: %p\n",
                                   (const char *) aVarName, (void *) aVar);

            if (aVar != NULL)          // if OK

            {                          // begin iterate

                Variable *aSortRows =  // sort
                    aVar->SortRows();  // rows

                TRACE("Rows: %p\n",
                                       (void *) aSortRows);

                MacroUnique aUnique;   // uniqueness macro

                CviStr  aMacVars;      // generate macro variables to be set

                int aRowNum = 1;       // row counter
                int aLoopCnt = 1;      // loop iteration counter

                if (strlen(aVarAlias) == 0)
                    aVarAlias.Set(aVar->GetPropName());

                Variable *aPrevRow = NULL;
                Variable *aRow = aSortRows;


                // Possible room for improvements:
                // 1. Replace only in aNewCond first...
                // 2. Perhaps change RemoveUnknownColumns to ReplaceColumns, passing
                //    the row address so it can replace values. Could be more efficient
                //    than iterating all columns if most columns are not used.
                //    NOTE: Would have to pass in previous row, too.

                while (aRow != NULL)

                {

                   CviStr aIterData;

                   CviStr aNewCond(aCond); // condition to evaluate
                                       
                   CviStr aVelocityCond;   // velocity condition to add


                   TRACE("   Iterating over row\n");

                   Variable *aVelCond = aRow->FindSubVar("VELOCITY_CONDITION");

                   if (aVelCond != NULL)
                   {
                       aIterData.Print("#if ( %s )\n",
                                       (const char *) aVelCond->Value());
                   }

                   aIterData.Add(itsContent); // add content

                   if (aVelCond != NULL)
                   {
                       aIterData.Add("#end\n");
                   }

                   Variable *aColVar = aRow->GetSubVar();
                   Variable *aPrevColVar = NULL;
                   
                   if (aPrevRow != NULL)    // use previous row if we have it
                       aPrevColVar = aPrevRow->GetSubVar();

                   CviStr aLoopFind; // loop counter find
                   CviStr aLoopRepl; // loop counter replace
                   
                   aLoopFind.Print("%s[loopCnt]",
                                   (const char *) aVarAlias);

                   aLoopRepl.Print("%d", aLoopCnt);

                   aIterData.Replace(aLoopFind, aLoopRepl);

                   aNewCond.Replace(aLoopFind, aLoopRepl);

                   if (itsStep->ProcType() == 'Y')
                   {
                       aMacVars.Reset();
                       aMacVars.Print("#SET_VAR(%s,%s)\n",
                                      (const char *) aLoopFind,
                                      (const char *) aLoopRepl);

                   }
//                       MacroGenerateStep.Replace(aLoopFind, aLoopRepl);

                   while (aColVar != NULL) // for all columns

                   {                   // begin find/replace

                       CviStr aFind;   // find
                       CviStr aRepl;   // replace
                       CviStr aPrevFind; // previous row find
                       CviStr aPrevRepl; // previous row replace


                       aFind.Print("%s[%s]",
                                   (const char *) aVarAlias,
                                   (const char *) aColVar->GetName());

                       aPrevFind.Print("%s.Prev[%s]",
                                       (const char *) aVarAlias,
                                       (const char *) aColVar->GetName());

                       if (aPrevColVar != NULL)
                           GenRowVarName(aPrevRepl,
                                         aVar,
                                         aPrevColVar,
                                         aRowNum-1,
                                         true);

                       GenRowVarName(aRepl, aVar, aColVar, aRowNum, true);
                            
                       aIterData.Replace(aPrevFind, aPrevRepl);

                       aIterData.Replace(aFind, aRepl);

                       if (itsStep->ProcType() == 'Y')
                       {
                           aMacVars.Print("#SET_VAR(%s,%s)\n",
                                          (const char *) aPrevFind,
                                          (const char *) aPrevRepl);

                           aMacVars.Print("#SET_VAR(%s,%s)\n",
                                          (const char *) aFind,
                                          (const char *) aRepl);

                       }

                       aNewCond.Replace(aFind, aRepl);

                       TRACE("Replacing %s with %s\n",
                             (const char *) aFind,
                             (const char *) aRepl);

                       if (aPrevColVar != NULL)
                           aPrevColVar = aPrevColVar->GetNext();

                       aColVar = aColVar->GetNext();

                       itsIterationCount ++;  // bump iteraton count

                   };                  // end find/replace

                   RemoveUnknownColumns // remove
                       (aIterData,     // unknowns
                        aVarAlias);

                   RemoveUnknownColumns // remove
                       (aNewCond,      // unknowns
                        aVarAlias);

                   CviStr aReason;     // evaluation reason

                   TRACE("#FOR_EACH processing condition %s\n", (const char *) aNewCond);

                   aUnique.Process(aNewCond, itsStep, itsVariables, false);

                   //GetTaskCviPgm()->Print("Post-Unique Condition: %s\n", (const char *) aNewCond);

                   if (strlen(aNewCond) > 0 &&
                       !EvaluateExpression(aNewCond,
                                           itsStep,
                                           itsVariables,
                                           aReason))

                   {                   // begin ignore

                       aLoopCnt --;    // offset loop counter

                       TRACE("Ignoring data as expression %s is false.\n",
                                              (const char *) aNewCond);

                       aIterData.Reset(); // clear out data
                       aMacVars.Reset();  // clear out data

                   }                   // end ignore

                   if (strlen(aReason) > 0)

                   {                   // begin evaluation error


                       GetTaskCviPgm()->Print("%s: Error processing #FOR_EACH condition.\n%s\n",
                                                  (const char *) itsStep->GetMember(),
                                                  (const char *) aReason);

                       SETRC(aRc,8);   // set failed code

                   };                  // end evaluation error

                   if (aRc == 0)       // if we are OK

                   {
                       // somehow need to handle passing parameters over for 
                       // GenerateStep....but can't do that if there are other conditions
                       // between here and the call.
                       // Perhaps rather than adding variables to GenerateStep, should add
                       // the content into here somehow?
                       // Add #SET statements for variable list processing?

                       //GetTaskCviPgm()->Print("Subprocessing %80.80s\n",
                       //                      (const char *) aIterData);
                       //
                       //aRc = itsStep->ProcessStatements(aIterData,
                       //                                 itsVariables,
                       //                                 itsIncludes,
                       //                                 NULL);
                   }

                   anUnrolled.Add(aMacVars);

                   anUnrolled.Add(aIterData);

                   aIterData.Reset();

                   aPrevRow = aRow;    // save previous row

                   aRow = aRow->GetNext();

                   aRowNum ++;         // bump row number

                   aLoopCnt ++;        // bump loop counter

                };

                aVar->DeleteSorted(aSortRows); // delete sorted rows

            };                         // end iterate

            TRACE("Data:\n%s\n", (const char *) anUnrolled);

            itsContent.Set(anUnrolled); // setup new content

        }                              // end use parm

        else

        if (aParm == NULL)             // if not all parms passed

        {                              // begin complain

            GetTaskCviPgm()->Print("%s is missing parameters. Must have 1 parameter.\n",
                                   (const char *) itsType);

            aRc = 8;                   // failed

        }                              // end complain

        return(aRc);                   // return result

    }

} aForEach;

//****************************************************************************
// 
// Class        : Iterate
//
// Description  : Iterate processing
// 
//****************************************************************************
class Iterate : public ContentBlock

{

private:

    CviStr  itsVarName;                // variable name

    int     itsMin;                    // minimum
    int     itsMax;                    // maximum

public:

    Iterate() : ContentBlock("ITERATE") {};

    virtual int EvalParms()
    {

        int aRc = 0;

        itsVarName.Set("${ITER}");     // set default variable name

        TRACE("ITERATE PARAMETERS: %s\n",
                               (const char *) itsParms);

        MacroParms *aParm = GenParmList(itsParms);

        if (aParm != NULL)             // if found

        {                              // begin process parameter

            CviStr aVarMin;            // min
            CviStr aVarMax;            // max

            aVarMin = aParm->itsData;  // assume minimum value

            if (strstr(aVarMin, "=") != NULL) // if has an assignment
            {                          // begin get var name
                CviStr aTmp;
                itsVarName.Reset();      // reset to clear out default name
                itsVarName.Print("${%s}", aVarMin.Tok("="));
                aTmp.Set(aVarMin.Tok(""));
                aVarMin.Set(aTmp);
            }                          // end get real ans alias

            aParm = aParm->itsNext;    // next!

            if (aParm != NULL)         // if set

            {                          // begin use max range

                aVarMax = aParm->itsData; // set conditional

                aParm = aParm->itsNext;// next!

            }                          // end use max range

            else                       // otherwise

                aVarMax = aVarMin;     // use min

            CviStr aReason;            // evaluation reason

            EvaluateExpression(aVarMin,
                               itsStep,
                               itsVariables,
                               aReason);

            if (strlen(aReason) > 0)   // if failed

            {                          // begin complain

                GetTaskCviPgm()->Print("%s: Error processing #ITERATE minimum value expression.\n%s\n",
                                       (const char *) itsStep->GetMember(),
                                       (const char *) aReason);

                SETRC(aRc,8);          // set failed code
                                       
            }                          // end complain

            EvaluateExpression(aVarMax,
                               itsStep,
                               itsVariables,
                               aReason);

            if (strlen(aReason) > 0)   // if failed

            {                          // begin complain

                GetTaskCviPgm()->Print("%s: Error processing #ITERATE maximum value expression.\n%s\n",
                                       (const char *) itsStep->GetMember(),
                                       (const char *) aReason);

                SETRC(aRc,8);          // set failed code
                                       
            }                          // end complain
                                       
            itsMin = atol(aVarMin);    // set min
            itsMax = atol(aVarMax);    // set max

        }                              // end process parameters

        else                           // otherwise

        {

            GetTaskCviPgm()->Print("%s is missing parameters. Must have 2 parameters.\n",
                                   (const char *) itsCall);

            aRc = 8;                   // failure

        }

        return(aRc);

    }

    virtual int ProcessContent()
    {

        int aRc = 0;

        CviStr anUnrolled;             // unrolled content
        CviStr aMacVars;               // macro variables


        TRACE("ITEREATE CONTENT BLOCK:\n%s\n",
                               (const char *) itsContent);

        TRACE("Will iterate using range %d-%d for variable %s\n",
                               itsMin,
                               itsMax,
                               (const char *) itsVarName);

        int aLoopCnt = itsMin;         // start at the beginning

        while (aRc == 0 &&             // while OK and
              ((itsMin == itsMax &&
                aLoopCnt == itsMax) || // same values or
               (itsMin < itsMax &&     // loop - either up or down
                aLoopCnt <= itsMax) ||
               (itsMax < itsMin &&
                aLoopCnt >= itsMax)))

        {                              // begin iterate

           aMacVars.Reset();           // reset

           CviStr aIterData(itsContent);

           CviStr aLoopRepl;           // replacement string 

           aLoopRepl.Print("%d", aLoopCnt);

           aIterData.Replace(itsVarName, aLoopRepl);

           if (itsStep->ProcType() == 'Y')
           {
               aMacVars.Print("#SET_VAR(%s,%s)\n",
                              (const char *) itsVarName,
                              (const char *) aLoopRepl);

//               MacroGenerateStep.Replace(itsVarName, aLoopRepl);
           }

           TRACE("   Iteration %d %s %s\n",
                                  aLoopCnt,
                                  (const char *) itsVarName,
                                  (const char *) aLoopRepl);

           anUnrolled.Add(aMacVars);
           anUnrolled.Add(aIterData);

           aIterData.Reset();

           if (itsMin <= itsMax)       //   if ascending

               aLoopCnt ++;            // bump loop counter

           else                        // otherwise

               aLoopCnt --;            // knock loop counter

        };                             // end iterate

        TRACE("Data:\n%s\n", (const char *) anUnrolled);

        itsContent.Set(anUnrolled);    // set new content

        
        return(aRc);                   // return result

    }

} aIterate;


//****************************************************************************
// 
// Class        : Include
//
// Description  : Include processing
// 
//****************************************************************************
class Include : public ContentBlock

{

private:

    CviStr itsDupes;                   // duplicate keeper

public:

    Include() : ContentBlock("INCLUDE")
    {

        itsCall.Set("#INCLUDE=");      // setup starting

        itsEnd.Reset();                // single-line item...no end tag

    };

    virtual void Reset()               // reset processing for new content

    {                                  // begin reset dupes

        ContentBlock::Reset();         // call base reset

        itsDupes.Reset();              // reset

    };                                 // end reset dupes

    virtual int GetParms(const char *theCall)
    {
        return(0);                     // will not process parms separately from processing
    }

    virtual int ProcessContent()
    {

        int aRc = 0;

        CviStr aIncMem;                // member to include
        CviStr aVelocity;              // velocity condition

        TRACE("INCLUDE CONTENT BLOCK:\n%s\n",
                               (const char *) itsContent);


        char    aIncAgain = false;     // include again flag


        CviStr aIncLine(itsContent);

        itsContent.Reset();            // reset final content

        const char *aTok =             // find possible
            aIncLine.Tok(":");         // token

        if (aTok != NULL)              // if valid

        {                              // begin grab include member name

             aIncMem = aTok;           // grab name

             itsStep->MarkTemplate(aIncMem); // mark inclusion of this template

        };                             // end grab include member name

        while (aTok != NULL)           // while token found

        {                              // begin process token

            CviStr aToken = aTok;      // get token into new string

            const char *aKey = aToken.Tok("=");

            if (aKey != NULL)          // if value OK

            {                          // begin process key/value pair

                if (!strcmp(aKey,      // if INC_AGAIN is set
                            "INC_AGAIN"))

                {                      // begin process INC_AGAIN

                    TRACE("Setting up to include %s again\n",
                                           (const char *) aIncMem);

                    aIncAgain = true;  // include again has been set

                }                      // end process INC_AGAIN

                if (!strcmp(aKey,      // if CONDITION
                            "COND"))

                {                      // begin test CONDITION

                    CviStr aExpressions
                             (aToken.Tok(""));

                    CviStr aReason;

                    TRACE("Processing include expressions %s\n",
                                           (const char *) aExpressions);

                    aVelocity.Reset(); // clear velocity translation

                    if (EvaluateExpression(aExpressions,
                                           itsStep,
                                           itsVariables,
                                           aReason,
                                           aVelocity) &&
                        strlen(aReason) == 0)
                    {

                        TRACE("Will include %s\n",
                                               (const char *) aIncMem);

                    }

                    else

                    if (strlen(aReason) > 0)

                    {

                        GetTaskCviPgm()->Print("%s: Error processing INCLUDE.\n%s\n",
                                               (const char *) itsStep->GetMember(),
                                               (const char *) aReason);

                        SETRC(aRc,8);  // set failed code

                    }

                    else               // otherwise

                    {                  // begin do not include

                        aIncMem.Reset();

                    };                 // end do not include

                };                     // end test CONDITION

            };                         // end process key/value pair

            aTok = aIncLine.Tok(":");  // next!

        };                             // end process token

        if (strlen(aIncMem) != 0)      // if member name set

        {                              // begin find include member

            WorkflowStep *aInc = itsIncludes;

            while (aInc != NULL &&     // while no match on include steps
                   strcmp(aInc->GetName(),
                          aIncMem))

                aInc = aInc->GetNext();

            if (aInc == NULL)          // if not found

                GetTaskCviPgm()->Warn("Include member %s was not found! (%p)\n",
                                       (const char *) aIncMem,
                                       (void *) itsIncludes);

            int aDoInc = false;        // include? I think not...for now

            CviStr aCheck;             // dupe check

            aCheck.Print(":%s:",
                         (const char *) aIncMem);

            if (aIncAgain)             // if include again is OK

            {                          // begin include by INC_AGAIN

                TRACE("Included %s via INC_AGAIN", (const char *) aIncMem);

                aDoInc = true;         // include

            }                          // end include by INC_AGAIN

            else                       // otherwise

            if (aInc != NULL    &&     // if found
                strstr(itsDupes,       // and not a duplicate
                       aCheck) == NULL)

            {                          // begin process include

                TRACE("Included %s - not duplicated\n", (const char *) aIncMem);

                itsDupes.Add(aCheck);  // add to list to prevent dupes

                aDoInc = true;         // include

            };                         // end process include

            if (aDoInc && aInc != NULL)// pass the test?

            {                          // begin finalize include

                itsContent.Set(aInc->GetContent());

                TRACE("INCLUDE %s: Content\n%s\n",
                                       (const char *) aInc->GetName(),
                                       (const char *) itsContent);

            };                         // end finalize include

        };                             // end find include member

        return(aRc);                   // return result

    }

} aInclude;


//****************************************************************************
// 
// Class        : WriteIf
//
// Description  : WriteIf processing
// 
//****************************************************************************
class WriteIf : public ContentBlock

{

private:

    CviStr itsCond;                    // condition
    CviStr itsReason;                  // condition evaluation error reason
    CviStr itsPrevCond;                // previous conditions

    bool itsLastMatch;                 // was previous statement a match?
    bool itsAnyMatch;                  // any condition match?
    bool itsWriteVelocity;             // write velocity?

public:

    WriteIf() : ContentBlock("WRITE_IF")
    {

        itsCall.Set("#WRITE_IF=");     // setup starting

        itsEnd.Reset();                // single-line item...no end tag

        itsLastMatch = false;          // no last match

        itsAnyMatch = false;           // nothing matches yet

        itsWriteVelocity = false;      // don't write velocity format

    };

    virtual void Reset()               // reset processing for new content

    {                                  // begin reset dupes

        ContentBlock::Reset();         // call base reset

        itsCond.Reset();               // reset condition

        itsPrevCond.Reset();           // clear prior conditions

        itsLastMatch = false;          // no last match

        itsAnyMatch = false;           // nothing matches yet

        itsWriteVelocity = false;      // don't write velocity format

    };                                 // end reset dupes

    virtual int GetParms(const char *theCall)
    {
        return(0);                     // will not process parms separately from processing
    }

    virtual int ProcessContent()
    {

        int aRc = 0;

        TRACE("WRITE_IF CONTENT BLOCK:\n%s\n",
                               (const char *) itsContent);


        CviStr aIncLine(itsContent);
        CviStr aIncData;               // data to write
        CviStr aRewritten;             // rewritten

        itsContent.Reset();            // reset final content

        const char *aTok =             // find possible
            aIncLine.Tok(":");         // token

        if (aTok != NULL)              // if token found

        {                              // begin process token

            if (*aTok == '+')          // if this is a continuation

            {                          // begin merge conditions

                if (strlen(itsCond) != 0)// if have previous condition

                {                      // begin setup to add to it

                    const char *aLast = ((const char *) itsCond) + strlen(itsCond) - 1;

                    if (*aLast == '+') // if it ends with +

                        itsCond.Replace(aLast,
                                        1,
                                        " "); // replace with space

                }                      // end set to add to it

                itsCond += aTok + 1;   // skip beyond the + sign

                TRACE("Continued WRITE_IF: %s\n", (const char *) itsCond);

            }                          // end merge conditions

            else                       // otherwise

               itsCond = aTok;         // set conditional

            aIncData = aIncLine.Tok("");// get remaining data to write

        };                             // end process token

        if (strlen(itsCond) != 0 &&      // if conditional set
            *(((const char *) itsCond) + strlen(itsCond) - 1) != '+')

        {                              // begin evaluate 

            TRACE("Processing WRITE_IF expression %s\n",
                                   (const char *) itsCond);

            if (strstr(itsCond,         // if ELSE
                       "ELSE") == itsCond)

            {                          // begin setup for ELSE

                const char *aElseCond = 
                    strstr(itsCond, "(");

                if (aElseCond != NULL) // if we have a condition

                    itsCond.Set(aElseCond); // use it

                else                   // otherwise

                    itsCond.Set("1");    // make condition be true

                TRACE("ELSE will evaluate %s\n", (const char *) itsCond);

            }                          // end setup for ELSE

            else                       // otherwise

            if (strstr(itsCond,        // if not continuation
                       "...") != itsCond)

            {                          // begin new IF

                itsLastMatch = false;  // no match yet

                itsAnyMatch = false;   // no match yet

                itsWriteVelocity = false; // do not write velocity

            };                         // end new IF

            aRewritten.Reset();        // reset

            if (itsWriteVelocity)

            {

                CviStr aTemp(aIncData);

                aIncData.Reset();
                if (strcmp(itsCond, "..."))
                {

// TO DO: Check why #[[]# ends up in the rewritten expression without quotes!

                    aRc = RewriteExpression(itsCond,
                                            itsStep,
                                            itsVariables,
                                            aRewritten);

                    if (aRc > 0)

                    {
                        itsReason.Set(aRewritten);
                    }
                    else
                    {
                        itsPrevCond.Print(" == false && \n( %s )",
                                          (const char *) aRewritten);
                    }

                };

                aIncData.Print("#if ( %s )\n%s\n#end",
                               (const char *) itsPrevCond,
                               (const char *) aTemp);

            }

            else

            if ((!strcmp(itsCond, "...") &&  // if continuation and
                 itsLastMatch) ||            // last entry matched
                (!itsAnyMatch &&             // if not match so far for this IF
                 EvaluateExpression(itsCond, // and expression is OK
                                    itsStep,
                                    itsVariables,
                                    itsReason,
                                    aRewritten) &&
                 strlen(aRewritten) == 0))

            {                          // begin write data

                TRACE("Will write data.\n");

                itsLastMatch = true;   // last match is OK

                itsAnyMatch = true;    // set any match

            }                          // end write data

            else                       // otherwise

            {                          // begin don't write

                itsLastMatch = false;  // no last match

                aIncData.Reset();      // don't write data if conditional fails

            };                         // end don't write

            if (strlen(itsReason) > 0)

            {                          // begin error

                GetTaskCviPgm()->Print("%s: Error processing WRITE_IF condition.\n%s\n",
                                           (const char *) itsStep->GetMember(),
                                           (const char *) itsReason);

                itsStep->DisplayStepsWith(itsIncludes, itsCond);

                itsReason.Reset();     // clear reason

                aRc = 8;               // set failed code

            };                         // end error

        };                             // end evaluate 

        if (strlen(aIncData) != 0)     // if data set

        {                              // begin write data

           itsContent.Set(aIncData);   // add content 
           itsContent.Add("\n");       // add newline on

        };                             // end find include member

        return(aRc);                   // return result

    }

} aWriteIf;

//****************************************************************************
// 
// Class        : IfThenElse
//
// Description  : IfThenElse processing
// 
//****************************************************************************
class IfThenElse : public ContentBlock

{

public:

    IfThenElse() : ContentBlock("IF")
    {

        itsMid.Set("#ELSE(");          // ELSE is a mid-content clause

    };

    virtual bool EvalMid(int &theRc)   // evaluate mid-level content
    {

        CviStr aReason;                // reason

        TRACE("EVAL MID: %s\n", (const char *) itsParms);

        if (strlen(itsParms) == 0)     // if no parms

            return true;               // true

        if (EvaluateExpression(itsParms, // if expression is OK
                               itsStep,
                               itsVariables,
                               aReason))

            return true;               // OK

        if (strlen(aReason) > 0)       // if bad evaluation

        {                              // begin give failure

            GetTaskCviPgm()->Print("%s: Error processing #IF.\n%s\n\n",
                                   (const char *) itsStep->GetMember(),
                                   (const char *) aReason);

            SETRC(theRc,8);            // failed

        };                             // end give failure

        return false;                  // not good

    }

} aIfThenElse;

//****************************************************************************
// 
// Class        : GenerateStep
//
// Description  : GenerateStep processing
// 
//****************************************************************************
class GenerateStep : public ContentBlock

{

private:

    VariableList itsVarList;

    CviStr itsStepContent;

public:

    GenerateStep() : ContentBlock("GENERATE_STEP")
    {

        itsCall.Set("#GENERATE_STEP("); // setup starting

        itsEnd.Reset();                // single-line item...no end tag

    };
    
    void Replace(const char *theFind, const char *theReplace)
    {
        itsVarList.Add(theFind, theReplace, 2); // Build list of variables
    }

    virtual int ProcessContent() 
    {

        MacroParms *aParm = GenParmList(itsParms);
        int aRc = 0;
        
        if (aParm != NULL)             // if parms OK

        {

            CviStr aName;              // step name
            CviStr aData;              // step data

            aData.Set(itsStep->GetOrigContent());

            aName.Set(aParm->itsData); // grab new step name

            if (strstr(aName, "#") != NULL)

                ProcessInternMacros(aName,  // it is conceivable it could use macros
                                    itsStep,
                                    itsVariables,
                                    false); // process any macros

            itsVariables->ReplaceWithValues(aName,
                                            false);

            Variable *aVar = itsVarList.GetFirst();
            while (aVar != NULL)       // replace iteration/for variables
            {
                aData.Replace // find/replace
                     (aVar->GetPropName(),
                      aVar->Value());

               aVar = itsVarList.GetNext();
            }

            aData.Add("--END CONTENT--\n");
            aData.Print("NAME=%s\n", (const char *) aName);

            TRACE("CREATING NEW STEP:\n%s\n",
                               (const char *) aData);

            aRc = itsStep->AddStep(aData);

            itsContent.Reset();        // clear out content

        }

        return(aRc);                   // return result

    }

} aGenerateStep;


//****************************************************************************
//
// Method       : WorkflowStep::WorkflowStep
//
// Description  : Constructor
//
//****************************************************************************
WorkflowStep::WorkflowStep(const CviStr &theName)

{                                      // begin constructor

itsMember = theName;                   // save member name

itsName = theName;                     // default step name = member name

itsLocked = false;                     // not yet locked into place

itsAutoEnable = true;                  // assume auto-enable

itsSuspend = false;                    // do not suspend

itsOptional = false;                   // not optional

itsHasOptional = false;                // no optional children yet

itsBypassIfSaveAsExists = false;       // do not bypass if file exists

itsGrp = NULL;                         // no group member is known

itsMaxRc = 0;                          // assume MAXRC of 0

itsBypass = FALSE;                     // not bypassed...yet

itsIgnoreDeps = FALSE;                 // do NOT ignore dependencies

itsReqCnt = 0;                         // no requirements lines yet

itsRefCnt = 0;                         // no references yet

itsWritten = 0;                        // not yet written

itsRealStep = 1;                       // assume it will be a "real" step yet

itsNext = NULL;                        // nothing is next

itsGrpNext = NULL;                     // group next

itsChild = NULL;                       // no children yet

itsGrpChild = NULL;                    // group child list

itsOrderChild = NULL;                  // ordered child list

itsGrpOrderChild = NULL;               // group ordered child list

itsDepPriorSteps = false;              // do not depend on all prior steps

itsPrev = NULL;                        // previous in final list

itsProcType = 0;                       // not processing yet

itsPromptList = NULL;                  // no variables to prompt

itsCombine = 0;                        // combine by default

itsComboCnt = 0;                       // reset combined step counter

itsTarget = TARGET_CONFIGURATION;      // default target

itsBaseStep = NULL;                    // base step

};                                     // end constructor


//****************************************************************************
//
// Method       : WorkflowStep::~WorkflowStep
//
// Description  : Destructor
//
//****************************************************************************
WorkflowStep::~WorkflowStep()

{                                      // begin constructor

if (itsPromptList != NULL)             // if allocated

    delete itsPromptList;              // remove it
                                       
}                                      // end constructor

//****************************************************************************
//
// Method       : WorkflowStep::ReParseData
//
// Description  : 1) Variable list
//
//****************************************************************************
int WorkflowStep::ReParseData(VariableList *thePropVars)

{                                      // begin ReParseData   

itsBypass = false;                     // do not bypass

itsReqCnt = 0;                         // no requirements yet

return(ParseData(itsReRunData,         // parse
                 thePropVars));        // data

}                                      // end ReParseData

//****************************************************************************
//
// Method       : WorkflowStep::ParseData
//
// Description  : 1) Data to parse
//                2) Variable list
//
//****************************************************************************
int WorkflowStep::ParseData(CviStr &theData,
                            VariableList *thePropVars)

{                                      // begin ParseData   =

const char *aLine;                     // individual line

int aInBlock = 0;                      // inside a block? Not yet
int aRc = 0;                           // OK so far

bool aSaveReRun = false;               // assume we will not save re-run data
bool aAddToOrig = false;               // assume we will not save to original data


if (strlen(itsOrigContent) == 0)       // if original content not yet set
    aAddToOrig = true;                 // add to original content
                                       
if (strlen(itsReRunData) == 0)         // if empty

    aSaveReRun = true;                 // we will save data for rerun

aLine = theData.Tok("\n");             // process each line

while (aLine != NULL)                  // while line found

{                                      // begin process line

    CviStr aLineStr;                   // line string


    const char *aComment =             // search for
        strstr(aLine, "# #");          // dev comment


    if (aComment != NULL)              // if found


        aLineStr.Print("%*.*s",
                       (int) (aComment - aLine),
                       (int) (aComment - aLine),
                       aLine);

    else                               // otherwise

    {

        aLineStr.Truncate();           // truncate
        aLineStr.Add(aLine);           // use full line

    }

    if (strlen(aLineStr) == 0)         // if no line

    {                                  // begin next

        aLine = theData.Tok("\n");     // grab next line

        continue;

    };                                 // end next 

    aLineStr.Trim(' ');                // remove trailing spaces

    if (aInBlock > 0)                  // if inside a block 

    {                                  // begin check block data

        if (strstr(aLineStr,             // if 
                   "--") == aLineStr  && // end of block
            !strcmp(aLineStr+strlen(aLineStr)-2,
                    "--"))

            aInBlock = 0;              // no longer in block

        if (aAddToOrig &&
            aInBlock != 'Y'  &&        // if not in DYNAMIC block
            aInBlock != 0)             // but in some other block
        {
            itsOrigContent.Print("%s\n",
                                 (const char *) aLine);

        }

        switch(aInBlock)               // check block type

        {                              // end block type

            case 'C'                 : // content
                  
            {

               const char *aPtr = aLineStr;
      
               if (!strcmp(itsType, "MACRO"))

               {                         // begin trim starting space

                   while (*aPtr != 0 &&  // while not the end and
                          *aPtr == ' ')  // while space
                       aPtr ++;          // bypass space
           
                    if (*aPtr != '#')     // if # not found
      
                       aPtr = aLineStr;  // use full line

               };                        // end trim starting space

// ----------------------------
// z/OSMF does NOT handle large SYSOUT data very well. It causes the JVM
// to get OutOfMemory even if the JVM is allowed 4G of memory...yikes!
// We will try SYSUDUMP for now...
//

               if (strstr(itsType, "JCL") != NULL &&
                   strstr(aPtr, "SYSUDUMP") != NULL &&
                   strstr(aPtr, "SYSOUT=*") != NULL)

               {

                    GetTaskCviPgm()->Warn("Removing SYSUDUMP statement from JCL to avoid z/OSMF bug in handling large output.\n");
                                       
               }

               else
// ----------------------------

               itsContent.Print("%s\n",  // write out
                                aPtr);   // the line
             }

             break;                    // end content

            case 'Y'                 : // dynamic
                  
              itsDynamic.Print("%s\n",  // write as-is
                               (const char *) aLineStr);

             break;                    // end dynamic

            case 'D'                 : // description
                  
              itsDescription.Print("%s\n",  // write as-is
                                    (const char *) aLineStr);

             break;                    // end instructions

            case 'I'                 : // instructions
                  
              itsInstructions.Print("%s\n",  // write as-is
                                    (const char *) aLineStr);

             break;                    // end instructions

        };                             // end block type

    }                                  // end check block data

    if (aInBlock == 0)                 // if not in a block anymore

    {                                  // begin check for block content

        if (aAddToOrig)
        {
            itsOrigContent.Print("%s\n",
                                 (const char *) aLine);

        }

        if (strstr(aLine,              // if found content
                   "--DYNAMIC--") == aLine)

        {                              // begin grab content

            aInBlock = 'Y';            // inside dynamic block

        };                             // end grab content

        if (strstr(aLine,              // if found content
                   "--INSTRUCTIONS--") == aLine)

        {                              // begin grab content

            aInBlock = 'I';            // inside instruction block

        };                             // end grab content

        if (strstr(aLine,              // if found content
                   "--CONTENT--") == aLine)

        {                              // begin grab content

            aInBlock = 'C';            // inside content block

        };                             // end grab content

        if (strstr(aLine,              // if found description
                   "--DESCRIPTION--") == aLine)

        {                              // begin grab description

            aInBlock = 'D';            // inside description block

        };                             // end grab description

        if (strstr(aLine,              // if found comment
                   "--COMMENT--") == aLine)

        {                              // begin grab comment

            aInBlock = '#';            // inside comment block

        };                             // end grab comment

    };                                 // end check for block content

    if (aInBlock == 0)                 // if not inside a block

    {                                  // begin process value= tags

        CviStr aSaveLine(aLineStr);    // saved line

        const char *aToken =           // grab first
            aLineStr.Tok("=");         // token

        if (aToken == NULL ||          // if no token or
            *aToken == '#')            // if comment

        {                              // begin do nothing

        }                              // end do nothing

        else

        if (!strcmp(aToken, "NAME"))       // if name

            itsName = aLineStr.Tok(" ");   // get name

        else

        if (!strcmp(aToken, "GROUP"))      // if group

            itsGroup = aLineStr.Tok(" ");  // get group

        else

        if (!strcmp(aToken, "TARGET"))     // if target

        {                              // begin get target

            const char *aValue = aLineStr.Tok(" \n");

            if (aValue != NULL)        // if OK

            {                          // begin test setting
                                       
                if (!strcmp(aValue, "DEPLOYMENT"))

                    itsTarget = TARGET_DEPLOYMENT;

                else

                if (!strcmp(aValue, "CONFIGURATION"))

                    itsTarget = TARGET_CONFIGURATION;

                else

                {

                    GetTaskCviPgm()->Print("%s: TARGET %s is invalid. Must be DEPLOYMENT or CONFIGURATION.\n",
                                           (const char *) itsMember,
                                           aValue);

                    SETRC(aRc,8);      // fail

                }

            }                          // end set testing

        }                              // end get taregt
                                       
        else

        if (!strcmp(aToken, "SAVE") ||     // if save
            !strcmp(aToken, "SAVE-AS"))    // or save-as 

            itsSaveAs = aLineStr.Tok("\n");  // get save-as

        else

        if (!strcmp(aToken, "DESCRIPTION"))// if description

            itsDescription = aLineStr.Tok("\n");  // get description

        else

        if (!strcmp(aToken, "TITLE"))       // if title

            itsTitle = aLineStr.Tok("\n");  // get title

        else

        if (!strcmp(aToken, "SKILLS"))      // if skills

            itsSkills = aLineStr.Tok("\n"); // get skill

        else

        if (!strcmp(aToken, "MAXRC"))      // if MAXRC

            itsMaxRc = atoi(aLineStr.Tok(" "));  // get maximum RC

        else

        if (!strcmp(aToken, "BYPASS"))     // if BYPASS

        {                                   // begin BYPASS options


            const char *aValue =            // grab
                aLineStr.Tok(" ");          // value

            if (!strcmp(aValue,             // if it should bypass if 
                        "SAVE_AS_EXISTS"))  // file exists

               itsBypassIfSaveAsExists = true;    // setup to bypass

            else                       // otherwise

            {                          // begin bad value

                GetTaskCviPgm()->Print("BAD BYPASS VALUE %s. SHOULD BE SAVE_AS_EXISTS\n",
                                       aValue);

                SETRC(aRc,8);          // set warning to ignore

            };                         // end bad value

        }                                   // end BYPASS options

        else

        if (!strcmp(aToken, "AUTO"))       // if automatic

        {                                  // begin determine auto setting

            const char *aValue = aLineStr.Tok(" ");

            if (!strcmp(aValue, "FALSE") ||
                !strcmp(aValue, "false") ||
                !strcmp(aValue, "OFF") ||
                !strcmp(aValue, "off") ||
                !strcmp(aValue, "NO") ||
                !strcmp(aValue, "no"))

                itsAutoEnable = false;

        }                                  // end determine auto setting

        else

        if (!strcmp(aToken, "ALWAYS_READY")) // if alyays read flag

        {                                  // begin always ready 

            const char *aValue = aLineStr.Tok(" ");

            if (!strcmp(aValue, "TRUE") ||
                !strcmp(aValue, "true") ||
                !strcmp(aValue, "ON") ||
                !strcmp(aValue, "on") ||
                !strcmp(aValue, "YES") ||
                !strcmp(aValue, "yes"))

                itsIgnoreDeps = true;      // ignore dependencies

        }                                  // end always ready

        else

        if (!strcmp(aToken, "SUSPEND"))    // suspend automation? 

        {                                  // begin determine auto setting

            const char *aValue = aLineStr.Tok(" ");

            if (!strcmp(aValue, "TRUE") ||
                !strcmp(aValue, "true") ||
                !strcmp(aValue, "ON") ||
                !strcmp(aValue, "on") ||
                !strcmp(aValue, "YES") ||
                !strcmp(aValue, "yes"))

                itsSuspend = true;         // mark to suspend automation 

        }                                  // end determine auto setting

        else

        if (!strcmp(aToken, "OPTIONAL"))   // optional?

        {                                  // begin determine optional setting

            const char *aValue = aLineStr.Tok(" ");

            if (!strcmp(aValue, "TRUE") ||
                !strcmp(aValue, "true") ||
                !strcmp(aValue, "ON") ||
                !strcmp(aValue, "on") ||
                !strcmp(aValue, "YES") ||
                !strcmp(aValue, "yes"))

                itsOptional = true;        // mark as optional

        }                                  // end determine optional setting

        else

        if (!strcmp(aToken, "COMBINE_STEPS")) // combine steps?

        {                                  // begin combine steps

            const char *aValue = aLineStr.Tok(" ");

            if (!strcmp(aValue, "TRUE") ||
                !strcmp(aValue, "true") ||
                !strcmp(aValue, "ON") ||
                !strcmp(aValue, "on") ||
                !strcmp(aValue, "YES") ||
                !strcmp(aValue, "yes"))
                                       
                itsCombine = 'Y';          // do combine

            if (!strcmp(aValue, "FALSE") ||
                !strcmp(aValue, "false") ||
                !strcmp(aValue, "OFF") ||
                !strcmp(aValue, "off") ||
                !strcmp(aValue, "NO") ||
                !strcmp(aValue, "no"))
                                       
                itsCombine = 'N';          // do not combine

        }                                  // end combine steps

        else

        if (!strcmp(aToken, "TYPE"))       // if type

            itsType = aLineStr.Tok(" ");   // get type

        else

        if (!strcmp(aToken, "JOBNAME"))    // if jobname

            itsJobName = aLineStr.Tok(" ");// get jobname

        else

        if (!strcmp(aToken, "SUCCESS"))    // if success pattern

            itsSuccess = aLineStr.Tok(" ");// set pattern

        else

        if (!strcmp(aToken, "DEPENDS") || // if dependency or
            !strcmp(aToken, "PREREQ"))    // prerequisite line

        {                                  // begin add dependency

            CviStr aDeps;                  // dependencies

            aDeps.Set(aLineStr.Tok(" "));  // get values

            if (strstr(aDeps,
                       "_ALL_PRIOR_STEPS_") != NULL)

                itsDepPriorSteps = true;   // it'll require all prior steps
                                           // to be completed

            else                           // otherwise

            {                              // begin add dependencies

                AddDepends(aDeps);         // add

                itsPreReqs = itsDepends;   // set prerequisites the same

            };                             // end add dependencies

        }                                  // end add dependency

        else

        if (!strcmp(aToken, "REQUIRES") || // if requirements list
            !strcmp(aToken, "INCLUDE_IF")) // INCLUDE_IF line

        {                              // begin add requirement


            if (aSaveReRun)            // if we should save for re-run

                itsReRunData.Print("%s\n",
                                   (const char *) aSaveLine);

            if (itsReqCnt == 0)        // if this is our first requirement

                itsBypass = TRUE;      // assume that we will bypass it

            itsReqCnt ++;              // bump requirement counter

            // NOTE: Each requirement line is treated as another "OR".
            //       In other words, only 1 requirement line needs to be satisified
            //       to not bypass.
            //       For a requirement line to be considered met, all entries in the
            //       line must be satisified.

            if (thePropVars != NULL && // if we are using property variables and
                itsBypass)             // if we are set to bypass

            {                          // begin process requirement line

                CviStr aExpressions    // get expressions
                         (aLineStr.Tok(""));

                CviStr aReason;        // failure reason

                itsBypass =            // evaluate expression. Set to bypass
                    !EvaluateExpression// if expressions evaluate to false
                      (aExpressions,
                       this,
                       thePropVars,
                       aReason);       

                if (strlen(aReason) > 0)

                {

                    GetTaskCviPgm()->Print("%s: Error processing INCLUDE_IF.\n%s\n\n",
                                           (const char *) itsMember,
                                           (const char *) aReason);

                    SETRC(aRc,8);      // set failed code

                }

                if (itsBypass)         // if bypassing

                    TRACE("Step %s bypassing so far - %s\n",
                         (const char *) itsName,
                         (const char *) aReason);

                else                   // begin satisfied

                    TRACE("Step %s, Requirement %s satisfied.\n",
                                           (const char *) itsName,
                                           (const char *) aExpressions);

            };                         // end process requirement line

        }                              // end add requirement

        else

        if (strstr(aLine,              // if found END
                   "--END ") == aLine)

        {                              // begin ignore

            // Do nothing
                                       
        }                              // end ignore

        else                           // unrecognized

        if (strlen(aToken) != 0)       // if we had a token at all

        {                              // begin unrecognized tag

            if (strlen(itsType) != 0 ||  // if we have a TYPE
                strlen(itsDescription) != 0 ||
                strlen(itsTitle) != 0)

                TRACE
                    ("Tag %s not recognized - ignoring.\n",
                     aToken);


                                       // do something to complain?

        };                             // end unrecognized tag

    };                                 // end process value= tags

    aLine = theData.Tok("\n");         // grab next line

};                                     // end process line

if (strlen(itsInstructions)==0 &&      // no instructions and
    strlen(itsDynamic)==0 &&
    !strcmp(itsType, "SAVE"))          // just saving member?

    itsInstructions.Set(itsContent);   // use content as instructions

if (strlen(itsInstructions) != 0 &&    // if instructions don't appear to have HTML tags to start
    *((const char *) itsInstructions) != '<')

{                                      // begin wrap with preformmated indicator

    CviStr anInst(itsInstructions);    // save

    itsInstructions.Reset();           // reset instructions

    itsInstructions.Print("<pre><font face=\"courier\">\n%s</font></pre>\n",
                          (const char *) anInst);

}                                      // end wrap with preformatted indicator

if (strlen(itsSkills) == 0)            // if no skills specified

    itsSkills = "Installer";           // default skills

if (strlen(itsDescription) == 0)       // if no description

    itsDescription = itsTitle;         // try to use the title

if (strlen(itsDescription) == 0)       // if no description

    itsDescription = itsName;          // use the name

if (strlen(itsTitle) == 0)             // if no title

    itsTitle = itsName;                // use the description

if (strlen(itsDynamic) == 0)
    itsOrigContent.Reset();            // reset original content

if (strlen(itsType) == 0              || // no type known
    !strcmp(itsType, "PROMPT")        || // not a prompt
    !strcmp(itsType, "TRANSLATE")     || // not a translation
    !strcmp(itsType, "DATASET_LIST")  || // not dataset list
    !strcmp(itsType, "DATASET_POPULATE") || // not dataset populate
    !strcmp(itsType, "INCLUDE")       || // not include
    !strcmp(itsType, "WFMACRO")       || // not WFMACRO
    !strcmp(itsType, "MACRO"))          // not macro

    itsRealStep = 0;                   // not a real step

if (itsRealStep &&
    !ValidateValue(itsName,
                   "0123456789-_"))    // if bad name

{

    GetTaskCviPgm()->Print("%s: NAME %s IS INVALID.\nMAY ONLY CONTAIN ENGLISH LETTERS, NUMBERS, HYPHEN, AND UNDERSCORE AND MUST START WITH A LETTER.\n",
                           (const char *) itsMember,
                           (const char *) itsName);

    SETRC(aRc,4);                      // set warning to ignore

};

if (strcmp(itsType, "GROUP") &&        // if not a group name
    strlen(itsContent) +               // and there is
    strlen(itsInstructions) == 0)      // no content

{                                      // begin malformed template or not a template

    TRACE("%s: DOES NOT APPEAR TO BE A TEMPLATE.\n",
                           (const char *) itsMember);

    SETRC(aRc,4);                      // set warning to ignore

};                                     // end malformed template or not a template

if (strlen(itsTitle) > 100)            // if title is too large

{                                      // begin complain

    GetTaskCviPgm()->Print("%s: TITLE MUST BE < 100 CHARACTERS.\n",
                           (const char *) itsMember);

    SETRC(aRc,8);                      // set failure

};                                     // end complain

if (strlen(itsSuccess) == 0 &&         // if no success pattern
    !strcmp(itsType, "TSO-UNIX-shell"))// and UNIX shell script

{                                      // begin complain

    GetTaskCviPgm()->Print("%s: SUCCESS PATTERN REQUIRED FOR TEMPLATE TYPE %s.\n",
                           (const char *) itsMember,
                           (const char *) itsType);

    SETRC(aRc,8);                      // set failure

};                                     // end complain
    
if (strcmp(itsType, "JCL")          && // if not JCL and
    strcmp(itsType, "TSO-REXX-JCL") && // not TSO-REXX_JCL and
    strcmp(itsType, "TSO-UNIX-shell") && // not TSO-UNIX-shell and
    strcmp(itsType, "DATASET_ALLOC") && // not dataset allocation
    strcmp(itsType, "DATASET_DELETE") && // not dataset delete
    strcmp(itsType, "DATASET_COPY")  && // not dataset copy
    strcmp(itsType, "DATASET_LIST")  && // not dataset list
    strcmp(itsType, "DATASET_POPULATE")   && // not dataset populate
    strcmp(itsType, "INCLUDE")       && // not include
    strcmp(itsType, "GROUP")         && // not group  
    strcmp(itsType, "MACRO")         && // not macro
    strcmp(itsType, "WFMACRO")       && // not workflow macro
    strcmp(itsType, "PROMPT")        && // not a prompt      
    strcmp(itsType, "SAVE")          && // not save
    strcmp(itsType, "TRANSLATE")     && // not translate
    strcmp(itsType, "INSTRUCTIONS"))    // nor instructions

{                                      // begin invalid type

    if (strcmp(itsType, ""))           // if type not empty

    {                                  // begin complain

        GetTaskCviPgm()->Print("%s: TEMPLATE TYPE %s IS INVALID.\n",
                               (const char *) itsMember,
                               (const char *) itsType);

        if (strcmp(itsType, "Instructions"))

           SETRC(aRc,8);               // set failure

    }                                  // end complain

    else                               // otherwise

        SETRC(aRc,4);                  // set warning to ignore

};                                     // end invalid type


return(aRc);                           // return result

};                                     // end ParseData

//****************************************************************************
//
// Method       : WorkflowStep::MarkTemplate
//
// Description  : Mark template member as used by a step
//
// Parameters   : 1) Member name to mark
//
//****************************************************************************
void WorkflowStep::MarkTemplate(const char *theMember)
{
  AddList(itsTemplates, theMember);    // add member to list
}

//****************************************************************************
//
// Method       : WorkflowStep::Translate
//
// Description  : Perform translation template work
//
//****************************************************************************
int WorkflowStep::Translate(CviStr &theContent)

{

int aRc = 0;                           // return code is OK

int aInTranslate = 0;                  // not in translate

CviStr aName;                          // name to search
CviStr aValue;                         // value to replace

CviStr aCommands(itsContent);          // content are the commands


const char *aCmdLine = NULL;

while ((aCmdLine = aCommands.Tok("\n")) != NULL)

{

    CviStr aLine(aCmdLine);


    while (strlen(aLine) >= 1 &&
           *((const char *)aLine + strlen(aLine) - 1) == '\\')

    {                                  // begin line continued

          aLine.Replace(strlen(aLine)-1, "\n"); // replace with newline

          aCmdLine = aCommands.Tok("\n");

          if (aCmdLine == NULL)        // no more?

              break;                   // break out

          else                         // otherwise

              aLine += aCmdLine;       // add to existing line

    }                                  // end line continued

    aLine.Replace("&slash;", "/");

    const char *aKey = NULL;

    if (aInTranslate > 0)

        aKey = (const char *) aLine;

    else

        aKey = aLine.Tok(" ");

    if (aKey == NULL)
        continue;

    if (aInTranslate == 2)

    {
        aInTranslate --;

        aName.Set(aKey);

    }

    else

    if (aInTranslate == 1)

    {

        aInTranslate --;

        aValue.Set(aKey);

        aName.VelocityEscape();

        TRACE("Translating %s to %s\n",
              (const char *) aName,
              (const char *) aValue);

        theContent.Replace(aName,      // perform
                           aValue);    // translation

        aName.VelocityUnescape();

        TRACE("Translating %s to %s\n",
              (const char *) aName,
              (const char *) aValue);

        theContent.Replace(aName,      // perform
                           aValue);    // translation

    }

    else

    if (!strcmp(aKey, "TRANSLATE"))

    {

        aInTranslate = 2;              // next two lines are translation name/value

    };

};

if (itsNext != NULL)

    itsNext->Translate(theContent);


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : WorkflowStep::AddStep
//
// Description  : 1) Parameters
//
//****************************************************************************
int WorkflowStep::AddStep(CviStr &theParms,
                          CviStr &theContent)
{

int aRc = 0;                           // all is well


WorkflowStep *aStep = new WorkflowStep(itsName);

aStep->itsMember.Set(itsMember);         
aStep->itsName.Set(itsName);             
aStep->itsGroup.Set(itsGroup);           
aStep->itsDepends.Set(itsDepends);     
aStep->itsPreReqs.Set(itsPreReqs);       
aStep->itsType.Set(itsType);            
aStep->itsDescription.Set(itsDescription);
aStep->itsSkills.Set(itsSkills);
aStep->itsTitle.Set(itsTitle);
//aStep->itsInstructions.Set(itsInstructions);
//aStep->itsContent.Set(theContent);
aStep->itsSaveAs.Set(itsSaveAs);
aStep->itsJobName.Set(itsJobName);
aStep->itsDescendents.Set(itsDescendents);
aStep->itsReqDescendents.Set(itsReqDescendents);
aStep->itsSuccess.Set(itsSuccess);
aStep->itsMaxRc = itsMaxRc;
aStep->itsBypass = itsBypass;
aStep->itsReqCnt = itsReqCnt;
aStep->itsSuspend = itsSuspend;
aStep->itsAutoEnable = itsAutoEnable;
aStep->itsOptional = itsOptional;
aStep->itsDepPriorSteps = itsDepPriorSteps;
aStep->itsRefCnt = itsRefCnt;
aStep->itsHasOptional = itsHasOptional;
aStep->itsIgnoreDeps = itsIgnoreDeps;
aStep->itsWritten = itsWritten;
aStep->itsRealStep = itsRealStep;
aStep->itsCombine = itsCombine;
aStep->itsBypassIfSaveAsExists = itsBypassIfSaveAsExists;
aStep->itsTarget = itsTarget;

aStep->itsBaseStep = this;             // we are based upon this step

if (itsNext != NULL)
    itsNext->itsPrev = aStep;

aStep->itsNext = itsNext;
itsNext = aStep;

if (itsProcType == 'I')                // if processing instructions
    aStep->itsInstructions.Set(theContent); // set instrunctions

if (itsProcType == 'C')                // if processing content
    aStep->itsContent.Set(theContent); // set content

CviStr aParms(theParms);               // parameters

const char *aParm = aParms.Parm(",");  // get parameter

while (aRc == 0 &&                     // while OK and
       aParm != NULL)                  // for all parms

{                                      // begin process parm

    CviStr aData(aParm);               // setup data

    if (strstr(aParm, "SAVE-AS") != aParm)
        aData.VelocityUnescape();

    TRACE("Parm %s\n", (const char *) aData);

    aRc = aStep->ParseData(aData);     // parse

    aParm = aParms.Parm(",");          // next!

};                                     // end process parm

WorkflowStep *aOtherSteps = this;      // clear it
while (aOtherSteps->itsPrev != NULL)   // while we have prior steps
    aOtherSteps = aOtherSteps->itsPrev;// go to the top

while (aOtherSteps != NULL          && // while OK
       (aStep == aOtherSteps ||
        strcmp(aOtherSteps->itsName, aStep->itsName)))
    aOtherSteps = aOtherSteps->itsNext;

if (aOtherSteps != NULL)               // if duplicate
{
    GetTaskCviPgm()->Print("ERROR: Dynamic step %s is a duplicate of an existing step.\n",
                           (const char *) aStep->itsName);
    aRc = 8;                           // fail
}
else

    GetTaskCviPgm()->Print("Created dynamic step %s\n",
                           (const char *) aStep->itsName);


return(aRc);                           // return

}


//****************************************************************************
//
// Method       : WorkflowStep::AddStep
//
// Description  : Add a dynamic step
//
// Parameters   : 1) Data block
//
//****************************************************************************
int WorkflowStep::AddStep(CviStr &theParms)
{

int aRc = 0;                           // all is well


WorkflowStep *aStep = new WorkflowStep(itsName);

CviStr aParms(theParms);               // parameters
aRc = aStep->ParseData(aParms);              

aStep->itsMember.Set(itsMember);         
aStep->itsDescendents.Set(itsDescendents);
aStep->itsReqDescendents.Set(itsReqDescendents);
aStep->itsReqCnt = itsReqCnt;
aStep->itsDepPriorSteps = itsDepPriorSteps;
aStep->itsRefCnt = itsRefCnt;
aStep->itsRealStep = itsRealStep;
aStep->itsBypassIfSaveAsExists = itsBypassIfSaveAsExists;
aStep->itsCombine = itsCombine;

aStep->itsDynamic.Reset();             // reset dynamic data
aStep->itsOrigContent.Reset();         // reset original content

aStep->itsDepends.Set(itsDepends);     
aStep->itsPreReqs.Set(itsPreReqs);       

aStep->itsDescendents.Set(itsDescendents);
aStep->itsReqDescendents.Set(itsReqDescendents);

aStep->itsReqCnt = itsReqCnt;
aStep->itsDepPriorSteps = itsDepPriorSteps;
aStep->itsRefCnt = itsRefCnt;
aStep->itsIgnoreDeps = itsIgnoreDeps;
aStep->itsReRunData.Set(itsReRunData);

aStep->itsGrp = itsGrp;                // should have the same parent

aStep->itsRealStep = itsRealStep;
aStep->itsBypassIfSaveAsExists = itsBypassIfSaveAsExists;

aStep->itsBypass = itsBypass;          // copy bypass flag

aStep->itsBaseStep = this;             // we are based upon this step

if (itsNext != NULL)
    itsNext->itsPrev = aStep;

aStep->itsNext = itsNext;
itsNext = aStep;

WorkflowStep *aOtherSteps = this;      // clear it
while (aOtherSteps->itsPrev != NULL)   // while we have prior steps
    aOtherSteps = aOtherSteps->itsPrev;// go to the top

while (aOtherSteps != NULL          && // while OK
       (aStep == aOtherSteps ||
        strcmp(aOtherSteps->itsName, aStep->itsName)))
    aOtherSteps = aOtherSteps->itsNext;

if (aOtherSteps != NULL)               // if duplicate
{
    GetTaskCviPgm()->Print("ERROR: Dynamic step %s is a duplicate of an existing step.\n",
                           (const char *) aStep->itsName);
    aRc = 8;                           // fail
}
else

    GetTaskCviPgm()->Print("Created dynamic step %s\n",
                           (const char *) aStep->itsName);


return(aRc);                           // return

}

//****************************************************************************
//
// Method       : AddDeps
//
// Description  : Add dependencies
//
// Parameters   : 1) Dependency list
//                2) Dependency list to add
//
//****************************************************************************
static void AddDeps(      CviStr &theDepList,
                    const CviStr &theDepends)

{                                      // begin AddDepends

CviStr aDepends = theDepends;          // copy list

const char *aDep = aDepends.Tok(",");  // dependency list

while (aDep != NULL)                   // for all tokens

{                                      // begin merge dependency

    CviStr aCheck;                     // dependency checker

    if (strlen(aDep) != 0)             // if length OK
 
    {                                  // begin check/add

        aCheck.Print(",%s,", aDep);    // form delimited token
    
        if (strstr(theDepList,         // if token is
                   aCheck) == NULL)    // not found in our list

        {                              // begin add to list

            theDepList.Add(aCheck);    // add dependency

            theDepList.Replace         // eliminate
                (",,", ",");           // double commas

        };                             // end add to list

    };                                 // end check/add

    aDep = aDepends.Tok(",");          // get next dependency

};                                     // end merge dependency

};                                     // end AddDepends

//****************************************************************************
//
// Method       : IsDep  
//
// Description  : Is item in list?
//
// Parameters   : 1) Dependency list
//                2) Dependency item to check
//
//****************************************************************************
static int  IsDep(      CviStr &theDepList,
                  const CviStr &theDepend)

{                                      // begin RemDepends

int aRc = false;                       // assume it isn't in the list


CviStr aCheck;                         // dependency checker

aCheck.Print(",%s,",                   // get item to check
             (const char *) theDepend);        

if (strstr(theDepList, aCheck) != NULL)// if found

    aRc = true;                        // mark as found


return(aRc);                           // return result

};                                     // end RemDepends

//****************************************************************************
//
// Method       : AddCoDep
//
// Description  : Add codependency to list
//
// Parameters   : 1) List
//                2) Codependency to check
//                3) List to add
//
//****************************************************************************
static void AddCoDep(      CviStr &theDepList,
                     const CviStr &theCoDep,
                     const CviStr &theDepends)

{

if (IsDep(theDepList, theCoDep))

    AddDeps(theDepList, theDepends);

}

//****************************************************************************
//
// Method       : RemDeps
//
// Description  : Remove dependencies
//
// Parameters   : 1) Dependency list
//                2) Dependency list to add
//
//****************************************************************************
static void RemDeps(      CviStr &theDepList,
                    const CviStr &theDepends)

{                                      // begin RemDepends

CviStr aDepends = theDepends;          // copy list

const char *aDep = aDepends.Tok(",");  // dependency list

while (aDep != NULL)                   // for all tokens

{                                      // begin merge dependency

    CviStr aCheck;                     // dependency checker

    aCheck.Print(",%s,", aDep);        // form delimited token

    theDepList.Replace(aCheck, ",");   // remove dependency

    aDep = aDepends.Tok(",");          // get next dependency

};                                     // end merge dependency

theDepList.Replace                     // eliminate
    (",,", ",");                       // double commas

if (!strcmp(theDepList, ","))          // if empty

    theDepList.Reset();                // clear it

};                                     // end RemDepends

//****************************************************************************
//
// Method       : WorkflowStep::AddDepends
//
// Description  : Add dependencies
//
// Parameters   : 1) Dependency list
//
//****************************************************************************
void WorkflowStep::AddDepends(const CviStr &theDepends)

{                                      // begin AddDepends

AddDeps(itsDepends, theDepends);       // use static function

if (strstr(theDepends, itsName))       // if our name may be in it

    RemDeps(itsDepends, itsName);      // remove ourself

};                                     // end AddDepends

//****************************************************************************
//
// Method       : WorkflowStep::AddToBaseDeps
//
// Description  : Add dynamic step to base dependencies 
//
// Parameters   : None
//
//****************************************************************************
void WorkflowStep::AddToBaseDeps(WorkflowStep *theDynStep)


{                                      // begin AddDepends

AddCoDep(itsDepends,                   // add to list if our base is in list
         theDynStep->itsBaseStep->itsName,               
         theDynStep->itsName);              
                                       
AddCoDep(itsPreReqs,                   // add to list if our base is in list
         theDynStep->itsBaseStep->itsName,               
         theDynStep->itsName);              

};                                     // end AddDepends

//****************************************************************************
//
// Method       : WorkflowStep::AddDescendents
//
// Description  : Add to descendent list
//
// Parameters   : 1) Descendent list
//
//****************************************************************************
void WorkflowStep::AddDescendents(const CviStr &theDescendents)

{                                      // begin AddDescendents

AddDeps(itsDescendents, theDescendents); // use static function

};                                     // end AddDescendents

//****************************************************************************
//
// Method       : WorkflowStep::AddReqDescendents
//
// Description  : Add to required descendent list
//
// Parameters   : 1) Required descendent list
//
//****************************************************************************
void WorkflowStep::AddReqDescendents(const CviStr &theDescendents)

{                                      // begin AddReqDescendents

AddDeps(itsReqDescendents, theDescendents); // use static function

};                                     // end AddReqDescendents

//****************************************************************************
//
// Method       : WorkflowStep::AddPreReqs
//
// Description  : Add prerequisites
//
// Parameters   : 1) Prequisite list
//
//****************************************************************************
void WorkflowStep::AddPreReqs(const CviStr &thePreReqs)

{                                      // begin AddPreReqs

AddDeps(itsPreReqs, thePreReqs);       // use static function

if (strstr(itsPreReqs, itsName))       // if our name may be in it

    RemDeps(itsPreReqs, itsName);      // remove ourself

};                                     // end AddPreReqs

//****************************************************************************
//
// Method       : WorkflowStep::RemDepends
//
// Description  : Remove dependencies
//
// Parameters   : 1) Dependency list
//
//****************************************************************************
void WorkflowStep::RemDepends(const CviStr &theDepends)

{                                      // begin RemDepends

RemDeps(itsDepends, theDepends);       // use static function

};                                     // end RemDepends

//****************************************************************************
//
// Method       : WorkflowStep::RemPreReqs
//
// Description  : Remove prereqs
//
// Parameters   : 1) Prereq list
//
//****************************************************************************
void WorkflowStep::RemPreReqs(const CviStr &thePreReqs)

{                                      // begin RemPreReqs

RemDeps(itsPreReqs, thePreReqs);       // use static function

};                                     // end RemPreReqs

//****************************************************************************
//
// Method       : ProcessSplits
//
// Description  : Process line splits, if needed
//
// Parameters   : 1) Step
//
//****************************************************************************
void ProcessSplits(CviStr &theContent)

{                                      // begin ProcessSplits

int aRc = 0;                           // all is well

int aSplitStyle = 0;                   // no split style set yet
int aMaxLen = 80;                      // 80 is maximum length

CviStr aCmd("#SPLIT_STYLE(");          // command to examine


TRACE("Processing splits\n");

if (strstr(theContent, aCmd) != NULL)  // if it has #SPLIT_STYLE content

{                                      // begin do splits

    CviStr aContent(theContent);       // get our content

    theContent.Reset();                // reset our content

    const char *aLine = aContent.Tok("\n");// grab first line

    while (aLine != NULL)              // for all lines

    {                                  // begin search for content

        if (strstr(aLine,              // if content starts with #SPLIT_STYLE=
                   aCmd) == aLine)     

        {                              // begin set style

            const char *aOpen = strstr(aLine, aCmd) + strlen(aCmd);
            const char *aEnd = strstr(aOpen, ")");

            if (aEnd != NULL)          // if fully-formed

            {                          // begin process parms

                CviStr aParms(aOpen, (int) (aEnd - aOpen));

                const char *aParm = aParms.Parm(",");

                if (aParm != NULL &&   // if OK and
                    !strcmp(aParm, "PROPERTY"))
                    aSplitStyle = 'P'; // use property style
                else
                if (aParm != NULL &&   // if OK and
                    !strcmp(aParm, "NOMARK"))
                    aSplitStyle = 'N'; // use property style
                else
                if (aParm != NULL &&   // if OK and
                    !strcmp(aParm, "JCL")) 
                {
                    aSplitStyle = 'J'; // use JCL style
                    aMaxLen = 71;
                }
                else                   // otherwise
                if (aParm != NULL &&   // if OK and
                    !strcmp(aParm, "VSET")) 
                {
                    aSplitStyle = 'V'; // use VSET style
                    aMaxLen = 71;
                }
                else                   // otherwise
                if (aParm != NULL)     // if we have parm but invalid
                    DPRINT("WARNING: #SPLIT_STYLE of %s is not valid",
                           aParm)
                else                   // otherwise

                {
                    aSplitStyle = 0;   // reset
                    aMaxLen = 80;      // to default
                }

                aParm = NULL;          // no parm

                if (aSplitStyle != 0)  // if OK

                    aParm = aParms.Parm(",");

                if (aParm != NULL)     // if OK

                    aMaxLen = atoi(aParm); // set max length

            };                         // end process parms


        }                              // end set style

        else                           // otherwise

        if (aSplitStyle != 0)          // if we want to check for split

        {

            CviStr anUnescaped(aLine);

            anUnescaped.VelocityUnescape();

            const char *aPlainLine = anUnescaped;

            if (strlen(aPlainLine) > aMaxLen)
            {

                int aTot = 0;
                int aAdj = 0;
                int aTotLen = strlen(aPlainLine);

                while (aTot < aTotLen)
                {
                    if (aTot > 0)
                    {
                        if (aSplitStyle == 'N')
                           theContent.Print("\n");
                        else
                           theContent.Print("\\\n");
                        if (aSplitStyle == 'J')
                        {
                            theContent.Print("//             ");
                            aAdj=15;
                        }
                        else
                        if (aSplitStyle == 'V')
                        {
                            theContent.Print("//*            ");
                            aAdj=15;
                        }
                    }

                    int aLen = strlen(aPlainLine + aTot);

                    if (aSplitStyle == 'J' ||  // JCL must end at preclise location
                        aSplitStyle == 'V' ||  // same with VSET
                        aSplitStyle == 'N')    // NOMARK will just end
                    {
                        if (aLen > (aMaxLen-aAdj))
                            aLen = (aMaxLen-aAdj);
                    }                  
                    else                    // otherwise, end 1 character short to account for continuation char
                    {
                        if (aLen > aMaxLen-1)
                            aLen = aMaxLen-1;
                    }

                    CviStr anEscLine(aPlainLine + aTot,
                                     aLen);

                    anEscLine.VelocityEscape();

                    theContent.Print("%s",
                                     (const char *) anEscLine);

                    aTot += aLen;
                };
                theContent.Print("\n");

            }
            else
            {
                theContent.Add(aLine);
                theContent.Add("\n");
            }


        } 

        else

        {
            theContent.Add(aLine);  // add content to new content as-is
            theContent.Add("\n");   // add newline
        }

        aLine = aContent.Tok("\n");    // grab next line
    
    };                                 // end search for content

};                                     // end search for #SPLIT_STYLE

TRACE("SPLIT_STYLE EXIT: %d\n", aRc);

};                                     // end ProcessSplits

//****************************************************************************
//
// Method       : ProcessTab
//
// Description  : Process #TAB
//
// Parameters   : 1) Content
//              
//****************************************************************************
static void ProcessTab(CviStr &theContent)

{

CviStr aMacro("#TAB(");                // primary macro 

const char *aData =                    // find first macro
    strstr(theContent, aMacro);        // statement

while (aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, ")");

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        CviStr aTag;                   // tag holder
        CviStr aParms;                 // parmeters
        CviStr aPad;                   // padding to add

        int    aTabCol = 0;            // tab column

        int    aLen = 0;               // pad length

        aTag.Print("%*.*s",            // setup just the tag
                   (int) (aEnd-aData)+1,
                   (int) (aEnd-aData)+1,
                   aData);
                                       
        const char *aName = strstr(aData, "(");

        aName ++;                      // bump to get name

        aParms.Print("%*.*s",          // get all parameters
                     (int) (aEnd-aName),
                     (int) (aEnd-aName),
                     aName);

        if (strlen(aParms) == 0)       // if no data

            aParms.Set("72");          // assume 72

        const char *aParm = aParms.Tok(",");

        if (aParm != NULL)             // first tab set?

        {                              // begin find tab stop

            int aTabWidth = 0;         // tab width

            aTabCol = atoi(aParm);     // set length

            if (aTabCol > 0)           // if set

                aTabCol --;            // decrement tab column to make 0-relative

            aParm = aParms.Tok(",");   // find next

            if (aParm != NULL)         // if OK

                aTabWidth = atoi(aParm);// get width

            const char *aContent =         // get start of
                (const char *) theContent; // content

            const char *aLine = aData; // grab location

            while (*aLine != '\n' &&   // while previous end of line not found and
                   aLine != aContent)  // not at the start of the string

                aLine --;              // back up

            if (aLine != aContent)     // if not start of string
    
                aLine ++;              // bump past newline
        

            CviStr aLineData;          // line data

            aLineData.Add(aLine, (int)(aData - aLine));

            aLineData.VelocityUnescape(); // remove Velocity escape

            ReplaceConditionalVars(aLineData);// replace any conditionals remaining with empty strings

            aLen = aTabCol -           // get padding length
                    strlen(aLineData); 

            while (aLen < 0         && // while not good
                   aTabWidth > 0)      // and tab width set

                aLen += aTabWidth;     // bump 

            TRACE("Tab and width %d %d Len %d\n",
                                   aTabCol,
                                   aTabWidth,
                                   aLen);

            if (aLen > 0)              // if length is legit
    
                aPad.Print("%*.*s",    // setup padding
                           aLen,
                           aLen,
                           " ");       

        };                             // end find tab stop

        theContent.Replace(aData, strlen(aTag), aPad);

//        TRACE("New Data: %s\n",
//                               (const char *) theContent + aFoundIdx);

        aData = strstr((const char *) theContent + aFoundIdx + strlen(aPad), aMacro);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro

};                                     // end process macro 

//****************************************************************************
//
// Method       : WorkflowStep::FinalizeContent
//
// Description  : Prepare content for XML
//
// Parameters   : 1) Buffer to prepare
//
//****************************************************************************
void WorkflowStep::FinalizeContent(CviStr &theStr)

{                                      // begin finalize content

theStr.XMLEscape();                    // use escape sequences

if (strstr(theStr, "${NL}") != NULL || // if $NL is being used
    strstr(theStr, "$NL") != NULL)

{                                      // begin add $NL set

    CviStr aData(theStr);              // new data

    theStr.Reset();                    // clear destination string

    theStr.Print(SET_NL);              // set newline variable

    theStr.Add(aData);                 // add remaining data

};                                     // end add $NL set

if (strlen(theStr) > 0 &&              // if non-empty content
    *(const char *) theStr == ' ')     // and starts with space

    theStr.Replace(0, "$!{ZBMCSPC} "); // replace with bogus Velocity variable to preserve leading whitespace

};                                     // end write macro

//****************************************************************************
//
// Method       : WorkflowStep::PrepareContent
//
// Description  : Prepare content for XML
//
// Parameters   : 1) Buffer to prepare
//                2) Workflow variables (not really used anymore)
//                3) Property values
//                4) Add line breaks?
//                5) Sub JCL variables?
//
//****************************************************************************
void WorkflowStep::PrepareContent(CviStr &theStr,
                                  VariableList *theWfVars,
                                  VariableList *thePropVars,
                                  char theAddBr,
                                  char theSubJCLVar)


{                                      // begin write macro

//
// At one time it was thought that maybe we should let Velocity do all substitution.
// That sounds good, but the problem is that things like #TAB wouldn't work correctly.
// So we attempt to do all substitution here. If variables are having to be left
// in the XML then we probably need to look at adding funtionality that resolves it
// here.
// It is believed that no variables are being left, other than things that are already
// prefixed such as ${_workflow-workflowOwnerUpper}. That is a workflow engine variable
// and we can't be replacing that.

if (thePropVars != NULL)               // if we have property values

{                                      // begin replace with values

    thePropVars->                      // replace with values
       ReplaceWithValues(theStr);      

    if (theSubJCLVar)                  // if replace JCL variables
    {
        thePropVars->TranslateJCLValues // translate JCL-style variables
          (theStr);      
        theStr.ReplaceSymbols();       // replace system symbols
    }

};                                     // end replace with values

if ( theWfVars != NULL)                // if we have workflow variables

    theWfVars->                        // replace with Velocity names
      ReplaceWithVelocityNames(theStr);

ProcessTab(theStr);                    // process #TAB. Should probably be
                                       // the last thing to process before escaping
                                       // characters as other substitutions taking place
                                       // after this would potentially mess up the result

ProcessSplits(theStr);                 // process splits
                                 
if (strstr(theStr, "<pre>") == NULL && // if not preformatted text
    theWfVars != NULL)                 // and not instructions/content
{

    while (strstr(theStr, "\n\n") != NULL)
       theStr.Replace("\n\n", "\n$NL\n");// escape blank lines

}

};                                     // end write macro

//****************************************************************************
//
// Method       : WorkflowStep::AddConditions 
//
// Description  : AddConditions XML           
//
// Parameters   : 1) Output string       
//
// Returns      : Count of items processed
//
// z/OSMF currently checks step dependencies first and then conditions.
// We would like to be able to disable steps if they are not needed based
// upon some boolean variables (such as DBC_NEEDED).
// Ideally, we would set up step dependencies as z/OSMF workflow step
// dependencies natively. However, we can't do that because the conditions
// are not checked until the dependencies are checked. This means if a step
// isn't even relevant then it shows up as 'Not Ready' until its dependencies
// are satisfied and then it goes into a state indicating it isn't relevant...
// On top of this, there is not a way to say a step isn't relevant, so we are
// currently marking it skipped. IBM has indicated they may add the feature to
// mark a step as 'hidden', in which case we'd set steps to hidden rather than
// skipped.
//
// Current solution is to only add prereqStep statements and simply not add
// steps that aren't going to be used. This means the workflow must be built
// at the customer site.
//
// Leaving much of the more complex code in place as reference should we
// want to add expressions in the future.
//
//****************************************************************************
int WorkflowStep::AddConditions(CviStr &theXml,
                                VariableList *thePropVars)

{                                      // begin AddConditions

int aRc = 0;                           // return code


if (strcmp(itsType, "GROUP"))          // if not a group step

{                                      // begin add conditions

    WorkflowStep *aStep = this;        // start with our step

    while (aStep != NULL            && // while we have a step and
           aStep->itsGrp != NULL)      // it is a group member

    {                                  // begin add group dependencies

        AddPreReqs                     // we need our parent prerequisites
            (aStep->itsGrp->itsPreReqs);

        aStep = aStep->itsGrp;         // process its group as well

    };                                 // end add group dependencies

    if (itsDepPriorSteps)              // if it depends on all prior steps

    {                                  // begin add prior steps

        TRACE("Adding prior steps as prerequisites.\n");

        aStep = itsPrev;               // run up the previous chain

        while (aStep != NULL)          // for all steps

        {                              // begin add to our prerequisite list

            TRACE(" Previous: %s\n",
                                   (const char *) aStep->GetName());

            if (!aStep->Optional() &&  // if step is not optional
                strcmp(aStep->itsType, // and it is not
                       "GROUP"))       // a GROUP step

               AddPreReqs(aStep->itsName); // add it to our prereq list

            aStep = aStep->itsPrev;    // get previous

        };                             // end add to our prerequisite list

    };                                 // end add prior steps

    CviStr aReqs(itsPreReqs);          // get prerequisite dependencies

    const char *aReq = NULL;           // no requirements yet

    if (!itsIgnoreDeps)                // if not ignoring dependencies

         aReq = aReqs.Tok(",");        // get prerequisite

    else                               // otherwise

        TRACE("%s will have no dependencies\n", (const char *) itsName);
    
    while (aReq != NULL)               // for all of them
   
    {                                  // begin add prerequisite

        if (strlen(aReq) > 0)          // if dependency OK

        {                              // begin add prereq

#ifdef _COMPLEX_DEPENDS_
        // If we use state expressions to hide/skip steps then we must add
        // prerequisite steps as expresions, too.
            if (strlen(aReqends) != 0) // if empty
                                    
                aReqends.Add(" &amp;&amp;\n         ");  // add AND

            aReqends.Print("( ${%s.stepState}.startsWith(\"Complete\") || ${%s.stepState}.startsWith(\"Skipped\") ) ",
                           aReq, aReq);
#else
            theXml.Print("  <prereqStep name=\"%s\"/>\n",
                         aReq);

#endif
        };                             // end add prereq
   
        aReq = aReqs.Tok(",");         // get next one
                                       
    };                                 // end add prerequisite

    if (itsBypassIfSaveAsExists)       // if we want to bypass if SaveAs destination exists

    {                                  // begin determine if it exists

        CviStr aFile(itsSaveAs);       // full name

        if (thePropVars != NULL)       // if properties set

            thePropVars->              // replace variables
               ReplaceWithValues(aFile, false);

        const char *aDSN = aFile.Tok("(");
        const char *aMem = aFile.Tok("()");

        if (aDSN != NULL &&            // if DSN OK and
            aMem != NULL)              // member OK

        {                              // begin check

            MemberList aMemList;

            TRACE("Checking %s %s\n", aDSN, aMem);

            Member *aMember = aMemList.GetMembers(aDSN);

            if (aMember != NULL)      // if OK

            {                          // begin process list

                while (aMember != NULL)

                {

                    const char *aStr = (const char *) *aMember;

                    if (!strcmp(aStr, aMem))       // if found

                    {                              // begin add conditions

                        TRACE("FOUND! Marking to Skip step.\n");

                        theXml.Print("    <condition>\n");
                        theXml.Print("      <expression>true</expression>\n"); // always process states
                        theXml.Print("      <description/>\n");
                        theXml.Print("      <targetStateSet>\n");
                        theXml.Print("        <stateExpression state=\"skipped\">true</stateExpression>\n");
                        theXml.Print("      </targetStateSet>\n");
                        theXml.Print("  </condition>\n");  // end condition

                        break;                     // exit loop

                    };                             // and add conditions

                    aMember = aMember->GetNext();  // get next member

                };                                 // end process member

            };                         // end process list

        };                             // end check

    };                                 // end determine if it exists

};                                     // end add conditions

return(aRc);                           // we are OK

};                                     // end AddConditions

//****************************************************************************
//
// Method       : WorkflowStep::ResetReferences
//
// Description  : Reset macro reference variables
//
//****************************************************************************
void WorkflowStep::ResetReferences(WorkflowStep *theSteps,
                                   VariableList *theVars)

{                                      // begin ResetReferences

WorkflowStep *aStep = theSteps;        // get head of list

while (aStep != NULL)                  // for all macros

{                                      // begin reset reference counter variable

    if (aStep->itsRefCnt != 0)         // if not zero already

    {                                  // begin reset/clear variable

        aStep->itsRefCnt = 0;          // not referenced

        aStep->SetReferenceVar(theVars); // set reference variable to match

    };                                 // end reset/clear variable

    aStep = aStep->GetNext();          // next!

};                                     // end reset reference counter variable

};                                     // end ResetReferences

//****************************************************************************
//
// Method       : WorkflowStep::BumpReferences
//
// Description  : Bump reference count and create new variable
//
//****************************************************************************
void WorkflowStep::BumpReferences(VariableList *theVars)

{                                      // begin BumpReferences

itsRefCnt ++;                          // bump number

SetReferenceVar(theVars);              // set reference variable

};                                     // end BumpReferences

//****************************************************************************
//
// Method       : WorkflowStep::SetReferenceVar
//
// Description  : Set reference variable
//
//****************************************************************************
void WorkflowStep::SetReferenceVar(VariableList *theVars)

{                                      // begin BumpReferences

CviStr aVarName;                       // variable name
CviStr aValue;                         // reference string


aVarName.Print("%s_REF",               // reference counter variable name
              (const char *) GetName());

aValue.Print("%d",                     // setup reference count variable value
             itsRefCnt);        

theVars->Add(aVarName,                 // add to the list
             aValue,                   // or replace existing entry
             2);

TRACE("Set %s to %s\n",
                       (const char *) aVarName,
                       (const char *) aValue);


};                                     // end BumpReferences

//****************************************************************************
//
// Method       : WorkflowStep::DisplayStepsWith
//                                      
// Description  : Display steps containing the given content
//
// Parameters   : 1) Step list
//                2) Content
//
//****************************************************************************
void WorkflowStep::DisplayStepsWith(WorkflowStep *theSteps,
                                    const char *theData)

{

TRACE("Looking for steps containing condition %s\n",
                       theData);

WorkflowStep *aStep = theSteps;

while (aStep != NULL)                  // for all steps

{                                      // begin check step

    if (strstr(aStep->GetContent(),    // if content
               theData))               // contains this data

    {                                  // begin display

        GetTaskCviPgm()->Print(" This condition appears in member %s\n",
                               (const char *) aStep->GetMember());

    };                                 // end display

    aStep = aStep->GetNext();          // next!

};                                     // end check step
 
}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessDynamic
//
// Description  : Process workflow dynamic section
//
// Parameters   : 1) Variable list
//
//****************************************************************************
int WorkflowStep::ProcessDynamic(VariableList *theVars,
                                 WorkflowStep *theIncludes,
                                 WorkflowStep *theWfMacro)

{

int aRc = 0;                           // OK so far


itsProcType = 'Y';                     // process type


if (aRc == 0)

    aRc = ProcessStatements(itsDynamic,    // process
                            theVars,       // dynamic          
                            theIncludes,
                            theWfMacro);

itsProcType = 0;                       // not processing

return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessStatements
//
// Description  : Process workflow statements
//
// Parameters   : 1) String to operate upon
//                2) Variable list
//                3) Include list
//                4) WF Macro list
//
//****************************************************************************
int WorkflowStep::ProcessStatements(VariableList *theVars,
                                    WorkflowStep *theIncludes,
                                    WorkflowStep *theWfMacros)

{

int aRc = 0;                           // all is well


int aOrigContentLen = strlen(itsContent);
int aOrigInstructLen = strlen(itsInstructions);

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aRc == 0)                          // if OK so far

{                                      // begin process instructions

    itsProcType = 'I';                 // process instructions

    aRc = ProcessStatements(itsInstructions,
                            theVars,
                            theIncludes,
                            theWfMacros);

};                                     // end process instructions

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aRc == 0)                          // if OK so far

{                                      // begin process content

    itsProcType = 'C';                 // processing content

    aRc = ProcessStatements(itsContent,
                            theVars,
                            theIncludes,
                            theWfMacros);

};                                     // end process content

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aRc == 0)                          // if OK so far

{                                      // begin process save-as

    itsProcType = 'S';                 // save-as

    aRc = ProcessStatements(itsSaveAs,
                            theVars,
                            theIncludes,
                            theWfMacros);

};                                     // end process save-as

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aRc == 0)                          // if OK so far

{                                      // begin process titles

    itsProcType = 'S';                 // save-as

    aRc = ProcessStatements(itsTitle,
                            theVars,
                            theIncludes,
                            theWfMacros);

};                                     // end process titles

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aRc == 0)                          // if OK so far

{                                      // begin process descriptions

    itsProcType = 'S';                 // save-as

    aRc = ProcessStatements(itsDescription,
                            theVars,
                            theIncludes,
                            theWfMacros);

};                                     // end process descriptions

itsProcType = 0;                       // not processing now

ResetReferences(theWfMacros,           // reset reference counters
                theVars);              // and reference variables

if (aOrigContentLen > 0 &&             // if we had content
    strlen(itsContent) == 0)           // but now we do not

{                                      // begin bypass step

    TRACE("Step %s no loner has content - bypassing.\n",
                           (const char *) itsName);
    
    itsBypass = TRUE;                  // bypass this step

};                                     // end bypass step

if (aOrigInstructLen > 0 &&            // if we had instruction data
    strlen(itsInstructions) == 0)      // but now we do not

{                                      // begin bypass step

    TRACE("Step %s no longer has instructions - bypassing.\n",
                           (const char *) itsName);
    
    itsBypass = TRUE;                  // bypass this step

};                                     // end bypass step


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessStatements
//
// Description  : Process workflow statements
//
// Parameters   : 1) String to operate upon
//                2) Variable list
//                3) Include list
//                4) WF Macro list
//
//****************************************************************************
int WorkflowStep::ProcessStatements(CviStr &theContent,
                                    VariableList *theVars,
                                    WorkflowStep *theIncludes,
                                    WorkflowStep *theWfMacros)

{

int aRc = 0;                           // all is well

        
// If no # characters, no need to process...
if (strstr(theContent, "#") == NULL) return 0;

CviStr aCurLine;                       // current line

ContentBlock::itsTop->Reset();         // reset processing

const char *aLine = (const char *) theContent;

while (aRc == 0 &&                     // while we have
       aLine != NULL)                  // data to process and OK

{                                      // begin process line

    aCurLine.Reset();                  // reset current line

    const char *aNl = strstr(aLine, "\n");

    int anOffset = (int) (aLine - (const char *) theContent);

    while (*aLine == ' ') aLine ++;

    if (*aLine == '#')                // if might be control statement

    {                                  // begin setup search line
        if (aNl > aLine)
         aCurLine.Add(aLine, (int) (aNl - aLine));
        else
         aCurLine.Add(aLine);

    };                                 // end setup search line

    if (itsProcType == 'Y' &&
        strstr(aCurLine, "#SET_VAR(") == (const char *) aCurLine)
    {

        const char *aStart = 
            strstr(aCurLine, "#SET_VAR(") + strlen("#SET_VAR(");

        CviStr aParms(aStart);

        const char *aTok = aParms.Tok(",");

        if (aTok != NULL)
        {
            CviStr aVar(aTok);

            const char *aData = aParms.Tok("");

            if (aData != NULL)
            {
                CviStr aVarData;
                
                aVarData.Add(aData, strlen(aData)-1);

                aGenerateStep.Replace(aVar, aVarData);
            }

        }

        if (aNl != NULL)               // if have a next line
            aLine = aNl + 1;           // use next line
        else                           // otherwise
            aLine = NULL;              // terminate

    };                                 // end generate macro

    if (*aLine == '#' && (strstr(aCurLine, "(") != NULL ||
                          strstr(aCurLine, "=") != NULL))
    {
    
        int aFc = ContentBlock::itsTop->ProcessBlock(theContent,
                                                     anOffset,
                                                     this,
                                                     theVars,
                                                     theIncludes);

        if (aFc >= 0)                  // if processed

        {                              // begin reset processing

            aLine = (const char *) theContent + anOffset;

            aRc = aFc;                 // use this return code

        }                              // end reset processing
        else                           // otherwise
        if (aNl != NULL)               // if have a next line
            aLine = aNl + 1;           // use next line
        else                           // otherwise
            aLine = NULL;              // terminate

    }
    else                               // otherwise
    if (aNl != NULL)                   // if have a next line
        aLine = aNl + 1;               // use next line
    else                               // otherwise
        aLine = NULL;                  // terminate

};                                     // end process line

if (aRc == 0)                          // if OK

    aRc = ProcessWfMacros              // process workflow macros
                  (theContent,
                   theVars,
                   theIncludes,    
                   theWfMacros);

if (aRc == 0)                          // if still OK

    ProcessIfEmpty(theContent,         // process IF_EMPTY (should come last)
                   theVars);             
                       


TRACE("Process Statements Exit: %d\n", aRc);
    
return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessIfEmpty
//                                      
// Description  : Process #IF_EMPTY content
//                Supports nesting
//
// Parameters   : 1) Content
//                2) Variable list
//                3) Include list
//                4) Workflow macros
//
//****************************************************************************
int WorkflowStep::ProcessIfEmpty(CviStr &theContent,
                                 VariableList *theVars)

{                                      // begin ProcessIfEmpty

int aRc = 0;                           // OK

CviStr aMacro;                         // macro 


aMacro.Print("#IF_EMPTY");             // setup search name

const char *aData =                    // find first
    strstr(theContent, aMacro);        // statement

while (aRc == 0 &&                     // while OK and
       aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, "\n"); // grab newline

    const char *aMacEnd = aEnd;        // macro end

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        int aDepth = 1;                // depth of IfEmpty

        const char *aCheckSt = aEnd+1; // start of loop data
        const char *aCheckEnd = aCheckSt;

        const char *anEmptySt = NULL;  // no empty data
        const char *anEmptyEnd = NULL;
        const char *aCheckDataEnd = NULL;


        while (aDepth > 0 &&           // while OK and
               *aCheckEnd != 0)        // still more data to process

        {                              // begin locate end of IfEmpty

            if (*aCheckEnd == '#' &&
                strstr(aCheckEnd, "#IF_EMPTY") == aCheckEnd)

            {                          // begin new loop

                aDepth ++;             // bump depth

            }                          // end new loop

            else                       // otherwise

            if (aDepth == 1 &&
                *aCheckEnd == '#' &&
                strstr(aCheckEnd, "#EMPTY_CONTENT") == aCheckEnd)

            {                          // begin content empty


                aCheckDataEnd = aCheckEnd; // end of loop data

                anEmptySt = aCheckEnd + strlen("#EMPTY_CONTENT"); // empty iteration area

                if (*anEmptySt == '\n')
                    anEmptySt ++;

            }                          // end content empty

            else

            if (*aCheckEnd == '#' &&
                strstr(aCheckEnd, "#END_IF_EMPTY") == aCheckEnd)

            {                          // begin end of loop

                if (anEmptySt == NULL)

                    aCheckDataEnd = aCheckEnd; // mark end of loop data

                else                   // otherwise

                    anEmptyEnd = aCheckEnd;

                aMacEnd = aCheckEnd + strlen("#END_IF_EMPTY"); // end of macro

                if (*aMacEnd == '\n')
                    aMacEnd ++;

                aDepth --;             // rollback depth

            }                          // end of loop

            if (aDepth != 0)           // if more to go

               aCheckEnd ++;           // next!

        };                             // end locate end of IfEmpty

        if (aDepth > 0)                // if no matching close

        {                              // begin error

            GetTaskCviPgm()->Print("ERROR: No closing #END_IF_EMPTY found for #IF_EMPTY in template member %s, name %s\n",
                                   (const char *) itsMember,
                                   (const char *) itsName);

            aRc = 8;                   // fail

            break;                     // break look

        };                             // end error

        CviStr aCheckData;             // loop data
        CviStr anEmptyData;            // empty data area

        if (*aCheckSt == '\n')         // if newline
            aCheckSt ++;               // bump past it

        aCheckData.Add(aCheckSt, (int) (aCheckDataEnd - aCheckSt));

        TRACE("CHECKING #%s# to replace with #%s#\n",
                               (const char *) aCheckData,
                               (const char *) anEmptyData);

        if (anEmptySt != NULL &&       // if we have an empty area
            anEmptyEnd != NULL)

            anEmptyData.Add(anEmptySt, // grab empty code area
                           (int) (anEmptyEnd - anEmptySt));

        if (strlen(anEmptyData) != 0 &&  // was nothing found
            strlen(aCheckData) == 0)     // but we have an 'empty' area?

            aCheckData.Set(anEmptyData); // if so, use it

        TRACE("Data:\n%s\n", (const char *) aCheckData);

        theContent.Replace(aData, (int) (aMacEnd - aData), aCheckData); 

        aData = strstr((const char *) theContent + aFoundIdx, aMacro);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro


return(aRc);                           // return result

};                                     // end ProcessIfEmpty

//****************************************************************************
//
// Method       : GenerateAlloc
//
// Description  : Generate data for allocation 
//
// Parameters   : 1) JCL output
//                2) Dataset list
//                3) Tag
//
// Returns      : Count of items
//
//****************************************************************************
static int GenerateAlloc(CviStr &theJCL,
                         Dataset *theDatasets,
                         CviStr &theTag)

{

int aCount = 0;                        // counter

Dataset *aDataset = theDatasets;       // start with first dataset
                                       
while (aDataset != NULL)               // for all datasets
                                       
{                                      // begin check dataset
                                              
    if (aDataset->IsNeeded()        && // if it is needed
        aDataset->Allocate()        && 
        !strcmp(aDataset->GetTag(), theTag) && // and tag matches
        strlen(aDataset->GetJCL()) > 0)        // and we have JCL to write
                                               
    {                                  // begin add to JCL

        if (strlen(aDataset->GetCondition()) > 0)

        {

            theJCL.Add("#if ( ");      // open if

            CviStr aCond(aDataset->GetCondition()); // grab conditions

            theJCL.Add(aCond);         // add to #if

#ifdef _SRC_

            int aCondCnt = 0;              // conditional count
            const char *aLinePtr = aCond.Tok("\n"); // get each condition

            while (aLinePtr != NULL)       // process all lines
            {                
                CviStr aLine(aLinePtr);    // save line value

                const char *aNext =        // get next token
                    aCond.Tok("\n");

                if (strlen(aLine) > 0)     // OR them together
                {

                    if (aCondCnt > 0)
                        theJCL.Print(" && ");

                    if (aNext != NULL)     // if we have more, negate it as it was a not-passed condition
                        theJCL.Print(" !( %s )\n",
                                     (const char *) aLine);
                    else
                        theJCL.Print(" ( %s )\n",
                                     (const char *) aLine);
                    aCondCnt ++;
                }
                aLinePtr = aNext;          // next!
            }
#endif

            theJCL.Add(" ) \n");

            theJCL.Add("/*\n");
            theJCL.Add(aDataset->GetJCL());
            theJCL.Add("#end\n");

        }

        else

        {

            theJCL.Add("/*\n");            // add comment
            theJCL.Add(aDataset->GetJCL());// add JCL

        }

        aCount ++;                     // add to count
                                       
    }                                  // end add to JCL
                                       
    aDataset = aDataset->GetNext();    // next!
                                       
}                                      // end check dataset

return(aCount);                        // return count

}

//****************************************************************************
//
// Method       : GenerateDelete
//
// Description  : Generate data for deletion
//
// Parameters   : 1) JCL output
//                2) Dataset list
//                3) Tag
//
// Returns      : Count of items
//
//****************************************************************************
static int GenerateDelete(CviStr &theJCL,
                          Dataset *theDatasets,
                          CviStr &theTag)

{

int aCount = 0;                        // counter

Dataset *aDataset = theDatasets;       // start with first dataset
                                       
while (aDataset != NULL)               // for all datasets
                                       
{                                      // begin check dataset
                                              
    if (aDataset->IsNeeded()        && // if it is needed
        aDataset->Allocate()        && 
        !strcmp(aDataset->GetTag(), theTag) && // and tag matches
        strlen(aDataset->GetJCL()) > 0)        // and we have JCL to write
                                               
    {                                  // begin add to JCL
                                       
        if (aCount == 0)               // if this will be our first

        {                              // begin add delete setup

            theJCL.Print("//DELRTE  EXEC PGM=IEFBR14\n");

        };                             // end add delete setup

        theJCL.Add("/*\n");            // add comment

        theJCL.Print("//DEL%03d   DD DSN=%s,\n"
                     "//            DISP=(MOD,DELETE,DELETE),\n"
                     "//            SPACE=(TRK,(0,0))\n",
                     aCount,
                     aDataset->GetDSN());
                                       
        aCount ++;                     // add to count
                                       
    }                                  // end add to JCL
                                       
    aDataset = aDataset->GetNext();    // next!
                                       
}                                      // end check dataset

return(aCount);                        // return count

}

//****************************************************************************
//
// Method       : ProcessDatasetAlloc
//
// Description  : Generate dataset allocation lines
//
// Parameters   : 1) Dataset list
//                2) Property variables
//
// Returns      : Count of datasets allocated
//
//****************************************************************************
static int  ProcessDatasetAlloc(CviStr &theContent,
                                Dataset *theDatasets,
                                VariableList *thePropVars)

{

int aCount = 0;                        // counter


CviStr aMacro;                         // macro 


aMacro.Print("#GEN_ALLOC_JCL(");       // setup search name

const char *aData =                    // find first
    strstr(theContent, aMacro);        // statement

while (aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, ")");

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        CviStr aCall;                  // tag holder
        CviStr aParms;                 // parmeters
        CviStr aTag;                   // tag           

        const char *aName = strstr(aData, "(");

        aName ++;                      // bump to get name

        aParms.Print("%*.*s",          // get all parameters
                     (int) (aEnd-aName),
                     (int) (aEnd-aName),
                     aName);

        if (*(aEnd+1) == '\n')         // if ends in newline

            aEnd ++;                   // use it during replace as well

        aCall.Print("%*.*s",           // setup just the tag
                    (int) (aEnd-aData)+1,
                    (int) (aEnd-aData)+1,
                    aData);
                                       
        const char *aParm = aParms.Tok(",");

        if (aParm != NULL)             // if found

            aTag = aParm;              // grab tag

        CviStr aJCL;                   // data

        aCount +=                      // generate
            GenerateAlloc(aJCL,        // JCL for allocation
                          theDatasets,
                          aTag);

        theContent.Replace(aData, strlen(aCall), aJCL);

        aData = strstr((const char *) theContent + aFoundIdx + strlen(aJCL), aMacro);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro

return(aCount);                        // return the count


}

//****************************************************************************
//
// Method       : ProcessDatasetDelete
//
// Description  : Generate dataset delete lines
//
// Parameters   : 1) Dataset list
//                2) Property variables
//
// Returns      : Count of datasets deleted
//
//****************************************************************************
static int  ProcessDatasetDelete(CviStr &theContent,
                                 Dataset *theDatasets,
                                 VariableList *thePropVars)

{

int aCount = 0;                        // counter


CviStr aMacro;                         // macro 


aMacro.Print("#GEN_DELETE_JCL(");      // setup search name

const char *aData =                    // find first
    strstr(theContent, aMacro);        // statement

while (aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, ")");

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        CviStr aCall;                  // tag holder
        CviStr aParms;                 // parmeters
        CviStr aTag;                   // tag           

        const char *aName = strstr(aData, "(");

        aName ++;                      // bump to get name

        aParms.Print("%*.*s",          // get all parameters
                     (int) (aEnd-aName),
                     (int) (aEnd-aName),
                     aName);

        if (*(aEnd+1) == '\n')         // if ends in newline

            aEnd ++;                   // use it during replace as well

        aCall.Print("%*.*s",           // setup just the tag
                    (int) (aEnd-aData)+1,
                    (int) (aEnd-aData)+1,
                    aData);
                                       
        const char *aParm = aParms.Tok(",");

        if (aParm != NULL)             // if found

            aTag = aParm;              // grab tag

        CviStr aJCL;                   // data

        aCount +=                      // generate
            GenerateDelete(aJCL,       // JCL for allocation
                           theDatasets,
                           aTag);

        theContent.Replace(aData, strlen(aCall), aJCL);

        aData = strstr((const char *) theContent + aFoundIdx + strlen(aJCL), aMacro);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro

return(aCount);                        // return the count


}

//****************************************************************************
//
// Method       : GenerateCompress
//
// Description  : Generate JCL for compression
//
// Parameters   : 1) JCL output
//                2) Dataset list
//                3) Tag
//                4) Property values
//
// Returns      : Count of items
//
//****************************************************************************
static int GenerateCompress(CviStr &theJCL,
                            Dataset *theDatasets,
                            CviStr &theTag,
                            VariableList *thePropVars)

{

int aCount = 0;                        // counter

CviStr  aDD;                           // DD entries
CviStr  aSYSIN;                        // SYSIN entries


TRACE("Processing dataset compress\n");

Dataset *aDataset = theDatasets;       // start with first dataset

while (aDataset != NULL)               // for all datasets

{                                      // begin check dataset

    Populate *aPop = aDataset->GetPopulate();

    TRACE("Dataset %s, Populate %p, Needed %d, Allocate %d\n",
                           aDataset->GetID(),
                           aPop,
                           aDataset->IsNeeded(),
                           aDataset->Allocate());

//
// We were going to check to see if we created the DSN. If not, we would bypass
// the compress for it. But we decided to compress anyway.
//
//        strlen(aDataset->GetJCL()) > 0 && // and it has JCL (we are creating the DSN)

    if (aDataset->IsNeeded()           && // if it is needed
        aDataset->Allocate()           &&
        !strcmp(aDataset->GetTag(), theTag) && // and tag matches
        aPop      != NULL)                // and population is needed

    {                                  // begin add to JCL

        aCount ++;                     // bump the counter

        CviStr aIn;                    // input DD
        CviStr aOut;                   // output DD

        CviStr aIf;                    // IF for conditional

        bool    aUseCond = FALSE;      // not conditional yet

        if (strlen(aDataset->GetCondition()) > 0)

        {

            aUseCond = true;           // we are conditional

            aIf.Add("#if ( ");         // open if

            CviStr aCond(aDataset->GetCondition()); // grab conditions

            aIf.Add(aCond);            // add to #if

#ifdef _SRC_

            int aCondCnt = 0;              // conditional count
            const char *aLinePtr = aCond.Tok("\n"); // get each condition

            while (aLinePtr != NULL)       // process all lines
            {                
                CviStr aLine(aLinePtr);    // save line value

                const char *aNext =        // get next token
                    aCond.Tok("\n");

                if (strlen(aLine) > 0)     // OR them together
                {

                    if (aCondCnt > 0)
                        aIf.Print(" && ");

                    if (aNext != NULL)     // if we have more, negate it as it was a not-passed condition
                        aIf.Print(" !( %s )\n",
                                     (const char *) aLine);
                    else
                        aIf.Print(" ( %s )\n",
                                     (const char *) aLine);
                    aCondCnt ++;
                }
                aLinePtr = aNext;          // next!
            }
#endif

            aIf.Add(" ) \n");

        }

        aIn.Print("IN%d", aCount);     // set input

        aOut.Print("OUT%d", aCount);   // set output


        if (aUseCond)                  // if conditional

            aDD.Add(aIf);              // add IF

        aDD.Print("//%-8.8s DD DSN=%s,\n",
                  (const char *) aIn,
                  (const char *) aDataset->GetDSN());

        aDD.Print("//          DISP=SHR\n");


        aDD.Print("//%-8.8s DD DSN=%s,\n",
                  (const char *) aOut,
                  (const char *) aDataset->GetDSN());

        aDD.Print("//          DISP=SHR\n");

        aDD.Print("//*\n");

        if (aUseCond)                  // if conditional

            aDD.Add("#end\n");         // close if

        if (aUseCond)                  // if conditional

            aSYSIN.Add(aIf);           // add IF

        aSYSIN.Print("  COPY I=((%s,R)),O=%s,LIST=NO\n",
                     (const char *) aIn,
                     (const char *) aOut);

        if (aUseCond)                  // if conditional

            aSYSIN.Add("#end\n");      // add IF

    }                                  // end add to JCL

    aDataset = aDataset->GetNext();    // next!

}                                      // end check dataset

if (strlen(aDD) != 0)                  // if we had things to do 

{                                      // begin build compress step

    theJCL.Print("//*********************************************************************\n");
    theJCL.Print("//* Compress datasets\n");
    theJCL.Print("//*********************************************************************\n");
    theJCL.Print("//*\n");
    theJCL.Print("//COMPRESS EXEC PGM=IEBCOPY,REGION=4096K\n");
    theJCL.Print("//SYSPRINT DD SYSOUT=*\n");
    theJCL.Print("//SYSUT3    DD UNIT=SYSALLDA,SPACE=(CYL,(50,10))\n");  
    theJCL.Print("//SYSUT4    DD UNIT=SYSALLDA,SPACE=(CYL,(50,10))\n");
    theJCL.Print("//FCOPYOFF  DD DUMMY\n");
    theJCL.Print("//*\n");
    theJCL.Add(aDD);
    theJCL.Print("//SYSIN     DD *\n");
    theJCL.Add(aSYSIN);
    theJCL.Print("//*\n");

};                                     // end build compress step


return(aCount);                        // return the count


}
//****************************************************************************
//
// Method       : ProcessDatasetCompress
//
// Description  : Generate dataset compress lines
//
// Parameters   : 1) Job content
//                2) Dataset list
//                3) Property values
//
// Returns      : Count of datasets allocated
//
//****************************************************************************
static int  ProcessDatasetCompress(CviStr &theContent,
                                   Dataset *theDatasets,
                                   VariableList *thePropVars)

{

int aCount = 0;                        // counter


CviStr aMacro;                         // macro 


aMacro.Print("#GEN_COMPRESS_STEP(");   // setup search name

const char *aData =                    // find first 
    strstr(theContent, aMacro);        // statement

while (aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, ")");

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        CviStr aCall;                  // tag holder
        CviStr aParms;                 // parmeters
        CviStr aTag;                   // tag           

        const char *aName = strstr(aData, "(");

        aName ++;                      // bump to get name

        aParms.Print("%*.*s",          // get all parameters
                     (int) (aEnd-aName),
                     (int) (aEnd-aName),
                     aName);

        if (*(aEnd+1) == '\n')         // if ends in newline

            aEnd ++;                   // use it during replace as well

        aCall.Print("%*.*s",           // setup just the tag
                    (int) (aEnd-aData)+1,
                    (int) (aEnd-aData)+1,
                    aData);
                                       
        const char *aParm = aParms.Tok(",");

        if (aParm != NULL)             // if found

            aTag = aParm;              // grab tag

        CviStr aJCL;                   // data

        aCount +=                      // generate
            GenerateCompress(aJCL,     // JCL for allocation
                             theDatasets,
                             aTag,
                             thePropVars);

        TRACE("Replacing %*.*s with %s\n",
                               strlen(aCall),
                               strlen(aCall),
                               aData,
                               (const char *) aJCL);

        theContent.Replace(aData, strlen(aCall), aJCL);

        TRACE("Replaced\n");

        aData = strstr((const char *) theContent + aFoundIdx + strlen(aJCL), aMacro);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro

return(aCount);                        // return the count

}

//****************************************************************************
//
// Method       : GenerateDatasetCopy
//
// Description  : Generate dataset copy lines
//
// Parameters   : 1) Job content
//                2) Dataset list
//                3) Property values
//
// Returns      : Count of datasets
//
//****************************************************************************
static int GenerateDatasetCopy(CviStr &theJCL,
                               Dataset *theDatasets,
                               CviStr &theTag,
                               VariableList *thePropVars)

{

int aCount = 0;                        // counter

CviStr  aDD;                           // DD entries
CviStr  aSYSIN;                        // SYSIN entries
CviStr  aCopyJCL;                      // copy steps


Dataset *aDataset = theDatasets;       // start with first dataset

while (aDataset != NULL)               // for all datasets

{                                      // begin check dataset

    Populate *aPop = aDataset->GetPopulate();

    if (aDataset->IsNeeded()        && // if it is needed
        aDataset->Allocate()        &&
        !strcmp(aDataset->GetTag(), theTag) && // and tag matches
        aPop      != NULL)             // and population is needed

    {                                  // begin add to JCL

        aCount ++;                     // bump counter

        bool    aUseCond = FALSE;      // not conditional yet

        if (strlen(aDataset->GetCondition()) > 0)

        {

            aUseCond = true;           // we are conditional

            theJCL.Add("#if ( ");      // open if

            CviStr aCond(aDataset->GetCondition()); // grab conditions

            theJCL.Add(aCond);         // add to #if

#ifdef _SRC_

            int aCondCnt = 0;              // conditional count

            const char *aLinePtr = aCond.Tok("\n"); // get each condition

            while (aLinePtr != NULL)       // process all lines
            {                
                CviStr aLine(aLinePtr);    // save line value

                const char *aNext =        // get next token
                    aCond.Tok("\n");

                if (strlen(aLine) > 0)     // OR them together
                {

                    if (aCondCnt > 0)
                        theJCL.Print(" && ");

                    if (aNext != NULL)     // if we have more, negate it as it was a not-passed condition
                        theJCL.Print(" !( %s )\n",
                                     (const char *) aLine);
                    else
                        theJCL.Print(" ( %s )\n",
                                     (const char *) aLine);
                    aCondCnt ++;
                }
                aLinePtr = aNext;          // next!
            }
#endif

            theJCL.Add(" ) \n");

        }

        theJCL.Print("//*********************************************************************\n");
        theJCL.Print("//* POPULATE %s\n", (const char *) aDataset->GetDSN());
        theJCL.Print("//*********************************************************************\n");
        theJCL.Print("//*\n");
        theJCL.Print("//CPY%d EXEC PGM=IEBCOPY,REGION=4096K\n", aCount);
        theJCL.Print("//SYSPRINT DD SYSOUT=*\n");
        theJCL.Print("//SYSUT3    DD UNIT=SYSALLDA,SPACE=(CYL,(5,1))\n");  
        theJCL.Print("//SYSUT4    DD UNIT=SYSALLDA,SPACE=(CYL,(5,1))\n");
        theJCL.Print("//FCOPYOFF  DD DUMMY\n");
        theJCL.Print("//*\n");
        theJCL.Print("//OUT       DD DSN=%s,\n", (const char *) aDataset->GetDSN());
        theJCL.Print("//             DISP=SHR\n");
        theJCL.Print("//*\n");


        aSYSIN.Reset();                // clear SYSIN

        int aPopCount = 0;             // population counter

        while (aPop != NULL)           // for all population entries

        {                              // begin generate copy JCL

            if (strlen(aPop->GetSourceDSN()) > 0)

            {                          // begin use it

                aPopCount ++;          // bump counter

                CviStr aIn;
                                       // input DD
                aIn.Print("IN%d", aPopCount);

                theJCL.Print("//%-8.8s DD DSN=%s,\n",
                               (const char *) aIn,
                               (const char *) aPop->GetSourceDSN());

                theJCL.Print("//          DISP=SHR\n");

                theJCL.Print("//*\n");

                aSYSIN.Print("  COPY I=((%s,R)),O=OUT\n",
                             (const char *) aIn);

            };                         // end use it

            aPop = aPop->GetNext();    // next!

        };                             // end generate copy JCL

        theJCL.Print("//SYSIN    DD *\n");

        theJCL.Add(aSYSIN);

        theJCL.Print("//*\n");

        if (aUseCond)                  // if conditional

            theJCL.Add("#end\n");      // close if

    }                                  // end add to JCL

    aDataset = aDataset->GetNext();    // next!

}                                      // end check dataset

return(aCount);                        // return the count


}

//****************************************************************************
//
// Method       : ProcessDatasetCopy
//
// Description  : Generate dataset copy lines
//
// Parameters   : 1) Job content
//                2) Dataset list
//                3) Property values
//
// Returns      : Count of datasets
//
//****************************************************************************
static int  ProcessDatasetCopy(CviStr &theContent,
                               Dataset *theDatasets,
                               VariableList *thePropVars)

{

int aCount = 0;                        // counter


CviStr aMacro;                         // macro 


aMacro.Print("#GEN_COPY_STEPS(");      // setup search name

const char *aData =                    // find first
    strstr(theContent, aMacro);        // statement

while (aData != NULL)                  // for all data

{                                      // begin process macro

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aEnd = strstr(aData, ")");

    if (aEnd != NULL)                  // if end of tag found

    {                                  // begin process tag

        CviStr aCall;                  // tag holder
        CviStr aParms;                 // parmeters
        CviStr aTag;                   // tag           

        const char *aName = strstr(aData, "(");

        aName ++;                      // bump to get name

        aParms.Print("%*.*s",          // get all parameters
                     (int) (aEnd-aName),
                     (int) (aEnd-aName),
                     aName);

        if (*(aEnd+1) == '\n')         // if ends in newline

            aEnd ++;                   // use it during replace as well

        aCall.Print("%*.*s",           // setup just the tag
                    (int) (aEnd-aData)+1,
                    (int) (aEnd-aData)+1,
                    aData);
                                       
        const char *aParm = aParms.Tok(",");

        if (aParm != NULL)             // if found

            aTag = aParm;              // grab tag

        CviStr aJCL;                   // data

        aCount +=                      // generate
            GenerateDatasetCopy(aJCL,  // JCL for copy
                                theDatasets,
                                aTag,
                                thePropVars);

        TRACE("Replacing %*.*s with %s\n",
                               strlen(aCall),
                               strlen(aCall),
                               aData,
                               (const char *) aJCL);

        theContent.Replace(aData, strlen(aCall), aJCL);

        TRACE("Replaced\n");

        aData = strstr((const char *) theContent + aFoundIdx + strlen(aJCL), aMacro);

//        TRACE("Next: %p\n", aData);

    }                                  // end process tag

    else

        aData = strstr((const char *) theContent + aFoundIdx + 1, aMacro);

};                                     // end process macro


return(aCount);                        // return the count

}

//****************************************************************************
//
// Method       : ProcessInternMacros
//
// Description  : Process internal macros
//
// Parameters   : 1) Property variables
//
//****************************************************************************
static void ProcessInternMacros(CviStr &theData,
                                WorkflowStep *theStep,
                                VariableList *thePropVars,
                                char theEscape)

{
                                       
WorkflowIntMacro::itsTop->Process(theData, theStep, thePropVars, theEscape);

//if (theStep != NULL)                   // if step passed
//
//    theStep->ProcessIfEmpty            // process IF_EMPTY (should come last
//                (theData,              // to make sure all conditions have procssed)
//                 thePropVars);             
                       

}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessIntMacros
//
// Description  : Process internal macros
//
// Parameters   : 1) Property variables
//
//****************************************************************************
void WorkflowStep::ProcessIntMacros(VariableList *thePropVars)

{

ProcessInternMacros(itsInstructions, this, thePropVars, true);   // process instructions

ProcessInternMacros(itsContent, this, thePropVars, true);        // process content

if (strstr(itsSaveAs, "#") != NULL &&                            // might have macros?
    strcmp(itsType, "GROUP"))                                    // and not a group member

   ProcessInternMacros(itsSaveAs, this, thePropVars, true);      // process save-as

ProcessInternMacros(itsTitle, this, thePropVars, true);   // process instructions

ProcessInternMacros(itsDescription, this, thePropVars, true);   // process instructions


}

//****************************************************************************
//
// Method       : WorkflowStep::AddMacros
//
// Description  : Add needed Velocity macros
//
// Parameters   : 1) MACRO step list
//
//****************************************************************************
int WorkflowStep::AddMacros(WorkflowStep *theMacros)

{                                      // begin AddMacros

int aRc = 0;                           // all is well
char aAdded = 0;                       // added flag

CviStr aAddedCont;                     // list of macros added for content
CviStr aAddedInst;                     // list of macros added for instructions
CviStr aAddedSave;                     // list of macros added for save-as


WorkflowStep *aMacro = theMacros;      // start at head of list

while (aMacro != NULL)                 // for all macros

{                                      // begin find/add

    CviStr aMacName;                   // macro name holder for list


    aMacName.Print(",%s,",             // set name
                   (const char *) aMacro->GetName());


    CviStr aRef;                       // reference
                                       
    aRef.Print("#%s",                  
               (const char *) aMacro->GetName());

    if (strstr(aAddedCont,             // if not process 
               aMacName) == NULL &&    // yet and
        strstr(itsContent,             // if content has
               aRef) != NULL)          // reference to this macro
                                       
    {                                  // begin add macro
                                       
        aAddedCont.Add(aMacName);      // mark as added

        aAdded = 1;                    // added
                                       
        CviStr aContent(itsContent);   // grab original content
                                       
        itsContent.Reset();            // reset content
                                       
        const char *aStr = aContent.Tok("\n");
                                       
        while (aStr != NULL)           // for all lines
                                       
        {                              // begin look for macro call
                                       
            if (strstr(aStr, aRef) != NULL &&
                *aStr != '#' &&    
                aStr[strlen(aStr)-1] == ')')
                                   
                itsContent.Print("%s$NL\n",
                                 aStr);
                                       
            else                       
                                       
                itsContent.Print("%s\n", aStr);
                                       
            aStr = aContent.Tok("\n"); // next!
                                       
        };                             // end look for macro call
                                       
        TRACE("Adding content for %s: %s...\n",
               (const char *) aMacName,
               (const char *) aMacro->GetContent());
                                       
        CviStr aMacContent(aMacro->GetContent()); // grab macro content
                                       
        aMacContent.XMLEscape();       // escape special characters

        itsContent.Add(aMacContent);   // add content
                                       
    };                                 // end add macro

    if (strstr(aAddedInst,             // if not process 
               aMacName) == NULL &&    // yet and
        strstr(itsInstructions,        // if content has
               aRef) != NULL)          // reference to this macro

    {                                  // begin add macro

        aAddedInst.Add(aMacName);      // mark as added

        aAdded = 1;                    // added

        CviStr aMacContent(aMacro->GetContent()); // grab macro content

        aMacContent.XMLEscape();       // escape special characters

        itsInstructions.Add(aMacContent); // add content

    };                                 // end add macro

    if (strstr(aAddedSave,             // if not process 
               aMacName) == NULL &&    // yet and
        strstr(itsSaveAs,              // if "Save As" has
               aRef) != NULL)          // reference to this macro

    {                                  // begin add macro

        aAddedSave.Add(aMacName);      // mark as added

        aAdded = 1;                    // added

        CviStr aMacContent(aMacro->GetContent()); // grab macro content

        aMacContent.XMLEscape();       // escape special characters

        itsSaveAs.Add(aMacContent);    // add content

    };                                 // end add macro

    aMacro = aMacro->GetNext();        // next!

    if (aMacro == NULL && aAdded)      // if end of list, but progress

    {                                  // begin iterate again

        aMacro = theMacros;            // start over

        aAdded = 0;                    // nothing has been added yet

    };                                 // end iterate again


};                                     // end find/add


return(aRc);                           // return result

};                                     // end AddMacros

//****************************************************************************
//
// Method       : WorkflowStep::ProcessWfMacro
//
// Description  : Process workflow macro
//
// Parameters   : 1) MACRO step list
//
//****************************************************************************
int WorkflowStep::ProcessWfMacro(CviStr &theContent,
                                 WorkflowStep *theMacro,
                                 VariableList *theVars,
                                 WorkflowStep *theIncludes,
                                 WorkflowStep *theWfMacros)

{                                      // begin ProcessWfMacro

int aRc = 0;                           // all is well

CviStr aCall;                          // call reference
                                       
aCall.Print("#%s(",                      
            (const char *) theMacro->GetName());

const char *aData =                    // find first
    strstr(theContent, aCall);         // statement
                                       
TRACE("Looking for %s - %p\n", (const char *) aCall, aData);
                                       
while (aData != NULL)                  // if found
                                       
{                                      // begin process macro
                                       
    static int aCounter = 0;
    static int aCacheCount = 0;
    aCounter ++;
    CviStr aPassedParms;               // parms

    long aFoundIdx =                   // convert to index
        (long) (aData - (const char *) theContent);

    const char *aStParms =             // get
        aData + strlen(aCall);         // to start of parameters

    const char *aEnd = 
        LocateEndParm(aStParms,
                      aCall,
                      aData,
                      this);

    if (aEnd == NULL)                  // if not closed off

    {                                  // begin error

        aRc = 8;                       // fail

        break;                         // break

    }                                  // end error

    aPassedParms.Print("%*.*s",  // get all parameters
                       (int) (aEnd-aStParms),
                       (int) (aEnd-aStParms),
                       aStParms);
    
    if (strstr(aPassedParms, "#") != NULL)
        ProcessInternMacros(aPassedParms,
                            this,
                            theVars,
                            true);

    if (aEnd != NULL)                  // if end found
                                       
    {                                  // begin work on macro

        char aEndNl = true;            // assume statement ends with newline
        CviStr aTag;                   // tag holder
        CviStr aNewContent;

        if (*(aEnd+1) == '\n')         // if it ends in a newline

            aEnd ++;                   // replace newline as well

        else                           // otherwise

            aEndNl = false;            // we don't end with a newline

        aTag.Print("%*.*s",            // setup just the tag
                   (int) (aEnd-aData)+1,
                   (int) (aEnd-aData)+1,
                   aData);             

        if (aMacroCache.GetAnswer(theMacro,
                                  aPassedParms,
                                  aNewContent) == 0)

        {

            TRACE("Cached result used for %s parms (%s)\n",
                                   (const char *) theMacro->GetName(),
                                   (const char *) aPassedParms);

            aCacheCount ++;
        }
        else
        {

            CviStr aSavedParms(aPassedParms);

            VariableList aVars;            // variable list

            MarkTemplate
                (theMacro->itsMember);     // mark inclusion of this template

            theMacro->BumpReferences(&aVars);   // bump reference counter
            theMacro->SetReferenceVar(theVars); // use for conditional statements as well

            CviStr aContent(theMacro->GetContent()); // get macro content

            TRACE("Tag: %s\n", (const char *) aTag);

            const char *aStr = aContent.Tok("\n");

            while (aStr != NULL)           // for all lines

            {                              // begin process line

                CviStr aLineStr(aStr);     // line string

                if (strstr(aStr, "#PARMS(") != NULL)

                {                          // begin get parms

                    aStParms =             // get start of parms
                        strstr(aLineStr, "(") + 1;

                    const char *aEndParms = strstr(aStParms, ")");

                    if (aEndParms != NULL)

                    {                  // begin process parms

                        CviStr aMacParms;   // macro parms

                        aMacParms.Print("%*.*s",
                                       (int) (aEndParms - aStParms),
                                       (int) (aEndParms - aStParms),
                                       aStParms);

                        const char *aMacParm =  
                            aMacParms.Parm(",");

                        while (aMacParm != NULL) // for all macros parms

                        {                  // begin matched to passed parm

                            while (*aMacParm == ' ')
                                aMacParm ++;

                            CviStr aMParm(aMacParm);

                            aMParm.Trim(' ');       // trim white space off end

                            const char *aPassParm =     // get passed parm
                                aPassedParms.Parm(",");

                            if (aPassParm != NULL)      // if we have a passed-in parm

                            {                           // begin create variables

                                while (*aPassParm == ' ')
                                    aPassParm ++;

                                CviStr aPParm(aPassParm);

                                aPParm.Trim(' ');       // trim white space off end

                                TRACE("PARMS: %s %s\n", 
                                                       (const char *) aMParm, (const char *) aPParm);

                                aVars.Add(aMParm, aPParm, 1);

                            }              // end create variables

                            else           // otherwise

                            {              // begin use empty string

                                TRACE("PARMS: %s (empty)\n", 
                                                       (const char *) aMParm);

                                aPassedParms.Reset();   // no more passed-in parms

                                aVars.Add(aMParm, "", 1);

                            };             // and use empty string

                            aMacParm =     // get next macro parm
                                aMacParms.Parm(",");

                        }                  

                    };                     // end process parms

                }                          // end get parms

                else                       // otherwise

                {                          // begin copy line

                    aNewContent.Add(aLineStr);
                    aNewContent.Add("\n");

                };                         // end copy line

                aStr = aContent.Tok("\n"); // next!

            };                             // end process line

            TRACE("Preliminary content: %s\n", (const char *) aNewContent);

            aVars.ReplaceWithValues(aNewContent, false); // replace so any conditionals will hold up

            if (aRc == 0)                  // if OK so far

                aRc = ProcessStatements(aNewContent,
                                        theVars,
                                        theIncludes,
                                        theWfMacros);

            aVars.ReplaceWithValues(aNewContent, false); // could have references from includes

            TRACE("ADDING Cached result for %s: %s\n",
                                   (const char *) aSavedParms,
                                   (const char *) aNewContent);

            aMacroCache.AddAnswer(theMacro, aSavedParms, aNewContent);

        }

        TRACE("Macro count %d, cached %d\n",
                               aCounter, aCacheCount);

        TRACE("Final content: %s\n", (const char *) aNewContent);

        if (!aEndNl                 && // if we don't end with a newline
            strlen(aNewContent) > 1)   // and we have content

        {                              // begin strip trailing newline

            const char *aLastChar =
                (const char *) aNewContent + strlen(aNewContent) - 1;

            if (*aLastChar == '\n')    // if it ends with a newline

                 aNewContent.Replace(aLastChar, 1, ""); // remove it

        };                             // end strip trailing newline

        theContent.Replace(aData, strlen(aTag), aNewContent);

        aData = strstr((const char *) theContent + aFoundIdx, aCall);


    }                                  // end work on macro

    else                               // otherwise

        aData = strstr((const char *) theContent + aFoundIdx + 1, aCall);
                                       
};                                     // end process macro


return(aRc);                           // return result

};                                     // end ProcessWfMacro

//****************************************************************************
//
// Method       : WorkflowStep::GenStdProperties
//
// Description  : Generate standard properties
//
// Parameters   : None
//
//****************************************************************************
void WorkflowStep::GenStdProperties(VariableList *thePropVars)
{

if (thePropVars != NULL)

{

    thePropVars->Add("JOBNAME", itsJobName, 2);

    thePropVars->Add("$JOBNAME", itsJobName, 2);

    thePropVars->Add("NAME", itsName, 2);

    thePropVars->Add("$NAME", itsName, 2);

    thePropVars->Add("GROUP", itsGroup, 2);
                                   
    thePropVars->Add("$GROUP", itsGroup, 2);

}

}

//****************************************************************************
//
// Method       : WorkflowStep::AddUNIXDSNPrompt
//
// Description  : 1) Variable to prompt
//
//****************************************************************************
const char *WorkflowStep::AddUNIXDSNPrompt(VariableList *thePropVars,
                                           Variable *theDSNVar)
{

itsAutoEnable = false;                 // no longer automatic-eligible

if (itsPromptList == NULL)             // if not yet allocated

    itsPromptList = new VariableList;  // allocate

CviStr aVarName;                       // variable name

aVarName.Print("PROMPT%p",
               (void *) theDSNVar);


itsPromptList->Add(aVarName, "", 1);   // add, if not already there


Variable *aVar = thePropVars->GetVariable(aVarName);

if (aVar == NULL &&                    // if not found
    thePropVars->Add(aVarName, "") == 0)   // and we added it

{                                      // begin add prompt info

    aVar = thePropVars->GetVariable(aVarName);

    GetTaskCviPgm()->Print("Variable for %s - %p\n", (const char *) aVarName, (void *) aVar);

    CviStr aValue;                     // velocity name

    aValue.Print("${instance-%s}",   // add Velocity name
                 (const char *) aVarName);

    aVar->AddSubVariable(new Variable("VELOCITY", aValue));

    aValue.Reset();                    // reset


    aValue.Print("SMP/E Target DSN for the %s UNIX File System",
                 (const char *) theDSNVar->FindSubVar("BMC-ORIG-LLQ")->Value());

    aVar->AddSubVariable(new Variable("LABEL", aValue));

    aValue.Reset();                    // reset


    aValue.Print("Enter the SMP/E Target DSN for the %s UNIX File System that was mounted at \"%s\".",
                 (const char *) theDSNVar->FindSubVar("BMC-ORIG-LLQ")->Value(),
                 (const char *) theDSNVar->FindSubVar("MOUNT")->Value());

    aVar->AddSubVariable(new Variable("DESCRIPTION", aValue));

    aValue.Reset();                    // reset


    aValue.Print("Enter a valid DSNAME.",
                 (const char *) theDSNVar->FindSubVar("BMC-ORIG-LLQ")->Value());

    aVar->AddSubVariable(new Variable("ABSTRACT", aValue));

    aValue.Reset();                    // reset


    aVar->AddSubVariable(new Variable("VALIDATION", "DSNAME"));
    aVar->AddSubVariable(new Variable("CATEGORY", "Datasets"));
//    aVar->AddSubVariable(new Variable("MINLEN", "1"));
//    aVar->AddSubVariable(new Variable("MAXLEN", "44"));
    aVar->AddSubVariable(new Variable("DEFAULT", theDSNVar->FindSubVar("DSN")->Value()));

};                                     // end add prompt info


return((const char *) aVar->FindSubVar("VELOCITY")->Value()); // return variable name to use

}

//****************************************************************************
//
// Method       : WorkflowStep::ProcessWfMacros
//
// Description  : Process workflow macros
//
// Parameters   : 1) MACRO step list
//
//****************************************************************************
int WorkflowStep::ProcessWfMacros(CviStr &theContent,
                                  VariableList *theVars,
                                  WorkflowStep *theIncludes,
                                  WorkflowStep *theWfMacros)

{                                      // begin AddWfMacros

int aRc = 0;                           // all is well


WorkflowStep *aMacro = theWfMacros;    // start at head of list

while (aMacro != NULL)                 // for all macros

{                                      // begin find/add

    ProcessWfMacro(theContent,         // process macro
                   aMacro,
                   theVars,
                   theIncludes,
                   theWfMacros);

    aMacro = aMacro->GetNext();        // next!

};                                     // end find/add


return(aRc);                           // return result

};                                     // end AddWfMacros

//****************************************************************************
//
// Method       : WorkflowStep::GenGroup
//
// Description  : Generate steps for group
//
// Parameters   : 1) Output string       
//                2) Workflow variables
//                3) Property variables
//                4) Velocity macros
//                5) Translation templates
//
// Returns      : Count of items processed
//
//****************************************************************************
int  WorkflowStep::GenGroup(CviStr &theXml,
                            VariableList *theWfVars,
                            VariableList *thePropVars,
                            WorkflowStep *theMacros,
                            WorkflowStep *theTranslates)

{                                      // begin GenGroup

int aStepCnt = 0;                      // step counter

TRACE("GenGroup called for %s\n",
                     (const char *) itsName);


WorkflowStep *aStep = itsGrpOrderChild;// get child

while (aStep != NULL)                  // for all steps

{                                      // begin process child

    if (itsGrp != NULL)                // do we have a parent?

    {                                  // begin set combine values

        if (itsCombine == 0)           // if at default

            itsCombine = itsGrp->itsCombine;

        if (itsCombine == 0)           // if set for default

            itsCombine = 'Y';          // default to combine

        if (strlen(itsJobName) == 0)

            itsJobName.Set(itsGrp->itsJobName);

        if (strlen(itsJobName) == 0)
        
           itsJobName.Set("CREATMEM"); 

        TRACE("SAVE AS for %s: %s and parent %s %s\n",
              (const char *) itsName,
              (const char *) itsSaveAs,
              (const char *) itsGrp->itsName,
              (const char *) itsGrp->itsSaveAs);

        if (strlen(itsSaveAs) == 0)

            itsSaveAs.Set(itsGrp->itsSaveAs);

    };                                 // end set combine values

    if (!aStep->itsBypass           && // if not bypassing
        aStep->itsTarget == TARGET_CONFIGURATION)

        aStepCnt += aStep->GenXML(theXml, // generate XML for it
                                  theWfVars,
                                  thePropVars,
                                  theMacros,
                                  theTranslates);

    else                               // otherwise 

       TRACE("Bypassing step %s\n",
                              (const char *) aStep->itsName);

    aStep = aStep->itsOrderChild;      // get next child

};                                     // end process child


return(aStepCnt);                      // return step counter

};                                     // end GenGroup

//****************************************************************************
//
// Method       : WorkflowStep::FindSoloChild
//
// Description  : Finds first solo child
//
//****************************************************************************
WorkflowStep * WorkflowStep::FindSoloChild()

{                                      // begin FindSoloChild   

TRACE("Finding solo child for %s\n",
      (const char *) itsName);

WorkflowStep *aChild = itsGrpOrderChild;

while (aChild != NULL)                 // while we have a child

{                                      // begin locate unbypassed child

    TRACE("   Checking solo child for %s\n",
          (const char *) aChild->itsName);

    if (!aChild->itsBypass &&          // if not bypassed
        strcmp(aChild->itsType,        // and not
               "GROUP"))               // a group

    {                                  // begin found unbypassed child

        return(aChild);                // return the child

    };                                 // end found unbypassed child

    if (!strcmp(aChild->itsType,       // group?
                "GROUP"))

    {                                  // begin check children of group

        WorkflowStep *aRc = 
            aChild->FindSoloChild();   // check group

        if (aRc != NULL)               // if found

            return(aRc);               // use it

    };                                 // end check children of group

    aChild = aChild->itsOrderChild;    // grab next child

};                                     // end locate unbypass child

return(NULL);                          // not found

}                                      // end FindSoloChild

//****************************************************************************
//
// Method       : WorkflowStep::GenXML
//
// Description  : Generate XML           
//
// Parameters   : 1) Output string       
//                2) Workflow variables
//                3) Property variables
//                4) Velocity macros
//                5) Translate templates
//
// Returns      : Count of items processed
//
//****************************************************************************
int WorkflowStep::GenXML(CviStr &theXml,
                         VariableList *theWfVars,
                         VariableList *thePropVars,
                         WorkflowStep *theMacros,
                         WorkflowStep *theTranslates)

{                                      // begin Generate

if (!itsRealStep)                      // if not a real step

    return 0;                          // do nothing


int     aStepCnt = 0;                  // count of child steps

char    aGenOnly = false;              // generate only?

WorkflowStep *aSavePrev = aPrev;       // save previous in case we need to back out

itsPrev = aPrev;                       // save previous

aPrev = this;                          // we are "previous" for our children

TRACE("GENXML Starting for %s\n", (const char *) itsName);

GenStdProperties(thePropVars);         // generate standard properties

if (!strcmp(itsType, "GROUP"))         // if GROUP

{                                      // begin process group

    TRACE("Generating content for group %s\n",
                           (const char *) itsName);

    CviStr aChildXml;                  // child XML

    int aChildCnt = 
        GenGroup(aChildXml,            // generate
                 theWfVars,            // XML for group
                 thePropVars,
                 theMacros,
                 theTranslates);

    aStepCnt += aChildCnt;             // bump step counter

    // If child count is good, process

    if (strlen(aChildXml) > 0       && // if XML was generated
         aChildCnt > 1)                // and it has multple children

    {                                  // begin add step

        theXml.Print                   // <step> tag
               ("\n<step name=\"%s\" ",
                (const char *) itsName);

        if (Optional())                // if it is optional
            theXml.Add("optional=\"true\">\n");    
        else                           // if not...
            theXml.Add("optional=\"false\">\n");

        itsTitle.VelocityUnescape();   // remove escape sequences

        PrepareContent(itsTitle,       // prepare title for XML
                       NULL,
                       thePropVars);

        FinalizeContent(itsTitle);

        itsTitle.VelocityUnescape();   // remove escape sequences

        PrepareContent(itsDescription, // prepare description for XML
                       NULL,
                       thePropVars);
        
        FinalizeContent(itsDescription);
        
        itsDescription.VelocityUnescape(); // remove escape sequences

        theXml.Print                   // <step> tag
               ("  <title>%.100s</title>\n  <description>%s</description>\n",
                (const char *) itsTitle,
                (const char *) itsDescription);

        theXml.Add(aChildXml);         // add child XML

        theXml.Add("</step>\n\n");     // end of step

        itsWritten = 1;                // step was written

    }                                  // end add step

    else                               // otherwise

    if (strlen(aChildXml) > 0)         // if valid

    {                                  // begin just use child

        TRACE("Group %s is not necessary.\n",
              (const char *) itsName);

        theXml.Add(aChildXml);         // add child

        // ERROR: If we are going to remove the group then anything relying on the group
        //        has to be pulled.

        WorkflowStep *aChild = FindSoloChild();

        if (aChild != NULL)
        {
            // Need to swap all prereqs to use the child's name...

            SetBypass();               // bypass

            AddPreReqs(aChild->itsName);// act as if this was a pre-req so RemoveMissingPrereqs
                                        // will add it to anyone relying on the group

            ((BuildWorkflow *) GetTaskCviPgm())->RemoveMissingPrereqs();        // remove missing prereqs

            aPrev = aChild;            // put this child as if it was the previous entry
                                       
        }
        else
        {
            GetTaskCviPgm()->Print("ERROR: Unused child not found!\n");
        }

    }                                  // end just use child

    else                               // otherwise

    {                                  // begin remove prereqs

        TRACE("Bypassing step %s, group %s - no content was generated.\n",
                               (const char *) itsName,
                               (const char *) itsGroup);

        SetBypass();                   // bypass us

        ((BuildWorkflow *) GetTaskCviPgm())->RemoveMissingPrereqs();        // remove missing prereqs

        aPrev = aSavePrev;             // restore previous

    };                                 // end remove prereqs

}                                      // end process group

else                                   // otherwise

{                                      // begin process leaf step

    TRACE("Generating step %s\n",
                            (const char *) itsName);

    if (!itsCombineSteps)              // if combining steps is NOT OK

        itsGrp->itsCombine = 'N';      // then do NOT combine

//
// This block needs to be split into its own method..
//

if (!strcmp(itsType, "SAVE")        && // if SAVE type
    (itsGrp != NULL &&                 // and we have a parent
     itsGrp->itsCombine == 'Y'))       // and parent is marked to combine

{                                      // begin convert to JCL
                                       
    CviStr aContent;                   // new content

    int aMemCnt = 1;                   // member count

    itsType.Set("JCL");                // make it JCL

    GenSaveJCL(aContent, aMemCnt);     // generate save JCL

    WorkflowStep *aStep = itsOrderChild; // do we have other peers?

    while (aStep != NULL)              // for all additional steps

    {
        TRACE("Looking to combine step %s %s\n",
                               (const char *) aStep->itsType,
                               (const char *) aStep->itsName);

        if (aStep->Bypassed(true))     // check for bypass (deep check)

        {
            
        }

        else

        if ((!strcmp(aStep->itsType, "SAVE") ||
             !strcmp(aStep->itsType, "DATASET_DELETE")) &&
            strlen(aContent) + strlen(aStep->itsContent) < 900000)  // 1M limit in z/OSMF...keep under it

        {

            MarkTemplate(aStep->itsTemplates); // combine template members used

            AddDeps(itsDepends, aStep->itsDepends);
            AddPreReqs(aStep->itsPreReqs);
            aStep->SetBypass();        // bypass

            if (aMemCnt == 1)          // if first item being combined

            {                          // begin change instructons

                itsInstructions.Set("Multiple member creation steps have been consolidated into a single JCL job for performance reasons. "
                                    "Submit this job to create the following members:<br/><ul>");
                itsInstructions.Print("    <li>%s</li>\n", (const char *) itsSaveAs);

            }                          // end change instructions

            aMemCnt ++;                // bump member count

            aStep->GenSaveJCL(aContent, // generate save JCL
                              aMemCnt);

            itsInstructions.Print("    <li>%s</li>\n", (const char *) aStep->itsSaveAs);

//            itsTitle.Reset();              // reset step title
//
//            itsTitle.Print("Create members %s through %s",
//                           (const char *) itsSaveAs,
//                           (const char *) aStep->itsSaveAs);
        }
        else
        if (!strcmp(aStep->itsType, "SAVE") ||
            !strcmp(aStep->itsType, "DATASET_DELETE"))

        {                              // begin setup for split

            break;                     // break out of loop

        }                              // end setup for split
                                       
        else
            break;

        aStep = aStep->itsOrderChild;  // next!
    }

// Originally had aMemCnt > 1 so it would leave save-as steps with adjacent steps alone.
// However, we would like to try make it convert them to JCL to be consistent and to
// get around the long-standing issue of z/OSMF not allowing saves to happen if the
// dataset is being browsed...

    if (aMemCnt >= 1)                  // if combined steps

    {                                  // begin handle end of combination

        CviStr aJobName;               // job name

        itsGrp->itsComboCnt ++;        // add combination counter

        itsMaxRc = 4;                  // a 4 is OK

        CviStr aMember(itsMember);     // default member name

        ProcessInternMacros            // run
              (itsSaveAs,              // internal
               this,                   // macros
               thePropVars,            // without
               false);                 // escape

        thePropVars->ReplaceWithValues(itsSaveAs,
                                       false);

        const char *aMem = 
            strstr(itsSaveAs, "(");    // got a member name?

        if (aMem != NULL)              // if found

        {                              // begin use member
           
            aMember.Set(aMem+1);       // copy name

            aMember.Trim(')');         // remove trailing )

        };                             // end use member

        itsSaveAs.Reset();             // do not attempt to save JCL by default

        if (itsGrp != NULL)            // if we have a parent

        {                              // grab save/job info

            itsSaveAs.Set(itsGrp->itsSaveAs); // use the save-as pattern from it

            if (aMemCnt > 1 ||         // if combined or
                strlen(itsJobName) == 0) // if we have no job name set

               aJobName.Set(itsGrp->itsJobName);

        };                             // end grab save/job info

        if (strlen(aJobName) > 0)      // if set

        {

            aJobName.Replace("${MEMBER}",
                             aMember);

            itsSaveAs.Replace("${MEMBER}",
                               aMember);

            aJobName.Replace("$MEMBER",
                             aMember);

            itsSaveAs.Replace("$MEMBER",
                               aMember);

            ProcessInternMacros        // run
                  (aJobName,           // internal
                   this,               // macros
                   thePropVars,        // without
                   false);             // escape

            thePropVars->ReplaceWithValues(aJobName,
                                           false);

            CviStr aNum;               // number

            aNum.Print("%d", itsGrp->itsComboCnt); // set job number

            if (strlen(aJobName) > 0)  // if length is OK

            {                          // begin use it

                itsJobName.Reset();    // clear current jobname

                itsJobName.Print("%.*s%s",
                                (int) 8 - strlen(aNum),
                                (const char *) aJobName,
                                (const char *) aNum);

            }                          // end use it

            itsSaveAs.Replace("$JOBNAME", itsJobName);
            itsSaveAs.Replace("${JOBNAME}", itsJobName);

            TRACE("JOBNAME: %s\nSave: %s\n",
                  (const char *) itsJobName,
                  (const char *) itsSaveAs);

        }

        itsContent.Set(aContent);      // use the new content

        itsTitle.Reset();              // reset title

        if (strlen(itsJobName) > 0)    // if we have a job name
        
            itsTitle.Print("Job %s: ",
                           (const char *) itsJobName);

          itsTitle.Add(itsGrp->itsTitle);  

//        itsTitle.Print("Batch Creation Of %d Members",
//                       aMemCnt);

        itsInstructions.Print("</ul>\n"); // terminate list

        itsDescription.Set(itsInstructions); // set to be the same

        ProcessInternMacros        // run
              (itsDescription,     // internal
               this,               // macros
               thePropVars,        // without
               false);             // escape

        thePropVars->ReplaceWithValues(itsDescription,
                                       false);

        BuildWorkflow *aPgm = (BuildWorkflow *) GetTaskCviPgm();

        aPgm->RemoveMissingPrereqs();  // after combining, adjust prereqs

    }                                  // end handle end of combination

    else                               // otherwise

    {                                  // begin leave it as a save-as step

        itsType.Set("SAVE");           // set it back to SAVE type

    }                                  // end leave it as a save-as step

};                                     // end convert to JCL

// SRC: End convert SAVE to JCL


    TRACE("GENXML preparing content for %s\n", (const char *) itsName);

    theXml.Print                   // <step> tag
           ("\n<step name=\"%s\" ",
            (const char *) itsName);

    if (Optional())                // if it is optional
        theXml.Add("optional=\"true\">\n");    
    else                           // if not...
        theXml.Add("optional=\"false\">\n");

    PrepareContent(itsTitle,       // prepare title for XML
                   NULL,
                   thePropVars);

    FinalizeContent(itsTitle);
    
    itsTitle.VelocityUnescape();   // remove escape sequences

    PrepareContent(itsDescription, // prepare description for XML
                   NULL,
                   thePropVars);

    FinalizeContent(itsDescription);
    
    itsDescription.VelocityUnescape();   // remove escape sequences
    
    PrepareContent(itsSkills);         // prepare skills for XML

    FinalizeContent(itsSkills);

    theXml.Print                       // <step> tag
           ("  <title>%.100s</title>\n  <description>%s</description>\n",
            (const char *) itsTitle,
            (const char *) itsDescription);

    itsWritten = 1;                    // step was written

    if (strlen(itsContent) > 0      || // if it has content
        strlen(itsInstructions) > 0)   // to write

    {                                  // begin assume leaf step

        TRACE("GENXML adding conditions for %s\n", (const char *) itsName);

        AddConditions(theXml,          // add conditions
                      thePropVars);

        if (itsPromptList != NULL)     // if there are prompts

        {                              // begin process prompts

            Variable *aPrompt = itsPromptList->GetFirst();

            while (aPrompt != NULL)    // for all prompts

            {                          // begin add prompt

                CviStr aScope(aPrompt->GetVelocityName());     // get velocity name

                const char *aScopeVal = aScope.Tok("${-", 2);
                const char *aNameVal = aScope.Tok("}");

                if (aScopeVal != NULL &&
                    !strcmp(aScopeVal, "INFR"))
                    aScopeVal = "global";

                if (aScopeVal != NULL &&
                    aNameVal != NULL)

                    theXml.Print("  <variableValue name=\"%s\" scope=\"%s\" noPromptIfSet=\"false\" required=\"true\"/>\n",
                                 aNameVal,
                                 aScopeVal);

                else

                    theXml.Print("  <variableValue name=\"%s\" scope=\"instance\" noPromptIfSet=\"false\" required=\"true\"/>\n",
                                 (const char *) aPrompt->GetName());

                aPrompt = itsPromptList->GetNext();

            };                         // end add prompt

        }                              // end process prompts

        TRACE("GENXML preparing additional content for %s\n", (const char *) itsName);

        if (theTranslates != NULL)     // if translation is wanted

        {
            theTranslates->Translate(itsContent);
            theTranslates->Translate(itsInstructions);
            theTranslates->Translate(itsSaveAs);
        }

        PrepareContent(itsInstructions,// prepare instructions for XML
                       theWfVars,
                       thePropVars);

        if (strlen(itsContent) > 0)    // if have content

            PrepareContent(itsContent, // prepare for XML
                           theWfVars,
                           thePropVars,
                           1);

        if (strlen(itsSaveAs) > 0)     // if have save-as

            PrepareContent(itsSaveAs,
                           theWfVars,
                           thePropVars,
                           false,
                           true);

        if (theTranslates != NULL)     // if we have translations

        {
            theTranslates->Translate(itsContent);
            theTranslates->Translate(itsInstructions);
            theTranslates->Translate(itsSaveAs);
        }

        AddMacros(theMacros);

        FinalizeContent(itsInstructions);
        FinalizeContent(itsContent);
        FinalizeContent(itsSaveAs);

        CviStr aWfVars;                // workflow variables used

        Variable *aVar = theWfVars->GetFirst(); // grab workflow variables

        while (aVar != NULL)           // for all variables

        {                              // begin check for variable

            TRACE("CHECKING FOR %s %s in content\n",
                  (const char *) aVar->GetName(),
                  (const char *) aVar->GetVelocityName());

            if (aVar->IsUsed(itsContent)) // is it used?

            {                          // begin prompt/show variable

                TRACE("Adding variable %s\n", 
                      aVar->GetName());

                Variable *aReq = aVar->FindSubVar("REQUIRED");

                CviStr aScope(aVar->GetVelocityName());     // get velocity name

                const char *aScopeVal = aScope.Tok("${-", 2);
                const char *aNameVal = aScope.Tok("}");

                if (aVar->FindSubVar("IS_USED") == NULL)

                    aVar->AddSubVariable
                        (new Variable("IS_USED",
                                      "YES"));

                if (aNameVal == NULL ||
                    strlen(aNameVal) == 0)
                    aNameVal = aVar->GetPropName();

                if (aScopeVal != NULL &&
                    !strcmp(aScopeVal, "INFR"))
                    aScopeVal = "global";

                if (aScopeVal == NULL ||
                    strlen(aScopeVal) == 0)

                    aScopeVal = "instance";

                if (aReq != NULL)
                    aWfVars.Print("  <variableValue name=\"%s\" scope=\"%s\" noPromptIfSet=\"false\" required=\"true\"/>\n",
                                  aNameVal,
                                  aScopeVal);
                else
                    aWfVars.Print("  <variableValue name=\"%s\" scope=\"%s\" noPromptIfSet=\"false\" required=\"false\"/>\n",
                                  aNameVal,
                                  aScopeVal);

            }                          // end prompt/show variable

            aVar = theWfVars->GetNext(); // next!

        }                              // end check for variable

        if (itsSuspend              && // if suspend is enabled
            !itsGenOnly)               // and not just generating

            theXml.Print("  <suspend/>\n"); // add suspend flag

        theXml.Add(aWfVars);           // add workflow variable info

        theXml.Print                   // write instructions tag
               ("  <instructions substitution=\"true\">\n%s\n  </instructions>\n",
                (const char *) itsInstructions);

        CviStr aAuto("true");          // auto-enable

        if (!itsAutoEnable          && // not auto-enabled and
            !itsGenOnly)               // not just generating

            aAuto.Set("false");        // set as false

        theXml.Print                   // write other standard stuff
               ("  <weight>500</weight>\n"
                "  <skills>%s</skills>\n"
                "  <autoEnable>%s</autoEnable>\n"
                "  <canMarkAsFailed>false</canMarkAsFailed>\n",
                (const char *) itsSkills,
                (const char *) aAuto);

        if (strlen(itsContent) > 0)    // if it has content

        {                              // begin write content

            TRACE("GENXML preparing final content for %s\n", (const char *) itsName);

#ifdef _SRC_
// Check for nested Velocity escape sequences #[[ fsfsfsfs #[[ fsafs ]]#  fsafs ]]#
            const char *aOpen=strstr(itsContent,"#[[");
            while (aOpen != NULL)
            {
              const char *aClose = strstr(aOpen, "]]#");
              if (aClose != NULL)
              {
//                 GetTaskCviPgm()->Print("Step %s has escapes\n", (const char *) itsName);
                 const char *aNext = strstr(aOpen+1, "#[[");
                 if (aNext != NULL && aNext < aClose)
                 {
                     GetTaskCviPgm()->Print("Warning: %s has nested Velocity escaping...\n%*.*s\n",
                           (const char *) itsName,
                           (int) (aClose - aOpen),
                           (int) (aClose - aOpen),
                           aOpen);
                 }
                 else
                     aOpen = aNext;
              };
            };
#endif

            TRACE("GENXML finalizing content for %s\n", (const char *) itsName);

            if (!strcmp(itsType, "JCL")           || // if JCL
                !strcmp(itsType, "DATASET_ALLOC") || // dataset allocation (JCL)
                !strcmp(itsType, "DATASET_DELETE") || // dataset delete (JCL)
                !strcmp(itsType, "DATASET_COPY")  || // dataset copy (JCL)
                !strcmp(itsType, "SAVE")          || // saving JCL
                !strcmp(itsType, "TSO-UNIX-shell") || // UNIX shell
                !strcmp(itsType, "TSO-REXX-JCL"))    // or REXX JCL

            {                          // begin add JCL

                CviStr aType = itsType;// default type


                if (strstr(itsType, "DATASET_ALLOC") != NULL ||
                    strstr(itsType, "DATASET_COPY") != NULL)

                    aType.Set("JCL");  // mark as JCL

                theXml.Print("  <template>\n");

                theXml.Print("    <inlineTemplate substitution=\"true\">");

                if (itsGenOnly &&
                    strstr(itsType, "SAVE") == NULL &&
                    strlen(itsSaveAs) != 0)

                {                  // begin generating only, slap on jobcard

                    aGenOnly = true; // generate only

                    CviStr aJobCard;

                    if (thePropVars != NULL)

                       thePropVars->ValueByName
                          ("CVIBLDWF_JOBCARD", aJobCard);

                    aJobCard.Replace("JOBNAME", (const char *) itsJobName);

                    theXml.Print("%s\n", (const char *) aJobCard);

                }                  // end generating only, slap on jobcard

                else                   // otherwise

                if (itsGenJobname &&            // if we are generating job names and
                    strstr(itsType, "SAVE") == NULL  &&   // not SAVE
                    strcmp(itsType, "DATASET_DELETE") &&  // and not DATASET_DELETE
                    strlen(itsJobName) > 0)     // if we have a job name

                {                      // begin add jobname

                    CviStr aJob;

                    aJob.Print("//%s JOB\n",
                               (const char *) itsJobName);

                    PrepareContent(aJob,
                                   theWfVars,
                                   thePropVars);

                    theXml += aJob;    // add to XML

                };                     // end add jobname
                
                theXml.Print("%s</inlineTemplate>\n",
                             (const char *) itsContent);

                TRACE("%s type %s\n",
                                       (const char *) itsName,
                                       (const char *) itsType);

                if (!aGenOnly &&            // not just generating JCL and
                    strcmp(itsType, "DATASET_DELETE") && // not dataset delete
                    strstr(itsType,         // if this is something to submit,
                           "SAVE") == NULL) // not just save

                {                      

                    TRACE("Writing out submitAs\n");

                    if (strlen(itsSuccess) != 0)

                    {

                        PrepareContent(itsSuccess, // prepare success content
                                       theWfVars,
                                       thePropVars);

                        FinalizeContent(itsSuccess);
                        
                        itsSuccess.VelocityUnescape();

                        theXml.Print("    <submitAs maxRc=\"%d\">%s</submitAs>\n"
                                     "    <successPattern>%s</successPattern>\n",
                                     itsMaxRc,
                                     (const char *) aType,
                                     (const char *) itsSuccess);

                    }

                    else

                        theXml.Print("    <submitAs maxRc=\"%d\">%s</submitAs>\n"
                                     "    <maxLrecl>80</maxLrecl>\n",
                                     itsMaxRc,
                                     (const char *) aType);

                }

                if (strlen(itsSaveAs) > 0) // if we should save it

                {                      // begin add save

                    //itsSaveAs.ReplaceSymbol("SYSPLEX",
                    //                        "${_workflow-sysplexName}");

                    //itsSaveAs.ReplaceSymbol("SYSNAME",
                    //                        "${_workflow-systemName}");

                    //PrepareContent(itsSaveAs,  // prepare for XML
                    //               theWfVars,
                    //               thePropVars,
                    //               false,
                    //               true);
                    
                    // Don't unescape - let it always do substitution
                    //if (strstr(itsSaveAs, "${") == NULL)
                    //{
                    //    itsSaveAs.VelocityUnescape();
                    //    theXml.Print("    <saveAsDataset substitution=\"false\">%s</saveAsDataset>",
                    //                 (const char *) itsSaveAs);
                    //}
                    //else
                    //
                        theXml.Print("    <saveAsDataset substitution=\"true\">%s</saveAsDataset>",
                                     (const char *) itsSaveAs);

                }                      // end add save

                theXml.Print("  </template>\n");

            };                         // end add JCL

        };                             // end has content

    };                                 // end assum leaf step

    theXml.Print("</step>\n\n");       // end of step

    itsContent.Reset();                // no longer need content
    itsInstructions.Reset();           // no longer need instructions

    aStepCnt ++;                       // bump step count

};                                     // end process leaf step 

TRACE("GENXML finished %s\n", (const char *) itsName);


return(aStepCnt);                      // return step counter

};                                     // end Generate

//****************************************************************************
//
// Method       : WorkflowStep::GenSaveJCL
//
// Description  : Generate JCL to create a member
//
// Parameters   : 1) Output string (will append)      
//
// Returns      : Nada
//
//****************************************************************************
void WorkflowStep::GenSaveJCL(CviStr &theJCL,
                              int theStepNum)

{                                      // begin GenSaveJCL

CviStr aCond;                          // conditional for step 

itsSaveAs.Replace("$JOBNAME", itsJobName);
itsSaveAs.Replace("${JOBNAME}", itsJobName);
itsSaveAs.Replace("$NAME", itsName);
itsSaveAs.Replace("${NAME}", itsName);
itsSaveAs.Replace("$GROUP", itsGroup);
itsSaveAs.Replace("${GROUP}", itsGroup);

itsContent.Replace("$JOBNAME", itsJobName);
itsContent.Replace("${JOBNAME}", itsJobName);
itsContent.Replace("$NAME", itsName);
itsContent.Replace("${NAME}", itsName);
itsContent.Replace("$GROUP", itsGroup);
itsContent.Replace("${GROUP}", itsGroup);

if (itsBypassIfSaveAsExists)           // should we bypass if member exists?

{                                      // begin setup bypass check

    theJCL.Print("//CHECK%d EXEC PGM=IKJEFT01\n", theStepNum);
    theJCL.Print("//SYSTSPRT DD SYSOUT=*\n");
    theJCL.Print("//SYSTSIN DD *\n");
    theJCL.Print("LISTDS '%s'\n", (const char *) itsSaveAs);
    theJCL.Print("/*\n");

    aCond.Print(",COND=(0,EQ,CHECK%d)", theStepNum);

}                                      // end setup bypass check

theJCL.Print("//CREAT%d EXEC PGM=IEBGENER%s\n",
             theStepNum,
             (const char *) aCond);

theJCL.Add("//SYSPRINT DD SYSOUT=*\n");
theJCL.Add("//SYSIN DD DUMMY\n");
theJCL.Print("//SYSUT2 DD DSN=%s,\n",
                 (const char *) itsSaveAs);
theJCL.Add("//            DISP=(SHR,KEEP)\n");
theJCL.Add("//SYSUT1 DD DATA,DLM=Z8\n");

theJCL.Add(itsContent);                // add content

theJCL.Add("Z8\n");                    // add ending marker

theJCL.Add("//*\n");                   // add blank line

}                                      // end GenSaveJCL

//****************************************************************************
//
// Method       : BuildWorkflow::BuildWorkflow
//
// Description  : Constructor
//
//****************************************************************************
BuildWorkflow::BuildWorkflow(int theArgc,
                             const char **theArgv) :
    CviPgm("CVIWFBLD", theArgc, theArgv)

{                                      // begin constructor

itsSteps = NULL;                       // no steps yet

itsGroups = NULL;                      // no groups yet

itsLastStep = NULL;                    // last step processed

itsIncludes = NULL;                    // no includes yet

itsMacros = NULL;                      // no macros yet

itsWfMacros = NULL;                    // no workflow macros yet 

itsPopSteps = NULL;                    // no population steps yet

itsDatasets = NULL;                    // no dataset steps yet

itsTempCount = 0;                      // no templates found yet

itsRealCount = 0;                      // no real templates yet

itsPropVars = &itsProdVars;            // use product variables

itsPrompts = NULL;                     // no prompt templates

itsTranslates = NULL;                  // no translate templates

itsAddTargets =                        // create target
   new Variable("TARGET","");          // merge variable

};                                     // end constructor

//****************************************************************************
//
// Method       : BuildWorkflow::Init
//
// Description  : Initializes
//
//****************************************************************************
int BuildWorkflow::Init(void)          // initialize

{                                      // begin Init

int aRc = 0;                           // return code

Print("Workflow wiZard %s Initializing\n",
      CVI_VERSTRING);
Print("Built %s %s\n", __DATE__, __TIME__);

#ifdef PTF
#define MacLit( MacLitParm) #MacLitParm
#define MacToStr( MacParm ) MacLit(MacParm)
Print("Maintenance level %s applied.", MacToStr(PTF));
#else
Print("No maintenance has been applied.");
#endif

Print("\n");


return(aRc);                           // return result

}                                      // end init

//****************************************************************************
//
// Method       : BuildWorkflow::Reset
//
// Description  : Reset workflow builder variables
//
//****************************************************************************
void BuildWorkflow::Reset(void)        // reset

{                                      // begin Reset

itsTempCount = 0;                      // reset

itsRealCount = 0;                      // reset

};                                     // end reset

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessInput
//
// Description  : Process input commands
//
//****************************************************************************
int BuildWorkflow::ProcessInput()      // process input

{

int aRc = 0;                           // OK so far

CviStr aData;                          // input data


int aArg = 1;
while (aArg < itsArgc)
{
    aData.Add(itsArgv[aArg]);
    aData.Add("\n");
    aArg ++;
}

Trace("Read input: %d\n", aRc);
Trace("Data: \n%s\n", (const char *) aData);

if (aRc == 0)                          // if read was good

{                                      // begin process data

    const char *aLine =                // get first
        aData.Tok("\n");               // line

    while (aLine != NULL)              // for all lines

    {                                  // begin process line

        CviStr aStmt(aLine);           // statement

        aStmt.Trim(' ');               // trim trailing spaces

        const char *aCmd =             // get 
            aStmt.Tok("=");            // command

        if (aCmd != NULL)              // valid?

        {                              // begin process command

            const char *aVal =         // get value
                aStmt.Tok("");         // get remaining line

           if (!strcmp(aCmd,           // template DD?
                       "TEMPLATE"))

           {                           // begin TEMPLATE

               if (aVal != NULL)       // if valid

                   AddList(itsTemplateDSNList,
                           aVal);

           }                           // end TEMPLATE

           else                        // otherwise

           if (strstr(aCmd,
                      "PROP") == aCmd) // if property

           {                           // begin PROP*

               if (aVal != NULL &&     // if valid
                   strcmp(aCmd, "PROPERTY")) // and not PROPERTY


               {                       // begin store pair

                   CviStr aPair;

                   aPair.Print("%s=%s",
                               (const char *) aCmd,
                               (const char *) aVal);

                   AddList(itsPropertiesDSNList,
                           aPair);

               }                       // end store pair

               else                    // otherwise

               if (aVal != NULL)       // if valid

               {
                   itsPropertiesMem.Reset();

                   itsPropertiesMem.Print("//'%s'",
                                          aVal);

               }


           }                           // end PROP*

           else                        // otherwise

           if (!strcmp(aCmd,
                       "WORKFLOW"))    // if workflow member

           {                           // begin WORKFLOW_MEMBER

               itsWorkflowMem.Reset();  // reset

               if (aVal != NULL)       // if valid

                   itsWorkflowMem.Print("//'%s'",
                                       (const char *) aVal);


           }                           // end WORKFLOW_MEMBER

           else                        // otherwise

           if (!strcmp(aCmd,
                       "WORKFLOW_DSN")) // if workflow DSN

           {                           // begin WORKFLOW_DSN

               if (aVal != NULL)       // if valid

                   itsWorkflowDSN.Set(aVal);


           }                           // end WORKFLOW_DSN

           else                        // otherwise
           if (!strcmp(aCmd,           // template DD?
                       "TRACE"))    

           {                           // begin TEMPLATE

               ToggleTrace(true);

               if (aVal != NULL)       // if valid

                   AddList(itsTemplateDSNList,
                           aVal);

           }                           // end TEMPLATE

           else                        // otherwise

           {                           // begin failure

               Error("%s is not a valid command.\n",
                     aCmd);

               aRc = 8;                // fail

           }                           // end failure

        };                             // end process command

        aLine = aData.Tok("\n");       // next line

    };                                 // end process line

}                                      // end process data

else                                   // otherwise

{                                      // begin fail

    aRc = 8;                           // failed

    Error("Failed opening SYSIN DD\n");

};                                     // end fail

Trace("Processed input: %d\n",
      aRc);


return(aRc);                           // return result

}                                      // end ProcessInput


//****************************************************************************
//
// Method       : BuildWorkflow::Work
//
// Description  : Does actual work
//
//****************************************************************************
int BuildWorkflow::Work()              // build workflow

{

int aRc = 0;                           // OK so far


if (aRc == 0)                          // if OK

    aRc = ProcessInput();              // process input commands

if (aRc == 0)                          // if OK

    aRc = GenStdProperties();          // generate standard properties

if (aRc == 0)                          // if OK

    aRc = ReadProperties               // read in regular
            (itsPropertiesMem,         // properties file
             true);                    // replace pre-existing entries

if (aRc == 0)                          // if OK

    aRc = ReadOverrideProperties();    // read override properties

if (aRc == 0)                          // if OK

    aRc = ProcessShared();             // process shared stuff first

aMacroCache.Clear();                   // clear workflow macro cache from shared

if (aRc == 0)                          // if OK

    aRc = ResolveUnixDSNs();           // try to resolve unknown UNIX DSNs

if (aRc == 0)                          // if OK

{
    if (itsSteps == NULL)
    {
        aRc = ReadTemplates();         // read and parse templates  
        if (aRc == 0)                  // if OK
            AddRowVars();              // add row variables
    }
    else
    {
        AddRowVars();                  // make sure row vars are added
        aRc = ParseTemplates();        // re-parse templates
    }
}

if (aRc == 0)                          // if OK

    aRc = ProcessPrompts();            // process prompts

if (aRc == 0)                          // if OK

    aRc = ProcessDSN();                // process DSN steps

if (aRc == 0)                          // if OK
                                       
    aRc = ProcessPopulate();           // process populate

if (aRc == 0)                          // if OK
                                       
    aRc = CreatePopArray();            // create population array for DSNs

if (aRc == 0)                          // if OK

    aRc = SizePopulate();              // size population templates Accumulated 1.25

if (aRc == 0)                          // if OK

    aRc = MergeSetVariables();         // merge set variables

if (aRc == 0)                          // if OK

    aRc = ProcessTranslates();         // process translation templates

if (aRc == 0)                          // if OK

    aRc = ProcessStatements();         // process statements

if (aRc == 0)                          // if all is well
                                       
    aRc = AssignGroups();              // assign groups
                                       
if (aRc == 0)                          // if all is well
                                       
    aRc = OrderSteps(itsGroups);       // order the steps, starting with root group
                                       
if (aRc == 0)                          // if still OK
                                       
    RemoveMissingPrereqs();            // adjust for bypassed prerequisites
                                       
if (aRc == 0)                          // if all is well

{                                      // begin generate or re-run

    if (strlen(itsWorkflowMem) > 0)    // if workflow DD is specified

        aRc = GenerateWF
                (itsWorkflowMem);      // generate workflow

    if (aRc == 0 &&                    // if OK and
        strlen(itsWorkflowDSN) > 0)    // is WFOUT defined?

         aRc = GenerateDepWFs();       // generate workflows

}                                      // end generate or re-run
                                       
if (aRc == 0)                          // if OK

    aRc = DisplaySteps();              // display steps

Print("\nProcessing complete: %d\n", aRc);

return(aRc);                           // return code holder

}

//****************************************************************************
//
// Method       : BuildWorkflow::ResolveUnixDSNs
//
// Description  : Attempt to resolve unknown datasets for UNIX mount points,,
//
//****************************************************************************
int BuildWorkflow::ResolveUnixDSNs()   // resolve UNIX DSNs

{                                      // begin ResolveUnixDSNs

Variable *aVar = itsPropVars->GetVariable("TARGET");

CviStr aHLQs;                          // HLQs

int aRc = 0;                           // assume the best


if (aVar != NULL)                      // if OK

    aVar = aVar->GetSubVar();          // get subvariables

Variable *aHLQVar = aVar;              // start with first one

while (aHLQVar != NULL)                // for all targets

{                                      // begin find HLQs

    Variable *aDSN = aHLQVar->FindSubVar("DSN");

    if (aDSN != NULL)                  // if OK

    {                                  // begin get HLQ

        const char *aName = aDSN->Value();

        const char *aLast = aName + strlen(aName);

        while (aLast != aName &&       // back up
               *aLast != '.')          // to last .
            aLast --;

        CviStr aHLQ;                   // HLQ holder

        aHLQ.Print("%*.*s",            // set HLQ
                   (int) (aLast - aName),
                   (int) (aLast - aName),
                   aName);

        if (strlen(aHLQ) != 0 &&       // if HLQ is OK
            AddList(aHLQs, aHLQ))      // and it is new

        {                              // begin trace

            TRACE("Found HLQ %s from %s\n", (const char *) aHLQ, aName);

        }                              // end trace

    }                                  // end get HLQ

    aHLQVar = aHLQVar->GetNext();      // next!

}                                      // end find HLQs

while (aVar != NULL)                   // for all targets

{                                      // begin find targets without a dataset

    Variable *aMount =                 // see if it has a 
           aVar->FindSubVar("MOUNT");  // mount point

    if (aMount != NULL)                // does this have a mount point?

    {                                  // begin UNIX target

        bool    aMountOK = false;      // assume mount point is not OK

        Variable *aLLQ = aVar->FindSubVar("BMC-ORIG-LLQ");
        Variable *aDSN = aVar->FindSubVar("DSN");

        DIR *aDir = opendir(aMount->Value());

        if (aDir != NULL)              // if opened

        {                              // begin try to get location

            dirent *anEnt = readdir(aDir);

            while (!aMountOK &&        // while mount is not known to be OK
                   anEnt != NULL)      // and we have more entries to process

            {                          // begin examine entry

                if (strcmp(anEnt->d_name, ".") &&  // if not .
                    strcmp(anEnt->d_name, ".."))   // nor ..

                    aMountOK = true;   // we are OK

                anEnt = readdir(aDir); // next!

            };                         // end examine entry
                                       
            closedir(aDir);            // close it

        };                             // end try to get location

        if (!aMountOK)                 // if mount is not OK

            aVar->AddSubVariable
                    (new Variable("MOUNT_STATUS",
                                  "SUSPECT"));

        else

            aVar->AddSubVariable
                    (new Variable("MOUNT_STATUS",
                                  "OK"));

        Print("UNIX mount point %s status: %s\n",
              (const char *) aMount->Value(),
              (const char *) aVar->FindSubVar("MOUNT_STATUS")->Value());

    }                                  // end UNIX taregt

    aVar = aVar->GetNext();            // next!                       

};                                     // end find targets without a dataset


return(aRc);                           // return result

}                                      // end ResolveUnixDSNs



//****************************************************************************
//
// Method       : BuildWorkflow::ProcessShared
//
// Description  : Process shared properties
//
//****************************************************************************
int BuildWorkflow::ProcessShared()     // process shared stuff

{                                      // begin ProcessShared

VariableList *aSaveList = itsPropVars; // save list

int aRc = 0;                           // assume the best

CviStr aDSNs(itsPropertiesDSNList);    // get p

const char *aDSN = aDSNs.Tok(",");     // get first DD

while (aDSN != NULL)                   // for all DDs

{                                      // begin iterate DDs

    CviStr aDSNStr(aDSN);              // get DSN into string


    const char *aDD = aDSNStr.Tok("=");// get DD portion

    if (aDD != NULL)                   // if DD is OK

    {                                  // begin process 

        const char *aDSN = aDSNStr.Tok(""); // get DSN

        VariableList aShrList;         // shared property list

        itsPropVars = &aShrList;       // use this list


        CviStr aPrefix;                // prefix for variables
        CviStr aDDName;                // DDname


        GenStdProperties();            // generate standard properties

        aPrefix.Print("%s-",           // prefix - trailing portion of name
                      aDD + strlen("PROP"));

        aDDName.Print("//'%s", aDSN);  // setup name

        aDDName.Trim(' ');             // remove trailing spaces

        Print("\nPROCESSING SHARED PROPERTIES FOR %s %s\n",
              aDD,
              aDSN);

        aRc = ReadProperties(aDDName); // read in 
        
        if (aRc == 0)                  // if OK

            aRc = ReadOverrideProperties(true);    // read override properties

        if (aRc == 0)                  // if OK

            aRc = ReadTemplates();     // read templates

        if (aRc == 0)                  // if OK

            AddRowVars();              // add row variables

        if (aRc == 0)

            aRc = ProcessDSN();        // process DSNs

        if (aRc == 0)                  // if OK

            aRc = ProcessPopulate      // process population entries
                    (aPrefix);         // adding appropriate prefix

        Dataset *aDS = itsDatasets;    // top of dataset list

        while (aDS != NULL)            // for all datasets

        {                              // begin add scope to DSNID from properties

            CviStr aDSNID;             // new ID
            CviStr aDSN;               // DSN value

            aDSNID.Print("%s%s",       // setup to have prefix
                         (const char *) aPrefix,
                         aDS->GetID());

            aDS->SetID(aDSNID);        // set new ID

            aDS->SetAllocate(false);   // do not use for allocation/population

            aDS = aDS->GetNext();      // next!

        };                             // end add scope to DSNID from properties

        Variable *aVar =               // get
            aShrList.GetFirst();       // first variable

        while (aVar != NULL)           // for all variables

        {                              // begin add to product list

            const char *aName =        // get
                aVar->GetName();       // name

            CviStr aValue(aVar->Value());
                   
            Variable *aSubVar =        // get subvariable, if any
                aVar->GetSubVar();

            Variable *aMatchVar = NULL; // match set variable

            if (aSubVar != NULL)       // if this has subvariables

                aMatchVar =            // find matching variable
                  itsProdVars.GetVariable(aName);

            if (!strcmp(aName, "TARGET")) // if target DSNs

                aMatchVar = itsAddTargets; // use this one to merge into

            if (aMatchVar != NULL)     // if matching set variable

            {                          // begin add to existing set?

                aMatchVar->            // add subvariables to list
                    AddSubVariable(aSubVar);

                aVar->DetachSubVar();  // detach subvariables

            }                          // end add to existing set?

            else                       // otherwise

            {                          // begin create new

                CviStr aNewName(aName);// setup name

                if (strstr(aName,      // if not prefixed properly
                           aPrefix) != aName &&
                    strstr(aName,      // and not a rowvar
                           "ROWVAR_") == NULL)

                {

                    aNewName.Reset();

                    aNewName.Print("%s%s",  // setup with prefix
                                   (const char *) aPrefix,
                                   aName);
                };

                aShrList.TranslateJCLValues(aValue);

                TRACE("Adding %s=%s to product properties\n",
                      (const char *) aNewName,
                      (const char *) aValue);

                if (aSubVar != NULL)   // if we have subvariables

                {                      // begin add subvariables

                    TRACE("Adding subvariables\n");

                    itsProdVars.Add(aNewName, // add to product variables
                                    aValue,
                                    1);

                    aMatchVar =            // grab variable
                        itsProdVars.GetVariable(aNewName);

                    aMatchVar->            // add subvariables to list
                        AddSubVariable(aSubVar);

                    TRACE("Var %p...subvar %p\n", 
                          aMatchVar,
                          aMatchVar->GetSubVar());

                    aVar->DetachSubVar();  // detach subvariables

                }                      // end add subvariables

                else

                    itsProdVars.Add(aNewName, // add to product variables
                                    aValue,
                                    1);

            }                          // end add to product variables

            aVar = aShrList.GetNext(); // get next

        };                             // end add to product list

        Reset();                       // reset

        Print("FINISHED PROCESSING SHARED PROPERTIES FOR %s ---\n",
              aDSN);

    };                                 // end process shared property DD

    aDSN = aDSNs.Tok(",");             // next!

};                                     // end iterate DDs

itsPropVars = aSaveList;               // restore list


return(aRc);                           // return

};                                     // end ProcessShared


//****************************************************************************
//
// Method       : BuildWorkflow::ReadOverrideProperties
//
// Description  : Reads override property file
//
//****************************************************************************
int BuildWorkflow::ReadOverrideProperties(bool theReRead)

{                                      // begin ReadOverrideProperties

int r_c = 0;                           // all OK for now

char aCurDir[128];                     // current directory
                                       
aCurDir[0] = 0;                        // clear out to be safe
                                       
getcwd(aCurDir, sizeof(aCurDir)-1);    // get working directory
                                       
if (strlen(aCurDir) > 0)               // if OK
                                       
{                                      // begin read from home
                                       
    CviStr aProp(aCurDir);             // property file
                                       
    aProp.Print("/ngibldwf.properties");
                                       
    ReadProperties                     // read in user override, if available
      (aProp, true);                   // properties file
                                       
    aProp.Reset();                     // reset

    aProp.Set(aCurDir);                // property file
                                       
    aProp.Print("/cvibldwf.properties");
                                       
    ReadProperties                     // read in user override, if available
      (aProp, true, theReRead);        // properties file - be silent about it if re-reading
                                       
};                                     // end read from home

if (!theReRead)                        // if not a re-read

{                                      // begin process/display

    CviStr aVal;                       // variable value

    if (itsPropVars->ValueByName       // get variable value
                    ("CVIBLDWF_COMBINE_STEPS",
                     aVal) == 0 &&     // check if it should 
        !strcmp(aVal, "OFF"))          // disable combining steps

    {                                  // begin disable combining steps
                                       
        itsCombineSteps = false;       // turn off step combining
                                       
        Print("Combining of steps has been disabled.\n");
                                       
    };                                 // end disable combining steps

    if (itsPropVars->ValueByName       // get variable value
                    ("CVIBLDWF_DEV_PRINT",
                     aVal) == 0 &&     // check if it should 
        !strcmp(aVal, "ON"))           // enable dev print

    {                                  // begin enable dev print
                                       
        itsDevPrint = true;            // turn on dev print
                                       
        DPRINT("Additional DEV messages are now enabled.\n");
                                       
    };                                 // end enable dev print

    if (itsPropVars->ValueByName       // get variable value
                    ("CVIBLDWF_GEN_JOBNAME",
                     aVal) == 0 &&     // check if jobname should be set standard
        strcmp(aVal, "STANDARD"))      // or not

    {                                  // begin do NOT generate JOBNAME
                                       
        itsGenJobname = false;         // do NOT generate JOBNAME for job card
                                       
        Print("Job names will NOT be generated in the job card\n");
                                       
    };                                 // end do NOT generate JOBNAME

    if (itsPropVars->ValueByName       // get variable value
                    ("CVIBLDWF_GENERATE_ONLY",
                     aVal) == 0 &&     // check if it should setup to just
        !strcmp(aVal, "ON"))           // generate jobs

    {                                  // begin do NOT generate JOBNAME
                                       
        itsGenOnly = true;             // only generate JCL, do not submit, when possible
                                       
        Print("JCL will be setup to be generated rather than submitted.\n");
                                       
    };                                 // end do NOT generate JOBNAME

    CviStr aJCLOut;                    // JCL OUT DSN
    Variable *aVar = itsPropVars->GetVariable("CVIBLDWF_JCLOUT");

    if (aVar != NULL)                  // if found

        aJCLOut.Set(aVar->Value());    // grab value

    if (itsPropVars->ValueByName       // get variable value
                   ("CVIBLDWF_TRACE",  // to check if trace
                    aVal) == 0 &&      // should be on
        !strcmp(aVal, "ON"))

    {                                  // begin do NOT generate JOBNAME
                                       
        ToggleTrace(1);                // turn on trace

        aTrcPgm = this;                // enable for our macro

        Print("Trace has been enabled\n");
                                       
    };                                 // end do NOT generate JOBNAME

};                                     // end process/display

return(r_c);                           // return 

}                                      // end ReadOverrideProperties

//****************************************************************************
//
// Method       : BuildWorkflow::MergeSetVariables
//
// Description  : Merge needed set variables
//
//****************************************************************************
int BuildWorkflow::MergeSetVariables()

{

Variable *aVar =                       // get target
   itsPropVars->GetVariable("TARGET"); // variable

if (aVar != NULL)                      // if OK

{                                      // begin merge with other targets

    Variable *aSubVar =                // get subvars of merge list
        itsAddTargets->GetSubVar();     

    if (aSubVar != NULL)               // if OK

    {                                  // begin merge

        aVar->AddSubVariable(aSubVar); // add it

        AddRowVars(aSubVar);           // generate row variables

        itsAddTargets->DetachSubVar(); // detach subvariable list from holding list

    };                                 // end merge

};                                     // end merge with other targets

return(0);                             // should always be OK

}

//****************************************************************************
//
// Method       : BuildWorkflow::AddRowVars
//
// Description  : Add row/column variables
//
// This code creates a variable for each row in "set/multirow" variable lists.
// The for_each processing uses these variables and it gives the workflow
// template developer a choice of using them directly if they wish...
//
//****************************************************************************
void BuildWorkflow::AddRowVars()

{

Variable *aVar = itsPropVars->GetFirst(); // get first variable

while (aVar != NULL)                   // for all variables

{                                      // begin look for set

    Variable *aSubVar =                // see if it has subvariables (set)
        aVar->GetSubVar();             

    while (aSubVar != NULL)            // for all rows
                                       
    {                                  // begin get children
                                       
        AddRowVars(aSubVar);           // add row variables

        aSubVar = aSubVar->GetNext();  // get next row
                                       
    };                                 // end get children

    aVar = itsPropVars->GetNext();     // get next variable

};                                     // end look for set
                                       
}

//****************************************************************************
//
// Method       : BuildWorkflow::AddRowVars
//
// Description  : Add row/column variables
//
//****************************************************************************
void BuildWorkflow::AddRowVars(Variable *theVar)

{

AddColVars(itsPropVars, theVar);       // add column substitution variables

}

//****************************************************************************
//
// Function     : ParseJCLProp
//
// Description  : Process JCL property line (SET line)
//
//****************************************************************************
void ParseJCLProp(CviStr &theString,
                  CviStr &theName,
                  CviStr &theValue)

{

if (strstr(theString, "// ") ==        // if it starts with // 
     (const char *) theString       && // and
    strstr(theString, " SET ") != NULL)// has SET statement

{                                      // begin locate name/value

    const char *aSet =                 // find SET
        strstr(theString, " SET ");    // locate SET

    if (aSet != NULL)                  // if OK
        aSet += strlen(" SET ");       // skip beyond it to find name

    while (aSet != NULL &&             // while SET found OK and
           *aSet != 0 &&               // not end of string
           *aSet == ' ')               // still a space

        aSet ++;                       // skip whitespace

    if (aSet != NULL)                  // if name found

    {                                  // begin get name/value

        CviStr aNameVal(aSet);         // name/value

        const char *aTok =             // get
            aNameVal.Tok("=");         // name

        if (aTok != NULL)              // if OK

            theName.Set(aTok);         // use it

        aTok = aNameVal.Tok("");       // get remainder

        if (aTok != NULL            && // if OK and
            *aTok == '\'')             // starts with '
                                       
        {                              // begin get rest of value

            aTok ++;                   // skip quote

            const char *aVal = aTok;   // start of value

            while (*aTok != 0 &&       // if not end of string
                   *aTok != '"')       // and no ending quote found
                                       
                aTok ++;               // keep going

            if (*aTok == '\'')         // if found the end
             
            {                          // begin use value

                theValue.Print("%.*s",
                               aTok - aVal,
                               aVal);

            }                          // end use value

        }                              // end get rest of value

        else                           // otherwise

        {                              // begin set basic

            const char *aVal = aTok;   // get value

            while (*aTok != 0 &&       // not end of string and
                   *aTok != ' ')       // not space
                aTok ++;               // keep going

            theValue.Print("%.*s",
                           aTok - aVal,
                           aVal);

        }                              // end set basic

    }                                  // end get name/value

};                                     // end locate name/value

}

//****************************************************************************
//
// Function     : ParseProperty
//
// Description  : Process property file line
//
//****************************************************************************
void ParseProperty(CviStr &theString,
                   CviStr &theName,
                   CviStr &theValue)

{

const char *aTok =                     // find
    theString.Tok("=");                // delimiter
                                       
if (*((const char *) theString) != '#' &&  // if not commented out and
    aTok != NULL)                      // if delimiter found
                                       
{                                      // begin process variable
                                       
    theName.Set(aTok);                 // set variable name
                                       
    theName.Replace(" ", "");          // strip any whitespace
    theName.Replace("\t", "");         // strip any whitespace
                                       
    const char *aTok =                 // get remaining
        theString.Tok("");             // string
                                       
    if (aTok != NULL)                  // if OK
                                       
        theValue.Set(aTok);            // use it
                                       
    theValue.Replace("\\n", "\n");     // change out newlines
                                       
}

}

//****************************************************************************
//
// Method       : BuildWorkflow::ReadProperties
//
// Description  : Reads properties file
//
//****************************************************************************
int BuildWorkflow::ReadProperties(const char *theFilename,
                                  bool theReplace,
                                  bool theSilent)

{                                      // begin ReadProperties

bool aJCL = false;                     // assume proprrties, not JCL

int r_c = 0;                           // all OK for now
int aCount = 0;                        // property count
int aDisplay = 0;                      // assume we shouldn't display

FILE *aInFp = fopen(theFilename,       // open
                    "r");              // properties

CviStr  aContent;                      // content of file

if (aInFp != NULL)                     // if OK

{                                      // begin read file

    if (!theSilent &&                  // if we shouldn't be silent
        strstr(theFilename,            // if DSN
               "//'") != NULL)         // passed

    {                                  // begin write message

        const char *aDDName =          // grab ddname
            strstr(theFilename, "//'");

        if (aDDName != NULL)           // if found

            Print("Reading properties from %s.\n",
                  aDDName+2);          

        else

            Print("Reading properties from %s.\n",
                  theFilename); 

    }                                  // end write message

    else                               // otherwise

    if (!theSilent)                    // if we shouldn't be silent

    {                                  // begin override

        Print("Reading override properties from %s\n",
              theFilename);

        aDisplay = 1;                  // display is on

    };                                 // end override

    char    aLine[512];                // input buffer

    while (fgets(aLine,                // while we have
                 sizeof(aLine),        // read more
                 aInFp) != NULL)       // data

        aContent.Add(aLine);           // add to content

    fclose(aInFp);                     // close file

}                                      // end read file

else                                   // otherwise

{                                      // begin fail

    if (strstr(theFilename, "//'") == theFilename)

        Print("Failed to open %s\n"  , // complain
              theFilename);

    r_c = 8;                           // failed

};                                     // end fail


const char *aLine = aContent.Tok("\n");// grab first line

if (strstr(aLine, "//*") == aLine)     // if JCL-style SET

    aJCL = true;                       // set JCL flag

CviStr  aVarStr;                       // variable string

while (aLine != NULL)                  // for all lines

{                                      // begin process line

   aVarStr.Set(aLine);                 // set variable string value

   while (!aJCL &&                     // while not JCL and continuation exists
          *(((const char *) aVarStr)+strlen(aVarStr)-1) == '\\')
   {
       aVarStr.Trim('\\');
       aVarStr.Add(aContent.Tok("\n"));
   }

   aVarStr.Replace("\n", "");          // strip newline

   CviStr aVarName;                    // variable name
   CviStr aVarVal;                     // variable value
                                       
   if (aJCL)                           // if JCL

       ParseJCLProp(aVarStr,           // parse JCL
                    aVarName,          // to get
                    aVarVal);          // name/value pair

   else                                // otherwise

       ParseProperty(aVarStr,          // parse
                     aVarName,         // property
                     aVarVal);         // to get name/value pair

   if (strlen(aVarName) != 0)          // if we have an entry
                                       
   {                                   // begin process variable
                                       
       if (!strcmp(aVarName,           // if jobStatement
                    "jobStatement"))
       {
          const char *aVal = aVarVal;  // begin check quoted

          if (*aVal == '"')            // replace start/end quotes
          {
              CviStr aTmp;             // strip quotes
              aTmp.Add(aVal+1,         
                       strlen(aVal)-2);
              aVarVal.Set(aTmp);       // replace with new
          }                            

       };                              // end check quoted

       if (aDisplay)

       {

           if (strstr(aVarVal, "\n") != NULL)

           {

               Print("Property %s value is:\n",
                     (const char *) aVarName);

               if (strstr(aVarName, "NGIBLDWF_") != NULL)

               {
                   aVarName.Replace("NGIBLDWF_",
                                    "CVIBLDWF_");

                   Print("    (Changed variable name to %s)\n",
                         (const char *) aVarName);

               }

               CviStr aDisp(aVarVal);

               const char *aLine = aDisp.Tok("\n");
               while (aLine != NULL)
               {
                   Print("      %s\n",
                         aLine);
                   aLine = aDisp.Tok("\n");
               }

           }

           else

               Print("Property %s value is %s\n",
                     (const char *) aVarName,
                     (const char *) aVarVal);


       }

       else

           TRACE("%s: Property %s value is %s\n",
                 theFilename,
                 (const char *) aVarName,
                 (const char *) aVarVal);

       char aDupeCheck = 0;

       if (theReplace)                 // if in replace mode

           aDupeCheck = 2;             // replace duplicates

       if (itsPropVars->Add            // add variable
                      (aVarName,      
                       aVarVal,
                       aDupeCheck) != 0)

       {                               // begin duplicate variable

           r_c = 8;                    // fail

           Error("PROPERTY VALUE %s HAS BEEN SPECIFIED TWICE.\n",
                 (const char *) aVarName);

       }                               // end duplicate variable

       else                            // otherwise

           aCount ++;                  // add to count

   };                                  // end process variable
                                       
   aLine = aContent.Tok("\n");         // grab first line

}                                      // end process line

if (r_c == 0 && !theSilent)            // if OK

    Print("Read %d properties.\n", aCount);

//Print("\n");


return(r_c);                           // return result

}                                      // end ReadProperties

//****************************************************************************
//
// Method       : BuildWorkflow::ReadTemplates
//
// Description  : Reads templates
//
//****************************************************************************
int BuildWorkflow::ReadTemplates(void)      

{                                      // begin ReadTemplates

int aRc = 0;                           // all OK for now
int aDDCnt = 0;                        // DD counter

CviStr aTempList;                      // templates process


Print("Processing workflow templates.\n");


CviStr aTempDSNList;                   // temmplate DSN list

Variable *aLocalTemps =                // get local
   itsPropVars->GetVariable("CVIBLDWF_LOCAL_TEMPLATES");

if (aLocalTemps == NULL)               // if not set
    aLocalTemps =                      // try old style
       itsPropVars->GetVariable("LOCAL_TEMPLATES");

if (aLocalTemps != NULL)               // if valid

    AddList(aTempDSNList,              // add local override
            aLocalTemps->Value());     // from properties

AddList(aTempDSNList, itsTemplateDSNList);// add all other template DSNs


const char *aTempDSN =                 // parse
     aTempDSNList.Tok(",");            // template list

while (aRc == 0 && aTempDSN != NULL)   // for all template DSNs

{                                      // begin process DSN

    CviStr TempDD;
    aDDCnt ++;

    TempDD.Print("TEMP%04.4d",
                 aDDCnt);

    CviStr  TempDSN(aTempDSN);         // get in a form where we can use char *

    __dyn_t aDyn;                      // dynamic allocation

    dyninit(&aDyn);                    // clear structure
                                       
    aDyn.__ddname = TempDD;            // DD
    aDyn.__dsname = TempDSN;           // DSN
    aDyn.__status = __DISP_SHR;        // share
    aDyn.__normdisp = __DISP_KEEP;     // keep

    dynalloc(&aDyn);                   // allocate

    MemberList memberList;             // DSN member list
                                       
    TRACE("Reading templates from %s\n",
      (const char *) aTempDSN);

    Member *aMember = memberList.GetMembers(TempDD);

    if (memberList.Failed())
    {
        Error("Unable to read %s.\n",
              aTempDSN);

        aRc = 8;                       // failure
    }

    while (aRc == 0 &&                 
           aMember != NULL)            // for all members

    {                                  // begin read member
                                       
        CviStr aStr = 
            (const char *) *aMember;   // get into our string format

        if (AddList(aTempList,         // if we haven't processed a member
                    aStr))             // by this name yet

           aRc = BuildStep(aStr,       // build it
                           TempDD);    // DSN to use

        aMember = aMember->GetNext();  // next!

    };                                 // end read member

    aTempDSN = aTempDSNList.Tok(",");  // next!

};                                     // end process DSN

itsStepNames.Reset();                  // clear out step list memory
                                       
Print("\n");                           // skip a line


return(aRc);                           // return result

}                                      // end ReadTemplates

//****************************************************************************
//
// Method       : BuildWorkflow::DisplaySteps
//
// Description  : Displays all steps
//
//****************************************************************************
int BuildWorkflow::DisplaySteps(void)      

{                                      // begin DisplaySteps

int aRc = 0;                           // all OK for now
int aCnt = 0;                          // step counter


Print("\nGenerated Step List:\n");     

aCnt = WalkTree(itsGroups, 0, 2);      // walk ordered tree, display written
                                       
Print("\nTotal Steps: %d\n", aCnt);    // display total step count

WorkflowStep *aStep = itsSteps;        // for all steps
while (aStep != NULL)                  
{                                      
    if (aStep->Bypassed())             
        TRACE("%s was bypassed.\n",    
              (const char *) aStep->GetName());
    aStep = aStep->GetNext();          
};                                     
                                       
return(aRc);                           // return result

}                                      // end DisplaySteps

//****************************************************************************
//
// Method       : BuildWorkflow::GenStdProperties
//
// Description  : Displays all steps
//
//****************************************************************************
int BuildWorkflow::GenStdProperties(void)      

{                                      // begin GenStdProperties

int aRc = 0;                           // all OK for now


CviStr aDate(itsStDateStr);            // current date
CviStr aTime;                          // current time
CviStr aTimestamp;                     // timestamp
CviStr aUniqueNum;                     // unique number


aTime.Print("%8.8s",                   // get time, no fractional seconds
            (const char *) itsStTimeStr);

itsPropVars->Add("EMPTY_STR", "");     // add empty string

aTimestamp.Print("%s at %s",           // get timestamp
                 (const char *) aDate, 
                 (const char *) aTime);

itsPropVars->Add("CUR_DATE", aDate);   // add date

itsPropVars->Add("CUR_TIME", aTime);   // add time

itsPropVars->Add("CUR_TIMESTAMP",      // add timestamp
                 aTimestamp);

const char *aTok = aDate.Tok("/");     // get first token

if (aTok != NULL)                      // if OK

    itsPropVars->Add("CUR_MONTH",      // add month
                    aTok);

aTok = aDate.Tok("/");                 // get second token
                                       
if (aTok != NULL)                      // if OK

    itsPropVars->Add("CUR_MDAY",       // add day of month
                    aTok);

aTok = aDate.Tok("/");                 // get second token
                                       
if (aTok != NULL                    && // if OK
    strlen(aTok) > 2)                  // and year looks OK

{                                      // begin add CUR_YEAR

    itsPropVars->Add("CUR_YEAR",       // add CUR_YEAR
                     aTok);

    itsPropVars->Add("CUR_YEAR2",      // add CUR_YEAR
                     aTok+2);          

};                                     // end add CUR_YEAR

aTok = aTime.Tok(":");                 // get first token

if (aTok != NULL)                      // if OK

    itsPropVars->Add("CUR_HOUR",       // add hour
                    aTok);

aTok = aTime.Tok(":");                 // get second token
                                       
if (aTok != NULL)                      // if OK

    itsPropVars->Add("CUR_MINUTE",     // add minute
                    aTok);

aTok = aTime.Tok(":");                 // get second token
                                       
if (aTok != NULL)                      // if OK
    
    itsPropVars->Add("CUR_SECOND",     // add second
                    aTok);

aUniqueNum.Print("${CUR_YEAR}${CUR_MONTH}${CUR_MDAY}${CUR_HOUR}${CUR_MINUTE}${CUR_SECOND}%d",
                 itsAsid);

itsPropVars->ReplaceWithValues         // setup unique number
                (aUniqueNum);

itsPropVars->Add("UNIQUE_NUM",         // add
                aUniqueNum);           // unique number

aUniqueNum.Reset();                    // reset to reuse

aUniqueNum.Print("${CUR_YEAR}${CUR_MONTH}${CUR_MDAY}${CUR_HOUR}${CUR_MINUTE}${CUR_SECOND}",
                 itsAsid);

itsPropVars->ReplaceWithValues         // setup unique number
                (aUniqueNum,
                 false);

ToBase(aUniqueNum, 36);                // convert to base 36

itsPropVars->Add("BASE36_TIME",        // set base-36 timestamp
                 aUniqueNum);

TRACE("BASE36_TIME: %s\n", (const char *) aUniqueNum);


CviStr aSysplex("&SYSPLEX");           // replace sysplex
TRACE("SYSPLEX: %s\n", (const char *) aSysplex);

aSysplex.ReplaceSymbols();
itsPropVars->Add("_WORKFLOW-sysplexName", aSysplex);
itsPropVars->Add("_workflow-sysplexName", aSysplex);

CviStr aSystem("&SYSNAME");            // replace sysname
aSystem.ReplaceSymbols();
itsPropVars->Add("_WORKFLOW-systemName", aSystem);
itsPropVars->Add("_workflow-systemName", aSystem);


itsPropVars->Add("_workflow-workflowOwnerUpper", GetTaskCviPgm()->GetUser());
itsPropVars->Add("_WORKFLOW-WORKFLOWOWNERUPPER", GetTaskCviPgm()->GetUser());

char aDsName[45];
char aMember[9];
char aRecFm;
int aRecL;
int aBlksize;

#ifdef _SRC_
FILE *aFp = fopen("DD:STEPLIB", "r");

if (osddinfo("STEPLIB",
             aDsName,
             aMember,
             &aRecFm,
             &aRecL,
             &aBlksize) == 0)

{

    itsPropVars->Add("WORKFLOW_DSN",   // add workflow DSN
                    aDsName);

}

#endif

return(aRc);                           // return result

}                                      // end GenStdProperties

//****************************************************************************
//
// Method       : BuildWorkflow::GenerateWF
//
// Description  : Generate the workflow XML
//
//****************************************************************************
int BuildWorkflow::GenerateWF(const char *theOut,
                              WorkflowStep *theGroup)

{                                      // begin GenerateWF

CviStr aDD("WFBLDOUT");
CviStr aDSN;
CviStr aMem;
CviStr aName(theOut);

const char *aPtr = aName.Tok("//'(", 3);  // get DSN name

if (aPtr != NULL)                      // if OK

{                                      // begin have DSN

    aDSN.Set(aPtr);                    // set DSN name

    aPtr = aName.Tok(")'");            // get member name

    if (aPtr != NULL)                  // if good

        aMem.Set(aPtr);                // use member name

};                                     // end have DSN

__dyn_t aDyn;                          // dynamic allocation

dyninit(&aDyn);                        // clear structure
                                   
aDyn.__ddname = aDD;                   // DD
aDyn.__dsname = aDSN;                  // DSN
aDyn.__status = __DISP_SHR;            // share
aDyn.__normdisp = __DISP_KEEP;         // keep
aDyn.__misc_flags = __CLOSE;           // close when finished

int aFc = dynalloc(&aDyn);                       // allocate

CviStr aOut;

Trace("DYNALLOC %s(%s) for %s(%s) - %d\n",
      (const char *) aDD,
      (const char *) aMem,
      (const char *) aDSN,
      (const char *) aMem,
      aFc);

aOut.Print("DD:%s(%s)",
           (const char *) aDD,
           (const char *) aMem);

FILE *aOutFp = fopen(aOut, "w,recfm=*");

int aRc = 0;                           // return code

CviStr aWorkflowXML;                   // workflow XML


if (theGroup == NULL)                  // if no group

    theGroup = itsGroups;              // get main groups

Print("Generating workflow %s.\n", (const char *) theOut);

if (aOutFp != NULL)                    // if opened

{                                      // begin write data

    if (strlen(itsMasterXML) > 0)      // if we had master XML

    {                                  // begin read/process master template

        TRACE("Processing master XML\n");

        const char *aLinePtr;          // line pointer

        while ((aLinePtr = itsMasterXML.Tok("\n")) != NULL)

        {                              // begin process lines

            CviStr aLine(aLinePtr);    // line to proces

            aLine.Trim(' ');           // trim trailing spaces

            TRACE("Master Line: %s\n", (const char *) aLine);

            if (strstr(aLine, "<variable ") != NULL)

            {                          // begin grab variable name

                aWorkflowXML.Add(aLine); // copy line as-is
                aWorkflowXML.Add("\n");  // add newline

                char *aPtr = strstr(aLine, "name=\"");

                if (aPtr != NULL)      // if OK

                {                      // begin grab name

                    aPtr = strstr(aPtr, "\"") + 1;

                    char *aEnd = aPtr + 1;

                    while (*aEnd != 0 &&  // find ending quote
                           *aEnd != '\"')
                        aEnd ++;

                    *aEnd = 0;         // terminate

                    char *aScope = strstr(aEnd+1, "scope=\"");

                    if (aScope != NULL)

                    {

                        aScope = strstr(aScope, "\"") + 1;

                        aEnd = aScope + 1;

                        while (*aEnd != 0 &&  // find ending quote
                               *aEnd != '\"')
                            aEnd ++;

                        *aEnd = 0;         // terminate

                    }

                    TRACE("Variable found: %s\n", aPtr);

                    // Why do this? Because a workflow may have variables and the templates
                    // may reference them. If the templates reference them then we need to
                    // add instance- prefixes to them.
                    // In practice, we do not seem to be using any workflow variables.

                    if (aScope != NULL)
                    {
                        TRACE("Scope: %s\n", aScope);
                        CviStr aFullName;

                        aFullName.Print("%s-%s",
                                        aScope,
                                        aPtr);
                        itsWfVars.Add(aFullName, NULL, 1); // add, reject duplicate
                    }
                    else
                    {
                        itsWfVars.Add(aPtr, NULL, 1); // add, reject duplicate
                    }

                };                     // end grab name

            }                          // end grab variable name

            else                       // otherwise

            if (strstr(aLine,          // if steps should be replaced
                       "--STEPS--") != NULL)

            {                          // begin add steps

                CviStr aStepXML;       // step XML

                aPrev = NULL;          // reset previous

                WorkflowStep *aTranslates = NULL;

                if (theGroup->GetTarget() ==      // if this will be
                      TARGET_DEPLOYMENT)          // intended for deployment time

                    aTranslates = itsTranslates;  // perform translations, if available

                if (theGroup != NULL)

                   theGroup->GenGroup  // generate for group
                        (aStepXML,     // Top level is a 'root' group
                         &itsWfVars,   // and itself shouldn't be written so
                         itsPropVars,  // that's why we call GenGroup and not GenXML
                         itsMacros,
                         aTranslates);

                Variable *aPrompt = itsWfVars.GetFirst();

                while (aPrompt != NULL)   // for all property file variables

                {                      // begin add to master

                    if (aPrompt->FindSubVar("LABEL") != NULL &&
                        aPrompt->FindSubVar("IS_USED") != NULL)

                    {

                        CviStr aScope(aPrompt->GetVelocityName());     // get velocity name

                        const char *aScopeVal = aScope.Tok("${-", 2);
                        const char *aNameVal = aScope.Tok("}");

                        if (!strcmp(aScopeVal, "INFR"))
                            aScopeVal = "global";

                        if (aScopeVal != NULL &&
                            aNameVal != NULL)

                            aWorkflowXML.Print("<variable name=\"%s\" scope=\"%s\" visibility=\"private\">\n",
                                                 aNameVal,
                                                 aScopeVal);

                        else

                            aWorkflowXML.Print("<variable name=\"%s\" scope=\"instance\" visibility=\"private\">\n",
                                                 aPrompt->GetPropName());

                        aWorkflowXML.Print(" <label>%s</label>\n",
                                             (const char *) aPrompt->FindSubVar("LABEL")->Value());

                        aWorkflowXML.Print(" <abstract>%s</abstract>\n",
                                             (const char *) aPrompt->FindSubVar("ABSTRACT")->Value());


                        aWorkflowXML.Print(" <description>%s</description>\n",
                                             (const char *) aPrompt->FindSubVar("DESCRIPTION")->Value());

                        aWorkflowXML.Print(" <category>%s</category>\n",
                                             (const char *) aPrompt->FindSubVar("CATEGORY")->Value());

                        Variable *aTypeVar = aPrompt->FindSubVar("TYPE");

                        CviStr aAttr;    // attributes
                        CviStr aChoices; // choices

                        const char *aType = "string";
                        const char *aValidation = NULL;


                        int aMinLen = 1;
                        int aMaxLen = 255;

                        if (aTypeVar != NULL)

                        {

                            if (!strcmp("HLQ", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "DSNAME";
                            }
                            else
                            if (!strcmp("DSNAME", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "DSNAME";
                            }
                            else
                            if (!strcmp("VOLSER", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "VOLSER";
                            }
                            else
                            if (!strcmp("UNIT", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "";
                                aMaxLen = 8;
                            }
                            else
                            if (!strcmp("DATACLASS", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "";
                                aMaxLen = 8;
                            }
                            else
                            if (!strcmp("STORCLASS", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "";
                                aMaxLen = 8;
                            }
                            else
                            if (!strcmp("MGMTCLASS", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "";
                                aMaxLen = 8;
                            }
                            else
                            if (!strcmp("STRING", aTypeVar->Value()))
                            {
                                aType = "string";
                                aValidation = "";
                            }
                            else
                            if (!strcmp("INTEGER", aTypeVar->Value()))
                            {
                                aType = "integer";
                                aValidation = "";
                            }
                            else
                            if (!strcmp("CHOICE", aTypeVar->Value()))
                            {
                                aType = "string";
                                aAttr.Set("valueMustBeChoice=\"true\" multiLine=\"false\"");

                                Variable *aSubVar = aPrompt->GetSubVar();

                                while (aSubVar != NULL)

                                {

                                    if (!strcmp("CHOICE", aSubVar->GetName()))              

                                    {
                                        aChoices.Print("   <choice>%s</choice>\n",
                                                       (const char *) aSubVar->Value());
                                    }

                                    aSubVar = aSubVar->GetNext();

                                }

                            }

                        }

                        if (aValidation == NULL &&
                            aPrompt->FindSubVar("VALIDATION") != NULL)

                        {
                            aValidation = 
                                (const char *) aPrompt->FindSubVar("VALIDATION")->Value();

                        }

                        if (!strcmp(aType, "string") &&
                            strlen(aAttr) == 0)
                        {
                            aAttr.Set("valueMustBeChoice=\"false\" multiLine=\"false\"");
                        }

                        aWorkflowXML.Print(" <%s %s>\n",
                                             aType,
                                             (const char *) aAttr);

                        if (aValidation != NULL && strlen(aValidation) > 0)

                                aWorkflowXML.Print("   <validationType>%s</validationType>\n",
                                                     aValidation);

                        else
                        if (!strcmp(aType, "string"))
                        {

                            if (aPrompt->FindSubVar("MINLEN") != NULL)
                                aMinLen = atoi(aPrompt->FindSubVar("MINLEN")->Value());

                            if (aPrompt->FindSubVar("MAXLEN") != NULL)
                                aMaxLen = atoi(aPrompt->FindSubVar("MAXLEN")->Value());

//                            aWorkflowXML.Print("   <minLength>%d</minLength>\n",
//                                                 aMinLen);

                            aWorkflowXML.Print("   <maxLength>%d</maxLength>\n",
                                                 aMaxLen);
                        }
                        else
                        if (!strcmp(aType, "integer"))
                        {

                            if (aPrompt->FindSubVar("MINVAL") != NULL)
                                aWorkflowXML.Print("   <minValue>%d</minValue>\n",
                                                   atoi(aPrompt->FindSubVar("MINVAL")->Value()));

                            if (aPrompt->FindSubVar("MAXVAL") != NULL)
                                aWorkflowXML.Print("   <maxValue>%d</maxValue>\n",
                                                   atoi(aPrompt->FindSubVar("MAXVAL")->Value()));

                        }

                        if (strlen(aChoices) > 0)

                            aWorkflowXML.Add(aChoices);

                        if (aPrompt->FindSubVar("DEFAULT") != NULL)
                            aWorkflowXML.Print("   <default>%s</default>\n",
                                                 (const char *) aPrompt->FindSubVar("DEFAULT")->Value());

                        aWorkflowXML.Print(" </%s>\n", aType);

                        aWorkflowXML.Print(" </variable>\n");
                    
                    }

                    aPrompt = itsWfVars.GetNext();

                };

                Variable *aVar = itsPropVars->GetFirst();

                while (aVar != NULL)

                {

                    if (aVar->IsUsed(aWorkflowXML))

                    {

                        aWorkflowXML.Print("<variable name=\"%s\" scope=\"instance\" visibility=\"private\">\n",
                                             aVar->GetName());

                        aWorkflowXML.Print(" <label>%s</label>\n",
                                             aVar->GetName());

                        aWorkflowXML.Print(" <abstract>.</abstract>\n");

                        aWorkflowXML.Print(" <description>%s</description>\n",
                                             aVar->GetName());

                        aWorkflowXML.Print(" <category>General</category>\n");

                        aWorkflowXML.Print(" <string valueMustBeChoice=\"false\" multiLine=\"false\">\n");

                        aWorkflowXML.Print(" </string>\n");
                        aWorkflowXML.Print(" </variable>\n");

                    };

                    aVar = itsPropVars->GetNext(); // next!

                };                     // end add to master

                if (strlen(aStepXML) == 0)

                {                      // begin add a default step

                    WorkflowStep aDefault("ERROR");

                    CviStr aDefTemp;   // default template


                    Print("No workflow steps generated. Generating instruction step indicating failure.\n");

                    aDefTemp.Print("TYPE=INSTRUCTIONS\n");
                    aDefTemp.Print("TITLE=Error Generating Workflow\n");
                    aDefTemp.Print("AUTO=OFF\n");
                    aDefTemp.Print("--INSTRUCTIONS--\n");
                    aDefTemp.Print("Something went wrong and the workflow generator found no workflow steps to perform.\n");
                    aDefTemp.Print("Please check the workflow generation job for more details.\n");


                    aDefault.ParseData(aDefTemp, itsPropVars);

                    aDefault.GenXML(aStepXML, &itsWfVars,
                                    itsPropVars,
                                    itsMacros,
                                    itsTranslates);

                };                     // end add a default step

                aWorkflowXML.Add(aStepXML);

            }                          // end add steps

            else                       // otherwise

            {                          // begin write

                CviStr aNewLine(aLine);// new line to write

                itsPropVars->          // replace with values
                        ReplaceWithValues(aNewLine, false);

                aWorkflowXML.Add(aNewLine);
                aWorkflowXML.Add("\n");

            };                         // end write

        };                             // end process lines

    }                                  // end read/process master template

    else                               // otherwise

    {                                  // begin fail

        Error("MASTER template not found!\n");

        aRc = 8;                       // hard failure

    };                                 // end fail

    const char *aTok =                 // grab first
        aWorkflowXML.Tok("\n");      // line

    while (aTok != NULL)               // for all lines

    {                                  // begin write output line

        CviStr aLine(aTok);            // line to write

        aLine.Add("\n");               // add newline

        fprintf(aOutFp,                // write out the XML line
                "%s",
                (const char *) aLine);

        aTok = aWorkflowXML.Tok("\n"); // get next line

    };                                 // end write output line

    fclose(aOutFp);                    // close output DSN

}                                      // end write data

else                                   // otherwise
                                       
{                                      // begin fail
                                       
    Error("Could not open WORKFLOW DD\n");
                                       
    aRc = 8;                           // hard failure
                                       
};                                     // end fail


return(aRc);                           // return result

}                                      // end GenerateWF

//****************************************************************************
//
// Method       : BuildWorkflow::GenerateDepWFs
//
// Description  : Generate the workflow XMLs for deployments
//
//****************************************************************************
int BuildWorkflow::GenerateDepWFs()

{                                      // begin GenerateWF

int aRc = 0;                           // return code



WorkflowStep *aGroup = itsGroups->GetGrpOrderChild(); // get top group

while (aRc == 0 && aGroup != NULL)     // for all groups

{                                      // begin generate deployment WFs
                                       
    if (aGroup->GetTarget() == TARGET_DEPLOYMENT &&
        !aGroup->Bypassed())           

    {                                  // begin new workflow

        aGroup->SetWritten();          // we wrote it

        CviStr aWfName;                // WF name

        aWfName.Print("//'%s(%s)'",
                      (const char *) itsWorkflowDSN,
                      (const char *) aGroup->GetName());
                                       
        aRc = GenerateWF(aWfName,      // generate
                         aGroup);      

    }                                  // end new workflow

    aGroup = aGroup->GetOrderChild();  // get next in the order

}                                      // end generate deploymnet WFs


return(aRc);                           // return result

}                                      // end GenerateWF

//****************************************************************************
//
// Method       : BuildWorkflow::BuildStep
//
// Description  : Build a step
//
// Parameters   : 1) Member name to process
//
//****************************************************************************
int BuildWorkflow::BuildStep(const char *theMember,
                             const char *theDSN)

{

int aRc = 0;                           // OK so far

TRACE("Processing member %s\n",        // give a little
      (const char *) theMember);       // diagnostic information


CviStr aName;


if (strstr(theDSN, ".") == NULL)
    aName.Print("DD:%s(%s)",
                (const char *) theDSN,
                (const char *) theMember);
                
else
    aName.Print("//'%s(%s)'",
                (const char *) theDSN,
                (const char *) theMember);

FILE *aFp = fopen(aName, "r,samethread");

if (aFp != NULL)                       // if OK

{                                      // begin read

    char aFirstRec = true;             // first record?

    static int aCount = 0;             // DSN count

    static CviStr aTempData;           // template data - keep so we retain good buffer size

    aTempData.Truncate();              // truncate (keep allocated for largest template seen so far)

    aCount ++;                         // bump counter

    TRACE("Reading content for #%d: %s\n",
          aCount,
          (const char *) theMember);

    TRACE("Read\n");

    char aBuffer[8192];

    int aRecLen = fread(aBuffer,
                        1,
                        sizeof(aBuffer),
                        aFp);

    while (aRecLen > 0)                // for all data

    {                                  // begin read in data
                                       
        aTempData.Add(aBuffer, aRecLen); // append new data

        if (aFirstRec &&               // if first record and
            strlen(aTempData) !=       // must have
                (aRecLen))             // NULL characters

        {                              // begin reject
                                       
            TRACE("Rejecting %s: appears to be binary.\n",
                  (const char *) theMember);

            aRecLen = 0;               // no more data

            break;                     // break out of this while loop

        };                             // end reject

        if (aFirstRec &&               // if first record and
            strstr(aTempData, "<?xml ") != NULL && // XML
            strcmp(theMember, "MASTER"))  // and not MASTER

        {                              // begin reject
                                       
            TRACE("Rejecting %s: appears to be XML.\n",
                  (const char *) theMember);

            aRecLen = 0;               // no more data

            break;                     // break out of this while loop

        };                             // end reject

        //aTempData.Trim(' ');           // trim trailing space
        //aTempData += "\n";             // add newline

        aFirstRec = false;             // no longer first record

        aRecLen = fread(aBuffer,
                        1,
                        sizeof(aBuffer),
                        aFp);

    };                                 // end read in data

    if (!strcmp(theMember, "MASTER"))  // if master

        itsMasterXML.Set(aTempData);   // save master XML

    else                               // otherwise

    if (strstr(aTempData,              // if it seems plausible that it could
               "TYPE=") != NULL)       // be valid

        aRc = CreateStep(aTempData,    // create step
                         theMember);   // member name is default name

    else                               // not a template

        TRACE("Rejecting %s: does not appear to be a template.\n",
              (const char *) theMember);

    fclose(aFp);                       // close member

}                                      // end read

else                                   // otherwise

{                                      // begin error

    Print("Unable to open template dataset.\n");

};                                     // end error


return(aRc);                           // return code holder

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessDatasetList
//
// Description  : Process dataset list template
//
// Parameters   : 1) Dataset list workflow object
//
//****************************************************************************
int BuildWorkflow::ProcessDatasetList(WorkflowStep *theStep)

{

Dataset *aDataset = NULL;              // dataset

CviStr aContent(theStep->GetContent());// content
CviStr aLineStr;                       // line string
CviStr aHLQID;                         // HLQ ID

VariableList aHLQList;                 // HLQ List


int aRc = 0;                           // OK so far
int aInJCL = 0;                        // not in JCL



const char *aLine =                    // get first
    aContent.Tok("\n");                // line

while (aLine != NULL)                  // for all lines

{                                      // begin process line

    aLineStr.Set(aLine);               // set line string

    if (aInJCL &&                      // if we are inside JCL and
        (strstr(aLineStr, "DSN=") ==   // line has DSN= 
            (const char *) aLineStr || // at the start OR
          strstr(aLineStr, "HLQ=") ==  // line has HLQ= 
             (const char *) aLineStr || // at the start OR
         strlen(aLineStr) == 0))       // line is empty

        aInJCL = 0;                    // we aren't inside JCL anymore
                                       
    if (strlen(aLineStr) == 0)         // empty line?

        aHLQID.Reset();                // reset HLQ ID

    if ((*aLine == '/'              && // if part of JCL
         aDataset != NULL)          || // and we have a dataset
        aInJCL)                        // or already inside JCL

    {                                  // begin process JCL line

        aInJCL = 1;                    // incside JCL

        aLineStr.Replace("${DSN}",     // replace DSN
                      aDataset->GetDSN());

        aLineStr.Replace("${DSNID}",   // replace ID
                      aDataset->GetID());

        itsPropVars->                  // replace with other variables
             ReplaceWithValues(aLineStr);

        aDataset->AddJCL(aLineStr);    // add to JCL

        TRACE("Added JCL %s\n",        // trace
              (const char *) aLineStr);

    }                                  // end process JCL line

    else                               // otherwise

    {                                  // begin find name/value pair
             
        const char *aKeyPtr =          // get first
            aLineStr.Tok("=");         // token

        if (aKeyPtr != NULL)           // if key found

        {                              // begin process key

            CviStr aKey(aKeyPtr);      // save key
            CviStr aValue;             // value

            const char *aValuePtr =    // get rest of the
                aLineStr.Tok("");      // data

            if (aValuePtr != NULL)     // if valid

                aValue.Set(aValuePtr); // use it

            itsPropVars->ReplaceWithValues(aValue, false);

            if (!strcmp(aKey, "TAG"))  // if TAG

            {                          // begin process TAG

                if (aDataset != NULL)  // if dataset pointer OK

                    aDataset->Tag(aValue); // tag it

            }                          // end process TAG

            else                       // otherwise

            if (!strcmp(aKey, "ALLOCATE")) // if ALLOCATE

            {                          // begin process ALLOCATE

                if (!strcmp(aValue,"Y") &&
                    aDataset != NULL)  // if dataset pointer OK
                {
                    aDataset->MarkAllocate(); // mark to always allocate (no POP entry needed)
                    TRACE("Marking dataset %s as allocation needed.\n",
                          (const char *) aDataset->GetDSN());
                }


            }                          // end process TAG

            else                       // otherwise

            if (!strcmp(aKey, "HLQ"))  // if HLQ

            {                          // begin HLQ

                aValuePtr = aValue.Tok(",");

                if (aValuePtr != NULL)

                    aHLQID.Set(aValuePtr);

            }                          // end HLQ

            else                       // otherwise

            if (!strcmp(aKey, "VALUE") && // HLQ VALUE?
                strlen(aHLQID) > 0)  

            {                          // begin HLQ VALUE

                CviStr aHLQVal;        // HLQ Value

                aValuePtr = aValue.Tok(",");

                if (aValuePtr != NULL)

                {                      // begin process value

                    aHLQVal.Set(aValuePtr);

                    CviStr aParms(aValue.Tok(""));

                    aKeyPtr = aParms.Tok("=");
                    aValuePtr = aParms.Tok("");

                    if (aKeyPtr != NULL &&
                        aValuePtr != NULL &&
                        !strcmp(aKeyPtr, "COND"))

                    {              // begin conditional

                        CviStr aCond(aValuePtr);
                        CviStr aReason;

                        TRACE("HLQ %s COND: %s\n",
                                               (const char *) aHLQID,
                                               (const char *) aCond);

                        if (EvaluateExpression(aCond,
                                               theStep,
                                               itsPropVars,
                                               aReason) == 0)

                            aHLQVal.Reset(); // don't use if expression didn't match

                        if (strlen(aReason) > 0)

                        {          // begin evaluation error


                            GetTaskCviPgm()->Print("%s: Error processing population condition.\n%s\n",
                                                       (const char *) theStep->GetMember(),
                                                       (const char *) aReason);

                            SETRC(aRc,8);      // set failed code

                        };              // end evaluation error

                    };                  // end conditional

                    if (strlen(aHLQVal) > 0)

                    {                  // begin setup variable

                        TRACE("ADDING HLQ %s: %s\n",
                                             (const char *) aHLQID,
                                             (const char *) aHLQVal);

                        aHLQList.Add(aHLQID, aHLQVal);

                        aHLQID.Reset();

                    }                  // end setup variable

                };                     // end process value

            }                          // end HLQ VALUE

            else                       // otherwise

            if (!strcmp(aKey, "DSN"))  // if DSN

            {                          // begin process DSN

                CviStr aID;            // ID

                const char *aIDPtr =   // get
                    aValue.Tok(",");   // ID pointer

                if (aIDPtr != NULL)    // if set

                    aID.Set(aIDPtr);   // use it

                const char *aDSNPtr =  // get 
                    aValue.Tok(",");   // DSN

                if (aDSNPtr != NULL)   // if OK

                {                      // allocate object

                    CviStr aDSN(aDSNPtr);

                    aHLQList.ReplaceWithValues(aDSN, false);

                    aDataset = itsDatasets;             
                                       
                    while (aDataset != NULL &&          // check for
                           strcmp(aDataset->GetID(),    // duplicates
                                  aID))

                        aDataset = aDataset->GetNext();

                    if (aDataset == NULL) // if not found

                    {                  // begin add

                        aDataset = new Dataset(aID, aDSN, theStep->GetMember());

                        if (itsDatasets == NULL)
                            itsDatasets = aDataset;
                        else
                            itsDatasets->Add(aDataset);

                        itsPropVars->Add  // add dataset ID variable
                                   (aDataset->GetID(),
                                    aDataset->GetDSN());

                        TRACE("Variable %s created, value %s\n",
                              (const char *) aDataset->GetID(),
                              (const char *) aDataset->GetDSN());

                    }                  // end add 

                    else               // otherwise

                    {                  // begin duplicate

                        Error("Duplicate DSN ID %s found while processing template member %s.\n"
                              "The DSN ID was already defined by template member %s.",
                              (const char *) aID,
                              (const char *) theStep->GetMember(),
                              (const char *) aDataset->GetTemplateMember());

                        aDataset = NULL; // ignore it for processing

                        aRc = 8;       // error out

                    };                 // end duplicate

                };                     // end allocate object

            }                          // end process DSN

            else                       // other attribute

            if (aDataset != NULL)      // if OK

                aDataset->AddAttribute(aKey, aValue);

        };                             // end process key

    }                                  // end find name/value pair

    aLine = aContent.Tok("\n");        // get next line

};                                     // end process line


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::CreateStep
//
// Description  : Build a step
//
// Parameters   : 1) Raw step data
//
//****************************************************************************
int BuildWorkflow::CreateStep(CviStr &theData,
                              const char *theName)

{

int aRc = 0;                           // OK so far

// If I had to do this over again, I'd make a separate parsing method that would create
// a WorkflowStep object from the result (after seeing the "TYPE=" line). I may change
// this someday if I get a chance... - SRC


WorkflowStep *aStep =                  // allocate the step
    new WorkflowStep(theName);

aRc = aStep->ParseData(theData,        // parse
                       itsPropVars);   // data
                                       
TRACE("Step Name: #%s#\n", (const char *) aStep->GetName());
TRACE("Parse Code: #%d#\n", aRc);
TRACE("Target type: %d\n", aStep->GetTarget());
TRACE("Depends on : %s\n", (const char *) aStep->GetDeps());
TRACE("Type of Content: %s\n", (const char *) aStep->GetType());
TRACE("Title: %s\n", (const char *) aStep->GetTitle());
TRACE("Instructions: %s\n", (const char *) aStep->GetInstructions());
TRACE("Content: %s\n", (const char *) aStep->GetContent());

if (aRc == 0)                          // if OK

{                                      // begin check duplicates

    CviStr aCheckStr;                  // dupe check string


    aCheckStr.Print(",%s,",            // setup dupe check string
                    (const char *) aStep->GetName());

    if (strstr(itsStepNames,           // if duplicate
               aCheckStr) != NULL)     // step name

    {                                  // begin complain

        Print("DUPLICATE STEP DETECTED. STEP %s HAS ALREADY BEEN DEFINED.\n",
              (const char *) aStep->GetName());

        aRc = 8;                       // fail

    }                                  // end complain

    else                               // otherwise

        itsStepNames.Add(aCheckStr);   // add to list

};                                     // end check duplicates

if (aRc == 0)                          // if OK

{                                      // begin adjust counts

    itsTempCount ++;                   // assume it'll be good

    if (aStep->IsReal())               // is it a "real" step?

        itsRealCount ++;               // add to count

};                                     // end adjust count

if (aRc != 0)                          // if parse was bad

{                                      // begin ignore step

    delete aStep;                      // delete the step

    aStep = NULL;                      // prevent accidental reuse of freed memory

    if (aRc <= 4)                      // if just a warning

        aRc = 0;                       // let it slide - might not even be a template

}                                      // end ignore step

else                                   // otherwise

if (!strcmp(aStep->GetType(),          // if it is an
            "INCLUDE"))                // include

{                                      // begin process include

    aStep->SetNext(itsIncludes);       // set current head as tail of new item

    itsIncludes = aStep;               // new item is now the head of the list

}                                      // end process include

else

if (!strcmp(aStep->GetType(),          // if it is a
            "PROMPT"))                 // prompt template

{                                      // begin add as a prompt

    aStep->SetNext(itsPrompts);        // set current head as tail of new item

    itsPrompts = aStep;                // new item is now the head of the list

}                                      // end add as a prompt

else

if (!strcmp(aStep->GetType(),          // if it is a
            "TRANSLATE"))              // translate template

{                                      // begin add as a translate template

    aStep->SetNext(itsTranslates);     // set current head as tail of new item

    itsTranslates = aStep;             // new item is now the head of the list

}                                      // end add as a translate template

else

if (!strcmp(aStep->GetType(),          // if it is a
            "MACRO"))                  // macro

{                                      // begin process macro

    DPRINT("Adding macro %s\n", (const char *) aStep->GetName());

    aStep->SetNext(itsMacros);         // set current head as tail of new item

    itsMacros = aStep;                 // new item is now the head of the list

}                                      // end process macro

else

if (!strcmp(aStep->GetType(),          // if it is a
            "WFMACRO"))                // workflow macro

{                                      // begin process macro

    CviStr aRefCntVar;                 // reference counter variable name

    aRefCntVar.Print("%s_REF",         // reference counter variable name
                  (const char *) aStep->GetName());

    itsPropVars->Add(aRefCntVar,       // setup
                     "0",              // reference
                     3);               // counter

    aStep->SetNext(itsWfMacros);       // set current head as tail of new item

    itsWfMacros = aStep;               // new item is now the head of the list

}                                      // end process macro

else

if (!strcmp(aStep->GetType(),          // if it is a
            "DATASET_LIST"))           // dataset list template

{                                      // begin process dataset list template

    aStep->SetNext(itsDsnSteps);       // DSN steps

    itsDsnSteps = aStep;               // set new head of list

    //if (!aStep->Bypassed())            // if not bypassed
    //
    //   aRc = ProcessDatasetList(aStep); // process dataset list
    //
    //delete  aStep;                     // delete step
    //
    //aStep = NULL;                      // prevent accidental use of deleted object

}                                      // end process dataset list template

else

if (!strcmp(aStep->GetType(),          // if it is a
            "DATASET_POPULATE"))       // dataset populate template

{                                      // begin process dataset populate template

    TRACE("Adding %s to population step list\n",
          (const char *) aStep->GetName());
                                       
    aStep->SetNext(itsPopSteps);       // mark old head as our next

    itsPopSteps = aStep;               // we are head of the list

}                                      // end process dataset populate template

else

if (strcmp(aStep->GetType(), ""))      // if known step type

{                                      // begin add

    if (!strcmp(aStep->GetType(),      // if GROUP entry
                "GROUP"))

    {                                  // begin link on group list

        aStep->SetGrpNext(itsGroups);  // set old head as our next

        itsGroups = aStep;             // we are now the head of the list

    }                                  // end link on group list

    aStep->SetNext(itsSteps);          // set old head as our next
                                       
    itsSteps = aStep;                  // set new head of list

}                                      // end add

else                                   // otherwise

{                                      // begin invalid

    delete aStep;                      // remove step

};                                     // end invalid


return(aRc);                           // return code holder

}

//****************************************************************************
//
// Method       : BuildWorkflow::GetTracks
//
// Description  : Get track count for DSN, if exists
//
// Parameters   : 1) Variable prefix
//                2) DSN to locate
//
// Return       : # of tracks
//
//****************************************************************************
bool BuildWorkflow::GetTracks(const char *thePrefix,
                              const char *theDSN,
                              long &theTracks)

{

bool    aFound = false;                // assume it won't be found

theTracks = 0;                         // no tracks by default

CviStr aDSNPrefix;                     // DSN prefix


TRACE("Getting tracks for %s\n", theDSN);


Variable *aVar =                       // get set entry, if it exists
    itsPropVars->GetVariable(thePrefix);


if (aVar != NULL)                      // if found

{                                      // begin get track from set

    aVar = aVar->GetSubVar();          // get subvariable list

    while (aVar != NULL)               // for all variables

    {                                  // begin find DSN

        Variable *aDSNVar =            // locate
            aVar->FindSubVar("DSN");   // DSN

        if (aDSNVar != NULL         && // if found
            !strcmp(aDSNVar->Value(),  // and
                    theDSN))           // it is a match

        {                              // begin locate tracks

            aFound = true;             // was found on target list

            Variable *aTrackVar =      // get tracks
                aVar->FindSubVar("TRACKS");

            if (aTrackVar != NULL)     // if found

            {                          // begin use

                CviStr aTrackStr(aTrackVar->Value());

                theTracks = atol(aTrackVar->Value()); // get value

                if (strstr(aTrackStr,       // if contains
                           ".") != NULL)    // fractional portion

                    theTracks ++;           // round up

                TRACE("Found tracks %ld for %s\n",
                      theTracks,                 
                      theDSN);                 

                if (theTracks <= 0)    // if low

                   theTracks = 1;      // use minimum

                break;                 // break

            };                         // end use

        };                             // end locate tracks

        aVar = aVar->GetNext();        // next!

    };                                 // end find DSN

};                                     // end get track from set

if (aVar != NULL)                      // if it was found

    aVar = NULL;                       // don't search anymore


return(aFound);                        // return found status

}

//****************************************************************************
//
// Method       : BuildWorkflow::CreatePopArray
//
// Description  : Create an array variable of DSN data to be used by
//                #FOR_EACH calls and such
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::CreatePopArray(const char *theScope)

{

int aRc = 0;                           // OK so far
int aRowCnt = 0;                       // row counter

CviStr aScopeList;                     // scope list
CviStr aListName;                      // list name
CviStr aShortScope;                    // short scope


TRACE("Creating population array\n");

if (theScope == NULL)                  // if NULL scope

    aListName.Set("DSN_LIST");         // set default name

else                                   // otherwise

{

    aShortScope.Add(theScope,
                    strlen(theScope)-1);

    aListName.Print("%sDSN_LIST",      // setup to have scope
                    theScope);

}

if (theScope == NULL)                  // if no scope (first time through)

   itsPropVars->Add(aListName, "", 2); // setup property variable - replacing

TRACE("BUILDING %s\n", (const char *) aListName);

Variable *aDSVar =                     // get DSN LIST variable
    itsPropVars->GetVariable(aListName);

TRACE("%s Variable %p\n", (const char *) aListName, aDSVar);

Dataset *aDs = itsDatasets;            // start at head of dataset

while (aDs != NULL)                    // while we have a dataset

{                                      // begin process dataset

    const char *aID = aDs->GetID();    // get ID

    TRACE("DS %s %s Needed %d\n",
          aID,
          aDs->GetDSN(),
          aDs->IsNeeded());

    if (aDs->IsNeeded()            &&  // if needed and
        !aDs->IsListed()           &&  // not already listed
       ((theScope == NULL &&           // if no scope wanted
         strstr(aID, "-") == NULL) ||  // and no scope found
        (theScope != NULL &&           // or scope wanted
         strstr(aID, theScope) == aID)))// and scope matches

    {                                  // begin add to array

        aRowCnt ++;                    // add row count

        if (theScope != NULL)          // if no scope

            aDs->MarkListed();         // mark as listed

        Variable *aRowVar =            // create new
            new Variable("DSN_ROW");   // row variable

        CviStr aRowNum;                // row number string

        aRowNum.Print("%d", aRowCnt);  // convert to string

        aRowVar->AddSubVariable        // add column
            (new Variable("rownum",    // called rownum
                          aRowNum));

        if (theScope != NULL)

            aRowVar->AddSubVariable     // add column
                (new Variable("scope",  // called scope using scope
                              aShortScope));

        else                           // otherwise

            aRowVar->AddSubVariable    // add column
                (new Variable("scope", // called scope using scope
                              ""));    // that is empty

        aDs->CreateAttrCols(aRowVar,
                            itsPropVars); // generate columns

        aDSVar->AddSubVariable(aRowVar);

        Variable *aColVar = aRowVar->GetSubVar();

        while (aColVar != NULL)        // for all variables

        {                              // begin generate quick lookup names

            CviStr aName;              // set new name
                                       
            GenRowVarName(aName, aDSVar, aColVar, aRowCnt);

//            aName.Print("%s_%s_%d",    
//                        (const char *) aDSVar->GetPropName(),
//                        (const char *) aColVar->GetName(),
//                        aRowCnt);      
                                       
            TRACE("Adding %s %s \n",
                  (const char *) aName,
                  (const char *) aColVar->Value());

            itsPropVars->Add(aName,    // add "column" variable
                             aColVar->Value());
                                   
            aColVar = aColVar->GetNext();

        };                             // end generate quick lookup names

    }                                  // end add to array

    else                               // otherwise

    if (aDs->IsNeeded() &&             // if we have
        !aDs->IsListed()           &&  // not already listed
        theScope == NULL &&            // scope and DSN is needed
        strstr(aID, "-") != NULL)      


    {                                  // begin save scope to process later

        CviStr aScope;
        
        aScope.Print("%.*s",           // setup scope name
                     (int) (strstr(aID, "-") - aID) + 1,
                     aID);             

        AddList(aScopeList, aScope);   // add to list

    };                                 // end save scope to process later

    aDs = aDs->GetNext();              // next!

};                                     // end process dataset

const char *aScope = aScopeList.Tok(",");

while (aScope != NULL)                 // for all items

{                                      // begin process scope
                                       
    if (strlen(aScope) > 0)            // if valid

        CreatePopArray(aScope);        // use it

    aScope = aScopeList.Tok(",");      // next!

}                                      // end process scope


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::ParseTemplates
//
// Description  : Reparse templates
//
// Parameters   : 1) Head of list
//
//****************************************************************************
int BuildWorkflow::ParseTemplates(WorkflowStep *theList)
{

int aRc = 0;

while (aRc == 0 &&
       theList != NULL)

{
    aRc = theList->ReParseData(itsPropVars);
    theList = theList->GetNext();
}


return(aRc);

}


//****************************************************************************
//
// Method       : BuildWorkflow::ParseTemplates
//
// Description  : Reparse templates
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ParseTemplates()
{

int aRc = 0;

Print("Reparsing Templates\n");

if (aRc == 0)
    aRc = ParseTemplates(itsSteps);

if (aRc == 0)
    aRc = ParseTemplates(itsIncludes);

if (aRc == 0)
    aRc = ParseTemplates(itsMacros);

if (aRc == 0)
    aRc = ParseTemplates(itsWfMacros);

if (aRc == 0)
    aRc = ParseTemplates(itsPopSteps);

if (aRc == 0)
    aRc = ParseTemplates(itsDsnSteps);


return(aRc);

}


//****************************************************************************
//
// Method       : BuildWorkflow::ProcessDSN
//
// Description  : Process DSN templates
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ProcessDSN()

{

int aRc = 0;                           // OK so far


Print("Processing dataset definitions.\n");

WorkflowStep *aStep = itsDsnSteps;     // DSN steps

while (aRc == 0 &&
       aStep != NULL)                  // for all steps

{                                      // begin process step

    if (!aStep->Bypassed())            // if not bypassed

    {                                  // begin process DATASET_LIST

        aStep->ProcessStatements(itsPropVars,
                                 itsIncludes,
                                 itsWfMacros);

        aRc = ProcessDatasetList(aStep);// process it

    }                                  // emd process DATASET_LIST

    aStep = aStep->GetNext();          // next!

}                                      // end process step


return(aRc);                           // return

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessTranslates
//
// Description  : Process TRANSLATE templates
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ProcessTranslates()

{

int aRc = 0;                           // OK so far


Print("Processing translation definitions.\n");

WorkflowStep *aStep = itsTranslates;   // TRANSLATE steps

while (aRc == 0 &&
       aStep != NULL)                  // for all steps

{                                      // begin process step

    if (!aStep->Bypassed())            // if not bypassed

    {                                  // begin process DATASET_LIST

        aStep->ProcessStatements(itsPropVars,
                                 itsIncludes,
                                 itsWfMacros);

        aStep->ProcessIntMacros(itsPropVars);

        CviStr aContent(aStep->GetContent());

        itsPropVars->                  // replace with values
           ReplaceWithValues(aContent, // but do NOT escape
                             false);   // values...

        aStep->SetContent(aContent);   // set new content

    }                                  // emd process DATASET_LIST

    aStep = aStep->GetNext();          // next!

}                                      // end process step


return(aRc);                           // return

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessPopulate
//
// Description  : Process Populate templates
//                This could really use being broken out into more
//                methods...it is too long for my liking.
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ProcessPopulate(const char *thePrefix)

{

CviStr aLineStr;                       // line holder
CviStr aListName;                      // list name

int aRc = 0;                           // OK so far


if (aRc == 0)                          // if OK
                                       
    aRc = CreatePopArray();            // create preliminary population array for DSNs

Print("Processing dataset population definitions.\n");


if (thePrefix != NULL &&               // if we have a
    strlen(thePrefix) > 0)             // prefix

    aListName.Set(thePrefix);          // use prefix

aListName.Print("POP_LIST");           // setup name

itsPropVars->Add(aListName, "", 2);    // setup property variable - replacing

Variable *aPopVar =                    // get POP LIST variable
    itsPropVars->GetVariable(aListName);

TRACE("Creating list %s along the way\n",
      (const char *) aListName);

WorkflowStep *aStep = itsPopSteps;     // populate steps

while (aStep != NULL)                  // for all steps

{                                      // begin process step

    if (aStep->Bypassed())             // skip
    {                                  // bypassed
        aStep = aStep->GetNext();      // entries
        continue;
    }

    aStep->ProcessStatements(itsPropVars,
                             itsIncludes,
                             itsWfMacros);

    CviStr aContent(aStep->GetContent()); // get content


    CviStr aPopId;                     // population ID
    CviStr aFromDSN;                   // from DSN name
    CviStr aToDSN;                     // to DSN name
    CviStr aConditions;                // condition
    CviStr aHLQPrefix;                 // HLQ prefix
    CviStr aDynSuffix;                 // dynamic ID suffix
    CviStr aTags;                      // concatenation list
    CviStr aCond;                      // conditional

    const char *aLine =                // get first
        aContent.Tok("\n");            // line

    while (aLine != NULL)              // for all lines

    {                                  // begin process line

        aLineStr.Set(aLine);           // set line string

        const char *aKeyPtr =          // get first
            aLineStr.Tok("=");         // token

        if (aKeyPtr != NULL)           // if key found

        {                              // begin process key

            CviStr aKey(aKeyPtr);      // save key
            CviStr aValue;             // value

            const char *aValuePtr =    // get rest of the
                aLineStr.Tok("");      // data

            if (aValuePtr != NULL)     // if valid

                aValue.Set(aValuePtr); // use it

            if (!strcmp(aKey, "POPULATE") ||
                !strcmp(aKey, "DEFINE"))

            {                          // begin process POPULATE or DEFINE

                aToDSN.Reset();        // reset TO DSN
                aFromDSN.Reset();      // reset FROM DSN
                aPopId.Reset();        // reset population ID
                aTags.Reset();         // reset tags
                aConditions.Reset();   // reset conditions
                aCond.Reset();         // reset single condition

                aValuePtr = aValue.Tok(",");

                if (aValuePtr != NULL) // we have an ID

                {                      // begin ID

                    aPopId = thePrefix; // use prefix

                    aPopId.Add(aValuePtr);// set population ID

                    aValuePtr = aValue.Tok(",");

                    if (aValuePtr != NULL)

                    {                  // begin check for other parms

                        CviStr aParms(aValuePtr);

                        aKeyPtr = aParms.Tok("=");
                        aValuePtr = aParms.Tok("");


                        if (aKeyPtr != NULL &&
                            aValuePtr != NULL &&
                            !strcmp(aKeyPtr, "FROM"))

                        {              // begin FROM

                            aFromDSN = aValuePtr;  // set FROM DSN

                            itsPropVars->ReplaceWithValues(aFromDSN, false);

                            TRACE("FROM DSN %s is %s\n",
                                  (const char *) aValuePtr,
                                  (const char *) aFromDSN);

                        }              // end FROM

                    }                  // end check for other parms

                }                      // end ID

            }                          // end process POPULATE

            else                       // otherwise

            if (!strcmp(aKey, "TAG") && // if TAG
                strlen(aPopId) > 0)    // and we have a population ID set

            {                          // begin setup concat list

                TRACE("Adding tags %s\n",
                                       (const char *) aValue);

                AddList(aTags, aValue);

            }                          // end setup concat list

            else                       // otherwise

            if (!strcmp(aKey, "TO") && // if TO
                strlen(aPopId) > 0)    // and we have a population ID set

            {                          // begin TO DSN

                aCond.Reset();         // reset condition

                aValuePtr = aValue.Tok(",");

                if (aValuePtr != NULL) // we have a destination ID

                {                      // begin have TO ID

                    int aOK = 1;       // assume OK

                    aToDSN = aValuePtr;// save ID

                    itsPropVars->ReplaceWithValues(aToDSN, false);

                    aValuePtr = aValue.Tok(",");

                    if (aValuePtr != NULL)

                    {                  // begin check for other parms

                        CviStr aParms(aValuePtr);

                        aKeyPtr = aParms.Tok("=");
                        aValuePtr = aParms.Tok("");

                        if (aKeyPtr != NULL &&
                            aValuePtr != NULL &&
                            !strcmp(aKeyPtr, "COND"))

                        {              // begin conditional

                            CviStr aReason;

                            aCond.Set(aValuePtr);

                            aConditions.Print("\n%s", (const char *) aCond);

                            aOK = EvaluateExpression(aCond,
                                                     aStep,
                                                     itsPropVars,
                                                     aReason);

                            TRACE("FOUND POPULATION CONDITIONAL %s. Result %d, %s\n",
                                  (const char *) aCond,
                                  aOK,
                                  (const char *) aReason);

                            if (strlen(aReason) > 0)

                            {          // begin evaluation error


                                GetTaskCviPgm()->Print("%s: Error processing population condition.\n%s\n",
                                                           (const char *) aStep->GetMember(),
                                                           (const char *) aReason);

                                SETRC(aRc,8);      // set failed code

                            };         // end evaluation error

                        }              // end conditional

                        else           // otherwise

                        if (aKeyPtr != NULL &&
                            aValuePtr != NULL &&
                            !strcmp(aKeyPtr, "HLQPREFIX"))

                        {              // begin HLQ prefix

                            aHLQPrefix.Set(aValuePtr);

                            itsPropVars->ReplaceWithValues(aHLQPrefix, false);

                            TRACE("HLQ PREFIX: %s\n",
                                  aValuePtr);

                        }              // end HLQ prefix

                        if (aKeyPtr != NULL &&
                            aValuePtr != NULL &&
                            !strcmp(aKeyPtr, "DYNSUFFIX"))

                        {              // begin dynamic DSN suffix

                            aDynSuffix.Set(aValuePtr);

                            itsPropVars->ReplaceWithValues(aDynSuffix, false);

                            TRACE("DYNAMIC SUFFIX: %s\n",
                                  aValuePtr);

                        }              // end dynamic DSN suffix

                    }                  // end check for other parms

                    if (aOK)           // if all is OK

                    {                  // begin add population entry

                        bool aFromMacro = false;

                        TRACE("Locating dataset entry for %s\n",
                              (const char *) aToDSN);

                        Dataset *aDataset = itsDatasets;

                        while (aDataset != NULL &&
                               strcmp(aDataset->GetID(), aToDSN))
                            aDataset = aDataset->GetNext();

                        // Really should split this up to generate a new dataset entry for these type of
                        // TO entries...


                        // If it doesn't match an ID, maybe it is something else
                        if (aDataset ==  NULL)
                        {

                            CviStr aOldDSN(aToDSN);

                            ProcessInternMacros(aToDSN,
                                                aStep,
                                                itsPropVars);

                            if (!strcmp(aToDSN, aOldDSN))   // if no change

                            {                               // begin try to find previous pop entry

                                CviStr aToID;

                                if (thePrefix != NULL)
                                    aToID.Set(thePrefix);
                                aToID.Add(aOldDSN);

                                Variable *aVar = itsPropVars->GetVariable(aToID);

                                if (aVar != NULL)              // if found
                                    aToDSN.Set(aVar->Value()); // use it

                            }                               // end try to find previous pop entry
                            else
                                aFromMacro = true;          // came from a macro

                            if (!strcmp(aToDSN, aOldDSN))   // if no change

                                aToDSN.Reset();             // ignore it - it wasn't something else. Otherwise, use whatever it translated to be.

                        }

                        if (aDataset == NULL &&            // no dataset and
                            strlen(aToDSN) > 0 &&          // ToDSN has values
                            strstr(aToDSN, ".") != NULL)   // and chances are reasonable it is a DSN

                        {

                            int aCnt = 0;

                            CviStr aTempDSN(aToDSN);


                            TRACE("Finding HLQ for %s\n", (const char *) aToDSN);

                            const char *aDot = strstr(aTempDSN, ".");

                            TRACE("Dot: %p\n", aDot);

                            while (aDot != NULL)
                            {
                                if (strstr(aDot+1, ".") != NULL)

                                    aDot = strstr(aDot+1, ".");

                                else

                                    break;

                            }

                            if (aDot != NULL &&         // dot found and
                                aFromMacro)             // from a macro call (probably #TARGET_DSN variant)
                            {

                                CviStr anHLQ;
                                CviStr aPrefix("SMPHLQ");

                                if (strlen(aHLQPrefix) > 0)
                                    aPrefix.Set(aHLQPrefix);

                                anHLQ.Print("%.*s",
                                            (int) (aDot - (const char *) aTempDSN),
                                            (const char *) aTempDSN);

                                TRACE("HLQ %s\n", (const char *) anHLQ);

                                Variable *anHLQs = itsPropVars->GetVariable("${INTERN_HLQS}");

                                if (anHLQs == NULL)

                                {
                                    TRACE("Adding new variable for INTERN_HLQS\n");
                                    itsPropVars->Add("INTERN_HLQS");
                                    anHLQs = itsPropVars->GetVariable("${INTERN_HLQS}");
                                    TRACE("HLQS: %p\n", (void *) anHLQs);
                                }

                                aCnt = 0;

                                Variable *aSubVar = anHLQs->GetSubVar();

                                while (aSubVar != NULL &&
                                       (strstr(aSubVar->GetName(),
                                               aPrefix) == NULL ||
                                        strcmp(aSubVar->Value(),
                                               anHLQ)))
                                {
                                    aSubVar = aSubVar->GetNext();
                                    aCnt ++;
                                }

                                if (aSubVar == NULL)

                                {
                                    CviStr anHLQID;
                                    anHLQID.Print("%s%d",
                                                  (const char *) aPrefix,
                                                  aCnt+1);

                                    aSubVar = new Variable(anHLQID,
                                                           anHLQ);

                                    anHLQs->AddSubVariable(aSubVar);

                                    aSubVar->AddSubVariable(new Variable("name", anHLQID));
                                    aSubVar->AddSubVariable(new Variable("value", anHLQ));

                                    TRACE("New variable: %p\n", (void *) aSubVar);

                                }

                                if (aSubVar != NULL)
                                {
                                    CviStr aVar;
                                    aVar.Print("&%s.", (const char *) aSubVar->GetName());
                                    aToDSN.Replace(anHLQ, aVar);
                                }

                            }

                            TRACE("Adding dataset for %s\n", (const char *) aToDSN);

                            aDataset = itsDatasets;

                            aCnt = 0;
                            while (aDataset != NULL &&
                                   strcmp(aDataset->GetDSN(),
                                          aToDSN))
                            {
                                aDataset = aDataset->GetNext();
                                aCnt ++;
                            }

                            if (aDataset == NULL)

                            {

                                if (strlen(aDynSuffix) == 0)
                                    aDynSuffix.Set(aPopId);

                                CviStr aId;
                                aId.Print("DYNDD_%d_%s",
                                          aCnt+1,
                                          (const char *) aDynSuffix);

                                aDataset = new Dataset(aId,
                                                       aToDSN,
                                                       "INTERNAL");
                                
                                if (itsDatasets == NULL)

                                    itsDatasets = aDataset;

                                else

                                    itsDatasets->Add(aDataset);

                                aDataset->MarkNeeded();

                                aDataset = NULL; // do nothing more with it

                            };

                        }

                        else

                        if (aDataset == NULL)
                            aToDSN.Reset();     // ignore this one

                        if (aDataset != NULL || // if OK
                            strlen(aToDSN) > 0)

                        {              // begin add population record to dataset

                            CviStr aDsnName;


                            if (aDataset != NULL)

                                aDsnName.Set(aDataset->GetDSN());

                            else

                                aDsnName.Set(aToDSN);

                            if (thePrefix != NULL &&
                                strlen(thePrefix) > 0)

                                itsPropVars->TranslateJCLValues(aDsnName);  // Resolve DSN

                            CviStr aOrigFromDSN(aFromDSN);

                            if (aDataset != NULL &&
                                strlen(aFromDSN) == 0)

                            {          

                                TRACE("Marking %s as needed - no FROM value.\n",
                                      (const char *) aDataset->GetID());

                                aDataset->MarkAllocate(); // it needs to be allocated

                            }

                            ProcessInternMacros(aFromDSN,
                                                aStep,
                                                itsPropVars);
                                            
                            Dataset *aFromDs = itsDatasets;
                            while (aFromDs != NULL)
                            {                     
                                if (!strcmp(aFromDs->GetID(), aFromDSN))
                                {
                                    aFromDSN.Set(aFromDs->GetDSN());
                                    break;
                                }
                                aFromDs = aFromDs->GetNext();
                            }

                            TRACE("Post-processed FROM DSN: %s\n",
                                  (const char *) aFromDSN);

                            const char *aDSN = aFromDSN.Tok(",");

                            while (aDataset != NULL &&
                                   aDSN != NULL)

                            {          

                                TRACE("Adding FROM DSN %s to DSN %s %s\n",
                                      (const char *) aDSN,
                                      (const char *) aDataset->GetID(),
                                      (const char *) aDataset->GetDSN());

                                aDataset->AddPopulate(aPopId,
                                                      aDSN);

                                aDSN = aFromDSN.Tok(",");

                            }

                            if (itsPropVars->Add    // add POPID variable
                                   (aPopId,
                                    aDsnName,
                                    1) != 0)

                            {

                                Variable *aVar = itsPropVars->GetVariable(aPopId);

                                if (aVar != NULL)
                                    aVar = aVar->FindSubVar("TEMPLATE");

                                Error("Duplicate POPULATION ID %s found while processing template member %s.\n"
                                      "The POPULATION ID was already defined by template member %s.",
                                      (const char *) aPopId,
                                      (const char *) aStep->GetMember(),
                                      (const char *) aVar->Value());

                                SETRC(aRc,8);      // set failed code

                            }

                            else

                            {

                                TRACE("Variable %s created, value %s\n",
                                      (const char *) aPopId,
                                      (const char *) aDsnName);

                                Variable *aVar = itsPropVars->GetVariable(aPopId);

                                if (aVar != NULL)
                                    aVar->AddSubVariable(new Variable("TEMPLATE", aStep->GetMember()));

                                if (strlen(aFromDSN) > 0 ||
                                    strlen(aOrigFromDSN) == 0)

                                {

                                    Variable *aRow = new Variable("POP_ROW");

                                    aPopVar->AddSubVariable(aRow);

                                    aRow->AddSubVariable           // add 
                                        (new Variable("POPID",     // population ID
                                                      aPopId));

                                    aRow->AddSubVariable           // add 
                                        (new Variable("DSN",       // DSN
                                                      aDsnName));

                                    AddList(aTags, aPopId);        // add pop ID to tags

                                    aRow->AddSubVariable           // add concat list IDs
                                        (new Variable("TAGS",
                                                      aTags));           

                                }

                            }

                            aTags.Reset();
                            aPopId.Reset();
                            aToDSN.Reset();
                            aFromDSN.Reset();
                            aHLQPrefix.Reset();
                            aDynSuffix.Reset();

                        }              // end add population record to dataset

                    }                  // end add population entry

                }                      // end have TO ID

            }                          // end from DSN

        };                             // end process key

        aLine = aContent.Tok("\n");    // get next line

    };                                 // end process line

    aStep->SetBypass();                // bypass step from here on out - won't produce an actual WF step

    aStep = aStep->GetNext();          // next!

};                                     // end process step

TRACE("Finished reading population templates\n");


return(aRc);                           // return result

}                                      // end ProecssPopulate

//****************************************************************************
//
// Method       : BuildWorkflow::SizePopulate
//
// Description  : Size up population entries
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::SizePopulate()

{

int aRc = 0;                           // OK so far
int aAllocCnt = 0;                     // allocation count
int aSrcCnt = 0;                       // source count
int aTcbCnt = 0;                       // TCB count


Print("Computing dataset allocation sizes.\n");


Dataset *aDataset = itsDatasets;       // prepare to iterate

while (aDataset != NULL             && // for all datasets
       aRc == 0)                       // and all is well

{                                      // begin process dataset

    Populate *aPop =                   // populate entry
           aDataset->GetPopulate();    // get population entry

    if (aDataset->Allocate() &&        // if we want it
        aDataset->IsNeeded())

    {                                  // begin setup to process

        TRACE("Dataset %s, ID %s will be allocated\n",
              aDataset->GetDSN(),
              aDataset->GetID());

    
        TRACE("%s\n",                  // trace more
              aDataset->GetJCL());     // details

        unsigned long aTotSize = 0;    // clear size

        while (aRc == 0 &&
               aPop != NULL)           // for all population entries

        {                              // begin process population entry

            if (strlen(aPop->GetSourceDSN()) == 0)

            {                          // begin bypass processing

                aPop = aPop->GetNext();// next!

                continue;              // iterate now

            };                         // end bypass processing

            aSrcCnt ++;                // bump source count for trace

            long aTracks = 0;          // track count

            CviStr aSrcDSN(aPop->GetSourceDSN());

            if (GetTracks("TARGET",    // get track data
                          aSrcDSN,
                          aTracks))

            {                          // begin entry found

                if (aTracks > 0)       // if tracks are valid

                    aTotSize += aTracks;  // add to track count

                else                   // otherwise

                {                      // begin error

                    Error("Missing track data some dataset populating %s: %s\n",
                          (const char *) aDataset->GetDSN(),
                          (const char *) aSrcDSN);

                    aRc = 8;           // fail

                };                     // end error

            };                         // end entry found

            aPop = aPop->GetNext();    // next!

        };                             // end process population entry

        aDataset->SetReqSize(aTotSize, 'T'); // set required size

        if (aRc == 0)
            
        {

            aDataset->GetMemberCount(itsPropVars);

        }

//        aTcbCnt ++;                    // bump TCB count

        aAllocCnt ++;                  // bump allocation count

    }                                  // end setup to process

    else                               // otherwise

    {                                  // begin not allocating

        TRACE("Dataset %s, ID %s, NOT Needed: IsNeeded %d, Allocate %d\n",
              aDataset->GetDSN(),
              aDataset->GetID(),
              aDataset->IsNeeded(),
              aDataset->Allocate());


    };                                 // end not allocating

    aDataset = aDataset->GetNext();    // next!

    if (aRc != 0)                      // if we failed

        aDataset = NULL;               // we will be stopping...

    if (aDataset == NULL           ||  // if last one or
        aTcbCnt > COMPUTE_TCB_MAX)     // too many TCBs

    {                                  // begin wait on TCBs

        TRACE("...reading member count results\n");

        Dataset *aChkDataset =         // dataset to start
            itsDatasets;               // checking

        while (aChkDataset != NULL &&  // while OK
               (aDataset == NULL ||        // and end of list or
                aTcbCnt > COMPUTE_TCB_WM)) // not below watermark

        {                              // begin wait for tasks

#ifdef _SRC_
            if (aChkDataset->HasThread()) // has a thread?

            {                          // begin wait

                aTcbCnt --;            // decrement TCB count

                aChkDataset->WaitThread();         // wait on thread

                if (aChkDataset->GetRC() != 0)

                {
                    Error("Dataset member count thread for %s failed. %X\n",
                          (const char *) aChkDataset->GetDSN(),
                          aChkDataset->GetRC());

                    SETRC(aRc, 16);    // an ABEND happened...
                }
#endif

                const char *aSize =
                    aChkDataset->GenSize();        // compute size info

                if (aChkDataset->TotSize() > 0)    // if set

                {                                  // begin display info

                    TRACE("DSN %s TOTAL SIZE %d, TOTAL MEMBERS %d ALLOC %s\n",
                          aChkDataset->GetDSN(),
                          aChkDataset->TotSize(),
                          aChkDataset->TotMembers(),
                          aSize);

                    Populate *aPop =                   // populate entry
                           aChkDataset->GetPopulate();    // get population entry

                    while (aPop != NULL)        // trace pop

                    {                           // begin trace populate entries

                        if (strlen(aPop->GetSourceDSN()) > 0)

                           TRACE("%s has %d members.\n",
                                 (const char *) aPop->GetSourceDSN(),
                                 aPop->GetMemberCnt());

                        aPop = aPop->GetNext();

                    }                  // end trace populate entries

                    TRACE("\n");

                };                     // end displsay info
#ifdef _SRC_
            };                         // end wait
#endif

            aChkDataset = aChkDataset->GetNext();    // next!

        };                             // end wait for tasks

    };                                 // end wait on TCBs

};                                     // end process dataset

TRACE("Finished sizing population templates\n");

DPRINT("Will allocate %d datasets and populate from %d datasets.\n",
       aAllocCnt,
       aSrcCnt);


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessPrompts
//
// Description  : Process prompts
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ProcessPrompts()
{

int aRc = 0;                           // OK so far

WorkflowStep *aTemp = itsPrompts;      // get head of prompt list

while (aRc == 0 &&                     // while OK and
       aTemp != NULL)                  // for all templates

{                                      // begin process template

    Print("Processing prompt template %s\n",
          (const char *) aTemp->GetMember());

    CviStr aContent(aTemp->GetContent()); // grab content
    CviStr aBlock;                     // block


    const char *aLinePtr =             // grab first
        aContent.Tok("\n");            // line

    while (aRc == 0 &&                 // while OK
           aLinePtr != NULL)           // and we have a line

    {                                  // begin look for prompts

        CviStr aFullLine;              // full line

        while (*aLinePtr != '#' &&
               *(aLinePtr + strlen(aLinePtr) -1) == '\\')
        {
            aFullLine.Add(aLinePtr, strlen(aLinePtr)-1);

            aLinePtr = aContent.Tok("\n");

        }

        if (aLinePtr != NULL)

           aFullLine.Add(aLinePtr);

        aLinePtr = (const char *) aFullLine;

        if (*aLinePtr == '#')          // if comment

        {                              // begin do nothing

        }                              // end do nothing

        else                           // otherwise

        if (*aLinePtr == 0)            // if empty line

        {                              // begin process block

            aRc = CreatePrompt(aBlock); // create block

            aBlock.Reset();            // clear block

        }                              // end process block

        else                           // otherwise

        if (strstr(aLinePtr, "="))     // it has key/value pairs

        {                              // begin process pairs

            aBlock.Add(aLinePtr);      // add line to block
            aBlock.Add("\n");          // newline

        };                             // end process pairs
                                       
        aLinePtr = aContent.Tok("\n"); // get next line

    };                                 // end look for prompts

    if (aRc == 0 &&                    // if OK and
        strlen(aBlock) > 0)            // have another block to process

        aRc = CreatePrompt(aBlock);    // create block

    aTemp = aTemp->GetNext();          // next!

};                                     // end process template

itsWfVars.SortByNumber("SORT_ORDER");


return(aRc);                           // return status

}

//****************************************************************************
//
// Method       : BuildWorkflow::CreatePrompt
//
// Description  : Create a prompt from block of prompt data
//
// Parameters   : 1) Block of data
//
//****************************************************************************
int BuildWorkflow::CreatePrompt(CviStr &theBlock)
{

int aRc = 0;                           // OK so far

const char *aLinePtr = theBlock.Tok("\n"); // process each line

Variable *aVar = NULL;                 // property variable 
Variable *aWfVar = NULL;               // workflow variable


while (aRc == 0 &&                     // while OK and
       aLinePtr != NULL)               // for each line

{                                      // begin grab key/value pairs

    CviStr aLine(aLinePtr);            // grab for parsing

    CviStr aKey(aLine.Tok("="));       // grab key
    CviStr aValue(aLine.Tok(""));      // grab rest of it

    TRACE("PROMPT: %s %s\n", (const char *) aKey, (const char *) aValue);

    if (!strcmp(aKey, "PROMPT"))       // if this is the name

    {                                  // begin create variable

        CviStr aVarName(aValue);       // variable name

//        aVar = itsPropVars->GetVariable(aVarName);
//
//        if (aVar == NULL &&            // if not found and
//            itsPropVars->Add(aVarName, // variable has been
//                             "") == 0) // added successfully
//
//        {                              // begin setup variable info
//
//            aVar = itsPropVars->       // grab variable entry
//                    GetVariable(aVarName);
//
//            aValue.Reset();            // reset value
//
//            aValue.Print("${instance-%s}",   // add Velocity name
//                         (const char *) aVarName);
//
//            aVar->AddSubVariable(new Variable("VELOCITY", aValue));
//
//            aValue.Reset();                    // reset

            if (itsWfVars.GetVariable(aVarName) != NULL)

            {

                TRACE("...PROMPT already exists...\n");

                aRc = 4;
            }

            else

               itsWfVars.Add(aVarName);  // add to workflow variable list

            aWfVar = itsWfVars.GetVariable(aVarName);

            Variable *aVelScope = itsPropVars->GetVariable("VARSCOPE");

            Variable *aOrder = itsPropVars->GetVariable("ORDER");

            CviStr aFullScope;

            if (aVelScope == NULL)
                aFullScope.Set("instance-");
            else
                aFullScope.Print("%s-",
                                 (const char *) aVelScope->Value());

            if (aOrder != NULL)
                aFullScope.Print("%s_",
                                 (const char *) aOrder->Value());

            TRACE("Setting scope for %s to %s\n",
                   (const char *) aVarName,
                   (const char *) aFullScope);

            if (aWfVar != NULL)
                aWfVar->SetVelocityScope(aFullScope);

            if (aWfVar != NULL)
                TRACE("   Velocity name is %s\n",
                       (const char *) aWfVar->GetVelocityName());


//        }                              // end setup variable info
//
//        else                           // otherwise
//
//        {                              // begin duplicate...
//
//            aRc = 4;                   // warning
//
//            Print("ERROR: %s is a duplicate prompt name.\n",
//                  (const char *) aValue);
//
//        };                             // end duplicate...

    }                                  // end create variable

    else                               // otherwise

    {

        if (aVar != NULL)              // if no variable yet

            aVar->AddSubVariable(new Variable(aKey, aValue));

        if (aWfVar != NULL)            // if no variable yet

            aWfVar->AddSubVariable(new Variable(aKey, aValue));

    }

    aLinePtr = theBlock.Tok("\n");     // grab next line

};                                     // end grab key/value pairs


return(aRc);                           // return status

}

//****************************************************************************
//
// Method       : BuildWorkflow::RemoveMissingPrereqs
//
// Description  : Remove prequisite entries that have no template
//                Also takes care of setting up pre-reqs for groups with
//                optional steps.
//
// Parameters   : Nada
//
// Returns      : 0 - We'll always succeed
//
//****************************************************************************
int BuildWorkflow::RemoveMissingPrereqs()

{

int aRc = 0;                           // default return


CviStr  aStepList;                     // step list
CviStr  aGroupList;                    // group member

WorkflowStep *aStep = itsSteps;        // setup to process all steps

while (aStep != NULL)                  // for all steps

{                                      // begin process step
                                       
    if (!aStep->Bypassed()          && // if not bypassed
        !aStep->Optional())            // and it is not optional

        aStepList.Print(",%s,",        // add to list to keep dependency
                        (const char *) aStep->GetName());

    if (!strcmp(aStep->GetType(),      // if it is a group
                "GROUP")            && // and   
        aStep->HasOptional())          // it has optional children

        AddDeps(aGroupList,            // add to group list
                aStep->GetName());     // to replace group dep with required descendents

    aStep = aStep->GetNext();          // next!

};                                     // end process step

aStep = itsSteps;                      // prepare to run list again

while (aStep != NULL)                  // for all steps

{                                      // begin process step

    if (!aStep->Bypassed())            // and not bypassed

    {                                  // begin process step

        CviStr aDepList = aStep->GetPreReqs();// get dependencies

        const char *aDep = aDepList.Tok(",");   // get first dependency

        while (aDep != NULL)           // for all dependencies

        {                              // begin locate step

            CviStr aFind;              // what we should find


            aFind.Print(",%s,",        // setup to find this
                        (const char *) aDep);

            if (strstr(aStepList,      // if dependent step
                       aFind) == NULL) // was not found

            {                          // begin remove dependency

                TRACE("Removing %s as a prereq for step %s\n",
                      aDep,
                      (const char *) aStep->GetName());

                aStep->RemPreReqs(aDep);   // remove the dependency

                WorkflowStep *aPre = itsSteps;

                while (aPre != NULL     && // for all steps, locate prereq step
                       strcmp(aDep, aPre->GetName()))

                      aPre = aPre->GetNext(); 

                if (aPre != NULL)      // if found, we need its prereqs

                {                      // begin add prereqs

                    TRACE("...adding old prerequisites from %s to %s\n",
                          aDep,
                          (const char *) aStep->GetName());

                    aStep->AddPreReqs
                         (aPre->GetPreReqs());

                    WorkflowStep *aGrp = aPre->GetGrp();

                    while (aGrp != NULL)   // for all of the group

                    {                      // begin add prereqs

                        aStep->AddPreReqs(aGrp->GetPreReqs());

                        aGrp = aGrp->GetGrp(); // get its group

                    };                     // end add prereqs

                    aDepList = aStep->GetPreReqs();

                };                     // end add prereqs

            }                          // end remove dependency

            else                       // otherwise

            if (IsDep(aGroupList,      // if it is a group
                      aDep))           // dependency

            {                          // begin replace with steps

                TRACE("Replacing group dependency on %s\n",
                      (const char *) aDep);

                aStep->RemPreReqs(aDep); // remove dependency

                WorkflowStep *aGrp = itsGroups;

                while (aGrp != NULL &&
                       strcmp(aGrp->GetName(),
                              aDep))

                    aGrp = aGrp->GetGrpNext();

                if (aGrp != NULL)      // if found

                {

                    TRACE("Replacing group dependencies with %s\n",
                          (const char *) aGrp->GetReqDescendents());

                    aStep->AddPreReqs(aGrp->GetReqDescendents());

                    aDepList = aStep->GetPreReqs();

                };

            };                         // end replace with steps

            aDep = aDepList.Tok(",");  // get next dependency 

        };                             // end locate step

    };                                 // end process step

    aStep = aStep->GetNext();          // next!

};                                     // end process step

return(aRc);                           // return result

}                                      

//****************************************************************************
//
// Method       : BuildWorkflow::AdjustGroupDepends
//
// Description  : Adjust group dependency list - merge all member dependencies
//                into the group dependency
//
// Parameters   : Nada
//
// Returns      : 1 - Success
//                2 - Not all dependencies have been met
//
//****************************************************************************
int BuildWorkflow::AdjustGroupDepends()

{

int aRc = 0;                           // default return


WorkflowStep *aStep = itsSteps;        // grab first step

while (aStep != NULL)                  // for all steps
                                       
{                                      // begin process step
                                       
   WorkflowStep *aParent =             // get parent
       aStep->GetGrp();                
                                       
   while (aParent != NULL)             // for all parents
                                       
   {                                   // begin add dependency to parent
                                       
       TRACE("Adding dependencies %s to %s\n",
             (const char *) aStep->GetDeps(),
             (const char *) aParent->GetName());

       aParent->AddDepends(aStep->GetDeps());
                                       
       aParent = aParent->GetGrp();    // get next parent up
                                       
   };                                  // end add dependency to parent
                                       
   aStep = aStep->GetNext();           // next!
                                       
};                                     // end add dependencies to the group

//
// We must now make sure no parent steps have a dependency on their descendents
//

aStep = itsSteps;                      // grab first step

while (aStep != NULL)                  // for all steps
                                       
{                                      // begin process step
                                       
   WorkflowStep *aParent =             // get parent
       aStep->GetGrp();                
                                       
   while (aParent != NULL)             // for all parents
                                       
   {                                   // begin remove dependency from parent
                                       
       TRACE("Removing %s as a dependency for %s\n", 
             (const char *) aStep->GetName(),
             (const char *) aParent->GetName());

       aParent->RemDepends(aStep->GetName());

       aParent->AddDescendents(aStep->GetName());

       if (!aStep->Optional())         // if not optional

           aParent->AddReqDescendents(aStep->GetName()); // add to required list

       else                            // otherwise

           aParent->SetHasOptional();  // mark parent as having optional children
                                       
       aParent = aParent->GetGrp();    // get next parent up
                                       
   };                                  // end remove dependency from parent
                                       
   aStep = aStep->GetNext();           // next!
                                       
};                                     // end add dependencies to the group


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessStatements
//
// Description  : Process workflow statements
//
// Parameters   : 0 is good
//
//****************************************************************************
int BuildWorkflow::ProcessStatements()

{

int aRc = 0;                           // all is well


TRACE("TRACE PLACE 20\n");

WorkflowStep *aStep = itsSteps;        // start with head of step list

while (aRc == 0 &&                     // while OK and
       aStep != NULL)                  // for all steps

{                                      // begin process includes

    if (!aStep->Bypassed())            // if not bypassing this step

    {                                  // begin process content

        aStep->GenStdProperties(itsPropVars); // generate standard properties

        if (strlen(aStep->GetDynamic()) > 0)
        {

            aRc = aStep->ProcessDynamic(itsPropVars,
                                        itsIncludes,
                                        itsWfMacros);

            aStep->SetBypass();        // bypass this step

        }

        else

            aRc = aStep->ProcessStatements(itsPropVars,
                                           itsIncludes,
                                           itsWfMacros);

    };                                 // end process content

    aStep = aStep->GetNext();          // next!

};                                     // end process includes

TRACE("TRACE PLACE 21\n");

aStep = itsSteps;                      // locate all dynamic steps, adjust deps

while (aStep != NULL)                  // for all steps

{                                      // begin find base steps

    TRACE("TRACE PLACE 21.1\n");

    WorkflowStep *aBase =              // get base
        aStep->BaseStep();             // step

    WorkflowStep *aDepStep =           // locate base dependency in all steps
        itsSteps;

    TRACE("TRACE PLACE 21.2\n");

    while (aBase != NULL &&            // while we have a base and
           aDepStep != NULL)           // steps are found

    {                                  // begin update deps

        TRACE("TRACE PLACE 21.3: %p %p %p\n",
              (void *) aBase,
              (void *) aDepStep,
              (void *) aStep);

        aDepStep->AddToBaseDeps(aStep); // add to base dependencies

        TRACE("TRACE PLACE 21.4\n");

        aDepStep = aDepStep->GetNext();// next!

        TRACE("TRACE PLACE 21.5\n");

    };                                 // end update deps

    TRACE("TRACE PLACE 21.6\n");

    aStep = aStep->GetNext();          // next!

    TRACE("TRACE PLACE 21.7\n");

};                                     // end find base steps

TRACE("TRACE PLACE 22\n");


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::ProcessMacros
//
// Description  : Add Velocity macro code, if referenced
//
// Parameters   : None
//
//****************************************************************************
int BuildWorkflow::ProcessMacros()

{

int aRc = 0;                           // all is well

WorkflowStep *aStep = itsSteps;        // start with head of step list

while (aRc == 0 &&                     // while OK and
       aStep != NULL)                  // for all steps

{                                      // begin process step

    if (!aStep->Bypassed())            // if not bypassing this step

    {                                  // begin process content

        TRACE("Processing macros for %s\n",
              (const char *) aStep->GetMember());

        aStep->GenStdProperties(itsPropVars); // generate standard properties

//        aRc = aStep->AddMacros         // add macros, if needed
//                      (itsMacros);     // steps

        aStep->ProcessIntMacros(itsPropVars);

    };                                 // end process content

    aStep = aStep->GetNext();          // next!

};                                     // end process step


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::WalkTree
//
// Description  : Walk the tree
//
// Parameters   : 1) Parent node
//                2) Depth
//                3) Walk ordered?
//          
//
//****************************************************************************
int  BuildWorkflow::WalkTree(WorkflowStep *theParent,
                             int theDepth,
                             int theOrdered,
                             int theDeps)

{

int aCnt = 0;                          // no count yet

if (theParent == NULL)                 // if bad

    return(aCnt);                      // return now

const char *aName = theParent->GetName(); // get name
const char *aStatus = "";

if (theParent->Bypassed())             // if bypassed

    aStatus = "Bypassed ";             // mark as bypassed

if (*aName == 0)                       // if empty

    aName = "ROOT";                    // say it is the root

if (theOrdered                     &&  // if ordered
    (theOrdered == 1 || 
     theParent->IsWritten()) &&
    !theParent->Bypassed())

{

    Print("%*.*s%s%s\n", theDepth*2, theDepth*2, " ", aStatus, aName);

    CviStr aMembers(theParent->GetTemplates());

    if (strlen(aMembers) != 0)
        TRACE("%*.*s%s  The following members influence this step:\n",
              theDepth*2, theDepth*2, " ");

    const char *aMember = aMembers.Tok(",");
    while (aMember != NULL)
    {
        if (*aMember != 0)             // if not empty
           TRACE("%*.*s    %s\n",
                 theDepth*2, theDepth*2, " ", aMember);
        aMember = aMembers.Tok(",");
    }

    aCnt ++;                           // bump count

}
else
if (theDeps)                           // displaying dependencies?
{                                      // begin display deps
    CviStr aOrigDeps(theParent->GetDeps());
    CviStr aDeps;

    const char *aTok = aOrigDeps.Tok(",");
    while (aTok != NULL)               // get from searchable list to readable
    {
        if (strlen(aTok) > 0)
        {
            if (strlen(aDeps) == 0)
                aDeps.Set(aTok);
            else
                aDeps.Print(",%s", aTok);
        }
        aTok = aOrigDeps.Tok(",");
    }


    if (!strcmp(theParent->GetType(), "GROUP") ||   // all groups and leafs with dependencies
        strlen(aDeps) > 0)

        Print("%*.*s%s%s Reqs %s\n", theDepth*2, theDepth*2, " ", aStatus, aName, (const char *) aDeps);

}                                      // end display deps
else

    TRACE("%*.*s%s%s\n", theDepth*2, theDepth*2, " ", aStatus, aName);

WorkflowStep *aStep;                   // step
 
if (theOrdered)                        // if ordered
    
    aStep = theParent->GetGrpOrderChild(); // use ordered list

else                                   // otherwise

    aStep = theParent->GetGrpChild();  // use pre-sorted list

while (aStep != NULL)                  // for all steps

{                                      // begin display step

    if (aStep->Bypassed())             // if bypassed, don't add depth (collapsed group)

        aCnt += WalkTree(aStep, theDepth, theOrdered, theDeps);

    else

        aCnt += WalkTree(aStep, theDepth + 1, theOrdered, theDeps);

    if (theOrdered)                    // if ordered

        aStep = aStep->GetOrderChild();// get ordered child

    else                               // otherwise

        aStep = aStep->GetChild();     // get pre-sorted child

};                                     // end process step

return(aCnt);                          // return count

}

//****************************************************************************
//
// Method       : BuildWorkflow::WalkDeps
//
// Description  : Walk dependency list
//
// Parameters   : 1) Parent node
//          
//
//****************************************************************************
void BuildWorkflow::WalkDeps(WorkflowStep *theStep,
                             CviStr &theCircular,
                             int theLevel)

{

if (theStep == NULL)                   // if bad

    return;                            // return now

CviStr aDeps(theStep->GetDeps());      // get dependencies

if (theStep->GetGrp() != NULL)         // if part of a group

    RemDeps(aDeps, theStep->GetGrp()->GetDeps());

theStep->SetWritten();                 // set written state

const char *aTok = aDeps.Tok(",");     // get first dependency

while (strstr(theCircular,             // while no circular dep detected
              "*** Circular") == NULL && // and
       aTok != NULL)                   // for all deps

{                                      // begin display dependencies

    if (strlen(aTok) == 0)             // if nothing to it

    {                                  // begin skip

        aTok = aDeps.Tok(",");         // next

        continue;                      // iterate

    };                                 // end skip

    WorkflowStep *aDepStep =           // setup to search for the dependency
        itsSteps;     

    while (aDepStep != NULL &&         // find matching name
           strcmp(aTok, aDepStep->GetName()))

           aDepStep = aDepStep->GetNext();

    if (aDepStep != NULL)              // if OK

    {                                  // begin write

         theCircular.Print("%*.*s%s %s depends on %s\n",
                           3+theLevel*2,
                           3+theLevel*2,
                           " ",
                           (const char *) theStep->GetType(),
                           (const char *) theStep->GetName(),
                           (const char *) aDepStep->GetName(),
                           aTok);

        if (!aDepStep->IsWritten())    // if OK to write

        {                              // begin possibly write

             WalkDeps(aDepStep,        // walk dependency of immediate step
                      theCircular,      
                      theLevel+1);          

             if (strstr(theCircular, "*** Circular") == NULL &&
                 aDepStep->GetGrp() != NULL &&  // if part of a group
                 strlen(aDepStep->GetGrp()->GetName()) > 0)

             {

                 theCircular.Print("%*.*s%s %s depends on %s\n",
                                   3+theLevel*2,
                                   3+theLevel*2,
                                   " ",
                                   (const char *) theStep->GetType(),
                                   (const char *) theStep->GetName(),
                                   (const char *) aDepStep->GetGrp()->GetName(),
                                   aTok);

                 WalkDeps(aDepStep->GetGrp(),   // walk group
                          theCircular,
                          theLevel + 1);

             };


        }                              // end possibly write

        else                           // otherwise

        {                              // begin diagnose

            theCircular.Print("*** Circular dependency - %s refers to %s ***\n\n",
                              (const char *) theStep->GetName(),
                              (const char *) aDepStep->GetName());

            aDepStep->UnsetWritten();

//            Print("%s\n", (const char *) theCircular);

//            WalkDeps(aDepStep, theCircular);

        };                             // end diagnose

    }

//    else
//
//        Print("%*.*sMissing Dependency: %s\n",
//              3+theLevel*2,
//              3+theLevel*2,
//              " ",
//              aTok);

    aTok = aDeps.Tok(",");             // get next dependency

};                                     // end display dependencies

//if (!theStep->IsWritten())             // if this is the start of the loop
//
//{                                      // begin write more diagnostics
//
//    Print("%s\n", (const char *) theCircular);
//
//    theCircular.Reset();
//
//};                                     // end write more diagnostics

theStep->UnsetWritten();               // unset written

}

//****************************************************************************
//
// Method       : BuildWorkflow::AssignGroups
//
// Description  : Puts group members into their groups
//
// Returns      : 0 if all is well
//
//****************************************************************************
int BuildWorkflow::AssignGroups()

{

int aRc = 0;                           // return code


Print("Grouping steps.\n");

TRACE("Ready to assign groups\n");

CviStr aNullGrp("TYPE=GROUP\n");       // NULL group

WorkflowStep *aGroup =                 // allocate
    new WorkflowStep("");              // group step

aGroup->ParseData(aNullGrp);           // parse group data

aGroup->SetGrpNext(itsGroups);         // add to head of list

itsGroups = aGroup;                    // set head

WorkflowStep *aStep = itsSteps;        // start with first step

while (aStep != NULL)                  // for all steps

{                                      // begin process step

    aGroup = itsGroups;                // top of group list

    while (aGroup != NULL &&           // while group member is found
           strcmp(aGroup->GetName(),   // and group name
                  aStep->GetGroup()))  // does not match

           aGroup = aGroup->GetGrpNext(); // get next group

    if (aGroup != NULL)                // if found

    {                                  // begin add to group

        aStep->SetGrp(aGroup);         // save group member for child

        aStep->SetChild(aGroup->GetGrpChild());    // set group child

        aGroup->SetGrpChild(aStep);    // set new head

    }                                  // end add to group

    else                               // otherwise

    {                                  // begin abandon

        Warn("Member %s, step %s belongs to group %s but no GROUP template for %s found.\n",
              (const char *) aStep->GetMember(),
              (const char *) aStep->GetName(),
              (const char *) aStep->GetGroup(),
              (const char *) aStep->GetGroup());

    };                                 // end abandon

    aStep = aStep->GetNext();          // next!

};                                     // end process step

AdjustGroupDepends();                  // adjust group dependencies

CviStr aUnknownDeps                    // grab top
    (itsGroups->GetDeps());            // group (root) dependencies

if (strlen(aUnknownDeps) > 2)          // if we have junk on the root node

{                                      // begin display unknown prerequisites

    DPRINT("\nWARNING: The following dependencies were set but were not found.\n");

    const char *aTok = aUnknownDeps.Tok(",");

    while (aTok != NULL)               // for all tokens

    {                                  // begin display dependency

        if (strlen(aTok) > 0)          // if valid

            Print("   %s\n", aTok);    // display

        aTok = aUnknownDeps.Tok(",");  // get next

    };                                 // end display dependency

    Print("\n");                       // make it easier to read 

};                                     // end display unknown prerequisites

WalkTree(itsGroups);                   // walk the tree


return(aRc);                           // return result

}

//****************************************************************************
//
// Method       : BuildWorkflow::OrderSteps
//
// Description  : Orders all of the steps
//
// Parameters   : Parent step
//
// Returns      : 0 if all is well
//
//****************************************************************************
int BuildWorkflow::OrderSteps(WorkflowStep *theParent)

{

CviStr aDep;                           // dependency string

int aRc = 0;                           // OK so far
int aStuck = 0;                        // stuck flag


if (theParent == itsGroups)            // if first one
    Print("Determining step order.\n");

TRACE("Parent %s deps %s\n",
      (const char *) theParent->GetName(),
      (const char *) theParent->GetDeps());

while (!aStuck)                        // while not stuck

{                                      // begin try to order

    aStuck = 1;                        // we might get stuck

    WorkflowStep *aStep =              // get group
        theParent->GetGrpChild();      // children

    while (aStep != NULL)              // for all steps

    {                                  // begin process steps

        if (!aStep->Locked())          // if step not locked

        {                              // begin process unordered step

            WorkflowStep *aIns =               // get ordered child insert
                theParent->GetGrpOrderChild(); // point

            CviStr aDeps = aStep->GetDeps();

            TRACE("%s starting with deps %s\n",
                  (const char *) aStep->GetName(),
                  (const char *) aDeps);

            RemDeps(aDeps, theParent->GetDeps());

            TRACE("%s after parent removal deps %s\n",
                  (const char *) aStep->GetName(),
                  (const char *) aDeps);

            if (strlen(aDeps) == 0)    // if no dependencies yet

                aIns = NULL;           // then don't iterate - we can go anywhere

            while (aIns != NULL)       // while we have an insertion point

            {                          // begin find insertion point

                RemDeps(aDeps, aIns->GetName());
                RemDeps(aDeps, aIns->GetDeps());
                RemDeps(aDeps, aIns->GetDescendents());

                TRACE("Checking %s against %s - remaining deps\n",
                      (const char *) aStep->GetName(),
                      (const char *) aIns->GetName(),
                      (const char *) aDeps);

                if (strlen(aDeps) != 0)   // not first one?

                    aIns = aIns->GetOrderChild(); // get ordered child

                else                   // otherwise

                    break;             // we can stop now

            };                         // end find insertion point

            if (strlen(aDeps) == 0)    // if insertion point found

            {                          // begin place into list

                WorkflowStep *aSortIns = aIns;
                WorkflowStep *aSortPrev = aIns;

                if (aSortIns == NULL)  // if nothing set

                    aSortIns = theParent->GetGrpOrderChild();

                else                   // otherwise

                    aSortIns = aSortIns->GetOrderChild();

                TRACE("Checking sort for %s\n", (const char *) aStep->GetName());

                while (aSortIns != NULL &&
                       strcmp(aStep->GetName(),
                              aSortIns->GetName()) > 0)

                {

                    TRACE("Greater than %s\n", (const char *) aSortIns->GetName());

                    aSortPrev = aSortIns;
                    aSortIns = aSortIns->GetOrderChild(); 

                }

                if (aSortPrev != NULL) // if better spot found (alphabetical)

                    aIns = aSortPrev;  // use it

                aStuck = 0;            // we are not stuck

                if (aIns == NULL)      // if we will be a new item

                {                      // begin insert at head

                    TRACE("Adding to head\n");

                    aStep->SetOrderChild(theParent->GetGrpOrderChild());

                    theParent->SetGrpOrderChild(aStep);

                }                      // end insert at head

                else                   // otherwise

                {                      // begin insert after item

                    TRACE("Adding after %s\n", (const char *) aIns->GetName());

                    aStep->SetOrderChild(aIns->GetOrderChild());

                    aIns->SetOrderChild(aStep);  // we are the next item

                };                     // end insert after item

                aStep->SetLocked();    // step has been ordered

            };                         // end place into list

        };                             // end process unordered step

        aStep = aStep->GetChild();     // get next in child list
    
    };                                 // end process steps

    if (aStuck)                        // if we are stuck

    {                                  // begin indicate we are stuck

         aStep = theParent->GetGrpChild();

         while (aStep != NULL)         // for all steps

         {                             // begin locate failed steps

             if (!aStep->Locked())     // if step not locked

             {

                 CviStr aCircular;     // circular detection string

                 Print("Step %s cannot be placed in the list.",
                       (const char *) aStep->GetName());

                 aRc = 8;              // this is a failure

                 WalkDeps(aStep,       // walk dependencies
                          aCircular);

                 if (strlen(aCircular) > 0)

                     Print("%s\n", (const char *) aCircular);

             }

             aStep = aStep->GetChild();

         };                            // end locate failed steps

         if (aRc == 8)                 // if certified failure

         {                             // begin walk tree for deps

             WalkTree(theParent,       // walk to
                      2,               // display deps
                      0,
                      1);

             Print("*************************************\n\n");

         };                            // end walk tree for deps

    };                                 // end indicate we are stuck

};                                     // end try to order

WorkflowStep *aStep =                  // get group
    theParent->GetGrpOrderChild();     // children
                                       
while (aStep != NULL)                  // for all steps
                                       
{                                      // begin process steps

    WorkflowStep *aGrp = 
        aStep->GetGrpChild();

    if (aGrp != NULL)                  // if this one has children of its own

        OrderSteps(aStep);             // order its children, too

    aStep = aStep->GetOrderChild();    // next!

};                                     // end process steps

if (theParent == itsGroups)            // if root node

{                                      // begin process other stuff
                                       
    Print("Translating templates.\n");

    if (aRc == 0)                      // if OK

        aRc = ProcessMacros();         // process macros
    
    WorkflowStep *aStep = itsSteps;    // grab step list
    
    while (aRc == 0 &&
           aStep != NULL)              // for all steps
    
    {                                  // begin process dataset_alloc steps
    
        if (!strcmp(aStep->GetType(), "DATASET_ALLOC") &&
            !aStep->Bypassed())
    
        {
    
            CviStr aContent = aStep->GetContent();
    
    
            if (ProcessDatasetAlloc(aContent,    // generate allocation JCL
                                    itsDatasets,
                                    itsPropVars) == 0)
    
                aStep->SetBypass();    // bypass step if nothing was generated
    
            else                       // otherwise
    
                aStep->SetContent(aContent); // set new content
    
        }
    
        else

        if (!strcmp(aStep->GetType(), "DATASET_DELETE") &&
            !aStep->Bypassed())

        {

            CviStr aContent = aStep->GetContent();


            if (ProcessDatasetDelete(aContent,    // generate allocation JCL
                                    itsDatasets,
                                    itsPropVars) == 0)

                aStep->SetBypass();    // bypass step if nothing was generated

            else                       // otherwise

                aStep->SetContent(aContent); // set new content

        }

        else

        if (!strcmp(aStep->GetType(), "DATASET_COPY") &&
            !aStep->Bypassed())
    
        {
    
            CviStr aContent = aStep->GetContent();
    
            if (ProcessDatasetCompress(aContent,    // generate compress steps
                                       itsDatasets,
                                       itsPropVars) == 0)
    
                aStep->SetBypass();    // set to bypass step
    
            else
    
                ProcessDatasetCopy(aContent,  // generate copy steps
                                   itsDatasets,
                                   itsPropVars);

            aStep->SetContent(aContent);    // set new content
    
        }

        else

        if ((strstr(aStep->GetType(), "JCL") != NULL ||   // if JCL or
             strstr(aStep->GetType(), "SAVE") != NULL) && // SAE
            strlen(aStep->GetContent()) == 0)          // and no content found

        {                                  // begin no content!

            TRACE("JCL Step %s has no content - bypassing.\n",
                                   (const char *) itsName);

            aStep->SetBypass();        // bypass

        };                                 // end no content!

        aStep = aStep->GetNext();      // next!
                                       
    };                                 // end process dataset_alloc steps

//    Print("Ordered Step List:\n");
//    WalkTree(itsGroups, 0, 1);         // walk ordered tree

};                                     // end process other stuff


return(aRc);                           // return code holder

}

//****************************************************************************
//
// Method       : BuildWorkflow::~BuildWorkflow
//
// Description  : Destructor
//
//****************************************************************************
BuildWorkflow::~BuildWorkflow()        // destructor

{

if (itsAddTargets != NULL)             // if allocated

    delete itsAddTargets;              // remove 

itsAddTargets = NULL;                  // prevent reuse

//Print("CviStr_ReallocCount: %d\n", CviStr_ReallocCount);

}

//****************************************************************************
//
// Function     : main
//
// Parameters   : -opt value pairs
//
//****************************************************************************
int main(int theArgc, const char **theArgv)
{

setbuf(stdout, NULL);
setbuf(stderr, NULL);

CviPgm  *aPgm = NULL;                  // config program

aPgm = new BuildWorkflow               // build workflows
            (theArgc,
             theArgv);

aPgm->Trace("Checking trace - this statement causes a trace check to happen.\n");

if (aPgm->TraceEnabled())              // now check to see if the trace worked

   aTrcPgm = aPgm;                     // retain trace object if it did

int aRc = aPgm->Run();                 // run task

aPgm->Trace("MACRO CACHE HITS: %d  ANSWERS: %d\n",
            aMacroCache.itsHits,
            aMacroCache.itsAnswers);

ContentBlock *aProc = ContentBlock::itsTop;
while (aProc != NULL)
{
    aPgm->Trace("%s called %d times\n",
                aProc->GetType(),
                aProc->GetCount());
    aProc = aProc->GetNext();
};

delete     aPgm;                       // delete program

aTrcPgm = NULL;                        // clear trace program 


return(aRc);                           // terminate

}

