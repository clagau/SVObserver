//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file WindowTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy only a part of the image to a new buffer.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ToolObject.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTRCT
{
	class WindowTool : public ToolObject
	{
#pragma region Constructor
	public:
		WindowTool(GUID guid) : ToolObject(guid) {};

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn) override;
		virtual CString getName() const override{ return _T("Window Tool"); };
		static CString getDescription() { return _T("Copy a part of the input image (10 pixel smaller on all sides)."); };

		virtual bool run(SvTRC::ITriggerRecordRWPtr pTriggerRecord) override;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:

#pragma endregion Private Methods

#pragma region Member Variables
	protected:
		int m_offsetX = 10;
		int m_offsetY = 10;
#pragma endregion Member Variables
	};
} //namespace SvTRCT