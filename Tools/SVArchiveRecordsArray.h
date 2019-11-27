//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVArchiveRecord.h"
#include "Definitions/StringTypeDef.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvVol
{
class SVStringValueObjectClass;
}

struct SVObjectInfoStruct;

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

	HRESULT AllocateBuffers(long bufferNumber, BufferStructCountMap& rBufferMap, int toolPos);

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

private:
#pragma region Private Methods
	std::string AdaptDottedNameToInspectionName(std::string DottedName, std::string NewInspectionName);
#pragma endregion Private Methods

protected:
#pragma region Protected Members
	SVArchiveTool* m_pArchiveTool;
	typedef std::vector <SVArchiveRecord*> RecordsType;
	RecordsType m_vecRecords;
#pragma endregion Protected Members
};	// end class SVArchiveRecordsArray

} //namespace SvTo
