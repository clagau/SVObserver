//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "Definitions/TextDefineSvDef.h"
#include "ObjectInterfaces/IValueObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVLibrary/SVOINIClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int cAsyncDefaultBufferNumber = 5;
#pragma endregion Declarations


SV_IMPLEMENT_CLASS(SVArchiveTool, SvPb::ArchiveToolClassId);



SVArchiveTool::SVArchiveTool( SVObjectClass* POwner, int StringResourceID )
              : SVToolClass(false,POwner, StringResourceID)
{
	initializeArchiveTool();
}


SVArchiveTool::~SVArchiveTool()
{
	TheSVArchiveImageThreadClass().GoOffline();
	TheSVMemoryManager().ReleasePoolMemory(SvDef::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this);
	TheSVMemoryManager().ReleasePoolMemory(SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this);
	m_ResultCollection.SetArchiveTool(nullptr);
	m_ImageCollection.SetArchiveTool(nullptr);
}

void SVArchiveTool::initializeArchiveTool()
{
	m_currentImagePathRoot = "";

	m_ResultCollection.SetArchiveTool( this );
	m_ImageCollection.SetArchiveTool( this );

	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVToolArchiveObjectType;

	
	
	// Register Embedded Objects
	RegisterEmbeddedObject(
		&m_stringFileArchivePath,
		SvPb::ArchiveFilePathEId,
		IDS_OBJECTNAME_ARCHIVE_FILEPATH,
		false, SvOi::SVResetItemTool);

	RegisterEmbeddedObject(
		&m_svoArchiveImageNames,
		SvPb::ArchiveImageNamesEId,
		IDS_OBJECTNAME_ARCHIVE_IMAGE_NAMES,
		false, SvOi::SVResetItemTool);

	RegisterEmbeddedObject(
		&m_svoArchiveResultNames,
		SvPb::ArchiveResultNamesEId,
		IDS_OBJECTNAME_ARCHIVE_RESULT_NAMES,
		false, SvOi::SVResetItemTool);

	RegisterEmbeddedObject(&m_imageFileRootPath1, SvPb::ArchiveImageFileRootPart1EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART1, true, SvOi::SVResetItemTool);
	m_imageFileRootPath1.SetDefaultValue(_variant_t(""), true);

	RegisterEmbeddedObject(&m_imageFileRootPath2, SvPb::ArchiveImageFileRootPart2EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART2, true, SvOi::SVResetItemTool);
	m_imageFileRootPath2.SetDefaultValue(_variant_t(""), true);

	RegisterEmbeddedObject(&m_imageFileRootPath3, SvPb::ArchiveImageFileRootPart3EId, IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PART3, true, SvOi::SVResetItemTool);
	m_imageFileRootPath3.SetDefaultValue(_variant_t(""), true);

	RegisterEmbeddedObject(
		&m_dwAppendArchiveFile,
		SvPb::ArchiveAppendArchiveFileEId,
		IDS_OBJECTNAME_ARCHIVE_APPEND_ARCHIVE_FILE,
		false, SvOi::SVResetItemTool);
	m_dwAppendArchiveFile.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_bvoFormatResults,
		SvPb::FormatArchiveToolResultsEId,
		IDS_OBJECTNAME_FORMAT_ARCHIVE_TOOLS_RESULTS,
		false, SvOi::SVResetItemTool);
	m_bvoFormatResults.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveResultsMinimumNumberOfCharacters,
		SvPb::ArchiveResultsMinimumNumberOfCharactersEId,
		IDS_OBJECTNAME_ARCHIVE_RESULTS_MIN_CHARACTERS,
		false, SvOi::SVResetItemTool);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveResultsNumberOfDecimals,
		SvPb::ArchiveResultsNumberOfDecimalsEId,
		IDS_OBJECTNAME_ARCHIVE_RESULTS_NUMBER_OF_DECIMALS,
		false, SvOi::SVResetItemTool);
	m_dwArchiveResultsNumberOfDecimals.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_dwArchiveStopAtMaxImages,
		SvPb::ArchiveStopAtMaxImagesEId,
		IDS_OBJECTNAME_ARCHIVE_STOP_AT_MAX_IMAGES,
		false, SvOi::SVResetItemNone);
	m_dwArchiveStopAtMaxImages.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_useAlternativeImagePaths,
		SvPb::UseAlternativeImagePathsEId,
		IDC_USE_ALTERNATIVE_IMAGE_PATHS,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_dwArchiveMaxImagesCount,
		SvPb::ArchiveMaxImagesCountEId,
		IDS_OBJECTNAME_ARCHIVE_MAX_IMAGES_COUNT,
		false, SvOi::SVResetItemNone);
	m_dwArchiveMaxImagesCount.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(
		&m_evoArchiveMethod,
		SvPb::ArchiveMethodEId,
		IDS_OBJECTNAME_ARCHIVE_TOOL_METHOD,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_HeaderLabelNames,
		SvPb::ArchiveHeaderLabelEId,
		IDS_OBJECTNAME_HEADER_LABELS,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_HeaderObjectIDs,
		SvPb::ArchiveHeaderEId,
		IDS_OBJECTNAME_HEADER_OBJECT_STRINGS,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_bvoUseHeaders,
		SvPb::ArchiveUseHeadersEId,
		IDS_OBJECTNAME_ENABLE_HEADERS,
		false, SvOi::SVResetItemNone);

	// Register Embedded Objects
	RegisterEmbeddedObject(
		&m_baseFilename,
		SvPb::BaseFilenameEId,
		IDS_BASE_FILENAME,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_centerFilename,
		SvPb::CenterFilenameEId,
		IDS_CENTER_FILENAME,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_baseDirectoryname,
		SvPb::BaseDirectorynameEId,
		IDS_BASE_DIRECTORYNAME,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_FilenameIndex1, SvPb::FilenameIndex1EId, IDS_OBJECTNAME_FILENAME_INDEX1, true, SvOi::SVResetItemTool);
	m_FilenameIndex1.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_FilenameIndex2, SvPb::FilenameIndex2EId, IDS_OBJECTNAME_FILENAME_INDEX2, true, SvOi::SVResetItemTool);
	m_FilenameIndex2.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_DirectorynameIndex, SvPb::DirectorynameIndexEId, IDS_OBJECTNAME_DIRECTORYNAME_INDEX, true, SvOi::SVResetItemTool);
	m_DirectorynameIndex.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_SubfolderSelection, SvPb::SubfolderSelectionEId, IDS_OBJECTNAME_SUBFOLDER_SELECTION, true, SvOi::SVResetItemTool);
	m_SubfolderSelection.SetDefaultValue(_variant_t(0UL), true);

	RegisterEmbeddedObject(&m_SubfolderLocation, SvPb::SubfolderLocationEId, IDS_OBJECTNAME_SUBFOLDER_LOCATION, true, SvOi::SVResetItemTool);
	m_SubfolderLocation.SetDefaultValue(_variant_t(0UL), true);

	// no need to register image buffer

	//
	// Use the user changeable tool name as the results archive name.
	//
	SVFileNameClass FileClass;
	FileClass.SetExtension(_T(".txt"));
	FileClass.SetPathName(_T("D:\\TEMP"));

	std::string FileName = SvUl::Format(_T("%s__0"), GetName());
	FileClass.SetFileNameOnly(FileName.c_str());

	// Set Embedded defaults

	m_stringFileArchivePath.SetDefaultValue(FileClass.GetFullFileName(), true);
	m_svoArchiveImageNames.SetDefaultValue(_T(""), true);
	m_svoArchiveResultNames.SetDefaultValue(_T(""), true);
	m_imageFileRootPath1.SetDefaultValue(_bstr_t("D:\\TEMP"), true);
	m_imageFileRootPath2.SetDefaultValue(_bstr_t(""), true);
	m_imageFileRootPath3.SetDefaultValue(_bstr_t(""), true);
	m_dwAppendArchiveFile.SetDefaultValue(0, true);
	m_bvoFormatResults.SetDefaultValue(0, true);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetDefaultValue(8, true);
	m_dwArchiveResultsNumberOfDecimals.SetDefaultValue(2, true);
	m_dwArchiveStopAtMaxImages.SetDefaultValue(1, true);
	m_dwArchiveMaxImagesCount.SetDefaultValue(10, true);

	m_evoArchiveMethod.SetDefaultValue(SVArchiveAsynchronous, true);

	SvOi::NameValueVector EnumVector
	{
		{ _T("Synchronous"), SVArchiveSynchronous },
		{ _T("Change Mode"), SVArchiveGoOffline },
		{ _T("Asynchronous"), SVArchiveAsynchronous }
	};
	m_evoArchiveMethod.SetEnumTypes(EnumVector);

	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::OverwriteAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::OverwriteAttribute);
	m_HeaderObjectIDs.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	m_bInitializedForRun = false;
	m_eArchiveMethod = SVArchiveInvalidMethod;
	m_uiValidateCount = 0;

}


