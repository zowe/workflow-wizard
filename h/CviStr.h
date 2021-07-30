//****************************************************************************
// DESCRIPTION
//         Provides a 'string' class.  This is not as efficient as normal
//         char's but is able to expand strings dynamically to prevent
//         memory corruption.  
//****************************************************************************

#ifndef CVISTR_INC                     // reinclude check
#define CVISTR_INC                  

#include <stdarg.h>                    // va_arg

//****************************************************************************
//
// Class        : CviStr
//
// Description  : Handles dynamic buffer
//
//****************************************************************************
class   CviStr 

{

protected :                            // protected members

  char          itsEye[8];             // eye catcher

  char         *itsData;               // data for buffer

  int           itsDataLen;            // length of data
  int           itsAllocLen;           // allocated length

  int           itsTokOff;             // token offset

protected :                            // protected methods

  int  ReplaceQandB(const char *,      // replace string X with string Y
                    const char *);

public    :                            // public stuff

  CviStr();                            // default constructor

  CviStr(const CviStr &);              // copy constructor
                                       
  CviStr(const char *,                 // response constructor
         int theLen = -1);             

  void Add(const char *,               // append to buffer
           int theLen = -1);           // can limit length

  void Trim(char);                     // trim trailing character

  void TrimWhitespace();               // trim whitespace

  void XMLEscape();                    // escape special characters for XML

  void VelocityEscape();               // escape entire string for Velocity
  void VelocityUnescape();             // remove escape sequence for Velocity

  void ReplaceSymbols(void);           // replace system symbols

  void ReplaceSymbol(const char *,     // replace
                     const char *);    // symbol

  void ReplaceSymbols(char **theNames,
                      char **theValues,
                      int theCount);

  int  Replace(int, const char *);     // replace character at X with string

  int  Replace(const char *,           // replace string X with string Y
               const char *,
               bool theQuoteAndBraceAware = false);

  int  Replace(const char *,           // replace string at address
               int,                    // and length with
               const char *);          // new string

  void Set(const char *);              // set buffer

  void Print(const char *, ...);       // 'sprintf' into buffer

  void VsnPrint(const char *, va_list);// print with variable parameters

  void Reset();                        // reset

  void Truncate();                     // truncate data

  void ResetTok(int theOffset = -1);   // reset tokenizer

  const char *Tok(const char *);       // tokenize

  const char *Tok(const char *,        // tokenize
                  int theSkip);        // token skip count

  const char *Parm(const char *,       // get parameter (tok w/quote awareness)
                   bool theStripQuotes = true);

  virtual ~CviStr();                   // destructor

  void Grow(int);                      // grow string by X size

  operator char *() const;             // convert to char *

  CviStr &operator=(const CviStr &);   // set data from another string

  CviStr &operator=(const char *);     // convert from const char *

  CviStr &operator+=(const char *);    // add const char *

  CviStr &operator+=(const char);      // add const char

  CviStr &operator-=(const char *);    // remove ve const char *

  int operator==(const CviStr &);      // compare 

  int operator!=(const CviStr &);      // compare 

  int operator==(const char *);        // compare 

  int operator!=(const char *);        // compare 

};

inline const char *strcpy(CviStr &theStr, const char *theSrc)
{
   theStr = theSrc;
   return((const char *) theStr);
}

inline const char *strcpy(CviStr &theStr, const CviStr &theSrc)
{
   theStr = theSrc;
   return((const char *) theStr);
}

inline const char *strcat(CviStr &theStr, const char *theSrc)
{
   theStr += theSrc;
   return((const char *) theStr);
}

inline const char *strcat(CviStr &theStr, const CviStr &theSrc)
{
   theStr += theSrc;
   return((const char *) theStr);
}

#endif                                 // reinclude check

