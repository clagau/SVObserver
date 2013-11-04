// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMicroLogixAPIClass
// * .File Name       : $Workfile:   SVMicroLogixAPIClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:01:34  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"
#include "SVABPLCBaseClass.h"
#include "SVMicroLogixAPIClass.h"

SVMicroLogixAPIClass::SVMicroLogixAPIClass(long* aplErr):
SVRSLinxABPLCBaseClass (aplErr)
{
	// TODO: Add your specialized code here.
   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_MICROLOGIX;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_DONTCARE;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_APIETH;

   strcpy (svmczNonUCPortId, "AB:NAME");
}


SVMicroLogixAPIClass::~SVMicroLogixAPIClass()
{
	// TODO: Add your specialized code here.
}


long SVMicroLogixAPIClass::Open(TCHAR *apConnectionString)
{
   long     lErr;


//   TCHAR *pointer;

   lErr = 0;

   while (1)
   {
//      ParseSetupParameterString (apConnectionString);

//--- Copy to class member
      sv_tstrncpy (svmczConnectionString, 
                   apConnectionString, 
                   SV_NBRCHARS (svmczConnectionString));

      sv_strncpy (svmczNonUCDriverName,
                  T2A (m_ConnectionData.svmczDriverName),
                  sizeof (svmczNonUCDriverName));

      sv_strncpy (svmczNonUCIPAddress,
                  T2A (m_ConnectionData.svmczIPAddress),
                  sizeof (svmczNonUCIPAddress));


      lErr = SVPLCBaseClass::Open ();
      if (lErr < 0)
         break;


      break;
   }

   return lErr;
}


long SVMicroLogixAPIClass::DataTypeSize (SVPLCDataTypeEnum aDataType)
{
   long lRetValue;

   lRetValue = 0;

   while (1)
   {
//--- Actually, only the Micrologix 1200 and 1500 support Longs, the 
//--- Micrologix 1000 does not. ---------------------------------------------
      if (aDataType == SV_PLC_INT32)
      {
         lRetValue = 4;
      }
      else
      {
         lRetValue = SVPLCBaseClass::DataTypeSize (aDataType);
      }
      break;
   }

   return lRetValue;
}



long SVMicroLogixAPIClass::InitializeBlocks(SVPLCBlockStruct* apBlock)
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
                        svmczNonUCIPAddress,
                        "SLC500",
                        svmlDriverId);  // driver_id
               break;
            }
         case SV_PLC_INT32:
            {

               sprintf (pBlock->svmDefinitionString, "%s,%d,%s,%s,%s,%s,%s,%d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "LONG",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCIPAddress,
                        "SLC500",
                        svmlDriverId);  // driver_id
               break;
            }
         case SV_PLC_STRING:
            {
               sprintf (pBlock->svmDefinitionString, "%s,%d,%s,%s,%s,%s,%s,%d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "RAW",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCIPAddress,
                        "SLC500",
                        svmlDriverId);  // driver_id

               break;
            }
         case SV_PLC_SPFLOAT:
            {
//------------ Micrologix does not support floating point values.
               lErr = -1366;
               break;
            }
         default:
            {
               lErr = -1367;
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVMicroLogixAPIClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:01:34   bWalter
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
 *    Rev 1.0   15 Nov 2001 15:07:40   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The SVMicroLogixAPIClass class was added.  This derived class represents the MicroLogix series PLCs as connected via ethernet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/