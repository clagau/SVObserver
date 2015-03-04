// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVS7PBFMSClass
// * .File Name       : $Workfile:   SVS7PBFMS.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:33:28  $
// ******************************************************************************

/////////////////////////////////////////////////////////////////////
//    N O T E S
//
// Addressing:
//    The addressing in the S7 system is by byte. Word addresses should
//    start on even addresses and will take up two bytes.  Float addresses
//    should start on even addresses and be incremented by 4.
//    Example:
//          A word (16-bit int) is located at DB1.DBW40.  The next word
//          would be located at DBW42.
//    Example:
//          A float value is located at DB1.DBD48F.  The next word
//          would be located at DBD52F.
//
// Alignment and integer format:
//    The bytes for an integer in S7 are in reverse order to those in the PC.
//    Applicom swaps values accordingly
#include "stdafx.h"
#include <math.h>
#include <afxpriv.h>
#include "SVS7PBFMS.h"
#include "SVTimerLibrary/SVClock.h"
#include "plc_interface.h"
#include "SVSystemLibrary/SVThreadManager.h"


// ThreadID for Deferred operations thread

DWORD  SVS7PBFMSClass::m_svmdwDeferThreadId = 0; 

HANDLE SVS7PBFMSClass::m_svmhDeferThread = 0;

long   SVS7PBFMSClass::svmlStopDef = FALSE;

CMutex SVS7PBFMSClass::m_SVDeferQLock;
CMutex SVS7PBFMSClass::svmDeferThreadLock;

SVPLCCommQueueStructList SVS7PBFMSClass::svmRequestQueue;


//jab083100 CArray <long, long> SVS7PBFMSClass::msvUsedIds; 
// msvUnusedIds only contain a list of indexes which have been previously 
// removed from msvUsedIds.
//jab083100 CArray <long, long> SVS7PBFMSClass::msvUnusedIds; 

// Number of open objects
//jab083000 int   SVS7PBFMSClass::g_nOpencount = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SVS7PBFMSClass::SVS7PBFMSClass(long* aplErr):
SVPLCBaseClass (aplErr) 

{

   while (1)
   {
      m_ConnectionData.svmPLCManufacturer   = SV_PLCMANUFACTURER_SEIMENS;
      m_ConnectionData.svmPLCModel          = SV_PLCMODEL_STEP7;
      m_ConnectionData.svmPLCSubModel       = SV_PLCSUBMODEL_STEP7_300;
      m_ConnectionData.svmConnectionMethod  = SV_PLCCONNECTIONMETHOD_APIAPL;

      strcpy (svmczDriverName, "");
      strcpy( svmczPortId, "" );

      break;
   }

}



long  SVS7PBFMSClass::Initialize ()
{
   long  lErr;

   while (1)
   {
      lErr = SVPLCBaseClass::Initialize ();
      if (lErr < 0)
         break;

      svmDeferThreadLock.Lock ();
      if (m_svmhDeferThread == NULL)
      {
         m_svmhDeferThread = CreateThread (NULL, 0, S7DeferManager, this,
                                             0, &m_svmdwDeferThreadId);
         if (!m_svmhDeferThread)
         {
            lErr = -1553;
         }
      }
	  SVThreadManager::Instance().Add( m_svmhDeferThread, _T("PLC S7 DeferThread"));
      svmDeferThreadLock.Unlock ();
      
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



SVS7PBFMSClass::~SVS7PBFMSClass()
{
   DWORD dwExitCode;

   long  lErr = 0;


   while (1)
   {
//    The send and response threads need to be terminated prior to 
//    terminating the defered thread.  This should not have an ill affect
//    on the base class termination. ----------------------------------------

      TerminateSendAndResponseThreads ();
   
      svmRequestQueue.ResetParent (this, 
                                   NULL, 
                                   &m_SVDeferQLock);

      svmDeferThreadLock.Lock ();
      svmNbrInstancesLock.Lock ();
      if (svmlNbrInstances == 1)
      {

         svmlStopDef = TRUE;

		  SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

         do
         {

            GetExitCodeThread (m_svmhDeferThread, &dwExitCode);
			SVThreadManager::Instance().Remove(m_svmhDeferThread);
		    if (10000 < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start )))
            {
               TRACE0 ("SVS7: Defered Thread termination forced.\n");
               AfxMessageBox (_T("Defered Thread termination forced."));

               TerminateThread (m_svmhDeferThread,    // handle to the thread
                                0);   // exit code for the thread);

               lErr = -1554;
               break;
            }
         } while (dwExitCode == STILL_ACTIVE && lErr == 0);

         m_svmhDeferThread = NULL;
      }
      else
      {
      }

      svmDeferThreadLock.Unlock ();

      Close();
      break;
   }

   svmlNbrInstances = svmlNbrInstances - 1;
   svmNbrInstancesLock.Unlock ();
}

//-------------------------------------------------------------------
// Check the status of a PLC block
//-------------------------------------------------------------------

long SVS7PBFMSClass::CheckStatus (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      // The actual status is set in the thread that deals with the Applicom
      // module.
      if (apBlock->svmStatus != SV_PLCCOMMSTATUS_PENDING)
      {
//------ svmStatus will only be changed by the raw data during the pending
//------ state.
         break;
      }

      if (svmhApplicomLib == NULL)
      {
//------ If the Applicom DLL is not loaded, then we will fake the return of 
//------ the DTL_SUCCESS.
         apBlock->svmulRawStatus = S7_STATUS_SUCCESS;
      }


      switch ( (short) apBlock->svmulRawStatus)
      {
      case S7_STATUS_DEFREQ_NOTCOMPLETED:
         {
//--------- Still pending.

            break;
         }
      case S7_STATUS_SUCCESS:
      case S7_STATUS_TRANSDIF_WRITECOMPLETE:
         {
//            AfxMessageBox (_T("Almost complete"));

            apBlock->svmStatus = SV_PLCCOMMSTATUS_ALMOSTCOMPLETE;
            break;
         }

//--- Deferred read or write attempt when the deferred request register is 
//--- full exitbus() must be used to eliminate error
      case  S7_STATUS_DEFREQ_REGFULLEXIT: // 40
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
            apBlock->svmlCommunicationError = -1358;
            TRACE1 ("check status: REGFULL error, Number open = %d\n", svmlNbrOpen);
            break;
         }
      default:
               
         TRACE1 ("raw status = %d\n", apBlock->svmulRawStatus);
         apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
         apBlock->svmlCommunicationError = -1550;

         break;
      }     // switch

      break;
   }  // End while loop

   
   return apBlock->svmStatus;
}



