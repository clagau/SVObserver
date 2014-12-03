// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC
// * .File Name       : $Workfile:   PLCBaseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:35:50  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCBaseClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "math.h"
#include "plc_interface.h"
#include "SVSystemLibrary\SVThreadManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




long     SVPLCBaseClass::svmlNbrOpen = 0;

CMutex   SVPLCBaseClass::svmNbrOpenLock;

long     SVPLCBaseClass::svmlNbrInstances = 0;

CMutex   SVPLCBaseClass::svmNbrInstancesLock;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SVPLCElementStruct::SVPLCElementStruct ()
{
  svmPLCFileNbr = 0;
  svmElementType = SV_PLC_UNKNOWN;
  svmElement = 0;
  svmlBitNum = -1;
  svmBitMask = 0;
  svmlParsed = FALSE; 
}


SVPLCBaseClass::SVPLCBaseClass (long* aplErr)
{
   *aplErr = 0;

//TRB Move SetupConnection
//   svmhRSLinxLib = NULL;
   //svmhApplicomLib = NULL;

   while (1)
   {
      svmlIsOpen = FALSE;
      svmTimeOutLimit = 999999;
      svmlInitialized = 0;

      svmNbrInstancesLock.Lock ();
      svmlNbrInstances = svmlNbrInstances + 1;
      svmNbrInstancesLock.Unlock ();

//TRB Move SetupConnection
//    	svmhRSLinxLib = LoadLibrary(_T("dtl32.dll"));
//    	svmhApplicomLib = LoadLibrary(_T("Applicom.dll"));
      break;
   }
}


long SVPLCBaseClass::Initialize ()
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      svmhSendThread = CreateThread (NULL, 
                                     0, 
                                     PLCSendManager, 
                                     this, 
                                     0, 
                                     &svmdwSendThreadId);
      if (!svmhSendThread)
      {
         lErr = -1150;
         break;
      }
	  SVThreadManager::Instance().Add( svmhSendThread, _T("PLC SendThread"));

      svmhResponseThread = CreateThread (NULL, 
                                         0, 
                                         PLCResponseManager, 
                                         this, 
                                         0, 
                                         &svmdwResponseThreadId);
      if (!svmhResponseThread)
      {
         lErr = -1151;
         break;
      }
	  SVThreadManager::Instance().Add( svmhResponseThread, _T("PLC ResponseThread") );

      svmlInitialized = 1;
      break;
   }

   return lErr;
}


long SVPLCBaseClass::IsInitialized ()
{
   if (svmlInitialized == 0)
   {
      return 0;
   }
   else
   {
      return 1;
   }
}


long SVPLCBaseClass::Open ()
{
   long  lErr;

   lErr = 0;
   while (1)
   {
      if (IsInitialized ())
      {
         svmlNbrOpen = svmlNbrOpen + 1;
      }
      else
      {
         lErr = -1364;
         break;
      }

      break;
   }

   return lErr;
}

long SVPLCBaseClass::Close ()
{
   svmlNbrOpen = svmlNbrOpen - 1;
   svmlIsOpen = FALSE;
   return 0;
}


long SVPLCBaseClass::SetTimeOutLimit (__int64 aTimeOutLimit)
{
   if (aTimeOutLimit == 0)
   {
      svmTimeOutLimit = 0xefff;
   }
   else
      svmTimeOutLimit = aTimeOutLimit;

   return 0;
}


SVPLCBaseClass::~SVPLCBaseClass()
{
   long        lErr;

   lErr = 0;

   while (1)
   {
      lErr = TerminateSendAndResponseThreads ();

      break;
   }
   OutputDebugString( "Exiting ~PLCBaseClass\n" );
}



long SVPLCBaseClass::TerminateSendAndResponseThreads ()
{

   long  lErr;

   DWORD       dwExitCode;


   lErr = 0;

   while (1)
   {

      svmlStopSM = TRUE;
      svmlStopRM = TRUE;

	  SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

      do
      {
		  SVThreadManager::Instance().Remove(svmhSendThread);
         GetExitCodeThread (svmhSendThread, &dwExitCode);
		 if( 10000 < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ) ) )
         {
            lErr = -1156;
	        TerminateThread (svmhSendThread,    // handle to the thread
					         0);   // exit code for the thread);
            break;
         }
      } while (dwExitCode == STILL_ACTIVE && lErr == 0);
      
      do
      {
		  SVThreadManager::Instance().Remove(svmhResponseThread);
         GetExitCodeThread (svmhResponseThread, &dwExitCode);
         if( 10000 < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ) ) )
         {
            lErr = -1157;
	        TerminateThread (svmhResponseThread,    // handle to the thread
					         0);   // exit code for the thread);
            break;
         }
      } while (dwExitCode == STILL_ACTIVE && lErr == 0);

      break;
   }

   return lErr;

}



long SVPLCBaseClass::CreateBlocks (SVPLCBlockStruct**       apBlock, 
                                   SVPLCElementStructArray* apAddressArray)
{
   return CreateBlocksEx (apBlock, apAddressArray, 0);
}



