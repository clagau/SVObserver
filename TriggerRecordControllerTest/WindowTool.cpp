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
namespace SvTRCT
{
#pragma region Constructor
#pragma endregion Constructor

#pragma region Public Methods
	void WindowTool::reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn)
	{
		ToolObject::reset(pos, bufferStructIn);
		m_bufferStructOut = m_bufferStructIn;
		if (m_bufferStructIn.m_lSizeX - 2 * m_offsetX > 0)
		{
			m_bufferStructOut.m_lSizeX = m_bufferStructIn.m_lSizeX - 2 * m_offsetX;
		}

		if (m_bufferStructIn.m_lSizeY - 2 * m_offsetY > 0)
		{
			m_bufferStructOut.m_lSizeY = m_bufferStructIn.m_lSizeY - 2 * m_offsetY;
		}
	};

	bool WindowTool::run(SvTRC::ITriggerRecordRWPtr pTriggerRecord)
	{
		bool retValue = false;
		const auto pSourceImage = pTriggerRecord->getImage(m_pos);
		auto pDestinationImage = pTriggerRecord->createNewImageHandle(m_pos + 1);

		if (nullptr != pSourceImage && nullptr != pSourceImage->getHandle() && nullptr != pDestinationImage && nullptr != pDestinationImage->getHandle())
		{
			HRESULT hr = SVMatroxBufferInterface::CopyBuffer(pDestinationImage->getHandle()->GetBuffer(), pSourceImage->getHandle()->GetBuffer(), -10, -10);
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
