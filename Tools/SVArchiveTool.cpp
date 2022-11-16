//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveTool
//* .File Name       : $Workfile:   SVArchiveTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   17 Dec 2014 11:29:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <io.h>
//Moved to precompiled header: #include <numeric>
#include "SVArchiveTool.h"
#include "ArchiveToolHelper.h"
#include "SVArchiveImageThreadClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "Definitions/SVResetStruct.h"
#include "ObjectInterfaces/IValueObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "FilesystemUtilities/FileHelper.h"
#include "FilesystemUtilities/FileHelperManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "ArchiveDataAsynchron.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr long cAsyncDefaultMaximumImageQueueLength = 5;
#pragma endregion Declarations


SV_IMPLEMENT_CLASS(SVArchiveTool, SvPb::ArchiveToolClassId);


SVArchiveTool::SVArchiveTool(SVObjectClass* POwner, int StringResourceID)
	:SVToolClass(ToolExtType::None, POwner, StringResourceID)
{
	initializeArchiveTool();
}


SVArchiveTool::~SVArchiveTool()
{
	ArchiveDataAsynchron::Instance().GoOffline();
	SVArchiveImageThreadClass::Instance().GoOffline();
	SVMemoryManager::Instance().ReleaseMemory(getObjectId());
	m_ResultCollection.SetArchiveTool(nullptr);
	m_ImageCollection.SetArchiveTool(nullptr);
}

