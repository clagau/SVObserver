//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "ArchiveMethodEnum.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "ObjectInterfaces/ITRCImage.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}
namespace SvOi
{
class ITriggerRecordR;
}
class SVMatroxBuffer;

namespace SvTo
{

class SVArchiveTool;

typedef std::map<SVMatroxBufferCreateStruct, long > BufferStructCountMap;

class SVArchiveRecord
{
public:
#pragma region Constructor
//default 
	SVArchiveRecord() = default;
#pragma endregion Constructor
	
#pragma region Public Methods
	void InitArchiveRecord(SVArchiveTool* pArchiveTool, SVObjectReference refObject);

	void BuildDefaultImageFilePaths(); ///> assemble the filepaths that will be used _unless_ the "Alternative Image Paths" is checked
	HRESULT	GetNextImageFilePath(std::string& rImageFile);


	HRESULT  GetNextAlternativeImageFilePath(std::string&  ImageFilePath, std::string&  imageDirectoryPath);
	void BuildImageFileName();            // For images only

	HRESULT AllocateBuffers(long lBufferNumber, BufferStructCountMap& rBufferMap, int toolPos);
	long QueueImage(SvOi::ITRCImagePtr& rImage, const std::string& rFileName, const std::string& rImageDirectoryPath); // returns current queue length (-1 on error)
	
	HRESULT WriteImageQueue();
	long WriteArchiveImage(const SvOi::ITriggerRecordR* pTriggerRecord); // returns current queue length (-1 on error)
	static HRESULT WriteImageToFile( const SVMatroxBuffer& buf, const std::string& rFileName );

	void SetArchiveTool( SVArchiveTool* pArchiveTool );

	SvIe::SVImageClass* GetImage();

	const std::string&  GetImageObjectName() const { return m_ImageObjectName; }
	SVObjectReference&  GetObjectReference() { return m_svObjectReference; }
	const SVObjectReference&      GetObjectReference() const { return m_svObjectReference; }
#pragma endregion Public Methods

private:
	bool CountImages();
#pragma region Private Members
	SVObjectReference   m_svObjectReference;
	std::string         m_ImageObjectName;				// images only
	std::string         m_ImageFileName;				// images only
	long                m_lCountImages {0L};			// images only
	std::vector<std::string> m_FileNames;
	std::vector<SvOi::ITRCImagePtr>	m_ImageStoreVector;
	SVImageInfoClass    m_ImageInfo;					// images only
	long                m_lLastIndex {0L};
	long                m_lMaxIndex {0L};
	std::string			m_ImagePathRoot;
	std::string			m_formatString = _T("");

	//Attributes
	SVArchiveMethodEnum m_eArchiveMethod {SVArchiveInvalidMethod};
	SVArchiveTool*      m_pArchiveTool {nullptr};
	int m_toolPos = -1;

	friend class SVArchiveRecordsArray;
#pragma endregion Private Members
};

} //namespace SvTo

