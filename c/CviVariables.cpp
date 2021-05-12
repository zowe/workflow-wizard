/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Provides a list of variables
// 
//****************************************************************************
#include <stdio.h>                     // stdio
#include <string.h>                    // strings
#include <stdlib.h>                    // stdlib
#include <ctype.h>                     // c types

#include "CviVariables.h"              // include ourself
#include "CviPgm.h"                    // program object

//****************************************************************************
// The following are small methods not worthy of a lot of comment headers..
//****************************************************************************

void Variable::SetNext(Variable *theNext) { itsNext = theNext; }

Variable *Variable::GetNext() { return(itsNext); }

void Variable::SetHashNext(Variable *theNext) { itsHashNext = theNext; }

Variable *Variable::GetHashNext() { return(itsHashNext); }

Variable *Variable::GetSubVar() { return(itsSubVars); }

void Variable::DetachSubVar() { itsSubVars = NULL; }

int Variable::GetHash() { return(itsHash); }

char *Variable::GetName() { return(itsName); }

const char *Variable::GetPropName() { return(itsPropName); }

const char *Variable::GetVelocityName() { return itsVelocityName; }

CviStr &Variable::Value() { return(itsValue); }

void Variable::SetVelocityScope(const char *theScope) 
{
    itsVelocityName.Reset();
    itsVelocityName.Print              // name to use when
        ("${%s%s}",                    // converting to Velocity
         theScope,
         (const char *) itsPropName);  

    itsVelocityCondName.Reset();
    itsVelocityCondName.Print          // name to use when
        ("$!{%s%s}",                   // converting to Velocity
         theScope,
         (const char *) itsPropName);  
}

void Variable::ReplaceWithVelocityName (CviStr &theStr)
{ 
  theStr.Replace(itsName,
                 itsVelocityName);
  theStr.Replace(itsCondPropName,            
                 itsVelocityCondName);
}

void Variable::AddSubVariable(Variable *theVar) 
{

   Variable *aVar = itsSubVars;        // grab head
   while (aVar != NULL &&              // while head is OK            
          aVar->itsNext != NULL)       // and have next item
       aVar = aVar->itsNext;           // use next to find last one on the list

   if (aVar != NULL)                   // if we have items already
       aVar->SetNext(theVar);          // use this one - next
   else                                // otherwise
       itsSubVars = theVar;            // use this one - head

}

//****************************************************************************

//****************************************************************************
//
// Method       : RemScope
//
// Description  : Remove scope
//
// Parameters   : 1) String to operator upon
//
//****************************************************************************
static void RemScope(CviStr &theStr)

{

const char *aSt = theStr;              // record start of string
                                       
const char *aDash =                    // locate dash
    strstr(aSt, "-");                  
                                       
const char *aRp = aDash;               // start of replacement area
                                       
while (aRp > aSt &&                    // while OK and
       *(aRp-1) != '{')                // not {
                                       
    aRp--;                             // backup
                                       
theStr.Replace(aRp,                    // remove scope
               (aDash - aRp)+1,      
               "");                  

}

//****************************************************************************
//
// Method       : UnEscape
//
// Description  : Undo escape sequences
//
// Parameters   : 1) String to operator upon
//
//****************************************************************************
static void UnEscape(CviStr &theStr)

{

const char *aChar = theStr;            // get first character

const char *aEsc = strstr(aChar, "\\"); // find escape character


while (aEsc != NULL)                   // for all escape characters

{                                      // begin handle escape

    if (*(aEsc+1) != 0)                // if not end of string

    {                                  // begin replace

        char aRepl[2];                 // replacement

        aRepl[0] = *(aEsc+1);          // character to replace
        aRepl[1] = 0;                  // terminate

        if (aRepl[0] == 'n')           // if newline

            aRepl[0] = '\n';           // use newline

        if (aRepl[0] == 't')           // if tab

            aRepl[0] = '\t';           // use tab

        int aOff =                     // get our offset
            aEsc - (const char *) theStr;

        theStr.Replace(aEsc, 2, aRepl); // replace

        aEsc = ((const char *) theStr) + aOff; // recompute address as replace may changed storage

    };                                 // end replace

    aEsc = strstr(aEsc+1, "\\");       // find next escape character

};                                     // end handle escape

}

