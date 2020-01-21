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

	struct TrEventData
	{
		TrEventData(int ipPos, int trId)
			: m_inspectionPos(ipPos)
			, m_trId(trId)
		{
		}

		int m_inspectionPos = -1;
		int m_trId = -1;
	};

	class ITriggerRecordControllerR
	{
	public:
		virtual ~ITriggerRecordControllerR() = default;

		/// Get the list of GUID of the available inspections.
		/// \returns SvPb::InspectionList
		virtual const SvPb::InspectionList& getInspections() const = 0;

		/// Return if the TRC is in a valid state to use. If not init or reset is in progress is return false.
		/// \returns bool
		virtual bool isValid() const = 0;

		/// Get the last TrId. if no trigger record used yet it return -1;
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns int
		virtual int getLastTrId(int inspectionPos) const= 0;

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

		/// Create a trigger record instance from trigger record with trigger count, if it exist.
		/// \param inspectionPos [in] ID of the inspection.
		/// \param triggerCount [in] Trigger count
		/// \returns SvTrc::ITriggerRecordRPtr 
		virtual ITriggerRecordRPtr createTriggerRecordObjectPerTriggerCount(int inspectionPos, int triggerCount) = 0;

		/// Register a Callback function to call if TRC reset. (After this event the TRC is not longer valid. It is possible to get this event even it TRC already invalid)
		/// \param pCallback [in] Pointer of the callback-function.
		/// \returns int handleId of the Callback. It is needed to unregister the callback. (< 0 register was not successfully)
		virtual int registerResetCallback(std::function<void()> pCallback) = 0;

		/// Unregister a Callback function to call if TRC reset.
		/// \param handleId [in] The handleId was get by register of the callback-function.
		virtual void unregisterResetCallback(int handleId) = 0;

		/// Register a Callback function to call if reset is finished and TRC is ready to use.
		/// \param pCallback [in] Pointer of the callback-function.
		/// \returns int handleId of the Callback. It is needed to unregister the callback. (< 0 register was not successfully)
		virtual int registerReadyCallback(std::function<void()> pCallback) = 0;

		/// Unregister a Callback function to call if reset is finished and TRC is ready to use.
		/// \param handleId [in] The handleId was get by register of the callback-function.
		virtual void unregisterReadyCallback(int handleId) = 0;

		/// Register a Callback function to call if a new trigger record is finished.
		/// \param pCallback [in] Pointer of the callback-function. First parameter is inspection and second is trId.
		/// \returns int handleId of the Callback. It is needed to unregister the callback.
		virtual int registerNewTrCallback(std::function<void(TrEventData)> pCallback) = 0;

		/// Unregister a Callback function to call if a new trigger record is finished.
		/// \param handleId [in] The handleId was get by register of the callback-function. (< 0 register was not successfully)
		virtual void unregisterNewTrCallback(int handleId) = 0;

		/// Register a Callback function to call if a trigger record is set to interest.
		/// \param pCallback [in] Pointer of the callback-function. A vector of pairs: First parameter is inspection and second is trId.
		/// \returns int handleId of the Callback. It is needed to unregister the callback.
		virtual int registerNewInterestTrCallback(std::function<void(const std::vector<TrEventData>&)> pCallback) = 0;

		/// Unregister a Callback function to call if a trigger record is set to interest.
		/// \param handleId [in] The handleId was get by register of the callback-function. (< 0 register was not successfully)
		virtual void unregisterNewInterestTrCallback(int handleId) = 0;

		/// Set a list of trigger record to the list of interest. This means those Trs will be keep for longer. (But this method do nothing if TrOfInterest mode is on pause.)
		/// \param trVector [in] Vector of the Trs.
		/// \returns bool True, if set done. If pause it returns false.
		virtual bool setTrsOfInterest(const std::vector<ITriggerRecordRPtr>& trVector) = 0;

		/// Return a list of interestTr for one IP. The size of the list are between 0 and n.
		/// \param inspectionPos [in]
		/// \param n [in] Number of Tr requested.
		/// \returns std::vector<ITriggerRecordRPtr>
		virtual std::vector<ITriggerRecordRPtr> getTrsOfInterest(int inspectionPos, int n) = 0;

		/// Pause the setting of the interest Tr.
		/// \param pauseFlag [in] If true, the setting of interest Tr is paused.
		/// \param inspectionPos [in] Position of the inspection with should be paused. If inspectionPos>127 or inspectionPos<0, all inspection (0..127) will be paused. 
		virtual void pauseTrsOfInterest(bool pauseFlag, int inspectionPos = -1) = 0;

		/// Return if the pause of the interest Tr is set.
		/// \param inspectionPos [in] Position of the inspection with should be paused. If inspectionPos>127 or inspectionPos<0, the state of inspection 0 will be returned. 
		virtual bool isPauseTrsOfInterest(int inspectionPos = 0) const = 0;
	};

	enum class TRC_DataType
	{
		Local,
		Writer,
		Reader
	};
	void createTriggerRecordControllerInstance(TRC_DataType dataType);

	void destroyTriggerRecordController();

	/// Get an instance of the Tr controller in read modus.
	/// \returns ITriggerRecordControllerR&
	ITriggerRecordControllerR& getTriggerRecordControllerRInstance();

	/// Return the position of the inspection. If not available, it return -1.
	/// \param inspectionGuid [in] Guid of the inspection.
	/// \returns int
	int getInspectionPos(const GUID& inspectionGuid);
} // namespace SvTrc