long SVPLCBaseClass::CreateBlocksEx (SVPLCBlockStruct** apBlock, 
                                     SVPLCElementStructArray* apAddressArray, 
                                     long alPLCFixedDataLength)
{
   long        lErr;
   long        lLoop;
   long        lNbrElements;
  
   SVPLCBlockStruct* pCurrentBlock;
   SVPLCBlockStruct* pPreviousBlock;

   SVPLCElementStructArray newAddressArray;

   lErr = 0;

   while (1)
   {
      *apBlock = NULL;

      lNbrElements = apAddressArray->GetSize ();

      if (lNbrElements == 0)
      {
//------ If there are no elements, then set the block pointer to NULL 
//------ (already done) and exit.
         break;
      }

      lErr = ParseElements (apAddressArray);
      if (lErr < 0)
         break;

      for (lLoop = 0; lLoop < lNbrElements; lLoop++)
      {
         ((*apAddressArray) [lLoop])->svmlOriginalIndex = lLoop;
      }

      newAddressArray.Copy (*apAddressArray);

      lErr = SortElements (&newAddressArray);
      if (lErr < 0)
         break;

      *apBlock = new SVPLCBlockStruct;
      if (*apBlock == NULL)
      {
         lErr = -1188;
         break;
      }

      pCurrentBlock = *apBlock;

      pCurrentBlock->svmlPLCDataFixedLength = alPLCFixedDataLength;
      pCurrentBlock->svmlPCDataFixedLength =  alPLCFixedDataLength;

      for (lLoop = 0; lLoop < lNbrElements; lLoop++)
      {
//------ AddElementToBlock () must be able to handle empty blocks.
         lErr = AddElementToBlock (pCurrentBlock, newAddressArray [lLoop]);
         if (lErr == 1)
         {
//--------- ELEMENT DOES NOT BELONG TO pCurrentBlock ------------------------
            pCurrentBlock->svmpNextBlock = new SVPLCBlockStruct;
            pPreviousBlock = pCurrentBlock;
            pCurrentBlock = pCurrentBlock->svmpNextBlock;
            if (pCurrentBlock == NULL)
            {
               lErr = -1189;
               break;
            }

            pCurrentBlock->svmlPLCDataFixedLength = alPLCFixedDataLength;
            pCurrentBlock->svmlPCDataFixedLength = alPLCFixedDataLength;

            pCurrentBlock->svmpPreviousBlock = pPreviousBlock;
            pCurrentBlock->svmpNextBlock = NULL;

            lErr = 
               AddElementToBlock (pCurrentBlock, newAddressArray [lLoop]);
//--------- End of ELEMENT DOES NOT BELONG TO pCurrentBlock. ----------------
         }

         if (lErr < 0)
            break;
      }  // for (lLoop = 0; lLoop < lNbrElements; lLoop)

      lErr = InitializeBlocks (*apBlock);
      
      break;
   }  // while (1)

   return lErr;
}



long SVPLCBaseClass::DestroyBlocks (SVPLCBlockStruct** appBlock)
{
   long     lErr;

   SVPLCBlockStruct* pCurrentBlock;
   SVPLCBlockStruct* pNextBlock;

   lErr = 0;

   while (1)
   {
      pCurrentBlock = *appBlock;

      while (pCurrentBlock != NULL)
      {
         lErr = DeinitializeBlocks (pCurrentBlock);

         pNextBlock = pCurrentBlock->svmpNextBlock;
         delete pCurrentBlock;

         pCurrentBlock = pNextBlock;
      }

      *appBlock = NULL;

      break;
   }

   return lErr;
}



long SVPLCBaseClass::InitializeBlocks (SVPLCBlockStruct* apBlock)
{
// No initialization is required at the base class level.
   return 0;
}



long SVPLCBaseClass::DeinitializeBlocks (SVPLCBlockStruct* apBlock)
{
// No initialization is required at the base class level.
   return 0;
}



