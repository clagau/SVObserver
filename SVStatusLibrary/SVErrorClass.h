//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVError
//* .File Name       : $Workfile:   SVErrorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:41:48  $
//******************************************************************************

#ifndef SVERRORCLASS_H
#define SVERRORCLASS_H

#include "SVUtilityLibrary/SVString.h"

#define SV_ERROR_CONDITION     0xc0000000

/* lastErrorAction VALUES --------------------------------------------*/
/* The following predefined error actions have been set up so that    */
/* these actions can be generically set by TrapError ().              */
/* Inter dependencies.                                                */
/*    If DISPLAY is set, then either                                  */
/*       TIME_DURATION,                                               */
/*       ACKNOWLEDGE,                                                 */
/*       PASSWORD or                                                  */
/*       EVENT_DURATION must be set.                                  */
/*                                                                    */
/*    If MULTIPLE is set, then                                        */
/*       DISPLAY must be set.                                         */

/* NO_ACTION ---------------------------------------------------------*/
/* This value indicates that no resulting action is required. This    */
/* usually results from the program determining that in this          */
/* instance, the passed in error code was not an error condition.     */
#define NO_ACTION                  0

/* TERMINATE ---------------------------------------------------------*/
/* This flag indicates that an unrecoverable error has occurred. The  */
/* programmer should go off line, and not allow the operator to enter */
/* any further functionality (until perhaps a password is entered     */
/* (maybe a function called ClearTerminateFlag ()).  GO_OFFLINE is not*/
/* required when using TERMINATE.  This will happen automatically.    */
#define TERMINATE                  1

/* TIME_DURATION ----------------------------------------------------*/
/* To use this value, DISPLAY must be set. This flag indicates that  */
/* the error message will be displayed for a preset duration, after  */
/* which time the message will go away.                              */
#define TIME_DURATION              2

/* DISPLAY ----------------------------------------------------------*/
/* This indicates that an error message should be displayed.         */
#define DISPLAY                    4

/* ACKNOWLEDGE ------------------------------------------------------*/
/* To use this value, DISPLAY must be set. This flag indicates that  */
/* the error message should be displayed until acknowledged.         */
#define ACKNOWLEDGE                8

/* EVENT_DURATION ---------------------------------------------------*/
/* To use this value, DISPLAY must be set. This flag indicates that  */
/* the error message should be displayed until the condition         */
/* causing the error is removed. No specific implementation of this  */
/* case is being defined at this time.                               */
#define EVENT_DURATION            16

/* PASSWORD ---------------------------------------------------------*/
/* To use this value, DISPLAY must be set. Like ACKNOWLEDGE, the     */
/* operator will be required to acknowledge the error message,       */
/* however a password will be required.                              */
#define PASSWORD                  64

/* LOG --------------------------------------------------------------*/
/* When this flag is set, the error condition will be logged to      */
/* file. Actual logging is done using NT's event logging API         */
/* features.  Although eventually we may need to create our own      */
/* event viewer, for now we will settle for the WINNT event viewer.  */
/* Event logs can be created using any filename, but the NT event    */
/* event viewer can only read NT's predefined System, Security, and  */
/* Application event log files. So until we develop our own event    */
/* viewer, we will use the Application event log file.               */
#define LOG                      256

/* BRK --------------------------------------------------------------*/
/* The use of this value is to signal to the calling function that   */
/* it should exit processing of the a current loop structure (do ,   */
/* while, for, or even switch) by calling the break statement. Since */
/* the setting and testing of this value will usually be done by the */
/* same programmer, there is a lot of room for flexibility.          */
#define BRK                      512

/* SET_OUTPUTS ------------------------------------------------------*/
/* The purpose of this value is to set output bit(s) when an error   */
/* condition occurs. The values which this flag signals to be set    */
/* should be indirect so that the operator can choose whether they   */
/* will actually be tied to outputs or not.  A lot more fleshing out */
/* needs done here.                                                  */
#define SET_OUTPUTS             1024

/* GO_OFFLINE -------------------------------------------------------*/
/* When this bit is set, the program should shut down the inspection */
/* process and leave the Run Mode.  If the Terminate flag is set,    */
/* then this flag does not need to be set.                           */
#define GO_OFFLINE              2048

/* MULTIPLE ---------------------------------------------------------*/
/* To use this value, DISPLAY must be set. This indicates that the   */
/* the associated error message will permit other error messages to  */
/* be displayed, prior to this one being acknowledged (it will not   */
/* block "this" tread). By default, only one error message can be    */
/* displayed at a time. If a message is already being displayed      */
/* which does not have MULTIPLE set, then no other error messages    */
/* may be displayed until that one is acknowledged.                  */
#define MULTIPLE                4096
/* End of lastErrorAction VALUES. -----------------------------------*/

