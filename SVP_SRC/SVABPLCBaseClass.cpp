// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC
// * .File Name       : $Workfile:   SVABPLCBaseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:36:48  $
// ******************************************************************************

// SVSLC500BaseClass.cpp: implementation of the SVSLC500BaseClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"                      // ceil ()

#include "plc_interface.h"
#include <afxpriv.h>
#include "SVABPLCBaseClass.h"


CArray <long, long> SVABPLCBaseClass::msvUsedIds; 
// msvUnusedIds only contain a list of indexes which have been previously 
// removed from msvUsedIds.

CArray <long, long> SVABPLCBaseClass::msvUnusedIds; 


CMutex SVABPLCBaseClass::svmDriverIdListLock;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SVABPLCBaseClass::SVABPLCBaseClass(long* aplErr):
SVPLCBaseClass (aplErr)
{

   *aplErr = 0;

}


SVABPLCBaseClass::~SVABPLCBaseClass()
{
   svmDriverIdListLock.Lock ();
//   msvUsedIds.RemoveAt (svmlDriverId);
//   msvUnusedIds.Add (svmlDriverId);
   TRACE1 ("ID done = %d\n", svmlDriverId);
   svmDriverIdListLock.Unlock ();
   OutputDebugString( "Exiting ~SVABPLCBaseClass\n" );
}





long SVABPLCBaseClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   long  lErr;
   long  lRetValue;

   lErr = 0;

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
         lRetValue = 84;
         break;
      }
   case SV_PLC_SPFLOAT:
      {
         lRetValue = 4;
         break;
      }
   default:
      {
         lErr = -1250;
         break;
      }
   }

   if (lErr < 0)
      return lErr;
   else
      return lRetValue;
}



long SVABPLCBaseClass::CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock)
{
   long     lErr;
   long     lLoop;
   long     lOffset;

   long*    plPCValue;
   short*   pnPLCShortValue;  // Int16
   long*    pnPLCLongValue;   // Int32

   char*    pcPLCStringValue;
   
   float*   pfPLCFloatValue;

   lErr = 0;

   while (1)
   {
      plPCValue = (long *) apBlock->svmpParamBuffer;

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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->
                           svmlOriginalIndex] = 
                  pnPLCShortValue [lOffset];
            }
            break;
         }
      case SV_PLC_INT32:
         {
            pnPLCLongValue = (long *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize (); 
                 lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->
                           svmlOriginalIndex] = 
                  pnPLCLongValue [lOffset];
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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

//------------ Allen Bradley strings have a fixed size of 84 bytes.
               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 
                  atol (&pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH]);
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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               plPCValue [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 
                  (long) pfPLCFloatValue [lOffset];
            }

            break;
         }

      }

      break;
   }

   return lErr;
}




long SVABPLCBaseClass::CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock)
{
   long           lErr;
   long           lLoop;
   long           lOffset;

   long double*   pfPCValue;

   short*         pnPLCShortValue;
   long*          pnPLCLongValue;

   char*          pcPLCStringValue;
   
   float*         pfPLCFloatValue;

   lErr = 0;

   while (1)
   {
      pfPCValue = (long double *) apBlock->svmpParamBuffer;

      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            pnPLCShortValue = (short *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               pfPCValue [lLoop] = pnPLCShortValue [lOffset];
            }
            break;
         }
      case SV_PLC_INT32:
         {
            pnPLCLongValue = (long *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               pfPCValue [lLoop] = pnPLCLongValue [lOffset];
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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

//------------ Allen Bradley strings have a fixed size of 84 bytes.
               pfPCValue [lLoop] = atof (&pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH]);
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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               pfPCValue [lLoop] = pfPLCFloatValue [lOffset];
            }

            break;
         }

      }

      break;
   }

   return lErr;
}



