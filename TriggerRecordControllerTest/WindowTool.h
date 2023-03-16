//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file WindowTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy only a part of the image to a new buffer.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ToolObject.h"
#pragma endregion Includes

#pragma region Declarations
struct SVMatroxBufferCreateStruct;
#pragma endregion Declarations
namespace SvTrcT
{
	class WindowTool : public ToolObject
	{
#pragma region Constructor
	public:
		explicit WindowTool(uint32_t id);
		virtual ~WindowTool() = default;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvOi::ITriggerRecordControllerRW& recordController) override;
		virtual CString getName() const override{ return _T("Window Tool"); };
		static CString getDescription() { return _T("Copy a part of the input image (10 pixel smaller on all sides)."); };

		virtual bool run(const SvOi::ITriggerRecordRWPtr& pTriggerRecord) override;
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
		uint32_t m_childId;
		int m_childPos {-1};
#pragma endregion Member Variables
	};
} //namespace SvTrcT
