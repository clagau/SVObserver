//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLevelCreateStruct
//* .File Name       : $Workfile:   SVObjectLevelCreateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectLevelCreateStruct.h"
#include "SVObjectClass.h"
#pragma endregion Includes

SVObjectLevelCreateStruct::SVObjectLevelCreateStruct(SVObjectClass& rOwner)
: m_rOwner(rOwner)
{
}

SVObjectLevelCreateStruct::~SVObjectLevelCreateStruct()
{
}