//****************************************************************************
//
// Method       : Constructor
//
// Description  : Constructor for variables from property file.
//                These have a value set.
//
// Parameters   : 1) Variable name
//                2) Variable value
//
//****************************************************************************
Variable::Variable(const char *theName,
                   const char *theValue)
{                                          
                                       
itsHashNext = NULL;                    // no next
                                       
itsNext = NULL;                        // no next

itsSubVars = NULL;                     // no subvars yet
                                       
itsName.Set(theName);                  // set name

itsHash = GenHash(theName);            // generate hash value
                                       
if (*theName == '$')                   // if has $ already
                                       
{                                      // set preset property name
                                       
    itsPropName.Set(theName);          // use as-is
                                       
    itsCondPropName.Print("$!{%s}",
                          ((const char *) theName) + 1);

    CviStr aName(itsPropName);

    const char *aTok = aName.Tok("${}");

    itsVelocityName.Print              // name to use when
        ("${instance-%s}",             // converting to Velocity
         aTok);                     
                                       
    itsVelocityCondName.Print          // name to use when
        ("$!{instance-%s}",            // converting to Velocity
         aTok);                     
                                       
    itsJCLName.Print("&%s",            // JCL variable name
                     aTok);                
                                       
}                                      // end set preset property name
                                       
else                                   // otherwise
                                       
{                                      // begin set property name
                                       
    itsPropName.Print("${%s}",         // Velocity variable name
                      theName);        
                                       
    itsCondPropName.Print("$!{%s}",    // Velocity variable name
                          theName);

    itsVelocityName.Print              // name to use when
        ("${instance-%s}",             // converting to Velocity
         theName);                     
                                       
    itsVelocityCondName.Print          // name to use when
        ("$!{instance-%s}",            // converting to Velocity
         theName);                     
                                       
    itsJCLName.Print("&%s",            // JCL variable name
                     theName);             
                                       
};                                     // set property name
                                       
SetValue(theValue);                    // set value
                                       
}                                      

//****************************************************************************
//
// Method       : Constructor
//
// Description  : Constructor for variables from workflow XML
//
// Parameters   : 1) Variable name
//
//****************************************************************************
Variable::Variable(const char *theName)
{                                          
                                       
itsHashNext = NULL;                    // no next
                                       
itsNext = NULL;                        // no next

itsSubVars = NULL;                     // no subvars yet
                                       
itsPropName.Set(theName);              // save name

itsName.Print("${%s}",                 // name to search
              theName);                
                                       
itsCondPropName.Print("$!{%s}",        // name to search
                      theName);                
                                       
itsVelocityName.Print                  // name to use when
    ("${instance-%s}",                 // converting to Velocity
     (const char *) itsPropName);
                                       
itsVelocityCondName.Print              // name to use when
    ("$!{instance-%s}",                // converting to Velocity
     (const char *) itsPropName);
                                       
itsHash = GenHash(theName);            // generate hash value
                                       
}                                      

//****************************************************************************
//
// Method       : Constructor
//
// Description  : Construct new variable deep copy subvariables
//
// Parameters   : 1) Variable to copy
//
//****************************************************************************
Variable::Variable(const Variable *theVar)
{                                          
                                       
itsHashNext = NULL;                    // no next
                                       
itsNext = NULL;                        // no next

itsSubVars = NULL;                     // no subvars yet
                                       
itsName = theVar->itsName;             // copy name

itsVelocityName =                      // copy velocity
    theVar->itsVelocityName;           // name

itsVelocityCondName =                  // copy velocity
    theVar->itsVelocityCondName;       // name

itsJCLName = theVar->itsJCLName;       // copy JCL name

itsValue = theVar->itsValue;           // copy value

itsPropName = theVar->itsPropName;     // copy property name

itsCondPropName = theVar->itsCondPropName; // copy conditional property name

itsHash = theVar->itsHash;             // copy hash

                                       
Variable *aPrevSub = NULL;             // previous subvar

Variable *aSubVar = theVar->itsSubVars;// start of list
                                       
while (aSubVar != NULL)                // for all subvariables

{                                      // begin copy

    if (!strcmp(aSubVar->GetName(),    // skip 
                "rownum"))             // rownum

    {                                  // begin skip

    }                                  // end skip

    if (aPrevSub == NULL)              // if no previous

    {                                  // begin new head

        itsSubVars = new Variable(aSubVar); // allocate new head

        aPrevSub = itsSubVars;         // save for next iteration

    }                                  // end new head

    else                               // otherwise

    {                                  // begin new tail

        aPrevSub->itsNext = new Variable(aSubVar);

        aPrevSub = aPrevSub->itsNext;

    };                                 // end new tail

    aSubVar = aSubVar->itsNext;        // next!

};                                     // end copy
                                       
}                                      


//****************************************************************************
//
// Method       : Destructor
//
// Description  : Destructor
//
//****************************************************************************
Variable::~Variable()
{                                          
                                       
while (itsSubVars != NULL)             // for all variables

{                                      // begin destroy

    Variable *aVar = itsSubVars;       // grab list

    itsSubVars = itsSubVars->itsNext;  // get next one

    delete aVar;                       // delete

};                                     // end destroy

}                                      

//****************************************************************************
//
// Method       : GenHash
//
// Description  : Generate hash
//
// Parameters   : 1) String to hash
//
//****************************************************************************
int Variable::GenHash(const char *theName)