/* FACILITY VALUES --------------------------------------------------*/
#define MICROSOFT_CPP              1
#define BTRIEVE                    2
#define C_SCAPE                    3
#define AB_6008                    4
#define SVR                        5
#define AB_DHPLUS                  6
#define DOS                        7
#define CVIM2_RSP                  8
#define WIN32_API                 10
#define SVFocus                   11
/* End of FACILITY VALUES. ------------------------------------------*/


/*- MACROS -----------------------------------------------------------*/

/*- SV_TRAP_ERROR is meant to simplify calls to TrapError () so that  */
/*- the programmer does not need to enter the __FILE__ and __LINE__   */
/*- parameters.  The operator does, however, need to enter the local  */
/*- instantiation of the Error_Class.                                  */
#define SV_TRAP_ERROR(err, pC) err.TrapError(pC , __FILE__, __LINE__)

/*- SV_TRAP_ERROR_BRK must be used within a loop of some sort and     */
/*- automatically handles testing the return value for the BRK error  */
/*- error action.                                                     */
#define SV_TRAP_ERROR_BRK(err, pC) \
{ \
   if (err.TrapError (pC , __FILE__, __LINE__) & (BRK | TERMINATE)) \
      break; \
}

/*- SV_TRAP_ERROR_BRK_TSTFIRST works the same as SV_TRAP_ERROR_BRK,   */
/*- but does a pre-test of the errorCode to determine if an error     */
/*- condition exists before actually calling the TrapError function.  */
/*- This exists as a possible option for high speed applications      */
/*- attempting to reduce the number of function calls.  This is the   */
/*- same test that would be done within TrapError ().                 */
#define SV_TRAP_ERROR_BRK_TSTFIRST(err, pC) \
{ \
   if (err.msvlErrorCd & SV_ERROR_CONDITION) \
   { \
      if (err.TrapError (pC, __FILE__, __LINE__) & (BRK | TERMINATE)) \
         break; \
   } \
}
/*- End of MACROS. --------------------------------------------------*/

struct SVLogExtention
{
    time_t          timeAcknowledged;
    unsigned long   errorAction;
    unsigned long   securityLevel;      // Required security to
                                        //  acknowledge error.
    unsigned long   programCd;
    unsigned long   errorCd;
    unsigned long   lineNbr;
    unsigned long   setOutputs;
    unsigned long   errorDataOffset;
    unsigned long   errorDataLength;
    unsigned long   fileNameOffset;
    unsigned long   fileNameLength;
    char            filler [16];
};

struct SVLog
{
    _EVENTLOGRECORD NTDef;

    char            buffer [4096];      //    TCHAR SourceName[]
                                        //    TCHAR Computername[]
                                        //    SID   UserSid 
                                        //    TCHAR Strings[]     
                                        //    BYTE  Data[]     
                                        //    CHAR  Pad[] 
                                        //    SVLogExtention  SVDef
                                        //    TCHAR fileName []
                                        //    DWORD Length
};

/**
@SVObjectName Error

@SVObjectOverview Multiple instances of Error_Class may be created and used at the programmers option, however a single instance of Error_Class should NEVER be used across multiple threads. The static members of Error_Class will serve to keep error handling syncronized.

Error codes:  All functions written by SVR should return an error code of the form:

  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +---+-+-+-----------------------+-------------------------------+
 |SEV|C|R|     Facility          |               Code            |
 +---+-+-+-----------------------+-------------------------------+

where

SEV - is the severity code,
	00 - success 
	01 - information 
	10 - warning
	11 - error

C - is the customer code flag; all error codes processed by TrapError () will set this bit.

R - is a reserved bit (Microsoft), Facility is the facility code (?); until someone indicates a different use for this, this will contain an Seidenader Vision Inc. defined value indicating the source of the error code (i.e. Microsoft C++, SVR, WIN32, Btrieve, etc.).  Code is the returned error/status code.

The TrapError () function will use and manipulate these values in the following way.

SEV - This value must be set by the called function.  If either of the two SEV bits are non-zero, then ErrorTrap () will process the case. Otherwise, ErrorTrap () will simply return with ErrorAction set to zero (NO_ACTION).  In order for ErrorTrap () to process the condition, one of these two bits must be set, however during processing, TrapError () may still manipulate these bits on a case by case basis.

C - If TrapError () processes the error case, then this bit will be set.

R - This bit will not be cleared or set by TrapError () (accept for the "negative value" case).

Facility - This value will be set on a case by case basis. If TrapError () does set this value it will indicate the library returning the error code. 

Code - This value will be set on a case by case basis.

For possible ease of programming, TrapError () will accept a negative "error code" as an error case. SEV will be set and get the condition in to be processed.  Once the condition is being processed, TrapError () will test the entire error code to see if the value is between -1 and -65535. If so, TrapError will clear the R value, clear the Facility value, set Code to -errorCode, and then continue on the process the specific case.

@SVObjectOperations

*/

