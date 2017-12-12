//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jane Doe
//*****************************************************************************
/// This class implemented the interface IMatroxImageData
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MatroxImageData.h"
#include "SVImageBufferHandleImage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
MatroxImageData::MatroxImageData(SVImageBufferHandlePtr imageHandle) : m_ImageHandle( imageHandle )
{
}

MatroxImageData::~MatroxImageData()
{
}

#pragma endregion Constructor

#pragma region Public Methods
#pragma region virtual method (IMatroxImageData)
bool MatroxImageData::empty() const
{
	return (nullptr == m_ImageHandle);
}

SVBitmapInfo MatroxImageData::getBitmapInfo() const
{
	SVBitmapInfo bitmapInfo;
	if (!empty())
	{
		bitmapInfo = m_ImageHandle->GetBitmapInfo();
	}
	return bitmapInfo;	
}

BYTE* MatroxImageData::getBufferAddress() const
{
	BYTE* address = nullptr;
	if (!empty())
	{	
		address = m_ImageHandle->GetBufferAddress();
	}
	return address;
}

bool MatroxImageData::GetBuffer(SVMatroxBuffer& buffer) const
{
	bool retValue = !empty();
	if (retValue)
	{
		SVImageBufferHandleImage BufferHandle;
		m_ImageHandle->GetData( BufferHandle );
		buffer = BufferHandle.GetBuffer();
	}
	
	return retValue; 
}

HBITMAP MatroxImageData::GetHBitmap() const
{
	SVImageBufferHandleImage BufferHandle;
	m_ImageHandle->GetData(BufferHandle);
	const SVMatroxBuffer& buffer = BufferHandle.GetBuffer();
	HBITMAP hBitmap = nullptr;
	HRESULT status = SVMatroxBufferInterface::Create(hBitmap, buffer);

	return hBitmap;
}
#pragma endregion virtual method (IMatroxImageData)
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods

#pragma endregion Private Methods
