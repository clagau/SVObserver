// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSLC504APIClass
// * .File Name       : $Workfile:   SLC504APIClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:36:44  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"
#include "SVABPLCBaseClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <afxpriv.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVSLC504APIClass::SVSLC504APIClass(long* aplErr):
SVRSLinxABPLCBaseClass (aplErr)
{
   *aplErr = 0;

   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_SLC500;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_SLC504;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_APIKTX;

//   svmlDriverId = 0;
   strcpy (svmczNonUCPortId, "AB:LOCAL");

}



SVSLC504APIClass::~SVSLC504APIClass()
{

}



long SVSLC504APIClass::Open (TCHAR *apConnectionString)
{
   long  lErr;

   lErr = 0;
      
   while (1)
   {

//      ParseSetupParameterString (apConnectionString);
// TRB - Move Parse setup ... to Interface class

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



long SVSLC504APIClass::InitializeBlocks (SVPLCBlockStruct* apBlock)
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
                        "SLC500",
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
                        "SLC500",
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
                        "SLC500",
                        svmlDriverId);  // driver_id


               break;
            }
         default:
            {
               lErr = -1240;
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SLC504APIClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:36:44   bWalter
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
 *    Rev 1.0   25 Apr 2013 15:36:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   14 Nov 2001 09:58:36   jBrown
   Project:  SVFocusNT
   Change Request (SCR) nbr:  131, 141
   SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     The name of the base class from which SVSLC504APIClass was derived was changed from SVSLC500BaseClass to SVABPLCBaseClass.
   
   The following functions were modified in order to support Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs:
   
   SVSLC504APIClass::SVSLC504APIClass (), and
   SVSLC504APIClass::Open ()
   
   
   /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:10   mike
 * Initial revision.
*/