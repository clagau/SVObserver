// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSLC505APIClass
// * .File Name       : $Workfile:   SLC505APIClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:36:46  $
// ******************************************************************************

#include "stdafx.h"
#include "SLC505APIClass.h"

#include "plc_interface.h"
#include "SLC504APIClass.h" // for defining the call back function.

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

SVSLC505APIClass::SVSLC505APIClass(long* aplErr):
SVRSLinxABPLCBaseClass (aplErr)
{
   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_SLC500;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_SLC505;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_APIETH;

   strcpy (svmczNonUCPortId, "AB:NAME");
}

SVSLC505APIClass::~SVSLC505APIClass()
{
}

long SVSLC505APIClass::Open (TCHAR *apConnectionString)
{
   long     lErr = 0;

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


      lErr = SVRSLinxABPLCBaseClass::Open (apConnectionString);
      if (lErr < 0)
         break;

      break;
   }

   return lErr;
}

long SVSLC505APIClass::InitializeBlocks (SVPLCBlockStruct* apBlock)
{
   long              lErr;
   SVPLCBlockStruct* pBlock;

   lErr = 0;

   while (1)
   {
      SVRSLinxABPLCBaseClass::InitializeBlocks (apBlock);


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
               sprintf (pBlock->svmDefinitionString, "%s,%d,%s,%s,%s,%s,%s,%d",
                        pBlock->svmRebuiltDTAddress,
                        pBlock->svmlNbrElements,
                        "FLOAT",
                        "MODIFY",
                        svmczNonUCPortId,
                        svmczNonUCIPAddress,
                        "SLC500",
                        svmlDriverId);  // driver_id

               break;
            }
         default:
            {
               lErr = -1316;
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SLC505APIClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:36:46   bWalter
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
 *    Rev 1.0   25 Apr 2013 15:37:00   bWalter
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
 * 
 *    Rev 1.3   14 Nov 2001 10:03:56   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The name of the base class from which SVSLC505APIClass was derived was changed from SVSLC500BaseClass to SVABPLCBaseClass.
 * 
 * 
 * 
 * The following functions were modified in order to support Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs:
 * 
 * 
 * 
 * SVSLC505APIClass::SVSLC505APIClass (), and
 * 
 * SVSLC505APIClass::Open ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Dec 2000 19:36:04   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  78
 * SCR Title:  Incorporate reading of strings from the PLC.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The functions 
 * SVSLC504APIClass::InitializeBlocks (), and
 * SVSLC505APIClass::InitializeBlocks ()
 * 
 * were modified to remove setting the fixed string length for Allen-Bradley PLC string data types.  This is now being done in SVSLC500BaseClass::AddElementToBlock ().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2000 17:33:16   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  78
 * SCR Title:  Incorporate reading of strings from the PLC.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following function was modified in order to impliment reading strings from the PLC:
 * 
 *   SVSLC505APIClass::InitializeBlocks ()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 13:52:40   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Extensive changes to support integration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
*/