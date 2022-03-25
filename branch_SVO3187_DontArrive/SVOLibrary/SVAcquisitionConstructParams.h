//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionConstructParams
//* .File Name       : $Workfile:   SVAcquisitionConstructParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:50  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOLibrary/SVLut.h"

#pragma endregion Includes

struct SVAcquisitionConstructParams
{
	SVAcquisitionConstructParams();
	SVAcquisitionConstructParams( const SVAcquisitionConstructParams& p_rObject );
	SVAcquisitionConstructParams( LPCTSTR p_szDigitizerName, int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize,
		const SVLutInfo& p_rLUTInfo, int p_LRBandSize );
	SVAcquisitionConstructParams( LPCTSTR p_szDigitizerName, int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize );

	virtual ~SVAcquisitionConstructParams();

	SVAcquisitionConstructParams& operator=( const SVAcquisitionConstructParams& p_rObject );

	std::string m_DigitizerName;

	int m_Channel;
	int m_DigNumber;
	int m_BandSize;
	int m_BandMaxSize;

	bool m_LUTAndLRSet;

	SVLutInfo m_LUTInfo;

	int m_LRBandSize;
};

typedef std::vector< SVAcquisitionConstructParams > SVAcquisitionConstructParamsVector;
