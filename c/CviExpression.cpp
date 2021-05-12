/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Evaluates exprssions
// 
//****************************************************************************

#include <stdio.h>                     // stdio
#include <string.h>                    // strings
#include <stdlib.h>                    // stdlib
#include <ctype.h>                     // types
#include <math.h>                      // math

#include "CviExpression.h"             // include our own header
#include "CviStr.h"                    // strings
                                                      
//****************************************************************************
// 
// Simple stack operations
// 
//****************************************************************************
                                                      
class Stack                            // stack class
{
public:   
   CviStr itsValue;                    // value of item
   Stack *itsNext;                     // next on the stack

   Stack(const char *theVal)           // constructor
    : itsValue(theVal)
   {
      itsNext = NULL;                  // nothing is next
   };
};

static void Push(Stack **theStack,     // push item
                 const char *theVal)   // onto the stack

{                                      // begin Push
Stack *aStack = new Stack(theVal);     // new item
aStack->itsNext = *theStack;           // set next
*theStack = aStack;                    // set new head
}                                      // end Push

static void Push(Stack **theStack,     // push numerical
          long theVal)                 // item
{                                      // begin Push
char aValue[64];                       // temp conversion string
sprintf(aValue, "%ld", theVal);        // convert to string
Push(theStack, aValue);                // push string
}                                      // end Push

static int Pop(Stack **theStack)       // pop item

{                                      // begin Pop

int rc = 0;                            // return code

if (*theStack != NULL)                 // if we have item to pop

{                                      // begin pop item

   rc = 1;                             // we are OK

   Stack *aNext = (*theStack)->itsNext;// grab next in stack

   delete *theStack;                   // delete old item

   *theStack = aNext;                  // new head

};                                     // end pop item

return(rc);                            // return result

};                                     // end Pop

static int Pop(Stack **theStack,       // pop item
               CviStr &theVal)         // from stack

{                                      // begin Pop

int rc = 0;                            // return code

if (*theStack != NULL)                 // if we have item to pop

{                                      // begin pop item

   rc = 1;                             // we are OK

   theVal = (*theStack)->itsValue;     // set value for return

   Stack *aNext = (*theStack)->itsNext;// grab next in stack

   delete *theStack;                   // delete old item

   *theStack = aNext;                  // new head

};                                     // end pop item

return(rc);                            // return result

};                                     // end Pop


//****************************************************************************
// List of Operators
//****************************************************************************
struct Operator                        // operator entry
{                                      // begin Operator

   const char *itsName;                // name
   int   itsOrder;                     // order
   int   itsType;                      // type

};                                     // end Operator

#define TYPE_NOOP      0
#define TYPE_POWER      1
#define TYPE_MULTIPLY   2
#define TYPE_DIVIDE     3
#define TYPE_ADD        4
#define TYPE_SUBTRACT   5
#define TYPE_EQUAL      6
#define TYPE_NONEQUAL   7
#define TYPE_GREATER    8
#define TYPE_LESS       9
#define TYPE_GREATER_EQ 10
#define TYPE_LESS_EQ    11
#define TYPE_OR         12
#define TYPE_AND        13

static struct Operator aOpList[] =
{
   { "^", 5, TYPE_POWER },
   { "*", 10, TYPE_MULTIPLY },
   { "/", 10, TYPE_DIVIDE },
   { "+", 11, TYPE_ADD },
   { "-", 11, TYPE_SUBTRACT }, 
   { "=", 50, TYPE_EQUAL },
   { "EQ", 50, TYPE_EQUAL },
   { "NE", 50, TYPE_NONEQUAL },
   { ">", 50, TYPE_GREATER },
   { "GT", 50, TYPE_GREATER },
   { "<", 50, TYPE_LESS },
   { "LT", 50, TYPE_LESS },
   { "GE", 50, TYPE_GREATER_EQ },
   { "LE", 50, TYPE_LESS_EQ },
   { "OR", 60, TYPE_OR },
   { "|", 60, TYPE_OR },
   { "&", 61, TYPE_AND },
   { "AND", 61, TYPE_AND },
   { NULL, -1, TYPE_NOOP }
};

//****************************************************************************
//
// Method       : Expression::Precedence
//
// Description  : Get precedence value
//
// Parameters   : 1) String to check
//
// Returns      : Precedence order value
//
//****************************************************************************
int Expression::Precedence(const char *theTok)

{

Operator *anOp = &aOpList[0];          // grab operator list

while (anOp->itsName != NULL &&        // while not end of list and
       strcmp(anOp->itsName, theTok))  // strings don't match
   anOp ++;                            // go to next entry

return(anOp->itsOrder);                // return order (returns NULL entry's order if not found)

};

