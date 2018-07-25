//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordControllerR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecordController in read-modus.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ITriggerRecordR.h"
#pragma endregion Includes

namespace SvPb
{
	class ImageList;
	class InspectionList;
}

namespace SvTrc
{
	typedef void(CALLBACK *StartResetCallbackPtr)(void* pOwner);

	class ITriggerRecordControllerR
	{
	public:
		virtual ~ITriggerRecordControllerR() = default;

		/// Get the list of GUID of the available inspections.
		/// \returns SvPb::InspectionList
		virtual const SvPb::InspectionList& getInspections() const = 0;

		/// Get the last TRId. if no trigger record used yet it return -1;
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns int
		virtual int getLastTRId(int inspectionPos) const= 0;

		/// Return a list of the image definitions.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns const SvPb::ImageList&
		virtual const SvPb::ImageList& getImageDefList(int inspectionPos) = 0;

		/// Create a trigger record instance from trigger record with trigger record ID, if it exist.
		/// \param inspectionPos [in] ID of the inspection.
		/// \param trId [in] Trigger record ID
		/// \returns SvTrc::ITriggerRecordRPtr 
		virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) = 0;

		/// Register a Callback function to call if reset is started.
		/// \param pOwner [in] Pointer from the owner of the callback function.
		/// \param pCallback [in] Pointer of the callback-function.
		virtual void registerCallback(void* pOwner, StartResetCallbackPtr pCallback) = 0;

		/// Unregister a Callback function to call if reset is started.
		/// \param pOwner [in] Pointer from the owner of the callback function.
		/// \param pCallback [in] Pointer of the callback-function.
		virtual void unregisterCallback(void* pOwner, StartResetCallbackPtr pCallback) = 0;

		/// Return if Reset is started.
		/// \returns bool
		virtual bool isResetStarted() const = 0;
	};

	/// Get an instance of the TR controller in read modus.
	/// \returns ITriggerRecordControllerR&
	ITriggerRecordControllerR& getTriggerRecordControllerRInstance();

	/// Return the position of the inspection. If not available, it return -1.
	/// \param inspectionGuid [in] Guid of the inspection.
	/// \returns int
	int getInspectionPos(const GUID& inspectionGuid);
} // namespace SvTrc
