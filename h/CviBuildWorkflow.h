/*

  This program and the accompanying materials are

  made available under the terms of the Eclipse Public License v2.0 which accompanies

  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  

  SPDX-License-Identifier: EPL-2.0

  

  Copyright Contributors to the Zowe Project.

*/

//****************************************************************************
// DESCRIPTION
//         Utility to build a workflow from templates
// 
//****************************************************************************

#ifndef CVIBUILDWORKFLOW_INC           // reinclude check
#define CVIBUILDWORKFLOW_INC                   

//****************************************************************************
// Includes

#include "CviStr.h"                    // string class
#include "CviPgm.h"                    // CviPgm based
#include "CviVariables.h"              // needs variables

#include "btype.h"                     // types
#include "filestr.h"                   // file stream

          
//         
//****************************************************************************

//****************************************************************************
//
// Class        : WorkflowStep
//
// Description  : Holds a workflow step and its dependencies
//
//****************************************************************************
class   Dataset;
class   fileds;

class   WorkflowStep

{

protected:                             // protected members

   CviStr   itsMember;                 // member name
   CviStr   itsName;                   // step name
   CviStr   itsGroup;                  // group name
   CviStr   itsContent;                // step XML
   CviStr   itsDepends;                // dependencies
   CviStr   itsPreReqs;                // prerequisites
   CviStr   itsType;                   // step type
   CviStr   itsDescription;            // step description
   CviStr   itsSkills;                 // skills needed
   CviStr   itsTitle;                  // title
   CviStr   itsInstructions;           // instructions
   CviStr   itsSaveAs;                 // save as name
   CviStr   itsJobName;                // jobname to use
   CviStr   itsDescendents;            // descendent steps
   CviStr   itsReqDescendents;         // required descendent steps
   CviStr   itsSuccess;                // success pattern
   
   CviStr   itsDynamic;                // dynamic step iteration
   CviStr   itsOrigContent;            // original content

   CviStr   itsReRunData;              // save data for later re-run

   CviStr   itsTemplates;              // templates involved in this step

   int      itsMaxRc;                  // maximum return code

   int      itsLocked;                 // locked into position?

   int      itsBypass;                 // bypass step?
   int      itsReqCnt;                 // requirement counter
   int      itsSuspend;                // suspend automation?
   int      itsAutoEnable;             // auto-enable flag
   int      itsOptional;               // optional flag
   int      itsDepPriorSteps;          // depend on all prior steps?
   int      itsRefCnt;                 // referenced counter
   int      itsHasOptional;            // has optional steps underneath?
   int      itsIgnoreDeps;             // ignore depdendencies

   int      itsWritten;                // was this step written?
   int      itsRealStep;               // real step?

   int      itsComboCnt;               // combined count

   int      itsBypassIfSaveAsExists;   // bypass step if it exists

   char     itsProcType;               // processing type
   char     itsCombine;                // combine?
   char     itsTarget;                 // target user: VENDOR or CUSTOMER


   VariableList *itsPromptList;        // prompt list

   WorkflowStep *itsNext;              // next for chain of all steps (unordered)

   WorkflowStep *itsBaseStep;          // base step name

   WorkflowStep *itsOrderChild;        // ordered children - this really should be 'siblings'
   WorkflowStep *itsGrpOrderChild;     // ordered children

   WorkflowStep *itsChild;             // track children - this really should be 'siblings'
   WorkflowStep *itsGrpChild;          // group children

   WorkflowStep *itsPrev;              // previous

   WorkflowStep *itsGrpNext;           // group members  - next group member for list of groups
   WorkflowStep *itsGrp;               // group address  - direct chain back to parent group - keep this for prereqs

public:                                // public members

   WorkflowStep(const CviStr &theName);// constructor

   ~WorkflowStep();                    // destructor

   int AddStep(CviStr &theParms,       // add new step
               CviStr &theContent);    // new content

   int AddStep(CviStr &theParms);      // add new step w/override parms

   int ParseData(CviStr &theData,      // parse data
                 VariableList *thePropVars = NULL);

   int ReParseData(VariableList *thePropVars);

   int AddConditions(CviStr &,         // add conditions to step
                     VariableList *thePropVars);

   int GenGroup(CviStr &,              // generate steps for group
                VariableList *,
                VariableList *,
                WorkflowStep *);

   int GenXML(CviStr &,                // generate XML
              VariableList *,          
              VariableList *,
              WorkflowStep *);

   void GenSaveJCL(CviStr &,           // generate JCL to create a member
                   int theStepNum);    // JCL step number to create

   void GenStdProperties(VariableList *); // generate std properties for step

   void PrepareContent(CviStr &theStr, // prepare content
                       VariableList *theWkVars = NULL,
                       VariableList *thePropVars = NULL,
                       char theAddBr = 0, // add <br> to content?
                       char theSubJCLVar = 0); // sub JCL vars?

   void ResetReferences(WorkflowStep *theSteps,  // reset reference counters
                        VariableList *theVars);

