//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveTool
//* .File Name       : $Workfile:   SVArchiveTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>

#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVTool.h"
#include "SVOCore/SVImageObjectClass.h"

#include "ArchiveMethodEnum.h"
#include "ArchiveToolHelper.h"
#include "SVArchiveRecord.h"
#include "SVArchiveRecordsArray.h"

#pragma endregion Includes

class SVImageListClass;
class SVArchiveRecord;

class SVArchiveTool : public SVToolClass
{
public:
	SVArchiveTool( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = nullptr, 
		int StringResourceID = IDS_CLASSNAME_SVARCHIVETOOL
	    );
	virtual ~SVArchiveTool();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual HRESULT ResetObject() override;
	void UpdateTaskObjectOutputList();
	void RebuildResultsArchiveList();
	void AddImageToArray(SVImageClass* pImage);
	void RebuildImageArchiveList();
	void SetImageAttributesFromArchiveList(SVImageListClass* pImageList);

	BOOL CheckForUniqueArchiveFilePath(LPCTSTR pszArchiveFilePathToTest);

	BOOL GetFileArchive( CString& rcsName );
	BOOL GetImageArchivePath( CString& rcsName );
	BOOL SetFileArchive( LPCTSTR lpszName );
	BOOL SetImageArchivePath( LPCTSTR lpszName );

	virtual BOOL Validate() override;
	virtual BOOL OnValidate() override;
	virtual BOOL SetObjectDepth( int NewObjectDepth ) override;
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;

	static long CalculateImageMemory( SVImageClass* p_pImage );
	static long CalculateImageMemory( std::vector<SVImageClass*> p_apImages );

	//--ValidateArvhiveTool - called from the Tool Adjustment Dialog
	//-- This will cause the Archive Tool to run through a full validation cycle
	//--Previously the OnValidate would only run through the initial time or every 10th time
	//--This method guarantees that the OnValidate will run. 
	HRESULT ValidateArchiveTool();
	
	//--IsImagePathUsingKeywords - Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--when archiving images it needs to know if the images has keywords in it
	bool isImagePathUsingKeywords();

	//--GetTranslatedImagePath -Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--to get the already translated path.
	void getTranslatedImagePath(CString &ImagePath);

#pragma region Methods to replace processMessage
	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;
	virtual void goingOffline() override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override;
#pragma endregion Methods to replace processMessage
	//
	// The arrays for results and images to archive.
	//
	SVArchiveRecordsArray  m_arrayResultsInfoObjectsToArchive;
	SVArchiveRecordsArray  m_arrayImagesInfoObjectsToArchive;

	//
	// A string value to represent all the Guids for the
	// images so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass    m_stringArchiveImageGuids_OBSOLETE;
	SVStringValueObjectClass    m_stringArchiveResultGuids_OBSOLETE;
	SVStringValueObjectClass    m_svoArchiveImageNames;
	/// This variable holds an array of result names with an Zero based Index (In the svx  files these names have also a zero based index). 
	SVStringValueObjectClass    m_svoArchiveResultNames;

	SVDWordValueObjectClass     m_dwAppendArchiveFile;
	SVDWordValueObjectClass     m_dwArchiveStopAtMaxImages;
	SVDWordValueObjectClass     m_dwArchiveMaxImagesCount;
	SVEnumerateValueObjectClass m_evoArchiveMethod;

	SVArchiveMethodEnum         m_eArchiveMethod;
	SVBoolValueObjectClass      m_bvoUseHeaders;
	SVStaticStringValueObjectClass m_HeaderLabelNames;
	SVStaticStringValueObjectClass m_HeaderObjectGUIDs;

protected:
	SV_DECLARE_CLASS( SVArchiveTool )

	void PutSelectedObjectGuidsIntoObjectScript(
		CString& RStrScript, CString& RStrAliasTable, int Indent );

    virtual BOOL   onRun( SVRunStatusClass& RRunStatus ) override;

	HRESULT QueueArchiveString( CString strArchiveString );

	//
	// Data elements.
	//
	HRESULT WriteBuffers();
    CFile m_fileArchive;       // The file for archived results.
	UINT m_uiValidateCount;

    //
    // A flag used to indicate a first call to onRun() after a
    // going online message.
    //
    BOOL m_bInitializedForRun;

	std::vector<CString> m_aArchiveStringBuffer;

private:
	void initializeArchiveTool();
    HRESULT initializeOnRun();
	HRESULT AllocateImageBuffers();
	BOOL CreateTextArchiveFile();

	SVStringValueObjectClass	m_stringImageFileRootPath;
	SVStringValueObjectClass	m_stringFileArchivePath;

	//--m_bDriveError will will hold the error state of the drive.
	//--will be true if the hard drive has less than 100 MG of free space
	//--if images are selected for archive.
	bool m_bDriveError;
	CString m_ImageTranslatedPath;
	bool m_ArchiveImagePathUsingKW;
};