{                                      

int aHash = 0;                         // start with nothing

while (*theName != 0)                  // iterate over characters

{                                      // begin iterate characters

    if (*theName != '$' &&             // leave out
        *theName != '&' &&             // &$!{} out
        *theName != '!' &&             // so we can quickly hash                
        *theName != '{' &&             // for lookup up by Velocity names 
        *theName != '}')               // and JCL names

        aHash = aHash * 31 + *theName; // multiply by a prime and add to it

    theName++;                         // next character, please

};                                     // end iterate characters

aHash = aHash % VAR_HASH_SIZE;         // melt hash down to size

if (aHash < 0)                         // if it went negative

    aHash = aHash * -1;                // make it a positive

return(aHash);                         // return hash

};

//****************************************************************************
//
// Method       : TranslateJCLValue
//
// Description  : Translate JCL variable to value
//
// Parameters   : 1) String to translate
//
//****************************************************************************
void Variable::TranslateJCLValue       // translate JCL variable name to value
        (CviStr &theStr)               // the value        
                                        
{                                      
                                       
if (strstr(theStr,                     // if name
           itsJCLName) != NULL)        // is found
                                       
{

    theStr.ReplaceSymbol(itsName,      // do
                         itsValue);    // replacement

}

}                                      

//****************************************************************************
//
// Method       : IsUsed
//
// Description  : Determine if this variable is being used in a string.
//
// Parameters   : 1) String to check
//
//****************************************************************************
char Variable::IsUsed(const CviStr &theStr) // see if variable is used here
                                       
{                                      // begin IsUsed
                                       
CviStr aCheck1(itsVelocityName);       // check string
CviStr aCheck2(itsVelocityCondName);   // check string


aCheck1.Replace("}", ".");             // replace brace with "." to catch 
aCheck2.Replace("}", ".");             // ${instance-Var.toUpperCase()} and other such uses 

if (strstr(theStr,
           aCheck1) != NULL ||
    strstr(theStr,
           aCheck2) != NULL ||
    strstr(theStr,                     // if string has
           itsVelocityName) != NULL || // it
    strstr(theStr,
           itsVelocityCondName) != NULL)
                                       
    return(true);                      // it is used
                                       
return(false);                         // unused
                                       
}                                      // end IsUsed

//****************************************************************************
//
// Method       : SortRows
//
// Description  : Sort subvariables as if they are rows of a table
//
// Parameters   : None
//
//****************************************************************************
Variable *Variable::SortRows()

{

Variable *aSortRow = NULL;             // sorted row


Variable *aRow = itsSubVars;           // grab subvariables

while (aRow != NULL)                   // for all rows

{                                      // begin add to sorted list

    Variable *aNewRow =                // setup new row
        new Variable(aRow->GetName());

    Variable *aCol = aRow->itsSubVars;

//    while (aCol != NULL  &&            // locate first
//           !strcmp(aCol->GetName(),    // column that
//                   "rownum"))          // isn't rownum..
//        aCol = aCol->itsNext;
    
    aNewRow->itsSubVars = aCol;        // set our subvars

    Variable *aPrev = NULL;            // previous
    Variable *aIns = aSortRow;         // first possible insert spot

    bool aMisMatchWarn = false;        // no warning yet

    while (aIns != NULL)               // for all insert spots

    {                                  // begin compare
                                       
        Variable *aColI = aIns->itsSubVars;
        Variable *aColR = aNewRow->itsSubVars;

        while (aColI != NULL && aColR != NULL)
        {

            int aComp = 0;

            if (strcmp(aColI->GetName(),
                       "rownum"))       // ignore this automated column

            {

                if (strcmp(aColR->GetName(),
                           aColI->GetName()))
                {

                    if (!aMisMatchWarn)
                        GetTaskCviPgm()->Trace("WARNING: Row of %s has mismatched column orders: %s %s\n",
                                               (const char *) GetName(),
                                               (const char *) aColR->GetName(),
                                               (const char *) aColI->GetName());
                    aMisMatchWarn = true;
                }

                else

                {

                    aComp = strcmp(aColR->Value(),
                                   aColI->Value());

                };

            }

            if (aComp < 0)

            {                          // begin insert here

                //GetTaskCviPgm()->Trace("Inserting %s before %s\n",
                //                       (const char *) aColR->Value(),
                //                       (const char *) aColI->Value());

                if (aPrev == NULL)     // if new head

                    aSortRow = aNewRow;// we are the new head

                else                   // otherwise

                    aPrev->itsNext = aNewRow; // link 

                aNewRow->itsNext = aIns;  // set our next

                aNewRow = NULL;        // inserted

                break;                 // break out of while

            }                          // end insert here

            if (aComp > 0)             // if larger

                break;                 // move on

            aColR = aColR->itsNext;
            aColI = aColI->itsNext;
        }

        if (aNewRow == NULL)           // if inserted

            break;                     // break out

        aPrev = aIns;                  // mark previous
        aIns = aIns->itsNext;          // grab next

    };                                 // end compare

    if (aNewRow != NULL)               // if we haven't inserted it yet

    {                                  // begin insert at end

        //GetTaskCviPgm()->Print("Inserting %s at end\n",
        //                       (const char *) aNewRow->itsSubVars->Value());

        if (aPrev == NULL)             // if new head

            aSortRow = aNewRow;        // we are the new head

        else                           // otherwise

            aPrev->itsNext = aNewRow;  // link 

        aNewRow->itsNext = aIns;       // set our next

    }                                  // end insert at end

//    aNewRow->itsSubVars = aRow->itsSubVars;

    aRow = aRow->itsNext;              // next

};                                     // end add to sorted list

#ifdef _SRC_

aRow = aSortRow;                       // display

int aRowNum = 1;                       // starting row number

while (aRow != NULL)                   // begin renumber

{                                      // begin number

    GetTaskCviPgm()->Trace("ROW\n");   // display

    Variable *aCol = aRow->itsSubVars; // get columns

    while (aCol != NULL)               // for all columns
    {

        GetTaskCviPgm()->Trace("  %s: %s\n",
                               (const char *) aCol->GetName(),
                               (const char *) aCol->Value());
        aCol = aCol->itsNext;
    }

    aRow = aRow->itsNext;

    aRowNum ++;

}
#endif

return(aSortRow);                      // return new list

}

