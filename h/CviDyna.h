/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Dynamic allocation
// 
//****************************************************************************

#ifndef __CVIDYNA_H
#define __CVIDYNA_H
/* Declare the "cvidyna" function */
extern "OS"
   int CVIDYNA(int *, const char *, int *);

#define CVIDYNA_CONCAT 1                    /* concatenation request */
#define CVIDYNA_DECONCAT 2                /* deconcatenation request */
#define CVIDYNA_ALLOC_SYSOUT 3          /* sysout allocation request */
#define CVIDYNA_UNALLOC_SYSOUT 4      /* sysout unallocation request */

#endif

