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