//****************************************************************************
//
// Method       : DeleteSorted
//
// Description  : Deletes sorted row list
//
// Parameters   : 1) List to delete
//
//****************************************************************************
void Variable::DeleteSorted(Variable *theRow)
{

Variable *aRow = theRow;               // start at the beginning...

while (aRow != NULL)                   // for all rows

{                                      // begin clear columns

    aRow->itsSubVars = NULL;           // disconnect columns

    aRow = aRow->itsNext;              // next!

};                                     // end clear columns

if (theRow != NULL)                    // if OK

    delete theRow;                     // delete (cascades)

}



//****************************************************************************
//
// Method       : SetValue
//
// Description  : Set variable value
//
// Parameters   : 1) Value to set. May be an array, in which case
//                   children will be generated
//
//****************************************************************************
void Variable::SetValue(const char *theValue) 

{ 

const char *aValue = theValue;         // get value

while (*aValue == ' ')                 // while space
    aValue ++;                         // bump to skip past it

// The config tool is not properly closing the TARGET= list
// So we will relax this check
//if (strlen(aValue) > 1 &&            // if this may
//    *aValue == '[' &&                // be an array
//    *(theValue+strlen(aValue)-1) == ']')

// Alternative check
if (strlen(aValue) > 2 &&              // if this may
    *aValue == '[' &&                  // be an array
    *(aValue+1) == '{')                

{                                      // begin pocess array

   int   aRowCnt = 0;                  // row counter

   const char *theStart = NULL;        // start of row

   while (*theValue != 0)              // for all of the value

   {                                   // begin process value

       if (*theValue == '{')           // if opening of a new set

       {                               // begin add new child variable

          theStart = theValue;         // start of row

          theValue =                   // end of row
              strstr(theValue, "}");

          if (theValue != NULL)        // if OK

          {                            // begin row/children

              CviStr aRow;             // row
              CviStr aName;            // name
              CviStr aValue;           // value
              CviStr aRowNum;          // row number

              theStart ++;             // bump start

              GetTaskCviPgm()->Trace("Row of %s\n", (const char *) itsName);

              aRow.Add(theStart,       // setup row
                       (int) (theValue - theStart));

              Variable *aRowVar = new Variable(itsName);

              const char *aPtr = aRow; // start parsing name

              while (aPtr != NULL &&   // for all
                     *aPtr != 0)       // characters

              {                        // begin process entry

                  if (*aPtr == '"')    // if quote

                  {                    // begin find closing quote

                     aPtr ++;          // skip beyond quote

                     const char *aEnd = aPtr;

                     while (*aEnd != 0 &&
                            *aEnd != '"')

                     {                 // begin find closing quote

                        if (*aEnd == '\\') // if escape sequence

                            aEnd ++;      // skip escape sequence

                        if (*aEnd != 0)// if string has data left

                           aEnd ++;    // bump forward

                     };                // end find closing quote

                     if (aEnd != NULL) // if OK

                     {                 // begin set name or value

                         if (strlen(aName) == 0)

                         {             // begin set name

                             aName.Add(aPtr,
                                       (int) (aEnd - aPtr));

                         }             // end set name

                         else          // otherwise

                         {             // begin set value

                             aValue.Add(aPtr,
                                        (int) (aEnd - aPtr));

                             UnEscape(aName); // undo escape sequences

                             UnEscape(aValue); // undo escape sequences

                             GetTaskCviPgm()->Trace("   Sub Variable %s(%s)\n",
                                                    (const char *) aName,
                                                    (const char *) aValue);

                             Variable *aColVar = new Variable(aName, aValue);

                             aRowVar->AddSubVariable(aColVar);  // add subvariable (column)

                             aName.Reset();

                             aValue.Reset();

                         };            // end set value

                         aPtr = aEnd;  // end of item

                     };                // end set name or value

                  };                   // end find closing quote

                  aPtr ++;             // next

              };                       // end process entry

              if (aRowVar->GetSubVar() != NULL)

              {                        // begin add row

                  aRowCnt ++;          // bump row counter

                  aValue.Print("%d", aRowCnt);

                  aRowVar->AddSubVariable(new Variable("rownum", aValue));

                  aValue.Reset();      // reset value

                  AddSubVariable(aRowVar);  // add subvariable (row)

                  aRowNum.Print("%d", aRowCnt);

              }                        // end add row

              else                     // otherwise

                  delete aRowVar;      // remove row - it was empty

          }                            // end add row/children

          else                         // otherwise

              break;                   // break out of loop

       };                              // end add new child variable

       theValue ++;                    // bump

   };                                  // end process value

}                                      // end process array

else                                   // otherwise

    itsValue = theValue;               // just set the value

itsValue.Trim(' ');                    // trim trailing spaces


}