long SVPLCBaseClass::OrInt (long* aplValue, SVPLCElementStruct* apAddress)
{

   long        lErr;
   long        lPLCValue;

   lErr = 0;

   while (1)
   {
      lErr = GetInt (&lPLCValue, apAddress);
      if (lErr < 0)
         break;

      *aplValue = lPLCValue | *aplValue; 

      lErr = SendInt (*aplValue, apAddress);
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



long SVPLCBaseClass::AndInt (long* aplValue, SVPLCElementStruct* apAddress)
{


   long        lErr;
   long        lPLCValue;

   lErr = 0;

   while (1)
   {
      lErr = GetInt (&lPLCValue, apAddress);
      if (lErr < 0)
         break;

      *aplValue = lPLCValue & *aplValue; 

      lErr = SendInt (*aplValue, apAddress);
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



long SVPLCBaseClass::SendBit (long alValue, SVPLCElementStruct* apAddress)
{

   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;


   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      lErr = SendBitBlock (&alValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;

}



long SVPLCBaseClass::SendInt (long alValue, SVPLCElementStruct *apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;


   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      lErr = SendIntBlock (&alValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}


long SVPLCBaseClass::SendFloat (double adValue, SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;


   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      lErr = SendFloatBlock (&adValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}

/*
long SVPLCBaseClass::SendFloat (double adValue, SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   lErr = 0;

   while (1)
   {
      CopyBlockConnectionData (&block, &svmDefaultBlock);
      CopyElementAddressToBlock (&block, apAddress);
      block.svmpIndividualElementArray.Add (apAddress);

      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;
      
      lErr = SendFloatBlock (&adValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;

      break;
   }

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}
*/

/*
long SVPLCBaseClass::SendString (TCHAR*              aptczValue, 
                               long                alFixedLength, 
                               SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct block;

   lErr = 0;

   while (1)
   {
      block.svmlPLCDataFixedLength = alFixedLength;
      block.svmlPCDataFixedLength = alFixedLength;
      CopyBlockConnectionData (&block, &svmDefaultBlock);
      CopyElementAddressToBlock (&block, apAddress);
      block.svmpIndividualElementArray.Add (apAddress);

      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;
      
      lErr = SendStringBlock (aptczValue, alFixedLength, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;

      break;
   }

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}
*/

long SVPLCBaseClass::SendString (char*              aptczValue, 
                               long                alFixedLength, 
                               SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;


   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      CopyElementAddressToBlock (&block, apAddress);
      block.svmlPLCDataFixedLength = alFixedLength;
      block.svmlPCDataFixedLength = alFixedLength;
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      lErr = SendStringBlock ( &aptczValue, alFixedLength, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}

HRESULT SVPLCBaseClass::SendIntBlock( char** p_apstrAddresses, long* p_aplValues, long p_lSize)
{

	HRESULT hr=S_OK;
	SVPLCBlockStruct*        pBlock = NULL;

	SVPLCElementStructArray tempCArray;

	//--- We need to place the element in this array in order to parse the
	//--- address.
	SVPLCElementStruct* l_pElement = new SVPLCElementStruct[p_lSize];

	for( int i = 0 ; i < p_lSize ; i++ )
	{
		// copy address to element
		_tcscpy(l_pElement[i].svmPLCAddress, *(p_apstrAddresses + i));
		l_pElement[i].svmlOriginalIndex = 0;
		tempCArray.Add( &l_pElement[i] );
	}

	hr = CreateBlocks( &pBlock, &tempCArray ); // needs DestroyBlocks when finished.
	if( hr == S_OK )
	{
		hr = SendIntBlock (p_aplValues, pBlock);
		if (hr == 0)
		{
			// This statements need managed better so we do not have to wait for them to complete.
			hr = WaitTillComplete (pBlock);
		}
		DestroyBlocks ( &pBlock );
	}
	delete[] l_pElement;

	return hr;
}

// written by Tom Bair..
HRESULT SVPLCBaseClass::SendStringBlock ( char** p_psAddresses, char** p_psValues, long p_lSize)
{
	HRESULT l_hr  = S_OK;

	SVPLCBlockStruct*        pBlock = NULL;

	SVPLCElementStructArray tempCArray;

	SVPLCElementStruct* l_pElement = new SVPLCElementStruct[p_lSize];

	long l_StringSize = DataTypeSize(SV_PLC_STRING);
			

	for( long i = 0 ; i < p_lSize ; i++ )
	{
		// ElementStruct...
		strcpy_s( l_pElement[i].svmPLCAddress, l_StringSize, p_psAddresses[i] );
		l_pElement[i].svmlOriginalIndex = 0;
		tempCArray.Add( &l_pElement[i] );
	}

	l_hr = CreateBlocks( &pBlock, &tempCArray ); // needs DestroyBlocks when finished.
	if( l_hr == S_OK )
	{
		l_hr = SendStringBlock( p_psValues, l_StringSize, pBlock);
		if (l_hr == 0)
		{
			// This statements need managed better so we do not have to wait for completion.
			l_hr = WaitTillComplete (pBlock);
		}
		DestroyBlocks ( &pBlock );
	}
	delete[] l_pElement;

	return l_hr;
}



long SVPLCBaseClass::SendIntBlock (long* aplValueArray, SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1261;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) aplValueArray;
         pBlock->svmcpTxRxBuffer = 
            new char [pBlock->svmlNbrElements * 
                      lDataTypeSize];

         pBlock->svmPCDataType = SV_PCTYPE_INT;


         lErr = CopyLongToSendBuffer (pBlock);  

         if (lErr < 0)
            break;

         lErr = SendBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;

      }

      break;
   }

   return lErr;
}



long SVPLCBaseClass::SendFloatBlock (double* apfValueArray, SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1257;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) apfValueArray;
         pBlock->svmcpTxRxBuffer = 
            new char [pBlock->svmlNbrElements * 
                      lDataTypeSize];

         lErr = CopyDoubleToSendBuffer (pBlock);  

         pBlock->svmPCDataType = SV_PCTYPE_FLOAT;

         if (lErr < 0)
            break;

         lErr = SendBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;

      }

      break;
   }

   return lErr;
}



long SVPLCBaseClass::SendStringBlock (char**          aptczValueArray, 
                                    long              alFixedLength, 
                                    SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   int l_iStringCount = 0;
   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1256;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) *(aptczValueArray + l_iStringCount);

         if (pBlock->svmPLCDataType == SV_PLC_STRING)
         {
            pBlock->svmcpTxRxBuffer = 
              new char [pBlock->svmlNbrElements * 
                        lDataTypeSize + 1];
         }
         else
         {
            pBlock->svmcpTxRxBuffer = 
              new char [pBlock->svmlNbrElements * 
                        lDataTypeSize * 
                        pBlock->svmlPLCDataFixedLength];
         }

         pBlock->svmPCDataType = SV_PCTYPE_STRING;
         lErr = CopyStringToSendBuffer (pBlock);  

         pBlock->svmPCDataType = SV_PCTYPE_STRING;

         if (lErr < 0)
            break;

         lErr = SendBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;
		l_iStringCount++;
      }
      break;
   }
   return lErr;
}



long SVPLCBaseClass::SendBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock)
{
   return -1352;
}



long SVPLCBaseClass::SendBlock (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   SVPLCCommQueueStruct *pQueueElement;


   lErr = 0;

   while (1)
   {
      apBlock->svmStatus = SV_PLCCOMMSTATUS_BEGIN;

      pQueueElement = new SVPLCCommQueueStruct (apBlock);
      pQueueElement->svmlPendingOperation = 2;

      SVSendQLock.Lock ();
      svmSendQueue.AddTail (pQueueElement);
      SVSendQLock.Unlock ();
      
      break;
   }

   return lErr;
}



long SVPLCBaseClass::GetString (TCHAR* aptczValue, 
                                long alFixedLength, 
                                SVPLCElementStruct* apAddress)
{

   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;


   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);

      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;
      block.svmlPLCDataFixedLength = alFixedLength;
      block.svmlPCDataFixedLength = alFixedLength;
      
      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      if (lErr < 0)
         break;

      lErr = GetStringBlock (aptczValue, alFixedLength, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;


      break;
   }


   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}

long SVPLCBaseClass::GetLong (long* aplValue, SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;

   lErr = 0;

   while (1)
   {

//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      if (lErr < 0)
         break;

      lErr = GetLongBlock (aplValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}




long SVPLCBaseClass::GetFloat (double* apdValue, SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;

   lErr = 0;

   while (1)
   {
//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);

      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;
      
      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      if (lErr < 0)
         break;

      lErr = GetFloatBlock (apdValue, &block);

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      break;
   }

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}



long SVPLCBaseClass::GetInt (long* aplValue, SVPLCElementStruct* apAddress)
{
   __int64                 lErr;

   SVPLCBlockStruct        block;

   SVPLCElementStructArray tempCArray;

   lErr = 0;

   while (1)
   {

//--- We need to place the element in this array in order to parse the
//--- address.
      tempCArray.Add (apAddress);
      apAddress->svmlOriginalIndex = 0;

      lErr = ParseElements (&tempCArray);
      if (lErr < 0)
         break;

      CopyBlockConnectionData (&block, &svmDefaultBlock);
      block.svmpNextBlock = NULL;
      block.svmpPreviousBlock = NULL;

      AddElementToBlock (&block, apAddress);

      lErr = InitializeBlocks (&block);

      if (lErr < 0)
         break;

      lErr = GetIntBlock (aplValue, &block);
      if (lErr < 0)
         break;

      lErr = WaitTillComplete (&block);

      if (lErr < 0)
         break;
      break;
   }

   DeinitializeBlocks (&block);

   if (lErr < 0)
      return (long) lErr;
   else
      return 0;
}



long SVPLCBaseClass::GetIntBlock (long* valueArray, 
                                SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1258;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) valueArray;
         pBlock->svmcpTxRxBuffer = new char [pBlock->svmlNbrElements * lDataTypeSize];

         pBlock->svmPCDataType = SV_PCTYPE_INT;

         lErr = GetBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;
      }

      break;
   }

   return lErr;
}