//****************************************************************************
//
// Method       : Expression::IsOperator
//
// Description  : Is token an operator?
//
// Parameters   : 1) String to check
//
// Returns      : 0/1
//
//****************************************************************************
int Expression::IsOperator(const char *theTok)
{

int rc = 0;

Operator *anOp = &aOpList[0];          // grab operator list

while (anOp->itsName != NULL &&        // while not end of list and
       strcmp(anOp->itsName, theTok))  // names don't match
   anOp++;                             // go to next operator

if (anOp->itsName != NULL)             // if not end of list
    rc = 1;                            // then it is an operator

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : Expression::GetOperator
//
// Description  : Is token an operator?
//
// Parameters   : 1) String to check
//
// Returns      : Single-char operator
//
//****************************************************************************
int Expression::GetOperator(const char *theTok)
{

int rc = 0;

Operator *anOp = &aOpList[0];          // grab operator list

while (anOp->itsName != NULL &&        // while not end of list and
       strcmp(anOp->itsName, theTok))  // names don't match
   anOp++;                             // go to next operator

if (anOp != NULL)

    rc = anOp->itsType;

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : Expression::PadOperators
//
// Description  : Pad operators in expression for easier parsing
//
// Returns      : Precedence order value
//
//****************************************************************************
int Expression::PadOperators()
{

int rc = 0;

Operator *anOp = &aOpList[0];          // grab operator list

itsExpression.Replace("(", " ( ", true);
itsExpression.Replace(")", " ) ", true);

// To avoid inserting padding in double-character non-alpha operators, replace them with
// alpha equivalents. 
itsExpression.Replace("==", " EQ ", true);
itsExpression.Replace("<=", " LE ", true);
itsExpression.Replace(">=", " GE ", true);
itsExpression.Replace("||", " OR ", true);
itsExpression.Replace("&&", " AND ", true);
itsExpression.Replace("!=", " NE ", true);
itsExpression.Replace("^=", " NE ", true);

while (anOp->itsName != NULL)          // for all items in the table

{                                      // begin search/pad as needed

    if (!isalpha(*(anOp->itsName)))    // can only pad non-alphanumeric operators

    {                                  // begin pad

        CviStr aPad;                   // pad

        aPad.Print(" %s ",             // setup pad
                   anOp->itsName);

        itsExpression.Replace(anOp->itsName, aPad, true);

    }                                  // end pad

    anOp++;                            // next operator

};                                     // end search/pad as needed

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : Expression::IsNumber
//
// Description  : Is string a number?
//
// Parameters   : 1) String to check
//
// Returns      : 0/1
//
//****************************************************************************
int Expression::IsNumber(const char *theStr)

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
// Method       : Expression::MaskSpaces
//
// Description  : Mask spaces so tokenizing is easier
//
// Parameters   : 1) String to operate upon
//
// Returns      : 0 if OK
//
//****************************************************************************
int Expression::MaskSpaces(char *theStr)

{

int rc = 0;                            // result

char *aPtr = strstr(theStr, "\"");     // find quotation mark

while (aPtr != NULL)                   // for all quoted strings

{                                      // begin mask spaces

    aPtr ++;                           // bump past initial quote

    while (*aPtr != 0 &&               // while not end of string
           *aPtr != '"')               // and no ending quote found

    {                                  // begin check for spaces

        if (*aPtr == ' ')              // if space

            *aPtr = 0x01;              // set to odd character

        aPtr ++;                       // next!

    };                                 // end check for spaces

    if (*aPtr != 0)                    // if not end of string

       aPtr = strstr(aPtr+1, "\"");    // find next string

    else                               // otherwise

       aPtr = NULL;                    // break

};                                     // end mask spaces

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : Expression::UnmaskSpaces
//
// Description  : Unmask spaces that made tokenizing is easier
//
// Parameters   : 1) String to operate upon
//
// Returns      : 0 if OK
//
//****************************************************************************
int Expression::UnmaskSpaces(char *theStr)

{

int rc = 0;                            // result

char *aPtr = strstr(theStr, "\"");     // find quotation mark

while (aPtr != NULL)                   // for all quoted strings

{                                      // begin mask spaces

    aPtr ++;                           // bump past initial quote

    while (*aPtr != 0 &&               // while not end of string
           *aPtr != '"')               // and no ending quote found

    {                                  // begin check for spaces

        if (*aPtr == 0x01)             // if space
                                       
            *aPtr = ' ';               // set to odd character

        aPtr ++;                       // next!

    };                                 // end check for spaces

    if (*aPtr != 0)                    // if not end of string

       aPtr = strstr(aPtr+1, "\"");    // find next string

    else                               // otherwise

       aPtr = NULL;                    // break

};                                     // end mask spaces

return(rc);                            // return result

}

//****************************************************************************
//
// Method       : Expression::ProcessRPN
//
// Description  : Process token using RPN engine
//
// Parameters   : 1) Token
//
// Returns      : 0 if processing was OK
//
//****************************************************************************
int Expression::ProcessRPN(const char *theTok)

{

int rc = 0;                            // result


int aOp = GetOperator(theTok);         // get operator type

if (aOp != TYPE_NOOP)                  // if operator

{                                      // begin operate

    CviStr aLeft;                      // left operand
    CviStr aRight;                     // right operand

    if (!Pop(&itsRPNStack, aRight) ||  // try to
        !Pop(&itsRPNStack, aLeft))     // pop operands

    {

        rc = 8;

    }
    
    if (rc == 0)                       // if OK

    {                                  // begin use operator
   
       long aVal = 0;                  // value

       switch(aOp)                     // test operator
       {                               // begin cases of operator

           case TYPE_EQUAL           : // equal
                 
               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) == atol(aRight));   
               }
               else    
               if (!strcmp(aLeft, aRight))
                 aVal = 1;

               break;                     

           case TYPE_NONEQUAL        : // not equal

               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) != atol(aRight));   
               }
               else 
               if (strcmp(aLeft, aRight))
                 aVal = 1;

               break;


           case TYPE_GREATER         : // greater

               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) > atol(aRight));   
               }
               else 
               if (strcmp(aLeft, aRight) > 0)
                  aVal = 1;

               break;                  

           case TYPE_LESS            : // less
                 
               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) < atol(aRight));   
               }
               else 
               if (strcmp(aLeft, aRight) < 0)
                  aVal = 1;

               break;

           case TYPE_GREATER_EQ      : // greater or equal

               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) >= atol(aRight));   
               }
               else 
               if (strcmp(aLeft, aRight) >= 0)
                  aVal = 1;

               break;

           case TYPE_LESS_EQ         : // less or equal

               if (IsNumber(aLeft) && IsNumber(aRight))
               {
                 aVal = (atol(aLeft) <= atol(aRight));   
               }
               else 
               if (strcmp(aLeft, aRight) <= 0)
                  aVal = 1;

               break;

           case TYPE_OR              : // or

               aVal = (atol(aLeft) || atol(aRight));

               break;

           case TYPE_AND             : // and

               aVal = (atol(aLeft) && atol(aRight));

               break;

           case TYPE_ADD             : // add

               aVal = (atol(aLeft) + atol(aRight));

               break;

           case TYPE_SUBTRACT        : // subtract

               aVal = (atol(aLeft) - atol(aRight));

               break;


           case TYPE_MULTIPLY        : // multiply

               aVal = (atol(aLeft) * atol(aRight));

               break;

           case TYPE_DIVIDE          : // divide

               aVal = (atol(aLeft) / atol(aRight));

               break;

           case TYPE_POWER           : // power

               aVal = pow(atol(aLeft),atol(aRight));

               break;