class SVErrorClass
{
public:
   SVErrorClass ();

/*- LOCAL VARIABLE -------------------------------------------------*/

    unsigned long  msvlErrorCd;
    unsigned long  msvlProgramCd; // might not be needed.
    SVString        msvErrorData;
    SVString        msvParam1;
    SVString        msvParam2;
	bool           m_bDisplayError;	// Used to prevent calling the dialog 
								// if not in a tool adjustment dialog.

/*- TrapError () ---------------------------------------------------*/
/*- The programmer should normally call one of the following macros */
/*-    SV_TRAP_ERROR (),                                            */
/*-    SV_TRAP_ERROR_BRK (), or                                     */
/*-    SV_TRAP_ERROR_BRK_TSTFIRST ()                                */
/*- instead of calling TrapError () directly.                       */

/*- TrapError () will examine the SEV code of the errorCode. If the */
/*- value is zero, TrapError will simply return without executing   */
/*- further. If the value is not zero, then the function will       */
/*- process a case statement, based on the programCode, to analyze  */
/*- the data and give the correct response.                         */
/*- ErrorTrap () will clear errorCode after each error condition    */
/*- that is processed.                                              */
    unsigned long TrapError (unsigned long programCd,
                             LPCTSTR fileName,
                             long lineNbr);

/*- DisplayError () ------------------------------------------------*/
/*- This function is local for displaying errors, however this      */
/*- will coordinate with static (global) semaphores so that only    */
/*- error is displayed at a time.                                   */
    unsigned long DisplayError ();
/*- LogError () ---------------------------------------------------*/
/*- This function is local for displaying errors, however this      */
/*- will coordinate with static (global) semaphores so that only    */
/*- error is displayed at a time.                                   */
    unsigned long LogError ();     //Use Event logging

    long          GetComputerName (TCHAR *computerName);
    int           Next32Boundry (int px_oldIndex);
    int           GetSID (PSID px_PSIDPtr, int px_SIDBufferLength);

    int           ClearLastErrorCd ();
    unsigned long GetLastErrorCd () { return msvlLastErrorCd;}
    unsigned long GetLastErrorNbr () { return (msvlLastErrorCd & 0x0000ffff);}
    unsigned long GetLastProgramCd () { return msvlLastProgramCd;}
    unsigned long GetLastErrorAction () { return msvlLastErrorAction;}

protected:
/*- LAST VALUES ----------------------------------------------------*/
/*- Once an error has been processed, values specific to that error */
/*- are placed in variables which are set on a case by case basis   */
/*- and are prefixed with the "last" nomenclature.                  */
    unsigned long   msvlLastErrorCd;
    unsigned long   msvlLastProgramCd;
    unsigned long   msvlLastErrorAction;
    unsigned long   msvlLastDuration;
    unsigned long   msvlLastOutputs;
    unsigned long   msvlLastLineNbr;
    unsigned long   msvlLastFacillity;
    unsigned long   msvlLastMessageNbr;
    unsigned long   msvlLastSeverity;
    unsigned long   msvlLastNbrParams;
    unsigned long   msvlLastSecurityLevel;
    time_t          msvLastErrorTime;
    time_t          msvLastAcknowledgeTime;
    SVString         msvLastFileName;
    SVString         msvLastErrorData;
    SVString         msvLastParam1;
    SVString         msvLastParam2;
/*- End of LAST VALUES. --------------------------------------------*/

/*- STATIC VARIABLES -----------------------------------------------*/
    static unsigned long  msvlStaticErrorCd;
    static unsigned long  msvlStaticProgramCd;
    static unsigned long  msvlSstaticLineNbr;
    static unsigned long  msvlStaticErrorAction;
    static unsigned long  msvlStaticDuration;
    static unsigned long  msvlStaticOutputs;

    int		Err000_009 (int *found);    // not assigned.
    int     Err010_049 (int *found);    // Jim
    int     Err050_099 (int *found);    // Steve
    int     Err100_149 (int *found);    // Robert
    int     Err150_199 (int *found);    // SJones
    int     Err200_249 (int *found);    // Nick
    int     Err250_299 (int *found);
    int     Err300_349 (int *found);

};

#endif

