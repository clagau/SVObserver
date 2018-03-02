//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file CopyTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which rotated the image buffer.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RotationTool.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMatroxLibrary\SVMatroxImageRotateStruct.h"
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
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
	void RotationTool::reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn)
	{
		ToolObject::reset(pos, bufferStructIn);
		m_bufferStructOut = m_bufferStructIn;
	};

	bool RotationTool::run(SvTRC::ITriggerRecordRWPtr pTriggerRecord)
	{
		bool retValue = false;
		const auto pSourceImage = pTriggerRecord->getImage(m_pos);
		auto pDestinationImage = pTriggerRecord->createNewImageHandle(m_pos + 1);

		if (nullptr != pSourceImage && nullptr != pSourceImage->getHandle() && nullptr != pDestinationImage && nullptr != pDestinationImage->getHandle())
		{
			SVMatroxImageRotateStruct l_Rotate(pSourceImage->getHandle()->GetBuffer());
			l_Rotate.m_dAngle = m_roationValue;
			l_Rotate.m_dSrcCenX = m_bufferStructIn.m_lSizeX / 2;
			l_Rotate.m_dSrcCenY = m_bufferStructIn.m_lSizeY / 2;
			l_Rotate.m_dDstCenX = l_Rotate.m_dSrcCenX;
			l_Rotate.m_dDstCenY = l_Rotate.m_dSrcCenY;
			l_Rotate.m_eInterpolation = SVImageDefault;

			// Use MimRotate to Rotate, Translate or Copy buffer
			// Rotate...( and translate image )
			HRESULT MatroxCode = SVMatroxImageInterface::Rotate(pDestinationImage->getHandle()->GetBuffer(), l_Rotate);
			retValue = (S_OK == MatroxCode);
		}

		return retValue;
	}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods

#pragma endregion Private Methods
}