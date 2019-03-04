//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordControllerR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecordController in read-modus.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

namespace SvPb
{
	class ImageList;
	class InspectionList;
}

namespace SvTrc
{
class ITriggerRecordR;
typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
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

		/// Return a list of the data definitions.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns const SvPb::DataDefinitionList&
		virtual const SvPb::DataDefinitionList& getDataDefList(int inspectionPos) = 0;

		/// Create a trigger record instance from trigger record with trigger record ID, if it exist.
		/// \param inspectionPos [in] ID of the inspection.
		/// \param trId [in] Trigger record ID
		/// \returns SvTrc::ITriggerRecordRPtr 
		virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) = 0;

		/// Register a Callback function to call if reset is started (writer) or finished (reader).
		/// \param pCallback [in] Pointer of the callback-function.
		/// \returns int handleId of the Callback. It is needed to unregister the callback. (< 0 register was not successfully)
		virtual int registerResetCallback(std::function<void()> pCallback) = 0;

		/// Unregister a Callback function to call if reset is started (writer) or finished (reader).
		/// \param handleId [in] The handleId was get by register of the callback-function.
		virtual void unregisterResetCallback(int handleId) = 0;

		/// Register a Callback function to call if a new trigger record is finished.
		/// \param pCallback [in] Pointer of the callback-function. First parameter is inspection and second is trId.
		/// \returns int handleId of the Callback. It is needed to unregister the callback.
		virtual int registerNewTrCallback(std::function<void(int, int)> pCallback) = 0;

		/// Unregister a Callback function to call if a new trigger record is finished.
		/// \param handleId [in] The handleId was get by register of the callback-function. (< 0 register was not successfully)
		virtual void unregisterNewTrCallback(int handleId) = 0;
	};

	enum class TRC_DataType
	{
		Local,
		Writer,
		Reader
	};
	void createTriggerRecordControllerInstance(TRC_DataType dataType);

	/// Get an instance of the TR controller in read modus.
	/// \returns ITriggerRecordControllerR&
	ITriggerRecordControllerR& getTriggerRecordControllerRInstance();

	/// Return the position of the inspection. If not available, it return -1.
	/// \param inspectionGuid [in] Guid of the inspection.
	/// \returns int
	int getInspectionPos(const GUID& inspectionGuid);
} // namespace SvTrc