   void BumpReferences(VariableList *theVars);  // increment reference count w/var

   void SetReferenceVar(VariableList *theVars); // set reference variable

   void DisplayStepsWith(WorkflowStep *theSteps,
                         const char *theData);

   int  ProcessStatements(VariableList *theVars,        // process statements
                          WorkflowStep *theIncludes,
                          WorkflowStep *theWfMacros);

   int  ProcessStatements(CviStr &theContent,           // process statements
                          VariableList *theVars,
                          WorkflowStep *theIncludes,
                          WorkflowStep *theWfMacros);

   int  ProcessWfMacro(CviStr &theContent,  // process workflow macro
                       WorkflowStep *theWfMacro,
                       VariableList *theVars,
                       WorkflowStep *theIncludes,
                       WorkflowStep *theWfMacros);

   int  ProcessWfMacros(CviStr &theContent,  // process workflow macros
                        VariableList *theVars,
                        WorkflowStep *theIncludes,
                        WorkflowStep *theWfMacros);

   int  ProcessDynamic(VariableList *theVars,  // process dynamic section
                       WorkflowStep *theIncludes,
                       WorkflowStep *theWfMacros);

   int  ProcessIfEmpty(CviStr &theContent,  // process if_empty
                       VariableList *theVars);

   int  AddMacros(WorkflowStep *);     // add macros from macro steps

   void ProcessIntMacros(VariableList *);  // process TARGET_DSN

   void SetNext(WorkflowStep *theNext) { itsNext = theNext; };
   void SetOrderChild(WorkflowStep *theChild) { itsOrderChild = theChild; };
   void SetChild(WorkflowStep *theChild) { itsChild = theChild; };
   void SetGrpChild(WorkflowStep *theChild) { itsGrpChild = theChild; };
   void SetGrpOrderChild(WorkflowStep *theChild) { itsGrpOrderChild = theChild; };
   void SetGrpNext(WorkflowStep *theNext) { itsGrpNext = theNext; };
   void SetGrp(WorkflowStep *theGrp) { itsGrp = theGrp; };

   WorkflowStep *GetNext() { return itsNext; };
   WorkflowStep *GetChild() { return itsChild; };
   WorkflowStep *GetOrderChild() { return itsOrderChild; };
   WorkflowStep *GetGrpChild() { return itsGrpChild; };
   WorkflowStep *GetGrpOrderChild() { return itsGrpOrderChild; };
   WorkflowStep *GetGrp() { return itsGrp; };
   WorkflowStep *GetGrpNext() { return itsGrpNext; };
   WorkflowStep *BaseStep() { return itsBaseStep; };

   const CviStr &GetTitle() { return itsTitle; };
   const CviStr &GetMember() { return itsMember; };
   const CviStr &GetName() { return itsName; };
   const CviStr &GetGroup() { return itsGroup; };
   const CviStr &GetDesc() { return itsDescription; };
   const CviStr &GetContent() { return itsContent; };
   const CviStr &GetInstructions() { return itsInstructions; };
   const CviStr &GetType() { return itsType; };
   const CviStr &GetDynamic() { return itsDynamic; };
   const CviStr &GetOrigContent() { return itsOrigContent; };

   void SetLocked() { itsLocked = true; };
   int  Locked() { return itsLocked; };

   int  IsWritten() { return itsWritten; };
   void SetWritten() { itsWritten = 1; };
   void UnsetWritten() { itsWritten = 0; };
   int  IsReal() { return itsRealStep; };

   int  Bypassed(bool theDeep = false) 
   { 
       if (!theDeep)
          return itsBypass;

       bool aRc = false;

       // Group may not be marked as bypassed, but all of its children might be.
       // That makes it effectively bypassed.

       if (strcmp(itsType, "GROUP"))   // not group
           aRc = itsBypass;            // use bypass flag
       else                            // if group
       if (itsGrpOrderChild == NULL)   // if no children
           aRc = true;                 // it is as good as bypassed

       WorkflowStep *aStep = itsGrpOrderChild;

       while (!aRc && aStep != NULL)
       {
           aRc = aStep->Bypassed(theDeep);
           aStep = aStep->itsOrderChild; // get sibling
       }

       return(aRc);
   };

   void SetBypass() { itsBypass = 1; };

   int  Optional() { return itsOptional; };

   int  HasOptional() { return itsHasOptional; };
   void SetHasOptional() { itsHasOptional = true; };

   int  IgnoreDeps() { return itsIgnoreDeps; };

   void  SetContent(const char *theContent) { itsContent.Set(theContent); };

   char ProcType() { return itsProcType; };

   char GetTarget() { return itsTarget; };

   void AddDepends(const CviStr &);    // add dependencies
   void AddPreReqs(const CviStr &);    // add prerequisites
   void RemDepends(const CviStr &);    // remove dependency
   void RemPreReqs(const CviStr &);    // remove dependency

   void AddToBaseDeps(WorkflowStep *); // add base deps

   const CviStr &GetDeps() { return itsDepends; };
   const CviStr &GetPreReqs() { return itsPreReqs; };

