//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferController.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller of the whole image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ImageBufferSMHelper.h"
#include "ImageBufferLocalHelper.h"

#pragma warning( disable: 4244 )	//Disable warning for prototype conversion
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

#pragma region Declarations
struct MatroxBufferChildDataStruct;
struct MatroxImageProps;
class SVMatroxBufferCreateStruct;
class SVMatroxBufferCreateChildStruct;
class SVMatroxBuffer;
class ImageBufferSMHelper;

namespace SvTrc
{
class IImage;
typedef std::shared_ptr<IImage> IImagePtr;
}
#pragma endregion Declarations

namespace SvTrc
{
class ImageBufferController final
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
	/// Free all buffer and set this object empty.
	void clearAll();

	/// Reset the whole buffers to new structures.
	/// \param rImageStructList [in] New image Size list, entries with no needed image will be deleted.
	/// \returns std::vector<std::pair<int, int>> Return a vector of pairs of change structID. First is old ID, Second is new ID.
	std::vector<std::pair<int, int>> reset(const SvPb::ImageStructList& rImageStructList);

	const SvPb::ImageStructList& getImageStructList() const { return m_imageStructList; };

	/// Increase the reference count onces.
	/// \param pos [in] Buffer position.
	/// \returns bool true if increase successfully. Is false if pos invalid.
	bool increaseRefCounter(int pos);

	/// Decrease the reference count onces.
	/// \param pos [in] Buffer position.
	/// \returns bool true if decrease successfully. Is false if pos invalid or reference count already 0.
	bool decreaseRefCounter(int pos);

	IImagePtr getImage(int pos, long resetId, bool shouldUnlockAutomatically) const;

	/// Create the childBuffer and return it. The return pointer must be released before reset the TRC.
	/// \param parentPos [in] Position of the parent in the image-list.
	/// \param rBufferStruct [in] ChildBufferStruct of the child.
	/// \param resetTime [in] Time of the last reset.
	/// \param shouldUnlockAutomatically [in] Set the flag if buffer should automatically unlocked if image-ptr freed.
	/// \returns IImagePtr The image handle in read mode.
	IImagePtr getChildImage(int parentPos, const MatroxBufferChildDataStruct& rBufferDataStruct, long resetId, bool shouldUnlockAutomatically) const;

	/// Create the childBuffer and return it. The return pointer must be released before reset the TRC.
	/// \param bufferStruct [in] ChildBufferStruct of the child.
	/// \param resetId [in] Time of the last reset.
	/// \param bufferPos [in] Buffer position of the physical buffer (parent).
	/// \param shouldUnlockAutomatically [in] Set the flag if buffer should automatically unlocked if image-ptr freed.
	/// \returns SvTrc::IImagePtr The image handle in read mode.
	IImagePtr getChildImage(const SVMatroxBufferCreateChildStruct& bufferStruct, long resetId, int bufferPos, bool shouldUnlockAutomatically) const;

	/// Search an unused buffer in the required size and create a new image handle in write mode.
	/// \param structId [in] structure of the required buffer.
	/// \returns SvTRC::IImagePtr The image handle in write mode.
	IImagePtr createNewImageHandle(const SVMatroxBufferCreateStruct& bufferStruct, long resetId) const;

	/// Search an unused buffer in the required size and create a new image handle in write mode.
	/// \param structId [in] structId of the required buffer.
	/// \param imagePos [out] Set the bufferPosition to parameter.
	/// \param shouldUnlockAutomatically [in] If true, the lock data will be set to the Image-instance and if Image-instance will be destructed it will be unlocked automatically.
	/// \returns SvTrc::IImagePtr The image handle in write mode.
	IImagePtr createNewImageHandle(int structId, int& rImagePos, long resetId, bool shouldUnlockAutomatically = false) const;

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
#pragma endregion Private Methods

#pragma region Member Variables
private:
	int m_maxNumberOfRequiredBuffer = 9000;
	SvPb::ImageStructList m_imageStructList;
	long* m_imageRefCountArray = nullptr; //an array of the reference counts.
	int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.
	std::vector<SVMatroxBuffer> m_bufferVector;

	//This is the memoryHelper. To use SharedMemory use here ImageBufferSMHelper, for local memory ImageBufferLocalHelper
	//ImageBufferSMHelper m_memoryHelper;
	ImageBufferLocalHelper m_memoryHelper;
#pragma endregion Member Variables
};
} //namespace SvTrc
