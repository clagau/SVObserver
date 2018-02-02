//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordRW.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecordController in read/write-modus.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "ITriggerRecordControllerR.h"
#include "ITriggerRecordRW.h"
#pragma endregion Includes

namespace SvTRC
{
	class ITriggerRecordControllerRW : public ITriggerRecordControllerR
	{
	public:
		/// Set the list of GUID of the available inspections. ATTENTION: This command reset the whole controller.
		/// \param rInspectionList [in] inspection list.
		virtual void setInspections(const SvPB::InspectionList& rInspectionList) = 0;

		/// Create a new trigger record in a free slot and block this slot for this instance in write-modus.
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns SvTRC::ITriggerRecordRWPtr
		virtual ITriggerRecordRWPtr CreateTriggerRecordObjectToWrite(int inspectionPos) = 0;

		/// Delete intern the memory and start the process to create the new memory structure. ATTENTION: All old TR-instances of all IPs have to be deleted before.
		/// \param inspectionPos [in] ID of the inspection.
		/// \param TriggerRecordSize [in] The wished size of slots for the trigger records.
		/// \returns bool True if successfully.
		virtual bool StartResetTriggerRecordStructure(int inspectionPos, int TriggerRecordSize) = 0;

		/// Add a new image to the trigger record structure. It must called StartResetTriggerRecordStructure before.
		/// \param imageId [in] Guid of the image.
		/// \param bufferStruct [in] structure of the buffer of this image.
		/// \returns bool True if successfully.
		virtual bool AddImage2TriggerRecordStructure(const GUID& imageId, SVMatroxBufferCreateStruct bufferStruct) = 0;

		/// Finished the reset of the trigger record structure and create the new memory structure.
		/// \returns bool True if successfully.
		virtual bool FinishResetTriggerRecordStructure() = 0;
	};

	/// Get an instance of the TR controller in read/write modus.
	/// \returns ITriggerRecordControllerRW&
	ITriggerRecordControllerRW& getTriggerRecordControllerRWInstance();
} //namespace SvTRC