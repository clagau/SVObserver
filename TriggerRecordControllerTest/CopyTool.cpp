//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file CopyTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy the image to a new buffer.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "CopyTool.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
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
#pragma endregion Constructor

#pragma region Public Methods
void CopyTool::reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvTrc::ITriggerRecordControllerRW& recordController)
{
	ToolObject::reset(sourceId, sourcePos, bufferStructIn, recordController);
	m_bufferStructOut = m_bufferStructIn;
	m_trPos = recordController.addOrChangeImage(getObjectId(), m_bufferStructOut);
};

bool CopyTool::run(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
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
