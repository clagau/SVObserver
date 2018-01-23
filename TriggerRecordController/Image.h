//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file Image.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\SVImageBufferHandleInterface.h"
#include "IImage.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTRC
{
	class Image : public IImage
	{
#pragma region Constructor
	public:
		Image(SvOi::SVImageBufferHandlePtr buffer, bool isReadOnly = true) : m_buffer(buffer), m_isReadOnly(isReadOnly) {};

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool isReadOnly() const override { return m_isReadOnly; };
		virtual SvOi::SVImageBufferHandlePtr getHandle() const override { return m_buffer; };
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		bool m_isReadOnly = true;
		SvOi::SVImageBufferHandlePtr m_buffer;
#pragma endregion Member Variables
	};
} // namespace SvTRC