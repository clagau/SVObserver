// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDeviceParamCollection
// * .File Name       : $Workfile:   SVMatroxDeviceParamCollection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:33:06  $
// ******************************************************************************

#pragma once
#ifndef __SVMATROXDEVICESTRUCT_H__
#define __SVMATROXDEVICESTRUCT_H__

#include <SVDeviceParamCollection.h>
#include <SVDeviceParams.h>
#include <SVImagingDeviceParams.h>
#include <SVTemplate.h>
#include <map>

/*
struct SVMatroxDeviceSettingStruct
{
	bool    bSupports;
	long    lVal;
	long    lMin;
	long    lMax;
	long    lMil;

	SVMatroxDeviceSettingStruct()
	{
		bSupports = false;
		lVal = 0;
		lMin = 0;
		lMax = 0;
		lMil = 0;
	}
};
*/

struct SVMatroxDeviceParamCollection : public SVDeviceParamCollection
{
	// these are used in the allocation of a digitizer
	int     iMilDigNum;
	MIL_ID  milSystem;

	// these are filled after allocation of a digitizer
	MIL_ID  milDig;
	/*
	long    lBands;
	long    lSizeX;
	long    lSizeY;
	CString sSerialNumber;
	__int64 iSerialNumber;
	bool    bHardwareSupportsFormat7;
	CStdArray<CString, CString> saSupportedFormats;

	std::map < long, SVMatroxDeviceSettingStruct > mapSettings;
	// these are set based on our camera file settings
	bool    bSupportsHardwareTrigger;
	bool    bSupportsFormat7;
	*/
	bool	bHardwareTrigger;	// used only as a shortcut;
	bool    bValidDigitizer;
	bool    bRealStartFrame;




	SVMatroxDeviceParamCollection()
	{
		ClearDig();
		milSystem = M_NULL;
		bHardwareTrigger = false;
		bValidDigitizer = false;
		bRealStartFrame = false;
		/*
		iSerialNumber = 0;
		bSupportsHardwareTrigger = false;
		bSupportsFormat7 = false;
		bHardwareSupportsFormat7 = false;
		*/
	}

	void ClearDig()
	{
		milDig = M_NULL;
		/*
		lBands = 0;
		lSizeX = 0;
		lSizeY = 0;
		sSerialNumber = _T("");
		iSerialNumber = 0;
		*/
	}
	bool operator < (const SVMatroxDeviceParamCollection& a)
	{
		// we have a special case for 0 so that any bogus serial numbers are pushed to the end
		SVi64ValueDeviceParam* pParam;
		const SVi64ValueDeviceParam* pParamA;
		GetParameter( DeviceParamSerialNumber, pParam );
		pParamA = a.Parameter( DeviceParamSerialNumber ).DerivedValue( pParamA );
		if ( pParamA == NULL || pParamA->iValue == 0 )
			return true;
		else if ( pParam == NULL || pParam->iValue == 0 )
			return false;
		else
			return pParam->iValue < pParamA->iValue;
	}
};

typedef CStdArray < SVMatroxDeviceParamCollection, SVMatroxDeviceParamCollection& > SVMatroxDeviceParamCollectionArray;

#endif// #ifndef __SVMATROXDEVICESTRUCT_H__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxDeviceParamCollection.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:33:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Dec 2003 14:34:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed logic to allow multiple outstanding Triggers (2 by default)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Nov 2003 11:53:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed color stuff
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jul 2003 09:48:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include <SVImagingDeviceParams.h>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 11:21:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added files to support Matrox acquisition devices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/