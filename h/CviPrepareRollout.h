//****************************************************************************
// DESCRIPTION
//         Utility to build a workflow from templates
// 
//****************************************************************************
                        
#ifndef CVIPREPAREJCL_INC              // reinclude check
#define CVIPREPAREJCL_INC                   

//****************************************************************************
// Includes

#include "CviStr.h"                    // string class

#include "CviPgm.h"                    // CviPgm based

//         
//****************************************************************************

//****************************************************************************
//
// Class        : PrepareRollout
//
// Description  : Prepare JCL for rollout
//
//****************************************************************************
class   PrepareRollout : public CviPgm

{

protected :                            // protected members

  CviStr itsJCL;                       // JCL DD

protected:                             // protected members
     
  int   Process(CviStr &);             // process member

  virtual int Init();                  // initialize program

  virtual int Work(void);              // do work

public    :                            // public stuff

  PrepareRollout(int, const char **);  // constructor

  virtual ~PrepareRollout();           // destructor

};

#endif                                 // reinclude check
