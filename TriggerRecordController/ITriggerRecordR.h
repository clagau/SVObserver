//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecord in read-modus.
//******************************************************************************
#pragma once
//Moved to precompiled header: #include <memory>
#include "IImage.h"

namespace SvTrc
{
	struct TriggerData
	{
		explicit TriggerData(int triggerCount = 0) : m_TriggerCount(triggerCount) {};
		int m_TriggerCount {0};
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
		/// \param imageId [in] GUID of the image.
		/// \param lockImage (default:false) [in] If true image will locked by is own and unlock if IImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr getImage(const GUID& imageId, bool lockImage = false) const = 0;

		/// Return an pointer to an image instance (only in read only modus).
		/// \param pos [in] Position in this triggerRecord.
		/// \param lockImage (default:false) [in] If true image will locked by is own and unlock if IImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr getImage(int pos, bool lockImage = false) const = 0;

		/// Return an pointer to a child image instance (only in read only modus).
		/// \param childPos [in] Position in this triggerRecord childList.
		/// \param lockImage (default:false) [in] If true the (physical) buffer of this child image will locked by is own and unlock if IImagePtr freed. If false image will not separated lock, but by the TriggerRecord (ATTENTION: In this case, use this instance only as long as this TriggerRecord-instance existing.)
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr getChildImage(int childPos, bool lockImage = false) const = 0;

		/// Return the data corresponding to the GUID
		/// \param dataId [in] GUID of the value object.
		/// \returns _variant_t
		virtual _variant_t getDataValue(const GUID& dataId) const = 0;
		virtual _variant_t getDataValue(int pos) const = 0;

		/// Return true if value data has been written to the trigger record
		/// \returns bool
		virtual bool isValueDataValid() const = 0;

		/// Return if this triggerRecord is up-to-date and can be used.
		/// \returns bool
		virtual bool isObjectUpToTime() const = 0;
	};

	typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
} //namespace SvTrc