bool SVArchiveTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if ( bOk )
	{
		long Method = SVArchiveInvalidMethod;
		m_evoArchiveMethod.GetValue( Method );
		m_eArchiveMethod = static_cast<SVArchiveMethodEnum> (Method);
	}

	m_FilenameIndex1.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_FilenameIndex2.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_DirectorynameIndex.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_SubfolderSelection.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_SubfolderLocation.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);

	m_stringFileArchivePath.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable & ~SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute);
	m_stringFileArchivePath.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_svoArchiveImageNames.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_svoArchiveResultNames.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_imageFileRootPath1.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_imageFileRootPath2.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_imageFileRootPath3.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_dwAppendArchiveFile.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_bvoFormatResults.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveResultsMinimumNumberOfCharacters.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveResultsNumberOfDecimals.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	m_dwArchiveStopAtMaxImages.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_dwArchiveMaxImagesCount.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_evoArchiveMethod.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_HeaderLabelNames.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_HeaderObjectIDs.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_bvoUseHeaders.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	// These values will not be exposed for this tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool SVArchiveTool::InitializeAndValidate()
{
	if (m_ImageCollection.GetSize() > 0)
	{
		if (!updateCurrentImagePathRoot()) //needed for m_ImageCollection.InitializeObjects()
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


bool SVArchiveTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool result = SVToolClass::ResetObject(pErrorMessages);
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		m_lastBufferMap.clear();
	}
	
	long l_lArchiveMethod = 0;
	m_evoArchiveMethod.GetValue( l_lArchiveMethod );
	m_eArchiveMethod = static_cast<SVArchiveMethodEnum>( l_lArchiveMethod );

	// Put the archive tool text in the thread affinity list.
	if(m_eArchiveMethod == SVArchiveAsynchronous )
	{	// IsAllowed will return S_FALSE if the thread is not found with SVNone.
		if( S_FALSE == SVThreadManager::Instance().IsAllowed(_T( "Archive Tools(Asynchronous)") , SVNone))
		{	// Add thread to list with null handle so the user can set affinity before the thread is created.
			SVThreadManager::Instance().Add( nullptr, _T("Archive Tools(Asynchronous)"), SVAffinityUser );
		}
	}

	if (false == InitializeAndValidate())
	{
		return false;
	}

	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if ( pInspection && pInspection->IsResetStateSet( SvDef::SVResetStateArchiveToolCreateFiles ) )
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
bool SVArchiveTool::CreateTextArchiveFile(SvStl::MessageContainerVector *pErrorMessages)
{
	//
	// CFile object.
	//
	std::string FileArchivePath;

	GetArchiveResultFilepath( FileArchivePath );

	if( FileArchivePath.empty() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_PathEmpty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	ArchiveToolHelper athFile;
	athFile.Init( FileArchivePath );

	if (athFile.isUsingKeywords())
	{
		if(athFile.areTokensValid())
		{
			FileArchivePath = athFile.TranslatePath( FileArchivePath);
		}
	}
	
	//
	// Make sure the file is closed. Before creating/opening a new one.
	//
	if (m_fileArchive.is_open())
	{
		m_fileArchive.close();
	}
	
	if ( ! FileArchivePath.empty() )
	{
		SVFileNameClass svFileName( FileArchivePath.c_str() );

		if ( _access( svFileName.GetPathName().c_str(), 0 ) != 0 )
		{
			if (!SVFileNameManagerClass::Instance().CreatePath( svFileName.GetPathName().c_str() ) )
			{
				if (nullptr != pErrorMessages)
				{
					SvDef::StringVector msgList;
					msgList.push_back( FileArchivePath );
					SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10036, getObjectId() ); 
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
	
	m_fileArchive.open( FileArchivePath.c_str(), fileMode);

	if(!m_fileArchive.is_open())
	{
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector msgList;
			msgList.push_back( FileArchivePath );
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10036, getObjectId() ); 
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
		HRESULT hr = m_bvoUseHeaders.GetValue( bUseHeaders );
		if( S_OK == hr && bUseHeaders )
		{
			// Write Header
			SvDef::StringVector HeaderVector;
			m_HeaderLabelNames.GetArrayValues( HeaderVector );
			std::string strHeader;
			for( SvDef::StringVector::iterator it = HeaderVector.begin() ; it != HeaderVector.end() ; ++it)
			{
				strHeader += *it;
				if( it+1 != HeaderVector.end())
				{
					strHeader+= _T(",");
				}
			}
			if( !strHeader.empty() )
			{
				strHeader += _T("\r\n");
				m_fileArchive.write( strHeader.c_str(), strHeader.size());
			}
		}
	} 
	catch (...)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WriteToFileFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
		
	return true;
}

void local_remove_items( SvDef::StringVector& rVec, SvVol::SVStringValueObjectClass& rvo )
{
	SvDef::StringVector NameVector;
	SvDef::StringVector::iterator iterRemoved;

	if ( !rVec.empty() )
	{
		rvo.GetArrayValues( NameVector );

		for ( iterRemoved = rVec.begin(); iterRemoved != rVec.end(); ++iterRemoved )
		{
			SvDef::StringVector::iterator iterName;
			iterName = std::find(NameVector.begin(), NameVector.end(), *iterRemoved );
			if ( iterName != NameVector.end() )
			{
				NameVector.erase( iterName );
			}
		}

		rvo.SetArraySize( static_cast< int >( NameVector.size() ) );
		rvo.SetArrayValues( NameVector );
	}
}


bool SVArchiveTool::initializeOnRun(SvStl::MessageContainerVector *pErrorMessages)
{
	if (m_ImageCollection.GetSize() > 0) //if no images are to be archived we do not need to run through the checking of disk space
	{
		if (false == ValidateImagePathAndAvailableSpace(getObjectId(), pErrorMessages))
		{
			return false;
		}
		if (!ensureArchiveImageDirectoryExists())
		{
			return false;
		}
	}

	long Method(SVArchiveInvalidMethod);
	m_evoArchiveMethod.GetValue(Method);
	m_eArchiveMethod = static_cast<SVArchiveMethodEnum> (Method);

	m_ImageCollection.ResetImageCounts();
	
	//
	// Validate the image objects to be archived.
	//
	m_ImageCollection.ValidateImageObjects();
	
	if (m_eArchiveMethod == SVArchiveGoOffline || m_eArchiveMethod == SVArchiveAsynchronous )
	{
		if ( !AllocateImageBuffers(pErrorMessages) )
		{
			return false;
		}

		if (m_eArchiveMethod == SVArchiveAsynchronous )
		{
			/*HRESULT hrThreadOnline =*/ TheSVArchiveImageThreadClass().GoOnline();
		}
	}

	//
	// Validate the results objects to be archived in text format.
	//
	int nCountResults = m_ResultCollection.ValidateResultsObjects();
	
	m_ArchiveStringBuffer.clear();

	//
	// Don't create/open the results archive file if no results to
	// archive.
	//
	if ( ! nCountResults )
	{
		return true;
	}
	
	//
	// Create a file to store results in text format.
	//
	if (m_fileArchive.is_open())
	{
		return true;
	}

	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if (nullptr != pInspection && pInspection->IsResetStateSet(SvDef::SVResetStateArchiveToolCreateFiles ))	// If going online
	{
		//
		// Create and open the results to text Archive file.
		//
		return CreateTextArchiveFile(pErrorMessages);
	}
	
	return true;
}

bool SVArchiveTool::AllocateImageBuffers(SvStl::MessageContainerVector *pErrorMessages)
{
	TheSVMemoryManager().ReleasePoolMemory(SvDef::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this);
	TheSVMemoryManager().ReleasePoolMemory(SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this);
	if (m_eArchiveMethod == SVArchiveGoOffline || m_eArchiveMethod == SVArchiveAsynchronous )
	{
		// async doesn't really allocate any buffers but it does preparation work (initm_ImageInfo)
		DWORD dwMaxImages;
		m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );
		if (m_eArchiveMethod == SVArchiveAsynchronous)
		{
			SvLib::SVOINIClass SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
			DWORD asyncBufferNumber = SvimIni.GetValueInt(_T("Settings"), _T("ArchiveToolAsyncBufferNumber"), cAsyncDefaultBufferNumber);
			dwMaxImages = std::min(dwMaxImages, asyncBufferNumber);
		}
		long toolPos = -1;
		m_ToolPosition.GetValue(toolPos);
		BufferStructCountMap bufferMap;
		HRESULT hrAllocate = m_ImageCollection.AllocateBuffers(dwMaxImages, bufferMap, toolPos);

		if (SVArchiveAsynchronous == m_eArchiveMethod)
		{
			long imageCount = std::accumulate(bufferMap.begin(), bufferMap.end(), 0, [](long sum, std::pair<SVMatroxBufferCreateStruct, long> val) { return sum + val.second; });
			TheSVArchiveImageThreadClass().setMaxNumberOfBuffer(toolPos, dwMaxImages*imageCount);
		}

		assert( S_OK == hrAllocate );
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

					const char* memoryPoolName = (m_eArchiveMethod == SVArchiveAsynchronous) ? SvDef::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME : SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME;

					for (const auto& iter : bufferMap)
					{
						long bufferNumber = iter.second*dwMaxImages;
						__int64 l_lImageBufferSize = getBufferSize(iter.first) * bufferNumber;
						hrAllocate = TheSVMemoryManager().ReservePoolMemory(memoryPoolName, this, l_lImageBufferSize);
						if (S_OK != hrAllocate)
						{
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ArchiveTool_NotEnoughBuffer, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
				msgList.push_back( SvUl::Format(_T("%x"), hrAllocate) );
				SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_AllocImageFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() ); 
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
		
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
bool SVArchiveTool::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if( ValidateOnRun(pErrorMessages) && __super::onRun( rRunStatus, pErrorMessages ) )
	{
		//
		// If this is not a test mode or run mode (online) no work is required.
		// Usually a condition where user is 'moving' a tool that causes the
		// toolset to 'run'.
		//
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) )
		{
			return true;
		}
		
		if (!m_bInitializedForRun)
		{
			rRunStatus.SetFailed();
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ArchiveTool_InitFlagFalse, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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

			if (true)	// always do this for now (never buffer text) see FR00100.007.0460.xxx
			{
				//
				// Write the result to the archive file.
				// The file may not be open at this time if a test 'run'
				// is active for one cycle.
				//
				if (m_fileArchive.is_open())
				{
					try
					{
						m_fileArchive.write(Archive.c_str(), Archive.size());
					}
					catch (...)
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
							pErrorMessages->push_back(Msg);
						}
						rRunStatus.SetFailed();
						return false;
					}
				}
			}// end if ( !bBufferText || m_eArchiveMethod != SVArchiveGoOffline )
			else if (m_eArchiveMethod == SVArchiveGoOffline)
			{
				QueueArchiveString(Archive);
			}
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
		TRACE(_T("SVArchiveTool::onRun-WriteArchiveImageFiles-Name=%s\n"), GetCompleteName());
#endif
		//
		// Iterate the list of images to archive.
		//
		m_ImageCollection.WriteArchiveImageFiles(rRunStatus.m_triggerRecord);

		rRunStatus.SetPassed();

		return true;
	}
	
	rRunStatus.SetInvalid();
	return false;
}

HRESULT SVArchiveTool::QueueArchiveString( const std::string& rArchiveString )
{
	HRESULT hr = S_OK;
	m_ArchiveStringBuffer.push_back( rArchiveString );
	return hr;
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
		if (rObjectRef.ObjectAttributesAllowed() & SvPb::archivable)
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
			if (SvPb::archivableImage == (pImage->ObjectAttributesAllowed() & SvPb::archivableImage))
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
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

bool SVArchiveTool::GetArchiveResultFilepath(std::string& rName)
{
	bool Result = (S_OK == m_stringFileArchivePath.GetValue(rName));
	return Result;
}

const std::string SVArchiveTool::GetImageArchivePathPart1() const
{
	std::string result;
	m_imageFileRootPath1.getValue(result);
	return result;
}

const std::string SVArchiveTool::GetImageArchivePathPart2() const
{
	std::string result;
	m_imageFileRootPath2.getValue(result);
	return result;
}

const std::string SVArchiveTool::GetImageArchivePathPart3() const
{
	std::string result;
	m_imageFileRootPath3.getValue(result);
	return result;
}

bool SVArchiveTool::SetFileArchivePath(LPCTSTR lpszName)
{
	return S_OK == m_stringFileArchivePath.SetValue(std::string(lpszName));
}

HRESULT SVArchiveTool::WriteBuffers()
{
	HRESULT hr = S_OK;

	// write text
	if (m_fileArchive.is_open())
	{
		try
		{
			SvDef::StringVector::iterator iter;
			for ( iter = m_ArchiveStringBuffer.begin(); iter != m_ArchiveStringBuffer.end(); ++iter )
			{
				m_fileArchive.write( iter->c_str(), iter->size());
			}
		} 
		catch (...)
		{
			hr = E_FAIL;
		}
	}

	// write images

	HRESULT hrImages = m_ImageCollection.WriteImageQueue();
	if ( S_OK == hr )
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

	std::string baseDirectoryname;
	m_baseDirectoryname.GetValue(baseDirectoryname);

	std::string imageDirectoryPath = rImagePathRoot;

	std::string namedDirectoryname = SvUl::Format("%s%06ld", baseDirectoryname.c_str(), DirectorynameIndex);

	auto concatDir = [](const std::string& rA, const std::string& rB) {return rA + "\\" + rB;};

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

		if(namedDirectoryInCategoryDirectory)
		{
			imageDirectoryPath = concatDir(rImagePathRoot,concatDir(categoryDirectoryName,namedDirectoryname));
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
	std::string baseFilename;
	m_baseFilename.GetValue(baseFilename);

		double index;
		m_FilenameIndex1.getValue(index);
		uint32_t Index1 = static_cast<uint32_t> (index);

	std::string centerFilename;
	m_centerFilename.GetValue(centerFilename);

		m_FilenameIndex2.getValue(index);
		uint32_t Index2 = static_cast<uint32_t> (index);

	return SvUl::Format(_T("%s%08ld%s%04ld.bmp"), baseFilename.c_str(), Index1, centerFilename.c_str(), Index2);
}


long SVArchiveTool::CalculateImageMemory(SvIe::SVImageClass* pImage)
{
	assert(nullptr != pImage);
	long lImageSize = 0;
	if (nullptr != pImage )
	{
		SVImageInfoClass info = pImage->GetImageInfo();
		lImageSize = info.GetBufferSize();
	}
	return lImageSize;
}

long SVArchiveTool::CalculateImageMemory( std::vector<SvIe::SVImageClass*> imagePtrVector )
{
	long lTotalMemory = 0;
	for ( size_t i=0; i < imagePtrVector.size(); i++ )
	{
		SvIe::SVImageClass* pImage = imagePtrVector[i];
		lTotalMemory += CalculateImageMemory( pImage );

	}
	return lTotalMemory;
}

std::string SVArchiveTool::getUntranslatedImagePathRoot() const
{
	return GetImageArchivePathPart1() + GetImageArchivePathPart2() + GetImageArchivePathPart3();
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
	if (m_eArchiveMethod == SVArchiveAsynchronous )
	{
		TheSVArchiveImageThreadClass().GoOffline();
	}

	if (m_eArchiveMethod == SVArchiveGoOffline )
	{
		WriteBuffers();
	}

	// Close the text to archive file if necessary
	if(m_fileArchive.is_open())
	{
		m_fileArchive.close();
	}
}

void SVArchiveTool::OnObjectRenamed( const SVObjectClass& rRenamedObject, const std::string& rOldName )
{
	enum class ReplaceType { Search, Full, First};
	std::string newPrefix;
	std::string oldPrefix;

	ReplaceType replaceType {ReplaceType::Search};
	SvPb::SVObjectTypeEnum type = rRenamedObject.GetObjectType();
	if(SvPb::SVInspectionObjectType == type)
	{
		newPrefix = rRenamedObject.GetName();
		oldPrefix = rOldName;
		replaceType = ReplaceType::First;
	}
	else if(SvPb::SVBasicValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVRootObjectType, false);
		newPrefix = dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName();
		oldPrefix = dottedNameWithoutObjectname + _T(".") + rOldName;
		replaceType = ReplaceType::Full;
	}
	else if(SvPb::SVValueObjectType == type)
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

	for (int i = 0; i < iSize; i++ )
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
		m_svoArchiveResultNames.SetValue( Name, i );
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}


bool SVArchiveTool::updateCurrentImagePathRoot(bool displayMessageOnInvalidKeywords)
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
			if (displayMessageOnInvalidKeywords)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_InvalidKeywordsInPath, SvStl::SourceFileParams(StdMessageParams));
			}
			return false;
		}
	}

	return true;
}


