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
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <vector>

#include "SVTool.h"
#include "ArchiveMethodEnum.h"
#include "ArchiveToolHelper.h"
#include "SVArchiveRecord.h"
#include "SVArchiveRecordsArray.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTo
{

class SVArchiveRecord;

class SVArchiveTool : public SVToolClass
{
public:
	SVArchiveTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVARCHIVETOOL );
	virtual ~SVArchiveTool();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	SVObjectReferenceVector getResultArchiveList();
	void setResultArchiveList(const SVObjectReferenceVector& rObjectRefVector);
	SVObjectReferenceVector getImageArchiveList();
	void setImageArchiveList(const SVObjectReferenceVector& rObjectRefVector);

	bool GetFileArchive( std::string& rName );
	bool GetImageArchivePath( std::string& rName );
	bool SetFileArchive( LPCTSTR lpszName );
	bool SetImageArchivePath( LPCTSTR lpszName );

	static long CalculateImageMemory(SvIe::SVImageClass* pImage );
	static long CalculateImageMemory( std::vector<SvIe::SVImageClass*> imagePtrVector );

	//--IsImagePathUsingKeywords - Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--when archiving images it needs to know if the images has keywords in it
	bool isImagePathUsingKeywords();

	//--GetTranslatedImagePath -Called from SVArchiveRecord::BuildArchiveImageFilePath
	//--to get the already translated path.
	void getTranslatedImagePath(std::string& rImagePath);

#pragma region Methods to replace processMessage
	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo ) override;
	virtual void goingOffline() override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

	//@TODO[MZA][7.50][22.11.2016] The following parameters should be move to private, but now it is needed public because some other classes use it directly
	//
	// The arrays for results and images to archive.
	//
	SVArchiveRecordsArray  m_arrayResultsInfoObjectsToArchive;
	SVArchiveRecordsArray  m_arrayImagesInfoObjectsToArchive;

	SvVol::SVDWordValueObjectClass m_dwAppendArchiveFile;
	SvVol::SVDWordValueObjectClass m_dwArchiveStopAtMaxImages;
	SvVol::SVDWordValueObjectClass m_dwArchiveMaxImagesCount;
	SvVol::SVEnumerateValueObjectClass m_evoArchiveMethod;

	SVArchiveMethodEnum m_eArchiveMethod;
	SvVol::SVBoolValueObjectClass m_bvoUseHeaders;
	SvVol::SVStringValueObjectClass	m_HeaderLabelNames;
	SvVol::SVStringValueObjectClass	m_HeaderObjectGUIDs;

protected:
	SV_DECLARE_CLASS( SVArchiveTool )

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	HRESULT QueueArchiveString( const std::string& rArchiveString );

	//
	// Data elements.
	//
	HRESULT WriteBuffers();


private:
	void initializeArchiveTool();
	bool initializeOnRun(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool AllocateImageBuffers(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool CreateTextArchiveFile(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateImageSpace( bool shouldFullCheck, SvStl::MessageContainerVector *pErrorMessages=nullptr );
	bool ValidateOnRun(SvStl::MessageContainerVector *pErrorMessages=nullptr);


private:

	//
	// A string value to represent all the Guids for the
	// images so they can
	// be stored and retrieved from storage.
	//
	SvVol::SVStringValueObjectClass m_stringArchiveImageGuids_OBSOLETE;
	SvVol::SVStringValueObjectClass m_stringArchiveResultGuids_OBSOLETE;
	SvVol::SVStringValueObjectClass m_svoArchiveImageNames;
	/// This variable holds an array of result names with an Zero based Index (In the svx  files these names have also a zero based index). 
	SvVol::SVStringValueObjectClass m_svoArchiveResultNames;
	
	SvDef::StringVector m_ArchiveStringBuffer;

	//
	// A flag used to indicate a first call to onRun() after a
	// going online message.
	//
	bool m_bInitializedForRun;

	SvVol::SVStringValueObjectClass	m_stringImageFileRootPath;
	SvVol::SVStringValueObjectClass	m_stringFileArchivePath;

	std::string m_ImageTranslatedPath;
	bool m_ArchiveImagePathUsingKW;

	std::ofstream m_fileArchive;       // The file for archived results.
	UINT m_uiValidateCount;

	BufferStructCountMap m_lastBufferMap;
	DWORD m_lastMaxImages = 0;
};

} //namespace SvTo
