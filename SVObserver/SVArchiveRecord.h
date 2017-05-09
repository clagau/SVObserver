//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "ArchiveMethodEnum.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVOCore/SVTool.h"
#include "SVOCore/SVImageObjectClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes


class SVArchiveTool;
class SVImageClass;

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

	HRESULT	BuildArchiveImageFilePath(SVString& rImageFile);  // For images only
	void BuildFileName();                 // For images only
	void ConnectInputObject();
	void DisconnectInputObject();

	HRESULT AllocateBuffers( long lBufferSize );
	HRESULT QueueImage( SVMatroxBuffer& buf, const SVString& rFileName );
	HRESULT WriteImageQueue();
	HRESULT WriteImage( );
	static HRESULT WriteImage( SVMatroxBuffer& buf, const SVString& rFileName );

	void Init( SVArchiveTool* pArchiveTool );

	SVImageClass* GetImage();

	long                    GetImageMemorySize() { return m_lImageSize; }
	const SVString&         GetImageObjectName() { return m_ImageObjectName; }
	SVObjectReference&      GetObjectReference() { return m_svObjectReference; }
#pragma endregion Public Methods

private:
#pragma region Private Members
	typedef std::pair< SVString, SVDataManagerHandle > SVFileNameIndexHandlePair;
	typedef std::deque< SVFileNameIndexHandlePair > SVFileNameIndexHandleDeque;

	SVObjectReference   m_svObjectReference;
	SVString            m_ImageObjectName;           // images only
	SVString            m_FileNameImage;             // images only
	long                m_lCountImages;                // images only
	SVImageObjectClassPtr  m_ImageBufferPtr;                 // images only
	SVImageInfoClass    m_ImageInfo;                   // images only
	long                m_lLastIndex;
	long                m_lMaxIndex;
	SVFileNameIndexHandleDeque m_aFileNames;
	long                m_lImageSize;
	long                m_lDMBuffer;


	//Attributes
	SVArchiveMethodEnum m_eArchiveMethod;
	SVArchiveTool*      m_pArchiveTool;

	SVDataManagerHandle m_LastIndexHandle;
	SVSmartIndexArrayHandlePtr m_pImageCircleBuffer;

	friend class SVArchiveRecordsArray;
#pragma endregion Private Members
};