std::string SVArchiveTool::archiveImageDirectory()
{
	BOOL useAlternativeImagePaths = FALSE;
	m_useAlternativeImagePaths.GetValue(useAlternativeImagePaths);

	std::string imageDirectoryPath = useAlternativeImagePaths ? alternativeImageDirectory(m_currentImagePathRoot) : m_currentImagePathRoot;

	return imageDirectoryPath;
}


bool SVArchiveTool::ensureArchiveImageDirectoryExists()
{
	bool ok = false;

	BOOL useAlternativeImagePaths = FALSE;
	m_useAlternativeImagePaths.GetValue(useAlternativeImagePaths);

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
		SvDef::StringVector msgList;
		msgList.push_back(e.what());
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	
	return ok;
}


bool SVArchiveTool::ValidateImagePathAndAvailableSpace(uint32_t objectId, SvStl::MessageContainerVector* pErrorMessages)
{
	if (!updateCurrentImagePathRoot())
	{
		return false;
	}

	if(!ensureArchiveImageDirectoryExists())
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
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10037, objectId);
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
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Drive_Full, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectId);
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

bool SVArchiveTool::ValidateOnRun(SvStl::MessageContainerVector *pErrorMessages)	// called each onRun
{
	bool bOk = true;

	if ( (0 == m_uiValidateCount % 10))
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
