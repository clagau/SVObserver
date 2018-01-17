//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleInterface
//* .File Name       : $Workfile:   SVImageBufferHandleInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:42  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVUtilityLibrary/SVPOINT.h"
#pragma endregion Includes

class SVMatroxBuffer;

namespace SvOi
{
	class SVImageBufferHandleInterface
	{
	public:
		virtual ~SVImageBufferHandleInterface() {};

		virtual bool empty() const = 0;
		virtual void clear() = 0;

		virtual SVPoint GetPositionPoint() const = 0;
		virtual SVBitmapInfo GetBitmapInfo() const = 0;
		virtual unsigned char* GetBufferAddress() const = 0;

		virtual const SVMatroxBuffer& GetBuffer() const = 0;
		virtual SVMatroxBuffer& GetBuffer() = 0;

		//************************************
		//! GetHBitmap - convert matrox buffer into a HBITMAP
		//! \returns a HBITMAP handle.
		//************************************
		virtual HBITMAP GetHBitmap() const = 0;

	protected:
		SVImageBufferHandleInterface() {};

	private:
		// Do not implement
		SVImageBufferHandleInterface(const SVImageBufferHandleInterface& p_rObject) = delete;

		// Do not implement
		const SVImageBufferHandleInterface& operator=(const SVImageBufferHandleInterface& p_rObject) = delete;

	};

	typedef std::shared_ptr< SVImageBufferHandleInterface > SVImageBufferHandlePtr;
} //namespace SvOi