long SVPLCBaseClass::GetLongBlock (long* valueArray, 
                                SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1258;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) valueArray;
         pBlock->svmcpTxRxBuffer = new char [pBlock->svmlNbrElements * lDataTypeSize];

         pBlock->svmPCDataType = SV_PCTYPE_INT;

         lErr = GetBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;
      }

      break;
   }

   return lErr;
}



long SVPLCBaseClass::GetFloatBlock (double* apfValueArray, 
                                  SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1259;
            break;
         }

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);
         if (lDataTypeSize < 0)
         {
            lErr = lDataTypeSize;
            break;
         }

         pBlock->svmpParamBuffer = (char *) apfValueArray;
         pBlock->svmcpTxRxBuffer = new char [pBlock->svmlNbrElements * lDataTypeSize];

         pBlock->svmPCDataType = SV_PCTYPE_FLOAT;

         lErr = GetBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;
      }

      break;
   }

   return lErr;
}



long SVPLCBaseClass::GetStringBlock (TCHAR*              aptcValueArray, 
                                     int                 aFixedLength,
                                     SVPLCBlockStruct*   apBlock)
{
   long        lErr;
   long        lDataTypeSize;

   SVPLCBlockStruct* pBlock;  

   lErr = 0;


   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {

         if ((pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE) &&
             (pBlock->svmStatus != SV_PLCCOMMSTATUS_ERROR))
         {
            lErr = -1260;
            break;
         }

         pBlock->svmpParamBuffer = (char *) aptcValueArray;

         lDataTypeSize = DataTypeSize (pBlock->svmPLCDataType);

         pBlock->svmlPCDataFixedLength = aFixedLength;

//------ If initiated through call to GetString (), the fixed lengths will 
//------ already be calculated.  We're doing array processing, and the
//------ PLCDataFixedLength must be calculated, the following algorithm is
//------ complete because IT DOES NOT account for block size limitations.
         if (pBlock->svmlPLCDataFixedLength == 0)
         {
            apBlock->svmlPLCDataFixedLength = 
               apBlock->svmlPCDataFixedLength / lDataTypeSize;
         }

         if (lDataTypeSize > aFixedLength)
         {
            pBlock->svmcpTxRxBuffer = new char [lDataTypeSize * pBlock->svmlNbrElements];
         }
         else
         {
            pBlock->svmcpTxRxBuffer = new char [aFixedLength * pBlock->svmlNbrElements];
         }

         pBlock->svmPCDataType = SV_PCTYPE_STRING;

         lErr = GetBlock (pBlock);
         if (lErr < 0)
            break;

         pBlock = pBlock->svmpNextBlock;
      }

      break;
   }

   return lErr;
}



long SVPLCBaseClass::GetBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock)
{
   return -1353;
}



long SVPLCBaseClass::GetBlock (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   SVPLCCommQueueStruct *pQueueElement;


   lErr = 0;

   while (1)
   {
      apBlock->svmStatus = SV_PLCCOMMSTATUS_BEGIN;

      pQueueElement = new SVPLCCommQueueStruct (apBlock);
      pQueueElement->svmlPendingOperation = 1;

      SVSendQLock.Lock ();
      svmSendQueue.AddTail (pQueueElement);
      SVSendQLock.Unlock ();
      
      break;
   }

   return lErr;
}



long SVPLCBaseClass::WaitTillComplete (SVPLCBlockStruct* apBlock)
{

   long  lErr;
   long	lStatus;
   long  lTimeOut;
   double TimeOutValue;

   SVPLCBlockStruct* pBlock;

   lErr = 0;
   lTimeOut = FALSE;

   SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

   pBlock = apBlock;

   while (pBlock)
   {
      do
      {
		 Sleep(15);	// TB This hogs up too much processing time.

		 lStatus = apBlock->svmStatus;

//------ Time out should be handled in the ResponseManager ().
         TimeOutValue = static_cast<double>(svmTimeOutLimit * 3);

         if (TimeOutValue < 0.0)
			TimeOutValue = SVClock::GetMinTimeStamp();

		 if ((TimeOutValue < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start ))) && (lTimeOut == FALSE))
         {
			   lTimeOut = TRUE;
			   CancelTransaction (apBlock, -1251);
            continue;
         }

//         lErr = CheckStatus (pBlock);
//         if (lErr < 0)
//            break;

         if (lStatus == SV_PLCCOMMSTATUS_PENDING)
         {
            Sleep (30);
         }

         if (lStatus == SV_PLCCOMMSTATUS_ERROR)
         {
            lErr = pBlock->svmlCommunicationError;
            break;
         }

      } while (pBlock->svmStatus != SV_PLCCOMMSTATUS_COMPLETE);

      pBlock = pBlock->svmpNextBlock;
   }

   return lErr;
}


// CancelTransaction () should be used causiously. Any non-RSLinx origin
// to cancelling a transaction will result in the receive buffer (and possibly 
// the entire block structure) that RSLinx was handed being deleted. This 
// will result in a crash if RSLinx ever does respond.

long SVPLCBaseClass::CancelTransaction (SVPLCBlockStruct* apBlock, long aErr)
{

   while (1)
   {
      SVSendQLock.Lock ();
      SVPendingQLock.Lock ();

      apBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
      apBlock->svmlCommunicationError = aErr;

      SVPendingQLock.Unlock ();
      SVSendQLock.Unlock ();

		break;
	}

	return 0;
}



