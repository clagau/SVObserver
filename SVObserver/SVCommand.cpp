//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommand.cpp
//* .File Name       : $Workfile:   SVCommand.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.19  $
//* .Check In Date   : $Date:   19 Dec 2014 15:01:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"

#include "SVObserver_i.h"
#include "SVCommand.h"

#include "SVUtilityLibrary/DirectoryUtilities.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectNameInfo.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"

#include "SVInfoStructs.h"
#include "SVIPDoc.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "InspectionEngine/SVImageBuffer.h"	//SVImageOverlayClass
#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "Tools/SVTool.h"
#include "SVCommandInspectionCollectImageData.h"

#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "RemoteCommand.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "SVVisionProcessorHelper.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectInterfaces/ICommand.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr DWORD cDefaultNotAllowedStates = SV_STATE_UNAVAILABLE | SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING | SV_STATE_EDITING |
SV_STATE_CANCELING | SV_STATE_INTERNAL_RUN | SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING | SV_STATE_REMOTE_CMD;
#pragma endregion Declarations

#pragma region Constructor
SVCommand::SVCommand()
	: m_pStream(nullptr)
{

}

SVCommand::~SVCommand()
{

}
#pragma endregion Constructor

// ***************************************************************
// ***************************************************************
// The following group of functions are the same as above but they
// do NOT use XML to wrap each function call. These functions also
// break large files up into small blocks to decrease memory use.
// ***************************************************************
// ***************************************************************

STDMETHODIMP SVCommand::SVGetSVIMState(unsigned long* ulState)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		bSuccess = GlobalRCGetState(ulState);
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetSVIMState

STDMETHODIMP SVCommand::SVSetSVIMMode(unsigned long lSVIMNewMode)
{
	HRESULT hrResult;

	// Check if we are in an allowed state first
	// Not allowed to transition to or from Regression or Test
	if (lSVIMNewMode & (SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_64_SVIM_MODE_NOT_REMOTELY_SETABLE;
	}
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}
	do
	{
		hrResult = S_OK;

		try
		{
			hrResult = GlobalRCSetMode(lSVIMNewMode);
		}
		catch (...)
		{
			hrResult = S_FALSE;
		}

		if (!(S_OK == hrResult))
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	} while (0);

	return hrResult;
}// end SVSetSVIMMode

STDMETHODIMP SVCommand::SVGetSVIMMode(unsigned long* p_plSVIMMode)
{
	HRESULT hrResult;

	hrResult = S_OK;

	try
	{
		hrResult = GlobalRCGetMode(p_plSVIMMode);
	}
	catch (...)
	{
		hrResult = S_FALSE;
	}

	if (S_OK != hrResult)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	return hrResult;
}// end SVGetSVIMMode

STDMETHODIMP SVCommand::SVSetSVIMState(unsigned long ulSVIMState)
{
	HRESULT hrResult = S_OK;

	if (SVSVIMStateClass::CheckState(SV_STATE_EDITING))
	{
		hrResult = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else
	{
		bool bSuccess = false;
		try
		{
			if (ulSVIMState)
			{
				// Check if we are in an allowed state first
				// Not allowed to perform if State is Regression or Test
				if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
				{
					return SVMSG_63_SVIM_IN_WRONG_MODE;
				}
				GlobalRCGoOnline();
			}
			else
			{
				GlobalRCGoOffline();
			}
			bSuccess = true;
		}
		catch (...)
		{
			bSuccess = false;
		}

		if (!bSuccess)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			hrResult = S_FALSE;
		}
	}

	return hrResult;
}// end SVSetSVIMState

STDMETHODIMP SVCommand::SVGetSVIMConfig(long lOffset, long* lBlockSize, BSTR* bstrFileData, BOOL* bLastFlag)
{
	bool bHrSet = false;
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		std::string ConfigName = GlobalRCGetConfigurationName();

		bSuccess = !ConfigName.empty();

		// check offset: if zero then it is first time in
		if (lOffset < 1)
		{
			// on first time
			// pack files to temp area before sending packets

			if (bSuccess)
			{
				bSuccess = S_OK == GlobalRCSaveConfiguration(ConfigName.c_str());
			}
		}//offset < 1  end of the fist time

		//send data back to control

		if (bSuccess)
		{
			CFileException* ex = new CFileException;
			CFile binFile;
			if (binFile.Open(ConfigName.c_str(), CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, ex))
			{
				long lFileSize = (long)binFile.GetLength();
				long lBytesToGo = lFileSize - lOffset;
				if ((*lBlockSize > lBytesToGo) || (*lBlockSize < 1))
				{
					*lBlockSize = lBytesToGo;
					*bLastFlag = true;
				}

				*bstrFileData = SysAllocStringByteLen(nullptr, *lBlockSize);
				if (nullptr == (*bstrFileData))
				{
					AfxThrowMemoryException();
				}

				binFile.Seek(lOffset, CFile::begin);
				binFile.Read(*bstrFileData, *lBlockSize);
				// Close the file
				binFile.Close();
				ex->Delete();
			}// end if
			else
			{
				//unable to open file
				bSuccess = false;
				throw (ex);
			}
		}//end if
	}
	catch (CMemoryException* memEx)
	{
		TCHAR szCause[255];
		memEx->GetErrorMessage(szCause, 255);
		assert(false);
		hrResult = SVMSG_CMDCOMSRV_MEMORY_ERROR;
		bHrSet = true;
		memEx->Delete();
	}
	catch (CFileException* ex)
	{
		TCHAR szCause[255];
		ex->GetErrorMessage(szCause, 255);
		assert(false);
		hrResult = SVMSG_CMDCOMSRV_FILE_ERROR;
		bHrSet = true;
		ex->Delete();
	}
	catch (...)
	{
		bSuccess = false;
	}

	if ((!bSuccess) && (!bHrSet))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetSVIMConfig

STDMETHODIMP SVCommand::SVPutSVIMConfig(long lOffset, long lBlockSize, BSTR* pFileData, BOOL bLastFlag)
{
	//PackedFileName is static due to SVPutConfig which can be called multiple times for each data block sent
	//The file name is set only when the first block is sent (lOffset == 0) but is needed when the following blocks are sent
	//After the last block the file name is cleared
	static std::string PackedFileName;
	static ConfigFileType fileType{ ConfigFileType::SvzFormatDefaultName };
	HRESULT hrResult = S_OK;
	CFile binFile;
	bool bHrSet = false;

	// Check if we are in an allowed state first
	// Not allowed to perform if Mode is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);

	bool bSuccess = false;

	try
	{
		if (CreateDirPath(SvStl::GlobalPath::Inst().GetTempPath().c_str()))
		{
			bool bRet = false;
			CFileException* ex = new CFileException;
			if (lOffset < 1)
			{
				PackedFileName = SvStl::GlobalPath::Inst().GetTempPath(_T("Temp"));
				PackedFileName += SvDef::cPackedConfigExtension;
				//For old .pac file format the first 4 bytes are always 1
				DWORD fileVersion = (lBlockSize > sizeof(DWORD)) ? *(reinterpret_cast<DWORD*> (*pFileData)) : 0;
				//With SIAC the file name is not sent so we need to use the .svx name inside a .svz file
				fileType = (1 == fileVersion) ? ConfigFileType::PackedFormat : ConfigFileType::SvzFormatDefaultName;
				if (binFile.Open(PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, ex))
				{
					bRet = true;
				}
				else
				{
					throw (ex);
				}
			}
			else
			{
				if (binFile.Open(PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeBinary, ex))
				{
					bRet = true;
				}
				else
				{
					throw (ex);
				}
			}

			if (bRet)
			{
				binFile.Seek(lOffset, CFile::begin);
				binFile.Write(*pFileData, lBlockSize);
				binFile.Close();
				ex->Delete();
				bSuccess = true;
			}
		}

		if (bLastFlag)
		{
			bSuccess = S_OK == GlobalRCLoadPackedConfiguration(PackedFileName.c_str(), fileType);
			PackedFileName.clear();
			fileType = ConfigFileType::SvzFormatDefaultName;
		}
	}
	catch (CFileException* theEx)
	{
		TCHAR szCause[255];
		theEx->GetErrorMessage(szCause, 255);
		assert(false);
		hrResult = theEx->m_lOsError;
		theEx->Delete();
	}
	catch (...)
	{
		bSuccess = false;
	}

	if ((!bSuccess) && (!bHrSet))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);

	return hrResult;
}// end SVPutSVIMConfig

