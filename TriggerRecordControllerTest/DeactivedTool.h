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
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTRCT
{
	class DeactivedTool : public ToolObject
	{
#pragma region Constructor
	public:
		DeactivedTool(GUID guid) : ToolObject(guid) {};

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn) override;
		virtual CString getName() const override { return _T("(De)actived Tool"); };
		static CString getDescription() { return _T("Copy the whole input image every second time."); };
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
		bool m_isActive = false;
#pragma endregion Member Variables
	};
} //namespace SvTRCT