long SVS7PBFMSClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   short       status;
   long  lRetValue;

   status = 0;

   switch (aDataType)
   {
   case SV_PLC_BIT:
      {
         lRetValue = 2;
         break;
      }
   case SV_PLC_INT16:
      {
         lRetValue = 2;
         break;
      }
   case SV_PLC_STRING:
      {
         lRetValue = S7_MAX_STRING_LENGTH;
         break;
      }

   case SV_PLC_INT32:
   case SV_PLC_SPFLOAT:
      {
         lRetValue = 4;
         break;
      }
   default:
      {
         status = -1359;
         break;
      }
   }

   if (status < 0)
      return status;
   else
      return lRetValue;
}



long SVS7PBFMSClass::CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock)
{
   short    status;
   long     lLoop;
   long     lOffset;
   long     lDataSize;

   long*    plPCValue;
   short*   pnPLCShortValue;  // Int16

   char*    pcPLCStringValue;
   
   float*   pfPLCFloatValue;

   status = 0;

   while (1)
   {
      plPCValue = (long *) apBlock->svmpParamBuffer;

      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            pnPLCShortValue = (short *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize (); 
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement) / lDataSize;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->
                           svmlOriginalIndex] = 
                  pnPLCShortValue [lOffset];
            }
            break;
         }
      case SV_PLC_STRING:
         {
            pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement)  / DataTypeSize( apBlock->svmPLCDataType );

//------------ Allen Bradley strings have a fixed size of 42 bytes.
               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 
                  atol (&pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength]);
            }


            break;
         }
      case SV_PLC_SPFLOAT:
         {
            pfPLCFloatValue = (float *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                          apBlock->svmlFirstElement) / lDataSize;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 
                  (long) (pfPLCFloatValue [lOffset]);
            }

            break;
         }

      case SV_PLC_INT32:
         {
            long *pfPLCInt32Value = (long *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement) / lDataSize;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 
                  (long) pfPLCInt32Value [lOffset];
            }

            break;
         }
      }

      break;
   }

   return status;
}




long SVS7PBFMSClass::CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock)
{
   short           status;
   long           lLoop;
   long           lOffset;
   long           lDataSize;

   long double*   pfPCValue;

   short*         pnPLCShortValue;

   char*          pcPLCStringValue;
   
   float*         pfPLCFloatValue;

   status = 0;

   while (1)
   {
      pfPCValue = (long double *) apBlock->svmpParamBuffer;

      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            pnPLCShortValue = (short *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                          apBlock->svmlFirstElement) / lDataSize;

               pfPCValue [lLoop] = pnPLCShortValue [lOffset];
            }
            break;
         }
      case SV_PLC_STRING:
         {
            pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                          apBlock->svmlFirstElement) / lDataSize;

//------------ Allen Bradley strings have a fixed size of 42 bytes.
               pfPCValue [lLoop] = 
                  atof (&pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength]);
            }


            break;
         }
      case SV_PLC_SPFLOAT:
         {
            pfPLCFloatValue = (float *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement) / lDataSize;

               pfPCValue [lLoop] = pfPLCFloatValue [lOffset];
            }

            break;
         }

      case SV_PLC_INT32:
         {
            pfPLCFloatValue = (float *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize ();
                 lLoop++)
            {
               lOffset = (apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement) / lDataSize;

               pfPCValue [lLoop] = (float) pfPLCFloatValue [lOffset];
            }

            break;
         }
      }

      break;
   }

   return status;
}



long SVS7PBFMSClass::CopyReceiveBufferToString (SVPLCBlockStruct* apBlock)
{
   long           lErr;
   long           lLoop;
   long           lOffset;
   long           lDataSize;
   long           lAllocatedPLCStringSize;
   long           lActualPLCStringSize;
   long           lCopyStringSize;

   TCHAR*         ptcPCValue;

   char*          pcPLCStringValue;
   char*          cpSwappedBytes;

   lErr = 0;
   cpSwappedBytes = NULL;

   while (1)
   {
      ptcPCValue = (TCHAR *) apBlock->svmpParamBuffer;
      pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;

      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_INT16:
         {
            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize (); 
                 lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement + 
                         (lLoop * 2);

               lAllocatedPLCStringSize = (unsigned char) pcPLCStringValue [lOffset + 1];
               lActualPLCStringSize = (unsigned char) pcPLCStringValue [lOffset];

               if (apBlock->svmlPCDataFixedLength < lActualPLCStringSize)
               {
                  lCopyStringSize = apBlock->svmlPCDataFixedLength;
               }
               else
               {
                  lCopyStringSize = lActualPLCStringSize;
               }


//------------ +1 for NULL terminator, and + 1 for even (word) spacing (if 
//------------ neccessary).
               cpSwappedBytes = new char [lCopyStringSize + 2]; 
               if (cpSwappedBytes == NULL)
               {
                  lErr = -1357;
                  break;
               }

               SwapBytes (cpSwappedBytes, 
                          &pcPLCStringValue [lOffset + 2], 
                          (long) ceil ((double)lCopyStringSize / (double)2));

               sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPCDataFixedLength], 
                            A2T(cpSwappedBytes),
                            lCopyStringSize + 1);  // include NULL
            }
            break;
         }
      default:
         {
            lErr = -1356;
            break;
         }

      }

      break;
   }

   delete [] cpSwappedBytes;

   return lErr;
}



long SVS7PBFMSClass::CopyLongToSendBuffer (SVPLCBlockStruct* apBlock)

{
   short    status;
   long     lLoop;
   long     lTransmitBufferOffset;
   long     lDataSize;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   long*    plValueBuffer;


   status = 0;

   while  (1)
   {
      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
      {
                  
         switch (apBlock->svmPLCDataType)
         {
         case SV_PLC_BIT:
         case SV_PLC_INT16:
            {

//------------ Physical array of data which will be transferred.
               short*   pnTransmitBuffer;

               pnTransmitBuffer = (short*) apBlock->svmcpTxRxBuffer;
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement) / lDataSize;

               pnTransmitBuffer [lTransmitBufferOffset] = 
                  (short) plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         case SV_PLC_STRING:
            {
//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;

               pczTransmitBuffer = apBlock->svmcpTxRxBuffer;
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * S7_MAX_STRING_LENGTH;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%42.42s", 
                        plValueBuffer [apBlock->svmpIndividualElementArray 
                           [lLoop]->svmlOriginalIndex]);


               break;
            }
         case SV_PLC_SPFLOAT:
            {

//------------ Physical array of data which will be transferred.
               float*   pfTransmitBuffer;

               pfTransmitBuffer = (float*) apBlock->svmcpTxRxBuffer;
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) / lDataSize;

               pfTransmitBuffer [lTransmitBufferOffset] = (float)
                  plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }

         case SV_PLC_INT32:
            {

//------------ Physical array of data which will be transferred.
               long*   pfTransmitBuffer;

               pfTransmitBuffer = (long*) apBlock->svmcpTxRxBuffer;
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) / lDataSize;

               pfTransmitBuffer [lTransmitBufferOffset] =
                  plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         
         default:
            {
               status = -1237;
               break;
            }

         }  // switch (apBlock->svmPLCDataType)

         if (status  < 0)
            break;
      }  // for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.Ge...
      break;
   }  // while (1)

   return status;

}




