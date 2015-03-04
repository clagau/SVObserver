//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpClass
//* .File Name       : $Workfile:   SVASABtcpClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:36:54  $
//******************************************************************************

#include "stdafx.h"

#include "plc_interface.h"
#include "SVASABtcpClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	// a long time for debugging. The max for ASAB is 32000.
	#define PLC_TRANSACTION_TIME 32000
#else
	#define PLC_TRANSACTION_TIME 2000
#endif

SVASABtcpClass::SVASABtcpClass(long* aplErr):
SVABPLCBaseClass (aplErr)
{

	*aplErr = 0;


	m_ConnectionData.svmPLCManufacturer = SV_PLCMANUFACTURER_ALLENBRADLEY;
	m_ConnectionData.svmPLCModel =        SV_PLCMODEL_SLC500;


}

SVASABtcpClass::~SVASABtcpClass()
{
   OutputDebugString( "Exiting ~SVASABtcpClass\n" );
}



long SVASABtcpClass::CheckStatus (SVPLCBlockStruct* apBlock)
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

      if (!m_ASABtcpControl.IsValid())
      {
//------ If the RSLinx DLL is not loaded, then we will fake the return of the
//------ DTL_SUCCESS.
         apBlock->svmulRawStatus = ASABSuccess;
      }

      switch (apBlock->svmulRawStatus)
      {
      case 0xffff:
         {
//--------- Still pending.

            break;
         }
      case ASABSuccess:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ALMOSTCOMPLETE;
            break;
         }
	  case ASABTransactionTimedout:
		  {
			  apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
				apBlock->svmlCommunicationError = -3105;
			  break;
		  }
      case ASABErorWritingQuery:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3106;
            break;
         }
      case ASABConnectionTimeout:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3107;
            break;
         }
      case ASABBadTagFormat:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3108;
            break;
         }
      case ASABConttrolTerminated:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3109;
            break;
         }
      case ASABInvalidFunctionRequest:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3110;
            break;
         }
      case ASABErrorCreatingSocket:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3111;
            break;
         }
      case ASABErrorCreatingThread:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3112;
            break;
         }
      case ASABDemoHasExpired:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3113;
            break;
         }

      case ASABErrorRequestingSession:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3114;
            break;
         }
      case ASABInvalidResponseLength:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3115;
            break;
         }
      case ASABUnsupoprtedDataType:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3116;
            break;
         }
//--- 258; HEX 102.  From DTL.h: Cannot guarantee delivery, link layer timed 
//--- out or received a NAK.
      case 258:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3117;
            break;
         }

//--- 260; HEX 104.  From DTL.h:  Local port is disconnected.
//--- We have gotten this error when calling a read function imediatedly 
//--- after calling DTL_INIT ().  Apparently the read is occuring before
//--- RSLinx has made all its connections.
      case 260:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3118;
            break;
         }
//--- 272; HEX 110.  This has been shown to occur when the PLC address is not
//--- defined.
      case 272:  
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3119;
            break;
         }

      default:
         {
//			CString t;
//			t.Format (_T("Check status, rawstatus:  %d"), apBlock->svmulRawStatus);
//			AfxMessageBox (t);

            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -3120;
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




long SVASABtcpClass::Open (TCHAR *apConnectionString)
{
   long  lErr=0;

   svmNbrOpenLock.Lock ();

	if (!svmlIsOpen)
	{
		lErr = SVPLCBaseClass::Open();
		if( lErr >= 0 )
		{
			svmTimeOutLimit = PLC_TRANSACTION_TIME + 100;
			lErr = m_ASABtcpControl.Create();
			// If the control is not valid...
			if( m_ASABtcpControl.IsValid() )
			{
				svmlIsOpen = TRUE;
			}
			else
			{
			  KeepPrevError( lErr, -77001);
			}

		}
	}

	svmNbrOpenLock.Unlock ();
	return lErr;
}

long SVASABtcpClass::SetupPLC( LPCTSTR p_strNodeAddress, long p_lConnectTimeOut )
{

	HRESULT l_hr = SetTimeOutLimit( p_lConnectTimeOut + 20 );
	KeepPrevError( l_hr, m_ASABtcpControl.SetupPLC( p_strNodeAddress, p_lConnectTimeOut ));
	return l_hr ;
}


long SVASABtcpClass::Close ()
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

      if( m_ASABtcpControl.IsValid() )
      {
        if (svmlNbrOpen == 1)
         {
			 m_ASABtcpControl.Destroy();
         }
      }

      SVABPLCBaseClass::Close ();
      break;
   }

   svmNbrOpenLock.Unlock ();
   return lErr;
}

long SVASABtcpClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   long  lErr;
   lErr = 0;

   if( !this->m_ASABtcpControl.IsValid() )
   {
	   return -3121;
   }
   else
   {
	   return m_ASABtcpControl.DataTypeSize( aDataType );
   }
   return lErr;
}



