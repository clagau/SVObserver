//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVArchiveRecord.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvVol
{
class SVStringValueObjectClass;
}

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
	void  SetArchiveTool( SVArchiveTool* pTool );
	void ClearArray();
	void ResetImageCounts();

	HRESULT InitializeObjects(SvVol::SVStringValueObjectClass& svoObjects );	// use array capability of string vo

	SvDef::StringVector RemoveDisconnectedObject(uint32_t objectId);
	void ValidateImageObjects();

	HRESULT AllocateBuffers(long bufferNumber, BufferStructCountMap& rBufferMap, int toolPos);

	bool WriteArchiveImageFiles(const SvOi::ITriggerRecordRPtr& pTriggerRecord);
	HRESULT WriteImageQueue();

	int ValidateResultsObjects();
	std::string BuildResultsArchiveString();

	const std::vector <SVArchiveRecord>& getRecordVec() const;
	int GetSize();
	void emplaceRecordAtBack(const SVObjectReference& rObjectRef);
#pragma endregion Public Methods

private:
#pragma region Private Methods
	std::string AdaptDottedNameToInspectionName(std::string DottedName);
#pragma endregion Private Methods

protected:
#pragma region Protected Members
	SVArchiveTool* m_pArchiveTool;
	std::vector <SVArchiveRecord> m_vecRecords;
#pragma endregion Protected Members
};	// end class SVArchiveRecordsArray

} //namespace SvTo