//           default                   : // default

       };                              // end cases of operator

       //itsPgm->Trace("%s %s %s is %ld\n",
       //              (const char *) aLeft,
       //              theTok,
       //              (const char *) aRight,
       //              aVal);

       Push(&itsRPNStack, aVal);       // push answer onto stack

   }                                   // end use operator

}                                      // end operate

else                                   // otherwise

{                                      // just push

    int aRc = ReplaceVariable(&theTok);// replace token

    if (IsNumber(theTok) &&
        *(const char *) theTok != '#')
    {
        itsRewritten.Print(" %s", theTok);
    }
    else
    {

        itsRewritten.Print(" \"%s\"", theTok);
    }

    if (aRc == 4)                      // if unknown variable

    {                                  // begin use empty string

        theTok = "";                   // use empty string

    };                                 // end use empty string

    if (*theTok == '!'              && // if "not"
        IsNumber(theTok+1))            // and it has a number

    {                                  // begin modify

       if (atol(theTok+1) == 0)        // if zero
           theTok = "1";               // use 1
       else                            // otherwise
           theTok = "0";               // use 0

    };                                 // end modify

    Push(&itsRPNStack, theTok);        // push onto stack

};                                     // end push onto stack

return(rc);                            // return result

};

//****************************************************************************
//
// Method       : Expression::ReplaceVariable
//
// Description  : Replace variable
//
// Parameters   : 1) Token that may be a variable
//
// Returns      : 0 - Replacement OK
//                4 - Unknown variable found
//
//****************************************************************************
int Expression::ReplaceVariable(const char **theTok)

{

// Do nothing by default

return(0);                             // OK

}


//****************************************************************************
//
// Method       : Expression::Process
//
// Description  : Process expression
//
// Parameters   : 1) Place for result
//
// Returns      : 0 if processing was OK
//
//****************************************************************************
int Expression::Process(CviStr &theResult) // process expression