STDMETHODIMP SVCommand::SVGetSVIMFile(BSTR bstrSourceFile, long lOffset, long* lBlockSize, BSTR* pFileData, long* lLastPacketFlag)
{
	HRESULT hrResult = S_OK;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];

	CString strFilename(bstrSourceFile, ::SysStringLen(bstrSourceFile));

	_tsplitpath(strFilename, szDrive, szDir, szFile, szExt);

	if (!_tcscmp(szDrive, _T("")))
	{
		_tcscpy(szDrive, _T("C:"));
		_tcscpy(szDir, _T("\\Run\\"));
	}

	_tmakepath(szPath, szDrive, szDir, szFile, szExt);

	if (::_access(szPath, 0) == 0)
	{
		CFile binFile;
		CFileException l_FileException;

		if (binFile.Open(szPath, CFile::shareDenyNone | CFile::modeReadWrite | CFile::typeBinary, &l_FileException))
		{
			try
			{
				long lFileSize = (long)binFile.GetLength();
				long lBytesToGo = lFileSize - lOffset;

				if ((*lBlockSize > lBytesToGo) || (*lBlockSize < 1))
				{
					*lBlockSize = lBytesToGo;
					*lLastPacketFlag = true;
				}

				*pFileData = SysAllocStringByteLen(nullptr, *lBlockSize);
				if (nullptr == *pFileData)
				{
					AfxThrowMemoryException();
				}

				// Transfer the file data into the buffer
				binFile.Seek(lOffset, CFile::begin);

				binFile.Read(*pFileData, *lBlockSize);

				// Close the file
				binFile.Close();
			}
			catch (CFileException* theException)
			{
				hrResult = SVMSG_CMDCOMSRV_FILE_ERROR;
				theException->Delete();
			}
			catch (CMemoryException* memEx)
			{
				hrResult = SVMSG_CMDCOMSRV_MEMORY_ERROR; //use this value for now...
				memEx->Delete();
			}
			catch (...)
			{
				hrResult = SVMSG_CMDCOMSRV_ERROR;
			}
		}
		else
		{
			hrResult = l_FileException.m_lOsError;
		}

		if (!SUCCEEDED(hrResult))
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		hrResult = ERROR_FILE_NOT_FOUND;
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVPutSVIMFile(BSTR bstrDestFile, long lOffset, long lBlockSize, BSTR* pFileData)
{
	HRESULT hrResult = S_OK;

	std::string Path;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];

	CString strFilename(bstrDestFile, ::SysStringLen(bstrDestFile));

	_tsplitpath(strFilename, szDrive, szDir, szFile, szExt);

	if (!_tcscmp(szDrive, _T("")))
	{
		_tcscpy(szDrive, _T("C"));
		_tcscpy(szDir, _T("\\Run\\"));
	}

	_tmakepath(szPath, szDrive, szDir, szFile, szExt);

	Path = szDrive;
	Path += szDir;

	CreateDirPath(Path.c_str());

	CFile binFile;
	CFileException l_FileException;

	BOOL l_FileOpen = false;

	if (lOffset < 1)
	{
		l_FileOpen = binFile.Open(szPath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, &l_FileException);
	}
	else
	{
		l_FileOpen = binFile.Open(szPath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeBinary, &l_FileException);
	}

	if (l_FileOpen)
	{
		try
		{
			binFile.Seek(lOffset, CFile::begin);

			binFile.Write(*pFileData, lBlockSize);
		}
		catch (CFileException* theEx)
		{
			hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;
			theEx->Delete();

			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
		catch (...)
		{
			hrResult = SVMSG_CMDCOMSRV_ERROR;
		}

		// close the file
		binFile.Close();
	}
	else
	{
		hrResult = l_FileException.m_lOsError;
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVLoadSVIMConfig(BSTR bstrConfigFilename)
{
	HRESULT result = (SVSVIMStateClass::CheckState(cDefaultNotAllowedStates) || SVSVIMStateClass::isSvrcBlocked()) ? SVMSG_SVO_ACCESS_DENIED : S_OK;

	if (S_OK == result)
	{
		std::string ConfigFile = SvUl::createStdString(_bstr_t(bstrConfigFilename));

		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
		result = SVVisionProcessorHelper::Instance().LoadConfiguration(ConfigFile);
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	}

	return result;
}


STDMETHODIMP SVCommand::SVGetSVIMConfigName(BSTR* bstrConfigFilename)
{
	HRESULT Result(S_OK);

	try
	{
		std::string ConfigName = GlobalRCGetConfigurationName();

		Result = ConfigName.empty() ? E_FAIL : S_OK;

		if (S_OK == Result)
		{
			*bstrConfigFilename = _bstr_t(ConfigName.c_str()).Detach();
		}
	}
	catch (...)
	{
		Result = E_FAIL;
	}

	if (S_OK != Result)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	return Result;
}

STDMETHODIMP SVCommand::SVGetSVIMOfflineCount(unsigned long* pOfflineCount)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	if (nullptr != pOfflineCount)
	{
		*pOfflineCount = TheSVObserverApp.getOfflineCount();
		bSuccess = true;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVGetSVIMVersion(unsigned long* pVersion)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	if (nullptr != pVersion)
	{
		*pVersion = TheSVObserverApp.getCurrentVersion();
		bSuccess = true;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVGetSVIMConfigPrint(long, long* lBlockSize, BSTR* bstrConfigPrint, BOOL* bLastFlag)
{
	std::string ConfigPrint;
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		SVConfigurationPrint printConfig;
		printConfig.printConfigToStringBuffer(ConfigPrint);

		*bstrConfigPrint = _bstr_t(ConfigPrint.c_str()).Detach();
		*lBlockSize = ::SysStringByteLen(*bstrConfigPrint);
		*bLastFlag = true;

		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

struct SVGetImageListImageInfo
{
	_bstr_t m_ImageName;
	uint32_t m_ImageId;
	SVInspectionProcess* m_pInspection;

	SVGetImageListImageInfo() : m_ImageName(), m_ImageId(), m_pInspection(nullptr) {}

	SVGetImageListImageInfo(const _bstr_t& p_rImageName, uint32_t imageId, SVInspectionProcess* p_pInspection)
		: m_ImageName(p_rImageName), m_ImageId(imageId), m_pInspection(p_pInspection)
	{
	}
};





// Currently used only through external DCOM connection.
STDMETHODIMP SVCommand::SVGetImageList(SAFEARRAY* psaNames, long, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts)
{
	HRESULT hrResult = S_OK;

	typedef std::map< SVInspectionProcess*, std::set<uint32_t> > SVInspectionImageIdMap;
	typedef std::map< SVInspectionProcess*, std::shared_ptr<SVIPProductStruct> > InspectionImageDataMap;
	typedef std::deque< SVGetImageListImageInfo > SVImageNameIdDeque;

	try
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr == pConfig || !pConfig->IsConfigurationLoaded())
		{
			throw SVMSG_CONFIGURATION_NOT_LOADED;
		}

		unsigned long lNumberOfElements = psaNames->rgsabound[0].cElements;

		assert(nullptr != ppsaStatus);
		assert(nullptr != *ppsaStatus);	// must provide allocated SafeArray(LONG)
		assert((*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements);

		assert(nullptr != ppsaImages);
		assert(nullptr != *ppsaImages);	// must provide allocated SafeArray(LONG)
		assert((*ppsaImages)->rgsabound[0].cElements == lNumberOfElements);

		assert(nullptr != ppsaOverlays);
		assert(nullptr != *ppsaOverlays);// must provide allocated SafeArray(LONG)
		assert((*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements);

		assert(nullptr != ppsaProcCounts);
		assert(nullptr != *ppsaProcCounts);// must provide allocated SafeArray(LONG)
		assert((*ppsaProcCounts)->rgsabound[0].cElements == lNumberOfElements);

		long lDefaultStatus = S_FALSE;

		// set default status for objects in list
		for (long lIndex = 0; lIndex < static_cast<long> (lNumberOfElements); lIndex++)
		{
			/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
		}

		bool l_bItemNotFound = false;
		bool l_bInspectionNotFound = false;

		SVInspectionImageIdMap l_InspectionImageIds;
		SVImageNameIdDeque l_ImageNameIds;

		//go through list of names and make sure they are all valid
		// 1) Inspection exists
		// 2) Requested data item exists
		SVInspectionProcess* pInspection(nullptr);
		HRESULT hrOK = S_OK;

		for (long i = 0; i < static_cast<long> (lNumberOfElements) && SUCCEEDED(hrOK); i++)
		{
			BSTR bstrName = nullptr;
			SafeArrayGetElementNoCopy(psaNames, &i, &bstrName);
			std::string l_Name = SvUl::createStdString(bstrName);

			if (nullptr != pConfig && pConfig->GetInspectionObject(l_Name.c_str(), &pInspection) && nullptr != pInspection)
			{
				SvIe::SVImageClass* pImage = nullptr;
				SVObjectManagerClass::Instance().GetObjectByDottedName(l_Name, pImage);
				if (pImage)
				{
					l_InspectionImageIds[pInspection].insert(pImage->getObjectId());

					l_ImageNameIds.emplace_back(bstrName, pImage->getObjectId(), pInspection);
				}
				else	// couldn't find object
				{
					l_ImageNameIds.emplace_back(bstrName, SvDef::InvalidObjectId, pInspection);

					hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					l_bItemNotFound = true;

					/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &i, (void*)&hrOK);
				}
			}
			else	// couldn't find inspection
			{
				l_ImageNameIds.emplace_back(bstrName, SvDef::InvalidObjectId, nullptr);

				hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

				l_bInspectionNotFound = true;

				/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &i, (void*)&hrOK);
			}
		}// end for ( long l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )

		// requested values are not valid - return an error...
		if (!SUCCEEDED(hrOK))
		{
			throw hrOK;
		}

		InspectionImageDataMap  InspectionImageData;

		SVInspectionImageIdMap::iterator l_InspectionIter;

		for (l_InspectionIter = l_InspectionImageIds.begin(); l_InspectionIter != l_InspectionImageIds.end(); ++l_InspectionIter)
		{
			if (nullptr != l_InspectionIter->first)
			{

				uint32_t inspectionId = l_InspectionIter->first->getObjectId();
				CommandInspectionCollectImageData collectImageData(l_InspectionIter->first->getObjectId(), l_InspectionIter->second);
				CollectImageDataTask collectImageDataTask(std::move(collectImageData));

				
			std::future< std::shared_ptr<SVIPProductStruct>> futureProductStruct = collectImageDataTask.get_future();

				if (inspectionId > 0)
				{

					SvOi::IInspectionProcess* pInspec = dynamic_cast<SvOi::IInspectionProcess*>((SvOi::getObject(inspectionId)));
					if (pInspec)
					{
						SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<CollectImageDataTask>>(std::move(collectImageDataTask));
						pInspec->SubmitCommand(pCommand);
					}
					else
					{
						assert(false);

					}

					std::future_status status = futureProductStruct.wait_for(std::chrono::milliseconds{ 120000 });


					if (status == std::future_status::ready)
					{
						InspectionImageData[l_InspectionIter->first] = futureProductStruct.get();
					}
					else
					{
						l_bInspectionNotFound = true;
					}

				}
			}
		}

		for (long l = 0; l < static_cast<long> (lNumberOfElements); l++)
		{
			Sleep(0);

			bool bImageOk = false;
			long lProcessCount = -1;
			SvIe::SVImageOverlayClass l_OverlayClass;

			std::shared_ptr<SVIPProductStruct>  pProduct;
			InspectionImageDataMap::iterator l_Iter = InspectionImageData.find(l_ImageNameIds[l].m_pInspection);

			if (l_Iter != InspectionImageData.end())
			{
				pProduct = l_Iter->second;
			}

			if (nullptr != pProduct.get())
			{
				SVIPImageDataElementMap::const_iterator l_ImageIter = pProduct->m_ImageData.find(l_ImageNameIds[l].m_ImageId);

				if (l_ImageIter != pProduct->m_ImageData.end())
				{
					HRESULT hr = S_OK;

					char* pDIB = nullptr;

					BSTR bstrImage = SysAllocStringByteLen(nullptr, static_cast<UINT>(l_ImageIter->second.m_ImageDIB.size()));
					if (nullptr == bstrImage)
					{
						hr = -1568;
					}
					else
					{
						//------ Convert from wide character pointer to character 
						//------ pointer. Doesn't matter for binary data.
						pDIB = (char*)(bstrImage);
					}

					if (S_OK == hr)
					{
						// Copy data to DIB memory locations
						memcpy(pDIB, &(l_ImageIter->second.m_ImageDIB[0]), l_ImageIter->second.m_ImageDIB.size());

						lProcessCount = pProduct->m_TriggerCount;
						l_OverlayClass = l_ImageIter->second.m_OverlayData;

						// Add the status to the outgoing array
						HRESULT hrItem = S_OK;
						hr = SafeArrayPutElement(*ppsaStatus, &l, (void*)&hrItem);
						if (S_OK != hr)
						{
							throw - 1580;
						}

						// Add the image to the outgoing array
						SafeArrayPutElementNoCopy(*ppsaImages, &l, bstrImage);

						/*hr =*/ SafeArrayPutElement(*ppsaProcCounts, &l, (void*)&lProcessCount);

						long lSize = l_OverlayClass.GetBufferSize();
						BYTE* pBytes = l_OverlayClass.GetBuffer();

						BSTR bstrOverlay = ::SysAllocStringByteLen(nullptr, lSize);
						memcpy(bstrOverlay, pBytes, lSize);

						// Add overlays to the outgoing array
						SafeArrayPutElementNoCopy(*ppsaOverlays, &l, bstrOverlay);

						bImageOk = true;
					}
				}
			}

			if (!bImageOk)
			{
				// Add nullptr to the outgoing array
				lProcessCount = -1;
				::SafeArrayPutElement(*ppsaImages, &l, nullptr);
				::SafeArrayPutElement(*ppsaOverlays, &l, nullptr);
				::SafeArrayPutElement(*ppsaProcCounts, &l, (void*)&lProcessCount);

				l_bItemNotFound = true;
			}// end if ( !bImageOk )
		}// end for( long l = 0; l < lNumberOfElements; l++ )

		if (hrResult < 0)
		{
			throw hrResult;
		}

		if (l_bItemNotFound || l_bInspectionNotFound)
		{
			hrResult = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
	} // end try
	catch (HRESULT hr)
	{
		hrResult = hr;
	}
	catch (int hr)
	{
		hrResult = (HRESULT)hr;
	}
	catch (DWORD hr)
	{
		hrResult = (HRESULT)hr;
	}
	catch (...)
	{
		hrResult = -1579;
	}

	if (FAILED(hrResult))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	return hrResult;
}


STDMETHODIMP SVCommand::SVRegisterStream(SAFEARRAY*, VARIANT, SAFEARRAY**)
{
	return E_FAIL;
}

STDMETHODIMP SVCommand::SVUnRegisterStream(VARIANT)
{
	return E_FAIL;
}

STDMETHODIMP SVCommand::SVGetProductDataList(long lProcessCount, SAFEARRAY* psaNames, SAFEARRAY** ppsaData, SAFEARRAY** ppsaStatus)
{
	HRESULT hr = S_OK;

	BSTR bstr;
	SvDef::StringVector ValueNames;
	SVInspectionProcessVector InspectionVector;
	SVObjectReferenceVector aValueObjects;
	bool l_bItemNotExist = false;

	//check to see if in Run Mode.  if not return SVMSG_53_SVIM_NOT_IN_RUN_MODE

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		return SVMSG_53_SVIM_NOT_IN_RUN_MODE;
	}

	// VERIFY THE LIST OF NAMES

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	unsigned long lNumberOfElements = psaNames->rgsabound[0].cElements;

	assert(nullptr != ppsaStatus);
	assert(nullptr != *ppsaStatus);	// must provide allocated SafeArray(LONG)
	assert((*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements);

	assert(nullptr != ppsaData);
	assert(nullptr != *ppsaData);	// must provide allocated SafeArray(LONG)
	assert((*ppsaData)->rgsabound[0].cElements == lNumberOfElements);

	long lDefaultStatus = S_FALSE;

	// create String array of value names; set default Status
	for (long lIndex = 0; lIndex < static_cast<long> (lNumberOfElements); lIndex++)
	{
		SafeArrayGetElementNoCopy(psaNames, &lIndex, &bstr);
		std::string sName = SvUl::createStdString(_bstr_t(bstr));
		ValueNames.push_back(sName);

		SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
	}

	if (nullptr == pConfig || !pConfig->IsConfigurationLoaded())
	{
		hr = SVMSG_CONFIGURATION_NOT_LOADED;
		return hr;
	}

	//go through list of names and make sure they are all valid
	// 1) Inspection exists
	// 2) Requested data item exists
	// 3) all data items are on the same PPQ
	HRESULT hrOK = S_OK;
	uint32_t l_PPQId = SvDef::InvalidObjectId;

	for (long i = 0; i < static_cast<long> (lNumberOfElements) && SUCCEEDED(hrOK); i++)
	{
		SVInspectionProcess* pInspection(nullptr);
		//GetInspectionObject is only true if the pointer is valid
		if ((nullptr != pConfig) && pConfig->GetInspectionObject(ValueNames[i].c_str(), &pInspection))
		{
			InspectionVector.push_back(pInspection);	// add inspection object to list

			SVObjectReference ObjectRef;
			SVObjectManagerClass::Instance().GetObjectByDottedName(ValueNames[i].c_str(), ObjectRef);
			if (ObjectRef.getObject())
			{
				aValueObjects.push_back(ObjectRef);	// add data object pointer to the list

				if (SvDef::InvalidObjectId == l_PPQId) //set the first PPQ name as the comparison standard
				{
					l_PPQId = pInspection->GetPPQIdentifier();
				}
				else
				{
					if (l_PPQId != pInspection->GetPPQIdentifier())	// not on same PPQ
					{
						hrOK = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
						// set the status of all objects to this error
						for (long l = 0; l < static_cast<long> (lNumberOfElements); l++)
						{
							/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &l, (void*)&hrOK);
						}
						break;
					}
				}
			}
			else	// couldn't find data object
			{
				aValueObjects.push_back(SVObjectReference());
				hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &i, (void*)&hrOK);
			}
		}
		else	// couldn't find inspection
		{
			InspectionVector.push_back(nullptr);
			aValueObjects.push_back(SVObjectReference());
			hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &i, (void*)&hrOK);
		}
	}

	// requested values are not valid - return an error...
	if (!SUCCEEDED(hrOK))
	{
		hr = hrOK;
		return hr;
	}

	// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
	SVProductInfoStruct l_ProductInfo;

	SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(l_PPQId));

	if (nullptr != pPPQ && S_OK == pPPQ->GetProduct(l_ProductInfo, lProcessCount))
	{
		// Product is still valid; 
		// Build the array containing the result data

		for (long i = 0; i < static_cast<long> (lNumberOfElements); i++)
		{
			SVObjectReference ObjectRef = aValueObjects[i];
			std::string Value;
			HRESULT hrStatus = S_OK;

			if (nullptr != ObjectRef.getObject())
			{
				if (!ObjectRef.isEntireArray())
				{
					HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, ObjectRef.getValidArrayIndex());
					if (S_OK == hrGet)
					{
						// put value in return array
						_bstr_t bstrTemp = Value.c_str();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTemp.Detach());

						HRESULT hrItem = S_OK;
						SafeArrayPutElement(*ppsaStatus, &i, &hrItem);
					}// if OK
					else if (SVMSG_SVO_33_OBJECT_INDEX_INVALID == hrGet
						|| SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hrGet)
					{
						hrStatus = hrGet;
						// did not get value.  set value to default
						Value = ObjectRef.DefaultValue();
						if (Value.empty())
						{
							Value = _T("-1");
						}
						_bstr_t bstrTmpVal = Value.c_str();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal.Detach());
						//::SysFreeString(bstrTmpVal);
						::SafeArrayPutElement(*ppsaStatus, &i, &hr);
						l_bItemNotExist = true;
					}// else invalid or out of range index
					else	// some generic error; currently should not get here
					{
						assert(false);
						hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						// did not get value.  set value to -1
						Value = _T("-1");
						_bstr_t bstrTmpVal = Value.c_str();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal.Detach());
						//::SysFreeString(bstrTmpVal);
						::SafeArrayPutElement(*ppsaStatus, &i, &hr);
						l_bItemNotExist = true;
					}
				}// if ( !ref.IsEntireArray() )
				else	// GET ENTIRE ARRAY
				{
					// get all results and put them into a parsable string
					int NumResults = ObjectRef.getValueObject()->getResultSize();
					std::string ArrayValues;
					for (int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++)
					{
						HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, iArrayIndex);
						if (S_OK == hrGet)
						{
							if (iArrayIndex > 0)
							{
								ArrayValues += _T(",");
							}

							ArrayValues += SvUl::Format(_T("`%s`"), Value.c_str());
						}
						else
						{
							break;
						}
					}

					hrStatus = S_OK;
					_bstr_t bstrTmpVal = ArrayValues.c_str();
					SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal.Detach());
					::SafeArrayPutElement(*ppsaStatus, &i, &hrStatus);
				}// end if ( !ref.IsEntireArray() ) else
			}// end if ( nullptr != ref.Object() )
			else
			{
				// sValue is empty at this point
				_bstr_t bstrTmpVal = Value.c_str();
				SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal.Detach());

				HRESULT hrItem = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				SafeArrayPutElement(*ppsaStatus, &i, &hrItem);
				l_bItemNotExist = true;
			}
		}// end for (long i=0; i < lNumberOfElements; i++)
	}// end if (pPPQ->GetProductInfoStruct(lProcessCount, &pProductInfoStruct))
	else
	{
		hr = SVMSG_PRODUCT_NO_LONGER_AVAILABLE;	// product no longer alive
	}
	if (l_bItemNotExist)
	{
		hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}

	return hr;
}

