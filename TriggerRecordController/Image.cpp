//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
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
Image::Image(SvOi::SVImageBufferHandlePtr pBuffer, long ResetId, int bufferPos, bool isReadOnly, bool shouldUnlockAuto) :
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
			ImageBufferController::getImageBufferControllerInstance().decreaseRefCounter(m_bufferPos);
		}
	}
}

bool Image::isValid() const
{
	return (TriggerRecordController::getTriggerRecordControllerInstance().getResetId() == m_ResetId) || (cLocalTmpImagePos == m_bufferPos);
}

} //namespace SvTRC