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

namespace SvTrc
{
	class ITriggerRecordRW : public ITriggerRecordR
	{
	public:
		virtual ~ITriggerRecordRW() = default;

		/// Set the trigger data to the trigger record.
		/// \param rData [in] The new data.
		virtual void setTriggerData(const TriggerData& rData) = 0;

		/// For all image which have no buffer assign, initialize one. If already one buffer assign, do nothing for this.
		virtual void initImages() = 0;

		/// Set all images from the other trigger record to this record. 
		/// If in the Destination Tr for an image no buffer assigned, the image in source image will be stay or a new buffer assigned
		/// \param rDestTr [in] Destination trigger record.
		virtual void setImages(const ITriggerRecordR& rDestTr) = 0;

		/// Set a buffer to a image.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param imageId [in] Image Id in trigger record.
		/// \param pImage [in] Handle to the buffer which should set.
		virtual void setImage(const GUID& rImageId, const IImagePtr& pImage) = 0;

		/// Set a buffer to a image.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param pos [in] Image position in trigger record.
		/// \param pImage [in] Handle to the buffer which should set.
		virtual void setImage(int pos, const IImagePtr& pImage) = 0;

		/// Set a buffer to a image.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param rImageId [in] Image Id in trigger record.
		/// \param bufferPos [in] Position of the buffer to be set.
		virtual void setImage(const GUID& rImageId, int bufferPos) = 0;

		/// Set a buffer to a image.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param pos [in] Image position in trigger record.
		/// \param bufferPos [in] Position of the buffer to be set.
		virtual void setImage(int pos, int bufferPos) = 0;

		/// Return an pointer to a image instance in write modus. Content of the Buffer is random. Clear or Set the buffer by your own. ATTENTION: Use this instance only as long as this TriggerRecord-instance existing.
		/// \param pos [in] Position in this triggerRecord.
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr createNewImageHandle(int pos) = 0;

		/// Return an pointer to a image instance in write modus. Content of the Buffer is random. Clear or Set the buffer by your own. ATTENTION: Use this instance only as long as this TriggerRecord-instance existing.
		/// \param imageId [in] ID of the image in this triggerRecord.
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr createNewImageHandle(const GUID& imageId) = 0;

		/// Initialize value data
		virtual void initValueData() = 0;

		/// Write inspection value data 
		/// \param ValueObjectList [in] Value object list (Move semantics!)
		virtual void writeValueData(std::vector<_variant_t>&& ValueObjectList) = 0;
	};

	typedef std::shared_ptr< ITriggerRecordRW > ITriggerRecordRWPtr;
} //namespace SvTrc