STDMETHODIMP SVCommand::SVGetProductImageList(long lProcessCount, SAFEARRAY* psaNames, long, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus)
{
	HRESULT hrResult = S_OK;

	//check to see if in Run Mode.  if not return SVMSG_53_SVIM_NOT_IN_RUN_MODE

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		return SVMSG_53_SVIM_NOT_IN_RUN_MODE;
	}

	try
	{
		// SETUP
		long l(0);

		unsigned long lNumberOfElements = psaNames->rgsabound[0].cElements;

		assert(nullptr != ppsaStatus);
		assert(nullptr != *ppsaStatus);	// must provide allocated SafeArray(LONG)
		assert((*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements);

		assert(nullptr != ppsaImages);
		assert(nullptr != *ppsaImages);	// must provide allocated SafeArray(LONG)
		assert((*ppsaImages)->rgsabound[0].cElements == lNumberOfElements);

		assert(nullptr != ppsaOverlays);
		assert(nullptr != *ppsaOverlays);// must provide allocated SafeArray(LONG)
		assert((*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements);

		BSTR bstr = nullptr;
		SvIe::SVImageClassPtrVector ImageObjects;
		SVInspectionProcessVector aInspections;
		bool l_bItemNotFound = false;
		bool l_bInspectionNotFound = false;
		BSTR bstrOverlay = nullptr;

		long lDefaultStatus = S_FALSE;

		// set default status for objects in list
		for (long lIndex = 0; lIndex < static_cast<long> (lNumberOfElements); lIndex++)
		{
			/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
			// No overlays for no for product source images
			bstr = nullptr;
			/*hrTemp = */SafeArrayPutElement(*ppsaOverlays, &lIndex, bstr);
		}

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr == pConfig || !pConfig->IsConfigurationLoaded())
		{
			throw SVMSG_CONFIGURATION_NOT_LOADED;
		}

		//go through list of names and make sure they are all valid
		// 1) Inspection exists
		// 2) Requested data item exists
		// 3) all data items are on the same PPQ
		SVInspectionProcess* pInspection(nullptr);
		HRESULT hrOK = S_OK;
		uint32_t l_PPQId = SvDef::InvalidObjectId;

		for (l = 0; l < static_cast<long> (lNumberOfElements) && SUCCEEDED(hrOK); l++)
		{
			BSTR bstrName = nullptr;
			SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
			std::string strName = SvUl::createStdString(_bstr_t(bstrName));

			if (nullptr != pConfig && pConfig->GetInspectionObject(strName.c_str(), &pInspection))
			{
				aInspections.push_back(pInspection);
				SvIe::SVImageClass* pImage = nullptr;
				if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(strName.c_str(), pImage))
				{
					bool bImageOK = false;
					if (nullptr != dynamic_cast<SvIe::SVCameraImageTemplate*>(pImage)) // Source image
					{
						ImageObjects.push_back(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else if (pImage->ObjectAttributesSet() & SvPb::publishResultImage)	// Published result image
					{
						ImageObjects.push_back(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else
					{
						hrOK = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE;
						l_bItemNotFound = true;
						ImageObjects.push_back(nullptr);
						/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &l, (void*)&hrOK);
					}

					// if image is OK, check PPQ
					if (bImageOK)
					{
						if (SvDef::InvalidObjectId == l_PPQId) //set the first PPQ name as the comparison standard
						{
							l_PPQId = pInspection->GetPPQIdentifier();
						}
						else
						{
							if (l_PPQId != pInspection->GetPPQIdentifier())	// not on same PPQ
							{
								hrOK = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
								// set the status of all objects to this error
								for (long i = 0; i < static_cast<long> (lNumberOfElements); i++)
								{
									/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &i, (void*)&hrOK);
								}
								break;
							}
						}
					}
				}
				else	// couldn't find data object
				{
					hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					l_bItemNotFound = true;
					ImageObjects.push_back(nullptr);
					/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &l, (void*)&hrOK);

					// Add nullptr to the outgoing array
					::SafeArrayPutElement(*ppsaImages, &l, nullptr);
					::SafeArrayPutElement(*ppsaOverlays, &l, bstrOverlay);
				}
			}// if (TheSVObserverApp.m_pConfiguration->GetInspectionObject(strName, &pInspection))
			else	// couldn't find inspection
			{
				hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				l_bInspectionNotFound = true;
				ImageObjects.push_back(nullptr);
				aInspections.push_back(nullptr);
				/*HRESULT hrTemp = */SafeArrayPutElement(*ppsaStatus, &l, (void*)&hrOK);
			}
		}// end for ( long l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )

		// requested values are not valid - return an error...
		if (!SUCCEEDED(hrOK))
		{
			throw hrOK;
		}

		// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
		SVProductInfoStruct ProductInfo;

		SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(l_PPQId));

		if (nullptr != pPPQ)
		{
			if (S_OK == pPPQ->GetProduct(ProductInfo, lProcessCount))
			{
				// Product is still valid; 
				// Build the output images from the result data index in the product and the source images by name  (?)
				for (l = 0; l < static_cast<long> (lNumberOfElements); l++)
				{
					HRESULT hrImage = S_OK;
					if (nullptr != aInspections[l])
					{
						bool bImageOk = false;

						SvIe::SVImageClass* pImage = ImageObjects[l];
						if (nullptr != pImage)
						{
							// put image in return array
							BSTR bstrTemp = nullptr;
							HRESULT hr = SafeImageToBSTR(pImage, ProductInfo.m_svInspectionInfos[aInspections[l]->getObjectId()].m_triggerRecordComplete, &bstrTemp);

							if (SUCCEEDED(hr))
							{
								// Add the status to the outgoing array
								HRESULT hrItem = S_OK;
								SafeArrayPutElement(*ppsaStatus, &l, &hrItem);

								// Add the image to the outgoing array
								SafeArrayPutElementNoCopy(*ppsaImages, &l, bstrTemp);

								bImageOk = true;
							}
							else
							{
								hrImage = SVMSG_INVALID_IMAGE_SOURCE;
							}
						}// end if ( nullptr != aImageObjects[i] )

						if (!bImageOk)
						{
							// put nullptr image in return array
							SafeArrayPutElement(*ppsaImages, &l, nullptr);

							if (S_OK == hrImage)
							{
								hrImage = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							}
							SafeArrayPutElement(*ppsaStatus, &l, &hrImage);
							l_bItemNotFound = true;
						}// end if ( !bImageOk )
					}// end if ( nullptr != aInspections[l] )
				}// end for (long i=0; i < lNumberOfElements; i++)
			}// end if (pPPQ->GetProductInfoStruct(lProcessCount, &pProductInfoStruct))
			else
			{
				hrResult = SVMSG_PRODUCT_NO_LONGER_AVAILABLE;	// product no longer alive
			}
		}// end if ( nullptr != pPPQ )

		if (l_bItemNotFound || l_bInspectionNotFound)
		{
			if (S_OK == hrResult)
			{
				hrResult = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}

	} // end try
	catch (HRESULT hr)
	{
		hrResult = hr;
	}
	catch (int hr)
	{
		hrResult = (HRESULT)hr;
	}
	catch (DWORD hr)
	{
		hrResult = (HRESULT)hr;
	}
	catch (...)
	{
		hrResult = -12338;
	}

	if (FAILED(hrResult))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVSetLUT(BSTR bstrCameraName, SAFEARRAY* paulLUTTable)
{
	HRESULT hr = S_OK;

	std::string CameraName = SvUl::createStdString(_bstr_t(bstrCameraName));
	SvIe::SVVirtualCamera* pCamera(nullptr);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && pConfig->GetChildObjectByName(CameraName.c_str(), &pCamera) && nullptr != pCamera)
	{
		VARTYPE vt = VT_EMPTY;
		::SafeArrayGetVartype(paulLUTTable, &vt);
		if (vt == VT_UI4 || vt == VT_I4)
		{
			SVLut lut;
			/*hr = */pCamera->GetLut(lut);

			// Anything from the ActiveX we will set to freeform
			lut.SetTransformOperation(SVLutTransformOperationFreeform());

			bool bResult = lut.SetBandData(paulLUTTable);
			if (bResult)
			{
				hr = pCamera->SetLut(lut);
			}
			else
			{
				hr = S_FALSE;
			}
		}
		else	// wrong type of data in safearray
		{
			hr = SVMSG_INCORRECT_ARRAY_DATA_TYPE;
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
	}

	return hr;
}

STDMETHODIMP SVCommand::SVGetLUT(BSTR bstrCameraName, SAFEARRAY** ppaulLUTTable)
{
	HRESULT hr = S_OK;

	std::string CameraName = SvUl::createStdString(_bstr_t(bstrCameraName));
	SvIe::SVVirtualCamera* pCamera(nullptr);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && pConfig->GetChildObjectByName(CameraName.c_str(), &pCamera) && nullptr != pCamera)
	{
		SVLut lut;

		hr = pCamera->GetLut(lut);

		lut.GetBandData(*ppaulLUTTable);
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
	}

	return hr;
}

