/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//        Provides expression processing
//
//****************************************************************************
#ifndef EXPRESSION_INC                 // reinclude check      
#define EXPRESSION_INC                 
     
#include    "CviStr.h"                 // needs our string class
#include    "CviPgm.h"                 // needs cvipgm

//****************************************************************************
//
// Class        : Expression
//
// Description  : Evaluates expressions
//
//****************************************************************************
class   Stack;                         // will use simple stack mechanism

class   Expression

{

protected:

    CviPgm *itsPgm;                    // program object

    Stack *itsExpStack;                // expression list
    Stack *itsRPNStack;                // active RPN stack

    CviStr itsExpression;              // expression string
    CviStr itsRewritten;               // rewritten expression string (after tokens)

private:

    int Precedence(const char *theTok);  // get order of precedence for operator    
    int IsOperator(const char *theTok);  // detemrine if token is an operator    
    int GetOperator(const char *theTok); // get operator type
    int IsNumber(const char *theStr);    // is string a number?
    int MaskSpaces(char *theStr);        // mask spaces, as needed
    int UnmaskSpaces(char *theStr);      // unmask spaces, as needed

protected:

    virtual
        int ProcessRPN(const char *theTok);  // process token with RPN engine

    virtual
        int PadOperators();            // pad operators

    virtual
        int ReplaceVariable(const char **theTok);        // replace variable

public:

    int Process(CviStr &theResult);    // process expression

    Expression(const CviStr &);        // construct processor

};
  
#endif                                 // end reinclude check