long SVS7PBFMSClass::CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock)

{
   short    status;
   long     lLoop;
   long     lTransmitBufferOffset;
   long     lDataSize;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC Addresses in this
// array are not required to be continuous or sequencial.
   double*  pdValueBuffer;


   status = 0;

   while  (1)
   {
      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
      {
                  
         switch (apBlock->svmPLCDataType)
         {
         case SV_PLC_BIT:
         case SV_PLC_INT16:
            {

//------------ Physical array of data which will be transferred.
               short*   pnTransmitBuffer;

               pnTransmitBuffer = (short*) apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) / lDataSize;

               pnTransmitBuffer [lTransmitBufferOffset] = (short)
                  pdValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         case SV_PLC_STRING:
            {
//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;

               pczTransmitBuffer = apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * S7_MAX_STRING_LENGTH;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%.42f", 
                        pdValueBuffer [apBlock->svmpIndividualElementArray 
                           [lLoop]->svmlOriginalIndex]);


               break;
            }
         case SV_PLC_SPFLOAT:
            {

//------------ Physical array of data which will be transferred.
               float*   pfTransmitBuffer;

               pfTransmitBuffer = (float*) apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement) / lDataSize;

               pfTransmitBuffer [lTransmitBufferOffset] = (float)
                  pdValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }

         case SV_PLC_INT32:
            {

//------------ Physical array of data which will be transferred.
               long*   pfTransmitBuffer;

               pfTransmitBuffer = (long*) apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement) / lDataSize;

               pfTransmitBuffer [lTransmitBufferOffset] = (long)
                  pdValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         
         default:
            {
               status = -1238;
               break;
            }

         }  // switch (apBlock->svmPLCDataType)

         if (status  < 0)
            break;
      }  // for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.Ge...
      break;
   }  // while (1)

   return status;

}




long SVS7PBFMSClass::CopyStringToSendBuffer (SVPLCBlockStruct* apBlock)

{
   short    status;
   long     lCopyStringSize;
   long     lLoop;
   long     lTransmitBufferOffset;
   long     lNbrOfBytes;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   TCHAR*   ptczValueBuffer;

   status = 0;

   while  (1)
   {
      for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
      {
                  
         switch (apBlock->svmPLCDataType)
         {
         case SV_PLC_INT16:
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
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;


               lValueBufferOffset = apBlock-> 
                                       svmpIndividualElementArray [lLoop]->
                                          svmlOriginalIndex * 
                                    apBlock->svmlPLCDataFixedLength;

               pczTransmitBuffer [0] = (char) apBlock->svmlPLCDataFixedLength;
               lNbrOfBytes = _tcslen (&ptczValueBuffer [lValueBufferOffset]);
               pczTransmitBuffer [1] = (char) lNbrOfBytes;

               if (apBlock->svmlPLCDataFixedLength < lNbrOfBytes)
               {
                  lCopyStringSize = apBlock->svmlPLCDataFixedLength;
               }
               else
               {
                  lCopyStringSize = lNbrOfBytes;
               }

               sv_strncpy (&pczTransmitBuffer [lTransmitBufferOffset + 2], 
                           T2A(&ptczValueBuffer [lValueBufferOffset]),
                           lCopyStringSize);

               break;
            }
         default:
            {
               status = -1239;
               break;
            }

         }  // switch (apBlock->svmPLCDataType)

         if (status  < 0)
            break;
      }  // for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.Ge...
      break;
   }  // while (1)

   return status;

}




long SVS7PBFMSClass::ParseElements (SVPLCElementStructArray* apAddress)
{
   long     lErr;
   long     lLoop;
   long     lNbrOfElements;
//   long     lNbrOfWords;

   TCHAR*   pcIndex;

   SVPLCElementStruct* pElement;

   lErr = 0;

   while (1)
   {
      lNbrOfElements = apAddress->GetSize ();

      for (lLoop = 0; lLoop < lNbrOfElements; lLoop++)
      {

         pElement = (*apAddress) [lLoop];
         pElement->svmBitMask = 0;

         pcIndex = pElement->svmPLCAddress;

         // Note: for now, file number is always zero

         pElement->svmElementType = SV_PLC_UNKNOWN;
         if (!ParseAddress( pElement->svmPLCAddress, pElement->svmElement, pElement->svmElementType ))
         {
            lErr = -1345;
            break;
         }
         pElement->svmPLCFileNbr = pElement->svmElement / S7_BYTES_PER_DB;
         pElement->svmlParsed = TRUE;

      } // for (lLoop = 0; lLoop < lNbrOfElements; lLoop++)

      break;
   }
   return lErr;
}



long SVS7PBFMSClass::QuickSortElements (SVPLCElementStructArray* apSortArray,
                                        long    alBeginning,
                                        long    alEnd)

{
   long           lErr;

   long           i;
   long           j;

   SVPLCElementStruct*  pElement;
   SVPLCElementStruct*  pTemp;

	i = alEnd;
   j = alBeginning;


   lErr = 0;


   while (1)
   {
      if (i == alBeginning)
      {
//       This is not a fatal error, but there is nothing to sort.
         break;
      }


      pElement = (*apSortArray) [(i + j) / 2];

      do
      {
         while (((*apSortArray) [j]->svmPLCFileNbr < pElement->svmPLCFileNbr) ||
                (((*apSortArray) [j]->svmPLCFileNbr == pElement->svmPLCFileNbr) &&
                 ((*apSortArray) [j]->svmElement < pElement->svmElement)))
            j++;


         while (((*apSortArray) [i]->svmPLCFileNbr > pElement->svmPLCFileNbr) ||
                (((*apSortArray) [i]->svmPLCFileNbr == pElement->svmPLCFileNbr) &&
                 ((*apSortArray) [i]->svmElement > pElement->svmElement)))
            i--;


         if (i >= j)
         {
            if (i != j)
            {
               pTemp = (*apSortArray) [i];
               (*apSortArray) [i] = (*apSortArray) [j];
               (*apSortArray) [j] = pTemp;
            }

            i --;
            j ++;
         }

      } while (j <= i);

      if (alBeginning < i)
      {
         lErr = QuickSortElements (apSortArray,
                                   alBeginning,
                                   i);
         if (lErr < 0)
            break;
      }

      if (j < alEnd)
      {
         lErr = QuickSortElements (apSortArray,
                                   j,
                                   alEnd);

         if (lErr < 0)
            break;

      }

      break;
   }  // while (1) ....

   return lErr;
}


