//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RotationTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Class for a tool which rotated the image buffer.
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
	class RotationTool : public ToolObject
	{
#pragma region Constructor
	public:
		explicit RotationTool(uint32_t id) : ToolObject(id) {};
		virtual ~RotationTool() = default;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual void reset(uint32_t sourceId, int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvOi::ITriggerRecordControllerRW& recordController) override;
		virtual CString getName() const override { return _T("Rotation Tool"); };
		static CString getDescription() { return _T("Rotated the input image (by 45°)."); };

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
		double m_roationValue = 45;
#pragma endregion Member Variables
	};
} //namespace SvTrcT