   void AddDescendents(const CviStr &); // add descendents
   void AddReqDescendents(const CviStr &); // add required descendents
   const CviStr &GetDescendents() { return itsDescendents; };
   const CviStr &GetReqDescendents() { return itsReqDescendents; };

   WorkflowStep *FindSoloChild();   // find solo child for group collapse

   const char * AddUNIXDSNPrompt(VariableList *thePropVars,
                                 Variable *theDSNVar);

   void MarkTemplate                   // mark template member as used
            (const char *theMember);   // for this step

   const char *GetTemplates() { return (const char *) itsTemplates; };

};

//****************************************************************************
//
// Class        : BuildWorkflow
//
// Description  : Build workflow
//
//****************************************************************************

class   JSON_Entry;

class   BuildWorkflow : public CviPgm

{

protected :                            // protected members
                 
  fileds *itsTempDSN;                  // template fileds
  filestream *itsTempStream;           // template stream

  WorkflowStep *itsSteps;              // workflow steps
  WorkflowStep *itsLastStep;           // last workflow step
  WorkflowStep *itsGroups;             // groups
  WorkflowStep *itsIncludes;           // content includes
  WorkflowStep *itsMacros;             // macros
  WorkflowStep *itsWfMacros;           // workflow macros
  WorkflowStep *itsPopSteps;           // population steps
  WorkflowStep *itsDsnSteps;           // dsn steps
  WorkflowStep *itsPrompts;            // workflow prompts

  VariableList itsWfVars;              // workflow variables
  VariableList itsProdVars;            // product variables

  VariableList *itsPropVars;           // properties file variables

  Variable  *itsAddTargets;            // targets to add/merge

  CviStr itsPropertiesDsn;             // properties filename

  CviStr itsMasterXML;                 // master XML
  CviStr itsStepNames;                 // list of step names for dupe checking

  int    itsTempCount;                 // template count
  int    itsRealCount;                 // real template count (not includes, macros, etc.)

  bool   itsReRun;                     // re-run mode

protected:                             // protected members
     
  void  Reset();                       // reset
  
  int   CreateStep(CviStr &,           // create new step
                   CviStr &theName); 

  int   BuildStep(CviStr &theMem,      // build step from member
                  CviStr &theDSN);     // DSN to use

  int   AddStep(WorkflowStep **,       // add step
                WorkflowStep *);       // to list, in order

  int   AssignGroups();                // assign members to group records

  int   OrderSteps(WorkflowStep *theStep = NULL); // order the steps

  int   CreatePopArray(const char *theScope = NULL); // create population array
  
  int   ParseTemplates(WorkflowStep *aList);

  int   ParseTemplates();              // parse templates

  int   ProcessDSN();                  // process DSN steps

  int   ProcessPopulate(const char *thePrefix = ""); // process populate steps

  int   SizePopulate();                // size population entries

  int   ProcessPromptGroup(JSON_Entry *);

  int   ProcessPrompts();              // process prompt templates

  int   CreatePrompt(CviStr &);        // create prompt data from block data

  int   ProcessMacros();               // process macros  

  int   ProcessStatements();           // process statements

  int   AdjustGroupDepends();          // adjust group dependencies

  int   GenerateWF(const char *theOut, // generate workflow XML
                   WorkflowStep *theGroup = NULL);

  int   GenerateDepWFs();              // generate deployment workflow XML members

  int   GenerateReRun();               // generate items from re-run

  void  AddRowVars(Variable *theVar);  // add row variables

  void  AddRowVars();                  // add row variables

  int   MergeSetVariables();           // generate vars from sets

  int   GenStdProperties();            // generate standard properties

  int   ReadOverrideProperties(bool theReRead = false); // read override property file

  int   ReadProperties(const char *,   // read properties file
                       bool theReplace = false,
                       bool theSilent = false);

  int   ReadTemplates();               // read templates

  int   DisplaySteps();                // display steps

  int   ProcessShared();               // process shared properties/templates

  int   ResolveUnixDSNs();             // resolve UNIX datasets

  int   CleanupJCLOUT();               // cleanup JCL out

  bool  GetTracks(const char *thePrefix, // get track data, if it exists
                  const char *theDSN,
                  long &theTracks);

  int   WalkTree(WorkflowStep *theStep, // walk and display tree
                 int theDepth = 0,
                 int theOrder = 0,
                 int theDeps = 0);

  void  WalkDeps(WorkflowStep *theStep,  // walk dependencies
                 CviStr &theCircular,    // circular detection output
                 int theLevel = 1);

  int   ProcessDatasetList             // process dataset
            (WorkflowStep *);          // list

  virtual int Work(void);              // do work

  virtual int Init();                  // initialize program

public    :                            // public stuff

  BuildWorkflow(int,                   // constructor
                const char **,
                bool);                 // re-run flag
                                       
  int   RemoveMissingPrereqs();        // remove missing prerequisites

  virtual ~BuildWorkflow();            // destructor

};

#endif                                 // reinclude check

