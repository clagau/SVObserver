//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVErrorClass
//* .File Name       : $Workfile:   SVErrorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:08:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"
#include "SVMessage/SVMessage.h"
#include "MessageContainer.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

SVErrorClass::SVErrorClass ()
{
   msvlErrorCd = 0;
   msvlProgramCd = 0;; // might not be needed.

   msvlLastErrorCd = 0;
   msvlLastProgramCd = 0;
   msvlLastErrorAction = NO_ACTION;
   msvlLastDuration = 0;
   msvlLastOutputs = 0;
   msvlLastLineNbr = 0;
   msvlLastFacillity = 0;
   msvlLastMessageNbr = 0;
   msvlLastSeverity = 0;
   msvlLastNbrParams = 0;
   msvlLastSecurityLevel = 0;
   m_bDisplayError = false;

}

/*- TrapError () ---------------------------------------------------*/
/*- TrapError () will examine the SEV code of the errorCode. If the */
/*- value is zero, TrapError will simply return without executing   */
/*- further. If the value is not zero, then the function will       */
/*- process a case statement, based on the programCode, to analyze  */
/*- the data and give the correct response.                         */
/*- ErrorTrap () will clear errorCode after each error condition    */
/*- that is processed. ---------------------------------------------*/
unsigned long SVErrorClass::TrapError (unsigned long programCd,
                                       LPCTSTR      fileName,
                                       long          lineNbr)
{
   int              lx_found;

   //int              lx_err;
   //int              lx_err_save;
   //int              lx_err_ackupdate;
   //unsigned long    lx_tmp_time;
   SVString          lx_displayString;

   while (1)
   {
/*--- QUICK RETURN. -------------------------------------------------------*/
/*--- If the SEV portion of the errorCd is not set, then it is not         */
/*--- recognized as an error. ---------------------------------------------*/
      if (!(msvlErrorCd & SV_ERROR_CONDITION)) 
      {
         return NO_ACTION;
      }
/*--- End of QUICK RETURN. ------------------------------------------------*/

/*--- RECONFIGURING SHORTHAND ERROR CODES ---------------------------------*/
/*--- Setting the error codes to negative is the short hand meathod of     */
/*--- setting the "severity" bits. ----------------------------------------*/
      if ((msvlErrorCd & 0xffff0000) == 0xffff0000)
      {
         msvlErrorCd = (-((signed long) msvlErrorCd)) | SV_ERROR_CONDITION;
//         x.Format ("2error nbr = %x\n", msvlErrorCd);
//AfxMessageBox (x);

      }
/*--- End RECONFIGURING SHORTHAND ERROR CODES. ----------------------------*/

      msvlLastProgramCd = programCd;
      msvlLastErrorCd = msvlErrorCd;
      msvlLastLineNbr = lineNbr;

// String sizes have not yet been defined.  This will need addressed.

      msvLastFileName = SvUl_SF::Format ("%.30s", fileName);
      msvLastErrorData = SvUl_SF::Format ("%.30s", msvErrorData);
      msvLastParam1 = SvUl_SF::Format ("%.30s", msvParam1);
      msvLastParam2 = SvUl_SF::Format ("%.30s", msvParam2);
      msvlLastNbrParams = 0;        // If params are to be used, then 
                                //  this needs set within the 
                                //  specific case. Default should 
                                //  be zero so most cases don't 
                                //  need to deal with this.
      msvlErrorCd = 0;
      msvErrorData.clear();

      lx_found = TRUE; // It is assumed that the condition is 
                       //  defined. If not then the default 
                       //  condition should set lx_found to 
                       //  NO.

      if (msvlProgramCd < 1000)
      {
/*------ RESERVED ---------------------------------------------------------*/
/*------ Not sure why yet. ------------------------------------------------*/
         Err000_009 (&lx_found);
/*------ End of RESERVED --------------------------------------------------*/
      }
      else
      if (msvlProgramCd < 5000)
      {
         Err010_049 (&lx_found);
      }
      else
      if (msvlProgramCd < 10000)
      {
         Err050_099 (&lx_found);
      }
      else
      if (msvlProgramCd < 15000)
      {
         Err100_149 (&lx_found);
      }
      else
      if (msvlProgramCd < 20000)
      {
         Err150_199 (&lx_found);
      }
      else
      if (msvlProgramCd < 25000)
      {
         Err200_249 (&lx_found);
      }
      else
      if (msvlProgramCd < 30000)
      {
         Err250_299 (&lx_found);
      }
      else
      if (msvlProgramCd < 35000)
      {
         Err300_349 (&lx_found);
      }
      else
      {
/*------ DEFAULT ----------------------------------------------------------*/
         lx_found = FALSE;
/*------ End of DEFAULT. --------------------------------------------------*/
      }

/*--- This should only be executed if the programCd value could not be     */
/*--- found.                                                               */
      if (lx_found == FALSE)
      {
			msvLastErrorData = SvUl_SF::Format ("Prog.Cd = %d", msvlLastProgramCd);
         msvlLastProgramCd = 999;
         msvlLastFacillity = SVR;
         msvlLastMessageNbr = 0000;
         msvlLastSeverity = ERROR;
         msvlLastErrorAction = TERMINATE +
                               DISPLAY +
                               ACKNOWLEDGE +
                               LOG;
      }

      if( !m_bDisplayError && (msvlLastErrorAction & DISPLAY) )
	  {
		  msvlLastErrorAction &= ~DISPLAY;
		  msvlLastErrorAction |= LOG;
	  }


      if (msvlLastErrorAction & (TERMINATE | GO_OFFLINE))
      {
/*------- START GO OFFLINE -------------------------------------------------*/

/*------- End of START GO OFFLINE. -----------------------------------------*/
      }

      if (msvlLastErrorAction & DISPLAY)
      {
/*------- DISPLAY ERROR MESSAGE --------------------------------------------*/
			lx_displayString = SvUl_SF::Format("Error Nbr = %d, Program Cd = %d\nFile = %s, Line Nbr = %d\nData = <%s>", 
                                  msvlLastErrorCd & 0x0000ffff,
                                  msvlLastProgramCd,
                                  msvLastFileName.c_str(),
                                  msvlLastLineNbr,
                                  msvLastErrorData.c_str());
	      MessageBox(NULL, lx_displayString.c_str(), "", MB_OK);
/*------- End of DISPLAY ERROR MESSAGE. ------------------------------------*/
      }

      if (msvlLastErrorAction & (ACKNOWLEDGE | PASSWORD))
      {
/*------- ACKNOWLEDGE PASSWORD ---------------------------------------------*/

/*------- End of ACKNOWLEDGE PASSWORD. -------------------------------------*/
      }

      if (msvlLastErrorAction & LOG)
      {
/*------- ACKNOWLEDGE PASSWORD ---------------------------------------------*/
/*------- Does not log SVObserver data yet.                                 */
//          LogError ();
		//************************************************
		// Use SVException to get this information to the event log.
			lx_displayString = SvUl_SF::Format("Error Nbr = %lx, Program Cd = %d\n",
			  msvlLastErrorCd ,
			  programCd  );
		  // log to event log.
			SvStl::MessageContainer svE;
			svE.setMessage (SVMSG_SVO_35_LEGACY_ERROR_TRAP, lx_displayString.c_str(), 
				_T(__DATE__), _T(__TIME__), fileName, lineNbr, _T(__TIMESTAMP__), programCd );
			svE.logMessage();
		//*******************************************************
/*------- End of ACKNOWLEDGE PASSWORD. -------------------------------------*/
      }

      break;
   }

   return (msvlLastErrorAction);

}