long SVASABtcpClass::SendB (SVPLCBlockStruct* apBlock)
{
	long  lErr;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		m_ASABtcpControl.WriteBlock(apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
	               
	return lErr;
}



long SVASABtcpClass::SendInt16 (SVPLCBlockStruct* apBlock)
{
	long  lErr;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		m_ASABtcpControl.WriteBlock(apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
	               
	return lErr;
}

long SVASABtcpClass::SendInt32 (SVPLCBlockStruct* apBlock)
{
	long  lErr;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		m_ASABtcpControl.WriteBlock(apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
	               
	return lErr;
}


long SVASABtcpClass::SendStr (SVPLCBlockStruct* apBlock)
{
	long  lErr;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		m_ASABtcpControl.WriteBlock(apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
	               
	return lErr;
}


long SVASABtcpClass::CopyStringToSendBuffer (SVPLCBlockStruct* apBlock)

{
   long     lErr;
   long     lLoop;
   long     lTransmitBufferOffset;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   TCHAR*   ptczValueBuffer;

   lErr = 0;

   while  (1)
   {
      for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
      {
                  
         switch (apBlock->svmPLCDataType)
         {
         case SV_PLC_BIT:
         case SV_PLC_INT16:
            {

//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;
               long     lValueBufferOffset;

               pczTransmitBuffer = (char*) apBlock->svmcpTxRxBuffer;
               ptczValueBuffer = (TCHAR*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * apBlock->svmlPLCDataFixedLength;

               lValueBufferOffset = apBlock-> 
                                       svmpIndividualElementArray [lLoop]->
                                          svmlOriginalIndex * 
                                    apBlock->svmlPLCDataFixedLength;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%*.*s",
                        apBlock->svmlPLCDataFixedLength,
                        apBlock->svmlPLCDataFixedLength,
                        T2A (&ptczValueBuffer [lValueBufferOffset]));

               break;
            }
         case SV_PLC_INT32:
            {

//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;
               long     lValueBufferOffset;

               pczTransmitBuffer = (char*) apBlock->svmcpTxRxBuffer;
               ptczValueBuffer = (TCHAR*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * apBlock->svmlPLCDataFixedLength;

               lValueBufferOffset = apBlock-> 
                                       svmpIndividualElementArray [lLoop]->
                                          svmlOriginalIndex * 
                                    apBlock->svmlPLCDataFixedLength;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%*.*s",
                        apBlock->svmlPLCDataFixedLength,
                        apBlock->svmlPLCDataFixedLength,
                        T2A (&ptczValueBuffer [lValueBufferOffset]));

               break;
            }
         case SV_PLC_STRING:
            {
//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;

               pczTransmitBuffer = (char*) apBlock->svmcpTxRxBuffer;
               ptczValueBuffer = (char*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * SV_MAX_SLC500_ST_LENGTH;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%s", ptczValueBuffer );

               break;
            }
         default:
            {
               lErr = -1239;
               break;
            }

         }  // switch (apBlock->svmPLCDataType)

         if (lErr  < 0)
            break;
      }  // for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.Ge...
      break;
   }  // while (1)

   return lErr;

}


long SVASABtcpClass::SendSPFloat (SVPLCBlockStruct* apBlock)
{
	long  lErr;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		m_ASABtcpControl.WriteBlock(apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
	               
	return lErr;
}


long SVASABtcpClass::GetB (SVPLCBlockStruct* apBlock)
{
	long        lErr;
	CString     definition;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		lErr = m_ASABtcpControl.ReadBlock( apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
                   
   return lErr;
}


long SVASABtcpClass::GetInt16 (SVPLCBlockStruct* apBlock)
{
	long        lErr;
	CString     definition;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		//apBlock-> = 2;
		lErr = m_ASABtcpControl.ReadBlock( apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
                   
   return lErr;
}

long SVASABtcpClass::GetInt32 (SVPLCBlockStruct* apBlock)
{
	long        lErr;
	CString     definition;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		lErr = m_ASABtcpControl.ReadBlock( apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
                   
   return lErr;
}


long SVASABtcpClass::GetStr (SVPLCBlockStruct* apBlock)
{
	long        lErr;
	CString     definition;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		lErr = m_ASABtcpControl.ReadBlock( apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
                   
   return lErr;
}



long SVASABtcpClass::GetSPFloat (SVPLCBlockStruct* apBlock)
{
	long        lErr;
	CString     definition;

	lErr = 0;

	if( m_ASABtcpControl.IsValid() )
	{
		lErr = m_ASABtcpControl.ReadBlock( apBlock, static_cast<long>(svmTimeOutLimit), SolCallback );
	}
                   
   return lErr;
}


long SVASABtcpClass::InitializeBlocks (SVPLCBlockStruct* apBlock)
{
   long              lErr;
   long              lBlockNbr;
   SVPLCBlockStruct* pBlock;

   lErr = 0;
   lBlockNbr = 0;

   while (1)
   {
      SVPLCBaseClass::InitializeBlocks (apBlock);


      pBlock = apBlock;

      while (pBlock)
      {
         lBlockNbr = lBlockNbr + 1;

         TRACE2 ("PLC data type = %d, block number = %d\n", pBlock->svmPLCDataType, lBlockNbr);
         switch (pBlock->svmPLCDataType)
         {
         case SV_PLC_BIT:
         case SV_PLC_INT16:
         case SV_PLC_STRING:
         case SV_PLC_SPFLOAT:
         case SV_PLC_INT32:
               break;

         default:
            {
               lErr = -1318;
               break;
            }

         }

         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;

      } // while (pBlock)

      break;
   } // while (1)

   return lErr;
}


long SVASABtcpClass::DeinitializeBlocks (SVPLCBlockStruct* apBlock)
{
   long              lErr;

   lErr = 0;

   while (1)
   {
      SVPLCBaseClass::DeinitializeBlocks (apBlock);
      break;
   }

   return lErr;
}


long SVASABtcpClass::TestCommunications()
{
	long l_hr = 0;
	l_hr = m_ASABtcpControl.TestCommunication();
	return l_hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:36:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Apr 2013 13:29:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 15:45:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/