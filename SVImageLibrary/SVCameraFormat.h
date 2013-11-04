// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraFormat
// * .File Name       : $Workfile:   SVCameraFormat.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:18  $
// ******************************************************************************

#ifndef SVCAMERAFORMAT_H
#define SVCAMERAFORMAT_H

#include "SVOMFCLibrary/SVTemplate.h"
#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVImageEnums.h"

class SVCameraFormat
{
public:
	SVString m_strName;
	SVString strDescription;
	SVImageFormatEnum eImageType;
	long lWidthMax;
	long lHeightMax;
	long lWidth;
	long lHeight;
	long lHStep;
	long lVStep;
	long lHPos;
	long lVPos;
	long lHPosStep;
	long lVPosStep;
	long lFrameRate;
	bool bVariableROI;
	long lOrder;
	bool bColor;
	long lMinTransferTime;
	long lMaxTransferTime;
	bool bOptimizedTransferRate_X;
	bool bOptimizedTransferRate_Y;

	long m_lDCamFormat;
	long m_lDCamMode;
	long m_lDCamFrameRate;
	long m_lDCamColorFormat;

	SVCameraFormat();
	SVCameraFormat( const SVCameraFormat& rhs );
	const SVCameraFormat& operator = ( const SVCameraFormat& rhs );
	void AssignUserChangableValues( const SVCameraFormat& rhs );
	void AssignConstantValues( const SVCameraFormat& rhs );

	HRESULT ParseAndAssignCameraFormat( const CString& sCameraFormat );

private:

	HRESULT SetDCamData( const CString& sCameraFormat );

	void init();
};


#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVCameraFormat.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2011 09:38:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 11:33:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 08:56:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added include for HBitmapUtilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2005 15:18:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new parameter functions for the DLL and new image formats for the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Feb 2004 17:14:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added CalculatedLatency acquisition support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2003 07:56:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added flag for color image format
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:01:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 10:59:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added lVPos and lHPos
 * added AssignUserChangableValues function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jul 2003 10:15:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added lHPos and lVPos,
 * added AssignUserChangableValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:36:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   new file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