/*- End of TrapError (). --------------------------------------------*/


/*- LogError () -----------------------------------------------------*/
/*- LogError () will log the current "last error" inbformation into  */
/*- the event log. --------------------------------------------------*/

/*- This still does not log SVObserver data.                         */

unsigned long SVErrorClass::LogError ()
{
    int             lx_err;
    int             lx_index;
    int             lx_startOfBuffer;
    int             lx_TCHARSize;
    //int             lx_length;

    TCHAR           lx_computerName [512];

    char            lx_sidBuffer [2000];

    PSID            lx_sidPtr;


    SVLogExtention  lx_SVDef;
    SVLog           lx_logRecord;

    lx_err = 0;
    lx_sidPtr = (PSID) lx_sidBuffer;

    while (1)
    {

        lx_logRecord.NTDef.TimeGenerated = static_cast< DWORD >( msvLastErrorTime );
        time ((time_t *) &lx_logRecord.NTDef.TimeWritten);
        lx_logRecord.NTDef.EventID = msvlLastMessageNbr;

        switch( SV_SEVERITY( msvlLastErrorCd ) )
        {
        case SEV_FATAL:
            lx_logRecord.NTDef.EventType = EVENTLOG_ERROR_TYPE;
            break;
        case SEV_WARNING:
            lx_logRecord.NTDef.EventType = EVENTLOG_WARNING_TYPE;
            break;
        case SEV_INFORMATIONAL:
            lx_logRecord.NTDef.EventType = EVENTLOG_INFORMATION_TYPE;
            break;
        }

        lx_logRecord.NTDef.NumStrings = (unsigned short)(msvlLastNbrParams);
        lx_logRecord.NTDef.EventCategory = (unsigned short)(msvlLastFacillity);

        lx_SVDef.errorAction = msvlLastErrorAction;
        lx_SVDef.errorCd = msvlLastErrorCd & 0x0000ffff;
        lx_SVDef.lineNbr = msvlLastLineNbr;
        lx_SVDef.programCd = msvlLastProgramCd;
        lx_SVDef.securityLevel = msvlLastSecurityLevel;
        lx_SVDef.setOutputs = msvlLastOutputs;
        lx_SVDef.timeAcknowledged = msvLastAcknowledgeTime;

        lx_startOfBuffer = sizeof (_EVENTLOGRECORD);

        lx_index = 0;
        lx_TCHARSize = sizeof (TCHAR);

/*----- COPY SOURCE NAME --------------------------------------------*/
/*----- Copy Source Name to buffer, including NULL terminator. ------*/
        lx_index = lx_index +
                   (_stprintf ((TCHAR *)(&lx_logRecord.buffer [lx_index]),
                               _T("SVObserver")) + 1) * 
                   lx_TCHARSize;    
/*----- End of COPY SOURCE NAME. ------------------------------------*/

/*----- COPY COMPUTER NAME ------------------------------------------*/
/*----- Copy Computer Name to buffer, including NULL terminator. ----*/

        lx_err = GetComputerName (lx_computerName);
        if (lx_err < 0)
            break;

        
        lx_index = lx_index +
                   (_stprintf ((TCHAR *)(&lx_logRecord.buffer [lx_index]),
                               lx_computerName) + 1) * 
                   lx_TCHARSize;    

        lx_index = Next32Boundry (lx_index + lx_logRecord.NTDef.UserSidLength);
/*----- End of COMPUTER NAME. ---------------------------------------*/

/*----- COPY SID VALUE ----------------------------------------------*/
        lx_err = GetSID (lx_sidPtr, sizeof (lx_sidBuffer));
        if (lx_err < 0) 
        {
            break;
        }

        lx_logRecord.NTDef.UserSidLength = GetLengthSid (lx_sidPtr);
        lx_logRecord.NTDef.UserSidOffset = lx_index + lx_startOfBuffer;

        if (!CopySid (512,
                      &lx_logRecord.buffer [lx_index], 
                      lx_sidBuffer))
        {
            lx_err = -1062;
            break;
        }
        
/*----- End of COPY SID VALUE. --------------------------------------*/

        lx_logRecord.NTDef.StringOffset = lx_index + 
                                          lx_startOfBuffer;

        lx_index = lx_index + 
                   _stprintf ((_TCHAR *) (&lx_logRecord.buffer [lx_index]),
                              msvParam1.c_str()) *
                   lx_TCHARSize;
        lx_index = lx_index + 
                   _stprintf ((_TCHAR *) (&lx_logRecord.buffer [lx_index]), 
                              msvParam2.c_str()) *
                   lx_TCHARSize;

        lx_index = Next32Boundry (lx_index + 
                                  lx_logRecord.NTDef.UserSidLength);
        lx_logRecord.NTDef.DataOffset = lx_index;
        lx_index = lx_index + 
                   _stprintf ((_TCHAR *) (&lx_logRecord.buffer [lx_index]), 
                              msvErrorData.c_str()) *
                   lx_TCHARSize;

        lx_logRecord.NTDef.DataLength = static_cast<DWORD>(msvErrorData.size());
        
        *((long *) (&lx_logRecord.buffer [lx_index])) = lx_index + 4;

        lx_index = lx_index + 4;

        lx_logRecord.NTDef.Length = lx_index;
        break;
    }

    return lx_err;
}