long SVPLCBaseClass::CopyBlockConnectionData (SVPLCBlockStruct* apDestBlock, 
                                            SVPLCBlockStruct* apSourceBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      sv_tstrncpy (apDestBlock->svmInternalConnectionName, 
                   apSourceBlock->svmInternalConnectionName,
                   SV_NBRCHARS (apDestBlock->svmInternalConnectionName));

      break;
   }

   return lErr;
}



long SVPLCBaseClass::CopyElementAddressToBlock (SVPLCBlockStruct* apDestBlock,
                                              SVPLCElementStruct *apElement)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      sv_tstrncpy (apDestBlock->svmConnectionName,
                   apElement->svmConnectionName,
                   SV_NBRCHARS (apDestBlock->svmConnectionName));

      sv_tstrncpy (apDestBlock->svmTopicName,
                   apElement->svmTopicName,
                   SV_NBRCHARS (apDestBlock->svmTopicName));

      sv_tstrncpy (apDestBlock->svmItemName,
                   apElement->svmItemName,
                   SV_NBRCHARS (apDestBlock->svmItemName));

      apDestBlock->svmlPLCFileNbr = apElement->svmPLCFileNbr;
      apDestBlock->svmPLCDataType = apElement->svmElementType;
      apDestBlock->svmlFirstElement = apElement->svmElement;

      if (apDestBlock->svmlPLCDataFixedLength <= DataTypeSize (apElement->svmElementType))
         apDestBlock->svmlNbrElements = 1;
      else
         apDestBlock->svmlNbrElements = static_cast<long>(ceil (static_cast<double>(apDestBlock->svmlPLCDataFixedLength) / 
                                              DataTypeSize (apElement->svmElementType)));


//      if (apDestBlock->svmlPLCDataFixedLength % 
//          DataTypeSize (apElement->svmElementType))
//      {
//         apDestBlock->svmlNbrElements = apDestBlock->svmlNbrElements + 1;
//      }

      break;
   }

   return lErr;

}



long SVPLCBaseClass::AddElementToBlock (SVPLCBlockStruct*     apBlock,
                                      SVPLCElementStruct*   apElement)

{
   return -1197;
}



long SVPLCBaseClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   return -1158;
}



long SVPLCBaseClass::ParseElements (SVPLCElementStructArray* apAddress)
{
   return -1189;
}



long SVPLCBaseClass::QuickSortElements (SVPLCElementStructArray*  apSortArray,
                                        long                      alBeginning,
                                        long                      alEnd)
{
   return -1195;
}


long SVPLCBaseClass::SortElements (SVPLCElementStructArray* apAddress)
{
   long  lErr;
   lErr = 0;

   while (1)
   {
      lErr = QuickSortElements (apAddress,
                                0,
                                apAddress->GetSize () - 1);

      break;
   }

   return lErr;
}



long SVPLCBaseClass::SendB (SVPLCBlockStruct* apBlock)
{
   return -1169;
}



long SVPLCBaseClass::SendInt16 (SVPLCBlockStruct* apBlock)
{
   return -1184;
}



long SVPLCBaseClass::SendInt32 (SVPLCBlockStruct* apBlock)
{
   return -1185;
}



long SVPLCBaseClass::SendStr (SVPLCBlockStruct* apBlock)
{
   return -1198;
}



long SVPLCBaseClass::SendASCII (SVPLCBlockStruct* apBlock)
{
   return -1199;
}



long SVPLCBaseClass::SendSPFloat (SVPLCBlockStruct* apBlock)
{
   return -1200;
}



long SVPLCBaseClass::SendDPFloat (SVPLCBlockStruct* apBlock)
{
   return -1201;
}



long SVPLCBaseClass::GetB (SVPLCBlockStruct* apBlock)
{
   return -1202;
}



long SVPLCBaseClass::GetInt16 (SVPLCBlockStruct* apBlock)
{
   return -1203;
}



long SVPLCBaseClass::GetInt32 (SVPLCBlockStruct* apBlock)
{
   return -1204;
}



long SVPLCBaseClass::GetStr (SVPLCBlockStruct* apBlock)
{
   return -1205;
}



long SVPLCBaseClass::GetASCII (SVPLCBlockStruct* apBlock)
{
   return -1206;
}



long SVPLCBaseClass::GetSPFloat (SVPLCBlockStruct* apBlock)
{
   return -1207;
}



long SVPLCBaseClass::GetDPFloat (SVPLCBlockStruct* apBlock)
{
   return -1208;
}


long SVPLCBaseClass::GetMaxDTSize( void )
{
	return 512;
}

long SVPLCBaseClass::TestCommunications()
{
  return -1210;
}

/*
long SVPLCBaseClass::GetValue (TCHAR*   apczSourceString, 
                               TCHAR*   apczKeyString, 
                               TCHAR*   apczValueString,
                               long     alLength)
{
   TCHAR*   pntr1;
   TCHAR*   pntr2;
   TCHAR    czWhiteSpace [40];

   long  lFound;
   long  lErr;
   long  lIndex;

   lFound = FALSE;
   lErr = 0;

   while (1)
   {
//--- Yes, I am treating a comma as white space.
      _tcscpy (czWhiteSpace, _T("\a\b\f\n\r\t\v ,"));
      pntr1 = apczSourceString;

      apczValueString [0] = 0;

      while ((pntr1 = _tcsstr (pntr1, 
                               apczKeyString)) != NULL)
      {
         pntr2 = _tcsstr (pntr1, _T("="));

         if (pntr2 == NULL)
         {
            lErr = -1317;
            break;
         }

         if ((pntr1 != apczSourceString) &&
             (_tcschr(czWhiteSpace, *(pntr1 - 1)) == NULL))
         {
//--------- If we are not at the beginning of the string, and the previous
//--------- character is not whitespace, then this is not the correct key.
            pntr1 = pntr1 + 1;
            continue;
         }

//------ Move passed the currently found string.
         pntr1 = pntr1 + _tcslen (apczKeyString);

//------ Move passed any white space.
         pntr1 = pntr1 + _tcsspn (pntr1, czWhiteSpace);

         if (pntr2 != pntr1)
         {
//--------- If we are not looking at an equal sign... 
            pntr1 = pntr2;
            continue;
         }

//------ Move passed equal sign.
         pntr1 = pntr1 + 1;

//------ Move passed any white space.
         pntr1 = pntr1 + _tcsspn (pntr1, czWhiteSpace);

         lIndex = 0;
         while ((_tcschr(czWhiteSpace, *pntr1) == NULL) &&
                (lIndex < (alLength - 1)))
         {
            apczValueString [lIndex] = *pntr1;
            lIndex = lIndex + 1;
            pntr1 = pntr1 + 1;
         }

         apczValueString [lIndex] = 0;

         lFound = TRUE;
      } // while ((pntr1 = _tcsstr (pntr1, apczKeyString)) != NULL)

//if (apValueString->GetLength () != 0)
//AfxMessageBox (*apValueString);

      break;
   } // while (1)

   return lErr;
}
*/

