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