long SVErrorClass::GetComputerName (TCHAR *computerName)
{
    int             lx_retCd;

    HKEY            lx_regKey;

    long            lx_err;
    unsigned long   lx_type;
    unsigned long   lx_length;

    lx_err = 0;
    while (1)
    {
        lx_retCd = 
            RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                          _T("Software\\Microsoft\\Windows NT\\CurrentVersion"),
                          0,
                          KEY_QUERY_VALUE,
                          &lx_regKey);
        if (lx_retCd != ERROR_SUCCESS)
        {
            lx_err = -1055;
            break;
        }

        lx_retCd = RegQueryValueEx (lx_regKey, 
                                    _T("ComputerName"),
                                    0,
                                    &lx_type,
                                    (BYTE *) computerName,
                                    &lx_length);

        if (lx_retCd != ERROR_SUCCESS)
        {
            lx_err = -1056;
            break;
        }
        
        lx_retCd = RegCloseKey (lx_regKey);

        if (lx_retCd != ERROR_SUCCESS)
        {
            lx_err = -1057;
            break;
        }
        break;
    }

    return lx_err;
}


/*- End of LogError (). ---------------------------------------------*/

/*- GetSID () -------------------------------------------------------*/
int SVErrorClass::GetSID (PSID px_SIDPtr, int px_SIDBufferLength)
{

    HANDLE                  lx_hProcess; 
    HANDLE                  lx_hAccessToken; 

    char                    lx_ucTokInfoBuf [2000];

    unsigned long           lx_actualTokenLength;

    TOKEN_INFORMATION_CLASS lx_ticInfoClass;
    PTOKEN_USER             lx_ptuTokenUser;

    long                    lx_err;

    lx_err = 0;
    sprintf (lx_ucTokInfoBuf, "");

    while (1)
    {
        lx_hProcess = GetCurrentProcess ();
        if (lx_hProcess == 0)
        {
            lx_err = -1058;
            break;
        }

        if (!OpenProcessToken (lx_hProcess, 
                               (TOKEN_READ | TOKEN_QUERY_SOURCE),
                               &lx_hAccessToken)) 
        {
            lx_err = -1059;
            break;
        }

        lx_ticInfoClass = TokenUser;       // Enumerated type.
        if (!GetTokenInformation (lx_hAccessToken,
                                  lx_ticInfoClass,
                                  lx_ucTokInfoBuf,
                                  sizeof (lx_ucTokInfoBuf),
                                  &lx_actualTokenLength))
        {
            lx_err = -1060;
            break;
        }

        lx_ptuTokenUser = (PTOKEN_USER) lx_ucTokInfoBuf;
        if (!CopySid (px_SIDBufferLength, 
                      px_SIDPtr, 
                      lx_ptuTokenUser->User.Sid))
        {
            lx_err = -1061;
            break;
        }

    
        break;
    }

    return lx_err;
}
/*- End of GetSID (). -----------------------------------------------*/

int SVErrorClass::Next32Boundry (int px_oldIndex)
{
    return (((px_oldIndex - 1) / 4) + 1) * 4;
}


int SVErrorClass::ClearLastErrorCd ()
{
   msvlLastErrorCd = 0; 
   return 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\SVErrorClass.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 10:08:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:41:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Mar 2011 07:10:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with string conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2010 08:16:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jun 2007 14:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Oct 2005 14:33:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  515
 * SCR Title:  Fix Bug when trying to edit tools with invalid extents
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified logic to display error only if previous logic displayed error and m_bDisplayError was active.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 2005 15:39:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  515
 * SCR Title:  Fix Bug when trying to edit tools with invalid extents
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added displayError flag so the errors will go to the display.  Made the default for this flag false so the errors go to the event log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Feb 2005 12:21:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new files for SVOMFCLibrary 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   17 Apr 2003 15:51:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   04 Oct 2001 12:58:10   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
