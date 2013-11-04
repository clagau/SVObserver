// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRSLinxABPLCBaseClass
// * .File Name       : $Workfile:   SVRSLinxABPLCBaseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   13 May 2013 11:21:02  $
// ******************************************************************************

#include "stdafx.h"
#include "SVRSLinxABPLCBaseClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "plc_interface.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVRSLinxABPLCBaseClass::SVRSLinxABPLCBaseClass(long* aplErr):
SVABPLCBaseClass(aplErr)
{
   long     lNbrUnused;

   *aplErr = 0;

   while (1)
   {
      m_ConnectionData.svmPLCManufacturer = SV_PLCMANUFACTURER_ALLENBRADLEY;
      svmlDriverId = 0;
      strcpy (svmczNonUCDriverName, "");

      svmDriverIdListLock.Lock ();
      lNbrUnused = msvUnusedIds.GetSize (); 
      TRACE2 ("ID in use = %d, nbr open = %d\n", svmlDriverId, svmlNbrOpen);
      svmDriverIdListLock.Unlock ();

      break;
   }

}

SVRSLinxABPLCBaseClass::~SVRSLinxABPLCBaseClass()
{

}


long SVRSLinxABPLCBaseClass::DeinitializeBlocks (SVPLCBlockStruct* apBlock)
{
   long              lErr;
//   SVPLCBlockStruct* pBlock;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = DTL_UNDEF (apBlock->svmlTransactionId);
      }

      SVPLCBaseClass::DeinitializeBlocks (apBlock);
      break;
   }

   return lErr;
}


long SVRSLinxABPLCBaseClass::CheckStatus (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (apBlock->svmStatus != SV_PLCCOMMSTATUS_PENDING)
      {
//------ svmStatus will only be changed by the raw data during the pending
//------ state.
         break;
      }

      if (svmhRSLinxLib == NULL)
      {
//------ If the RSLinx DLL is not loaded, then we will fake the return of the
//------ DTL_SUCCESS.
         apBlock->svmulRawStatus = DTL_SUCCESS;
      }

      switch (apBlock->svmulRawStatus)
      {
      case 0xffff:
         {
//--------- Still pending.

            break;
         }
      case DTL_SUCCESS:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ALMOSTCOMPLETE;
            break;
         }
      case DTL_E_R_ONLY:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1159;
            break;
         }
      case DTL_E_INVTYPE:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1160;
            break;
         }
      case DTL_E_BADID:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1161;
            break;
         }
      case DTL_E_NOSUPPORT:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1162;
            break;
         }
      case DTL_E_TOOBIG:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1163;
            break;
         }
      case DTL_E_NODEF:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1164;
            break;
         }
      case DTL_E_CNVT:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1165;
            break;
         }
      case DTL_E_INVDEF:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1166;
            break;
         }

//--- 258; HEX 102.  From DTL.h: Cannot guarantee delivery, link layer timed 
//--- out or received a NAK.
      case 258:
         {
//--------- Possible disconnected cable. When SVFocus automatically starts --
//--------- RSLinx, there are conditions which causes this error because 
//--------- RSLinx doesn't start fast enough; 
//---------  SVFocus as system shell, 
//---------  Running from IDE debugger. -------------------------------------
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1329;
            break;
         }

//--- 260; HEX 104.  From DTL.h:  Local port is disconnected.
//--- We have gotten this error when calling a read function imediatedly 
//--- after calling DTL_INIT ().  Apparently the read is occuring before
//--- RSLinx has made all its connections.
      case 260:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1346;
            break;
         }
//--- 272; HEX 110.  This has been shown to occur when the PLC address is not
//--- defined.
      case 272:  
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1370;
            break;
         }

      default:
         {
CString t;
t.Format (_T("Check status, rawstatus:  %d"), apBlock->svmulRawStatus);
AfxMessageBox (t);

            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1167;
            break;
         }
      }
      break;
   }

   if (lErr < 0)
      return lErr;
   else 
      return apBlock->svmStatus;
}


long SVRSLinxABPLCBaseClass::Open (TCHAR *apConnectionString)
{
   long  lCount;
   long  lDriverFound;
   long  lErr;

   SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

   svmNbrOpenLock.Lock ();

   while (1)
   {
      if (svmlIsOpen == TRUE)
      {
         lErr = 1;
         break;
      }

      lErr = SVPLCBaseClass::Open ();
      if (lErr < 0)
         break;


      if (svmhRSLinxLib)
      {
         if (svmlNbrOpen == 1)
         {
//------ If the RSLinx DLL is loaded, then we can call the RSLinx function.
            lErr = -DTL_INIT (600);  // 641 = maximum number of data items
         }
      }
      
      if (lErr < 0)
         break;

      if (svmhRSLinxLib)
      {
		 double l_ElaspedTime = SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ) );
//------ If the RSLinx DLL is loaded, then we can call the RSLinx function.
         lErr = -DTL_DRIVER_LIST (svmDtlDrivers, 
                                  (unsigned long *) &svmlNbrDrivers, 
                                  (unsigned long ) (svmTimeOutLimit - l_ElaspedTime ));
         if (lErr < 0)
            break;

         if (svmlNbrDrivers == 0)
         {
            lErr = -1183;
            break;
         }

         lDriverFound = FALSE;
         for (lCount = 0; lCount < svmlNbrDrivers; lCount++)
         {
            if (!_stricmp (svmDtlDrivers [lCount].szDriverName,
                          svmczNonUCDriverName))
            {
               lDriverFound = TRUE;
               break;
            }
         }

      }
      else
      {
//------ If the RSLinx DLL is not loaded, then indicate that the driver was 
//------ found.
         lDriverFound = TRUE;
      }


      if (lDriverFound == FALSE)
      {
         lErr = -1315;
         break;
      }

      if (svmhRSLinxLib)
      {
		  double l_ElaspedTime = SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ) );
