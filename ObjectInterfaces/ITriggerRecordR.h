//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ITriggerRecordR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecord in read-modus.
//******************************************************************************
#pragma once
//Moved to precompiled header: #include <memory>
#include "ITRCImage.h"

namespace SvOi
{
	struct TriggerData
	{
		TriggerData() = default;
		explicit TriggerData(int triggerCount) : m_TriggerCount(triggerCount) {};
		int m_TriggerCount {-1};
	};

	class ITriggerRecordR
	{
	public:

		virtual ~ITriggerRecordR() = default;

		/// Return the unique id of this trigger record.
		/// \returns int
		virtual int getId() const = 0;

		virtual int getInspectionPos() const = 0;

		/// Return the trigger data.
		/// \returns const TriggerData&
		virtual const TriggerData& getTriggerData() const = 0;

		/// Return an pointer to an image instance (only in read only modus).
		/// \param pos [in] Position in this triggerRecord.
		/// \param lockImage (default:false) [in] If true image will locked by is own and unlock if ITRCImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
		/// \returns SvOi::ITRCImagePtr
		virtual ITRCImagePtr getImage(int pos, bool lockImage = false) const = 0;

		/// Return an pointer to a child image instance (only in read only modus).
		/// \param childPos [in] Position in this triggerRecord childList.
		/// \param lockImage (default:false) [in] If true the (physical) buffer of this child image will locked by is own and unlock if ITRCImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
		/// \returns SvOi::ITRCImagePtr
		virtual ITRCImagePtr getChildImage(int childPos, bool lockImage = false) const = 0;

		/// Return the data corresponding to the ID
		/// \param pos [in] Position of the value object in definition list.
		/// \returns _variant_t
		virtual _variant_t getDataValue(int pos) const = 0;

		/// Return true if value data has been written to the trigger record
		/// \returns bool
		virtual bool isValueDataValid() const = 0;

		/// Return if this triggerRecord is up-to-date and can be used.
		/// \returns bool
		virtual bool isObjectUpToTime() const = 0;

		/// Return the resetId with this tr is created.
		/// \returns long
		virtual long getResetId() const = 0;
	};

	typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;

	template<typename Container>
	int findObjectIdPos(const Container& rContainer, uint32_t objectId)
	{
		int pos = -1;
		auto imageIter = std::find_if(rContainer.begin(), rContainer.end(), [&objectId](const auto& rData)->bool
		{
			return (rData.objectid() == objectId);
		});
		if (rContainer.end() != imageIter)
		{
			pos = static_cast<int>(std::distance(rContainer.begin(), imageIter));
		}

		return pos;
	}
} //namespace SvOi