//-------------------------------------------------------------------
// AddElementToBlock
//
// Add a PLC element to an element block
//
//-------------------------------------------------------------------

long SVS7PBFMSClass::AddElementToBlock (SVPLCBlockStruct*     apBlock,
                                        SVPLCElementStruct*   apElement)
{
   long  lErr;
   long  lDataSize;

   lErr = 0;

   while (1)
   {
      if (apBlock->svmpIndividualElementArray.GetSize () == 0)
      {
         // This is the first element in the block - no need to
         // check for other elements in the array.  Initialize the
         // necessary fields in the block
         CopyElementAddressToBlock (apBlock,
                                    apElement);

         RebuildAddressString (apBlock);

//         apBlock->svmpIndividualElementArray.Add (apElement);

         lDataSize = DataTypeSize (apBlock->svmPLCDataType);

         if (apBlock->svmlPCDataFixedLength == 0)
         {
            apBlock->svmlPLCDataFixedLength = 1;
         }
         else
         {
//--------- The +2 is added to account for the buffer length and string 
//--------- length values at the beginning of the buffer. -------------------
            apBlock->svmlPLCDataFixedLength = 
               (apBlock->svmlPCDataFixedLength + 2) / 
               lDataSize;
         }

         if (apBlock->svmlPLCDataFixedLength > (218 / lDataSize))
         {
            apBlock->svmlPLCDataFixedLength = 218 / lDataSize;
         }
      }

      //--- The block is not empty, so we must check to see if the current 
      //--- element fits in the current block.
      // Note: svmPLCFileNbr = Data Block #

      if (apBlock->svmlPLCFileNbr != apElement->svmPLCFileNbr)
      {
         lErr = 1;
         break;
      }

      if (apBlock->svmPLCDataType != apElement->svmElementType)
      {
         //------ SAME FILE NUMBER< DIFFERENT DATA TYPE -----------------------
         lErr = -1551;
         break;
         //------ End of SAME FILE NUMBER< DIFFERENT DATA TYPE. ---------------
      }

      lDataSize = DataTypeSize (apElement->svmElementType);

//--- If the data element is not on an even boundry with the start of the
//--- block, then start a new block.
      if (((apElement->svmElement - apBlock->svmlFirstElement) % lDataSize) != 0)
      {
         lErr = 1;
         break;
      }

//--- Test to see if element would excede 256 byte block size.
// Note: Not necessary for Applicom/Siemens equipment
      if ((apElement->svmElement - apBlock->svmlFirstElement) > 
          (218 / lDataSize))
      {
         lErr = 1;
         break;
      }


      apBlock->svmlNbrElements = ((apElement->svmElement -                             
                                   apBlock->svmlFirstElement) / 
                                  lDataSize) + 
                                 apBlock->svmlPLCDataFixedLength;

      apBlock->svmpIndividualElementArray.Add (apElement);

      break;
   }

   return lErr;
}     // AddElementToBlock


//-------------------------------------------------------------------
// RebuildAddressString - rebuild the ASCII address string using the numeric
// address

long SVS7PBFMSClass::RebuildAddressString (SVPLCBlockStruct* apBlock)
{
   short       status;

   char  szFileType [8];

   status = 0;

   while (1)
   {
      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
         {
            sprintf (szFileType, "X");
            break;
         }
      case SV_PLC_INT16:
         {
            sprintf (szFileType, "W");
            break;
         }
      case SV_PLC_STRING:
         {
            sprintf (szFileType, "B");
            break;
         }
      case SV_PLC_INT32:
      case SV_PLC_SPFLOAT:
         {
            sprintf (szFileType, "D");
            break;
         }
      default:
         {
            status = -1209;
            break;
         }
      }

      if (status < 0)
         break;

      long  lDBNo;
      long  lOffset;

      if ( apBlock->svmPLCDataType == SV_PLC_BIT )
      {
         lDBNo = apBlock->svmlFirstElement / S7_BITS_PER_DB;
         lOffset = (apBlock->svmlFirstElement % S7_BITS_PER_DB) / S7_BITS_PER_BYTE;
         long lBitNo = apBlock->svmlFirstElement % S7_BITS_PER_BYTE;
         sprintf (apBlock->svmRebuiltDTAddress,
               "DB%d.DB%s%d.%d",
               lDBNo,
               szFileType,
               lOffset,
               lBitNo );
      }
      else
      {
         lDBNo = apBlock->svmlFirstElement / S7_BYTES_PER_DB;
         lOffset = apBlock->svmlFirstElement % S7_BYTES_PER_DB;
         sprintf (apBlock->svmRebuiltDTAddress,
               "DB%d.DB%s%d",
               lDBNo,
               szFileType,
               lOffset);
      }

      break;
   }

   return status;

}        // RebuildAddressString

//-------------------------------------------------------------------
// Open the connection for this object
//
// Argument:
//    TCHAR *apConnectionString     [in]
//       Connection string specfying Applicom channel and equipment number
//       Format is "<channel>/<equipment>".  The channel is always zero
//       The equipment can be any number between 0 and 127.
// Note:
//    The equipment number must be already configured in the Applicom
//    configuration utility (PCCONF)
//-------------------------------------------------------------------