{

int rc = 0;

CviStr aVal;                           // token value


//itsPgm->Print("Processing %s\n", (const char *) itsExpression);

MaskSpaces(itsExpression);             // mask spaces as needed

PadOperators();                        // pad operators

//itsPgm->Print("Padded %s\n", (const char *) itsExpression);

const char *aTok = itsExpression.Parm(" \n", false);   // grab first token

while (rc == 0 &&                      // for all 
       aTok != NULL)                   // tokens

{                                      // begin process token

   aVal = aTok;                        // copy token

   UnmaskSpaces(aVal);                 // unmask spaces

   const char aChar =                  // grab first
       *(const char *) aVal;           // character

   if (aChar == 0)                     // if empty

   {                                   // begin bypass token

   }                                   // end bypass token

   else                                // otherwise

   if (aChar == '(')                   // if opening parenthesis

   {                                   // begin push

      itsRewritten.Add("(");           // add to rewritten expression

      Push(&itsExpStack, aVal);        // push onto stack

   }                                   // end push

   else                                // otherwise

   if (aChar == ')')                   // if closing paranthesis

   {                                   // begin find match

      int aError = 1;                  // assume failure

      while (itsExpStack != NULL)      // while we have items on stack

      {                                // begin process stack

         CviStr aStkVal;               // value for pop

         Pop(&itsExpStack, aStkVal);   // pop stack

         if (!strcmp(aStkVal, "("))    // if found opening paranthesis 

         {                             // begin OK/stop

            aError = 0;                // no error

            itsRewritten.Add(")");     // add to rewritten expression

            break;                     // stop processing

         }                             // end OK/stop

         else                          // otherwise

         {                             // begin process token

            rc = ProcessRPN(aStkVal);  // process via RPN engine

         };                            // end process token

      };                               // end process stack

      if (aError)                      // if error

      {                                // begin complain

         itsPgm->Error("Imbalanced parenthesis is expression.\n");

         rc=8;                         // error

      };                               // end complain

   }                                   // end find match

   else                                // otherwise

   if (IsOperator(aVal))               // if operator

   {                                   // begin process operator

      if (!strcmp(aVal, "EQ") ||
          !strcmp(aVal, "="))
          itsRewritten.Add(" ==");
      else
      if (!strcmp(aVal, "OR"))
          itsRewritten.Add(" ||");
      else
      if (!strcmp(aVal, "AND"))
          itsRewritten.Add(" &&");
      else
      if (!strcmp(aVal, "LE"))
          itsRewritten.Add(" <=");
      else
      if (!strcmp(aVal, "GE"))
          itsRewritten.Add(" >=");
      else
      if (!strcmp(aVal, "NE"))
          itsRewritten.Add(" !=");
      else
          itsRewritten.Print(" %s",
                             (const char *) aVal);

      while (itsExpStack != NULL &&    // while we have operators on the stack
             IsOperator(itsExpStack->itsValue) &&
             Precedence(itsExpStack->itsValue) <=
             Precedence(aVal))         // and precedence order is right

      {                                // begin process stack

         rc = ProcessRPN               // process through RPN engine
                (itsExpStack->itsValue);

         Pop(&itsExpStack);            // remove from stack


      };                               // end process stack

      Push(&itsExpStack, aVal);        // push onto expression stack
                                       
   }                                   // end process operator

   else                                // otherwise

   {                                   // begin process

       rc = ProcessRPN(aVal);          // process via RPN engine

   };                                  // end process

   aTok = itsExpression.Parm(" \n", false);    // next token

};

while (rc == 0 &&                      // while OK and
       itsExpStack != NULL)            // items left on stack

{                                      // begin process remaining stack

    if (!strcmp(itsExpStack->itsValue, // if we found a 
                "("))                  // ( left on stack

    {                                  // begin complain

        itsPgm->Error("Imbalanced parenthesis in expression.\n");
        rc = 8;                        // error

    };                                 // end complain

    rc = ProcessRPN                    // process
            (itsExpStack->itsValue);

    Pop(&itsExpStack);                 // remove from stack

};                                     // end process remaining stack

Pop(&itsRPNStack, theResult);          // grab answer off of stack

CviStr aRem;
while (Pop(&itsRPNStack, aRem))        // while we have more items

{                                      // begin complain

    if (rc == 0)                       // if we were OK before

    {                                  // begin give error

        itsPgm->Error("Data remains on stack!\n%s\n", (const char *) aRem);

        rc = 8;                        // bad

    };                                 // end give error

}                                      // end complain


return(rc);                            // we're good

}

//****************************************************************************
//
// Method       : Expression::Expression
//
// Description  : Construct expression processor
//
// Parameters   : 1) Expression to process
//
//****************************************************************************
Expression::Expression(const CviStr &theExpression)
{

itsExpStack = NULL;                    // no stacks

itsRPNStack = NULL;                    // no stacks

itsExpression = theExpression;         // set expression

itsPgm = GetTaskCviPgm();              // get program object

}

