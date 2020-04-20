//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file CopyTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which copy the image to a new buffer.
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
class CopyTool : public ToolObject
{
#pragma region Constructor
public:
	CopyTool(uint32_t id) : ToolObject(id) {};
	virtual ~CopyTool() = default;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvTrc::ITriggerRecordControllerRW& recordController) override;
		virtual CString getName() const override { return _T("Copy Tool"); };
		static CString getDescription() { return _T("Copy the whole input image."); };
		virtual bool run(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord) override;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:

#pragma endregion Private Methods

#pragma region Member Variables
	protected:
#pragma endregion Member Variables
	};
} //namespace SvTrcT