void SVArchiveTool::initializeArchiveTool()
{
	m_currentImagePathRoot = "";

	m_ResultCollection.SetArchiveTool(this);
	m_ImageCollection.SetArchiveTool(this);

	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVToolArchiveObjectType;

	RegisterEmbeddedObject(
		&m_svoArchiveImageNames,
		SvPb::ArchiveImageNamesEId,
		IDS_OBJECTNAME_ARCHIVE_IMAGE_NAMES,
		false, SvOi::SVResetItemTool, true);

	RegisterEmbeddedObject(
		&m_svoArchiveResultNames,
		SvPb::ArchiveResultNamesEId,
		IDS_OBJECTNAME_ARCHIVE_RESULT_NAMES,
		false, SvOi::SVResetItemTool, false);

	RegisterEmbeddedObject(&m_maximumImageQueueLength, SvPb::MaximumImageQueueLengthEId, IDS_OBJECTNAME_MAX_IMAGE_QUEUE_LENGTH, false, SvOi::SVResetItemTool, true);
	m_maximumImageQueueLength.SetDefaultValue(_variant_t(cAsyncDefaultMaximumImageQueueLength), true);

	RegisterEmbeddedObject(&m_currentImageQueueLength, SvPb::CurrentImageQueueLengthEId, IDS_OBJECTNAME_CURRENT_IMAGE_QUEUE_LENGTH, false, SvOi::SVResetItemTool, false);
	m_currentImageQueueLength.SetDefaultValue(_variant_t(0l), true);
	
	UINT cAttribute { SvPb::audittrail};
	m_currentImageQueueLength.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	RegisterEmbeddedObject(&m_resultFilepath, SvPb::ArchiveResultFilepathEId, IDS_OBJECTNAME_ARCHIVE_RESULT_FILEPATH, true, SvOi::SVResetItemTool, false);
	m_resultFilepath.SetDefaultValue(_variant_t(""), true);
	m_resultFilepath.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_resultFolderpathPart1, SvPb::ArchiveResultFolderpathPart1EId, IDS_OBJECTNAME_ARCHIVE_RESULT_FOLDERPATH1, true, SvOi::SVResetItemTool, true);
	m_resultFolderpathPart1.SetDefaultValue(_variant_t(""), true);
	m_resultFolderpathPart1.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_resultFolderpathPart2, SvPb::ArchiveResultFolderpathPart2EId, IDS_OBJECTNAME_ARCHIVE_RESULT_FOLDERPATH2, true, SvOi::SVResetItemTool, true);
	m_resultFolderpathPart2.SetDefaultValue(_variant_t(""), true);
	m_resultFolderpathPart2.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_ResultFilename, SvPb::ArchiveResultFilenameEId, IDS_OBJECTNAME_ARCHIVE_RESULT_FILENAME, true, SvOi::SVResetItemTool, true);
	m_ResultFilename.SetDefaultValue(_variant_t(""), true);
	m_ResultFilename.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_imageFileRootPath1, SvPb::ArchiveImageFileRootPart1EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART1, true, SvOi::SVResetItemTool, true);
	m_imageFileRootPath1.SetDefaultValue(_variant_t(""), true);
	m_imageFileRootPath1.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_imageFileRootPath2, SvPb::ArchiveImageFileRootPart2EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART2, true, SvOi::SVResetItemTool, true);
	m_imageFileRootPath2.SetDefaultValue(_variant_t(""), true);
	m_imageFileRootPath2.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_imageFileRootPath3, SvPb::ArchiveImageFileRootPart3EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART3, true, SvOi::SVResetItemTool, true);
	m_imageFileRootPath3.SetDefaultValue(_variant_t(""), true);
	m_imageFileRootPath3.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(
		&m_dwAppendArchiveFile,
		SvPb::ArchiveAppendArchiveFileEId,
		IDS_OBJECTNAME_ARCHIVE_APPEND_ARCHIVE_FILE,
		false, SvOi::SVResetItemTool, true);
	m_dwAppendArchiveFile.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_bvoFormatResults,
		SvPb::FormatArchiveToolResultsEId,
		IDS_OBJECTNAME_FORMAT_ARCHIVE_TOOLS_RESULTS,
		false, SvOi::SVResetItemTool, true);
	m_bvoFormatResults.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveResultsMinimumNumberOfCharacters,
		SvPb::ArchiveResultsMinimumNumberOfCharactersEId,
		IDS_OBJECTNAME_ARCHIVE_RESULTS_MIN_CHARACTERS,
		false, SvOi::SVResetItemTool, true);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveResultsNumberOfDecimals,
		SvPb::ArchiveResultsNumberOfDecimalsEId,
		IDS_OBJECTNAME_ARCHIVE_RESULTS_NUMBER_OF_DECIMALS,
		false, SvOi::SVResetItemTool, true);
	m_dwArchiveResultsNumberOfDecimals.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveStopAtMaxImages,
		SvPb::ArchiveStopAtMaxImagesEId,
		IDS_OBJECTNAME_ARCHIVE_STOP_AT_MAX_IMAGES,
		false, SvOi::SVResetItemNone, true);
	m_dwArchiveStopAtMaxImages.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_useAlternativeImagePath,
		SvPb::UseAlternativeImagePathsEId,
		IDC_USE_ALTERNATIVE_IMAGE_PATH,
		false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject(
		&m_dwArchiveMaxImagesCount,
		SvPb::ArchiveMaxImagesCountEId,
		IDS_OBJECTNAME_ARCHIVE_MAX_IMAGES_COUNT,
		false, SvOi::SVResetItemNone, true);
	m_dwArchiveMaxImagesCount.SetOutputFormat(SvVol::OutputFormat_int);
	m_dwArchiveMaxImagesCount.SetMinMaxValues(1, 10'000'000);

	RegisterEmbeddedObject(
		&m_evoArchiveMode,
		SvPb::ArchiveMethodEId,
		IDS_OBJECTNAME_ARCHIVE_TOOL_METHOD,
		false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject(
		&m_evoImageFileFormat,
		SvPb::ArchiveImageFileFormatEId,
		IDS_OBJECTNAME_ARCHIVE_TOOL_FORMAT,
		false, SvOi::SVResetItemNone, true);
	

	RegisterEmbeddedObject(
		&m_HeaderLabelNames,
		SvPb::ArchiveHeaderLabelEId,
		IDS_OBJECTNAME_HEADER_LABELS,
		false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject(
		&m_HeaderObjectIDs,
		SvPb::ArchiveHeaderEId,
		IDS_OBJECTNAME_HEADER_OBJECT_STRINGS,
		false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject(
		&m_bvoUseHeaders,
		SvPb::ArchiveUseHeadersEId,
		IDS_OBJECTNAME_ENABLE_HEADERS,
		false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject(&m_baseDirectoryname, SvPb::BaseDirectorynameEId, IDS_BASE_DIRECTORYNAME, true, SvOi::SVResetItemTool, true);
	m_baseDirectoryname.SetDefaultValue(_bstr_t(""), true);
	m_baseDirectoryname.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_DirectorynameIndex, SvPb::DirectorynameIndexEId, IDS_OBJECTNAME_DIRECTORYNAME_INDEX, true, SvOi::SVResetItemTool, true);
	m_DirectorynameIndex.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_SubfolderSelection, SvPb::SubfolderSelectionEId, IDS_OBJECTNAME_SUBFOLDER_SELECTION, true, SvOi::SVResetItemTool, true);
	m_SubfolderSelection.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_SubfolderLocation, SvPb::SubfolderLocationEId, IDS_OBJECTNAME_SUBFOLDER_LOCATION, true, SvOi::SVResetItemTool, true);
	m_SubfolderLocation.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_baseFilename, SvPb::BaseFilenameEId, IDS_BASE_FILENAME, true, SvOi::SVResetItemTool, true);
	m_baseFilename.SetDefaultValue(_bstr_t(""), true);
	m_baseFilename.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_FilenameIndex1, SvPb::FilenameIndex1EId, IDS_OBJECTNAME_FILENAME_INDEX1, true, SvOi::SVResetItemTool, true);
	m_FilenameIndex1.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_centerFilename, SvPb::CenterFilenameEId, IDS_CENTER_FILENAME, true, SvOi::SVResetItemTool, true);
	m_centerFilename.SetDefaultValue(_bstr_t(""), true);
	m_centerFilename.setValueType(SvPb::TypeText);

	RegisterEmbeddedObject(&m_FilenameIndex2, SvPb::FilenameIndex2EId, IDS_OBJECTNAME_FILENAME_INDEX2, true, SvOi::SVResetItemTool, true);
	m_FilenameIndex2.SetDefaultValue(_variant_t(0UL), true);

	// no need to register image buffer

	//
	// Use the user changeable tool name as the results archive name.
	//

	// Set Embedded defaults
	m_svoArchiveImageNames.SetDefaultValue(_T(""), true);
	m_svoArchiveResultNames.SetDefaultValue(_T(""), true);
	m_resultFilepath.SetDefaultValue(_bstr_t(SvUl::Format("D:\\TEMP\\%s__0.txt", GetName()).c_str()), true);
	m_resultFolderpathPart1.SetDefaultValue(_bstr_t(""), true);
	m_resultFolderpathPart2.SetDefaultValue(_bstr_t(""), true);
	m_ResultFilename.SetDefaultValue(_bstr_t(""), true);
	m_imageFileRootPath1.SetDefaultValue(_bstr_t("D:\\TEMP"), true);
	m_imageFileRootPath2.SetDefaultValue(_bstr_t(""), true);
	m_imageFileRootPath3.SetDefaultValue(_bstr_t(""), true);
	m_dwAppendArchiveFile.SetDefaultValue(0, true);
	m_bvoFormatResults.SetDefaultValue(0, true);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetDefaultValue(8, true);
	m_dwArchiveResultsNumberOfDecimals.SetDefaultValue(2, true);
	m_dwArchiveStopAtMaxImages.SetDefaultValue(1, true);
	m_dwArchiveMaxImagesCount.SetDefaultValue(10, true);

	long async = static_cast<long>(ArchiveMode::asynchronous); //must cast to use class enum with SVEnumerateValueObjectClass...
	m_evoArchiveMode.SetDefaultValue(async, true); 

	SvDef::NameValueVector EnumVectorWhen
	{
		{ _T("Synchronous"), static_cast<long>(ArchiveMode::synchronous) }, //... or with NameValueVector
		{ _T("Change Mode"), static_cast<long>(ArchiveMode::goOffline) },
		{ _T("Asynchronous"), static_cast<long>(ArchiveMode::asynchronous) }
	};

	m_evoArchiveMode.SetEnumTypes(EnumVectorWhen);

	long bmp = static_cast<long>(ImageFileFormat::bmp); //must cast to use class enum with SVEnumerateValueObjectClass
	m_evoImageFileFormat.SetDefaultValue(bmp, true);

	SvDef::NameValueVector EnumVectorFormat
	{
		{ _T("BMP"), static_cast<long>(ImageFileFormat::bmp)}, //... or with NameValueVector
		{ _T("PNG"), static_cast<long>(ImageFileFormat::png)},
		{ _T("TIFF"), static_cast<long>(ImageFileFormat::tiff)},
		{ _T("MIM"), static_cast<long>(ImageFileFormat::mim)},
	};

	m_evoImageFileFormat.SetEnumTypes(EnumVectorFormat);

	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::OverwriteAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::OverwriteAttribute);
	m_HeaderObjectIDs.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	m_bInitializedForRun = false;
	m_archiveMode = ArchiveMode::invalid;
	m_uiValidateCount = 0;

}