long SVS7PBFMSClass::Open (TCHAR *apConnectionString)
{
   short       status = 0;
   long        lErr = 0;


   while (1)
   {
      if (svmlIsOpen == TRUE)
      {
         lErr = 1;
         break;
      }

//      ParseSetupParameterString (apConnectionString);
//TRB moving Parse setup ... to interface class.
 
//--- Copy to class member.
      sv_tstrncpy (svmczConnectionString,
                   apConnectionString,
                   SV_NBRCHARS (svmczConnectionString));

      svmNbrOpenLock.Lock ();

      lErr = SVPLCBaseClass::Open ();
      if (lErr < 0)
         break;

      if ( svmlNbrOpen == 1 )
      {
         if (svmhApplicomLib)
         {
            TRACE0 ("Init busX\n");
            initbus( &status );
         }
         else
         {
            status = 0;
         }
      
         if ( status != 0 )
         {
            lErr = TranslateError( status );
            svmlIsOpen = FALSE;
            break;
         }
         
      }

      svmPLCChannelNo = _ttoi (m_ConnectionData.svmczChannelNbr);
      svmPLCEquipNo = _ttoi (m_ConnectionData.svmczEquipmentNbr);

      svmlIsOpen = TRUE;
      break;
   }

   if (svmlIsOpen == FALSE)
   {
      svmlNbrOpen = svmlNbrOpen - 1;
      if (svmhApplicomLib)
      {
         exitbus( &status );
      }
   }

   svmNbrOpenLock.Unlock ();

   return lErr;
}     // Open()



long SVS7PBFMSClass::Close ()
{
   long  lErr;

   short status;

   lErr = 0;


   svmNbrOpenLock.Lock ();

	while (1)
	{
      if (svmlIsOpen == FALSE)
      {
         lErr = 1;
         break;
      }

      if (svmlNbrOpen == 1)
      {
         // If this is the last meobject open, then detach from the Applicom
         // card

         if (svmhApplicomLib)
         {
            TRACE0 ("Exit busX\n");
            exitbus( &status );

            if (status != 0)
            {
               lErr = -1300;
               break;
            }
         }
      }

      SVPLCBaseClass::Close ();  // Sets svmlIsOpen to FALSE.
      break;
	}

   svmNbrOpenLock.Unlock ();

   return lErr;

}     // Close()




