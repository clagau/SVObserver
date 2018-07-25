//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file CopyTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy only a part of the image to a new buffer.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "WindowTool.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
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
#pragma region Constructor
WindowTool::WindowTool(GUID guid)
	: ToolObject(guid)
{
	UuidCreateSequential(&m_childGuid);
};
#pragma endregion Constructor

#pragma region Public Methods
void WindowTool::reset(const GUID& sourceGuid, const SVMatroxBufferCreateStruct& bufferStructIn, SvTrc::ITriggerRecordControllerRW& recordController)
{
	ToolObject::reset(sourceGuid, bufferStructIn, recordController);
	m_bufferStructOut = m_bufferStructIn;
	MatroxBufferChildDataStruct childStruct;
	childStruct.m_lBand = 1;
	childStruct.m_lParentBandCount = 1;
	childStruct.m_lOffX = m_offsetX;
	childStruct.m_lOffY = m_offsetY;
	childStruct.m_lSizeX = m_bufferStructIn.m_lSizeX - 2 * m_offsetX;
	childStruct.m_lSizeY = m_bufferStructIn.m_lSizeY - 2 * m_offsetY;
	recordController.addOrChangeChildImage(m_childGuid, m_sourceGuid, childStruct);

	m_bufferStructOut = m_bufferStructIn;
	m_bufferStructOut.m_lSizeX = childStruct.m_lSizeX;
	m_bufferStructOut.m_lSizeY = childStruct.m_lSizeY;
	recordController.addOrChangeImage(getGuid(), m_bufferStructOut);
};

bool WindowTool::run(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	bool retValue = false;
	const auto pSourceImage = pTriggerRecord->getImage(m_childGuid);
	auto pDestinationImage = pTriggerRecord->createNewImageHandle(getGuid());

	if (nullptr != pSourceImage && nullptr != pSourceImage->getHandle() && nullptr != pDestinationImage && nullptr != pDestinationImage->getHandle())
	{
		HRESULT hr = SVMatroxBufferInterface::CopyBuffer(pDestinationImage->getHandle()->GetBuffer(), pSourceImage->getHandle()->GetBuffer());
		retValue = (S_OK == hr);
	}

	return retValue;
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods

#pragma endregion Private Methods
}