bool SVArchiveTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if (bOk)
	{
		long Method = static_cast<long>(ArchiveMode::invalid);
		m_evoArchiveMode.GetValue(Method);
		m_archiveMode = static_cast<ArchiveMode> (Method);

		long Format = static_cast<long>(ImageFileFormat::invalid);
		m_evoImageFileFormat.GetValue(Format);
		m_imageFileFormat = static_cast<ImageFileFormat> (Format);
	}

	m_FilenameIndex1.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_FilenameIndex2.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_DirectorynameIndex.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_SubfolderSelection.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_SubfolderLocation.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);

	m_resultFilepath.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_resultFolderpathPart1.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_resultFolderpathPart2.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_ResultFilename.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);

	m_imageFileRootPath1.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_imageFileRootPath2.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_imageFileRootPath3.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_bvoFormatResults.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveResultsNumberOfDecimals.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	m_dwArchiveStopAtMaxImages.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveMaxImagesCount.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_evoArchiveMode.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_evoImageFileFormat.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_HeaderLabelNames.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_HeaderObjectIDs.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_bvoUseHeaders.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	// These values will not be exposed for this tool.
	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	updateResultFilepathInformation(true);

	m_isCreated = bOk;

	return bOk;
}


bool SVArchiveTool::InitializeAndValidate(SvStl::MessageContainerVector* pErrorMessages)
{
	if (m_ImageCollection.GetSize() > 0)
	{
		if (!updateCurrentImagePathRoot(pErrorMessages)) //needed for m_ImageCollection.InitializeObjects()
		{
			return false;
		}
	}

	m_removeEntriesIfDisconnected = false;
	m_ImageCollection.InitializeObjects(m_svoArchiveImageNames);
	m_ImageCollection.ValidateImageObjects();	// makes sure the images are connected as inputs

	m_ResultCollection.InitializeObjects(m_svoArchiveResultNames);
	//the next line is needed to reset the names if objects have changed (e.g. because an array has become a single value).

	refreshResultArchiveList(assembleResultReferenceVector());
	m_removeEntriesIfDisconnected = true;
	m_ResultCollection.ValidateResultsObjects();	// makes sure the results are connected as inputs

	return true;
}


void SVArchiveTool::updateResultFilepathInformation(bool considerOldConfigurations)
{
	if (considerOldConfigurations)
	{
		if (GetResultFilename().empty()) //i.e. we do not currently have all the information on folder and filename (possibly because we have been reading a conficuration from SVO 10.20 or before?)
		{
			std::filesystem::path filepath = GetResultFilepath();
			auto filename = filepath.filename();
			// not checking for a valid filename here to satisfy existing script tests that contain configurations with invalid archive result paths
			auto newFolderpath = filepath.remove_filename();
			m_resultFolderpathPart1.setValue(_bstr_t(newFolderpath.string().c_str()));
			m_ResultFilename.setValue(_bstr_t(filename.string().c_str()));
		}
	}

	m_resultFilepath.SetValue(_bstr_t(GetUntranslatedFullResultFilepath().c_str())); //make sure the full filepath is available
}