long SVABPLCBaseClass::CopyReceiveBufferToString (SVPLCBlockStruct* apBlock)
{
   long           lErr;
   long           lLoop;
   long           lOffset;
   long           lDataSize;

   char*          cpSwappedBytes;

   TCHAR*         ptcPCValue;

   char*          pcPLCStringValue;

   lErr = 0;

   cpSwappedBytes = NULL;


   while (1)
   {
      ptcPCValue = (TCHAR *) apBlock->svmpParamBuffer;
      lDataSize = DataTypeSize (apBlock->svmPLCDataType);

      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;

            cpSwappedBytes = new char [apBlock->svmlPLCDataFixedLength + 1];

            if (cpSwappedBytes == NULL)
            {
               lErr = -1361;
               break;
            }

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize (); 
                 lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               SwapBytes (cpSwappedBytes, 
                          &pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength], 
                          static_cast<long>(ceil ((double) apBlock->svmlPLCDataFixedLength / (double) 2)));

               sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
                            A2T(cpSwappedBytes),
                            apBlock->svmlPLCDataFixedLength);
            }
            break;
         }
      case SV_PLC_INT32:
         {
            pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;
//?? swap words??   
            cpSwappedBytes = new char [apBlock->svmlPLCDataFixedLength + 1];

            if (cpSwappedBytes == NULL)
            {
               lErr = -1365;
               break;
            }

            for (lLoop = 0; 
                 lLoop < apBlock->svmpIndividualElementArray.GetSize (); 
                 lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;
CString temp;
temp.Format (_T("0x%x"),pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength]); 
//?? swap words
               SwapBytes (cpSwappedBytes, 
                          &pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength], 
                          static_cast<long>(ceil ((double) apBlock->svmlPLCDataFixedLength / (double) 2)));

               sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
                            A2T(cpSwappedBytes),
                            apBlock->svmlPLCDataFixedLength);
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
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

//------------ Allen Bradley strings have a fixed size of 84 bytes.
               if (apBlock->svmlPLCDataFixedLength >= SV_MAX_SLC500_ST_LENGTH)
               {
                  CopyPLCStringToPCString (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength],
                                           &pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH],
                                           SV_MAX_SLC500_ST_LENGTH);

//                  sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
//                               A2T(&pcPLCStringValue [lOffset * 42]),
//                               42);
               }
               else
               {
                  CopyPLCStringToPCString (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength],
                                           &pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH],
                                           apBlock->svmlPLCDataFixedLength * lDataSize);

//                  sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
//                               A2T(&pcPLCStringValue [lOffset * 42]),
//                               apBlock->svmlPLCDataFixedLength);
               }
            }
            break;
         }

      }

      break;
   }

   return lErr;
}



long SVABPLCBaseClass::CopyPLCStringToPCString (TCHAR*  atzPCString,
                                                 char*   apPLCRawData,
                                                 long    alMaxLength)
{
   char*    cpSwappedBytes;

   long     lErr;
   long     lNumberOfBytes;


   lErr = 0;
   cpSwappedBytes = NULL;

   while (1)
   {
//--- Create a temporary buffer to contain the swapped data.
      cpSwappedBytes = new char [alMaxLength];

      if (cpSwappedBytes == NULL)
      {
         lErr = -1355;
         break;
      }

      SwapBytes (cpSwappedBytes, 
                 apPLCRawData, static_cast<long>(ceil ((double) alMaxLength / (double) 2)));

//      lNumberOfBytes = ((short *) cpSwappedBytes) [0];
      lNumberOfBytes = ((short *) apPLCRawData) [0];

//--- Set zero termination of swapped string.
      cpSwappedBytes [lNumberOfBytes + 2] = 0;

      sv_tstrncpy (atzPCString,
                   A2T (&(cpSwappedBytes [2])),
                   lNumberOfBytes + 1);
      break;

   }

   delete [] cpSwappedBytes;

   return lErr;
}



long SVABPLCBaseClass::CopyLongToSendBuffer (SVPLCBlockStruct* apBlock)

