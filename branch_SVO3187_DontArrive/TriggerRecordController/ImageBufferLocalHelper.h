//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ImageBufferLocalHelper.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller for the local memory and the image buffers.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvPb
{
class ImageStructData;
}

class SVMatroxBuffer;
#pragma endregion Declarations

namespace SvTrc
{
class ImageBufferLocalHelper final
{
#pragma region Constructor
public:
	explicit ImageBufferLocalHelper(std::vector<SVMatroxBuffer>&);
	~ImageBufferLocalHelper();

private:
	ImageBufferLocalHelper(const ImageBufferLocalHelper&) = delete;
	ImageBufferLocalHelper& operator=(const ImageBufferLocalHelper&) = delete;

#pragma endregion Constructor

#pragma region Public Methods
public:
	/// Free all buffer and set this object empty.
	void clearAll();

	int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos);
	int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//void fillBufferData(SvPb::ImageStructData& rImageStruct, SVMatroxBufferCreateStruct& rBufferStruct, MatroxImageProps& rBufferProps);

#pragma endregion Private Methods

#pragma region Member Variables
private:
	std::vector<SVMatroxBuffer>& m_rBufferVector;
#pragma endregion Member Variables
};
} //namespace SvTrc