//****************************************************************************
//
// Method       : FindSubVar
//
// Description  : Locate variable and return pointer to it
//
// Parameters   : 1) Variable name
//
//****************************************************************************
Variable *Variable::FindSubVar(const char *theName)
{

Variable *aVar = itsSubVars;           // grab subvariables

while (aVar != NULL &&                 // for all
       strcmp(aVar->GetPropName(),     // variables
              theName) &&              // see if
       strcmp(aVar->GetName(),         // name or property matches
              theName))                // matches

    aVar = aVar->GetNext();            // get next variable


return(aVar);                          // return result

}

//****************************************************************************
//
// Method       : Variable list constructor
//
// Description  : Default constructor
//
//****************************************************************************
VariableList::VariableList()           // constructor

{                                      

    itsList = NULL;                    // no list yet
    itsIter = NULL;                    // no iteration yet

    memset((void *) itsHashList,       // clear
           0,                          // hash
           sizeof(itsHashList));       // list

}                                      

//****************************************************************************
//
// Method       : Variable list destructor
//
// Description  : Clean up
//
//****************************************************************************
VariableList::~VariableList()          // destructor
{

#ifdef _SRC_
        GetTaskCviPgm()->Print("Variable hash value diagnostics\n");
        int aIdx = 0;
        while (aIdx < VAR_HASH_SIZE)
        {
            int aCnt = 0;
            Variable *aVar = itsHashList[aIdx];
            while (aVar != NULL)
            {
                aCnt ++;
                aVar = aVar->GetHashNext();
            }
            GetTaskCviPgm()->Print("   Slot %d: %d entries\n",
                                   aIdx, aCnt);
            aIdx ++;
        }
#endif

while (itsList != NULL)                // remove variables
{
    Variable *aItem = itsList;
    itsList = itsList->GetNext();
    delete aItem;
}

}

//****************************************************************************
//
// Method       : GetFirst
//
// Description  : Get first variable in the list
//
// Parameters   : None
//
// Returns      : First item on list or NULL if nothing found
//
//****************************************************************************
Variable *VariableList::GetFirst()     // get first on the list

{                                  

itsIter = itsList;                     // grab list
return(itsIter);                       // return iterator

}                                      

//****************************************************************************
//
// Method       : GetNext
//
// Description  : Get next variable in the list
//
// Parameters   : None
//
// Returns      : Next item on list or NULL if nothing found
//
//****************************************************************************
Variable *VariableList::GetNext()      // get next

{

if (itsIter != NULL)                   // if OK, get next one

    itsIter = itsIter->GetNext();
                                       
return(itsIter);                       // return it

}

//****************************************************************************
//
// Method       : TranslateJCLValues
//
// Description  : Replace JCL-style variables with value
//
// Parameters   : 1) String to do replacement upon
//
//****************************************************************************
void VariableList::TranslateJCLValues  // translate JCL values
        (CviStr &theStr)               // for all list

{                                      // begin translate

Variable *aVar = itsList;              // start of list
                                       
if (strstr(theStr, "&") == NULL)       // if not found
                                       
    aVar = NULL;                       // do nothing

while (aVar != NULL)                   // for all variables
                                       
{                                      // begin search/replace
                                           
    aVar->TranslateJCLValue(theStr);   
                                       
    aVar = aVar->GetNext();            // next!
                                   
};                                     // end search/replace

}                                      // end translate

//****************************************************************************
//
// Method       : ReplaceWithVelocityNames
//
// Description  : Replace variables with Velocity instance- name 
//                The only time this is called is if we have workflow-defined
//                variables...which we do not have as of 2/13/2019 anyway.
//
// Parameters   : 1) String to do replacement upon
//
//****************************************************************************
void VariableList::ReplaceWithVelocityNames  // translate to instance- names      
        (CviStr &theStr)               // for all list
                                       
