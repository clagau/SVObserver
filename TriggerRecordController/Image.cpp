//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file Image.cpp
/// All Rights Reserved
//*****************************************************************************
/// Class to manage a image(handle)
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
#include "Image.h"
#include "ResetLocker.h"
#include "ImageBufferController.h"
#include "TriggerRecordController.h"
#include "ObjectInterfaces\SVImageBufferHandleInterface.h"
#pragma endregion Includes

namespace SvTrc
{
Image::Image(const SvOi::SVImageBufferHandlePtr& pBuffer, long ResetId, int bufferPos, bool isReadOnly, bool shouldUnlockAuto) :
	m_pBuffer(pBuffer), m_ResetId(ResetId), m_isReadOnly(isReadOnly), m_bufferPos(bufferPos), m_shouldUnlockAuto(shouldUnlockAuto)
{
};

Image::~Image()
{
	if (m_shouldUnlockAuto && 0 <= m_bufferPos)
	{
		auto pLock = ResetLocker::lockReset(m_ResetId);
		if (nullptr != pLock)
		{
			getImageBufferControllerInstance().decreaseImageRefCounter(m_bufferPos);
		}
	}
}

bool Image::isValid() const
{
	const auto* pTRC = getTriggerRecordControllerInstance();
	long resetId = (nullptr != pTRC) ? pTRC->getResetId() : -1L;

	// cppcheck-suppress knownConditionTrueFalse //constructor can change values and if can be true
	return (resetId == m_ResetId && 0 < m_ResetId && 0 < resetId) || (cLocalTmpImagePos == m_bufferPos);
}

} //namespace SvTRC