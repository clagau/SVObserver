//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordRW.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecord in read/write-modus.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "ITriggerRecordR.h"
#pragma endregion Includes

namespace SvTRC
{
	class ITriggerRecordRW : public ITriggerRecordR
	{
	public:
		/// Set the trigger data to the trigger record.
		/// \param rData [in] The new data.
		virtual void setTriggerData(const TriggerData& rData) = 0;

		/// Set all images from the other trigger record to this record.
		/// \param rDestTR [in] Destination trigger record.
		virtual void setImages(const ITriggerRecordR& rDestTR) = 0;

		/// Set a image to a buffer.
		/// \param pos [in] Image position in trigger record.
		/// \param bufferPos [in] Position of the buffer to be set.
		virtual void setImage(int pos, int bufferPos) = 0;

		/// Return an pointer to a image instance in write modus. Content of the Buffer is random. Clear or Set the buffer by your own. ATTENTION: Use this instance only as long as this TriggerRecord-instance existing.
		/// \param pos [in] Position in this triggerRecord.
		/// \returns SvTRC::IImagePtr
		virtual IImagePtr createNewImageHandle(int pos) = 0;
	};

	typedef std::shared_ptr< ITriggerRecordRW > ITriggerRecordRWPtr;
} //namespace SvTRC