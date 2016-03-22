//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParam
//* .File Name       : $Workfile:   SVMatroxGigeCameraParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:48  $
//******************************************************************************

#pragma  once

//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <comdef.h>

struct SVMatroxGigeCameraParam
{
	int m_ParameterID;
	int m_ParameterTypeID;
	_variant_t m_value;
	
	SVMatroxGigeCameraParam(int p_ParameterID, int p_ParameterTypeID, _variant_t& p_rValue);
	SVMatroxGigeCameraParam(const SVMatroxGigeCameraParam& rParam);
	~SVMatroxGigeCameraParam();
	
	SVMatroxGigeCameraParam& operator=(const SVMatroxGigeCameraParam& rParam);
	void Copy(const SVMatroxGigeCameraParam& rParam);
};

// List of Camera Parameters (SetParameters)
typedef std::deque<SVMatroxGigeCameraParam> SVMatroxGigeCameraParamDeque;