{
   long     lErr;
   long     lLoop;
   long     lTransmitBufferOffset;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.


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
               short*   pnTransmitBuffer;

               pnTransmitBuffer = (short*) apBlock->svmcpTxRxBuffer;
               short* plValueBuffer = (short*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pnTransmitBuffer [lTransmitBufferOffset] = (short)
                  plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         case SV_PLC_INT32:
            {
//------------ Physical array of data which will be transferred.
               long*   pnTransmitBuffer;

               pnTransmitBuffer = (long*) apBlock->svmcpTxRxBuffer;
               long* plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pnTransmitBuffer [lTransmitBufferOffset] = (long)
                  plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         case SV_PLC_STRING:
            {
//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;

               pczTransmitBuffer = apBlock->svmcpTxRxBuffer;
               char* plValueBuffer = (char*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * SV_MAX_SLC500_ST_LENGTH;

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
               float* plValueBuffer = (float*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pfTransmitBuffer [lTransmitBufferOffset] = (float)
                  plValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         default:
            {
               lErr = -1237;
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




long SVABPLCBaseClass::CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock)

{
   long     lErr;
   long     lLoop;
   long     lTransmitBufferOffset;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   double*  pdValueBuffer;


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
               short*   pnTransmitBuffer;

               pnTransmitBuffer = (short*) apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pnTransmitBuffer [lTransmitBufferOffset] = (short)
                  pdValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         case SV_PLC_INT32:
            {

//------------ Physical array of data which will be transferred.
               long*   pnTransmitBuffer;

               pnTransmitBuffer = (long*) apBlock->svmcpTxRxBuffer;
               pdValueBuffer = (double*) apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pnTransmitBuffer [lTransmitBufferOffset] = (long)
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
                   apBlock->svmlFirstElement) * SV_MAX_SLC500_ST_LENGTH;

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
                  apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                  apBlock->svmlFirstElement;

               pfTransmitBuffer [lTransmitBufferOffset] = (float)
                  pdValueBuffer [apBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex];

               break;
            }
         default:
            {
               lErr = -1238;
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




long SVABPLCBaseClass::CopyStringToSendBuffer (SVPLCBlockStruct* apBlock)

{
   long     lErr;
   long     lLoop;
   long     lTransmitBufferOffset;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   char*   pczValueBuffer;


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
               pczValueBuffer = apBlock->svmpParamBuffer;
               
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
                        &pczValueBuffer [lValueBufferOffset]);

               break;
            }
         case SV_PLC_INT32:
            {

//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;
               long     lValueBufferOffset;

               pczTransmitBuffer = (char*) apBlock->svmcpTxRxBuffer;
               pczValueBuffer = apBlock->svmpParamBuffer;
               
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
                        &pczValueBuffer [lValueBufferOffset]);

               break;
            }
         case SV_PLC_STRING:
            {

//------------ Physical array of data which will be transferred.
               char*    pczTransmitBuffer;
               long     lValueBufferOffset;

               pczTransmitBuffer = (char*) apBlock->svmcpTxRxBuffer;
               pczValueBuffer = apBlock->svmpParamBuffer;
               
//------------ The block transfer will write a set number of bytes of data, 
//------------ starting from some "start" address. Many addressable gaps may
//------------ exist in the *apBlock->svmpIndividualElementArray, but the 
//------------ transmit buffer must match the PLC memory block.
               lTransmitBufferOffset = 
                  (apBlock->svmpIndividualElementArray [lLoop]->svmElement -
                   apBlock->svmlFirstElement) * SV_MAX_SLC500_ST_LENGTH;

               lValueBufferOffset = apBlock-> 
                                       svmpIndividualElementArray [lLoop]->
                                          svmlOriginalIndex * 
                                    apBlock->svmlPLCDataFixedLength;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%*.*s",
                        42,
                        42,
                        &pczValueBuffer [lValueBufferOffset]);

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




long SVABPLCBaseClass::ParseElements (SVPLCElementStructArray* apAddress)
{
   long     lErr;
   long     lLoop;
   long     lNbrOfElements;
   long     lNbrOfWords;
   long     lNbrOfDWords;
   long     lParseStep;
   long     lBitNbr;
   long     lDataTypeSize;

   TCHAR*   pcIndex;

   SVPLCElementStruct* pElement;

   lErr = 0;

   while (1)
   {
      lNbrOfElements = apAddress->GetSize ();

      for (lLoop = 0; lLoop < lNbrOfElements; lLoop++)
      {
         lParseStep = 0;

         pElement = (*apAddress) [lLoop];
         pElement->svmBitMask = 0;

         pcIndex = pElement->svmPLCAddress;

         pElement->svmElementType = SV_PLC_UNKNOWN;

         while (*pcIndex != _T('\0'))
         {
            while (_istspace (*pcIndex))
               pcIndex++;

            if (lParseStep == 0)
            {
//------------ PARSE ELEMENT TYPE -------------------------------------------
               if (*pcIndex == '$')
               {
//--------------- We don't care whether they entered the address with a '$' 
//--------------- or not.
                  pcIndex++;
                  continue;
               }

               if (!_istalpha (*pcIndex)) 
               {
                  lErr = -1190;
                  break;
               }

               switch (*pcIndex)
               {
               case _T('B'):
               case _T('b'):
                  {
                     pElement->svmElementType = SV_PLC_BIT;
                     break;
                  }
               case _T('N'):
               case _T('n'):
                  {
                     pElement->svmElementType = SV_PLC_INT16;
                     break;
                  }
               case _T('L'):
               case _T('l'):
                  {
                     pElement->svmElementType = SV_PLC_INT32;
                     lDataTypeSize = DataTypeSize (pElement->svmElementType);
                     if (lDataTypeSize < 0)
                     {
                        lErr = lDataTypeSize;
                        break;
                     }
                     break;
                  }
               case _T('F'):
               case _T('f'):
                  {
                     pElement->svmElementType = SV_PLC_SPFLOAT;
                     break;
                  }
               case _T('S'):
               case _T('s'):

                  {
                     while (_istspace (*(++pcIndex)));

                     if ((*pcIndex == _T('T')) || (*pcIndex == _T('t')))
                     {
                        pElement->svmElementType = SV_PLC_STRING;
                     }
                     else
                     if (_istalpha (*pcIndex))
                     {
                        lErr = -1192;
                     }
                     else
                     {
                        lErr = -1193;
                     }

                     break;
                  }
               default:
                  {
                     lErr = -1191;
                     break;
                  }
               }

               if (lErr < 0)
                  break;

               pcIndex = pcIndex + 1;

               lParseStep = lParseStep + 1;
//------------ End of PARSE ELEMENT TYPE. -----------------------------------
            }
            else
            if (lParseStep == 1)
            {
//------------ PARSE FILE NUMBER --------------------------------------------
               if (!_istdigit (*pcIndex))
               {
                  lErr = -1194;
                  break;
               }

               pElement->svmPLCFileNbr = _ttol (pcIndex);

               while (_istdigit (*(++pcIndex)));
               lParseStep = lParseStep + 1;
//------------ End of PARSE FILE NUMBER. ------------------------------------
            }
            if (lParseStep == 2)
            {
//------------ PARSE ELEMENT NUMBER -----------------------------------------

//------------ We leave the possibillity of any single character delimiter.
               if (!_istdigit (*pcIndex))
               {
                  while (_istspace (*(++pcIndex)));
               }

               if (!_istdigit (*pcIndex))
               {
                  lErr = -1195;
                  break;
               }


               pElement->svmElement = _ttol (pcIndex);

               while (_istdigit (*(++pcIndex)));
               lParseStep = lParseStep + 1;
               
//------------ End of PARSE ELEMENT NUMBER. ---------------------------------
            }
            else
            if (lParseStep == 3)
            {
//------------ PARSE BIT NUMBER ---------------------------------------------

//------------ We leave the possibillity of any single character delimiter.
               if (!_istdigit (*pcIndex))
               {
                  while (_istspace (*(++pcIndex)));
               }

               if (!_istdigit (*pcIndex))
               {
                  lErr = -1195;
                  break;
               }

               lBitNbr = _ttol (pcIndex);


			   pElement->svmlBitNum = lBitNbr;

               if ((pElement->svmElementType == SV_PLC_BIT) ||
                   (pElement->svmElementType == SV_PLC_INT16))
               {
                  lNbrOfWords = lBitNbr / 16;
                  lBitNbr = lBitNbr % 16;

                  pElement->svmElement = pElement->svmElement + lNbrOfWords;
               }
               else
               if (pElement->svmElementType == SV_PLC_INT32)
               {
                  lNbrOfDWords = lBitNbr / 32;
                  lBitNbr = lBitNbr % 32;

                  pElement->svmElement = pElement->svmElement + lNbrOfDWords;
               }

               pElement->svmBitMask = ~(1 << lBitNbr);

               while (_istdigit (*(++pcIndex)));
               lParseStep = lParseStep + 1;

//------------ End of PARSE BIT NUMBER. -------------------------------------
            }


            if (lErr < 0)
            {
               break;
            }

            while (_istspace (*(pcIndex)))
               pcIndex++;
         } // while (pcIndex != _T(NULL))

         if (lErr < 0)
            break;

         pElement->svmlParsed = TRUE;

      } // for (lLoop = 0; lLoop < lNbrOfElements; lLoop++)

      break;
   }
   return lErr;
}



long SVABPLCBaseClass::QuickSortElements (SVPLCElementStructArray* apSortArray,
                                        long    alBeginning,
                                        long    alEnd)

{
   long                 lErr;

   long                 i;
   long                 j;

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
//         while (aSortArray [alSortMap [j]] < val)
//            j++;

         while (((*apSortArray) [j]->svmPLCFileNbr < pElement->svmPLCFileNbr) ||
                (((*apSortArray) [j]->svmPLCFileNbr == pElement->svmPLCFileNbr) &&
                 ((*apSortArray) [j]->svmElement < pElement->svmElement)))
            j++;


//         while (aSortArray [alSortMap [i]] > val)
//            i--;

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




long SVABPLCBaseClass::AddElementToBlock (SVPLCBlockStruct*     apBlock,
                                           SVPLCElementStruct*   apElement)
{
   long  lErr;
   long  lDataSize;

   lErr = 0;

   while (1)
   {
      if (apBlock->svmpIndividualElementArray.GetSize () == 0)
      {
         CopyElementAddressToBlock (apBlock,
                                    apElement);

         RebuildAddressString (apBlock);

//         apBlock->svmpIndividualElementArray.Add (apElement);

        
         lDataSize = DataTypeSize (apBlock->svmPLCDataType);


//------ PC fixed length is always characters.  PLC fixed length is always
//------ elements. ----------------------------------------------------------
         if (apElement->svmElementType == SV_PLC_STRING)
         {
	         apBlock->svmlPLCDataFixedLength = 1;
         }
         else
         {
            if (apBlock->svmlPCDataFixedLength == 0)
            {
               apBlock->svmlPLCDataFixedLength = 1;
            }
            else
            {
               apBlock->svmlPLCDataFixedLength = apBlock->svmlPCDataFixedLength / lDataSize;
            }


//--------- If the fixed length size is too large, we clip it to 128 bytes
//--------- that is the maximum limit that Allen-Bradley allows in a block 
//--------- transfer.
            if (apBlock->svmlPLCDataFixedLength > (128 / DataTypeSize (apBlock->svmPLCDataType)))
            {
               apBlock->svmlPLCDataFixedLength = 128 / DataTypeSize (apBlock->svmPLCDataType);
            }
         }
//         break;
      }

//--- The block is not empty, so we must check to see if the current 
//--- element fits in the current block.

      if (apBlock->svmlPLCFileNbr != apElement->svmPLCFileNbr)
      {
         lErr = 1;
         break;
      }

      if (apBlock->svmPLCDataType != apElement->svmElementType)
      {
//------ SAME FILE NUMBER< DIFFERENT DATA TYPE ------------------------------
         lErr = -1196;
         break;
//------ End of SAME FILE NUMBER< DIFFERENT DATA TYPE. ----------------------
      }

//--- Test to see if element would excede 128 byte block size.
      if (((apElement->svmElement + apBlock->svmlPLCDataFixedLength) - apBlock->svmlFirstElement) > 
          (128 / DataTypeSize (apBlock->svmPLCDataType)))
      {
         lErr = 1;
         break;
      }

      apBlock->svmlNbrElements = (apElement->svmElement + 
                                  apBlock->svmlPLCDataFixedLength) - 
                                 apBlock->svmlFirstElement;

      apBlock->svmpIndividualElementArray.Add (apElement);

      break;
   }

   return lErr;
}



long SVABPLCBaseClass::RebuildAddressString (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   char  szFileType [8];

   lErr = 0;

   while (1)
   {
      switch (apBlock->svmPLCDataType)
      {
      case SV_PLC_BIT:
         {
            sprintf (szFileType, "B");
            break;
         }
      case SV_PLC_INT16:
         {
            sprintf (szFileType, "N");
            break;
         }
      case SV_PLC_INT32:
         {
            sprintf (szFileType, "L");
            break;
         }
      case SV_PLC_STRING:
         {
            sprintf (szFileType, "ST");
            break;
         }
      case SV_PLC_SPFLOAT:
         {
            sprintf (szFileType, "F");
            break;
         }
      default:
         {
            lErr = -1209;
            break;
         }
      }

      if (lErr < 0)
         break;

      sprintf (apBlock->svmRebuiltDTAddress,
               "%s%d:%d",
               szFileType,
               apBlock->svmlPLCFileNbr,
               apBlock->svmlFirstElement);

      break;
   }

   return lErr;

}


long SVABPLCBaseClass::SendBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock)
{
   long        lErr;
//   long        lDataTypeSize;
   long        lNbrElements;
   long        lLoop;
   long        lCounter;

//   SVPLCElementStructArray tempCArray;

   SVPLCElementStruct currentElement;

   long*             plLocalValueArray;

   SVPLCBlockStruct* pInputBlock;  
   SVPLCBlockStruct* pNewBlock;  
   SVPLCBlockStruct* pCurrentBlock;  
   SVPLCBlockStruct* pPreviousBlock;  



   lErr = 0;
   lNbrElements = 0;
   plLocalValueArray = NULL;
   pNewBlock = NULL;

   while (1)
   {
      pInputBlock = apBlock;

      pNewBlock = new SVPLCBlockStruct;

      if (pNewBlock == NULL)
      {
         lErr = -1349;
         break;
      }

      pNewBlock->svmpPreviousBlock = NULL;
      pNewBlock->svmpNextBlock = NULL;

      pCurrentBlock = pNewBlock;

      while (pInputBlock != NULL)
      {

         for (lLoop = 0; 
              lLoop < pInputBlock->svmpIndividualElementArray.GetSize (); 
              lLoop++)
         {
            if (IsSameElement (&currentElement, pInputBlock->svmpIndividualElementArray [lLoop]))
            {
               
            }
            else
            {
               lErr = AddElementToBlock (pCurrentBlock, pInputBlock->svmpIndividualElementArray [lLoop]);
               if (lErr > 0)
               {
                  lErr = -1350;
                  break;
               }
               else
               if (lErr < 0)
               {
                  break;
               }
               currentElement = *(pInputBlock->svmpIndividualElementArray [lLoop]);
            }
         }

         if (lErr < 0)
            break;

         lNbrElements = lNbrElements + pNewBlock->svmpIndividualElementArray.GetSize ();

         pInputBlock = pInputBlock->svmpNextBlock;
         if (pInputBlock != NULL)
         {
            pCurrentBlock->svmpNextBlock = new SVPLCBlockStruct;
            pPreviousBlock = pCurrentBlock;
            pCurrentBlock = pCurrentBlock->svmpNextBlock;
            if (pNewBlock == NULL)
            {
               lErr = -1351;
               break;
            }

            pCurrentBlock->svmpPreviousBlock = pPreviousBlock;
            pCurrentBlock->svmpNextBlock = NULL;
         }
      }

      if (lErr < 0)
         break;

      lErr = InitializeBlocks (pNewBlock);
      if (lErr < 0)
         break;
//      pBlock = apBlock;    // Reset to beginning of linked list.

      plLocalValueArray = new long [lNbrElements];

      lErr = GetIntBlock (plLocalValueArray, pNewBlock);
      if (lErr < 0) 
         break;

      lErr = WaitTillComplete (pNewBlock);

      if (lErr < 0)
         break;

      pInputBlock = apBlock;
      pCurrentBlock = pNewBlock;

//--- Both the input block and the local (current) block should be sorted
//--- in the same order. They should both be referencing the same elements, 
//--- but the arrays associated with the local blocks have been condenced 
//--- so that the elements represent a unique addressable unit.  This is 
//--- done so that all bits referencing the same addressable unit will be
//--- used to modify a single instance of the variable to be written to 
//--- that unit.  That addressable unit is then written once, and the bit
//--- settings reflect all the values in the original array. ----------------
      while (pInputBlock != NULL)
      {
         lCounter = 0;

         for (lLoop = 0; 
              lLoop < pCurrentBlock->svmpIndividualElementArray.GetSize () ;
              lLoop++)
         {
            while ( lCounter < pInputBlock->svmpIndividualElementArray.GetSize() && 
				    IsSameElement (pCurrentBlock->svmpIndividualElementArray [lLoop],
                                  pInputBlock->svmpIndividualElementArray [lCounter]))
            {
               if (aplValueArray [pInputBlock->svmpIndividualElementArray [lCounter]->
                                  svmlOriginalIndex] == 0)
               {
//--------------- This should clear the bit. ----------------------------------
                  plLocalValueArray [pCurrentBlock->svmpIndividualElementArray [lLoop]->
                              svmlOriginalIndex] = 
                     plLocalValueArray [pCurrentBlock->svmpIndividualElementArray [lLoop]->
                              svmlOriginalIndex] & 
                     pInputBlock->svmpIndividualElementArray [lCounter]->svmBitMask;
               }
               else
               {
//--------------- This should set the bit. --------------------------------
                  plLocalValueArray [pCurrentBlock->svmpIndividualElementArray [lLoop]->
                              svmlOriginalIndex] = 
                     plLocalValueArray [pCurrentBlock->svmpIndividualElementArray [lLoop]->
                              svmlOriginalIndex] | 
                     ~pInputBlock->svmpIndividualElementArray [lCounter]->svmBitMask;
               }

               lCounter = lCounter + 1;
            } // while (IsSameElement ...

         } // for (lLoop = 0;...

         pInputBlock = pInputBlock->svmpNextBlock;
         pCurrentBlock = pCurrentBlock->svmpNextBlock;

      } // while (pInputBlock != NULL)

      lErr = SendIntBlock (plLocalValueArray, pNewBlock);
      if (lErr < 0) 
         break;

      lErr = WaitTillComplete (pNewBlock);
      if (lErr < 0)
         break;

      break;
   } // while (1)

   DestroyBlocks (&pNewBlock);
   delete [] plLocalValueArray ;
   return lErr;
}













long SVABPLCBaseClass::GetBitBlock (long* aplValueArray, SVPLCBlockStruct* apBlock)
{
   long        lErr;
   long        lNbrElements;
   long        lLoop;

   long*             plLocalValueArray;

   SVPLCBlockStruct* pBlock;  


   lErr = 0;
   lNbrElements = 0;
   plLocalValueArray = NULL;

   while (1)
   {
      pBlock = apBlock;

      while (pBlock != NULL)
      {
         lNbrElements = lNbrElements + pBlock->svmpIndividualElementArray.GetSize ();
         pBlock = pBlock->svmpNextBlock;
      }

      plLocalValueArray = new long [lNbrElements];

      if (plLocalValueArray == NULL)
      {
         lErr = -1354;
         break;
      }

      lErr = GetIntBlock (plLocalValueArray, apBlock);
      if (lErr < 0) 
         break;

      lErr = WaitTillComplete (apBlock);

      if (lErr < 0)
         break;

      pBlock = apBlock;


      while (pBlock != NULL)
      {
         for (lLoop = 0; 
              lLoop < pBlock->svmpIndividualElementArray.GetSize (); 
              lLoop++)
         {
            if ((plLocalValueArray 
                  [pBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] &
                 pBlock->svmpIndividualElementArray [lLoop]->svmBitMask) == 0)
            {
               aplValueArray [pBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 0;
            }
            else
            {
               aplValueArray [pBlock->svmpIndividualElementArray [lLoop]->svmlOriginalIndex] = 1;
            }
         }

         pBlock = pBlock->svmpNextBlock;
      }

      break;
   } // while (1)

   delete [] plLocalValueArray ;
   return lErr;
}



long SVABPLCBaseClass::IsSameElement (SVPLCElementStruct*   apElement1,
                                       SVPLCElementStruct*   apElement2)
{
   long  lErr;

   lErr = TRUE;

   while (1)
   {
      if (apElement1->svmPLCFileNbr != apElement2->svmPLCFileNbr)
      {
         lErr = FALSE;
      }
      else
      if (apElement1->svmElement != apElement2->svmElement)
      {
         lErr = FALSE;
      }


      break;
   }

   return lErr;
}



void DTL_CALLBACK SolCallback(DWORD adwParam,DWORD adwIostatus)
{
   SVPLCBlockStruct* l_pBlockRaw = reinterpret_cast<SVPLCBlockStruct*>(adwParam);

	try
	{
		SVPLCBlockStruct* l_pBlock = dynamic_cast<SVPLCBlockStruct*>(l_pBlockRaw);
		if( l_pBlock )
		{
			l_pBlock->svmulRawStatus = adwIostatus;
			TRACE1 ("CallBack: counter = %d\n", l_pBlock->svmlPLCDataFixedLength);
		}
	}
	catch(...)
	{
		TRACE(_T("Exception in callback"));
	}

}

long SVABPLCBaseClass::Open (TCHAR *apConnectionString)
{
   return -1313;
}

// Returns the maximum data table size in bytes.
long   SVABPLCBaseClass::GetMaxDTSize( void )
{
	return 512;
}

long   SVABPLCBaseClass::TestCommunications()
{
	return -1360;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABPLCBaseClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:36:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 13:29:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 15:38:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:01:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Method to test communications before going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2001 16:47:56   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The file SVSLC500BaseClass.cpp was renamed to SVABPLCBaseClass.cpp.
 * 
 * The file SVSLC500BaseClass.h was renamed to SVABPLCbaseClass.h.
 * 
 * For Version Manager history on these files it is necessary to include the old file history.
 * 
 * The class SVSLC500BaseClass was renamed to SVABPLCBaseClass.
 * 
 * Besides the name change, the following functions were modified:
 * 
 * SVABPLCBaseClass::SVABPLCBaseClass (),
 * SVABPLCBaseClass::CopyReceiveBufferToLong (),
 * SVABPLCBaseClass::CopyReceiveBufferToFloat (),
 * SVABPLCBaseClass::CopyReceiveBufferToString (),
 * SVABPLCBaseClass::CopyLongToSendBuffer (),
 * SVABPLCBaseClass::CopyDoubleToSendBuffer (),
 * SVABPLCBaseClass::CopyStringToSendBuffer (),
 * SVABPLCBaseClass::ParseElements (), and
 * SVABPLCBaseClass::RebuildAddressString ().
 * 
 * These functions were all modified to support the new Allen-Bradley 32 bit integer data type supported by the MicroLogix PLCs.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Feb 2001 17:48:42   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  86
 * SCR Title:  Fix problems with Allen-Bradley string reading
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Modified 
 * 
 * 
 * 
 * SVSLC500BaseClass::CopyReceiveBufferToLong ()
 * 
 * SVSLC500BaseClass::CopyReceiveBufferToFloat ()
 * 
 * SVSLC500BaseClass::CopyReceiveBufferToString ()
 * 
 * SVSLC500BaseClass::CopyLongToSendBuffer ()
 * 
 * SVSLC500BaseClass::CopyDoubleToSendBuffer ()
 * 
 * SVSLC500BaseClass::CopyStringToSendBuffer ()
 * 
 * SVSLC500BaseClass::AddElementToBlock ()
 * 
 * 
 * 
 * to support larger Allen-Bradley PLC string data type size.
 * 
 * 
 * 
 * SVSLC500BaseClass::CopyReceiveBufferToString (), and
 * 
 * SVSLC500BaseClass::AddElementToBlock ()
 * 
 * 
 * 
 * also had significant bug fixes stemming from initial read string implimentation.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Feb 2001 14:33:44   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  86
 * SCR Title:  Fix problems with Allen-Bradley string reading
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Modified the following functions to correct bugs in reading strings from the PLC.
 * 
 *  SVSLC500BaseClass::CopyPLCStringToPCString ()
 *  SVSLC500BaseClass::AddElementToBlock ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/