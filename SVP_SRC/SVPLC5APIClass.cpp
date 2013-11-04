// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC5APIClass
// * .File Name       : $Workfile:   SVPLC5APIClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:02:06  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"
#include "SVABPLCBaseClass.h"
#include "SVPLC5APIClass.h"

SVPLC5APIClass::SVPLC5APIClass(long* aplErr):
SVRSLinxABPLCBaseClass (aplErr)
{
	// TODO: Add your specialized code here.
   *aplErr = 0;

   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_PLC5;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_DONTCARE;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_APIKTX;

//   svmlDriverId = 0;
   strcpy (svmczNonUCPortId, "AB:LOCAL");
}

SVPLC5APIClass::~SVPLC5APIClass()
{
	// TODO: Add your specialized code here.
}

long SVPLC5APIClass::Open(TCHAR *apConnectionString)
{
   long  lErr;

   lErr = 0;
      
   while (1)
   {
//      ParseSetupParameterString (apConnectionString);
// TRB moving to parse setup string to interface class

//--- Copy to class member
      sv_tstrncpy (svmczConnectionString, 
                   apConnectionString, 
                   SV_NBRCHARS (svmczConnectionString));

//--- COPY DRIVER NAME TO NON-UNICODE ---------------------------------------
      sv_strncpy (svmczNonUCDriverName,
                  T2A (m_ConnectionData.svmczDriverName),
                  sizeof (svmczNonUCDriverName));
//--- End of COPY DRIVER NAME TO NON-UNICODE. -------------------------------


//--- COPY STATION NUMBER TO NON-UNICODE ------------------------------------
      sv_strncpy (svmczNonUCStationNbr,
                  T2A (m_ConnectionData.svmczDHNodeAddress),
                  sizeof (svmczNonUCStationNbr));
//--- End of COPY STATION NUMBER TO NON-UNICODE. ----------------------------

      lErr = SVRSLinxABPLCBaseClass::Open (apConnectionString);
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}

long SVPLC5APIClass::InitializeBlocks(SVPLCBlockStruct* apBlock)
{
   long              lErr;
   SVPLCBlockStruct* pBlock;

   lErr = 0;

   while (1)
   {
      SVPLCBaseClass::InitializeBlocks (apBlock);


      pBlock = apBlock;

      while (pBlock)
      {
         switch (pBlock->svmPLCDataType)
         {
         case SV_PLC_BIT:
         case SV_PLC_INT16:
            {
               sprintf (pBlock->svmDefinitionString, "%s,%d,%s,%s,%s,%s,%s,%d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "WORD",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCStationNbr,
                        "PLC5",
                        svmlDriverId);  // driver_id
               break;
            }
         case SV_PLC_STRING:
            {

               sprintf (pBlock->svmDefinitionString, "%s, %d, %s, %s, %s, %s, %s, %d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "RAW",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCStationNbr,
                        "PLC5",
                        svmlDriverId);  // driver_id

               break;
            }
         case SV_PLC_SPFLOAT:
            {
               sprintf (pBlock->svmDefinitionString, "%s, %d, %s, %s, %s, %s, %s, %d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "FLOAT",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCStationNbr,
                        "PLC5",
                        svmlDriverId);  // driver_id


               break;
            }
         default:
            {
               lErr = -1368;
               break;
            }

         }

         if (lErr < 0)
            break;

         if (svmhRSLinxLib)
         {
            lErr = -DTL_C_DEFINE (&pBlock->svmlTransactionId, pBlock->svmDefinitionString);
         }

         if (lErr < 0)
            break;

		 pBlock = pBlock->svmpNextBlock;

      } // while (pBlock)

      break;
   } // while (1)

   return lErr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLC5APIClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:02:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:58:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2002 15:58:48   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  141
 * SCR Title:  Add support for Allen-Bradley PLC5 via the KT card (Data Highway)
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Corrected syntax error.  Changed....
 * 
 * class SV_PLCAPI_DS SVPLC5APIClass::SVPLC5APIClass(long* aplErr):
 * to...
 * SVPLC5APIClass::SVPLC5APIClass(long* aplErr):	
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/