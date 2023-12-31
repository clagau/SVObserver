//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVArchiveRecord.h"
#include "SVArchiveRecordsArray.h"
#include "Definitions/ArchiveMethodEnum.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
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
	SVObjectReferenceVector assembleResultReferenceVector();
	void refreshResultArchiveList(const SVObjectReferenceVector& rObjectRefVector);
	SVObjectReferenceVector getImageArchiveList();
	void setImageArchiveList(const SVObjectReferenceVector& rObjectRefVector);
	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;
	virtual void changeSource(const SVObjectReference& rOldObject, SVObjectClass& rNewObject) override;

	std::string alternativeImageDirectory(const std::string& imagePathRoot);

	static long CalculateImageMemory(SvIe::SVImageClass* pImage );
	static long CalculateImageMemory( std::vector<SvIe::SVImageClass*> imagePtrVector );

	std::string getUntranslatedImagePathRoot() const;
	/// used by SVArchiveRecord::BuildArchiveImageFilePath
	const std::string getCurrentImagePathRoot() const;
	bool updateCurrentImagePathRoot(SvStl::MessageContainerVector* pErrorMessages, bool displayMessageOnInvalidKeywords = false);
	std::string archiveImageDirectory(); //the directory into which the next archive image is to be placed
	bool ensureArchiveImageDirectoryExists(SvStl::MessageContainerVector* pErrorMessages);

	std::string getNextImageFileName();
	virtual bool areImagesNeededInTRC() const override;
	
#pragma region Methods to replace processMessage
	virtual void disconnectObjectInput(uint32_t objectId) override;
	virtual void goingOffline() override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

	//@TODO[MZA][7.50][22.11.2016] The following parameters should be moved to private, but for now they are needed as public because some other classes use them directly
	//
	// The arrays for results and images to archive.
	//
	SVArchiveRecordsArray  m_ResultCollection;
	SVArchiveRecordsArray  m_ImageCollection;

	SvVol::SVDWordValueObjectClass m_bvoFormatResults;

	SvVol::SVDWordValueObjectClass m_dwArchiveResultsMinimumNumberOfCharacters;
	SvVol::SVDWordValueObjectClass m_dwArchiveResultsNumberOfDecimals;

	SvVol::SVDWordValueObjectClass m_dwArchiveStopAtMaxImages;
	SvVol::SVDWordValueObjectClass m_dwArchiveMaxImagesCount;

	SvDef::ArchiveMode m_archiveMode;
	ImageFileFormat m_imageFileFormat;

	SvVol::SVBoolValueObjectClass m_useAlternativeImagePath;

protected:
	SV_DECLARE_CLASS

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual bool useOverlayColorTool() const override { return false; };

	//
	// Data elements.
	//
	HRESULT WriteBuffers();

private:
	void initializeArchiveTool();
	void InitialiseTriggercountObject();
	bool initializeOnRun(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool AllocateImageBuffers(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool CreateTextArchiveFile(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateOnRun(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	bool ValidateImagePathAndAvailableSpace(uint32_t objectId, SvStl::MessageContainerVector* pErrorMessages);///< Checks the directory and the available space for storing image archive files.
	bool InitializeAndValidate(SvStl::MessageContainerVector* pErrorMessages);

	void updateResultFilepathInformation(bool considerOldConfigurations);

	std::string GetUntranslatedFullResultFilepath();

	std::string GetResultFolderpathPart1() const;
	std::string GetResultFolderpathPart2() const;
	std::string GetResultFilename() const;
	std::string GetResultFilepath() const;

	std::string GetImageDirectoryPathPart1() const;
	std::string GetImageDirectoryPathPart2() const;
	std::string GetImageDirectoryPathSeparator() const;
	std::string GetImageDirectoryPathPart3() const;

	//
	// A string value to represent all the ids for the
	// images so they can
	// be stored and retrieved from storage.
	//
	SvVol::SVStringValueObjectClass m_svoArchiveImageNames;
	/// This variable holds an array of result names with an zero based Index (In the svx files these names have also a zero based index). 
	SvVol::SVStringValueObjectClass m_svoArchiveResultNames;

	//
	// A flag used to indicate a first call to onRun() after a
	// going online message.
	//
	bool m_bInitializedForRun;

	SvVol::SVDWordValueObjectClass m_dwAppendArchiveFile;

	SvVol::SVEnumerateValueObjectClass m_evoArchiveMode;
	SvVol::SVEnumerateValueObjectClass m_evoImageFileFormat;

	SvVol::SVBoolValueObjectClass m_bvoUseHeaders;

	SvVol::SVStringValueObjectClass	m_HeaderLabelNames;
	SvVol::SVStringValueObjectClass	m_HeaderObjectIDs;

	SvVol::SVLongValueObjectClass m_currentImageQueueLength; // current _archive_ image queue length
	SvVol::LinkedValue m_maximumImageQueueLength; // maximum _archive_ image queue length

	SvVol::LinkedValue m_resultFolderpathPart1;
	SvVol::LinkedValue m_resultFolderpathPart2;
	SvVol::LinkedValue m_ResultFilename;
	SvVol::LinkedValue m_resultFolderpathPart3;
	SvVol::LinkedValue m_resultFilepath; //@TODO [Arvid][10.40][27.6.2023] should be a SVStringValueObjectClass. But if is, Tid_ObjectBuilder_SetObjectValueError is displayed when loading a existing configuration containing an Archive Tool

	SvVol::LinkedValue m_imageFileRootPath1; 
	SvVol::LinkedValue m_imageFileRootPath2;
	SvVol::LinkedValue m_imageFileRootPath3;

	std::string m_currentImagePathRoot; // needed by SVArchiveRecord

	std::ofstream m_fileArchive;       // The file for archived results.
	UINT m_uiValidateCount;

	BufferStructCountMap m_lastBufferMap;
	DWORD m_lastMaxImages = 0;

	SvOi::IObjectClass* m_pTriggerCountObject = nullptr; //used when the trigger count is incorporated into image filenames

	SvVol::LinkedValue m_FilenameIndex1;
	SvVol::LinkedValue m_FilenameIndex2;
	SvVol::LinkedValue m_DirectorynameIndex;
	SvVol::LinkedValue m_SubfolderSelection;
	SvVol::LinkedValue m_SubfolderLocation;

	SvVol::LinkedValue m_baseDirectoryname;
	SvVol::LinkedValue m_baseFilename;
	SvVol::LinkedValue m_centerFilename;

	SvVol::SVStringValueObjectClass	m_ImageFilepathrootSeparator;

	bool m_removeEntriesIfDisconnected = true;
	long m_imageCount = 0;
};

} //namespace SvTo