{                                      // begin translate
                                       
Variable *aVar = itsList;              // start of list
                                       
if (strstr(theStr, "${") == NULL)      // if not found
                                       
    aVar = NULL;                       // do nothing
                                       
while (aVar != NULL)                   // for all variables
                                       
{                                      // begin search/replace

    aVar->ReplaceWithVelocityName(theStr);
                                       
    aVar = aVar->GetNext();            // next!
                                       
};                                     // end search/replace
                                       
}                                      // end translate

//****************************************************************************
//
// Method       : Replace with Values
//
// Description  : Replace variables with value
//
// Parameters   : 1) String to do replacement upon
//
//****************************************************************************
void VariableList::ReplaceWithValues   // translate Velocity names to values
        (CviStr &theStr,               // for all list
         char theEscape)               // escape them?
                                       
{                                      // begin translate
                                       
static CviStr aFind;                   // string to find
static CviStr aVarName;                // variable name


const char *aVar = strstr(theStr, "$");

while (aVar != NULL)                   // for all variables
                                       
{                                      // begin search/replace
                                       
    long aFoundIdx =                   // convert to index
        (long) (aVar - (const char *) theStr);

    const char *aName = aVar;          // get name portion

    aVar ++;                           // bump to next character

    while (*aVar != 0 &&               // while not end of string and
           (*aVar == '_' ||            // has valid 
            *aVar == '-' ||            // Velocity-style
            *aVar == '!' ||            // characters
            *aVar == '{' ||            // for
            isalnum(*aVar)))           // name

        aVar ++;                       // keep going

    aFind.Reset();                     // reset
    aVarName.Reset();                  // reset

    char aForm = 'V';                  // assume Velocity form

    if ((aVar - aName) > 3 && 
        *aVar == '}' && 
        (*(aName+1) == '{' ||
         (*(aName+1) == '!' &&
          *(aName+2) == '{')))

        aFind.Print("%*.*s",           // setup for Velocity form
                    (int)(aVar - aName)+1,
                    (int)(aVar - aName)+1,
                    aName);

    else                               // otherwise

    {                                  // not Velocity

        aFind.Print("%*.*s",           // not Velocity-form (should be unusual...)
                    (int)(aVar - aName),
                    (int)(aVar - aName),
                    aName);

        aForm = 'U';                   // unusual form...

    };                                 // end not Velocity

    if (aForm == 'V')                  // if Velocity form

    {                                  // begin setup variable name

        const char *aVarN = strstr(aFind, "{");    // find {

        if (aVarN != NULL)             // if found

        {

            aVarN ++;                  // use it

            int aLen = strlen(aVarN);  // get length of find portion

            aVarName.Print("${%*.*s}",  // setup variable name
                           aLen-1,
                           aLen-1,
                           aVarN);

        };

    }                                  // end setup variable name

    else                               // otherwise

    {                                  // begin standard form

        aVarName.Set(aFind);           // use name as-is

    };                                 // end standard form

    int aHash = Variable::GenHash(aVarName);

    Variable *aVari = itsHashList[aHash]; // grab hash list head

    while (aVari != NULL            && // while OK and
           strcmp(aVarName,            // not a match
                  aVari->GetPropName()))

        aVari = aVari->GetHashNext();  // get next hash entry


//    if (aVari == NULL)
//        GetTaskCviPgm()->Print("Failed to find %s\n",
//                           (const char *) aVarName);

    if (aVari == NULL &&               // if not found and
        strstr(aVarName, "-") != NULL && // has a 'scope'
        strstr(aVarName, "_workflow-") == NULL &&
        strstr(aVarName, "_WORKFLOW-") == NULL)

    {                                  // begin unscope

        RemScope(aVarName);            // remove scope from name

//        GetTaskCviPgm()->Print("Trying %s instead\n",
//                               (const char *) aVarName);

        aHash = Variable::GenHash(aVarName);

        aVari = itsHashList[aHash];    // grab hash list head

        while (aVari != NULL            && // while OK and
               strcmp(aVarName,            // not a match
                      aVari->GetPropName()))

            aVari = aVari->GetHashNext();  // get next hash entry

    };                                 // end unscope

    if (aVari != NULL &&               // if found
        aVari->FindSubVar("VELOCITY") == NULL) // and not something to prompt

    {                                  // begin find/replace

        char aRepl = true;             // assume will do replace

        CviStr anEscaped(aVari->Value()); // copy for escaping

        if (theEscape)                 // if should be escaped
           anEscaped.VelocityEscape(); // add escape sequences

        const char *aClose =           // find closing
            strstr(aName, "]]#");      // escape sequence

        if (aClose != NULL)            // if close found
        {                              // begin locate open
            const char *aOpen = 
                strstr(aName, "#[[");  // locate open

            if (aOpen == NULL       || // if not found or
                aOpen > aClose)        // it is after close

            {                          // begin must be inside escape sequence
                                       
                GetTaskCviPgm()->Trace("Refusing to replace inside escape sequence.%*.*s\n",
                                       (int) (aClose - aName) + 3,
                                       (int) (aClose - aName) + 3,
                                       aName);

                aRepl = false;         // do not replace

            }                          // end must be inside escape sequence

        }                              // end locate open

        if (aRepl)                     // if replacing is OK
        {

            if (strlen(anEscaped) > 4 &&
                strstr(aName, "DBC_ENV") != NULL)

            {
                printf("%s\n", (const char *) anEscaped);
                printf("Ending: %s\n", ((const char *) anEscaped) + strlen(anEscaped) - 4);
                printf("Next: %x(%c)\n\n",
                       (int) *(aName + strlen(aFind)),
                       *(aName + strlen(aFind)));

            }

            if (strlen(anEscaped) > 4 &&
                !strcmp(((const char *) anEscaped) + strlen(anEscaped) - 4,
                        "#end") &&
                *(aName + strlen(aFind)) == '\n')
                  anEscaped.Add("$!{EOL}");
            theStr.Replace(aName, strlen(aFind), anEscaped);
        }

        aVar = strstr((const char *) theStr + aFoundIdx + strlen(aVari->Value()), "$");

    }                                  // end find replace

    else                               // otherwise

        aVar = strstr((const char *) theStr + aFoundIdx + 1, "$");

};                                     // end search/replace
                                       
}                                      // end translate

