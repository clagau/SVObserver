//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionConstructParams
//* .File Name       : $Workfile:   TriggerHandling/SVAcquisitionConstructParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:50  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVImageLibrary/SVLut.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

struct SVAcquisitionConstructParams
{
	SVAcquisitionConstructParams();
	SVAcquisitionConstructParams( const SVAcquisitionConstructParams& p_rObject );
	SVAcquisitionConstructParams( LPCTSTR p_szDeviceName, LPCTSTR p_szDigitizerName,
		int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize,
		const SVLutInfo& p_rLUTInfo, int p_LRBandSize, int p_LRConstrast, int p_LRBrightness );
	SVAcquisitionConstructParams( LPCTSTR p_szDeviceName, LPCTSTR p_szDigitizerName,
		int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize );

	virtual ~SVAcquisitionConstructParams();

	const SVAcquisitionConstructParams& operator=( const SVAcquisitionConstructParams& p_rObject );

	SVString m_DeviceName;
	SVString m_DigitizerName;

	int m_Channel;
	int m_DigNumber;
	int m_BandSize;
	int m_BandMaxSize;

	bool m_LUTAndLRSet;

	SVLutInfo m_LUTInfo;

	int m_LRBandSize;
	int m_LRConstrast;
	int m_LRBrightness;
};

typedef std::vector< SVAcquisitionConstructParams > SVAcquisitionConstructParamsVector;
