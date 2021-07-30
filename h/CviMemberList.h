//****************************************************************************
// DESCRIPTION
//         Gets a list of members from a library
// 
//****************************************************************************

#ifndef MEMBERLIST_INC                 // reinclude check
#define MEMBERLIST_INC                   

//****************************************************************************
// Includes

#include <stdio.h>                     // needs standard I/O

#include "CviStr.h"                    // needs string

#include "ihapds.h"
          
//         
//****************************************************************************

//****************************************************************************
//
// Class        : Member
//
// Description  : Holds a PDS member name
//
//****************************************************************************
class Member
{

protected:

    CviStr itsName;

    Member *itsNext;

public:
     Member(const char *theName)
     {
         if (theName != NULL)
             itsName.Set(theName);
         itsNext = NULL;
     }

     ~Member()
     {
         if (itsNext != NULL)
             delete itsNext;
     }

     void SetNext(Member *theNext)
     {
         itsNext = theNext;
     }

     Member *GetNext()
     {
         return(itsNext);
     }

     operator const char *() const     // convert to char *
     {
         return (const char *) itsName;
     }

};

       
//****************************************************************************
//
// Class        : MemberList
//
// Description  : Build list of members in a library
//
//****************************************************************************
class   MemberList

{

protected :                            // protected members
                 
    Member  *itsList;                  // member list

    int     itsCount;

public :                               // protected members
     
    MemberList()
    {
        itsList = NULL;
        itsCount = 0;
    }

    ~MemberList()
    {
        if (itsList != NULL)
            delete itsList;
        itsList = NULL;
    }

    bool Failed()
    {
        return(itsCount == -1);
    }

    void Reset()
    {
        if (itsList != NULL)
           delete itsList;
        itsList = NULL;
        itsCount = 0;
    }

    void AddMember(const char *theMem)
    {
        Member *aMem = new Member(theMem);

        if (itsList == NULL)
            itsList = aMem;
        else
        {
            aMem->SetNext(itsList);
            itsList = aMem;
        }
        itsCount ++;
    }

    Member *GetMembers(const char *theDSN)
    {

        CviStr aDSN;

        
        Reset();

        itsCount = -1;                 // assume failure

        if (strstr(theDSN, ".") == NULL)

            aDSN.Print("DD:%s", theDSN);

        else

            aDSN.Print("//'%s'", theDSN);

        #define MAX_ENTRIES 8
        #define ENTRY_SIZE  256

        char anEntry[MAX_ENTRIES][ENTRY_SIZE];

        FILE *aFp = fopen(aDSN, "rb,lrecl=256,noseek");

        //printf("%s open: %p\n",
        //       (const char *) aDSN,
        //       (void *) aFp);

        if (aFp != NULL)               // if valid

            itsCount = 0;              // mark count as good

        while (aFp != NULL &&
               !feof(aFp))
        {

            int aRecs = 
                fread(anEntry[0],
                      ENTRY_SIZE,
                      MAX_ENTRIES,
                      aFp);

            //printf("Read %d\n", aRecs);

            if (aRecs <= 0)
                break;

            int aRecNum = 0;

            while (aRecNum < aRecs)

            {

                int aBlkLen = *(short *)anEntry[aRecNum];

                pds *aPDSEntry = (pds *) (anEntry[aRecNum] + 2);

                while ((void *) aPDSEntry < (void *) (anEntry[aRecNum] + aBlkLen))
                {
                    CviStr aMem(aPDSEntry->pds2name, 8);
                    aMem.Trim(' ');

                    if (!strcmp(aMem, "\xff\xff\xff\xff\xff\xff\xff\xff"))
                    {
                        fclose(aFp);
                        aFp = NULL;
                        break;
                    }
                    else
                    if (strlen(aMem) > 0)
                    {
                        AddMember(aMem);
                    }
                    aPDSEntry = (pds *) (((char *) &aPDSEntry->pds2usrd) + (aPDSEntry->pds2indc & pds2lusr) * 2);
                }

                aRecNum ++;

            }

        }

        //printf("   count %d\n", itsCount);

        if (aFp != NULL)
            fclose(aFp);

        //printf("Closed...\n");

        return itsList;

    }

    operator int() const
    {
        return(itsCount);
    }


};

#endif                                 // reinclude check


