// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:44:34  $
// ******************************************************************************

#ifndef SVIOCONFIGURATIONINTERFACECLASS_INL
#define SVIOCONFIGURATIONINTERFACECLASS_INL

#include "SVIOConfigurationInterfaceClass.h"

SVIOConfigurationInterfaceClass& SVIOConfigurationInterfaceClass::Instance()
{
	static SVIOConfigurationInterfaceClass l_Object;

	return l_Object;
}

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOConfigurationInterfaceClass.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:44:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   15 Aug 2012 14:21:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  779
   SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated SVIOConfigurationInterfaceClass to become a singleton.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   15 Oct 2010 14:49:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  701
   SCR Title:  Add New Font Functionality to the Existing Command Interface
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove casting isssues and invalid return values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   20 Jun 2007 12:59:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   27 Jun 2006 08:49:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  564
   SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Set and Get Parameter Functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   22 Feb 2005 10:56:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  472
   SCR Title:  Fix problems with Basler cameras
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Switched logic to set flag for trigger and strobe inverters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   15 Jun 2004 13:56:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated ReadDDEInput and WriteDDEOutput methods to handle changes in SVDDEItemData struct.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jun 2004 13:47:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated ReadDDEInput and WriteDDEOutput methods to use the changes to the SVDDEItemData structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   11 Jun 2004 14:06:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated GetDigitalOutput method to fix the indexing problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   08 Jun 2004 11:24:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated GetDigitalOutputValue, SetDigitalOutputValue, and GetDigitalOutputState methods to fix a DDE issue with the data collected from the Digital IO and sent to the DDE State output.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   22 Jan 2004 12:25:34   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed name of function
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   21 Jan 2004 15:24:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added code to make strobe start frame optional
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   19 Jan 2004 17:19:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed bug with IO startup system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   16 Jan 2004 07:19:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added functions to allow configuring IO to work with special SVIM hardware
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   24 Nov 2003 09:34:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated Trigger and Strobe invert functions to select the correct lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   24 Nov 2003 07:02:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated Trigger and Strobe invert methods to handle all outputs instead of only the first 16.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   13 Nov 2003 09:20:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated ClearDigitalOutputs method to fix the problem with inverted or forced values set in I/O document.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   11 Nov 2003 10:44:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated both Invert Trigger and Invert Strobe methods to fix a variable problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   16 Oct 2003 14:52:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Update digital output port methods to fix problems with not setting enough data and added methods to handle the inverters for the tiggers and strobs.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