/*
//TRB Move SetupConnection


long SVPLCBaseClass::SetupConnection(TCHAR* apszConnection, long  alSize)
{

   long  lErr;
   long  lConnectionParameterStringSize;
   
   SetupDialogSheetClass SetupDlg (_T("PLC Connection"));

   lErr = 0;

   while (1)
   {
      lErr = ParseSetupParameterString (apszConnection);

      if (lErr < 0)
         break;

      SetupDlg.svmChannelNbr.Format (svmczChannelNbr);
      SetupDlg.svmEquipmentNbr.Format (svmczEquipmentNbr);
      SetupDlg.svmManufacturer.Format (svmczManufacturer);
      SetupDlg.svmPLCType.Format (svmczPLCType);
      SetupDlg.svmConnection.Format (svmczConnection);
      SetupDlg.svmDriverName.Format (svmczDriverName);
      SetupDlg.svmIPAddress.Format (svmczIPAddress);
      SetupDlg.svmDHNodeAddress.Format (svmczDHNodeAddress);
      SetupDlg.svmConnectionParameters.Format (apszConnection);

//TRB Move SetupConnection
      SetupDlg.svmhRSLinxLib = svmhRSLinxLib;
      SetupDlg.svmhApplicomLib = svmhApplicomLib;

      SetupDlg.SetWizardMode ();

      if (!SetupDlg.DoModal ())
      {
         lErr = -1322;
         break;
      }

      lConnectionParameterStringSize = SetupDlg.svmConnectionParameters.GetLength ();
      if (alSize <= lConnectionParameterStringSize)
      {
         lErr = -1334;
      }

      sv_tstrncpy (apszConnection, 
                   SetupDlg.svmConnectionParameters, 
                   lConnectionParameterStringSize);

//      (*apszConnection) = SetupDlg.svmConnectionParameters;
      break;
   }


   return lErr;
}
*/


/*
long SVPLCBaseClass::ParseSetupParameterString (TCHAR* apszConnection)
{

   long  lErr;

   lErr = 0;

   while (1)
   {

      lErr = GetValue (apszConnection,
                       SV_CHANNEL_NBR,
                       svmczChannelNbr,
                       SV_NBRCHARS (svmczChannelNbr));
      if (lErr < 0)
         break;
               
      lErr = GetValue (apszConnection,
                       SV_EQUIPMENT_NBR,
                       svmczEquipmentNbr,
                       SV_NBRCHARS (svmczEquipmentNbr));
      if (lErr < 0)
         break;
      
      lErr = GetValue (apszConnection,
                       SV_MANUFACTURER,
                       svmczManufacturer,
                       SV_NBRCHARS (svmczManufacturer));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_PLC_TYPE,
                       svmczPLCType,
                       SV_NBRCHARS (svmczPLCType));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_CONNECTION_TYPE,
                       svmczConnection,
                       SV_NBRCHARS (svmczConnection));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_DRIVER_NAME,
                       svmczDriverName,
                       SV_NBRCHARS (svmczDriverName));
      if (lErr < 0)
         break;

      lErr = GetValue (apszConnection,
                       SV_IP_ADDRESS,
                       svmczIPAddress,
                       SV_NBRCHARS (svmczIPAddress));
      if (lErr < 0)
         break;
      
      lErr = GetValue (apszConnection,
                       SV_DHNODE_ADDRESS,
                       svmczDHNodeAddress,
                       SV_NBRCHARS (svmczDHNodeAddress));
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}
*/


long SVPLCBaseClass::SwapBytes (char* apcDestination, 
                                char* apcSource, 
                                long alNumberOfWords)
{
   long     lErr;
   long     lLoop;
   short*   lpWordPointer;


   lErr = 0;

   while (1)
   {
      lpWordPointer = (short*) apcDestination;


      for (lLoop = 0; lLoop < alNumberOfWords; lLoop++)
      {
         lpWordPointer [lLoop] = (apcSource [lLoop * 2] << 8) + apcSource [(lLoop * 2) + 1];
      }
      
      break;
   }


   return lErr;
}



