//****************************************************************************
// DESCRIPTION
//         Makes z/OS calls
// 
//****************************************************************************

#ifndef CVIOS_INC                      // reinclude check
#define CVIOS_INC

#include    "CviDefs.h"                // need our defs
#include    "CviStr.h"                 // needs string

#include    <unistd.h>                 // need timeval

extern "OS" void *CviAttach(void *, void *, void *);
extern "OS" void *CviAttachJs(void *, void *, void *);
extern "OS" void CviDetach(void *);
extern "OS" void CviWait(void *, struct timeval *theTimeout);
extern "OS" void CviPost(void *);
extern "OS" int  CviHSleep(int);

// Name token defines
#define  NAME_TASK                   1 // task
#define  NAME_HOME                   2 // home
#define  NAME_PRIMARY                3 // primary
#define  NAME_SYSTEM                 4 // system 

void CviGetMvsName(char *);

char CviOsIsDefined(const char *);     // is dataset defined?

char CviOsGetDDs(CviStr &);            // get list of DDs

#endif                                 // reinclude check
