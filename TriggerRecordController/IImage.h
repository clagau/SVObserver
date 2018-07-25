//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file IImage.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the Image-Class.
//******************************************************************************
#pragma once
#include "stdafx.h"
#include "ObjectInterfaces\SVImageBufferHandleInterface.h"
//Moved to precompiled header: #include <memory>

namespace SvTrc
{
	class IImage
	{
	public:
		virtual ~IImage() = default;

		/// Return if image is only read-only. If only read only, do not use it for changing mil function.
		/// \returns bool
		virtual bool isReadOnly() const = 0;

		/// Get Handle to the image.
		/// \returns SvOi::SVImageBufferHandlePtr
		virtual SvOi::SVImageBufferHandlePtr getHandle() const = 0;

		/// Get the buffer position. (Needed for internal use)
		/// \returns int
		virtual int getBufferPos() const = 0;

		/// Return if not empty image buffer available. 
		/// \returns bool
		virtual bool isEmpty() const = 0;
	};

	typedef std::shared_ptr<IImage> IImagePtr;
} //namespace SvTrc
