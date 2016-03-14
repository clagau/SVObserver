//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamList
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:54  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxGigeCameraParamList.h"

SVMatroxGigeCameraParamList::SVMatroxGigeCameraParamList() 
: m_bUseTransient(false) 
{
}

void SVMatroxGigeCameraParamList::clear() 
{
	m_mainList.clear();
	m_transientList.clear();
	m_bUseTransient = false;
}

