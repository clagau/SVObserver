//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "ArchiveMethodEnum.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

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
	HRESULT InitializeObjects(SVArchiveTool* pToolArchive, SVStringValueObjectClass& svoObjects );	// use array capability of string vo

	SVStringVector RemoveDisconnectedObject(const SVObjectInfoStruct& p_rInfoObject);
	void ValidateImageObjects();

	HRESULT AllocateBuffers( long lBufferSize );

	bool WriteArchiveImageFiles( );
	HRESULT WriteImageQueue();

	int ValidateResultsObjects();
	SVString BuildResultsArchiveString();
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