long SVS7PBFMSClass::SendB (SVPLCBlockStruct* apBlock)
{
   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_WRITE_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::SendInt16 (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_WRITE_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::SendInt32 (SVPLCBlockStruct* apBlock)
{
   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_WRITE_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}




long SVS7PBFMSClass::SendStr (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_WRITE_OPER );
      break;
   }
   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::SendSPFloat (SVPLCBlockStruct* apBlock)
{
   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_WRITE_OPER );
      break;
   }
   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::GetB (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_READ_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::GetInt16 (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_READ_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::GetInt32 (SVPLCBlockStruct* apBlock)
{

   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_READ_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::GetStr (SVPLCBlockStruct* apBlock)
{
   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_READ_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}



long SVS7PBFMSClass::GetSPFloat (SVPLCBlockStruct* apBlock)
{
   while (1)
   {
      AddToRequestQueue( apBlock, S7_DEFER_READ_OPER );
      break;
   }

   return S7_STATUS_SUCCESS;
}


//-------------------------------------------------------------------
// Extract a number from a string.  The string is assumed to be base 10
// No alpha characters are allowed.  Since we are translating address,
// no negative numbers are allowed, either.
//
// Arguments:
//    CString  cstr     [in]
//             String containing number to be translated
//    long     &lVal    [out]
//             Returns translated number
//
// Returns:
//    bool     true if number was translated correctly
//             false of error occurred
//-------------------------------------------------------------------

bool SVS7PBFMSClass::ExtractNumber( CString &csStr, long &lVal )
{
	if ( csStr.GetLength() == 0 )
		return false;

	_TCHAR *p = csStr.GetBuffer(0);

	lVal = 0L;

   // Loop to translate string
   while ( *p )
	{
		if ( !isdigit(*p) )
			return false;
		lVal = 10*lVal + (*p - _TCHAR('0'));
		p++;
	}

	return true;
}

//-------------------------------------------------------------------
// ParseBitAddress - Get the equivalent Applicom (S7) numeric address 
//
//-------------------------------------------------------------------

bool SVS7PBFMSClass::ParseBitAddress( CString &csBitAddr, long &lBitAddress )
{
	long	lByteNo;

	long	lBitNo;

	if ( csBitAddr.GetLength() < 3 )
		return false;

	int index = csBitAddr.Find( _TCHAR( '.' ) );

	if ( index <= 0 )
		return false;

	if ( !ExtractNumber( csBitAddr.Left(index), lByteNo ) )
		return false;

	if ( !ExtractNumber( csBitAddr.Right( csBitAddr.GetLength() - index - 1 ), lBitNo ) )
		return false;

	lBitAddress = lByteNo * 8 + lBitNo;

	return true;

}

//-------------------------------------------------------------------
// ParseAddress
//
// Description:
//    Parse an ASCII addres into the equivalent Applicom S7 address
//
// Arguments:
//    _TCHAR *szAddr       [in]
//       ASCII form of data address ("Q8.0", "DB5.DW6" )
//    long lAddress        [out]
//       Equivalent Applicom S7 address in long integer form
//    SVPLCDataTypeEnum &eTyp    [out]
//       Data type (bit, string, integer, float)
//
// Returns:
//    true if address was parsed correctly, false otherwise
//-------------------------------------------------------------------

bool SVS7PBFMSClass::ParseAddress( _TCHAR *szAddr, long &lAddress, SVPLCDataTypeEnum &eTyp )
{

	int		nDecPlace;

	long	lDBNo, lDBOffs;

	CString csBitStr;

	CString csAddr( szAddr );
	
	CString csDBNo, csDBOffs;

	csAddr.MakeUpper();

	_TCHAR c = csAddr.GetAt(0);

	eTyp = SV_PLC_UNKNOWN;

	// Check the first character - must be 'F', 'I', 'Q', 'D', 'T' or 'C'
	switch( c )
	{
	case S7_ADDR_INPUT_CHAR:
	case S7_ADDR_OUTPUT_CHAR:
		if ( csAddr.GetAt(1) == S7_ADDR_DOUBLE_CHAR )
		{
			return false;
		}
	case S7_ADDR_FLAG_CHAR:
		// First char is 'F', 'I' or 'Q'.  Form can be either <c>Bx, <c>Wx or <c>x.y
		switch ( csAddr.GetAt(1) )
		{
		case S7_ADDR_BYTE_CHAR:
			eTyp = SV_PLC_STRING;
			break;
		case S7_ADDR_WORD_CHAR:
			if ( !ExtractNumber( csAddr.Right( csAddr.GetLength() - 2 ), lAddress ) )
				return false;
			eTyp = SV_PLC_INT16;
			break;

		case S7_ADDR_DOUBLE_CHAR:
			// Now see if the last character is the floating point char
			if ( ( csAddr.GetAt(csAddr.GetLength() - 1) != S7_ADDR_FLOAT_CHAR ) ||
					(csAddr.GetLength() < 4) )
			{
				return false;
			}

			if (!ExtractNumber( csAddr.Mid(2,csAddr.GetLength() - 3), lAddress ) )
				return false;

			eTyp = SV_PLC_SPFLOAT;
				
			break;

		default:
			// Must be numeric - check for form <c>x.y
			if ( !isdigit( csAddr.GetAt(1) ) )
			{
				// Error!!!!!
				return false;
			}
			if ( !ParseBitAddress( csAddr.Right( csAddr.GetLength() - 1 ), lAddress ) )
				return false;

			eTyp = SV_PLC_BIT;

		}
		break;


	case S7_ADDR_DBLOCK_CHAR:
		// Data block - address should be in the form DBx.<offset>, with <offset>
		// to be determined later
		if ( csAddr.GetAt(1) != _TCHAR('B'))
			return false;
		nDecPlace = csAddr.Find(_TCHAR('.'));
		if (nDecPlace < 0)
			return false;

		// Now get DB #
		csDBNo = csAddr.Mid(2, nDecPlace-2);
		if (!ExtractNumber( csDBNo, lDBNo ) )
			return false;

		// Now get the offset string (DBx.<offset>)
		csDBOffs = csAddr.Right( csAddr.GetLength() - nDecPlace - 1 );
		
		if ( ( csDBOffs.Left(2) != "DB" ) || ( csDBOffs.GetLength() < 3 ) )
			return false;

		// The address and type will be determined by the 3rd character in the
		// offset string (DB<c>)
		switch ( csDBOffs.GetAt( 2 ) )
		{
		case S7_ADDR_BYTE_CHAR:
			if ( !ExtractNumber( csDBOffs.Right( csDBOffs.GetLength() - 3 ), lDBOffs ) )
				return false;
			lAddress = lDBNo * S7_BYTES_PER_DB + lDBOffs;
			eTyp = SV_PLC_STRING;
			break;

		case S7_ADDR_WORD_CHAR:
			if ( !ExtractNumber( csDBOffs.Right( csDBOffs.GetLength() - 3 ), lDBOffs ) )
				return false;

			lAddress = lDBNo * S7_BYTES_PER_DB + lDBOffs;
			eTyp = SV_PLC_INT16;
			break;

		case S7_ADDR_DOUBLE_CHAR:
         // The form is DBx.DBDy (32-bit) or DBx.DBDyF (flt. pt.)
         // check the last char to determine which it is
         {
         _TCHAR fChar = csDBOffs.GetAt( csDBOffs.GetLength() - 1 );
         if ( fChar == S7_ADDR_FLOAT_CHAR)
         {
            if ( !ExtractNumber( csDBOffs.Mid( 3, csDBOffs.GetLength() - 4 ), lDBOffs ) )
				return false;
   			eTyp = SV_PLC_SPFLOAT;
         }
         else
         {
            if ( !ExtractNumber( csDBOffs.Right( csDBOffs.GetLength() - 3 ), lDBOffs ) )
	   			return false;
   			eTyp = SV_PLC_INT32;
         }
            // If this is a double-word, then the value must be float
			lAddress = lDBNo * S7_BYTES_PER_DB + lDBOffs;
			break;
         }
		case S7_ADDR_DBBIT_CHAR:
			if ( !ParseBitAddress( csDBOffs.Right( csDBOffs.GetLength() - 3 ), lDBOffs ) )
				return false;

			lAddress = lDBNo * S7_BITS_PER_DB + lDBOffs;

			eTyp = SV_PLC_BIT;
			break;
		default:
				return false;
		}

		break;
	case S7_ADDR_TIMER_CHAR:
	case S7_ADDR_COUNTER_CHAR:
		break;
	default:
		return false;
	};	// End switch ( c )


	return true;
}

//-------------------------------------------------------------------
// Translate a standard Applicom status error into its equivalent
// SVResearch status/error code
//-------------------------------------------------------------------

long SVS7PBFMSClass::TranslateError( short nStatus )
{
  	switch( nStatus )
	{
	   case S7_STATUS_SUCCESS:
		   return 0;
      case S7_STATUS_TRANSDIF_WRITECOMPLETE:
         return -1500;
	   case S7_STATUS_DEFREQ_NOTCOMPLETED:
         return -1501;
	   case S7_STATUS_CYCFUNC_NOTFOUND:
         return -1502;
	   case S7_STATUS_UNKNWN_FUNC:
         return -1503;
	   case S7_STATUS_INVALID_ADDR:
         return -1504;
	   case S7_STATUS_INVALID_DATA:
         return -1505;
	   case S7_STATUS_IRRETRIEVABLE_DATA:
         return -1506;
	   case S7_STATUS_BADFUNC_PARAM:
         return -1507;
	   case S7_STATUS_CYCFUNC_NOTACTIVE:
         return -1508;
	   case S7_STATUS_DEFREQ_REGFULLEXIT:
         return -1509;
	   case S7_STATUS_DEFREQ_REGFULLTRANSDIF:
         return -1510;
	   case S7_STATUS_EMPTY_TRANSDIF:
         return -1511;
	   case S7_STATUS_NOTMEMRESIDIENT:
         return -1512;
	   case S7_STATUS_BDNO_NOTCONFIGURED:
         return -1513;
	   case S7_STATUS_NO_INTERFACE:
         return -1514;
	   case S7_STATUS_DRIVE_SYSTEM:
         return -1515;
	   case S7_STATUS_NOKEY:
         return -1516;
	   case S7_STATUS_IFACE_MEMORY:
         return -1517;
	   default:
         return -1599;
   }
}     // TranslateError()





long SVS7PBFMSClass::InitializeBlocks (SVPLCBlockStruct* apBlock)
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


//-------------------------------------------------------------------
// S7 Deferred Transfer Manager
//
// This API call runs on a separate thread and handles all Applicom
// deferred mode calls needed to transfer information between the PC
// and the S7 PLC
//
//-------------------------------------------------------------------
DWORD WINAPI S7DeferManager (LPVOID apCallingClass)
{

   long              lErr;
   long              lDataSize;
   short             typ;
   short             nNbrElements;
   short             status;
//jab101700   int               dsize;
   short             nChan;
   short             nEquip;
   long              lAddr;

   long*             lpStopDef;
   HINSTANCE*        hpApplicomLib;

   SVPLCCommQueueStruct* pCurrent;

// pThreadParent may have only a limited life span.
   SVS7PBFMSClass*   pThreadParent;
   SVS7PBFMSClass*   pTransactionParent;

   CMutex*           pDeferQLock;
   SVPLCCommQueueStructList* pRequestQueue;

   short tabl [4096];

   SVPLCBlockStruct* pBlock;

   lErr = 0;
   pThreadParent = (SVS7PBFMSClass*) apCallingClass;

   lpStopDef = &pThreadParent->svmlStopDef;
   *lpStopDef = FALSE;

   pDeferQLock = &pThreadParent->m_SVDeferQLock;
   pRequestQueue = &pThreadParent->svmRequestQueue;

   hpApplicomLib = &pThreadParent->svmhApplicomLib;

   if (*hpApplicomLib)
   {
      initbus( &status );
   }

   while (!*lpStopDef)
   {
      pDeferQLock->Lock ();
      if (!pRequestQueue->IsEmpty ())
      {
		 TRACE1 ("DM number of elements on queue = %d\n", pRequestQueue->GetCount ());
         // Get the next transfer to be executed
         pCurrent = pRequestQueue->GetHead();
         pTransactionParent = (SVS7PBFMSClass *) pCurrent->svmpParent;

         pTransactionParent->ClearRegisters();
         pBlock = pCurrent->svmpBlock;
         TRACE1 ("S7DM current address = %s\n", A2T (pBlock->svmRebuiltDTAddress));

         long nOper = pCurrent->svmlPendingOperation;
         pRequestQueue->RemoveHead();
         
         if (pCurrent->svmpParent == NULL)
         {
//--------- Send and Response queues no longer exist, so it doesn't matter.
            delete pCurrent;
            continue;
         }

         delete pCurrent;

         pBlock->svmulRawStatus = S7_STATUS_DEFREQ_NOTCOMPLETED;
         
         // Set the arguments for the deferred requests (read or write)

         lDataSize = 
            pTransactionParent->DataTypeSize (pBlock->svmPLCDataType);
//jab101700         nb = pBlock->svmlNbrElements * 
//jab101700              pParent->DataTypeSize (pBlock->svmPLCDataType);
         nNbrElements = (short) pBlock->svmlNbrElements;
         nChan = pTransactionParent->svmPLCChannelNo;
         nEquip = pTransactionParent->svmPLCEquipNo;
         lAddr = pBlock->svmlFirstElement;

         // Determine whether we are reading or writing information
         switch (nOper)
         {
         case S7_DEFER_READ_OPER: // read, Get
            {
               // Read Operation
               CString tCS;
               tCS.Format( _T("Read address %d, count = %d\n"), 
                           pBlock->svmlFirstElement, 
                           nNbrElements );
               TRACE( tCS );
               switch (pBlock->svmPLCDataType)
               {
               case SV_PLC_BIT: // This is not supported right now.
                  {
                     if (*hpApplicomLib)
                     {
                        readdifbit( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       &status );
                     }

                     break;
                  }
               case SV_PLC_INT16:
                  {
                     if (*hpApplicomLib)
                     {
                        readdifword( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       &status );
                     }

                     break;
                  }

               case SV_PLC_STRING:
               case SV_PLC_ASCII:
                  {
                     nNbrElements = (short) pBlock->svmlPLCDataFixedLength;
                     tCS.Format( _T("Read string, addr = %d, %d bytes\n"), lAddr, nNbrElements);
                     TRACE( tCS );
                     if (*hpApplicomLib)
                     {
                        readdifbyte( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       &status );
                     }
                     break;
                  }

               case SV_PLC_SPFLOAT:
                  {
                     if (hpApplicomLib)
                     {
                        readdiffword (&nChan, 
                                      &nEquip, 
                                      &nNbrElements ,
                                      &lAddr,
                                      &status );
                     }
                     break;
                  }

               case SV_PLC_INT32:
                  {
                     // Translate the count
                     if (hpApplicomLib)
                     {
                        readdifdword( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       &status );
                     }
                     break;
                  }

               default:
                  {
                     lErr = -1552;
                     pBlock->svmlCommunicationError = lErr;
                     break;
                  }

               }
               break;
            }


         case S7_DEFER_WRITE_OPER: // write
            {
               CString tCS;
               tCS.Format( _T("Write address %d, count = %d\n"), pBlock->svmlFirstElement, nNbrElements );
               TRACE( tCS );
               switch (pBlock->svmPLCDataType)
               {
               case SV_PLC_BIT:
                  {
                     if (hpApplicomLib)
                     {
                        writedifpackbit( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       (short *)pBlock->svmcpTxRxBuffer,
                                       &status );
                     }
                    
                     break;
                  }
               case SV_PLC_INT16:
                  {
                     // Note: because of the way that Applicom works with 16-bit
                     // integers, the count will be n*2-1 for the read, where
                     // n = # of words.  This is because the read call will
                     // return integers on even and odd bytes.  The write,
                     // however, only needs to transfer n words
//jab101700                     if ( nb > 2 )
//jab101700                        nb = (nb + 1) / 2;
                     memcpy (tabl, 
                             pBlock->svmcpTxRxBuffer,
                             nNbrElements * lDataSize);
//jab101700                              pParent->DataTypeSize( pBlock->svmPLCDataType ) * nb );
                     if (hpApplicomLib)
                     {
                        writedifword( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       tabl,
                                       &status );
                     }
                     break;
                  }
               case SV_PLC_INT32:
                  {
                     // Just get the number of floating pt. numbers
//jab101700                     nb = (nb+4)/4;

                     if (hpApplicomLib)
                     {
                        writedifdword( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       (long *)pBlock->svmcpTxRxBuffer,
                                       &status );
                     }
                     break;
                  }
               case SV_PLC_STRING:
               case SV_PLC_ASCII:
                  {
                     // String length is in the fixed length in the case of a string
                     nNbrElements = (short) pBlock->svmlPLCDataFixedLength;
                     tCS.Format( _T("Send string, %d elements\n"), nNbrElements);
					 //Arvid 150108 changed the format string so that it matches the parameters

                     TRACE( tCS );

                     if (hpApplicomLib)
                     {
                        writedifpackbyte( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       (char *)pBlock->svmcpTxRxBuffer,
                                       &status );
                     }
                     break;
                  }
               case SV_PLC_DPFLOAT:
               case SV_PLC_SPFLOAT:
                  {
                     // Just get the number of floating pt. numbers
//jab101700                     nb = (nb+4)/4;

                     if (hpApplicomLib)
                     {
                        writediffword( &nChan, &nEquip, &nNbrElements,
                                       &lAddr,
                                       (float *)pBlock->svmcpTxRxBuffer,
                                       &status );
                     }
                     break;
                  }
               default:
                  {
                     lErr = -1552;
                     pBlock->svmlCommunicationError = lErr;
                     break;
                  }

               } // switch (apBlock->svmPLCDataType)
               break;
            }  // case 2:
         default:
            {
               lErr = -1262;
               pBlock->svmlCommunicationError = lErr;
               break;
            }
         } // switch (apBlock->svmlPendingOperation)

         // Perform the deferred request transfer here
         if ( status == 0 )
         {

            // Keep looping here until we get a status other than
            // "not complete"
            do
            {
               if (hpApplicomLib)
               {
                  transdifpack( &pTransactionParent->svmPLCChannelNo, 
                                &nNbrElements, 
                                &typ,
                                tabl ,
                                &status);
               }
            } while ( status == S7_STATUS_DEFREQ_NOTCOMPLETED );


            switch (status)
            {
            case S7_STATUS_SUCCESS:
               // Read is successful
               {
//jab101700               switch( typ )
//jab101700               {
//jab101700               case 1:
//jab101700               case 2:
//jab101700                  dsize = sizeof(char);
//jab101700                  break;
//jab101700               case 3:
//jab101700                  dsize = sizeof(short);
//jab101700                  break;
//jab101700               case 4:
//jab101700                  dsize = sizeof( long );
//jab101700                  break;
//jab101700               case 5:
//jab101700                  dsize = sizeof( float );
//jab101700                  break;
//jab101700               default:
//jab101700                  dsize = sizeof( char );
//jab101700                  break;
//jab101700               };
               CString cTS;
               cTS.Format( _T("memcpy(), %d bytes\n"), pBlock->svmlNbrElements * 
                             pTransactionParent->DataTypeSize (pBlock->svmPLCDataType));
               TRACE( cTS );
               memcpy (pBlock->svmcpTxRxBuffer, 
                       tabl, 
                       pBlock->svmlNbrElements * lDataSize);
//jab101700               memcpy( pBlock->svmcpTxRxBuffer, tabl, nb * dsize );
               }
               break;
            case S7_STATUS_TRANSDIF_WRITECOMPLETE:
               break;
            default:
               break;
            }    // End switch (status )

            CString cs;
            cs.Format( _T("transdif complete: count = %d,  status = %04x\n"), nNbrElements, status );
            TRACE( cs );
         }     // If (status == 0)
         else
         {
            // Error - an error has occurred trying to do a read or write statement
            pTransactionParent->ClearRegisters();
         }

         pBlock->svmulRawStatus = (long) status;

      } //  if (!pParent->svmSendQueue.IsEmpty ())

      pDeferQLock->Unlock ();

	   Sleep (30);
      

   } //  while (!pParent->svmlStopDef)

   if (hpApplicomLib)
   {
      exitbus( &status );
      TRACE0 ("S7DM exitbus called\n");
   }

   return 0;
}        // S7DeferManager


//-------------------------------------------------------------------
// AddToRequestQueue
//    Add an element to the deferred request queue.  A separate thread
//    (S7DeferManager) will take the element from the queue and perform
//    the needed deferred mode transfer

bool  SVS7PBFMSClass::AddToRequestQueue( SVPLCBlockStruct* apBlock, long lOperation )
{

   SVPLCCommQueueStruct *pQueueElement;

   while (1)
   {
      apBlock->svmStatus = SV_PLCCOMMSTATUS_BEGIN;
      apBlock->svmulRawStatus = S7_STATUS_DEFREQ_NOTCOMPLETED;

      if (svmhApplicomLib)
      {
         pQueueElement = new SVPLCCommQueueStruct (apBlock, this);
         pQueueElement->svmlPendingOperation = lOperation;

         m_SVDeferQLock.Lock ();
         svmRequestQueue.AddTail (pQueueElement);
         m_SVDeferQLock.Unlock ();
      }
      
      break;
   }

   return true;
}        // AddToRequestQueue


//-------------------------------------------------------------------
// Clear registers
//
// This routine is called to make sure that the Applicom deferred registers
// are cleared.  This might occur if a readdif command were issued and the
// program interrupted before the transdif function could be called
//-------------------------------------------------------------------

void SVS7PBFMSClass:: ClearRegisters ()
{
   short    tabl[ 1024 ];
   short    status, pending, nb2;

   // Check to see if any transactions are pending
   if (svmhApplicomLib)
   {
//      testtransdif(&svmPLCChannelNo, &pending, &nb2, &status);
      testtransdif(&svmPLCChannelNo, &pending, &nb2, &status);
   }
   else
   {
      pending = 0;
   }

   if ( pending == 0 )
   {
      // No pending transactions - exit
      return;
   }

   do
   {
      // There is at least one pending transaction.  Keep calling
      // transdifpak until no more transactions are pending
      short nb, typ;
      transdifpack( &svmPLCChannelNo, &nb, &typ, tabl, &status );
      TRACE3 ("ClearRegisters: nb = %d, typ = %d, status = %d\n", nb, typ, status);
   } while ( status != S7_STATUS_EMPTY_TRANSDIF );
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVP_SRC\SVS7PBFMS.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 13:33:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attributes and lables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 11:21:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 May 2013 13:02:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:37:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 13:29:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:05:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:59:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2002 17:16:56   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  142
 * SCR Title:  Remove thread spawning from within constructors
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed 
 * 
 * SVS7PBFMSClass::~SVS7PBFMSClass()
 * 
 * to destroy threads in a safer manor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Nov 2001 15:33:34   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  142
 * SCR Title:  Remove thread spawning from within constructors
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following classes were modified:
 * SVS7PBFMSClass::SVS7PBFMSClass ()
 * SVS7PBFMSClass::~SVS7PBFMSClass ()
 * 
 * The following function was added:
 * SVS7PBFMSClass::Initialize ()
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2001 10:26:46   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  83
 * SCR Title:  Integrate string handling for Siemens PLCs.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modified to impliment reading strings from Siemends PLCs:
 *  SVS7PBFMSClass::SVS7PBFMSClass ()
 *  SVS7PBFMSClass::~SVS7PBFMSClass ()
 *  SVS7PBFMSClass::CheckStatus ()
 *  SVS7PBFMSClass::CopyReceiveBufferToString ()
 *  SVS7PBFMSClass::AddElementToBlock ()   SVS7PBFMSClass::Open ()
 *  SVS7PBFMSClass::AddToRequestQueue ()
 *  S7DeferManager ()
 * 
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/