HRESULT SVCommand::ImageToBSTR(const SVImageInfoClass& rImageInfo, SvOi::SVImageBufferHandlePtr rImageHandle, BSTR* pbstr)
{
	HRESULT hr = S_OK;

	if (nullptr == rImageHandle || rImageHandle->empty())
	{
		hr = -1578;
	}

	if (S_OK == hr)
	{
		SVImageInfoClass oChildInfo = rImageInfo;
		SvOi::SVImageBufferHandlePtr oChildHandle = rImageHandle;
		BITMAPINFOHEADER* pbmhInfo = nullptr;
		SVBitmapInfo l_BitmapInfo;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;
		bool IsColor(false);

		char* pDIB = nullptr;

		long l_lType = SvPb::SVImageTypeEnum::SVImageTypeUnknown;
		long l_lBandNumber = 1;
		long l_lBandLink = 0;

		SvIe::SVImageClass* pImage(nullptr);

		oChildInfo.GetOwnerImage(pImage);

		if (nullptr != pImage)
		{
			l_lType = pImage->GetImageType();
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pImage->GetAncestor(SvPb::SVInspectionObjectType));
			if (nullptr != pInspection)
			{
				IsColor = pInspection->IsColorCamera();
			}
		}

		oChildInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber);
		oChildInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, l_lBandLink);

		if (IsColor && l_lType == SvPb::SVImageTypeEnum::SVImageTypePhysical && l_lBandNumber == 3)
		{
			oChildInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);

			/*HRESULT hrImage =*/ SvIe::SVImageProcessingClass::CreateImageBuffer(oChildInfo, oChildHandle);

			if (nullptr != oChildHandle)
			{
				// Copy input image to output image
				hr = SVMatroxBufferInterface::CopyBuffer(oChildHandle->GetBuffer(), rImageHandle->GetBuffer(), l_lBandLink);
			}
			else
			{
				hr = E_FAIL;
			}
		}

		if (l_lType == SvPb::SVImageTypeEnum::SVImageTypeLogical)
		{
			/*HRESULT hrImage =*/ SvIe::SVImageProcessingClass::CreateImageBuffer(oChildInfo, oChildHandle);

			if (nullptr != oChildHandle)
			{
				// Copy input image to output image
				hr = SVMatroxBufferInterface::CopyBuffer(oChildHandle->GetBuffer(), rImageHandle->GetBuffer());
			}
			else
			{
				hr = E_FAIL;
			}
		}

		// Get the BITMAPINFO from MIL
		l_BitmapInfo = oChildHandle->GetBitmapInfo();
		if (l_BitmapInfo.empty())
		{
			hr = SVMatroxBufferInterface::GetBitmapInfo(l_BitmapInfo, nullptr != oChildHandle ? oChildHandle->GetBuffer() : SVMatroxBuffer());
		}

		pbmhInfo = &(l_BitmapInfo.GetBitmapInfo()->bmiHeader);

		// Source images seem to be flipped even though MIL is not supposed to flip them
		if (pbmhInfo->biHeight > 0)
		{
			l_BitmapInfo.FlipHeight();
		}

		// Calculate the absolute height
		lHeight = pbmhInfo->biHeight * ((pbmhInfo->biHeight < 0) ? -1 : 1);

		// Make sure image size is calculated
		if (0 == pbmhInfo->biSizeImage)
		{
			pbmhInfo->biSizeImage = ((((pbmhInfo->biWidth * pbmhInfo->biBitCount) + 31) & ~31) >> 3) * lHeight;
		}

		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if (0 == lNumColor)
		{
			if (pbmhInfo->biBitCount != 24)
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof(RGBQUAD);

		// Calculate total size buffer needed for image
		lBufSize = sizeof(BITMAPINFOHEADER) + lTabSize + pbmhInfo->biSizeImage;

		*pbstr = SysAllocStringByteLen(nullptr, lBufSize);
		if (nullptr == *pbstr)
		{
			hr = -1568;
		}
		else
		{
			//------ Convert from wide character pointer to character 
			//------ pointer. Doesn't matter for binary data.
			pDIB = (char*)(*pbstr);
		}

		if (nullptr == oChildHandle)
		{
			hr = E_FAIL;
		}

		if (S_OK == hr)
		{
			// Copy data to DIB memory locations
			memcpy(pDIB, l_BitmapInfo.GetBitmapInfo(), sizeof(BITMAPINFOHEADER) + lTabSize);
			memcpy(pDIB + sizeof(BITMAPINFOHEADER) + lTabSize,
				oChildHandle->GetBufferAddress(),
				pbmhInfo->biSizeImage);
		}

		//--- For the case where compression is not zero the pDIB value can no longer
		//--- be used because the Global Memory Block has been unlocked. When 
		//--- alCompression equals zero it is set to nullptr only for compatabillity. 
		//--- We no longer need this value for reference. We still have the BSTR       
		//--- reference.
		pDIB = nullptr;
	}

	return hr;
}

