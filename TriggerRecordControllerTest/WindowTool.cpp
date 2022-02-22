//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CopyTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy only a part of the image to a new buffer.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "WindowTool.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "TriggerRecordControllerTestDlg.h"
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
WindowTool::WindowTool(uint32_t id)
	: ToolObject(id)
{
	m_childId = getNextObjectId();
};
#pragma endregion Constructor

#pragma region Public Methods
void WindowTool::reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvOi::ITriggerRecordControllerRW& recordController)
{
	ToolObject::reset(sourceId, sourcePos, bufferStructIn, recordController);
	m_bufferStructOut = m_bufferStructIn;
	MatroxBufferChildDataStruct childStruct;
	childStruct.m_lBand = 1;
	childStruct.m_lParentBandCount = 1;
	childStruct.m_lOffX = m_offsetX;
	childStruct.m_lOffY = m_offsetY;
	childStruct.m_lSizeX = m_bufferStructIn.m_lSizeX - 2 * m_offsetX;
	childStruct.m_lSizeY = m_bufferStructIn.m_lSizeY - 2 * m_offsetY;
	recordController.addOrChangeChildImage(m_childId, sourceId, childStruct);

	m_bufferStructOut = m_bufferStructIn;
	m_bufferStructOut.m_lSizeX = childStruct.m_lSizeX;
	m_bufferStructOut.m_lSizeY = childStruct.m_lSizeY;
	m_trPos = recordController.addOrChangeImage(getObjectId(), m_bufferStructOut);
};

bool WindowTool::run(const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	bool retValue = false;
	const auto pSourceImage = pTriggerRecord->getImage(m_sourcePos);
	auto pDestinationImage = pTriggerRecord->createNewImageHandle(m_trPos);

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
