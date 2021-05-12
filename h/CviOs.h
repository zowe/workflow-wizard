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
//****************************************************************************

#ifndef CVIOS_INC                      // reinclude check
#define CVIOS_INC

#include    "CviDefs.h"                // need our defs
#include    "CviStr.h"                 // needs string

#include    <unistd.h>                 // need timeval

extern "OS" void *CviOsLoad(const char *);
extern "OS" void CviOsDelete(const char *);
extern "OS" void *CviAttach(void *, void *, void *);
extern "OS" void *CviAttachJs(void *, void *, void *);
extern "OS" void CviDetach(void *);
extern "OS" void CviWait(void *, struct timeval *theTimeout);
extern "OS" void CviPost(void *);
extern "OS" int  CviSleep(int, int *theEcb = NULL);
extern "OS" int  CviHSleep(int);
extern "OS" int64_t CviOsTime(void);
extern "OS" void CviOsSetCdeName(const char *);
extern "OS" uint64_t CviOsStckTime(uint64_t *theTime);

// Name token defines
#define  NAME_TASK                   1 // task
#define  NAME_HOME                   2 // home
#define  NAME_PRIMARY                3 // primary
#define  NAME_SYSTEM                 4 // system 

extern "OS" int  CviOsCreateNt(const char *, int64_t *,
                               int theLevel = NAME_SYSTEM);
                      
extern "OS" int  CviOsDeleteNt(const char *,
                               int theLevel = NAME_SYSTEM);

extern "OS" int  CviOsGetNt(const char *, int64_t *,
                            int theLevel = NAME_SYSTEM);

void CviGetMvsName(char *);

char CviOsIsDefined(const char *);     // is dataset defined?

char CviOsGetDDs(CviStr &);            // get list of DDs


/* Set key zero... */
static __inline void SETKEYZERO()
{
  __register(0) void* r0;                           /* used by macro */
  __register(1) void* r1;                           /* used by macro */
  __register(14) void* r14;                         /* used by macro */
  __register(15) unsigned int r15;                  /* used by macro */
  __asm
  {
    MODESET KEY=ZERO                    ; Into PSW key zero
  }
}

/* Set key non zero... */
static __inline void SETKEYNONZERO()
{
  __register(0) void* r0;                           /* used by macro */
  __register(1) void* r1;                           /* used by macro */
  __register(14) void* r14;                         /* used by macro */
  __register(15) unsigned int r15;                  /* used by macro */
  __asm
  {
    MODESET KEY=NZERO                   ; Into TCB key
  }
}

/* Compare and Swap... */
static __inline int CS(unsigned int uiCompare, unsigned int uiReplace,
                       void *pvTarget, void *pvNewValue)
{
  /*
  ** The CS macro will test the compare value with 4-bytes at the
  ** target address and if equal will return 0, i.e. swap was
  ** performed. If not equal the new value ptr area is updated with
  ** the non-equal target value and a return value of 1 is set.
  */
  __register(0) unsigned int r0;                    /* used by macro */
  __register(1) unsigned int r1;                    /* used by macro */
  __register(2) void* r2;                           /* used by macro */
  __register(14) void* r14;                         /* used by macro */
  __register(15) volatile unsigned int r15;         /* used by macro */
  /* Set registers... */
  r0 = uiCompare;                               /* set compare value */
  r1 = uiReplace;                                  /* set swap value */
  r2 = pvNewValue;             /* return non-equal target value here */
  r14 = pvTarget;                                 /* set target addr */
  __asm
  {
    CVICS                             
  }
  return(r15);                                      /* return result */
}

  
#endif                                 // reinclude check