bool SVArchiveTool::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool result = SVToolClass::ResetObject(pErrorMessages);
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		m_lastBufferMap.clear();
		updateResultFilepathInformation(false);
	}

	long archiveMode = 0;
	m_evoArchiveMode.GetValue(archiveMode);
	m_archiveMode = static_cast<ArchiveMode>(archiveMode);

	long imageFileFormat = 0;
	m_evoImageFileFormat.GetValue(imageFileFormat);
	m_imageFileFormat = static_cast<ImageFileFormat>(imageFileFormat);

	if (false == InitializeAndValidate(pErrorMessages))
	{
		return false;
	}

	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (pInspection && pInspection->IsResetStateSet(SvDef::SVResetStateArchiveToolCreateFiles))
	{
		m_bInitializedForRun = initializeOnRun(pErrorMessages);

		result = result && m_bInitializedForRun;
	}
	else
	{
		m_ImageCollection.ResetImageCounts();
		m_ImageCollection.ValidateImageObjects();

		result = AllocateImageBuffers(pErrorMessages) && result;
	}

	InitialiseTriggercountObject();

	m_uiValidateCount = 0;

	if (SVMemoryManager::Instance().remainingMemoryInBytes() < 0)
	{
		SvDef::StringVector msgList;
		SvStl::MessageContainer Msg(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_AP_NotEnoughMemoryPleaseDeselect, SvStl::SourceFileParams(StdMessageParams), getObjectId());
		pErrorMessages->push_back(Msg);
		result = false;
	}

	return result;
}


void SVArchiveTool::InitialiseTriggercountObject()
{
	auto pInterface = GetInspectionInterface()->GetToolSetInterface();
	auto pObject = dynamic_cast<SvOi::IObjectClass*>(pInterface);

	SvDef::SVObjectTypeInfoStruct otis;
	otis.m_EmbeddedID = SvPb::TriggerCountEId;
	otis.m_ObjectType = SvPb::SVValueObjectType;
	otis.m_SubType = SvPb::SVLongValueObjectType;

	m_pTriggerCountObject = pObject->getFirstObject(otis);
}