HRESULT SVCommand::SafeImageToBSTR(SvIe::SVImageClass* pImage, const SvOi::ITriggerRecordRPtr pTriggerRecord, BSTR* pbstr)
{
	HRESULT hr = S_OK;

	if (nullptr != pImage)
	{
		SVImageInfoClass oChildInfo = pImage->GetImageInfo();
		oChildInfo.setDibBufferFlag(true);

		SvOi::SVImageBufferHandlePtr oChildHandle;

		SvIe::SVImageProcessingClass::CreateImageBuffer(oChildInfo, oChildHandle);

		pImage->SafeImageCopyToHandle(oChildHandle, pTriggerRecord);

		hr = ImageToBSTR(oChildInfo, oChildHandle, pbstr);
	}
	else
	{
		hr = -1578;
	}

	return hr;
}

HRESULT SVCommand::SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind copy of the requested element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
	void* pElement;
	HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
	if (S_OK == hr)
	{
		memcpy(pv, pElement, psa->cbElements);
	}

	return hr;
}

HRESULT SVCommand::SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
// Does a blind put of the specified element.
// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
//      not the contents of the BSTR.
{
	void* pElement;
	HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
	if (S_OK == hr)
	{
		memcpy(pElement, &pv, psa->cbElements);
	}

	return hr;
}

