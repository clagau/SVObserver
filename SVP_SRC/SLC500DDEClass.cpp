// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC500DDEClass
// * .File Name       : $Workfile:   SLC500DDEClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:35:54  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"
#include "SLC500DDEClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <afxpriv.h>
#include "PLCBaseClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SVPLC500DDEClass::SVPLC500DDEClass(long* aplErr):
SVPLCBaseClass (aplErr)
{
   *aplErr = 0;

   m_ConnectionData.svmPLCManufacturer =    SV_PLCMANUFACTURER_ALLENBRADLEY;
   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_SLC500;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_DONTCARE;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_DDE;

   svmlStationNbr = 0;
   svmlDriverId = 0;
   strcpy (svmczPortId, "1KT:0");
}



SVPLC500DDEClass::~SVPLC500DDEClass()
{

}



long SVPLC500DDEClass::CheckStatus (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
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
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1159;
            break;
         }
      case DTL_E_INVTYPE:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1160;
            break;
         }
      case DTL_E_BADID:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1161;
            break;
         }
      case DTL_E_NOSUPPORT:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1162;
            break;
         }
      case DTL_E_TOOBIG:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1163;
            break;
         }
      case DTL_E_NODEF:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1164;
            break;
         }
      case DTL_E_CNVT:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1165;
            break;
         }
      case DTL_E_INVDEF:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            apBlock->svmlCommunicationError = -1166;
            break;
         }
      default:
         {
            apBlock->svmStatus = SV_PLCCOMMSTATUS_ERROR;
            break;
         }
      }
      break;
   }

   return lErr;
}




long SVPLC500DDEClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
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
      }
   case SV_PLC_STRING:
      {
         lRetValue = SV_MAX_SLC500_ST_LENGTH;
      }
   case SV_PLC_SPFLOAT:
      {
         lRetValue = 4;
      }
   default:
      {
         lErr = -1250;
      }
   }

   if (lErr < 0)
      return lErr;
   else
      return lRetValue;
}





long SVPLC500DDEClass::CopyReceiveBufferToLong (SVPLCBlockStruct* apBlock)
{
   long     lErr;
   long     lLoop;
   long     lOffset;

   long*    plPCValue;
   long*    plPLCLongValue;

   char*    pcPLCStringValue;
   
   float*   pfPLCFloatValue;

   lErr = 0;

   while (1)
   {
      plPCValue = (long *) apBlock->svmpParamBuffer;

      switch (apBlock->svmPCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            plPLCLongValue = (long *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               plPCValue [lLoop] = plPLCLongValue [lOffset];
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
               plPCValue [lLoop] = atol (&pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH]);
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

               plPCValue [lLoop] = (long) pfPLCFloatValue [lOffset];
            }

            break;
         }

      }

      break;
   }

   return lErr;
}



