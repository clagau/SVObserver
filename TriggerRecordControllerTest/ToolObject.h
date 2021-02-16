//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ToolObject.h
/// All Rights Reserved 
//*****************************************************************************
/// Dummy for the ToolObject
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions\ObjectDefines.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvTrcT
{
	class ToolObject
	{
#pragma region Constructor
	public:
		explicit ToolObject(uint32_t objectId) : m_objectId(objectId) {};
		virtual ~ToolObject() = default;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		/// This method reset the tool to the new condition. It set the position and the input buffer struct and reset the output buffer struct.
		/// \param pos [in] Position of the tool in the tool set.
		/// \param bufferStructIn [in] Input buffer struct.
		virtual void reset(uint32_t , int sourcePos, const SVMatroxBufferCreateStruct& bufferStructIn, SvOi::ITriggerRecordControllerRW* ) { m_sourcePos = sourcePos; m_bufferStructIn = bufferStructIn; };
		uint32_t getObjectId() const { return m_objectId; };
		int getImagePos() const { return m_trPos; };
		const SVMatroxBufferCreateStruct& getBufferIn() const { return m_bufferStructIn; };
		const SVMatroxBufferCreateStruct& getBufferOut() const { return m_bufferStructOut; };
		virtual CString getName() const = 0;

		/// After an trigger, this method will run the operation of the tool.
		/// \param pTriggerRecord [in] The current record.
		/// \returns bool
		virtual bool run(const SvOi::ITriggerRecordRWPtr& pTriggerRecord) = 0;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:

#pragma endregion Private Methods

#pragma region Member Variables
	protected:
		int m_trPos {-1};
		uint32_t m_objectId = SvDef::InvalidObjectId;
		int m_sourcePos {-1};
		SVMatroxBufferCreateStruct m_bufferStructIn;
		SVMatroxBufferCreateStruct m_bufferStructOut;
#pragma endregion Member Variables
	};
} //namespace SvTrcT