HRESULT SVCommand::SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv)
// sets the pointer to point to the actual memory of the SAFEARRAY that contains the element.
{
	DWORD dwOffset = 0;
	DWORD dwIndexOffset = 1;
	for (int i = psa->cDims - 1; i >= 0; i--)
	{
		long lIndex = *(rgIndices + i);
		int iDimensionIndex = psa->cDims - 1 - i;
		if (lIndex < psa->rgsabound[iDimensionIndex].lLbound || lIndex >= (long)(psa->rgsabound[iDimensionIndex].lLbound + psa->rgsabound[iDimensionIndex].cElements))
		{
			return DISP_E_BADINDEX;
		}

		long dwIndex = (lIndex - psa->rgsabound[iDimensionIndex].lLbound);   // offset index in dimension used for figuring out memory offset

		dwOffset += dwIndex * dwIndexOffset;
		dwIndexOffset *= psa->rgsabound[iDimensionIndex].cElements;   // offset of dimension used in calculating next dim
	}
	dwOffset *= psa->cbElements;    // scale by size of element

	BYTE* pElement = ((BYTE*)psa->pvData) + dwOffset;
	*ppv = pElement;

	return S_OK;
}

HRESULT SVCommand::SVGetDataList(SAFEARRAY* psaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts)
{
	HRESULT Result = S_OK;
	HRESULT Status = S_OK;
	bool    ItemNotFound = false;
	bool    InspectionNotFound = false;

	//get number of elements out of the incoming safearray
	unsigned long Size = psaNames->rgsabound[0].cElements;

	assert(nullptr != ppsaStatus);
	assert(nullptr != *ppsaStatus);	// must provide allocated SafeArray(LONG)
	assert((*ppsaStatus)->rgsabound[0].cElements == Size);

	assert(nullptr != ppsaValues);
	assert(nullptr != *ppsaValues);	// must provide allocated SafeArray(LONG)
	assert((*ppsaValues)->rgsabound[0].cElements == Size);

	assert(nullptr != ppsaProcCounts);
	assert(nullptr != *ppsaProcCounts);// must provide allocated SafeArray(LONG)
	assert((*ppsaProcCounts)->rgsabound[0].cElements == Size);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	for (long l = 0; l < static_cast<long> (Size); l++)
	{
		std::string Value;
		SVInspectionProcess* pInspection(nullptr);
		Status = S_OK;

		// Get name of requested value out of the safearray
		BSTR bstrName = nullptr;
		SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		std::string Name = SvUl::createStdString(bstrName);
		long ProcessCount = -1;
		if (nullptr != pConfig)
		{
			pConfig->GetInspectionObject(Name.c_str(), &pInspection);
		}

		SVObjectReference ObjectRef;
		SVObjectManagerClass::Instance().GetObjectByDottedName(Name.c_str(), ObjectRef);

		if (nullptr != ObjectRef.getObject() || nullptr != pInspection)
		{
			//If inspection  is nullptr then object is of type BasicValueObject
			if (nullptr == pInspection)
			{
				SvVol::BasicValueObject* pValueObject = dynamic_cast<SvVol::BasicValueObject*>(ObjectRef.getObject());
				if (nullptr != pValueObject && !ObjectRef.isEntireArray())
				{
					if (S_OK != pValueObject->getValue(Value))
					{
						ItemNotFound = true;
					}
				}
				else
				{
					ItemNotFound = true;
				}
			}
			else
			{
				if (nullptr != ObjectRef.getObject())
				{
					ProcessCount = pInspection->getLastProductData().first.lTriggerCount;

					if (!ObjectRef.isEntireArray())
					{
						// was able to find the object
						HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, ObjectRef.getValidArrayIndex());
						if (S_OK == hrGet)
						{
							//got value
							Status = S_OK;
						}// if OK
						else if (hrGet == SVMSG_SVO_33_OBJECT_INDEX_INVALID
							|| hrGet == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE)
						{
							Status = hrGet;
							// did not get value.  set value to default
							Value = ObjectRef.DefaultValue();
							if (Value.empty())
							{
								Value = _T("-1");
							}
							ItemNotFound = true;
						}// else invalid or out of range index
						else
						{
							Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							// did not get value.  set value to -1
							Value = _T("-1");
							ItemNotFound = true;
						} //end else generic error
					}// if ( !ValueObjectRef.IsEntireArray() )
					else	// GET ENTIRE ARRAY
					{
						// get all results and put them into a parsable string
						int NumResults = ObjectRef.getValueObject()->getResultSize();
						std::string ArrayValues;
						for (int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++)
						{
							HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, iArrayIndex);
							if (S_OK == hrGet)
							{
								if (iArrayIndex > 0)
								{
									ArrayValues += _T(",");
								}
								ArrayValues += SvUl::Format(_T("`%s`"), Value.c_str());
							}
							else
							{
								break;
							}
						}

						Status = S_OK;
						Value = ArrayValues;
					}// end if ( !ValueObjectRef.IsEntireArray() ) else
				} // end if found object
				else
				{
					// could not find object
					Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					// did not get value.  set value to -1
					Value = _T("-1");
					ItemNotFound = true;
				} //else could not find object
			}// else inspection object
		}//end if object or inspection
		else
		{ //inspection not found
			Status = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			// did not get value.  set value to -1
			Value = _T("-1");
			InspectionNotFound = true;
		}// else inspection not found

		//Results have been prepared so can place them in the list
		_bstr_t bstrTmpVal = Value.c_str();
		SafeArrayPutElementNoCopy(*ppsaValues, &l, bstrTmpVal.Detach());
		::SafeArrayPutElement(*ppsaStatus, &l, &Status);
		::SafeArrayPutElement(*ppsaProcCounts, &l, &ProcessCount);
	}// end for

	if (ItemNotFound || InspectionNotFound)
	{
		Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}
	else
	{
		Result = Status;
	}

	return Result;
}

STDMETHODIMP SVCommand::SVRunOnce(BSTR bstrName)
{
	HRESULT              hrResult = S_FALSE;

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		SVInspectionProcess* pInspection(nullptr);
		if (SVConfigurationObject::GetInspection(SvUl::createStdString(bstrName).c_str(), pInspection))
		{
			hrResult = SvCmd::RunOnceSynchronous(pInspection->getObjectId());
		}
	}// end if
	else
	{
		//return an error.  not able to do runonce.
		hrResult = SVMSG_44_RUNONCE_ONLINE;
	}// end else

	return hrResult;
}

STDMETHODIMP SVCommand::SVSetSourceImage(BSTR bstrName, BSTR bstrImage)
{
	HRESULT                  hrResult = S_OK;

	if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION | SV_STATE_TEST | SV_STATE_RUNNING))
	{
		hrResult = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
		return hrResult;
	}

	SVInspectionProcess* pInspection(nullptr);
	if (SVConfigurationObject::GetInspection(SvUl::createStdString(bstrName).c_str(), pInspection))
	{
		SvIe::SVCameraImageTemplate* pMainImage = pInspection->GetToolSetMainImage();
		if (nullptr != pMainImage)
		{
			if (S_OK != pInspection->AddInputImageRequest(pMainImage, bstrImage))
			{
				hrResult = SVMSG_INVALID_IMAGE_SOURCE;
			}
		}
		else
		{
			hrResult = S_FALSE;
		}
	}
	else
	{
		hrResult = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}

	return hrResult;
}