DWORD WINAPI PLCResponseManager (LPVOID apCallingClass)
{
   long                 lErr;
   long                 lIsPendingQueueEmpty;

   SVPLCBaseClass*        pParent;
   SVPLCCommQueueStruct* pCurrent;

   POSITION             queuePosition;
   POSITION             prevQueuePosition;

   lErr = 0;
   pParent = (SVPLCBaseClass *) apCallingClass;

   pParent->svmlStopRM = FALSE;

   while (!pParent->svmlStopRM)
   {
      prevQueuePosition = NULL;
      pParent->SVPendingQLock.Lock ();
      if (!pParent->svmPendingQueue.IsEmpty ())
      {
//		 TRACE1 ("RM number of elements on queue = %d\n", pParent->svmPendingQueue.GetCount ());

         queuePosition = pParent->svmPendingQueue.GetHeadPosition ();
         if (queuePosition == NULL)
         {
//--------- This setting of SV_PLCCOMMSTATUS_ERROR is not realy kosher, but 
//--------- since the program is seriously screwed at this point, it's the 
//--------- only real way to indicate the problem to the calling thread.
            pCurrent->svmpBlock->svmlCommunicationError = -1168;
            pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            continue;
         }

         pCurrent = pParent->svmPendingQueue.GetAt (queuePosition);


         while 
            (pCurrent && !pParent->svmlStopRM)
         {

//            TRACE1 ("RM: pCurrent = %p\n", pCurrent);

            pParent->CheckStatus (pCurrent->svmpBlock);

//		      TRACE2 ("RM; blockpntr = %p, status = %d\n", pCurrent->svmpBlock, pCurrent->svmpBlock->svmStatus);
//		      TRACE1 ("RM; address = %S\n", &(pCurrent->svmpBlock->svmRebuiltDTAddress [0]));
//            TRACE1 ("RM: status = %d\n", pCurrent->svmpBlock->svmStatus);
            if (pCurrent->svmpBlock->svmStatus == 
                SV_PLCCOMMSTATUS_ALMOSTCOMPLETE)
            {
//               TRACE0 ("RM: SV_PLCCOMMSTATUS_ALMOSTCOMPLETE\n");
//------------ Response was successfully received.
               pParent->svmPendingQueue.RemoveAt (queuePosition);
               switch (pCurrent->svmlPendingOperation)
               {
               case 1:  //read operation
                  {
                     switch (pCurrent->svmpBlock->svmPCDataType)
                     {
                     case SV_PCTYPE_BIT:
                        {
                           break;
                        }
                     case SV_PCTYPE_INT:
                        {
                           lErr = pParent->
                              CopyReceiveBufferToLong (pCurrent->svmpBlock);

                           pCurrent->svmpBlock->svmlCommunicationError = 
                              lErr;
                           break;

                        }
                     case SV_PCTYPE_STRING:
                        {
                           lErr = pParent->
                              CopyReceiveBufferToString (pCurrent->svmpBlock);

                           pCurrent->svmpBlock->svmlCommunicationError = 
                              lErr;
                           break;
                        }
                     case SV_PCTYPE_FLOAT:
                        {
                           lErr = pParent->
                              CopyReceiveBufferToFloat (pCurrent->svmpBlock);

                           pCurrent->svmpBlock->svmlCommunicationError = 
                              lErr;

                           break;
                        }
                     default:
                        {
                           lErr = -1236;
                           pCurrent->svmpBlock->svmlCommunicationError = lErr;
                           break;
                        }

                     }
                     break;
                  }
               case 2:  //write operation
                  {
// no reason to do this on a write operation
//                     pParent->TransferToBuffer (pCurrent->svmpBlock);

                     break;
                  }
               default:
                  {
                     pCurrent->svmpBlock->svmlCommunicationError = -1235;

                     break;
                  }
               } // switch (pCurrent->svmlPendingOperation)

               if (pCurrent->svmpBlock->svmlCommunicationError < 0)
               {
//                  TRACE1 ("RM: communication error = %d\n", pCurrent->svmpBlock->svmlCommunicationError);
                  pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_ERRORDETECTED;
               }
               else
               {
                  delete [] pCurrent->svmpBlock->svmcpTxRxBuffer;
                  pCurrent->svmpBlock->svmcpTxRxBuffer = NULL;

                  pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_COMPLETE;

                  delete pCurrent;
                  pCurrent = NULL;
               }
            } // if (pCurrent->svmpBlock->svmStatus == SV_PLCCOMMSTATUS_ALMOSTCOMPLETE)
            else
            {
//------------ Response was not received.
//               TRACE0 ("RM: Response not received\n");
               if (pCurrent->svmpBlock->svmStatus == 
                   SV_PLCCOMMSTATUS_PENDING)
               {
//                  TRACE2 ("RM: prev position = %d, q position = %d\n", prevQueuePosition, queuePosition);
                  prevQueuePosition = queuePosition;
               }
               else
               if (pCurrent->svmpBlock->svmStatus == 
                   SV_PLCCOMMSTATUS_ERRORDETECTED)
               {
//--------------- Transmission error detected.

                  pParent->svmPendingQueue.RemoveAt (queuePosition);

                  delete [] pCurrent->svmpBlock->svmcpTxRxBuffer;
                  pCurrent->svmpBlock->svmcpTxRxBuffer = NULL;

                  pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
//                  TRACE1 ("RM: transmission error = %d\n", pCurrent->svmpBlock->svmlCommunicationError);
                  delete pCurrent;
               }
            }

            lIsPendingQueueEmpty = pParent->svmPendingQueue.IsEmpty ();
            if (lIsPendingQueueEmpty ||
                (prevQueuePosition == NULL)) 
            {
//               TRACE0 ("RM: pending queue is empty\n");
               pCurrent = NULL;
            }

            queuePosition = prevQueuePosition;

//--------- The following conditional block assumes that when items are 
//--------- removed from the list, that the "current" item was previous of
//--------- the deleted item. NOT TRUE!!!!!!!
            if ((queuePosition != NULL) && 
                (pCurrent))
            {
//               TRACE2 ("RM: 2prev position = %d, q position = %d\n", prevQueuePosition, queuePosition);
//               TRACE0 ("RM: Get Next\n");
               pParent->svmPendingQueue.GetNext (queuePosition);

               if (queuePosition == NULL)
               {
                  pCurrent = NULL;
               }
               else
               {
                  pCurrent = pParent->svmPendingQueue.GetAt (queuePosition);
               }
            }
            else
            {
//               TRACE0 ("RM: NO Get Next\n");
            }
            
         }  // while (pCurrent && !pParent->svmlStopRM)

      } // if (!pParent->svmPendingQueue.IsEmpty ())


      pParent->SVPendingQLock.Unlock ();

 	  Sleep (30);


   }  // while (!pParent->svmlStopRM)
   OutputDebugString( "Exiting PLCResponseManager Task\n" );
   return 0;
}


