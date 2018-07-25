//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ToolObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Dummy for the ToolObject
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ToolObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTrcT
{
#pragma region Public Methods
void ToolObject::reset(const GUID& sourceGuid, const SVMatroxBufferCreateStruct& bufferStructIn)
{
	m_sourceGuid = sourceGuid;
	m_bufferStructIn = bufferStructIn;
};

#pragma endregion Public Methods

}