//****************************************************************************
// DESCRIPTION
//         Base class for all 'program' objects.
// 
//****************************************************************************

#ifndef CVIPGM_INC                     // reinclude check
#define CVIPGM_INC

#include    <stdio.h>                  // standard I/O
#include    <stdarg.h>                 // valist
#include    <time.h>                   // time

#include    "CviDefs.h"                // CVI defines
#include    "CviStr.h"                 // CVI string

class CviPgm

{
 
  protected                         :  // protected members

      char  itsEye[16];                // eye-catcher
      char  itsName[9];                // task name
      char  itsMvsName[9];             // MVS name
      char  itsJobName[9];             // our job name
      char  itsJobStep[9];             // our job step
      char  itsJobId[9];               // our job ID
      char  itsStDateStr[11];          // start date string
      char  itsStTimeStr[13];          // start time string
      char  itsTimeStr[13];            // current time string
      char  itsDateStr[11];            // current date string
      char  itsUser[9];                // user ID

      int   itsAsid;                   // our ASID
      int   itsTcb;                    // TCB address at creation

      int   itsArgc;                   // argument count
      const char **itsArgv;            // arguments

  private                           :  // private members

      FILE  *itsStdOut;                // standard output
      FILE  *itsStdErr;                // standard error
      FILE  *itsTrcOut;                // trace output

      char  itsTrcClosed;              // trace closed?
      char  itsMainForTask;            // are we the main pgm for this task?

      int   _itsTrcOff;                // trace off master flag

      int   itsCvtOff;                 // current conversion offset

      char  itsCvtBuf[256];            // temp conversion buffer

  private                           :  // private methods

      void  GetMsgV(CviStr &,          // get message 
                    int theMsg,        // string      
                    va_list);          

      void  SetJobInfo();              // set job info

  protected                         :  // protected methods

      void  ToggleTrace(char);         // toggle trace on/off

      void  DumpTrace();               // dump in-memory trace

      void  Beautify(CviStr &,         // 'beautify' message
                     int theLen = 132);

      virtual int Init();              // initialize program

      virtual int Work();              // do work

      virtual int PrintOutput          // actually print output 
          (const char *);

      void Replace(char *,             // replace
                   const char *,       // token with
                   const char *,       // value
                   int theMaxLen = -1);// maximum length of new string

  public                            :

      CviPgm(const char *theName,      // standard constructor 
             int theArgc,              
             const char **theArgv);

      CviPgm(const char *theName);     // standard constructor 

      const char *GetStartDate();      // get start date

      const char *GetStartTime();      // get start time

      const char *GetDate();           // get current date

      const char *GetTime();           // get current time

      const char *GetName();           // get task name

      int   TraceEnabled();            // is trace enabled?

      void Print(const char *, ...);   // print output

      void Warn(const char *, ...);    // print warning output

      void Error(const char *, ...);   // print error output

      void DevPrint(const char *, ...);// print to dev output

      void  PrintLines(const char *);  // print lines out


      const char *GetMsg(CviStr &,     // get numbered
                         int theMsg,   // message
                         ...);         // 

      const char *GetReasonText        // get text for rc/reason
                    (int, int);

      const char *ToDecStr(int);       // convert to decimal string
      const char *ToHexStr(int);       // convert to hex string

      const char *GetJobName();        // get job name

      const char *GetUser();           // get user ID

      int Trace(const char *, ...);    // trace output

      int Dump(const char *,           // dump - title
               const char *,           // block
               int);                   // length

      int PrintDump(const char *,      // dump - title
                    const char *,      // block
                    int,               // length
                    char theError = FALSE); // not an error by default

      int Run();                       // run program

      virtual void ParseFailure();     // handle XML parse failure

      virtual ~CviPgm();               // destructor

};

extern CviPgm *aTaskCviPgm;            // CVIPGM for task

static inline CviPgm *GetTaskCviPgm() { return(aTaskCviPgm); }; 


#endif                                 // end reinclude check
