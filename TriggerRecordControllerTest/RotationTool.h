//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file RotationTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which rotated the image buffer.
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
	class RotationTool : public ToolObject
	{
#pragma region Constructor
	public:
		RotationTool(GUID guid) : ToolObject(guid) {};

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn) override;
		virtual CString getName() const override { return _T("Rotation Tool"); };
		static CString getDescription() { return _T("Rotated the input image (by 45°)."); };

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
		double m_roationValue = 45;
#pragma endregion Member Variables
	};
} //namespace SvTRCT
