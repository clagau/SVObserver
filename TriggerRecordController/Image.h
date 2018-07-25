//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file Image.h
/// All Rights Reserved 
//*****************************************************************************
/// Class to manage a image(handle)
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\SVImageBufferHandleInterface.h"
#include "IImage.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTrc
{
class Image : public IImage
{
#pragma region Constructor
public:
	/// Constructor
	/// \param pBuffer [in] Handle of the image buffer.
	/// \param ResetTime [in] The time of the last reset of the TR-Controller.	
	/// \param bufferPos [in] Position of the buffer, also needed for reference count.
	/// \param isReadOnly [in] Flag if buffer is read only.
	/// \param shouldUnlockAuto [in] false means no auto lock, if true then it MUST be increased outside, before the call of this constructor.
	Image(SvOi::SVImageBufferHandlePtr pBuffer, long ResetId, int bufferPos, bool isReadOnly = true, bool shouldUnlockAuto = false);

	virtual ~Image();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool isReadOnly() const override { return m_isReadOnly; };
	virtual SvOi::SVImageBufferHandlePtr getHandle() const override { return m_pBuffer; };
	virtual int getBufferPos() const override {	return m_bufferPos;	};
	virtual bool isEmpty() const override { return (nullptr == m_pBuffer || m_pBuffer->empty()); };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:

#pragma endregion Private Methods

#pragma region Member Variables
private:
	const bool m_isReadOnly = true;
	const bool m_shouldUnlockAuto = false;
	const long m_ResetId = 0;
	const int m_bufferPos = -1;  // if pos > -1, the buffer is locked and must be freed in Destructor. 
	SvOi::SVImageBufferHandlePtr m_pBuffer;
#pragma endregion Member Variables
};
} // namespace SvTrc
