//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferController.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller of the whole image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#include <mil.h>
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "IImage.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvTrc
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
		/// \param rImageStructList [in] New image Size list, entries with no needed image will be deleted.
		/// \returns std::vector<std::pair<int, int>> Return a vector of pairs of change structID. First is old ID, Second is new ID.
		std::vector<std::pair<int, int>> reset(const SvPb::ImageStructList& rImageStructList);

		const SvPb::ImageStructList& getImageStructList() const { return m_imageStructList; };
		
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
		/// \param structId [in] structId of the required buffer.
		/// \param imagePos [out] Set the bufferPosition to parameter.
		/// \returns SvTrc::IImagePtr The image handle in write mode.
		IImagePtr createNewImageHandle(int structId, int& rImagePos) const;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvPb::ImageStructList m_imageStructList;
		long* m_imageRefCountArray = nullptr; //an array of the reference counts.
		int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.
		std::vector<SVMatroxBuffer> m_bufferVector;
#pragma endregion Member Variables
	};
} //namespace SvTrc