long SVPLC500DDEClass::CopyReceiveBufferToFloat (SVPLCBlockStruct* apBlock)
{
   long           lErr;
   long           lLoop;
   long           lOffset;

   long double*   pfPCValue;

   long *         plPLCLongValue;

   char*          pcPLCStringValue;
   
   float*         pfPLCFloatValue;

   lErr = 0;

   while (1)
   {
      pfPCValue = (long double *) apBlock->svmpParamBuffer;

      switch (apBlock->svmPCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            plPLCLongValue = (long *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               pfPCValue [lLoop] = plPLCLongValue [lOffset];
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



long SVPLC500DDEClass::CopyReceiveBufferToString (SVPLCBlockStruct* apBlock)
{
   long           lErr;
   long           lLoop;
   long           lOffset;

   TCHAR*         ptcPCValue;

   char*          pcPLCStringValue;

   lErr = 0;

   while (1)
   {
      ptcPCValue = (TCHAR *) apBlock->svmpParamBuffer;

      switch (apBlock->svmPCDataType)
      {
      case SV_PLC_BIT:
      case SV_PLC_INT16:
         {
            pcPLCStringValue = (char *) apBlock->svmcpTxRxBuffer;

            for (lLoop = 0; lLoop < apBlock->svmpIndividualElementArray.GetSize (); lLoop++)
            {
               lOffset = apBlock->svmpIndividualElementArray [lLoop]->svmElement - 
                         apBlock->svmlFirstElement;

               sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
                            A2T(&pcPLCStringValue [lOffset * apBlock->svmlPLCDataFixedLength]),
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
                  sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
                               A2T(&pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH]),
                               SV_MAX_SLC500_ST_LENGTH);
               }
               else
               {
                  sv_tstrncpy (&ptcPCValue [lLoop * apBlock->svmlPLCDataFixedLength], 
                               A2T(&pcPLCStringValue [lOffset * SV_MAX_SLC500_ST_LENGTH]),
                               apBlock->svmlPLCDataFixedLength);
               }
            }
            break;
         }

      }

      break;
   }

   return lErr;
}



long SVPLC500DDEClass::CopyLongToSendBuffer (SVPLCBlockStruct* apBlock)

{
   long     lErr;
   long     lLoop;
   long     lTransmitBufferOffset;

// Logical array of data which will be transferred.  The 
// physical block may contain memory locations which are not
// referenced by this array, and the PLC addresses in this
// array are not required to be continuous or sequenciaL.
   long*    plValueBuffer;


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
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
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
               plValueBuffer = (long*) apBlock->svmpParamBuffer;
               
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




long SVPLC500DDEClass::CopyDoubleToSendBuffer (SVPLCBlockStruct* apBlock)

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




long SVPLC500DDEClass::CopyStringToSendBuffer (SVPLCBlockStruct* apBlock)

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
         case SV_PLC_STRING:
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
                   apBlock->svmlFirstElement) * SV_MAX_SLC500_ST_LENGTH;

               lValueBufferOffset = apBlock-> 
                                       svmpIndividualElementArray [lLoop]->
                                          svmlOriginalIndex * 
                                    apBlock->svmlPLCDataFixedLength;

               sprintf (&pczTransmitBuffer [lTransmitBufferOffset], "%*.*s",
                        SV_MAX_SLC500_ST_LENGTH,
                        SV_MAX_SLC500_ST_LENGTH,
                        T2A (&ptczValueBuffer [lValueBufferOffset]));

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




long SVPLC500DDEClass::ParseElements (SVPLCElementStructArray* apAddress)
{
   long     lErr;
   long     lLoop;
   long     lNbrOfElements;
   long     lNbrOfWords;
   long     lParseStep;
   long     lBitNbr;

   TCHAR*   pcIndex;

   SVPLCElementStruct* pElement;

   lErr = 0;

   while (1)
   {
      lNbrOfElements = apAddress->GetSize ();

      for (lLoop = 0; lLoop < lNbrOfElements; lLoop++)
      {
         lParseStep = 0;
         pElement = (*apAddress) [0];
         pcIndex = pElement->svmPLCAddress;

         pElement->svmElementType = SV_PLC_UNKNOWN;

         while (pcIndex != _T('\0'))
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
                  {
                     pElement->svmElementType = SV_PLC_BIT;
                     break;
                  }
               case _T('N'):
                  {
                     pElement->svmElementType = SV_PLC_INT16;
                     break;
                  }
               case _T('F'):
                  {
                     pElement->svmElementType = SV_PLC_SPFLOAT;
                     break;
                  }
               case _T('S'):

                  {
                     while (_istspace (*(++pcIndex)));

                     if (*pcIndex == _T('T'))
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
               lParseStep = lParseStep + 1;
//------------ End of PARSE ELEMENT TYPE. -----------------------------------
            }
            else
            if (lParseStep == 2)
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
            if (lParseStep == 3)
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
            if (lParseStep == 4)
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


               if ((pElement->svmElementType == SV_PLC_BIT) ||
                   (pElement->svmElementType == SV_PLC_INT16))
               {
                  lNbrOfWords = lBitNbr / 16;
                  lBitNbr = lBitNbr % 16;

                  pElement->svmElement = pElement->svmElement + lNbrOfWords;
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

            while (_istspace (*(++pcIndex)));
         } // while (pcIndex != _T(NULL))

      }

      break;
   }
   return lErr;
}



long SVPLC500DDEClass::QuickSortElements (SVPLCElementStructArray* apSortArray,
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

         while (((*apSortArray) [i]->svmPLCFileNbr < pElement->svmPLCFileNbr) ||
                (((*apSortArray) [i]->svmPLCFileNbr == pElement->svmPLCFileNbr) &&
                 ((*apSortArray) [i]->svmElement < pElement->svmElement)))
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




long SVPLC500DDEClass::AddElementToBlock (SVPLCBlockStruct*     apBlock,
                                        SVPLCElementStruct*   apElement)
{
   long  lErr;

   lErr = 0;

   while (1)
   {
      if (apBlock->svmpIndividualElementArray.GetSize () == 0)
      {
         CopyElementAddressToBlock (apBlock,
                                    apElement);

         RebuildAddressString (apBlock);

         apBlock->svmpIndividualElementArray.Add (apElement);

         break;
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

//--- Test to see if element would excede 64 byte block size.
      if ((apElement->svmElement - apBlock->svmlFirstElement) < 
          (64 / DataTypeSize (apBlock->svmPLCDataType)))
      {
         lErr = 1;
         break;
      }


      apBlock->svmlNbrElements = apElement->svmElement - 
                                 apBlock->svmlFirstElement;

      apBlock->svmpIndividualElementArray.Add (apElement);

      break;
   }

   return lErr;
}



long SVPLC500DDEClass::RebuildAddressString (SVPLCBlockStruct* apBlock)
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



long SVPLC500DDEClass::Open (TCHAR *apConnectionString)
{
   long  lErr;

   DTL_RETVAL dtlReturnValue;

   lErr = 0;

   while (1)
   {
      lErr = SVPLCBaseClass::Open ();
      if (lErr < 0)
         break;

      dtlReturnValue = DTL_INIT (64l);  // 641 = maximum number of data items

      switch (dtlReturnValue)
      {
      case DTL_SUCCESS:
         {
            break;
         }
      case DTL_E_NO_MEM:
         {
            lErr = -1170;
            break;
         }
      case DTL_E_NOREINIT:
         {
            lErr = -1171;
            break;
         }
      case DTL_E_MISSING_RSLINX_ACTIVATION:
         {
            lErr = -1239;
            break;
         }
      default:
         {
            lErr = -1172;
            break;
         }
      }
      
      if (lErr < 0)
         break;

//--- 5000 ms timeout -------------------------------------------------------
      dtlReturnValue = DTL_DRIVER_LIST (svmDtlDrivers, (unsigned long *) &svmlNbrDrivers, 5000l);   

      switch (dtlReturnValue)
      {
      case DTL_SUCCESS:
         {
            break;
         }
      case DTL_E_TIME:
         {
            lErr = -1173;
            break;
         }
      case DTL_E_NOINIT:
         {
            lErr = -1174;
            break;
         }
      case DTL_E_BADPARAM:
         {
            lErr = -1175;
            break;
         }
      default:
         {
            lErr = -1176;
            break;
         }
      }
      
      if (lErr < 0)
         break;

      if (svmlNbrDrivers == 0)
      {
         lErr = -1183;
         break;
      }

//--- I do not know what the driver list is. For now I will assume that my
//--- driver is the first in the list and that it is supporting my KTX card.
      DTL_DRIVER_OPEN (0, svmDtlDrivers [0].szDriverName, 5000l);

      switch (dtlReturnValue)
      {
      case DTL_SUCCESS:
         {
            break;
         }
      case DTL_E_TIME:
         {
            lErr = -1177;
            break;
         }
      case DTL_E_NOINIT:
         {
            lErr = -1178;
            break;
         }
      case DTL_E_DRIVER_ID_ILLEGAL:
         {
            lErr = -1179;
            break;
         }
      case DTL_E_DRIVER_ID_INUSE:
         {
            lErr = -1180;
            break;
         }
      case DTL_E_DRIVER_NAME_INVALID:
         {
            lErr = -1181;
            break;
         }
      default:
         {
            lErr = -1182;
            break;
         }
      }
      
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



long SVPLC500DDEClass::Close ()
{
	while (1)
	{
		DTL_UNINIT (DTL_E_FAIL);
		break;
	}
   return 0;
}



long SVPLC500DDEClass::SendB (SVPLCBlockStruct* apBlock)
{
   long  lErr;

   lErr = 0;

   while (1)
   {


      break;
   }
   return lErr;
}



long SVPLC500DDEClass::GetInt16 (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
//      sprintf (szPortId, "AB:NAME",
               
      sprintf (apBlock->svmDefinitionString, "%s, %d, %s, %s, %s, %d, %s, %d",
               apBlock->svmRebuiltDTAddress,
               apBlock->svmlNbrElements,
               "WORD",
               "MODIFY",
               svmczPortId,
               svmlStationNbr,
               "SLC500",
               svmlDriverId);  // driver_id

                         // data address
//      lErr = DTLCDefine (&apBlock->svmlTransactionId, apBlock->svmDefinitionString);

      if (lErr < 0)
         break;

//      lErr = DTLReadCB (apBlock->svmlTransactionId,
//                        (unsigned char *) apBlock->svmcpTxRxBuffer,
//                        5000l,
//                        SolCallback,
//                       (unsigned long) apBlock);
                   
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}



long SVPLC500DDEClass::GetStr (SVPLCBlockStruct* apBlock)
{
   long        lErr;
   CString     definition;

   lErr = 0;

   while (1)
   {
//      sprintf (szPortId, "AB:NAME",
               
      sprintf (apBlock->svmDefinitionString, "%s, %d, %s, %s, %s, %d, %s, %d",
               apBlock->svmRebuiltDTAddress,
               apBlock->svmlNbrElements,
               "RAW",
               "MODIFY",
               svmczPortId,
               svmlStationNbr,
               "SLC500",
               svmlDriverId);  // driver_id

                         // data address
//      lErr = DTLCDefine (&apBlock->svmlTransactionId, apBlock->svmDefinitionString);

      if (lErr < 0)
         break;

//      lErr = DTLReadCB (apBlock->svmlTransactionId,
//                        (unsigned char *) apBlock->svmcpTxRxBuffer,
//                        5000l,
//                        SolCallback,
//                        (unsigned long) apBlock);
                   
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SLC500DDEClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:35:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:06   mike
 * Initial revision.
*/