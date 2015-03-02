//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	John Doe
//*****************************************************************************
/// This class implemented the interface IMatroxImageData
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IMatroxImageData.h"
#include "SVImageBufferHandleInterface.h"
#include "SVImageBufferHandleImage.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class MatroxImageData : virtual public SvOi::IMatroxImageData
/// \brief This class implemented the interface IMatroxImageData.
{
#pragma region Constructor
public:
    MatroxImageData(SVSmartHandlePointer imageHandle);

#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region virtual method (IMatroxImageData)
	virtual bool empty() const override;

	virtual SVBitmapInfo getBitmapInfo() const override;

	virtual BYTE* getBufferAddress() const override;

	virtual bool GetBuffer(SVMatroxBuffer& buffer) const override;
#pragma endregion virtual method (IMatroxImageData)

	void setImageHandle(SVSmartHandlePointer imageHandle) { m_ImageHandle = imageHandle; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:

#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVSmartHandlePointer m_ImageHandle;
#pragma endregion Member Variables
};