// Create a file to store selected results in text format.
//
bool SVArchiveTool::CreateTextArchiveFile(SvStl::MessageContainerVector* pErrorMessages)
{
	//
	// CFile object.
	//
	std::string FileArchivePath = GetUntranslatedFullResultFilepath();

	if (FileArchivePath.empty())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_PathEmpty, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	ArchiveToolHelper athFile;
	athFile.Init(FileArchivePath);

	if (athFile.isUsingKeywords())
	{
		if (athFile.areTokensValid())
		{
			FileArchivePath = athFile.TranslatePath(FileArchivePath);
		}
	}

	//
	// Make sure the file is closed before creating/opening a new one.
	//
	if (m_fileArchive.is_open())
	{
		m_fileArchive.close();
	}

	if (!FileArchivePath.empty())
	{
		SvFs::FileHelper svFileName(FileArchivePath.c_str());

		if (_access(svFileName.GetPathName().c_str(), 0) != 0)
		{
			if (!SvFs::FileHelperManager::Instance().CreatePath(svFileName.GetPathName().c_str()))
			{
				if (nullptr != pErrorMessages)
				{
					SvDef::StringVector msgList;
					msgList.push_back(FileArchivePath);
					SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
	}

	//
	// Check for append option selected.
	//
	DWORD appendOption;
	m_dwAppendArchiveFile.GetValue(appendOption);

	std::ofstream::openmode fileMode = std::ofstream::in | std::ofstream::out | std::ofstream::binary;
	fileMode |= appendOption ? std::ofstream::app : std::ofstream::trunc;

	m_fileArchive.open(FileArchivePath.c_str(), fileMode);

	if (!m_fileArchive.is_open())
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(FileArchivePath);
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	//
	// Write a date and time stamp to the archive file.
	//
	auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::stringstream stringStream;
	stringStream << std::put_time(std::localtime(&in_time_t), _T("%d/%m/%Y %H:%M:%S"));

	// Format date and time as DD/MM/YYYY HH:MM:SS
	//
	std::string TimeStamp = stringStream.str() + _T("\r\n");

	//
	// Write the result to the archive file.
	//
	assert(m_fileArchive.is_open());
	try
	{
		m_fileArchive.write(TimeStamp.c_str(), TimeStamp.size());

		BOOL bUseHeaders = false;
		HRESULT hr = m_bvoUseHeaders.GetValue(bUseHeaders);
		if (S_OK == hr && bUseHeaders)
		{
			// Write Header
			SvDef::StringVector HeaderVector;
			m_HeaderLabelNames.GetArrayValues(HeaderVector);
			std::string strHeader;
			for (SvDef::StringVector::iterator it = HeaderVector.begin(); it != HeaderVector.end(); ++it)
			{
				strHeader += *it;
				if (it + 1 != HeaderVector.end())
				{
					strHeader += _T(",");
				}
			}
			if (!strHeader.empty())
			{
				strHeader += _T("\r\n");
				m_fileArchive.write(strHeader.c_str(), strHeader.size());
			}
		}
	}
	catch (...)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WriteToFileFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

void local_remove_items(SvDef::StringVector& rVec, SvVol::SVStringValueObjectClass& rvo)
{
	SvDef::StringVector NameVector;
	SvDef::StringVector::iterator iterRemoved;

	if (!rVec.empty())
	{
		rvo.GetArrayValues(NameVector);

		for (iterRemoved = rVec.begin(); iterRemoved != rVec.end(); ++iterRemoved)
		{
			SvDef::StringVector::iterator iterName;
			iterName = std::find(NameVector.begin(), NameVector.end(), *iterRemoved);
			if (iterName != NameVector.end())
			{
				NameVector.erase(iterName);
			}
		}

		rvo.SetArraySize(static_cast<int>(NameVector.size()));
		rvo.SetArrayValues(NameVector);
	}
}


bool SVArchiveTool::initializeOnRun(SvStl::MessageContainerVector* pErrorMessages)
{
	if (m_ImageCollection.GetSize() > 0) //if no images are to be archived we do not need to run through the checking of disk space
	{
		if (false == ValidateImagePathAndAvailableSpace(getObjectId(), pErrorMessages))
		{
			return false;
		}
		if (!ensureArchiveImageDirectoryExists(pErrorMessages))
		{
			return false;
		}
	}

	long Method(static_cast<long>(ArchiveMode::invalid));
	m_evoArchiveMode.GetValue(Method);
	m_archiveMode = static_cast<ArchiveMode> (Method);

	m_ImageCollection.ResetImageCounts();

	//
	// Validate the image objects to be archived.
	//
	m_ImageCollection.ValidateImageObjects();

	ArchiveDataAsynchron::Instance().GoOnline();
	if (m_archiveMode == ArchiveMode::goOffline || m_archiveMode == ArchiveMode::asynchronous)
	{
		if (!AllocateImageBuffers(pErrorMessages))
		{
			return false;
		}

		if (m_archiveMode == ArchiveMode::asynchronous)
		{
			/*HRESULT hrThreadOnline =*/ SVArchiveImageThreadClass::Instance().GoOnline();
			
		}
	}

	// Validate the results objects to be archived in text format.
	int nCountResults = m_ResultCollection.ValidateResultsObjects();

	// Don't create/open the results archive file if no results to archive.
	if (!nCountResults)
	{
		return true;
	}

	// Create a file to store results in text format.
	if (m_fileArchive.is_open())
	{
		return true;
	}
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection && pInspection->IsResetStateSet(SvDef::SVResetStateArchiveToolCreateFiles))	// If going online
	{
		// Create and open the results to text Archive file.
		return CreateTextArchiveFile(pErrorMessages);
	}

	return true;
}

bool SVArchiveTool::AllocateImageBuffers(SvStl::MessageContainerVector* pErrorMessages)
{
	if (SVMemoryManager::Instance().hasOwner(getObjectId()))
	{
		SVMemoryManager::Instance().ReleaseMemory(getObjectId());
	}
	if (m_archiveMode == ArchiveMode::goOffline || m_archiveMode == ArchiveMode::asynchronous)
	{
		DWORD dwMaxImages;
		m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
		if (m_archiveMode == ArchiveMode::asynchronous)
		{
			_variant_t maxImageQueueLength;
			m_maximumImageQueueLength.GetValue(maxImageQueueLength);
			dwMaxImages = std::min(dwMaxImages, static_cast<DWORD>(maxImageQueueLength.ullVal)); //even if the max. queue length is higher, we never will need more than the maximum number of images

		}
		long toolPos = -1;
		m_ToolPosition.GetValue(toolPos);
		BufferStructCountMap bufferMap;
		HRESULT hrAllocate = m_ImageCollection.AllocateBuffers(dwMaxImages, bufferMap, toolPos);

		if (ArchiveMode::asynchronous == m_archiveMode)
		{
			long imageCount = std::accumulate(bufferMap.begin(), bufferMap.end(), 0, [](long sum, std::pair<SVMatroxBufferCreateStruct, long> val) { return sum + val.second; });
			if (imageCount > 0)
			{
				SVArchiveImageThreadClass::Instance().setMaxNumberOfBuffer(toolPos, dwMaxImages * imageCount);
			}
		}

		assert(S_OK == hrAllocate);
		if (S_OK == hrAllocate)
		{
			if (m_lastBufferMap != bufferMap || m_lastMaxImages != dwMaxImages)
			{
				try
				{
					SvOi::ITriggerRecordControllerRW& rTRC = SvOi::getTriggerRecordControllerRWInstanceThrow();
					SvOi::TRC_RAIIPtr pResetRaii;
					if (false == rTRC.isResetStarted())
					{
						pResetRaii = rTRC.startResetTriggerRecordStructure();
					}
					rTRC.removeAllImageBuffer(getObjectId());

					for (const auto& iter : bufferMap)
					{
						long bufferNumber = iter.second * dwMaxImages;
						__int64 l_lImageBufferSize = getBufferSize(iter.first) * bufferNumber;
						hrAllocate = SVMemoryManager::Instance().ReserveMemory(getObjectId(), l_lImageBufferSize);
						if (S_OK != hrAllocate)
						{
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ArchiveTool_NotEnoughBuffer, SvStl::SourceFileParams(StdMessageParams), getObjectId());
								pErrorMessages->push_back(Msg);
							}
							if (nullptr != pResetRaii)
							{
								pResetRaii->free();
							}
							m_lastBufferMap.clear();
							return false;
						}

						rTRC.addImageBuffer(getObjectId(), iter.first, bufferNumber);
					}
					if (nullptr != pResetRaii)
					{
						pResetRaii->free();
					}
					m_lastBufferMap = bufferMap;
					m_lastMaxImages = dwMaxImages;
				}
				catch (const SvStl::MessageContainer& rExp)
				{
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(rExp);
					}
					return false;
				}
			}
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%x"), hrAllocate));
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_AllocImageFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
bool SVArchiveTool::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	
	if (ValidateOnRun(pErrorMessages) && __super::onRun(rRunStatus, pErrorMessages))
	{
		//
		// If this is not a test mode or run mode (online) no work is required.
		// Usually a condition where user is 'moving' a tool that causes the
		// toolset to 'run'.
		//
		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_STOPING))
		{
			return true;
		}

		if (!m_bInitializedForRun)
		{
			rRunStatus.SetFailed();
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ArchiveTool_InitFlagFalse, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		//
		// Iterate the array of objects to archive and build a string.
		//
		std::string Archive = m_ResultCollection.BuildResultsArchiveString();

		if (!Archive.empty())
		{
			Archive += _T("\r\n");
			if (m_fileArchive.is_open())
			{
				ArchiveDataAsynchron::ArchiveData data(std::move(Archive), &m_fileArchive);
				ArchiveDataAsynchron::Instance().QueueData(data);
			}

		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
		TRACE(_T("SVArchiveTool::onRun-WriteArchiveImageFiles-Name=%s\n"), GetCompleteName());
#endif

		m_currentImageQueueLength.SetValue(m_ImageCollection.WriteAllArchiveImages(rRunStatus.m_triggerRecord));

		rRunStatus.SetPassed();

		return true;
	}

	rRunStatus.SetInvalid();
	return false;
}



// Called by SVToolAdjustmentArchivePage::OnInitDialog().
//
SVObjectReferenceVector SVArchiveTool::assembleResultReferenceVector()
{
	SVObjectReferenceVector Result;

	const auto& rRecVec = m_ResultCollection.getRecordVec();
	for (const auto& rRecord : rRecVec)
	{
		const SVObjectReference& rObjectRef = rRecord.GetObjectReference();
		if (rObjectRef.ObjectAttributesAllowed() & SvPb::viewable)
		{
			Result.push_back(rObjectRef);
		}
	}
	return Result;
}


//  Called by SVToolAdjustmentArchivePage::OnOK().
//
void SVArchiveTool::refreshResultArchiveList(const SVObjectReferenceVector& rObjectRefVector)
{
	m_ResultCollection.ClearArray();

	auto size = static_cast<int> (rObjectRefVector.size());
	m_svoArchiveResultNames.SetArraySize(size);

	for (int i = 0; i < size; i++)
	{
		const SVObjectReference& rObjectRef = rObjectRefVector[i];

		m_ResultCollection.emplaceRecordAtBack(rObjectRef);

		m_svoArchiveResultNames.SetValue(rObjectRef.GetCompleteName(true), i);
	}
}

void SVArchiveTool::setImageArchiveList(const SVObjectReferenceVector& rObjectRefVector)
{
	m_ImageCollection.ClearArray();

	auto size = static_cast<int> (rObjectRefVector.size());
	m_svoArchiveImageNames.SetArraySize(size);
	m_svoArchiveImageNames.SetResultSize(size);

	for (int i = 0; i < size; i++)
	{
		const SVObjectReference& rObjectRef = rObjectRefVector[i];

		m_svoArchiveImageNames.SetValue(rObjectRef.GetCompleteName(), i);
	}
}

SVObjectReferenceVector SVArchiveTool::getImageArchiveList()
{
	SVObjectReferenceVector Result;
	const auto& rRecVec = m_ImageCollection.getRecordVec();
	for (const auto& rRecord : rRecVec)
	{
		SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (rRecord.GetObjectReference().getFinalObject());
		if (nullptr != pImage)
		{
			if (SvPb::viewable == (pImage->ObjectAttributesAllowed() & SvPb::viewable))
			{
				Result.emplace_back(rRecord.GetObjectReference());
			}
		}
	}

	return Result;
}

std::vector<std::string> SVArchiveTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 4> cToolAdjustNameList
	{
		_T("Archive Results"),
		_T("Archive Image"),
		_T("Conditional"),
		_T("Comment"),
	};
	return {cToolAdjustNameList.begin(), cToolAdjustNameList.end()};
}

std::string SVArchiveTool::GetUntranslatedFullResultFilepath()
{
	auto filepath = GetResultFolderpathPart1() + GetResultFolderpathPart2() + _T("\\") + GetResultFilename();

	return std::regex_replace(filepath, std::regex(_T("\\\\\\\\")), _T("\\")); //remove double backslashes
}

std::string SVArchiveTool::GetResultFolderpathPart1() const
{
	std::string pathPart1;
	m_resultFolderpathPart1.getValue(pathPart1);
	return pathPart1;
}

std::string SVArchiveTool::GetResultFolderpathPart2() const
{
	std::string pathPart2;
	m_resultFolderpathPart2.getValue(pathPart2);
	return pathPart2;
}

std::string SVArchiveTool::GetResultFilename() const
{
	std::string filename;
	m_ResultFilename.getValue(filename);
	return filename;
}

std::string SVArchiveTool::GetResultFilepath() const
{
	std::string resultFilepath;
	m_resultFilepath.getValue(resultFilepath);
	return resultFilepath;
}

std::string SVArchiveTool::GetImageDirectoryPathPart1() const
{
	std::string pathPart1;
	m_imageFileRootPath1.getValue(pathPart1);
	return pathPart1;
}

std::string SVArchiveTool::GetImageDirectoryPathPart2() const
{
	std::string pathPart2;
	m_imageFileRootPath2.getValue(pathPart2);
	return pathPart2;
}

std::string SVArchiveTool::GetImageDirectoryPathPart3() const
{
	std::string pathPart3;
	m_imageFileRootPath3.getValue(pathPart3);
	return pathPart3;
}

HRESULT SVArchiveTool::WriteBuffers()
{
	HRESULT hr = S_OK;
	// write images
	HRESULT hrImages = m_ImageCollection.WriteImageQueue();
	if (S_OK == hr)
	{
		hr = hrImages;
	}
	return hr;
}


const std::string SVArchiveTool::alternativeImageDirectory(const std::string& rImagePathRoot)
{
	_variant_t temporaryVariant;

	m_DirectorynameIndex.GetValue(temporaryVariant);
	uint32_t DirectorynameIndex = static_cast<uint32_t>(temporaryVariant);

	m_SubfolderSelection.GetValue(temporaryVariant);
	uint32_t SubfolderSelection = static_cast<uint32_t>(temporaryVariant);

	m_SubfolderLocation.GetValue(temporaryVariant);
	uint32_t SubfolderLocation = static_cast<uint32_t>(temporaryVariant);

	m_baseDirectoryname.GetValue(temporaryVariant);
	std::string baseDirectoryname = SvUl::createStdString(temporaryVariant);


	std::string imageDirectoryPath = rImagePathRoot;

	std::string namedDirectoryname = SvUl::Format("%s%06ld", baseDirectoryname.c_str(), DirectorynameIndex);

	auto concatDir = [](const std::string& rA, const std::string& rB) {return rA + "\\" + rB; };

	if (SubfolderSelection != 0)
	{
		std::string categoryDirectoryName = "Warn";
		switch (SubfolderSelection)
		{
			case 1:
				categoryDirectoryName = "Pass";
				break;
			case 2:
				categoryDirectoryName = "Fail";
				break;
		}

		bool namedDirectoryInCategoryDirectory = (SubfolderLocation != 0);

		if (namedDirectoryInCategoryDirectory)
		{
			imageDirectoryPath = concatDir(rImagePathRoot, concatDir(categoryDirectoryName, namedDirectoryname));
		}
		else
		{
			imageDirectoryPath = concatDir(rImagePathRoot, concatDir(namedDirectoryname, categoryDirectoryName));
		}
	}
	else
	{
		imageDirectoryPath = concatDir(rImagePathRoot, namedDirectoryname);
	}

	return imageDirectoryPath;
}


std::string SVArchiveTool::getNextImageFileName()
{
	_variant_t temporaryVariant;
	m_baseFilename.GetValue(temporaryVariant);
	std::string baseFilename = SvUl::createStdString(temporaryVariant);

	double index;
	m_FilenameIndex1.getValue(index);
	uint32_t Index1 = static_cast<uint32_t> (index);

	m_centerFilename.GetValue(temporaryVariant);
	std::string centerFilename = SvUl::createStdString(temporaryVariant);

	m_FilenameIndex2.getValue(index);
	uint32_t Index2 = static_cast<uint32_t> (index);

	return SvUl::Format(_T("%s%08ld%s%04ld%s"), baseFilename.c_str(), Index1, centerFilename.c_str(), Index2, firstImageFileNameExtension(m_imageFileFormat).c_str());
}


long SVArchiveTool::CalculateImageMemory(SvIe::SVImageClass* pImage)
{
	assert(nullptr != pImage);
	long lImageSize = 0;
	if (nullptr != pImage)
	{
		SVImageInfoClass info = pImage->GetImageInfo();
		lImageSize = info.GetBufferSize();
	}
	return lImageSize;
}

long SVArchiveTool::CalculateImageMemory(std::vector<SvIe::SVImageClass*> imagePtrVector)
{
	long lTotalMemory = 0;
	for (size_t i = 0; i < imagePtrVector.size(); i++)
	{
		SvIe::SVImageClass* pImage = imagePtrVector[i];
		lTotalMemory += CalculateImageMemory(pImage);

	}
	return lTotalMemory;
}

std::string SVArchiveTool::getUntranslatedImagePathRoot() const
{
	return GetImageDirectoryPathPart1() + GetImageDirectoryPathPart2() + GetImageDirectoryPathPart3();
}

const std::string SVArchiveTool::getCurrentImagePathRoot() const
{
	return m_currentImagePathRoot;
}

void SVArchiveTool::disconnectObjectInput(uint32_t objectId)
{
	SvDef::StringVector vecRemovedImage = m_ImageCollection.RemoveDisconnectedObject(objectId);
	SvDef::StringVector vecRemovedResult = m_ResultCollection.RemoveDisconnectedObject(objectId);

	if (m_removeEntriesIfDisconnected)
	{
		local_remove_items(vecRemovedImage, m_svoArchiveImageNames);
		local_remove_items(vecRemovedResult, m_svoArchiveResultNames);
	}
}

void SVArchiveTool::goingOffline()
{
	ArchiveDataAsynchron::Instance().GoOffline();
	if (m_archiveMode == ArchiveMode::asynchronous)
	{
		SVArchiveImageThreadClass::Instance().GoOffline();
	}

	if (m_archiveMode == ArchiveMode::goOffline)
	{
		WriteBuffers();
	}

	// Close the text to archive file if necessary
	if (m_fileArchive.is_open())
	{
		m_fileArchive.close();
	}
}

void SVArchiveTool::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	enum class ReplaceType { Search, Full, First };
	std::string newPrefix;
	std::string oldPrefix;

	ReplaceType replaceType {ReplaceType::Search};
	SvPb::SVObjectTypeEnum type = rRenamedObject.GetObjectType();
	if (SvPb::SVInspectionObjectType == type)
	{
		newPrefix = rRenamedObject.GetName();
		oldPrefix = rOldName;
		replaceType = ReplaceType::First;
	}
	else if (SvPb::SVBasicValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVRootObjectType, false);
		newPrefix = dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName();
		oldPrefix = dottedNameWithoutObjectname + _T(".") + rOldName;
		replaceType = ReplaceType::Full;
	}
	else if (SvPb::SVValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVInspectionObjectType, false);
		newPrefix = dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName();
		oldPrefix = dottedNameWithoutObjectname + _T(".") + rOldName;
		replaceType = ReplaceType::Full;
	}
	else
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, false);
		newPrefix = dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T(".");
		oldPrefix = dottedNameWithoutObjectname + _T(".") + rOldName + _T(".");
		replaceType = ReplaceType::Search;
	}


	int iSize = m_svoArchiveResultNames.getResultSize();

	for (int i = 0; i < iSize; i++)
	{
		std::string Name;
		m_svoArchiveResultNames.GetValue(Name, i);

		switch (replaceType)
		{
			case ReplaceType::Search:
				SvUl::searchAndReplace(Name, oldPrefix.c_str(), newPrefix.c_str());
				break;
			case ReplaceType::First:
				if (0 == Name.compare(0, oldPrefix.length(), oldPrefix))
				{
					Name.replace(0, oldPrefix.length(), newPrefix);
				}
				break;
			case ReplaceType::Full:
			{
				std::string indirectTmp = Name;
				size_t pos = indirectTmp.find('[');
				if (std::string::npos != pos)
				{	//if array ("[x]") in the name, remove it for the check
					indirectTmp = indirectTmp.substr(0, pos);
				}
				//only replace the name if it is the full name. Do NOT replace parts of the name, because then it is another object with a similar name.
				if (oldPrefix == indirectTmp)
				{
					SvUl::searchAndReplace(Name, oldPrefix.c_str(), newPrefix.c_str());
				}
			}
			break;
		}
		m_svoArchiveResultNames.SetValue(Name, i);
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}