//****************************************************************************
//
// Method       : ReplaceVariable
//
// Description  : Replace variable name with value (for expression handling)
//                The expression handler could also call ReplaceWithValues...
//                If that were the case then someone could do something like
//                  ${VARNAME}22="22" - but why? This keeps it such that the
//                variable name is not being combined with anything without
//                intention...
//
// Parameters   : 1) String to do replacement upon
//
//****************************************************************************
void VariableList::ReplaceVariable(CviStr &theStr)

{
                                           
if (ValueByProperty(theStr, theStr) != 0)  // locate property
                                       // or value and replace our string with value 
    ValueByName(theStr, theStr);       // or leave alone if not found

}

//****************************************************************************
//
// Method       : Add
//
// Description  : Add new variable
//
// Parameters   : 1) Name
//                2) Value
//                3) Duplicate check (0 - None, 1 - Reject, 2 - Replace) 
//
//****************************************************************************
int VariableList::Add(const char *theName,
                      const char *theValue,
                      char theDupeCheck)
{

char aAdd = true;                      // assume add should happen

int aRc = 0;                           // assume all is well

int aHash = Variable::GenHash(theName); // get hash value

if (theDupeCheck)                      // if checking for duplicates

{                                      // begin check for duplicates

    aAdd = false;                      // assume we will find duplicate

    Variable *aItem = itsHashList[aHash]; // get hashed list

    while (aItem != NULL &&            // while we have an item and
           strcmp(aItem->GetName(),    // names do not
                  theName) &&          // match
        strcmp(aItem->GetPropName(),   // names do not
               theName))               // match

        aItem = aItem->GetHashNext();  // try next one

    if (aItem != NULL)                 // if duplicate found

    {                                  // begin duplicate

        if (theDupeCheck == 2)         // if replace wanted

        {                              // replace

            aItem->SetValue(theValue); // set value
                                       
            if (aItem->itsSubVars != NULL)
            {
                delete aItem->itsSubVars;
                aItem->itsSubVars = NULL;
            };

        }                              // end replace

        else                           // otherwise

            aRc = 4;                   // give warning

    }                                  // end duplicate

    else                               // otherwise

        aAdd = true;                   // we need to add

}                                      // end check for duplicates

if (aAdd)                              // if we should add

{                                      // begin find new

    Variable *aVar;                    // setup to 

    if (theValue != NULL)              // if value specified

        aVar = new Variable(theName,   // use property-style constructor
                            theValue);     

    else                               // otherwise

        aVar = new Variable(theName);  // use WF-style constructor

    aVar->SetNext(itsList);            // use current head as our tail
    itsList = aVar;                    // set new head of list

    aVar->SetHashNext(itsHashList[aHash]); // use current head as our tail
    itsHashList[aHash] = aVar;         // set new head

};                                     // end find new


return(aRc);                           // return the result

}

