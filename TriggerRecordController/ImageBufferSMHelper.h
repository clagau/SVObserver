//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferSMHelper.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller for the shared memory and the image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvPb
{
class ImageStructData;
}

namespace SvSml
{
class SharedDataStore;
typedef  std::unique_ptr<SharedDataStore> DataStorePointer;
}

struct MatroxImageProps;
class SVMatroxBufferCreateStruct;
class SVMatroxBuffer;
#pragma endregion Declarations

namespace SvTrc
{
class ImageBufferSMHelper final
{
#pragma region Constructor
public:
	ImageBufferSMHelper(std::vector<SVMatroxBuffer>& rBufferVector, int maxNumberOfRequiredBuffer);
	~ImageBufferSMHelper();

private:
	ImageBufferSMHelper(const ImageBufferSMHelper&) = delete;
	ImageBufferSMHelper& operator=(const ImageBufferSMHelper&) = delete;

#pragma endregion Constructor

#pragma region Public Methods
public:
	/// Free all buffer and set this object empty.
	void clearAll();

	void removeMemory(std::string memoryName);

	int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos);
	int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void fillBufferData(SvPb::ImageStructData& rImageStruct, SVMatroxBufferCreateStruct& rBufferStruct, MatroxImageProps& rBufferProps);

#pragma endregion Private Methods

#pragma region Member Variables
private:
	std::map<std::string, SvSml::DataStorePointer> m_sharedMemoryMap;
	std::vector<SVMatroxBuffer>& m_rBufferVector;
	const int m_maxNumberOfRequiredBuffer;
#pragma endregion Member Variables
};
} //namespace SvTrc
