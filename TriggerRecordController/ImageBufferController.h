//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferController.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller of the whole image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\TriggerRecordController.pb.h"
#pragma warning( pop )
#include <mil.h>
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "IImage.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvTRC
{
	class ImageBufferController
	{
#pragma region Constructor
	public:
		static ImageBufferController& getImageBufferControllerInstance();

	private:
		ImageBufferController();
		~ImageBufferController();

		ImageBufferController(const ImageBufferController&) = delete;
		ImageBufferController& operator=(const ImageBufferController&) = delete;

#pragma endregion Constructor

#pragma region Public Methods
	public:
		/// Reset the whole buffers to new structures.
		/// \param trNumbers [in] Numbers of trigger records (this is the multiplier per needed image)
		/// \param imageSizeList [in] New image Size list, entries with no needed image will be deleted.
		/// \param imageList [in] New image list.
		void reset(int trNumbers, const SvPB::ImageSizeList& rImageSizeList, const SvPB::ImageList& rImageList);

		const SvPB::ImageSizeList& getImageSizeList() const { return m_imageSizeList; };
		const SvPB::ImageList& getImageList() const { return m_imageList; };

		/// Increase the reference count onces.
		/// \param pos [in] Buffer position.
		/// \returns bool true if increase successfully. Is false if pos invalid.
		bool IncreaseRefCounter(int pos);

		/// Decrease the reference count onces.
		/// \param pos [in] Buffer position.
		/// \returns bool true if decrease successfully. Is false if pos invalid or reference count already 0.
		bool DecreaseRefCounter(int pos);

		std::shared_ptr<IImage> getImage(int pos) const;

		/// Search an unused buffer in the required size and create a new image handle in write mode.
		/// \param sizeID [in] SizeID of the required buffer.
		/// \param imagePos [out] Set the bufferPosition to parameter.
		/// \returns SvTRC::IImagePtr The image handle in write mode.
		IImagePtr createNewImageHandle(int sizeID, int& rImagePos) const;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvPB::ImageSizeList m_imageSizeList;
		SvPB::ImageList m_imageList;
		int* m_imageRefCountArray = nullptr; //an array of the reference counts.
		int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.
		std::vector<SVMatroxBuffer> m_bufferVector;
#pragma endregion Member Variables
	};
} //namespace SvTRC