//****************************************************************************
//
// Method       : ValueByName
//
// Description  : Get variable value by name
//
// Parameters   : 1) Variable name  
//                2) Variable value 
//                
//
//****************************************************************************
int VariableList::ValueByName(const char *theName,
                              CviStr &theValue)
{

int r_c = 4;                           // assume not found

int aHash =                            // create
     Variable::GenHash(theName);       // hash for name
    
Variable *aVar = itsHashList[aHash];   // list to locate item

while (aVar != NULL &&                 // for all
       strcmp(aVar->GetName(),         // variables, see if name
              theName))                // matches

    aVar = aVar->GetHashNext();        // get next variable

if (aVar == NULL &&                    // if not found and
    strstr(theName, "-") != NULL)      // has a 'scope'

{                                      // begin unscope

    CviStr aVarName(theName);          // variable name

    RemScope(aVarName);                // remove scope from name

    aHash = Variable::GenHash(aVarName);// rehash

    aVar = itsHashList[aHash];         // list to locate item

    while (aVar != NULL &&             // for all
           strcmp(aVar->GetName(),     // variables, see if name
                  aVarName))           // matches

        aVar = aVar->GetHashNext();    // get next variable

};                                     // end unscope

if (aVar != NULL)                      // if found

{                                      // begin get value

    r_c = 0;                           // we are OK

    theValue.Set(aVar->Value());       // set return value

};                                     // end get value


return(r_c);                           // return

}

//****************************************************************************
//
// Method       : ValueByProperty
//
// Description  : Get property value
//
// Parameters   : 1) Variable name 
//                2) Variable value
//
//****************************************************************************
int VariableList::ValueByProperty(const char *theName,
                                  CviStr &theValue)
{

int r_c = 4;                           // assume not found

int aHash = Variable::GenHash(theName);
   

Variable *aVar = itsHashList[aHash];   // list to locate item

while (aVar != NULL &&                 // for all
       strcmp(aVar->GetPropName(),     // variables, see if name
              theName))                // matches

    aVar = aVar->GetHashNext();        // get next variable

if (aVar == NULL &&                    // if not found and
    strstr(theName, "-") != NULL)      // has a 'scope'

{                                      // begin unscope

    CviStr aVarName(theName);          // variable name

    RemScope(aVarName);                // remove scope from name

    aHash = Variable::GenHash(aVarName);// rehash

    aVar = itsHashList[aHash];         // list to locate item

    while (aVar != NULL &&             // for all
           strcmp(aVar->GetPropName(), // variables, see if name
                  aVarName))           // matches

        aVar = aVar->GetHashNext();    // get next variable

};                                     // end unscope


if (aVar != NULL)                      // if found

{                                      // begin get value

    r_c = 0;                           // we are OK

    theValue.Set(aVar->Value());       // set return value

};                                     // end get value


return(r_c);                           // return

}

//****************************************************************************
//
// Method       : FindVariable
//
// Description  : Return variable object with given prefix/value or name.
//                Value will be matched with a comma-delimited list of values
//                in the variable.
//
// Parameters   : 1) Variable prefix 
//                2) Variable value  
//
//****************************************************************************
Variable *VariableList::FindVariable(const char *thePrefix, 
                                     const char *theValue)  
{

Variable *aItem = itsList;             // grab item

while (aItem != NULL)                  // while item is OK and

{                                      // begin check variable

    const char *aName = aItem->GetName(); // get name

    if (strstr(aName,                  // correct prefix
               thePrefix) == aName)    // found

    {                                  // begin check value

        if (strstr(aItem->Value(),     // may contain
                   theValue) != NULL)  // value

        {                              // begin tokenize

            CviStr aValue(aItem->Value());

            const char *aTok =         // tokenize
                aValue.Tok(",");       // by commas

            while (aTok != NULL &&     // locate token with value
                   strcmp(aTok, theValue))

                aTok = aValue.Tok(",");

            if (aTok != NULL)          // if found

                break;                 // break - we found out

        };                             // end tokenize

    };                                 // end check value

    aItem = aItem->GetNext();          // next!

};                                     // end check variable


return(aItem);                         // return variable

}

//****************************************************************************
//
// Method       : GetVariable
//
// Description  : Locate variable and return pointer to it
//
// Parameters   : 1) Variable name
//
//****************************************************************************
Variable *VariableList::GetVariable(const char *theName,
                                    bool theScope)
{

int aHash = Variable::GenHash(theName);
   

Variable *aVar = itsHashList[aHash];   // list to locate item

while (aVar != NULL &&                 // for all
       strcmp(aVar->GetPropName(),     // variables
              theName) &&              // see if
       strcmp(aVar->GetName(),         // name or property matches
              theName))                // matches

    aVar = aVar->GetHashNext();        // get next variable

if (aVar == NULL &&                    // if not found and
    theScope &&                        // we should use scope (if passed) and
    strstr(theName, "-") != NULL)      // scope specified

{                                      // begin drop scope

    const char *aName = strstr(theName, "-") +1;

    aHash = Variable::GenHash(aName);

    Variable *aVar = itsHashList[aHash]; // list to locate item

    while (aVar != NULL &&             // for all
           strcmp(aVar->GetPropName(), // variables
                  aName) &&            // see if
           strcmp(aVar->GetName(),     // name or property matches
                  aName))              // matches

        aVar = aVar->GetHashNext();    // get next variable

};                                     // end drop scope


return(aVar);                          // return result

}

