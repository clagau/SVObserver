//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverEnums
//* .File Name       : $Workfile:   SVObserverEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   10 Jun 2013 16:43:58  $
//******************************************************************************

#ifndef SVOBSERVERENUMS_H
#define SVOBSERVERENUMS_H

#include <deque>

enum
{
	SVDH_MIL_DIGITIZER    = 0x0000FFFF,
	SVDH_SAPERA_DIGITIZER = 0xFFFF0000
};

// Mask descriptor...
enum
{
	SV_NOT_SET = 0x00000000,
	SV_MIL     = 0x00000001,
	SV_SAPERA  = 0x00000002,
	SV_HOST    = 0x00000004
};

enum SVIMProductEnum
{
	SVIM_PRODUCT_INVALID_TYPE = 0,
	SVIM_PRODUCT_FULL       = 1000,
	SVIM_PRODUCT_05         = 1001,
	SVIM_PRODUCT_RGB_MONO   = 1002,
	SVIM_PRODUCT_RGB_COLOR  = 1003,
	SVIM_PRODUCT_D3         = 1004,	// equivalent to the old SVIM_1394_BOARD
	//SVIM_TEST_BOARD       = 1005, // not used
	SVIM_PRODUCT_SVPC       = 1006,
	SVIM_PRODUCT_DIGITAL    = 1007,
	SVIM_PRODUCT_D1         = 1008,
	SVIM_PRODUCT_D2         = 1009,
	SVIM_PRODUCT_D1_COLOR   = 1010,
	SVIM_PRODUCT_D2_COLOR   = 1011,
	SVIM_PRODUCT_D3_COLOR   = 1012,
	SVIM_PRODUCT_D1_HUB     = 1013,
	SVIM_PRODUCT_D2_HUB     = 1014,
	SVIM_PRODUCT_D3_HUB     = 1015,
	SVIM_PRODUCT_X1         = 1016,
	SVIM_PRODUCT_X1_COLOR   = 1017,
	SVIM_PRODUCT_X1_HUB     = 1018,
	SVIM_PRODUCT_X3         = 1019,
	SVIM_PRODUCT_X3_COLOR   = 1020,
	SVIM_PRODUCT_X2			= 1021,
	SVIM_PRODUCT_X2_COLOR	= 1022,
	SVIM_PRODUCT_X2_GD2A	= 1023, // Kontron GIGE Matrox Dongle
	SVIM_PRODUCT_X2_GD2A_COLOR	= 1024, // Kontron GIGE Matrox Dongle Color
	SVIM_PRODUCT_X2_GD4A	= 1025,
	SVIM_PRODUCT_X2_GD4A_COLOR	= 1026,
	SVIM_PRODUCT_X2_GD8A	= 1027,
	SVIM_PRODUCT_X2_GD8A_COLOR	= 1028,
	SVIM_PRODUCT_X2_GD1A	= 1029,
	SVIM_PRODUCT_X2_GD1A_COLOR	= 1030,
	SVIM_PRODUCT_X2_GD8A_NONIO	= 1031,
	SVIM_PRODUCT_X2_GD8A_NONIO_COLOR = 1032,
	SVIM_PRODUCT_TYPE_UNKNOWN = 0xFFFF,
};

typedef std::deque<SVIMProductEnum> SVIMProductEnumList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVConfigurationLibrary\SVObserverEnums.h_v  $
 * 
 *    Rev 1.0   10 Jun 2013 16:43:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 May 2013 10:05:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums to SVConfigurationLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:44:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   21 Jan 2013 11:40:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Non I/O product identifiers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Jan 2013 16:11:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Added new typedef to file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   11 Dec 2012 16:41:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SVIMProductEnum) Added SVIM_PRODUCT_X2_GD1A and SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   13 Aug 2012 11:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   09 Jul 2012 13:45:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A option for compatibility with this legacy product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Jul 2012 14:23:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 Jul 2012 13:26:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * Added new configuration type SVIM X2-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   12 Aug 2011 14:16:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   11 Aug 2011 07:28:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added system type and product type to support GigE color cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Dec 2009 12:08:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   10 Nov 2009 12:47:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Gige Color system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Jun 2009 08:49:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVIM_PRODUCT_X2_GD2A definition
 * Added SVIM_PRODUCT_X2_GD2A_COLOR definition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Jan 2009 12:33:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for X2 platform
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Feb 2007 08:31:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X3 Product Type Enum.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Nov 2005 16:03:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 Jul 2005 14:34:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVIM_PRODUCT_D2_HUB and SVIM_PRODUCT_D3_HUB
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Feb 2005 14:57:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   26 Oct 2004 09:10:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVIM_PRODUCT_D1_HUB
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Jul 2004 10:37:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVIM_PRODUCT_INVALID_TYPE enum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Nov 2003 13:44:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed enumeration names
 * added board enumerations
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Aug 2003 12:54:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated information used to compile and handle new hardware resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Jul 2003 10:42:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Removed Imaging items from file and moved them to SVImageLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 12:20:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Mar 2003 07:58:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Digital and Test SVIM types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Mar 2003 07:10:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SVIMProductEnum (moved from SVOConfigAssistantDlg)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Feb 2003 10:12:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SV_FALSE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:09:02   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved commonly used imaging enums to central source include file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