DWORD WINAPI PLCSendManager (LPVOID apCallingClass)
{
   long					lErr;
   long					lTemp = 0;

   SVPLCCommQueueStruct* pCurrent;

   SVPLCBaseClass*      pParent;

   POSITION             queuePosition;
   POSITION             prevQueuePosition;


   pParent = (SVPLCBaseClass*) apCallingClass;

   pParent->svmlStopSM = FALSE;

   OutputDebugString( "Started PLCSendManager Task\n");
   while (!pParent->svmlStopSM)
   {
      lErr = 0;

      prevQueuePosition = NULL;

      pParent->SVSendQLock.Lock ();
      if (!pParent->svmSendQueue.IsEmpty ())
      {
         queuePosition = pParent->svmSendQueue.GetHeadPosition ();
         pCurrent = pParent->svmSendQueue.GetAt (queuePosition);

         pCurrent->svmpBlock->svmulRawStatus = 0xffff;

         switch (pCurrent->svmlPendingOperation)
         {
         case 1: // read, Get
            {
               switch (pCurrent->svmpBlock->svmPLCDataType)
               {
               case SV_PLC_BIT:
                  {
//                     TRACE1 ("SM: Define ID = %d\n", pCurrent->svmpBlock->svmlTransactionId);
                     lErr = pParent->GetB (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_INT16:
                  {
                     lErr = pParent->GetInt16 (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_INT32:
                  {
                     lErr = pParent->GetInt32 (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_STRING:
                  {
                     lErr = pParent->GetStr (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
/*
               case SV_PLC_ASCII:
                  {
                     pParent->SendASCII (pCurrent->svmpBlock);
                     break;
                  }
*/
               case SV_PLC_SPFLOAT:
                  {
                     lErr = pParent->GetSPFloat (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_DPFLOAT:
                  {
                     lErr = pParent->GetDPFloat (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               default:
                  {
                     lErr = -1152;
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }

               }
               break;
            }
         case 2: // write
            {
               switch (pCurrent->svmpBlock->svmPLCDataType)
               {
               case SV_PLC_BIT:
                  {
                     lErr = pParent->SendB (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_INT16:
                  {
                     lErr = pParent->SendInt16 (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_INT32:
                  {
                     lErr = pParent->SendInt32 (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_STRING:
                  {
                     lErr = pParent->SendStr (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_ASCII:
                  {
                     lErr = pParent->SendASCII (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;

                     break;
                  }
               case SV_PLC_SPFLOAT:
                  {
                     lErr = pParent->SendSPFloat (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               case SV_PLC_DPFLOAT:
                  {
                     lErr = pParent->SendDPFloat (pCurrent->svmpBlock);
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }
               default:
                  {
                     lErr = -1152;
                     pCurrent->svmpBlock->svmlCommunicationError = lErr;
                     break;
                  }

               } // switch (pCurrent->svmpBlock->svmPLCDataType)
               break;
            }  // case 2:
         default:
            {
               lErr = -1262;
               pCurrent->svmpBlock->svmlCommunicationError = lErr;
               break;
            }
         } // switch (pCurrent->svmlPendingOperation)

         pParent->svmSendQueue.RemoveAt (queuePosition);

         if (pCurrent->svmpBlock->svmlCommunicationError < 0)
         {
            pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            delete pCurrent;
         }
         else
         {
            
            pParent->SVPendingQLock.Lock ();

//            TRACE1 ("SM: pCurrent = %p\n", pCurrent);
//	         TRACE2 ("SM; blockpntr = %p, status = %d\n", pCurrent->svmpBlock, pCurrent->svmpBlock->svmStatus);
//		      TRACE1 ("SM; address = %S\n", &(pCurrent->svmpBlock->svmRebuiltDTAddress [0]));

            pParent->svmPendingQueue.AddTail (pCurrent);

//--------- The following needs done within the Lock/Unlock loop so that 
//--------- the receive thread does not begin operation with the wrong
//--------- status indicated.
            pCurrent->svmpBlock->svmStatus = SV_PLCCOMMSTATUS_PENDING;

   			lTemp = lTemp + 1;

            pParent->SVPendingQLock.Unlock ();


         }

      } //  if (!pParent->svmSendQueue.IsEmpty ())

      pParent->SVSendQLock.Unlock ();
	   Sleep (30);

   } //  while (!pParent->svmlStopSM)

   OutputDebugString( "Exiting PLCSendManager Task\n" );
   return 0;
}



long SVPLCCommQueueStructList::ResetParent (void*     apOldParent, 
                                            void*     apNewParent,
                                            CMutex*   apQLock)
{

   long                    lErr;

   POSITION                queuePosition;
   SVPLCCommQueueStruct*   pCurrent;


   lErr = 0;

   while (1)
   {
      if (apQLock != NULL)
      {
         apQLock->Lock ();
      }


      if (!IsEmpty ())
      {
         queuePosition = GetHeadPosition ();
         pCurrent = GetAt (queuePosition);

         while (pCurrent)
         {
            if (pCurrent->svmpParent == apOldParent)
            {
               pCurrent->svmpParent = apNewParent;
            }

            GetNext (queuePosition);

            if (queuePosition == NULL)
            {
               pCurrent = NULL;
            }
            else
            {
               pCurrent = GetAt (queuePosition);
            }
         } // while (pCurrent)
      } // if (!IsEmpty ())

      break;
   } // while (1)

   if (apQLock != NULL)
   {
      apQLock->Unlock ();
   }

   return lErr;
}


int	sv_tstrncpy (TCHAR		   *destString,
			          const TCHAR	*sourceString,
				       int			   nbrOfBytes)
{
	_tcsncpy (destString, sourceString, nbrOfBytes);

	destString [nbrOfBytes - 1] = 0;

	return 0;
}


int	sv_strncpy (char		   *destString,
			         const char	*sourceString,
				      int			   nbrOfBytes)
{
	strncpy (destString, sourceString, nbrOfBytes);

	destString [nbrOfBytes - 1] = 0;

	return 0;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVP_SRC\PLCBaseClass.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 13:35:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attributes and lables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 11:21:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 May 2013 13:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:36:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 13:29:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 15:32:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:01:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Method to test communications before going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:55:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   13 Nov 2001 15:22:22   jBrown
   Project:  SVFocusNT
   Change Request (SCR) nbr:  142
   SCR Title:  Remove thread spawning from within constructors
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     The following functions were added in order to separate class construction from class initialization.
   
   SVPLCBaseClass::Initialize (), and 
   SVPLCBaseClass::IsInitialized ()
   
   The following functions were modified in order to separate class construction from class initialization.
   
   SVPLCBaseClass::Open (), and
   SVPLCBaseClass::SVPLCBaseClass ()
   
   
   /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:04   mike
 * Initial revision.
*/