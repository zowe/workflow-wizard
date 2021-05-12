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

#ifndef CVIVARIABLES_INC               // reinclude check
#define CVIVARIABLES_INC                   

#include "CviStr.h"                    // incldue strings

//****************************************************************************
// 
// List of Variables
//
// This is implemented as a linked list so we can iterate all entries easily
// but each entry is also on a chain in a list by hash value. This allows
// us to easily locate a specific variable name entry to locate its value.
// Adding this hash lookup has cut CPU time to 1/3rd of what it was before.
// 
// There are a few times when the code does walk every entry in the list but
// these are rare occasions (such as looking up target DSN information). The
// target DD processing should probably be modified to track those entries
// separately since multiple properties really relate to a single DSN. The
// CPU cost is not worth the effort but it may be worth the effort to reduce
// complexity
// 
// 
//****************************************************************************
#define VAR_HASH_SIZE 4096              // hash size for variables
//#define VAR_HASH_SIZE 400              // hash size for variables

class   VariableList;                  // variable list is a friend class

class  Variable                        // variable
{

private:
    CviStr itsName;                    // variable name
    CviStr itsVelocityName;            // velocity name
    CviStr itsVelocityCondName;        // velocity name
    CviStr itsJCLName;                 // JCL name
    CviStr itsValue;                   // variable value
    CviStr itsPropName;                // property name
    CviStr itsCondPropName;            // conditional property name $!{varname}

    int    itsHash;                    // hash value

    Variable *itsNext;                 // next

    Variable *itsHashNext;             // hashed next

    Variable *itsSubVars;              // subvariables
   
protected:

    void SetNext(Variable *theNext);   // set next

    void SetHashNext(Variable *theNext); // set next in hash list

    Variable *GetHashNext();           // get next in hash list

    int GetHash();                     // return hash value

    friend VariableList;               // our little friend...

public:

    static int GenHash(const char *theName); // generate hash

    Variable(const char *theName,      // constructor
             const char *theValue);    // property-style

    Variable(const char *theName);     // constructor - workflow style

    Variable(const Variable *);        // copy - deep

    ~Variable();                       // destructor

    Variable *GetNext();               // get next

    void SetVelocityScope(const char *); // set velocity scope

    void SetValue(const char *theValue);// set value

    void ReplaceWithVelocityName       // replace variables in string with 
            (CviStr &theStr);          // the Velocity name

    void TranslateJCLValue             // translate JCL variable name to value
            (CviStr &theStr);          // the value        

    char IsUsed(const CviStr &theStr); // see if variable is used here

    char *GetName();                   // return name

    const char *GetPropName();         // return property name

    const char *GetVelocityName();     // get velocity name

    void AddSubVariable(Variable *);   // add subvariable

    Variable *FindSubVar(const char *);// find subvariable

    Variable *GetSubVar();             // get subvariable

    void DetachSubVar();               // detach subvariable list (forget)

    Variable *SortRows();              // sort "row"-style variables

    void DeleteSorted(Variable *);     // delete sorted list

    CviStr &Value();                   // return value

};

//****************************************************************************
// 
// List of Variables
//
// Currently setup as a linked list, but want to add a hash list as well
// for efficiency.
//
//****************************************************************************
class   VariableList

{

private:

    Variable    *itsList;              // master list of variables - linked list
    Variable    *itsIter;              // list iterator for GetFirst/GetNext

    Variable    *itsHashList[VAR_HASH_SIZE]; // hashed list

public:

    VariableList();                    // constructor

    ~VariableList();                   // destructor

    Variable *GetFirst();              // get first on the list

    Variable *GetNext();               // get next

    int Add(const char *theName,       // add variable
            const char *theValue = NULL,
            char theDupeCheck = 0);

    int ValueByName(const char *theName, // get value by name
                    CviStr &theValue);

    int ValueByProperty(const char *theName, // get value by property tag
                        CviStr &theValue);

    Variable *FindVariable(const char *thePrefix, // find variable with prefix
                           const char *theValue); // and value

    Variable *GetVariable(const char *theName, // get variable
                          bool theScope = false);

    void ReplaceVariable               // replace variable (for expression handling)
            (CviStr &theStr);

    void ReplaceWithValues             // translate Velocity names to values
            (CviStr &theStr,           // for all list
             char theEscape = true);   // assume escaping is desired

    void ReplaceWithVelocityNames      // translate to instance- names      
            (CviStr &theStr);          // for all list

    void TranslateJCLValues            // translate JCL values
            (CviStr &theStr);          // for all list


};                                                                 
#endif                                 // reinclude check

