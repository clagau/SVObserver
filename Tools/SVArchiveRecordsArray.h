//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "ArchiveMethodEnum.h"
#include "SVArchiveRecord.h"
#include "Definitions/StringTypeDef.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvTo
{

class SVArchiveTool;
class SVArchiveRecord;

class SVArchiveRecordsArray
{
public:
#pragma region Constructor
	SVArchiveRecordsArray();
	virtual ~SVArchiveRecordsArray();
#pragma endregion Constructor

#pragma region Public Methods
	void SetArchiveTool( SVArchiveTool* pTool );
	void ClearArray();
	void ResetImageCounts();

	void ConvertStringToGuids( SVArchiveTool* pToolArchive, LPCTSTR pszStrValue );	// FOR BACKWARD COMPATIBILITY
	HRESULT InitializeObjects(SVArchiveTool* pToolArchive, SvVol::SVStringValueObjectClass& svoObjects );	// use array capability of string vo

	SvDef::StringVector RemoveDisconnectedObject(const SVObjectInfoStruct& p_rInfoObject);
	void ValidateImageObjects();

	HRESULT AllocateBuffers(long bufferNumber, BufferStructCountMap& rBufferMap);

	bool WriteArchiveImageFiles(const SvTrc::ITriggerRecordRPtr& pTriggerRecord);
	HRESULT WriteImageQueue();

	int ValidateResultsObjects();
	void BuildArchiveImageFilePaths();
	std::string BuildResultsArchiveString();
	void DisconnectAllResultObjects();

	int GetSize();
	SVArchiveRecord* GetAt(int i);
	int Add( SVArchiveRecord* pRecord );
#pragma endregion Public Methods

protected:
#pragma region Protected Members
	SVArchiveTool* m_pArchiveTool;
	typedef std::vector <SVArchiveRecord*> RecordsType;
	RecordsType m_vecRecords;
#pragma endregion Protected Members
};	// end class SVArchiveRecordsArray

} //namespace SvTo
