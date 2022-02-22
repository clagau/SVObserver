//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParam
//* .File Name       : $Workfile:   SVMatroxGigeCameraParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:48  $
//******************************************************************************
#pragma  once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

struct SVMatroxGigeCameraParam
{
	int m_ParameterID;
	_variant_t m_value;
	
	SVMatroxGigeCameraParam(int p_ParameterID, _variant_t& p_rValue);
	SVMatroxGigeCameraParam(const SVMatroxGigeCameraParam& rParam);
	~SVMatroxGigeCameraParam();
	
	SVMatroxGigeCameraParam& operator=(const SVMatroxGigeCameraParam& rParam);
	void Copy(const SVMatroxGigeCameraParam& rParam);
};

// List of Camera Parameters (SetParameters)
typedef std::deque<SVMatroxGigeCameraParam> SVMatroxGigeCameraParamDeque;

