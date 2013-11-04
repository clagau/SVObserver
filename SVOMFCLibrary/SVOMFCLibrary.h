// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOMFCLibrary
// * .File Name       : $Workfile:   SVOMFCLibrary.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:09:26  $
// ******************************************************************************

#ifndef SVOMFCLIBRARY_H
#define SVOMFCLIBRARY_H

#include "SVOMFCLibraryGlobals.h"

const CString SVO_PRODUCT_CORECO_A4_MONO	( _T("SVIM A4 MONO") );
const CString SVO_PRODUCT_CORECO_A2_MONO	( _T("SVIM A2 MONO") );
const CString SVO_PRODUCT_CORECO_A1_RGB		( _T("SVIM A1 RGB") );
const CString SVO_PRODUCT_MATROX_D1			  ( _T("SVIM D1") );
const CString SVO_PRODUCT_MATROX_D2			  ( _T("SVIM D2") );
const CString SVO_PRODUCT_MATROX_D3			  ( _T("SVIM D3") );
const CString SVO_PRODUCT_INTEK_D1			  ( _T("SVIM X1") );
const CString SVO_PRODUCT_INTEK_D3			  ( _T("SVIM X3") );
const CString SVO_PRODUCT_KONTRON_X2		( _T("SVIM X2") );
const CString SVO_PRODUCT_KONTRON_X2_GD1A	( _T( "SVIM X2-GD1A" ) );
const CString SVO_PRODUCT_KONTRON_X2_GD2A	( _T("SVIM X2-GD2A") );
const CString SVO_PRODUCT_KONTRON_X2_GD4A   ( _T("SVIM X2-GD4A") );
const CString SVO_PRODUCT_KONTRON_X2_GD8A   ( _T("SVIM X2-GD8A") );
const CString SVO_PRODUCT_KONTRON_X2_GD8A_NONIO ( _T("SVIM Non I/O") );

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOMFCLibrary.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:09:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jan 2013 16:05:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added string for Non I/O SVIM
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Dec 2012 16:35:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (global) Added SVO_PRODUCT_KONTRON_X2_GD1A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Aug 2012 11:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Jul 2012 13:20:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A for compatibility with legacy product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 14:19:06   jspila
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
 *    Rev 1.7   02 Jul 2012 13:20:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Jul 2012 12:47:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Aug 2011 14:19:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Product Type String for "SVIM X2-G4A" for the 4 port GigE product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jun 2009 17:35:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Kontron X2 GD2A product string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jan 2009 10:40:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Feb 2007 08:48:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Support for X3 Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Nov 2005 15:28:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added constant strings for product name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
