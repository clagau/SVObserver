//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferController.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller of the whole image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#include "DataControllerBase.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

#pragma region Declarations
struct MatroxBufferChildDataStruct;
struct SVMatroxBufferCreateChildStruct;
struct SVMatroxBufferCreateStruct;
struct MatroxImageProps;
class SVMatroxBuffer;

namespace SvOi
{
class ITRCImage;
typedef std::shared_ptr<ITRCImage> ITRCImagePtr;
}
#pragma endregion Declarations

namespace SvTrc
{
void fillBufferData(SvPb::ImageStructData& rImageStruct, SVMatroxBufferCreateStruct& rBufferStruct, MatroxImageProps& rBufferProps);

class ImageBufferController final
{
#pragma region Constructor
public:
	explicit ImageBufferController(DataControllerBase& rDataController);
	~ImageBufferController();

	ImageBufferController(const ImageBufferController&) = delete;
	ImageBufferController& operator=(const ImageBufferController&) = delete;

#pragma endregion Constructor

#pragma region Public Methods
public:
	/// Reset the whole buffers to new structures.
	/// \param rImageStructList [in] New image Size list, entries with no needed image will be deleted.
	/// \param isGlobalInit [in] If true, memory and image will not be changed. If false (default), memory and image will be changed.
	/// \returns std::vector<std::pair<int, int>> Return a vector of pairs of change structID. First is old ID, Second is new ID.
	std::vector<std::pair<int, int>> reset(SvPb::ImageStructList&& rImageStructList, bool isGlobalInit = false);

	const SvPb::ImageStructList& getImageStructList() const { return m_rDataController.getImageStructList(); };

	/// Increase the reference count onces.
	/// \param pos [in] Buffer position.
	/// \returns bool true if increase successfully. Is false if pos invalid.
	bool increaseImageRefCounter(int pos);

	/// Decrease the reference count onces.
	/// \param pos [in] Buffer position.
	/// \returns bool true if decrease successfully. Is false if pos invalid or reference count already 0.
	bool decreaseImageRefCounter(int pos);

	SvOi::ITRCImagePtr getImage(int pos, long resetId, bool shouldUnlockAutomatically) const;

	/// Create the childBuffer and return it. The return pointer must be released before reset the TRC.
	/// \param parentPos [in] Position of the parent in the image-list.
	/// \param rBufferStruct [in] ChildBufferStruct of the child.
	/// \param resetTime [in] Time of the last reset.
	/// \param shouldUnlockAutomatically [in] Set the flag if buffer should automatically unlocked if image-ptr freed.
	/// \returns SvOi::ITRCImagePtr The image handle in read mode.
	SvOi::ITRCImagePtr getChildImage(int parentPos, const MatroxBufferChildDataStruct& rBufferDataStruct, long resetId, bool shouldUnlockAutomatically) const;

	/// Create the childBuffer and return it. The return pointer must be released before reset the TRC.
	/// \param bufferStruct [in] ChildBufferStruct of the child.
	/// \param resetId [in] Time of the last reset.
	/// \param bufferPos [in] Buffer position of the physical buffer (parent).
	/// \param shouldUnlockAutomatically [in] Set the flag if buffer should automatically unlocked if image-ptr freed.
	/// \returns SvOi::ITRCImagePtr The image handle in read mode.
	SvOi::ITRCImagePtr getChildImage(const SVMatroxBufferCreateChildStruct& bufferStruct, long resetId, int bufferPos, bool shouldUnlockAutomatically) const;

	/// Search an unused buffer in the required size and create a new image handle in write mode.
	/// \param structId [in] structure of the required buffer.
	/// \returns SvOi::ITRCImagePtr The image handle in write mode.
	SvOi::ITRCImagePtr createNewImageHandle(const SVMatroxBufferCreateStruct& bufferStruct, long resetId) const;

	/// Search an unused buffer in the required size and create a new image handle in write mode.
	/// \param structId [in] structId of the required buffer.
	/// \param imagePos [out] Set the bufferPosition to parameter.
	/// \param shouldUnlockAutomatically [in] If true, the lock data will be set to the Image-instance and if Image-instance will be destructed it will be unlocked automatically.
	/// \returns SvOi::ITRCImagePtr The image handle in write mode.
	SvOi::ITRCImagePtr createNewImageHandle(int structId, int& rImagePos, long resetId, bool shouldUnlockAutomatically = false) const;

	/// Reduced the required numbers of buffer from the imageList by numbers.
	/// \param imageList [in] The image list.
	/// \param numbers [in] Numbers of buffer to reduce per image.
	void reduceRequiredBuffers(const SvPb::ImageList& imageList, int numbers);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	/// Add the required memory to rNeededBufferSpace and to rStructData (if not already set). It check also if the whole space are enough and throw an exception if it run out of memory.
	/// \param rStructData [in,out] image struct.
	/// \param requiredNumbers [in] Number of buffer of this struct.
	/// \param rNeededBufferSpace [in,out] Get the current space and add the space of the new image buffers.
	/// \returns void
	void addRequiredMemory(SvPb::ImageStructData& rStructData, int requiredNumbers, long long& rNeededBufferSpace);

	/// Update structId to fit to the position in newImageStructList
	/// \param rImageStructList [in,out] Image-struct-List.
	/// \returns std::vector<std::pair<int, int>> Retuen a vector with old/new pairs of the structId.
	std::vector<std::pair<int, int>> updateImageStructListAndGetMoveVec(::google::protobuf::RepeatedPtrField< ::SvPb::ImageStructData >& rImageStructList) const;

	/// Step through source imageSize list to fit buffer to the new required structure and fit also the valid imageSize list. (ATTENTION: Throw exception if error happens)
	/// \param rImageStructListSource [in] The new required list.
	/// \param rImageStructList [in,out] The list which if fit to the current valid memory.
	/// \param rBufVec [in,out] The buffer vector of the mil-Buffer-handles.
	void fitBufferToNewStruct(const SvPb::ImageStructList& rImageStructListSource, SvPb::ImageStructList &rImageStructList, std::vector<SVMatroxBuffer> &rBufVec);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	DataControllerBase& m_rDataController;
#pragma endregion Member Variables
};
} //namespace SvTrc
