//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "ArchiveMethodEnum.h"
#include "SVTool.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}

namespace SvTo
{

class SVArchiveTool;

typedef std::map<SVMatroxBufferCreateStruct, long > BufferStructCountMap;

class SVArchiveRecord
{
public:
#pragma region Constructor
	SVArchiveRecord();
	virtual ~SVArchiveRecord();
#pragma endregion Constructor
	
#pragma region Public Methods
	void InitArchiveRecord
		(
			SVArchiveTool*    pToolArchive,
			SVObjectReference refObject    // a reference to an object required
		);

	void BuildArchiveImageFilePaths();
	HRESULT	GetNextFileName(std::string& rImageFile);  // For images only
	void BuildFileName();                 // For images only
	void ConnectInputObject();
	void DisconnectInputObject();

	HRESULT AllocateBuffers(long lBufferNumber, BufferStructCountMap& rBufferMap);
	HRESULT QueueImage(SvTrc::IImagePtr& rImage, const std::string& rFileName );
	HRESULT WriteImageQueue();
	HRESULT WriteImage(const SvTrc::ITriggerRecordRPtr pTriggerRecord);
	static HRESULT WriteImage( SVMatroxBuffer& buf, const std::string& rFileName );

	void Init( SVArchiveTool* pArchiveTool );

	SvIe::SVImageClass* GetImage();

	const std::string&         GetImageObjectName() { return m_ImageObjectName; }
	SVObjectReference&      GetObjectReference() { return m_svObjectReference; }
#pragma endregion Public Methods

private:
#pragma region Private Members
	SVObjectReference   m_svObjectReference;
	std::string            m_ImageObjectName;           // images only
	std::string            m_FileNameImage;             // images only
	long                m_lCountImages;                // images only
	std::vector<std::string> m_FileNames;
	std::vector<SvTrc::IImagePtr>	m_ImageStoreVector;
	SVImageInfoClass    m_ImageInfo;                   // images only
	long                m_lLastIndex;
	long                m_lMaxIndex;

	//Attributes
	SVArchiveMethodEnum m_eArchiveMethod;
	SVArchiveTool*      m_pArchiveTool;

	friend class SVArchiveRecordsArray;
#pragma endregion Private Members
};

} //namespace SvTo