//------ If the RSLinx DLL is loaded, then we can call the RSLinx function.
         lErr = -DTL_DRIVER_OPEN (svmlDriverId, 
                                  svmczNonUCDriverName, 
                                  (unsigned long) (svmTimeOutLimit - l_ElaspedTime));

		   TRACE2 ("SVSLC500BaseClass::Open; driver id = %d, err = %d\n", svmlDriverId, lErr);
      }
 
      if (lErr < 0)
         break;

//--- TEST TO SEE IF RSLINX CAN GET CONNECTION ------------------------------
      SVPLCElementStruct   address;
      long                 lValue;

	   SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

//--- n100:5 is an arbitrary value.  Could be any integer address
      _stprintf (address.svmPLCAddress, _T("N7:0"));

      long lCounter;
      lCounter = 0;
      do 
      {
         lCounter = lCounter + 1;
         lErr = GetInt (&lValue, &address);

         if ((lErr == -1346) && ( 5000 < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ) ) ) )
         {
            lErr = -1347;
            break;
         }
         TRACE2 ("counter = %d, err = %d\n", lCounter, lErr);

      } while (lErr == -1346);   // -1346 indicates that local connection could 
                                 // not be established.

      if ((lErr == -1347) ||     // Time Out.
          (lErr == -20) ||       // RSLinx: Bad "define" id. RSLinks error.
          (lErr == -7) ||        // RSLinx: Invalid Remote Station Address.
          (lErr == -1329))       // Possible disconnected cable.
      {
         break;
      }
      else
      {
         lErr = 0;
      }

//--- End of TEST TO SEE IF RSLINX CAN GET CONNECTION. ----------------------

      svmlIsOpen = TRUE;
      break;
   }

   if (svmlIsOpen == FALSE)
   {
      svmlNbrOpen = svmlNbrOpen - 1;
   }

   svmNbrOpenLock.Unlock ();
   return lErr;
}

long SVRSLinxABPLCBaseClass::Close ()
{
   long  lErr;

   lErr = 0;

   svmNbrOpenLock.Lock ();
   while (1)
   {
      if (svmlIsOpen == FALSE)
      {
         lErr = 1;
         break;
      }

      if (svmhRSLinxLib)
      {
         if (svmlNbrOpen == 1)
         {
            DTL_UNINIT (DTL_E_FAIL);
         }
      }

      SVPLCBaseClass::Close ();
      break;
   }

   svmNbrOpenLock.Unlock ();
   return lErr;
}


long SVRSLinxABPLCBaseClass::SendB (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_WRITE_CB (apBlock->svmlTransactionId,
                               (unsigned char *) apBlock->svmcpTxRxBuffer,
                               (unsigned long) svmTimeOutLimit,
                               SolCallback,
                               (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }
   return lErr;
}



long SVRSLinxABPLCBaseClass::SendInt16 (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_WRITE_CB (apBlock->svmlTransactionId,
                               (unsigned char *) apBlock->svmcpTxRxBuffer,
                               (unsigned long) svmTimeOutLimit,
                               SolCallback,
                               (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }
   return lErr;
}

long SVRSLinxABPLCBaseClass::SendStr (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_WRITE_CB (apBlock->svmlTransactionId,
                               (unsigned char *) apBlock->svmcpTxRxBuffer,
                               (unsigned long) svmTimeOutLimit,
                               SolCallback,
                               (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }
   return lErr;
}



long SVRSLinxABPLCBaseClass::SendSPFloat (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_WRITE_CB (apBlock->svmlTransactionId,
                               (unsigned char *) apBlock->svmcpTxRxBuffer,
                               (unsigned long) svmTimeOutLimit,
                               SolCallback,
                               (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;         
   }
   return lErr;
}



long SVRSLinxABPLCBaseClass::GetB (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_READ_CB (apBlock->svmlTransactionId,
                              (unsigned char *) apBlock->svmcpTxRxBuffer,
                              (unsigned long) svmTimeOutLimit,
                              SolCallback,
                              (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



long SVRSLinxABPLCBaseClass::GetInt16 (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_READ_CB (apBlock->svmlTransactionId,
                              (unsigned char *) apBlock->svmcpTxRxBuffer,
                              (unsigned long) svmTimeOutLimit,
                              SolCallback,
                              (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}


long SVRSLinxABPLCBaseClass::GetStr (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_READ_CB (apBlock->svmlTransactionId,
                              (unsigned char *) apBlock->svmcpTxRxBuffer,
                              (unsigned long) svmTimeOutLimit,
                              SolCallback,
                              (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}


long SVRSLinxABPLCBaseClass::GetSPFloat (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
      if (svmhRSLinxLib)
      {
         lErr = -DTL_READ_CB (apBlock->svmlTransactionId,
                              (unsigned char *) apBlock->svmcpTxRxBuffer,
                              (unsigned long) svmTimeOutLimit,
                              SolCallback,
                              (unsigned long) apBlock);
      }
                   
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVRSLinxABPLCBaseClass.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 11:21:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 May 2013 13:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:36:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 13:29:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:04:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/