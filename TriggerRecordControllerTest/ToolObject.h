//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ToolObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Dummy for the ToolObject
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "TriggerRecordController\ITriggerRecordRW.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTRCT
{
	class ToolObject
	{
#pragma region Constructor
	public:
		ToolObject(GUID guid) : m_guid(guid) {};

#pragma endregion Constructor

#pragma region Public Methods
	public:
		/// This method reset the tool to the new condition. It set the position and the input buffer struct and reset the output buffer struct.
		/// \param pos [in] Position of the tool in the tool set.
		/// \param bufferStructIn [in] Input buffer struct.
		virtual void reset(int pos, const SVMatroxBufferCreateStruct& bufferStructIn) { m_pos = pos, m_bufferStructIn = bufferStructIn; };
		const GUID& getGuid() const { return m_guid; };
		const SVMatroxBufferCreateStruct& getBufferIn() const { return m_bufferStructIn; };
		const SVMatroxBufferCreateStruct& getBufferOut() const { return m_bufferStructOut; };
		virtual CString getName() const = 0;

		/// After an trigger, this method will run the operation of the tool.
		/// \param pTriggerRecord [in] The current record.
		/// \returns bool
		virtual bool run(SvTRC::ITriggerRecordRWPtr pTriggerRecord) = 0;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:

#pragma endregion Private Methods

#pragma region Member Variables
	protected:
		GUID m_guid;
		int m_pos;
		SVMatroxBufferCreateStruct m_bufferStructIn;
		SVMatroxBufferCreateStruct m_bufferStructOut;
#pragma endregion Member Variables
	};
} //namespace SvTRCT