bool SVArchiveTool::updateCurrentImagePathRoot(SvStl::MessageContainerVector* pErrorMessages, bool displayMessageOnInvalidKeywords)
{
	m_currentImagePathRoot = getUntranslatedImagePathRoot();

	ArchiveToolHelper athImagePath;
	athImagePath.Init(m_currentImagePathRoot);

	if (athImagePath.isUsingKeywords())
	{
		if (athImagePath.areTokensValid())
		{
			m_currentImagePathRoot = athImagePath.TranslatePath(m_currentImagePathRoot);
		}
		else
		{
			if (pErrorMessages || displayMessageOnInvalidKeywords)
			{
				auto type = SvStl::MsgType::Log;
				if (displayMessageOnInvalidKeywords)
				{
					type = type | SvStl::MsgType::Display;
				}
				SvStl::MessageManager msg(type);
				msg.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidKeywordsInPath, SvStl::SourceFileParams(StdMessageParams));
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(msg.getMessageContainer());
				}
			}
			return false;
		}
	}

	return true;
}


std::string SVArchiveTool::archiveImageDirectory()
{
	BOOL useAlternativeImagePaths = FALSE;
	m_useAlternativeImagePath.GetValue(useAlternativeImagePaths);

	std::string imageDirectoryPath = useAlternativeImagePaths ? alternativeImageDirectory(m_currentImagePathRoot) : m_currentImagePathRoot;

	return imageDirectoryPath;
}


