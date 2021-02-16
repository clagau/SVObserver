//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file DeactivedTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which switch between active and deactive.
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
	class DeactivedTool : public ToolObject
	{
#pragma region Constructor
	public:
		explicit DeactivedTool(uint32_t id) : ToolObject(id) {};
		virtual ~DeactivedTool() = default;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvOi::ITriggerRecordControllerRW* pTcrRW) override;
		virtual CString getName() const override { return _T("(De)actived Tool"); };
		static CString getDescription() { return _T("Copy the whole input image every second time."); };
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
		bool m_isActive = false;
#pragma endregion Member Variables
	};
} //namespace SvTrcT