STDMETHODIMP SVCommand::SVSetInputs(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY**)
{
	HRESULT result = (SVSVIMStateClass::CheckState(cDefaultNotAllowedStates) || SVSVIMStateClass::isSvrcBlocked()) ? SVMSG_SVO_ACCESS_DENIED : S_OK;

	if (S_OK != result)
	{
		return result;
	}

	HRESULT                Status = S_OK;

	long lNumberOfElements1 = psaNames->rgsabound[0].cElements;
	long lNumberOfElements2 = psaValues->rgsabound[0].cElements;

	if ((lNumberOfElements1 == 0) || (lNumberOfElements1 != lNumberOfElements2))
	{
		result = SVMSG_TOO_MANY_REQUESTED_ITEMS;
		return result;
	}

	SvDef::StringSet		ParameterNames;
	SVNameStorageMap		ParameterObjects;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	for (long l = 0; l < lNumberOfElements1; l++)
	{
		BSTR bstrName = nullptr;
		Status = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		if (FAILED(Status)) { break; }

		std::string Name = SvUl::createStdString(bstrName);

		BSTR bstrValue = nullptr;
		Status = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
		if (FAILED(Status)) { break; }

		std::string Value = SvUl::createStdString(bstrValue);

		//!Check if an inspection name
		SVInspectionProcess* pInspection(nullptr);
		if (nullptr != pConfig && pConfig->GetInspectionObject(Name.c_str(), &pInspection))
		{
			//If Remote inputs then the name must be replaced otherwise add "Inspections." to name
			constexpr const char* FqnRemoteInput = _T(".Remote Input");
			if (std::string::npos != Name.find(FqnRemoteInput))
			{
				SvUl::searchAndReplace(Name, pInspection->GetName(), SvDef::FqnRemoteInputs);
			}
			else if (std::string::npos != Name.find(pInspection->GetName()))
			{
				std::string Prefix{ SvDef::FqnInspections };
				Prefix += '.';
				Name = Prefix + Name;
			}
		}

		SVStorage	Parameter;
		SvUl::SVSAFEARRAY	Values;

		//Values must be in array format
		Values.Add(_variant_t(bstrValue));
		Parameter.m_StorageType = SVVisionProcessor::SVStorageValue;
		Parameter.m_Variant = Values.operator _variant_t();
		ParameterObjects[std::string(Name)] = Parameter;
	}// end for	

	result = Status;

	if (S_OK == result && 0 != ParameterObjects.size())
	{
		SVNameStatusMap SetItemsResult;
		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
		result = SVVisionProcessorHelper::Instance().SetItems(ParameterObjects, SetItemsResult, false);
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	}

	return result;
}

HRESULT SVCommand::SVSetImageList(SAFEARRAY* psaNames, SAFEARRAY* psaImages, SAFEARRAY** ppsaStatus)
{
	HRESULT hr = S_OK;
	HRESULT hrStatus = S_OK;
	SVInspectionProcess* pInspection(nullptr);
	SvIe::SVImageClass* pImage(nullptr);
	BSTR bstrName = nullptr;
	BSTR bstrImage = nullptr;
	std::string TmpName;
	bool l_bItemNotFound = false;

	do
	{
		long lNumberOfElements1 = psaNames->rgsabound[0].cElements;
		long lNumberOfElements2 = psaImages->rgsabound[0].cElements;

		if ((lNumberOfElements1 == 0) || (lNumberOfElements1 != lNumberOfElements2))
		{
			hr = SVMSG_TOO_MANY_REQUESTED_ITEMS;
			break;
		}// end if

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		for (long l = 0; l < lNumberOfElements1; l++)
		{
			bool bAddRequest = false;

			hrStatus = ::SafeArrayGetElement(psaNames, &l, &bstrName);
			if (FAILED(hrStatus)) { break; }

			TmpName = SvUl::createStdString(_bstr_t(bstrName));
			::SysFreeString(bstrName);

			hrStatus = SafeArrayGetElementNoCopy(psaImages, &l, &bstrImage);
			if (FAILED(hrStatus)) { break; }

			//GetInspectionObject is only true if the pointer is valid
			if (nullptr != pConfig && pConfig->GetInspectionObject(TmpName.c_str(), &pInspection) && nullptr != pInspection)
			{
				SVObjectClass* l_pObject = nullptr;
				//got the inspection.
				if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(TmpName.c_str(), l_pObject))
				{
					pImage = dynamic_cast<SvIe::SVImageClass*>(l_pObject);

					if (nullptr != pImage &&
						(pImage->ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable)
					{
						// currently all SvPb::remotelySetable parameters are also SvPb::setableOnline
						// if this changes, this code needs updated
						bAddRequest = true;
					}// end if
					else
					{
						// Item is not allowed to be set remotely
						hrStatus = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;
						::SafeArrayPutElement(*ppsaStatus, &l, &hrStatus);
					}// end else
				}// end if
				else
				{
					// object not found.  send back status
					hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus, &l, &hrStatus);
					l_bItemNotFound = true;
				}// end else
			}// end if
			else
			{
				//did not find inspection.
				//put an error back into the list
				hrStatus = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus, &l, &hrStatus);
				l_bItemNotFound = true;
			} // end else

			if (bAddRequest)
			{
				SVImageInfoClass l_ImageInfo;

				SVInputImageRequestInfoStruct* pInRequest = new SVInputImageRequestInfoStruct;
				pInRequest->m_ObjectName = TmpName;

				l_ImageInfo = pImage->GetImageInfo();

				if (S_OK == SvIe::SVImageProcessingClass::LoadImageBuffer((void*)bstrImage,
					pInRequest->m_ImageInfo,
					pInRequest->m_ImageHandlePtr,
					l_ImageInfo))
				{
					//add request to inspection process
					pInspection->AddInputImageRequest(SVInputImageRequestInfoStructPtr{ pInRequest });
				}
				else
				{
					//error
					l_bItemNotFound = true;
					delete pInRequest;
				}
			}
		}
	} while (0);

	hr = hrStatus;
	if (l_bItemNotFound)
	{
		hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}

	return hr;
}

HRESULT SVCommand::SVSetToolParameterList(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY**)
{
	HRESULT result = (SVSVIMStateClass::CheckState(cDefaultNotAllowedStates) || SVSVIMStateClass::isSvrcBlocked()) ? SVMSG_SVO_ACCESS_DENIED : S_OK;

	if (S_OK != result)
	{
		return result;
	}

	HRESULT Status = S_OK;

	long NumberOfElements = psaNames->rgsabound[0].cElements;
	long NumberOfElementsValues = psaValues->rgsabound[0].cElements;

	if (0 == NumberOfElements)
	{
		result = SVMSG_REQUESTED_LIST_IS_EMPTY;
		return result;
	}
	else if (NumberOfElements != NumberOfElementsValues)
	{
		result = SVMSG_TOO_MANY_REQUESTED_ITEMS;
		return result;
	}

	SVConfigurationObject* pConfig = nullptr;
	SVNameStorageMap		ParameterObjects;

	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	for (long l = 0; l < NumberOfElements; l++)
	{
		BSTR bstrName = nullptr;
		Status = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		if (FAILED(Status)) { break; }

		std::string Name = SvUl::createStdString(bstrName);

		BSTR bstrValue = nullptr;
		Status = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
		if (FAILED(Status)) { break; }

		std::string Value = SvUl::createStdString(bstrValue);

		//!Check if an inspection name
		SVInspectionProcess* pInspection(nullptr);
		if (nullptr != pConfig && pConfig->GetInspectionObject(Name.c_str(), &pInspection) && nullptr != pInspection)
		{
			if (std::string::npos != Name.find(pInspection->GetName()))
			{
				std::string Prefix{ SvDef::FqnInspections };
				Prefix += '.';
				Name = Prefix + Name;
			}
		}
		SVStorage	Parameter;
		SvUl::SVSAFEARRAY	Values;

		//Values must be in array format
		Values.Add(_variant_t(bstrValue));
		Parameter.m_StorageType = SVVisionProcessor::SVStorageValue;
		Parameter.m_Variant = Values.operator _variant_t();
		ParameterObjects[std::string(Name)] = Parameter;
	}// end for ( long l = 0; l < lNumberOfElements; l++ )
	result = Status;

	if (S_OK == result && 0 != ParameterObjects.size())
	{
		SVNameStatusMap SetItemsResult;
		SVSVIMStateClass::AddState(SV_STATE_REMOTE_CMD);
		result = SVVisionProcessorHelper::Instance().SetItems(ParameterObjects, SetItemsResult, false);
		SVSVIMStateClass::RemoveState(SV_STATE_REMOTE_CMD);
	}

	return result;
}

HRESULT SVCommand::SVLockImage(long, long, BSTR)
{
	return E_FAIL;
}

HRESULT SVCommand::SVGetLockedImage(long, long, BSTR*, BSTR*)
{
	return E_FAIL;
}

HRESULT SVCommand::SVUnlockImage(long)
{
	return E_FAIL;
}

HRESULT SVCommand::SVUnlockAllImages()
{
	return E_FAIL;
}

STDMETHODIMP SVCommand::SVGetRemoteInputCount(long* pCount)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig && nullptr != pCount)
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
			if (nullptr != pInputObjectList)
			{
				*pCount = pInputObjectList->getRemoteInputCount();
				bSuccess = true;
			}
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetRemoteInputCount

STDMETHODIMP SVCommand::SVSetRemoteInput(long lIndex, VARIANT vtValue)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
			if (nullptr != pInputObjectList && pInputObjectList->SetRemoteInput(lIndex, _variant_t(vtValue)))
			{
				bSuccess = true;
			}
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = SVMSG_CMDCOMSRV_ERROR;
	}

	return hrResult;
}// end SVSetRemoteInput