bool SVArchiveTool::ensureArchiveImageDirectoryExists(SvStl::MessageContainerVector* pErrorMessages)
{
	bool ok = false;

	BOOL useAlternativeImagePaths = FALSE;
	m_useAlternativeImagePath.GetValue(useAlternativeImagePaths);

	auto imageDirectoryPath = archiveImageDirectory();

	try
	{
		ok = std::filesystem::create_directories(imageDirectoryPath);

		if (!ok)
		{
			if (std::filesystem::is_directory(imageDirectoryPath))
			{
				return true;
			}
		}
	}
	catch (std::exception& e)
	{
		ok = false;
		imageDirectoryPath = e.what(); //use imageDirectoryPath as problem description
	}
	if (!ok)
	{
		SvDef::StringVector msgList;
		msgList.push_back(imageDirectoryPath);
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams));

		if(pErrorMessages)
		{
			pErrorMessages->push_back(Exception.getMessageContainer());
		}
	}

	return ok;
}


bool SVArchiveTool::ValidateImagePathAndAvailableSpace(uint32_t objectId, SvStl::MessageContainerVector* pErrorMessages)
{
	if (!updateCurrentImagePathRoot(pErrorMessages))
	{
		return false;
	}

	if (!ensureArchiveImageDirectoryExists(pErrorMessages))
	{
		return false;
	}

	ULARGE_INTEGER lFreeBytesAvailableToCaller;
	ULARGE_INTEGER lTotalNumberOfBytes;
	ULARGE_INTEGER lTotalNumberOfFreeBytes;

	BOOL bOk = ::GetDiskFreeSpaceEx(m_currentImagePathRoot.c_str(),   // pointer to the directory name (which must end in a slash or backslash and specify an existing directory)
		&lFreeBytesAvailableToCaller, // receives the number of bytes on disk available to the caller
		&lTotalNumberOfBytes,         // receives the number of bytes on disk
		&lTotalNumberOfFreeBytes);    // receives the free bytes on disk

	if (!bOk)
	{
		if (nullptr != pErrorMessages)
		{
			DWORD  ErrorCd = GetLastError();
			if (ErrorCd == ERROR_PATH_NOT_FOUND)
			{ //should not ever get here since the path is validated above
				SvDef::StringVector msgList;
				msgList.push_back(m_currentImagePathRoot);
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams), objectId);
				pErrorMessages->push_back(Msg);
			}
		}
		return false;
	}

	//
	// Make sure we have at least 100 MB free on the drive.
	//
	//For systems with 16GB of memory the amount of memory will be 300Meg
	if (((__int64)100'000'000) > lFreeBytesAvailableToCaller.QuadPart)
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::string(m_currentImagePathRoot));
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Drive_Full, msgList, SvStl::SourceFileParams(StdMessageParams), objectId);
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

bool SVArchiveTool::ValidateOnRun(SvStl::MessageContainerVector* pErrorMessages)	// called each onRun
{
	bool bOk = true;

	if ((0 == m_uiValidateCount % 10))
	{
		if (m_ImageCollection.GetSize() > 0) //If no images are to be archived we do not need to run through the checking of disk space.
		{
			bool setCurrentImagePathRoot = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING);
			if (setCurrentImagePathRoot)
			{
				bOk = ValidateImagePathAndAvailableSpace(getObjectId(), pErrorMessages);
			}
		}
	}
	m_uiValidateCount++;

	return bOk;
}

} //namespace SvTo