// This method is used to connect the event object to the application.
HRESULT SVCommand::StoreEventObserver(DWORD, CComPtr< SVCommand > p_pObserver)
{
	return S_OK;
}

// This method is used to disconnect the event object to the application.
HRESULT SVCommand::ReleaseEventObserver(DWORD, CComPtr< SVCommand > p_pObserver)
{
	return S_OK;
}

// Stub for SVGetTransferValueDefinitionList
STDMETHODIMP SVCommand::SVGetTransferValueDefinitionList(BSTR bstrInspectionName,
	long*,
	VARIANT* p_pvData)
{
	if (nullptr == p_pvData)
	{
		return S_FALSE;
	}

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection(nullptr);

	if (SVConfigurationObject::GetInspection(SvUl::createStdString(bstrInspectionName).c_str(), pInspection))
	{
		// Get Data Definition list from inspection
		SVToolSet* pToolSet = pInspection->GetToolSet();
		SvIe::SVTaskObjectListClass* pTaskObjectList = static_cast <SvIe::SVTaskObjectListClass*> (pToolSet);

		std::vector<SvOi::IObjectClass*> selectedObjects;
		pTaskObjectList->getOutputList(std::back_inserter(selectedObjects));

		selectedObjects.erase(std::remove_if(selectedObjects.begin(), selectedObjects.end(), [](auto* pObject) 
			{return nullptr == dynamic_cast<SvOi::IValueObject*> (pObject) || 0 == (pObject->ObjectAttributesSet() & SvPb::dataDefinitionValue); }), selectedObjects.end());

		// Copy list to Safearray

		// Get Attributes and add to lists.
		// 1st Version of Data Transfer Definitions....January 2008..TRB

		SAFEARRAYBOUND l_saBounds[2];

		// First Dimension number of objects in list..
		l_saBounds[0].cElements = static_cast<ULONG>(selectedObjects.size());
		l_saBounds[0].lLbound = 0;

		// Second Dimension is the parts fo the Transfer Definition
		// 0 = Name, 1 = Writable, 2 = Data Type, 3 = EnumList
		l_saBounds[1].cElements = 4;
		l_saBounds[1].lLbound = 0;

		SAFEARRAY* l_psaData;	//( VT_VARIANT, l_saBounds);
		l_psaData = ::SafeArrayCreate(VT_VARIANT, 2, &l_saBounds[0]);
		long  l_Index[2];
		for (size_t i = 0; i < selectedObjects.size(); ++i)
		{
			l_Index[0] = static_cast<long>(i);
			// Name
			l_Index[1] = 0;
			_variant_t Value;
			Value.SetString(selectedObjects[i]->GetCompleteName().c_str());
			/*hr = */::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (selectedObjects[i]->ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = l_bWritable;
			/*hr = */::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Data Type
			l_Index[1] = 2;
			Value.Clear();
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (selectedObjects[i]);
			if (nullptr != pValueObject)
			{
				Value.SetString(pValueObject->getTypeName().c_str());
			}
			/*hr = */::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Enumeration List.
			l_Index[1] = 3;
			Value.Clear();
			if (selectedObjects[i]->GetObjectSubType() == SvPb::SVEnumValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SvVol::SVEnumerateValueObjectClass* pEnumVO = dynamic_cast<SvVol::SVEnumerateValueObjectClass*>(selectedObjects[i]);
				if (nullptr != pEnumVO)
				{
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast<ULONG>(pEnumVO->GetEnumVector().size());
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY* l_psaTemp = SafeArrayCreate(VT_BSTR, 1, l_rgsabound);
					for (long j = 0; j < static_cast<long>(pEnumVO->GetEnumVector().size()); ++j)
					{
						_bstr_t bstTmp = pEnumVO->GetEnumVector()[j].first.c_str();
						SafeArrayPutElement(l_psaTemp, &j, bstTmp.Detach());
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;
				}
			}
			else if (selectedObjects[i]->GetObjectSubType() == SvPb::SVBoolValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SvVol::SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<SvVol::SVBoolValueObjectClass*>(selectedObjects[i]);
				if (nullptr != l_pBoolVO)
				{
					SvDef::StringVector ValidTypes;
					l_pBoolVO->GetValidTypes(ValidTypes);
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast<ULONG>(ValidTypes.size());
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY* l_psaTemp = SafeArrayCreate(VT_BSTR, 1, l_rgsabound);
					for (long j = 0; j < static_cast<long>(ValidTypes.size()); ++j)
					{
						_bstr_t bstTmp = ValidTypes[j].c_str();
						SafeArrayPutElement(l_psaTemp, &j, bstTmp.GetBSTR());
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;

				}
			}
			hr = ::SafeArrayPutElement(l_psaData, l_Index, &Value);
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if (nullptr != p_pvData)
		{
			hr = VariantClear(p_pvData);
		}
		_variant_t Value;
		Value.vt = VT_ARRAY | VT_VARIANT;
		Value.parray = l_psaData;
		*p_pvData = Value.Detach();
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

// Stub for SVGetTransferImageDefinitionList
STDMETHODIMP SVCommand::SVGetTransferImageDefinitionList(BSTR bstrInspectionName,
	long*,
	VARIANT* p_pvData)
{
	if (nullptr == p_pvData)
	{
		return SVMSG_INVALID_VARIANT_PARAMETER;
	}

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection(nullptr);

	if (SVConfigurationObject::GetInspection(SvUl::createStdString(bstrInspectionName).c_str(), pInspection))
	{
		// Get Image list from the tool set.
		SvIe::SVImageClassPtrVector ImageList;
		SVToolSet* pToolSet = pInspection->GetToolSet();
		pToolSet->GetImageList(ImageList);

		std::vector<SvIe::SVImageClass*> objectList;

		int nCount = static_cast<int>(ImageList.size());
		for (int i = 0; i < nCount; i++)
		{
			SvIe::SVImageClass* pImage = ImageList[i];

			if (nullptr != pImage)
			{
				//
				// Check for the required Output object attributes.
				//
				if ((pImage->ObjectAttributesSet() & SvPb::dataDefinitionImage) != 0)
				{
					objectList.push_back(pImage);
				}
			}
		}

		// Copy list to Safearray

		SAFEARRAYBOUND l_saBounds[2];

		// First Dimension number of objects in list..
		l_saBounds[0].cElements = static_cast<ULONG>(objectList.size());
		l_saBounds[0].lLbound = 0;

		// Second Dimension is the parts fo the Transfer Definition
		// 0 = Name, 1 = Writable, 2 = Result is Published, 3 = Source Image Name
		l_saBounds[1].cElements = 4;
		l_saBounds[1].lLbound = 0;

		SAFEARRAY* l_psaData; //( VT_VARIANT, l_saBounds);
		l_psaData = ::SafeArrayCreate(VT_VARIANT, 2, l_saBounds);
		long l_Index[2];
		for (size_t i = 0; i < objectList.size(); i++)
		{
			l_Index[0] = static_cast<long>(i);
			// Name
			l_Index[1] = 0;
			_variant_t Value;
			Value.SetString(objectList[i]->GetCompleteName().c_str());
			/*hr =*/ ::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (objectList[i]->ObjectAttributesAllowed() & SvPb::remotelySetable) == SvPb::remotelySetable;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = l_bWritable;
			/*hr = */::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Published Image
			l_Index[1] = 2;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = (objectList[i]->ObjectAttributesSet() & SvPb::publishResultImage) != 0;
			//l_saData.PutElement( l_Index, l_vTmp );
			/*hr = */::SafeArrayPutElement(l_psaData, l_Index, &Value);

			// Fully Qualified Source Image Name
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (objectList[i]->GetTool());
			l_Index[1] = 3;
			Value.Clear();
			if (nullptr != pTool)
			{
				SvVol::SVStringValueObjectClass* pSourceNames = pTool->GetInputImageNames();
				if (nullptr != pSourceNames)
				{
					SAFEARRAYBOUND l_rgsabound[1];
					long l_lSize = pSourceNames->getArraySize();
					l_rgsabound[0].cElements = l_lSize;
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY* l_psaTemp = SafeArrayCreate(VT_BSTR, 1, l_rgsabound);
					for (long l_lIndex = 0; l_lIndex < l_lSize; l_lIndex++)
					{
						std::string strTmp;
						/*HRESULT l_hr =*/ pSourceNames->getValue(strTmp, l_lIndex);
						_bstr_t bstTmp = strTmp.c_str();
						SafeArrayPutElement(l_psaTemp, &l_lIndex, bstTmp.GetBSTR());
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;
				}
			}
			hr = ::SafeArrayPutElement(l_psaData, l_Index, &Value);
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if (nullptr != p_pvData)
		{
			hr = VariantClear(p_pvData);
		}
		_variant_t Value;
		Value.vt = VT_ARRAY | VT_VARIANT;
		Value.parray = l_psaData;
		*p_pvData = Value.Detach();
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP SVCommand::SVIsAvailiable()
{
	return S_OK;
}

