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
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>

#include "SVObserver.h"

#include "SVObserver_i.h"
#include "SVCommand.h"

#include "SVLibrary/DirectoryUtilities.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVCommandLibrary/SVFileSystemCommandFactory.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVMatroxLibrary/SVMatroxCommandFactory.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectNameInfo.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"

#include "SVInfoStructs.h"
#include "SVIPDoc.h"
#include "SVRemoteCommand.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "InspectionEngine/SVImageBuffer.h"	//SVImageOverlayClass
#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVInputObjectList.h"
#include "SVStreamCommandFactory.h"
#include "SVInputStreamManager.h"
#include "InspectionEngine/SVTool.h"
#include "SVCommandStreamManager.h"
#include "SVCommandInspectionCollectImageData.h"

#include "SVOLicenseManager.h"
#include "RemoteCommand.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "SVStorageResult.h"
#include "SVVisionProcessorHelper.h"
#include "RangeClassHelper.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

volatile bool CSVCommand::m_bRunStreamData = false;
volatile bool CSVCommand::m_bRegisteredStream = false;
volatile DWORD CSVCommand::m_dwStreamDataProcessId = 0;
volatile long CSVCommand::m_lLastStreamedProduct = -1;
volatile HANDLE CSVCommand::m_hStopStreamEvent = nullptr;
volatile HANDLE CSVCommand::m_hStreamingThread = nullptr;
SvDef::StringVector CSVCommand::m_InspectionNames;

std::vector<SVActiveXLockStruct> CSVCommand::m_aSVActXLock;
std::vector<StreamDataStruct*> CSVCommand::m_arStreamList;
std::vector<ProductDataStruct*> CSVCommand::m_arProductList;
#pragma endregion Declarations

#pragma region Constructor
CSVCommand::CSVCommand()
	: m_pStream( nullptr )
{

}

CSVCommand::~CSVCommand()
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

STDMETHODIMP CSVCommand::SVGetSVIMState(unsigned long *ulState)
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
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetSVIMState

STDMETHODIMP CSVCommand::SVSetSVIMMode(unsigned long lSVIMNewMode)
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
			hrResult = GlobalRCSetMode( lSVIMNewMode );
		}
		catch (...)
		{
			hrResult = S_FALSE;
		}

		if( !(S_OK == hrResult) )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	} while (0);

	return hrResult;
}// end SVSetSVIMMode

STDMETHODIMP CSVCommand::SVGetSVIMMode(unsigned long* p_plSVIMMode)
{
	HRESULT hrResult;

	hrResult = S_OK;

	try
	{
		hrResult = GlobalRCGetMode( p_plSVIMMode );
	}
	catch (...)
	{
		hrResult = S_FALSE;
	}

	if( S_OK != hrResult )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
	}

	return hrResult;
}// end SVGetSVIMMode

STDMETHODIMP CSVCommand::SVSetSVIMState(unsigned long ulSVIMState)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	if( SVSVIMStateClass::CheckState( SV_STATE_EDITING ) )
	{
		hrResult = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else
	{
		try
		{
			if( ulSVIMState )
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

		if( !bSuccess )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}
	}

	return hrResult;
}// end SVSetSVIMState

STDMETHODIMP CSVCommand::SVGetSVIMConfig( long lOffset, long *lBlockSize, BSTR *bstrFileData, BOOL *bLastFlag)
{
	std::string PackedFileName;
	std::string ConfigName;
	SVPackedFile PackedFile;
	CFile binFile;
	CFileStatus Status;
	long lSize = 0;
	long lBytesRead = 0;
	long lFileSize = 0;
	long lBytesToGo = 0;
	CFileException *ex;
	bool bHrSet = false;
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		ConfigName = GlobalRCGetConfigurationName();

		bSuccess = !ConfigName.empty();
		if (bSuccess)
		{
			PackedFileName = ConfigName + _T(".svf");
		}

		// check offset: if zero then it is first time in
		if (lOffset < 1)
		{
			// on first time
			// pack files to temp area before sending packets

			if (bSuccess)
			{
				GlobalRCSaveConfiguration();
				bSuccess = PackedFile.PackFiles(ConfigName.c_str(), PackedFileName.c_str());
			}
		}//offset < 1  end of the fist time

		//send data back to control

		if (bSuccess)
		{
			ex = new CFileException;
			if (binFile.Open(PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, ex))
			{
				lFileSize = (long)binFile.GetLength();
				lBytesToGo = lFileSize - lOffset;
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
	catch (CMemoryException *memEx)
	{
		TCHAR szCause[255];
		memEx->GetErrorMessage(szCause, 255);
#ifdef _DEBUG
		std::string Formatted = _T("ERROR - ");
		Formatted += szCause;
		ASSERT(false);// szFormatted
#endif
		hrResult = SVMSG_CMDCOMSRV_MEMORY_ERROR;
		bHrSet = true;
		memEx->Delete();
	}
	catch (CFileException *ex)
	{
		TCHAR szCause[255];
		ex->GetErrorMessage(szCause, 255);
#ifdef _DEBUG
		std::string Formatted = _T("ERROR - ");
		Formatted += szCause;
		ASSERT(false);//szFormatted
#endif
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
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetSVIMConfig

STDMETHODIMP CSVCommand::SVPutSVIMConfig(long lOffset, long lBlockSize, BSTR *bstrFileData, BOOL bLastFlag)
{
	std::string configFileName;
	std::string PackedFileName;
	SVPackedFile svPackedFile;
	HRESULT hrResult = S_OK;
	CFile binFile;
	bool bRet = false;
	CFileException *ex;
	bool bHrSet = false;

	// Check if we are in an allowed state first
	// Not allowed to perform if Mode is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVSVIMStateClass::AddState( SV_STATE_REMOTE_CMD );

	bool bSuccess = false;

	try
	{
		if (CreateDirPath(SvStl::GlobalPath::Inst().GetTempPath().c_str()))
		{
			PackedFileName = SvStl::GlobalPath::Inst().GetTempPath(_T("temp.svf"));
			ex = new CFileException;
			if (lOffset < 1)
			{
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
				binFile.Write(*bstrFileData, lBlockSize);
				binFile.Close();
				ex->Delete();
				bSuccess = true;
			}
		}

		if (bLastFlag)
		{
			// make sure file exists
			bSuccess = (0 == _access(PackedFileName.c_str(), 0));

			// global function to close config and clean up c:\run dir
			GlobalRCCloseAndCleanConfiguration();

			if (bSuccess)
			{
				//unpack the files in the c:\run directory
				bSuccess = svPackedFile.UnPackFiles(PackedFileName.c_str(), SvStl::GlobalPath::Inst().GetRunPath().c_str());
				if (!bSuccess)
				{
					hrResult = SVMSG_CMDCOMSRV_PACKEDFILE_ERROR;
					bHrSet = true;
					SvStl::MessageMgrStd Exception(SvStl::LogOnly);
					Exception.setMessage(hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
				}
			}

			if (bSuccess)
			{
				// check for a good path on the config in the packed file
				configFileName = svPackedFile.getConfigFilePath();
				bSuccess = !(configFileName.empty());
			}

			if (bSuccess)
			{
				// make sure file exists
				bSuccess = (0 == _access(configFileName.c_str(), 0));
			}

			if (bSuccess)
			{
				//load the config
				bSuccess = GlobalRCOpenConfiguration(configFileName.c_str());
			}
		}
	}
	catch (CFileException *theEx)
	{
		TCHAR szCause[255];
		theEx->GetErrorMessage(szCause, 255);
#ifdef _DEBUG
		std::string Formatted = _T("Error - ");
		Formatted += szCause;
		ASSERT(false);//sFormatted
#endif
		hrResult = theEx->m_lOsError;
		theEx->Delete();
	}
	catch (...)
	{
		bSuccess = false;
	}

	if ((!bSuccess) && (!bHrSet))
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	SVSVIMStateClass::RemoveState( SV_STATE_REMOTE_CMD );

	return hrResult;
}// end SVPutSVIMConfig

STDMETHODIMP CSVCommand::SVGetSVIMFile(BSTR bstrSourceFile, long lOffset, long *lBlockSize, BSTR *bstrFileData, long *lLastPacketFlag)
{
	HRESULT hrResult = S_OK;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];

	CString strFilename( bstrSourceFile, ::SysStringLen( bstrSourceFile ) );

	_tsplitpath( strFilename, szDrive, szDir, szFile, szExt );

	if( !_tcscmp( szDrive, _T("") ) )
	{
		_tcscpy( szDrive, _T("C:") );
		_tcscpy( szDir, _T("\\Run\\") );
	}

	_tmakepath( szPath, szDrive, szDir, szFile, szExt );

	if( ::_access( szPath, 0 ) == 0 )
	{
		CFile binFile;
		CFileException l_FileException;

		if( binFile.Open( szPath, CFile::shareDenyNone | CFile::modeReadWrite | CFile::typeBinary, &l_FileException ) )
		{
			try
			{
				long lFileSize = (long)binFile.GetLength();
				long lBytesToGo = lFileSize - lOffset;
				long lCurrentBytesRead = lOffset;

				if ( (*lBlockSize > lBytesToGo) || (*lBlockSize < 1) )
				{
					*lBlockSize = lBytesToGo;
					*lLastPacketFlag = true;
				}

				*bstrFileData = SysAllocStringByteLen( nullptr, *lBlockSize );
				if (nullptr == *bstrFileData)
				{
					AfxThrowMemoryException( );
				}

				// Transfer the file data into the buffer
				binFile.Seek( lOffset, CFile::begin );

				binFile.Read( *bstrFileData, *lBlockSize );

				// Close the file
				binFile.Close();
			}
			catch(CFileException* theException)
			{
#ifdef _DEBUG
				TCHAR   szCause[255];
				theException->GetErrorMessage(szCause, 255);
				std::string Formatted = _T("ERROR - ");
				Formatted += szCause;
#endif

				hrResult = SVMSG_CMDCOMSRV_FILE_ERROR;

				theException->Delete();
			}
			catch (CMemoryException* memEx)
			{
#ifdef _DEBUG
				TCHAR szCause[255];
				memEx->GetErrorMessage(szCause,255);
				std::string Formatted = _T("ERROR - ");
				Formatted += szCause;
#endif

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

		if( ! SUCCEEDED( hrResult ) )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		hrResult = ERROR_FILE_NOT_FOUND;
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SVPutSVIMFile(BSTR bstrDestFile, long lOffset, long lBlockSize, BSTR *bstrFileData)
{
	HRESULT hrResult = S_OK;

	std::string Path;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];

	CString strFilename( bstrDestFile, ::SysStringLen( bstrDestFile ) );

	_tsplitpath( strFilename, szDrive, szDir, szFile, szExt );

	if( !_tcscmp( szDrive, _T("") ) )
	{
		_tcscpy( szDrive, _T("C") );
		_tcscpy( szDir, _T("\\Run\\") );
	}

	_tmakepath( szPath, szDrive, szDir, szFile, szExt );

	Path = szDrive;
	Path += szDir;

	CreateDirPath( Path.c_str() );

	CFile binFile;
	CFileException l_FileException;

	BOOL l_FileOpen = false;

	if( lOffset < 1 )
	{
		l_FileOpen = binFile.Open( szPath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, &l_FileException );
	}
	else
	{
		l_FileOpen = binFile.Open( szPath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeBinary, &l_FileException );
	}

	if( l_FileOpen )
	{
		try
		{
			binFile.Seek( lOffset, CFile::begin );

			binFile.Write( *bstrFileData, lBlockSize );
		}
		catch (CFileException *theEx)
		{
#ifdef _DEBUG
			TCHAR szCause[255];
			theEx->GetErrorMessage(szCause,255);
			std::string Formatted = _T("ERROR - ");
			Formatted += szCause;
#endif

			hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;

			theEx->Delete();

			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
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

STDMETHODIMP CSVCommand::SVLoadSVIMConfig(BSTR bstrConfigFilename)
{
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	std::string ConfigFile;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];
	CFileException ex;

	// Check the mode first - Not allowed to perform if Mode is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVSVIMStateClass::AddState( SV_STATE_REMOTE_CMD );

	bool bHrSet = false;
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		ConfigFile = SvUl::createStdString(_bstr_t(bstrConfigFilename));

		//split filename into peices
		_tsplitpath(ConfigFile.c_str(), szDrive, szDir, szFile, szExt);

		if (!_tcscmp(szDrive, _T("")))
		{ //just the file name, search the run directory for the filename
			if (0 == _tcscmp(szExt, _T(".svx")) || 0 == _tcscmp(szExt, _T("")))
			{
				_tmakepath(szPath, _T("C"), _T("\\Run\\"), szFile, _T("svx"));
				//check for existence of file first
				bSuccess = (0 == _access(szPath, 0));

				if (bSuccess)
				{
					ConfigFile = szPath;
					//global function to close config and clean up c:\run dir
					GlobalRCCloseAndCleanConfiguration();
				}
			}
			else
			{
				bSuccess = false;
			}
		}
		else if (0 == _tcscmp(szExt, _T(".svx"))) //fully qualified path with svx extension
		{
			//check for existence of file first
			bSuccess = (0 == _access(ConfigFile.c_str(), 0));

			if (bSuccess)
			{
				//global function to close config and clean up c:\run dir
				GlobalRCCloseAndCleanConfiguration();
			}
			else
			{
				throw ((CFileException*)(&ex));
			}
		}
		else
		{
			bSuccess = false;
		}

		if (bSuccess)
		{
			bSuccess = GlobalRCOpenConfiguration(ConfigFile.c_str());
		}
	}
	catch (CFileException* &theEx)
	{
		hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;
		bHrSet = true;
		Exception.setMessage(hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		theEx->Delete();
	}
	catch (...)
	{
		bSuccess = false;
	}

	if ((!bSuccess) && (!bHrSet))
	{
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	SVSVIMStateClass::RemoveState( SV_STATE_REMOTE_CMD );

	return hrResult;
}


STDMETHODIMP CSVCommand::SVGetSVIMConfigName(BSTR *bstrConfigFilename)
{
	HRESULT Result( S_OK );

	try
	{
		std::string ConfigName = GlobalRCGetConfigurationName();

		Result = ConfigName.empty() ? E_FAIL : S_OK;

		if( S_OK == Result )
		{
			*bstrConfigFilename = _bstr_t(ConfigName.c_str()).Detach();
		}
	}
	catch (...)
	{
		Result = E_FAIL;
	}

	if( S_OK != Result )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
	}

	return Result;
}

STDMETHODIMP CSVCommand::SVGetSVIMOfflineCount(unsigned long *ulOfflineCount)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		*ulOfflineCount = TheSVObserverApp.getOfflineCount();
		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMVersion(unsigned long *ulVersion)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		*ulVersion = TheSVObserverApp.getCurrentVersion();
		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	if (!bSuccess)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMConfigPrint(long lOffset, long *lBlockSize, BSTR *bstrConfigPrint, BOOL *bLastFlag)
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
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		hrResult = S_FALSE;
	}

	return hrResult;
}

struct SVGetImageListImageInfo
{
	_bstr_t m_ImageName;
	SVGUID m_ImageId;
	SVInspectionProcess* m_pInspection;

	SVGetImageListImageInfo() : m_ImageName(), m_ImageId(), m_pInspection( nullptr ) {}

	SVGetImageListImageInfo( const _bstr_t& p_rImageName, const SVGUID& p_rImageId, SVInspectionProcess* p_pInspection )
		: m_ImageName( p_rImageName ), m_ImageId( p_rImageId ), m_pInspection( p_pInspection ) {}
};


// Currently used only through external DCOM connection.
STDMETHODIMP CSVCommand::SVGetImageList(SAFEARRAY* psaNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts)
{
	HRESULT hrResult = S_OK;

	typedef std::map< SVInspectionProcess*, SVGuidSet > SVInspectionImageIdMap;
	typedef std::map< SVInspectionProcess*, SVCommandInspectionCollectImageDataPtr > SVInspectionImageDataMap;
	typedef std::deque< SVGetImageListImageInfo > SVImageNameIdDeque;

	try
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr == pConfig || !pConfig->IsConfigurationLoaded() )
		{
			throw SVMSG_CONFIGURATION_NOT_LOADED;
		}

		long lNumberOfElements = psaNames->rgsabound[0].cElements;

		ASSERT( nullptr != ppsaStatus );
		ASSERT( nullptr != *ppsaStatus );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( nullptr != ppsaImages );
		ASSERT( nullptr != *ppsaImages );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaImages)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( nullptr != ppsaOverlays );
		ASSERT( nullptr != *ppsaOverlays );// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( nullptr != ppsaProcCounts );
		ASSERT( nullptr != *ppsaProcCounts );// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaProcCounts)->rgsabound[0].cElements == lNumberOfElements );

		long lDefaultStatus = S_FALSE;

		// set default status for objects in list
		for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
		{
			HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
		}

		bool l_bItemNotFound = false;
		bool l_bInspectionNotFound = false;

		SVInspectionImageIdMap l_InspectionImageIds;
		SVImageNameIdDeque l_ImageNameIds;

		//go through list of names and make sure they are all valid
		// 1) Inspection exists
		// 2) Requested data item exists
		SVInspectionProcess* pInspection( nullptr );
		HRESULT hrOK = S_OK;

		for ( long i = 0; i < lNumberOfElements && SUCCEEDED( hrOK ); i++ )
		{
			BSTR bstrName = nullptr;
			SafeArrayGetElementNoCopy( psaNames, &i, &bstrName );
			std::string l_Name = SvUl::createStdString(bstrName);

			if( nullptr != pConfig && pConfig->GetInspectionObject(l_Name.c_str(), &pInspection) && nullptr != pInspection )
			{
				SVImageClass* pImage = nullptr;
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Name, pImage );
				if ( pImage )
				{
					l_InspectionImageIds[ pInspection ].insert( pImage->GetUniqueObjectID() );

					l_ImageNameIds.push_back( SVGetImageListImageInfo( bstrName, pImage->GetUniqueObjectID(), pInspection ) ); 
				}
				else	// couldn't find object
				{
					l_ImageNameIds.push_back( SVGetImageListImageInfo( bstrName, SVGUID(), pInspection ) ); 

					hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;

					l_bItemNotFound = true;

					HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
				}
			}
			else	// couldn't find inspection
			{
				l_ImageNameIds.push_back( SVGetImageListImageInfo( bstrName, SVGUID(), nullptr ) ); 

				hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;

				l_bInspectionNotFound = true;

				HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
			}
		}// end for ( long l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )

		// requested values are not valid - return an error...
		if ( ! SUCCEEDED( hrOK ) )
		{
			throw hrOK;
		}

		SVInspectionImageDataMap l_InspectionImageData;

		SVInspectionImageIdMap::iterator l_InspectionIter;

		for( l_InspectionIter = l_InspectionImageIds.begin(); l_InspectionIter != l_InspectionImageIds.end(); ++l_InspectionIter )
		{
			if( nullptr != l_InspectionIter->first )
			{
				SVCommandInspectionCollectImageDataPtr l_DataPtr{ new SVCommandInspectionCollectImageData(l_InspectionIter->first->GetUniqueObjectID(), l_InspectionIter->second) };
				SVObjectSynchronousCommandTemplate< SVCommandInspectionCollectImageDataPtr > l_Command( l_InspectionIter->first->GetUniqueObjectID(), l_DataPtr );

				if( S_OK == l_Command.Execute( 120000 ) )
				{
					l_InspectionImageData[ l_InspectionIter->first ] = l_DataPtr;
				}
				else
				{
					l_bInspectionNotFound = true;
				}
			}
		}

		for( long l = 0; l < lNumberOfElements; l++ )
		{
			Sleep(0);

			bool bImageOk = false;
			BSTR bstrImage = nullptr;
			long lProcessCount=-1;
			SVImageOverlayClass l_OverlayClass;

			SVCommandInspectionCollectImageDataPtr l_DataPtr;

			SVInspectionImageDataMap::iterator l_Iter = l_InspectionImageData.find( l_ImageNameIds[ l ].m_pInspection );

			if( l_Iter != l_InspectionImageData.end() )
			{
				l_DataPtr = l_Iter->second;
			}

			if(nullptr != l_DataPtr)
			{
				SVIPImageDataElementMap::const_iterator l_ImageIter = l_DataPtr->GetProduct().m_ImageData.find( l_ImageNameIds[ l ].m_ImageId );

				if( l_ImageIter != l_DataPtr->GetProduct().m_ImageData.end() )
				{
					HRESULT hr = S_OK;

					char* pDIB = nullptr;

					bstrImage = SysAllocStringByteLen( nullptr, static_cast< UINT >( l_ImageIter->second.m_ImageDIB.size() ) );
					if (nullptr == bstrImage)
					{
						hr = -1568;
					}
					else
					{
						//------ Convert from wide character pointer to character 
						//------ pointer. Doesn't matter for binary data.
						pDIB = (char *) (bstrImage);    
					}

					if ( S_OK == hr )
					{
						// Copy data to DIB memory locations
						memcpy( pDIB, &(l_ImageIter->second.m_ImageDIB[0]), l_ImageIter->second.m_ImageDIB.size() );
					}

					if ( S_OK == hr )
					{
						lProcessCount = l_DataPtr->GetProduct().m_TriggerCount;

						l_OverlayClass = l_ImageIter->second.m_OverlayData;
					}

					if ( S_OK == hr )
					{
						// Add the status to the outgoing array
						HRESULT hrItem = S_OK;
						hr = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrItem);
						if (S_OK != hr)
						{
							throw -1580;
						}

						// Add the image to the outgoing array
						SafeArrayPutElementNoCopy( *ppsaImages, &l, bstrImage );

						hr = SafeArrayPutElement(*ppsaProcCounts, &l, (void*) &lProcessCount);

						BYTE* pBytes = nullptr;
						long lSize = l_OverlayClass.GetBufferSize();
						pBytes = l_OverlayClass.GetBuffer();

						BSTR bstrOverlay = ::SysAllocStringByteLen( nullptr, lSize );
						memcpy( bstrOverlay, pBytes, lSize );

						// Add overlays to the outgoing array
						SafeArrayPutElementNoCopy( *ppsaOverlays, &l, bstrOverlay );

						bImageOk = true;
					}
				}
			}

			if ( !bImageOk )
			{
				// Add nullptr to the outgoing array
				lProcessCount = -1;
				::SafeArrayPutElement( *ppsaImages, &l, nullptr );
				::SafeArrayPutElement( *ppsaOverlays, &l, nullptr );
				::SafeArrayPutElement( *ppsaProcCounts, &l, (void*) &lProcessCount );

				l_bItemNotFound = true;
			}// end if ( !bImageOk )
		}// end for( long l = 0; l < lNumberOfElements; l++ )

		if (hrResult < 0)
		{
			throw hrResult;
		}

		if ( l_bItemNotFound || l_bInspectionNotFound )
		{
			hrResult = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
	} // end try
	catch ( HRESULT hr )
	{
		hrResult = hr;
	}
	catch ( int hr )
	{
		hrResult = (HRESULT) hr;
	}
	catch ( DWORD hr )
	{
		hrResult = (HRESULT) hr;
	}
	catch (...)
	{
		hrResult = -1579;
	}

	if( FAILED( hrResult ) )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
	}

	return hrResult;
}


STDMETHODIMP CSVCommand::SVRegisterStream(SAFEARRAY* psaName, VARIANT vtInterface, SAFEARRAY** ppsaStatus)
{
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	HRESULT hr = S_OK;
	HRESULT hrRet = S_OK;
	BSTR bstrName;
	std::string sName;
	std::string sInspectionName;
	bool bSamePPQ = true;
	SVInspectionProcess* pInspection( nullptr );
	bool bNotAllItemsFound= false;
	bool l_bGoodInspection = false;

	//put check to see if a configuration is loaded.  if it is not,
	// then error out...
	try 
	{
		if ( m_dwStreamDataProcessId == 0 ) 
		{ // set Id to vtInterface
			m_dwStreamDataProcessId = vtInterface.ulVal;
		}

		if ( m_bRegisteredStream ) 
		{ // registration already happened...
			// check vtInterface - if same set exception that the same control
			// tried to register the stream twice
			if ( m_dwStreamDataProcessId != vtInterface.ulVal )
			{
				// another interface is requesting
				hrRet = SVMSG_STREAM_ALREADY_REGISTERED_BY_ANOTHER_CONTROL;
			}

			if ( m_dwStreamDataProcessId == vtInterface.ulVal )
			{
				hrRet = SVMSG_STREAM_ALREADY_REGISTERED_BY_THIS_CONTROL;
			}

			if( S_OK != hrRet )
			{
				Exception.setMessage( hrRet, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			}

			return hrRet;
		}// end if

		hr = Fire_TestConnection();
		if ( S_OK != hr )
		{
			hrRet = SVMSG_SERVER_UNABLE_TO_CONNECT_TO_CONTROL;
			Exception.setMessage( hrRet, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );

			return hrRet;
		}// end else
		
		CSVCommand* pThis = this;
		StreamDataStruct* pstData;

		m_InspectionNames.clear();

		//check list of names to make sure they are all on the same PPQ
		long lNumberOfElements = psaName->rgsabound[0].cElements;

		for (long lIndex = 0; lIndex < lNumberOfElements; lIndex++)
		{
			SafeArrayGetElementNoCopy(psaName, &lIndex, &bstrName);
			sName = SvUl::createStdString(_bstr_t(bstrName));
			sInspectionName = SvUl::Left(sName, sName.find('.'));
			SvDef::StringVector::iterator Iter = std::find(m_InspectionNames.begin(), m_InspectionNames.end(), sInspectionName);
			if (m_InspectionNames.end() == Iter)
			{
				// add inspection name to list
				m_InspectionNames.push_back(sInspectionName);
			}//end if
		}// end for

		SVGUID PPQId;

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		SvDef::StringVector::iterator Iter(m_InspectionNames.begin());
		for (; m_InspectionNames.end() != Iter && bSamePPQ; ++Iter)
		{
			if (nullptr != pConfig && pConfig->GetInspectionObject(Iter->c_str(), &pInspection) && nullptr != pInspection)
			{
				if (PPQId.empty())
				{
					PPQId = pInspection->GetPPQIdentifier();
				}
				else
				{
					if (PPQId != pInspection->GetPPQIdentifier())
					{
						bSamePPQ = false;
					}
				}
			}
			else
			{
				m_InspectionNames.erase(Iter);
			}
		}

		// requested values are not on same PPQ - return an error...
		if (!bSamePPQ)
		{
			hrRet = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
			Exception.setMessage(hrRet, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			// go through all items in psaStatus and set to hrRet;
			lNumberOfElements = (*ppsaStatus)->rgsabound[0].cElements;
			for (long l = 0; l < lNumberOfElements; l++)
			{
				::SafeArrayPutElement(*ppsaStatus, &l, &hrRet);
			}
			m_bRegisteredStream = false;
			hrRet = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			return hrRet;
		}

		HRESULT hrStatus = S_OK;
		//create list of value objects
		for (long x = 0; x < lNumberOfElements; x++)
		{
			std::string Temp;

			SafeArrayGetElementNoCopy(psaName, &x, &bstrName);
			Temp = SvUl::createStdString(_bstr_t(bstrName));

			sInspectionName = SvUl::Left(Temp, Temp.find('.'));
			bool bFound = false;

			pstData = new StreamDataStruct;

			if (nullptr != pConfig && pConfig->GetInspectionObject(Temp.c_str(), &pInspection))
			{
				if (sInspectionName == pInspection->GetName())
				{
					bFound = true;
				}

				if (bFound)
				{
					l_bGoodInspection = true;
					pstData->m_InspectionID = pInspection->GetUniqueObjectID();
					pstData->strValueName = Temp;

					SVObjectReference ObjectRef;
					HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName(Temp.c_str(), ObjectRef);
					if (S_OK != hrFind)
					{
						pstData->pValueObject = nullptr;
						m_arStreamList.push_back(pstData);
						hrRet = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						::SafeArrayPutElement(*ppsaStatus, &x, &hrRet);
					}// end if
					else
					{
						if (nullptr != ObjectRef.getValueObject())
						{
							hrStatus = S_OK;
							pstData->pValueObject = ObjectRef.getObject();
							pstData->arrayIndex = ObjectRef.ArrayIndex();
							m_arStreamList.push_back(pstData);
						}// end if
						else
						{
							pstData->pValueObject = nullptr;
							m_arStreamList.push_back(pstData);
							hrRet = SVMSG_OBJECT_NOT_PROCESSED;
							::SafeArrayPutElement(*ppsaStatus, &x, &hrRet);
							bNotAllItemsFound = true;
						}// end else
					}// end else
				}// end if
			} // if ( nullptr != pConfig && pConfig->GetInspectionObject(sTmp, &pInspection) )
			else
			{	// Inspection Not Found
				// put following error code in
				pstData->m_InspectionID.clear();
				pstData->pValueObject = nullptr;
				pstData->strValueName = Temp;
				m_arStreamList.push_back(pstData);

				// to compile 
				hrRet = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus, &x, &hrRet);
			}// end else
		}// end for

		Iter = m_InspectionNames.begin();
		for (; m_InspectionNames.end() != Iter && bSamePPQ; ++Iter)
		{
			pInspection = nullptr;
			if (nullptr != pConfig) { pConfig->GetInspectionObject(Iter->c_str(), &pInspection); }
			if (nullptr != pInspection && *Iter == pInspection->GetName())
			{
				SVCommandStreamManager::Instance().EnableInspectionCallback(pInspection->GetUniqueObjectID());
			}
		}

		RebuildStreamingDataList();

		m_bRunStreamData = true;
		m_bRegisteredStream = true;

		SVCommandStreamManager::Instance().InsertCommandCallback(
			boost::bind(&CSVCommand::StreamingDataCallback, this, _1),
			boost::bind(&CSVCommand::RebuildStreamingDataList, this));

		m_hStopStreamEvent = ::CreateEvent(nullptr, false, false, nullptr);
		m_hStreamingThread = ::CreateThread(nullptr, 0, SVStreamDataThread, this, 0, nullptr);
		SVThreadManager::Instance().Add(m_hStreamingThread, "SIAC Streaming Data");
	}//try
	catch(...)
	{
		// send error message
		hr = S_FALSE;
		m_bRegisteredStream = false;
		m_bRunStreamData = false;
		m_dwStreamDataProcessId = 0;
	}

	if ( !(l_bGoodInspection) )
	{
		m_bRegisteredStream = false;
		m_dwStreamDataProcessId = 0;
		bNotAllItemsFound = true;
	}

	if ( hr != S_FALSE )
	{
		if (bNotAllItemsFound)
		{
			hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
		}
		else
		{
			hr = hrRet;
		}
	}

	return hr;
}

STDMETHODIMP CSVCommand::SVUnRegisterStream(VARIANT vtInterface)
{
	HRESULT hr = S_OK;

	if (m_dwStreamDataProcessId == vtInterface.ulVal || m_dwStreamDataProcessId == 0)
	{
		if ( m_bRegisteredStream )
		{
			if( m_hStopStreamEvent )
			{
				::SetEvent( m_hStopStreamEvent );
				::CloseHandle(m_hStopStreamEvent);
				m_hStopStreamEvent = nullptr;
			}

			if( m_hStreamingThread )
			{
				::CloseHandle(m_hStreamingThread);
				SVThreadManager::Instance().Remove( m_hStreamingThread );
				m_hStreamingThread = nullptr;
			}

			m_bRegisteredStream = false;
			m_bRunStreamData = false;

			SVCommandStreamManager::Instance().EraseCommandCallback();
			m_InspectionNames.clear();
			m_dwStreamDataProcessId = 0;

			for(auto pStream : m_arStreamList )
			{
				delete pStream;
			}// end for
			m_arStreamList.clear();

			for(auto pProduct : m_arProductList)
			{
				delete pProduct;
			}// end for
			m_arProductList.clear();
		}// end if
		else
		{
			hr = SVMSG_STREAM_NOT_REGISTERED;
		}// end else
	}
	else
	{
		hr = SVMSG_STREAM_ALREADY_REGISTERED_BY_ANOTHER_CONTROL;
	}
	return hr;
}

VOID CALLBACK SVStreamingDataAPCProc( DWORD_PTR dwParam )
{
	// do nothing ...
	// let the APC cause the waiting object to return
}

HRESULT CSVCommand::StreamingDataCallback( const SVInspectionCompleteInfoStruct& p_rData )
{
	ProductDataStruct* pProductData(nullptr);
	PacketDataStruct oPacketData;
	StreamDataStruct *pStreamData;
	long l;
	long lSize;

	if( m_bRegisteredStream )
	{
		// Protect the to be streamed list
		::EnterCriticalSection( CProductCriticalSection::Get());

		// search the list for this product
		bool bFound = false;
		lSize = static_cast<long> (m_arProductList.size());

		for( l = 0; l < lSize; l++ )
		{
			pProductData = m_arProductList[l];
			if(nullptr != pProductData)
			{
				if( pProductData->lProductCount == p_rData.m_ProductInfo.ProcessCount() )
				{
					bFound = true;
					break;
				}// end if
			}// end if
		}// end for

		// it is not in the list then we need to add it
		if( !bFound )
		{
			ProductDataStruct* pProductData = new ProductDataStruct;
			pProductData->lProductCount = p_rData.m_ProductInfo.ProcessCount();
			pProductData->m_PacketDataVector.resize(m_arStreamList.size());

			// add the product to the outgoing list
			m_arProductList.push_back( pProductData );
		}// end if

		// We will go ahead and make a copy of the data now
		lSize = static_cast<long> (m_arStreamList.size());
		for( l = 0; l < lSize; l++ )
		{
			pStreamData = m_arStreamList[l];

			SVGUIDSVInspectionInfoStructMap::const_iterator l_Iter = p_rData.m_ProductInfo.m_svInspectionInfos.find( p_rData.m_InspectionID );

			if( p_rData.m_ProductInfo.m_svInspectionInfos.end() != l_Iter )
			{
				oPacketData.lState = l_Iter->second.oInspectedState;
				oPacketData.pValueObject = pStreamData->pValueObject;
				SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (oPacketData.pValueObject);
				if( nullptr != pValueObject )
				{
					if (pStreamData->arrayIndex > 0)
					{
						// Ensure valueObject is an Array
						if( pValueObject->isArray())
						{
							// Ensure index is not ot of bounds
							if (pStreamData->arrayIndex < pValueObject->getArraySize())
							{
								pValueObject->getValue( oPacketData.strValue, pStreamData->arrayIndex, p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex() );
							}
							else
							{
								oPacketData.strValue.clear();
							}
						}
						else
						{
							oPacketData.strValue.clear();
						}
					}
					else
					{
						pValueObject->getValue( oPacketData.strValue, -1, p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex() );
					}
				}// end if
				else
				{
					oPacketData.strValue.clear();
				}// end else

				pProductData->m_PacketDataVector[l] = oPacketData;
			}// end if
		}// end for

		pProductData->lCallbackCount++;

		// Unprotect the to be streamed list
		::LeaveCriticalSection( CProductCriticalSection::Get() );

		// if streaming thread is starved, temporarily boost it
		if( 20 < m_arProductList.size() )
		{
			::SetThreadPriority( m_hStreamingThread, THREAD_PRIORITY_NORMAL );
		}// end if

		::QueueUserAPC( SVStreamingDataAPCProc, m_hStreamingThread, reinterpret_cast<ULONG_PTR>(nullptr) );
	}// end if

	return S_OK;
}

DWORD WINAPI CSVCommand::SVStreamDataThread(LPVOID lpParam)
{
	SAFEARRAY *saNames;
	SAFEARRAY *saValues;
	SAFEARRAY *saProcessCount;
	HRESULT hr = S_OK;
	SAFEARRAYBOUND sabound[1];
	StreamDataStruct *pStreamData;
	ProductDataStruct *pProductData;
	PacketDataStruct oPacketData;
	std::string strValue;
	bool bRunning = true;
	DWORD dwResult;
	BSTR bstr;
	long lSize;
	long lIndex;
	long lStreamCount = 0;
	long lInspectIndex;
	long lProductCount;
	long lPacketCount;
	long lNoProduct = -1;
	long l;
	long k;

	CSVCommand *pThis = reinterpret_cast<CSVCommand*> (lpParam);
	if (nullptr == pThis)
	{
		return 0L;
	}
	long lStreamSize = static_cast<long> (pThis->m_arStreamList.size());

	// Initialize COM
	CoInitialize( nullptr );    

	do
	{
		if( lStreamCount )
		{
			dwResult = WAIT_IO_COMPLETION;
		}// end if
		else
		{
			dwResult = ::WaitForSingleObjectEx( pThis->m_hStopStreamEvent, INFINITE, true);
		}// end else

		switch( dwResult )
		{
		case WAIT_OBJECT_0 :
			bRunning = false;
			break;
		case WAIT_IO_COMPLETION :
			// pass one - check for products to stream now that we are awake
			lProductCount = static_cast<long> (pThis->m_arProductList.size());
			lStreamCount = 0;
			hr = S_OK;

			for( l = 0; l < lProductCount; l++ )
			{
				pProductData = pThis->m_arProductList[l];
				if( nullptr != pProductData && pProductData->lCallbackCount == m_InspectionNames.size() )
				{
					lStreamCount++;
				}// end if

			}//end for

			// pass two - build the packets to stream from these products
			if( lStreamCount)
			{
				// pass two 
				sabound[0].lLbound = 0;
				sabound[0].cElements = lStreamCount * lStreamSize;
				lPacketCount = lStreamCount * lStreamSize;
				lIndex = 0;

				saNames			= ::SafeArrayCreate( VT_BSTR, 1, sabound );
				saValues		= ::SafeArrayCreate( VT_BSTR, 1, sabound );
				saProcessCount	= ::SafeArrayCreate( VT_I4, 1, sabound );

				lProductCount = static_cast<long> (pThis->m_arProductList.size());
				for( l = 0; lStreamCount && l < lProductCount; l++ )
				{
					pProductData = pThis->m_arProductList[l];

					// Verify again that this is one of the ready products
					if( nullptr == pProductData || pProductData->lCallbackCount != m_InspectionNames.size() )
					{
						continue;
					}

					for( k = 0; k < lStreamSize; k++ )
					{
						lInspectIndex = -1;

						pStreamData = pThis->m_arStreamList[k];
						oPacketData = pProductData->m_PacketDataVector[k];

						if( nullptr == pStreamData || nullptr == pStreamData->pValueObject || 
							nullptr == oPacketData.pValueObject || 
							( oPacketData.lState & PRODUCT_INSPECTION_NOT_RUN ) == PRODUCT_INSPECTION_NOT_RUN ||
							pStreamData->pValueObject != oPacketData.pValueObject )
						{
							// product not found or item not found or product not inspected. 
							// send -1 for process count to keep list same size, as what they requested.
							bstr = _bstr_t( pStreamData->strValueName.c_str()).Detach();
							SafeArrayPutElementNoCopy( saNames, &lIndex, bstr );

							strValue = _T( "" );
							bstr = _bstr_t(pStreamData->strValueName.c_str()).Detach();
							bstr = _bstr_t(strValue.c_str()).Detach();
							SafeArrayPutElementNoCopy( saValues, &lIndex, bstr );

							::SafeArrayPutElement( saProcessCount, &lIndex, &lNoProduct );

							lIndex++;
						}// end if
						else
						{
							// product and item were found, add values to SAFEARRAYS
							bstr = _bstr_t(pStreamData->strValueName.c_str()).Detach();
							SafeArrayPutElementNoCopy( saNames, &lIndex, bstr );

							_bstr_t bTemp = oPacketData.strValue.c_str();
							SafeArrayPutElementNoCopy( saValues, &lIndex, bTemp.Detach() );

							::SafeArrayPutElement( saProcessCount, &lIndex, &pProductData->lProductCount );

							lIndex++;
						}//end else

					} // end for

					m_lLastStreamedProduct = pProductData->lProductCount;
					lProductCount = static_cast<long> (pThis->m_arProductList.size());
					lStreamCount--;

					::EnterCriticalSection(CProductCriticalSection::Get());

					pThis->m_arProductList.erase(pThis->m_arProductList.begin() + l);

					::LeaveCriticalSection(CProductCriticalSection::Get() );
					delete pProductData;
					l--;
				} // end for

				try
				{
					hr = pThis->Fire_StreamingData( saProcessCount, saNames, saValues );
				}// end try
				catch( ... )
				{
					hr = S_FALSE;
				}// end catch

				::SafeArrayDestroy( saNames );
				::SafeArrayDestroy( saValues );
				::SafeArrayDestroy( saProcessCount );
			}// end if

			// pass three - check for products to stream before we go back to sleep
			lProductCount = static_cast<long> (pThis->m_arProductList.size());
			lStreamCount = 0;

			for( l = 0; l < lProductCount; l++ )
			{
				pProductData = pThis->m_arProductList[l];
				if( pProductData->lCallbackCount == static_cast<long> (m_InspectionNames.size()) )
				{
					lStreamCount++;
				}// end if
			}//end for

			// Make sure thread priority is set back down
			::SetThreadPriority( m_hStreamingThread, THREAD_PRIORITY_NORMAL );

			// Check to make sure that the streaming data didn't have a fatal error
			if( FAILED( hr ) )
			{
				SVCommandStreamManager::Instance().EraseCommandCallback();
				m_InspectionNames.clear();
				m_dwStreamDataProcessId = 0;

				lSize = static_cast< long >( m_arStreamList.size() );
				for( l = 0; l < lSize; l++ )
				{
					pStreamData = m_arStreamList[l];
					delete pStreamData;
				}// end for
				m_arStreamList.clear();

				lSize = static_cast<long> (m_arProductList.size());
				for( l = 0; l < lSize; l++ )
				{
					pProductData = m_arProductList[l];
					delete pProductData;
				}// end for
				m_arProductList.clear();

				bRunning = false;
			}// end if

			break;
		default:
			break;
		}// end switch
	} while( bRunning );

	//check to see if handle is still valid.  if it is close it
	if( pThis->m_hStopStreamEvent )
	{
		::CloseHandle( pThis->m_hStopStreamEvent );
		pThis->m_hStopStreamEvent = nullptr;
	}// end if

	if( pThis->m_hStreamingThread )
	{
		::CloseHandle( pThis->m_hStreamingThread );
		pThis->m_hStreamingThread = nullptr;
	}// end if

	//reset flags
	m_bRegisteredStream = false;
	m_bRunStreamData = false;

	// Uninitialize COM
	CoUninitialize();

	return 0L;
}

STDMETHODIMP CSVCommand::SVGetProductDataList(long lProcessCount, SAFEARRAY* psaNames, SAFEARRAY** ppsaData, SAFEARRAY** ppsaStatus)
{
	HRESULT hr = S_OK;

	BSTR bstr;
	SvDef::StringVector ValueNames;
	SVInspectionProcessVector InspectionVector;
	SVObjectReferenceVector aValueObjects;
	bool l_bItemNotExist = false;

	//check to see if in Run Mode.  if not return SVMSG_53_SVIM_NOT_IN_RUN_MODE

	if ( !SVSVIMStateClass::CheckState(SV_STATE_RUNNING) )
	{
		return SVMSG_53_SVIM_NOT_IN_RUN_MODE;
	}

	// VERIFY THE LIST OF NAMES

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lNumberOfElements = psaNames->rgsabound[0].cElements;

	ASSERT( nullptr != ppsaStatus );
	ASSERT( nullptr != *ppsaStatus );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

	ASSERT( nullptr != ppsaData );
	ASSERT( nullptr != *ppsaData );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaData)->rgsabound[0].cElements == lNumberOfElements );

	long lDefaultStatus = S_FALSE;

	// create String array of value names; set default Status
	for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
	{
		SafeArrayGetElementNoCopy(psaNames, &lIndex, &bstr);
		std::string sName = SvUl::createStdString( _bstr_t(bstr) );
		ValueNames.push_back( sName );

		SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
	}

	if ( nullptr == pConfig || !pConfig->IsConfigurationLoaded() )
	{
		hr = SVMSG_CONFIGURATION_NOT_LOADED;
		return hr;
	}

	//go through list of names and make sure they are all valid
	// 1) Inspection exists
	// 2) Requested data item exists
	// 3) all data items are on the same PPQ
	HRESULT hrOK = S_OK;
	SVGUID l_PPQId;

	for( long i = 0; i < lNumberOfElements && SUCCEEDED( hrOK ); i++ )
	{
		SVInspectionProcess* pInspection( nullptr );
		//GetInspectionObject is only true if the pointer is valid
		if ( (nullptr != pConfig) && pConfig->GetInspectionObject(ValueNames[i].c_str(), &pInspection))
		{
			InspectionVector.push_back(pInspection);	// add inspection object to list

			SVObjectReference ObjectRef;
			SVObjectManagerClass::Instance().GetObjectByDottedName( ValueNames[i].c_str(), ObjectRef );
			if( ObjectRef.getObject() )
			{
				aValueObjects.push_back(ObjectRef);	// add data object pointer to the list

				if ( l_PPQId.empty() ) //set the first PPQ name as the comparison standard
				{
					l_PPQId = pInspection->GetPPQIdentifier();
				}
				else
				{
					if ( l_PPQId != pInspection->GetPPQIdentifier() )	// not on same PPQ
					{
						hrOK = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
						// set the status of all objects to this error
						for (long l = 0; l < lNumberOfElements; l++)
						{
							HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);
						}
						break;
					}
				}
			}
			else	// couldn't find data object
			{
				aValueObjects.push_back( SVObjectReference() );
				hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
			}
		}
		else	// couldn't find inspection
		{
			InspectionVector.push_back( nullptr );
			aValueObjects.push_back( SVObjectReference() );
			hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
		}
	}

	// requested values are not valid - return an error...
	if ( ! SUCCEEDED( hrOK ) )
	{
		hr = hrOK;
		return hr;
	}

	// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
	SVProductInfoStruct l_ProductInfo;

	SVPPQObject* pPPQ = dynamic_cast< SVPPQObject* >( SVObjectManagerClass::Instance().GetObject( l_PPQId ) );

	if( nullptr != pPPQ && S_OK == pPPQ->GetProduct( l_ProductInfo, lProcessCount ) )
	{
		// Product is still valid; 
		// Build the array containing the result data

		for (long i=0; i < lNumberOfElements; i++)
		{
			SVObjectReference ObjectRef = aValueObjects[i];
			std::string Value;
			HRESULT hrStatus = S_OK;

			if ( nullptr != ObjectRef.getObject() )
			{
				SVDataManagerHandle	l_BucketHandle;

				l_ProductInfo.GetResultDataIndex( l_BucketHandle );

				if ( !ObjectRef.isEntireArray() )
				{
					HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, ObjectRef.getValidArrayIndex(), l_BucketHandle.GetIndex());
					if ( S_OK == hrGet )
					{
						// put value in return array
						_bstr_t bstrTemp = Value.c_str();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTemp.Detach());

						HRESULT hrItem = S_OK;
						SafeArrayPutElement(*ppsaStatus, &i, &hrItem);
					}// if OK
					else if ( SVMSG_SVO_33_OBJECT_INDEX_INVALID == hrGet
						|| SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hrGet )
					{
						hrStatus = hrGet;
						// did not get value.  set value to default
						Value = ObjectRef.DefaultValue();
						if ( Value.empty() )
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
						ASSERT( false );
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
					for ( int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++ )
					{
						HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, iArrayIndex, l_BucketHandle.GetIndex());
						if ( S_OK == hrGet )
						{
							if ( iArrayIndex > 0 )
							{
								ArrayValues += _T(",");
							}

							ArrayValues += SvUl::Format( _T("`%s`"), Value.c_str() );
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
	if ( l_bItemNotExist )
	{
		hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}

	return hr;
}

STDMETHODIMP CSVCommand::SVGetProductImageList(long lProcessCount, SAFEARRAY* psaNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus)
{
	HRESULT hrResult = S_OK;

	//check to see if in Run Mode.  if not return SVMSG_53_SVIM_NOT_IN_RUN_MODE

	if ( !SVSVIMStateClass::CheckState(SV_STATE_RUNNING) )
	{
		return SVMSG_53_SVIM_NOT_IN_RUN_MODE;
	}

	try
	{
		// SETUP
		long l( 0 );

		long lNumberOfElements = psaNames->rgsabound[0].cElements;

		ASSERT( nullptr != ppsaStatus );
		ASSERT( nullptr != *ppsaStatus );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( nullptr != ppsaImages );
		ASSERT( nullptr != *ppsaImages );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaImages)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( nullptr != ppsaOverlays );
		ASSERT( nullptr != *ppsaOverlays );// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements );

		BSTR bstr = nullptr;
		SVImageClassPtrVector ImageObjects;
		SVInspectionProcessVector aInspections;
		bool l_bItemNotFound = false;
		bool l_bInspectionNotFound = false;
		BSTR bstrOverlay = nullptr;

		long lDefaultStatus = S_FALSE;

		// set default status for objects in list
		for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
		{
			HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
			// No overlays for no for product source images
			// overlays are not bucketized
			bstr = nullptr;
			hrTemp = SafeArrayPutElement(*ppsaOverlays, &lIndex, bstr);
		}

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if ( nullptr == pConfig || !pConfig->IsConfigurationLoaded() )
		{
			throw SVMSG_CONFIGURATION_NOT_LOADED;
		}

		//go through list of names and make sure they are all valid
		// 1) Inspection exists
		// 2) Requested data item exists
		// 3) all data items are on the same PPQ
		SVInspectionProcess* pInspection( nullptr );
		HRESULT hrOK = S_OK;
		SVGUID l_PPQId;

		for ( l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )
		{
			BSTR bstrName = nullptr;
			SafeArrayGetElementNoCopy( psaNames, &l, &bstrName );
			std::string strName = SvUl::createStdString( _bstr_t(bstrName) );

			if (nullptr != pConfig && pConfig->GetInspectionObject(strName.c_str(), &pInspection))
			{
				aInspections.push_back( pInspection );
				SVImageClass* pImage = nullptr;
				if(S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(strName.c_str(), pImage))
				{
					bool bImageOK = false;
					if( dynamic_cast< SVCameraImageTemplate* >( pImage ) ) // Source image
					{
						ImageObjects.push_back(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else if ( pImage->ObjectAttributesSet() & SvDef::SV_PUBLISH_RESULT_IMAGE )	// Published result image
					{
						ImageObjects.push_back(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else
					{
						hrOK = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE;
						l_bItemNotFound = true;
						ImageObjects.push_back( nullptr );
						HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);
					}

					// if image is OK, check PPQ
					if ( bImageOK )
					{
						if ( l_PPQId.empty() ) //set the first PPQ name as the comparison standard
						{
							l_PPQId = pInspection->GetPPQIdentifier();
						}
						else
						{
							if ( l_PPQId != pInspection->GetPPQIdentifier() )	// not on same PPQ
							{
								hrOK = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
								// set the status of all objects to this error
								for (long i = 0; i < lNumberOfElements; i++)
								{
									HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
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
					ImageObjects.push_back( nullptr );
					HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);

					// Add nullptr to the outgoing array
					::SafeArrayPutElement( *ppsaImages, &l, nullptr );
					::SafeArrayPutElement( *ppsaOverlays, &l, bstrOverlay );
				}
			}// if (TheSVObserverApp.m_pConfiguration->GetInspectionObject(strName, &pInspection))
			else	// couldn't find inspection
			{
				hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				l_bInspectionNotFound = true;
				ImageObjects.push_back( nullptr );
				aInspections.push_back( nullptr );
				HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);
			}
		}// end for ( long l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )

		// requested values are not valid - return an error...
		if ( ! SUCCEEDED( hrOK ) )
		{
			throw hrOK;
		}

		// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
		SVProductInfoStruct ProductInfo;

		SVPPQObject* pPPQ = dynamic_cast< SVPPQObject* >( SVObjectManagerClass::Instance().GetObject( l_PPQId ) );

		if ( nullptr != pPPQ )
		{
			if( S_OK == pPPQ->GetProduct( ProductInfo, lProcessCount ) )
			{
				// Product is still valid; 
				// Build the output images from the result data index in the product and the source images by name  (?)
				for (l=0; l < lNumberOfElements; l++)
				{
					HRESULT hrImage = S_OK;
					if ( nullptr != aInspections[l] )
					{
						bool bImageOk = false;

						SVImageClass* pImage = ImageObjects[l];
						if (nullptr != pImage)
						{
							SVImageInfoClass svImageInfo;
							SvOi::SVImageBufferHandlePtr svImageHandle;
							BSTR bstrImage = nullptr;

							// this works for Source Images (SVMainImageClass) and Published Result images
							SVImageIndexStruct svIndex( pImage->GetSourceImageIndex( &ProductInfo.oPPQInfo.m_ResultImagePublishedDMIndexHandle, ProductInfo.m_svCameraInfos ) );

							// put image in return array
							BSTR bstrTemp = nullptr;

							HRESULT hr = SafeImageToBSTR( pImage, svIndex, &bstrTemp);

							if ( SUCCEEDED( hr ) )
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

						if ( !bImageOk )
						{
							// put nullptr image in return array
							SafeArrayPutElement(*ppsaImages, &l, nullptr);

							if ( S_OK == hrImage )
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

		if ( l_bItemNotFound || l_bInspectionNotFound )
		{
			if ( S_OK == hrResult )
			{
				hrResult = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}

	} // end try
	catch ( HRESULT hr )
	{
		hrResult = hr;
	}
	catch ( int hr )
	{
		hrResult = (HRESULT) hr;
	}
	catch ( DWORD hr )
	{
		hrResult = (HRESULT) hr;
	}
	catch (...)
	{
		hrResult = -12338;
	}

	if( FAILED( hrResult ) )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SVSetLUT(BSTR bstrCameraName, SAFEARRAY* paulLUTTable)
{
	HRESULT hr = S_OK;

	std::string CameraName = SvUl::createStdString( _bstr_t(bstrCameraName) );
	SVVirtualCamera* pCamera( nullptr );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( nullptr != pConfig && pConfig->GetChildObjectByName(CameraName.c_str(), &pCamera) && nullptr != pCamera )
	{
		VARTYPE vt = VT_EMPTY;
		::SafeArrayGetVartype(paulLUTTable, &vt);
		if ( vt == VT_UI4 || vt == VT_I4 )
		{
			SVLut lut;

			hr = pCamera->GetLut( lut );

			// Anything from the ActiveX we will set to freeform
			lut.SetTransformOperation(SVLutTransformOperationFreeform());

			bool bResult = lut.SetBandData(paulLUTTable);
			if (bResult)
			{
				hr = pCamera->SetLut( lut );
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

STDMETHODIMP CSVCommand::SVGetLUT(BSTR bstrCameraName, SAFEARRAY** ppaulLUTTable)
{
	HRESULT hr = S_OK;

	std::string CameraName = SvUl::createStdString( _bstr_t(bstrCameraName) );
	SVVirtualCamera* pCamera( nullptr );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && pConfig->GetChildObjectByName(CameraName.c_str(), &pCamera) && nullptr != pCamera )
	{
		SVLut lut;

		hr = pCamera->GetLut( lut );

		lut.GetBandData(*ppaulLUTTable);
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
	}

	return hr;
}

HRESULT CSVCommand::ImageToBSTR(SVImageInfoClass&  rImageInfo, SvOi::SVImageBufferHandlePtr rImageHandle, BSTR* pbstr)
{
	HRESULT hr = S_OK;

	SVImageBufferHandleImage l_MilBuffer;
	if (nullptr == rImageHandle || rImageHandle->empty() )
	{
		hr = -1578;
	}

	if ( S_OK == hr )
	{
		HRESULT l_Code;

		SVImageInfoClass oChildInfo;
		SvOi::SVImageBufferHandlePtr oChildHandle;
		BITMAPINFOHEADER* pbmhInfo = nullptr;
		SVBitmapInfo l_BitmapInfo;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;
		bool IsColor( false );

		char* pDIB = nullptr;

		bool bDestroyHandle = false;

		oChildInfo = rImageInfo;
		oChildHandle = rImageHandle;

		long l_lType = SvDef::SVImageTypeEnum::SVImageTypeUnknown;
		long l_lBandNumber = 1;
		long l_lBandLink = 0;

		SVImageClass* pImage( nullptr );

		oChildInfo.GetOwnerImage( pImage );

		if( nullptr != pImage )
		{
			l_lType = pImage->GetImageType();
			SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( pImage->GetAncestor( SvDef::SVInspectionObjectType ) );
			if( nullptr != pInspection )
			{
				IsColor = pInspection->IsColorCamera();
			}
		}

		oChildInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber );
		oChildInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, l_lBandLink );

		if ( IsColor && l_lType == SvDef::SVImageTypeEnum::SVImageTypePhysical && l_lBandNumber == 3)
		{
			bDestroyHandle = true;

			oChildInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

			HRESULT hrImage = SVImageProcessingClass::CreateImageBuffer( oChildInfo, oChildHandle );

			SVImageBufferHandleImage l_ChildMilBuffer;

			if (nullptr != oChildHandle)
			{
				// Copy input image to output image
				l_Code = SVMatroxBufferInterface::CopyBuffer(oChildHandle->GetBuffer(), rImageHandle->GetBuffer(), l_lBandLink);
			}
			else
			{
				l_Code = E_FAIL;
			}
		} 

		if ((l_lType == SvDef::SVImageTypeEnum::SVImageTypeLogicalAndPhysical) ||
			(l_lType == SvDef::SVImageTypeEnum::SVImageTypeLogical))
		{
			bDestroyHandle = true;

			HRESULT hrImage = SVImageProcessingClass::CreateImageBuffer( oChildInfo, oChildHandle );

			SVImageBufferHandleImage l_ChildMilBuffer;

			if (nullptr != oChildHandle)
			{
				// Copy input image to output image
				l_Code = SVMatroxBufferInterface::CopyBuffer(oChildHandle->GetBuffer(), rImageHandle->GetBuffer());
			}
			else
			{
				l_Code = E_FAIL;
			}
		}

		// Get the BITMAPINFO from MIL
		l_BitmapInfo = oChildHandle->GetBitmapInfo();
		if( l_BitmapInfo.empty() )
		{
			l_Code = SVMatroxBufferInterface::GetBitmapInfo( l_BitmapInfo, nullptr != oChildHandle ? oChildHandle->GetBuffer() : SVMatroxBuffer() );
		}

		pbmhInfo = &( l_BitmapInfo.GetBitmapInfo()->bmiHeader );

		// Source images seem to be flipped even though MIL is not supposed to flip them
		if( pbmhInfo->biHeight > 0 )
		{
			l_BitmapInfo.FlipHeight();
		}

		// Calculate the absolute height
		lHeight = pbmhInfo->biHeight * ( ( pbmhInfo->biHeight < 0 ) ? -1 : 1 );

		// Make sure image size is calculated
		if( 0 == pbmhInfo->biSizeImage )
		{
			pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;
		}

		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( pbmhInfo->biBitCount != 24 )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		// Calculate total size buffer needed for image
		lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;

		*pbstr = SysAllocStringByteLen(nullptr, lBufSize);
		if (nullptr  == *pbstr)
		{
			hr = -1568;
		}
		else
		{
			//------ Convert from wide character pointer to character 
			//------ pointer. Doesn't matter for binary data.
			pDIB = (char *) (*pbstr);    
		}

		if(nullptr == oChildHandle)
		{
			hr = E_FAIL;
		}

		if ( S_OK == hr )
		{
			// Copy data to DIB memory locations
			memcpy( pDIB, l_BitmapInfo.GetBitmapInfo(), sizeof( BITMAPINFOHEADER ) + lTabSize );
			memcpy( pDIB + sizeof( BITMAPINFOHEADER ) + lTabSize, 
				oChildHandle->GetBufferAddress(), 
				pbmhInfo->biSizeImage );
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

HRESULT CSVCommand::SafeImageToBSTR( SVImageClass *p_pImage, SVImageIndexStruct p_svIndex, BSTR *pbstr)
{
	HRESULT hr = S_OK;

	if( nullptr != p_pImage )
	{
		SVImageInfoClass oChildInfo = p_pImage->GetImageInfo();

		SvOi::SVImageBufferHandlePtr oChildHandle;

		SVImageProcessingClass::CreateImageBuffer( oChildInfo, oChildHandle );

		p_pImage->SafeImageCopyToHandle( p_svIndex, oChildHandle );

		hr = ImageToBSTR( oChildInfo, oChildHandle, pbstr);
	}
	else
	{
		hr = -1578;
	}

	return hr;
}

HRESULT CSVCommand::SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
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

HRESULT CSVCommand::SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
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

HRESULT CSVCommand::SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv)
	// sets the pointer to point to the actual memory of the SAFEARRAY that contains the element.
{
	DWORD dwOffset=0;
	DWORD dwIndexOffset=1;
	for (int i=psa->cDims-1; i >= 0 ; i--)
	{
		long lIndex = *(rgIndices+i);
		int iDimensionIndex = psa->cDims-1 - i;
		if ( lIndex < psa->rgsabound[iDimensionIndex].lLbound || lIndex >= (long)( psa->rgsabound[iDimensionIndex].lLbound + psa->rgsabound[iDimensionIndex].cElements ) )
		{
			return DISP_E_BADINDEX;
		}

		long dwIndex = (lIndex - psa->rgsabound[iDimensionIndex].lLbound);   // offset index in dimension used for figuring out memory offset

		dwOffset += dwIndex * dwIndexOffset;
		dwIndexOffset *= psa->rgsabound[iDimensionIndex].cElements;   // offset of dimension used in calculating next dim
	}
	dwOffset *= psa->cbElements;    // scale by size of element

	BYTE* pElement = ((BYTE*) psa->pvData) + dwOffset;
	*ppv = pElement;

	return S_OK;
}

HRESULT CSVCommand::SVGetDataList(SAFEARRAY* psaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts)
{
	HRESULT Result = S_OK;
	HRESULT Status = S_OK;
	bool    ItemNotFound = false;
	bool    InspectionNotFound = false;

	//get number of elements out of the incoming safearray
	long Size = psaNames->rgsabound[0].cElements;

	ASSERT( nullptr != ppsaStatus );
	ASSERT( nullptr != *ppsaStatus );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaStatus)->rgsabound[0].cElements == Size );

	ASSERT( nullptr != ppsaValues );
	ASSERT( nullptr != *ppsaValues );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaValues)->rgsabound[0].cElements == Size );

	ASSERT( nullptr != ppsaProcCounts );
	ASSERT( nullptr != *ppsaProcCounts );// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaProcCounts)->rgsabound[0].cElements == Size );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	for ( long l = 0; l < Size; l++ )
	{
		std::string Name;
		std::string Value;
		BSTR bstrName = nullptr;
		long ProcessCount = -1;
		SVObjectReference ObjectRef;
		SVInspectionProcess* pInspection( nullptr );

		Status = S_OK;

		// Get name of requested value out of the safearray
		SafeArrayGetElementNoCopy(psaNames,&l,&bstrName);
		Name = SvUl::createStdString( bstrName );
		ProcessCount = -1;
		if( nullptr != pConfig )
		{
			pConfig->GetInspectionObject( Name.c_str(),&pInspection );
		}

		SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), ObjectRef );

		if ( nullptr != ObjectRef.getObject() || nullptr != pInspection )
		{
			//If inspection  is nullptr then object is of type BasicValueObject
			if( nullptr == pInspection )
			{
				BasicValueObject* pValueObject = dynamic_cast< BasicValueObject* >( ObjectRef.getObject() );
				if( nullptr != pValueObject && !ObjectRef.isEntireArray() )
				{
					if ( S_OK != pValueObject->getValue( Value ) )
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
				SVObjectReference ObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), ObjectRef );

				if( nullptr != ObjectRef.getObject() )
				{
					ProcessCount = pInspection->LastProductGet( SV_OTHER ).ProcessCount();

					if( !ObjectRef.isEntireArray() )
					{
						// was able to find the object
						HRESULT hrGet = ObjectRef.getValueObject()->getValue( Value, ObjectRef.getValidArrayIndex() );
						if ( S_OK == hrGet )
						{
							//got value
							Status = S_OK;
						}// if OK
						else if (    hrGet == SVMSG_SVO_33_OBJECT_INDEX_INVALID 
							|| hrGet == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
						{
							Status = hrGet;
							// did not get value.  set value to default
							Value = ObjectRef.DefaultValue();
							if ( Value.empty() )
							{
								Value = _T("-1");
							}
							ItemNotFound = true;
						}// else invalid or out of range index
						else	// some generic error; currently should not get here
						{
							ASSERT(false);
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
						for ( int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++ )
						{
							HRESULT hrGet = ObjectRef.getValueObject()->getValue( Value, iArrayIndex  );
							if ( S_OK == hrGet )
							{
								if ( iArrayIndex > 0 )
								{
									ArrayValues += _T(",");
								}
								ArrayValues += SvUl::Format( _T("`%s`"), Value.c_str() );
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
		SafeArrayPutElementNoCopy(*ppsaValues,&l,bstrTmpVal.Detach());
		::SafeArrayPutElement(*ppsaStatus,&l,&Status);
		::SafeArrayPutElement(*ppsaProcCounts,&l,&ProcessCount);
	}// end for

	if ( ItemNotFound || InspectionNotFound )
	{
		Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}
	else
	{
		Result = Status;
	}

	return Result;
}

STDMETHODIMP CSVCommand::SVRunOnce(BSTR bstrName)
{
	USES_CONVERSION;

	HRESULT              hrResult = S_FALSE;	
	SVInspectionProcess* pInspection( nullptr );

	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( SVConfigurationObject::GetInspection( W2T(bstrName), pInspection) )
		{
			SvPB::InspectionRunOnceRequest requestMessage;
			SvPB::SetGuidInMessage(requestMessage.mutable_inspectionid(), pInspection->GetUniqueObjectID());
			hrResult = SvCmd::InspectionCommandsSynchronous(pInspection->GetUniqueObjectID(), &requestMessage, nullptr);
		}
	}// end if
	else
	{
		//return an error.  not able to do runonce.
		hrResult = SVMSG_44_RUNONCE_ONLINE;
	}// end else

	return hrResult;
}

STDMETHODIMP CSVCommand::SVSetSourceImage(BSTR bstrName, BSTR bstrImage)
{
	USES_CONVERSION;

	HRESULT                  hrResult = S_OK;
	SVInspectionProcess*     pInspection( nullptr );
	SVCameraImageTemplate*   pMainImage( nullptr );

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION | SV_STATE_TEST | SV_STATE_RUNNING ) )
	{
		hrResult = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
		return hrResult;
	}

	if( SVConfigurationObject::GetInspection( W2T(bstrName), pInspection) )
	{
		pMainImage = pInspection->GetToolSetMainImage();
		if( nullptr != pMainImage )
		{
			if( S_OK != pInspection->AddInputImageRequest( pMainImage, bstrImage ) )
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

STDMETHODIMP CSVCommand::SVSetInputs(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus)
{
	USES_CONVERSION;

	HRESULT                Result = S_OK;
	HRESULT                Status = S_OK;

	long lNumberOfElements1 = psaNames->rgsabound[0].cElements;
	long lNumberOfElements2 = psaValues->rgsabound[0].cElements;

	if ((lNumberOfElements1 == 0) || (lNumberOfElements1 != lNumberOfElements2))
	{
		Result = SVMSG_TOO_MANY_REQUESTED_ITEMS;
		return Result;
	}

	SVNameSet				ParameterNames;
	SVNameStorageMap		ParameterObjects;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

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
		SVInspectionProcess*   pInspection(nullptr);
		if (nullptr != pConfig && pConfig->GetInspectionObject(Name.c_str(), &pInspection))
		{
			//If Remote inputs then the name must be replaced otherwise add "Inspections." to name
			if (std::string::npos != Name.find(SvDef::FqnRemoteInput))
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
		SVSAFEARRAY	Values;

		//Values must be in array format
		Values.Add(_variant_t(bstrValue));
		Parameter.m_StorageType = SVVisionProcessor::SVStorageValue;
		Parameter.m_Variant = Values;
		ParameterObjects[std::string(Name)] = Parameter;
	}// end for	

	Result = Status;

	if (S_OK == Result && 0 != ParameterObjects.size())
	{
		SVNameStatusMap SetItemsResult;
		Result = SVVisionProcessorHelper::Instance().SetItems(ParameterObjects, SetItemsResult, false);
	}


	return Result;
}

HRESULT CSVCommand::SVSetImageList(SAFEARRAY *psaNames, SAFEARRAY *psaImages, SAFEARRAY **ppsaStatus)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	HRESULT hrStatus = S_OK;
	SVInspectionProcess* pInspection( nullptr );
	SVImageClass* l_pImageObject( nullptr );
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

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		for ( long l = 0; l < lNumberOfElements1; l++ )
		{
			bool bAddRequest = false;

			hrStatus = ::SafeArrayGetElement(psaNames, &l, &bstrName);
			if ( FAILED( hrStatus ) ) { break; }

			TmpName = SvUl::createStdString( _bstr_t(bstrName) );
			::SysFreeString(bstrName);

			hrStatus = SafeArrayGetElementNoCopy(psaImages, &l, &bstrImage);
			if ( FAILED( hrStatus ) ) { break; }

			//GetInspectionObject is only true if the pointer is valid
			if ( nullptr != pConfig &&  pConfig->GetInspectionObject(TmpName.c_str(), &pInspection)  && nullptr != pInspection )
			{
				SVObjectClass *l_pObject = nullptr;
				//got the inspection.
				if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(TmpName.c_str(), l_pObject))
				{
					l_pImageObject = dynamic_cast< SVImageClass* >( l_pObject );

					if ( nullptr != l_pImageObject &&
						(l_pImageObject->ObjectAttributesAllowed() & SvDef::SV_REMOTELY_SETABLE) == SvDef::SV_REMOTELY_SETABLE )
					{
						// currently all SvDef::SV_REMOTELY_SETABLE parameters are also SvDef::SV_SETABLE_ONLINE
						// if this changes, this code needs updated
						bAddRequest = true;
					}// end if
					else
					{
						// Item is not allowed to be set remotely
						hrStatus = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;
						::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
					}// end else
				}// end if
				else
				{
					// object not found.  send back status
					hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
					l_bItemNotFound = true;
				}// end else
			}// end if
			else
			{
				//did not find inspection.
				//put an error back into the list
				hrStatus = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
				l_bItemNotFound = true;
			} // end else

			if ( bAddRequest )
			{
				SVImageInfoClass l_ImageInfo;

				SVInputImageRequestInfoStruct *pInRequest = new SVInputImageRequestInfoStruct;
				pInRequest->m_ObjectName = TmpName;

				l_ImageInfo = l_pImageObject->GetImageInfo();

				if ( S_OK == SVImageProcessingClass::LoadImageBuffer( (void*)bstrImage, 
					pInRequest->m_ImageInfo, 
					pInRequest->m_ImageHandlePtr, 
					l_ImageInfo ) )
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
	} while(0);

	hr = hrStatus;
	if ( l_bItemNotFound )
	{
		hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}

	return hr;
}

namespace
{
	inline std::string StripBrackets(const std::string& rValue)
	{
		std::string Result( rValue );
		Result = SvUl::Trim( Result );
		size_t Pos = Result.rfind('[');
		if( std::string::npos != Pos )
		{
			Result = SvUl::Left(Result, Pos);
		}
		return Result;
	}

	inline std::string StripQuotes(const std::string& rValue)
	{
		std::string Result( rValue );
		Result = SvUl::Trim( Result );
		Result = SvUl::Trim( Result, _T("`\'") ); // strip single quotes or back-ticks
		return Result;
	}
}

HRESULT CSVCommand::SVSetToolParameterList(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus)
{
	USES_CONVERSION;

	HRESULT Result = S_OK;
	HRESULT Status = S_OK;

	long NumberOfElements = psaNames->rgsabound[0].cElements;
	long NumberOfElementsValues = psaValues->rgsabound[0].cElements;

	if ( 0 == NumberOfElements )
	{
		Result = SVMSG_REQUESTED_LIST_IS_EMPTY;
		return Result;
	}
	else if(NumberOfElements != NumberOfElementsValues)
	{
		Result = SVMSG_TOO_MANY_REQUESTED_ITEMS;
		return Result;
	}

	SVConfigurationObject*	pConfig = nullptr;
	SVNameStorageMap		ParameterObjects;

	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	for ( long l = 0; l < NumberOfElements; l++ )
	{
		BSTR bstrName = nullptr;
		Status = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		if ( FAILED( Status ) ) { break; }

		std::string Name = SvUl::createStdString( bstrName );

		BSTR bstrValue = nullptr;
		Status = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
		if ( FAILED( Status ) ) { break; }

		std::string Value = SvUl::createStdString( bstrValue );

		//!Check if an inspection name
		SVInspectionProcess*   pInspection(nullptr);
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
		SVSAFEARRAY	Values;

		//Values must be in array format
		Values.Add(_variant_t( bstrValue ));
		Parameter.m_StorageType = SVVisionProcessor::SVStorageValue;
		Parameter.m_Variant = Values;
		ParameterObjects[std::string(Name)] = Parameter;
	}// end for ( long l = 0; l < lNumberOfElements; l++ )
	Result = Status;

	if(S_OK == Result && 0 != ParameterObjects.size())
	{
		SVNameStatusMap SetItemsResult;
		Result = SVVisionProcessorHelper::Instance().SetItems(ParameterObjects, SetItemsResult, false);
	}

	return Result;
}

HRESULT CSVCommand::SVLockImage(long ProcessCount, long Index, BSTR bName)
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( nullptr == pConfig || !pConfig->IsConfigurationLoaded() )
	{
		hr = SVMSG_CONFIGURATION_NOT_LOADED;
		return hr;
	}

	// 1) Inspection exists
	// 2) Requested data item exists
	// 3) all data items are on the same PPQ
	SVActiveXLockStruct SVaxls;

	if(Index < m_aSVActXLock.size())	// Check Lock Structure.
	{

		SVaxls = m_aSVActXLock[Index];

		if(SVaxls.Valid())	// Image is Locked so Release
		{
			SVaxls.clear();
			m_aSVActXLock[Index] = SVaxls;
		}
	}

	SVImageClass* pImage = nullptr;
	SVImageIndexStruct l_svImageIndex;
	SVDataManagerHandle l_DMImageIndexHandle;

	SVInspectionProcess* pInspection = nullptr;

	std::string TmpName = SvUl::createStdString( _bstr_t(bName) );
	//GetInspectionObject is only true if the pointer is valid
	if ( nullptr != pConfig && pConfig->GetInspectionObject( TmpName.c_str(), &pInspection) )
	{
		if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(TmpName.c_str(), pImage))
		{
			SVPPQObject* pPPQ = pInspection->GetPPQ();	// inspection can be part of only one PPQ

			if( nullptr != pPPQ )
			{
				SVProductInfoStruct ProductInfo;

				if( S_OK == pPPQ->GetProduct( ProductInfo, ProcessCount ) )
				{
					SVaxls.m_Name = TmpName;
					SVaxls.m_ProcessCount = ProcessCount;
					if( SVCameraImageTemplate* pMainImage = dynamic_cast<SVCameraImageTemplate*>( pImage ) )
					{
						l_svImageIndex = pMainImage->GetSourceImageIndex( &ProductInfo.oPPQInfo.m_ResultImagePublishedDMIndexHandle, ProductInfo.m_svCameraInfos );
						l_DMImageIndexHandle.Assign( l_svImageIndex.m_CameraDMIndexHandle, SV_DCOM );
					}
					else if ( pImage->ObjectAttributesSet() & SvDef::SV_PUBLISH_RESULT_IMAGE )
					{
						l_svImageIndex = pImage->GetSourceImageIndex( &ProductInfo.oPPQInfo.m_ResultImagePublishedDMIndexHandle, ProductInfo.m_svCameraInfos );
						l_DMImageIndexHandle.Assign( l_svImageIndex.m_PublishedResultDMIndexHandle, SV_DCOM );
					}
					else	// locking not supported on regular images
					{
						hr = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE;
					}// end else
				}// end if (pPPQ->GetProductInfoStruct(lProcessCount, &pProductInfoStruct))
				else
				{
					hr = SVMSG_PRODUCT_NO_LONGER_AVAILABLE;	// product no longer alive
				}
			}// end if( nullptr != pPPQ )
			else
			{
				hr = SVMSG_NO_PPQ_FOUND;
			}
		}
		else	// couldn't find data object
		{
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}
	else	// couldn't find inspection
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}

	// Call Lock Image
	if ( S_OK == hr )
	{
		if ( nullptr != pImage && !l_svImageIndex.IsNull() )
		{
			SVImageInfoClass l_svImageInfo = pImage->GetImageInfo();
			SvOi::SVImageBufferHandlePtr l_svImageHandle;

			if( pImage->GetImageHandle( l_svImageIndex, l_svImageHandle ) && nullptr != l_svImageHandle)
			{
				SVImageInfoClass l_ImageInfo = l_svImageInfo;

				if ( S_OK == SVImageProcessingClass::CreateImageBuffer( l_ImageInfo, SVaxls.m_ImageHandlePtr ) && nullptr != SVaxls.m_ImageHandlePtr)
				{
					HRESULT l_Code = SVMatroxBufferInterface::CopyBuffer(SVaxls.m_ImageHandlePtr->GetBuffer(), l_svImageHandle->GetBuffer() );

					// Add locked image to Lock array
					if (Index >= static_cast<long> (m_aSVActXLock.size()))
					{
						m_aSVActXLock.resize(Index+1);
					}
					m_aSVActXLock[Index] = SVaxls;// Store Info in lock array
				}
				else
				{
					hr = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE;
				}
			}
			else
			{
				hr = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE;
			}
		}
		else
		{
			hr = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE;
		}
	}

	return hr;
}

HRESULT CSVCommand::SVGetLockedImage(long Index, long Compression, BSTR* bstrImage, BSTR* bstrOverlay)
{
	SVActiveXLockStruct SVaxls;
	HRESULT hr = S_FALSE;

	do
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if ( nullptr == pConfig || !pConfig->IsConfigurationLoaded() )
		{
			hr = SVMSG_CONFIGURATION_NOT_LOADED;
			break;
		}

		if( Index >= static_cast<long> (m_aSVActXLock.size()) )
		{
			hr = SVMSG_IMAGE_NOT_LOCKED;
			break;
		}

		SVaxls = m_aSVActXLock[Index];
		if( !SVaxls.Valid() )
		{
			hr = SVMSG_IMAGE_NOT_LOCKED;
			break;
		}

		// for right now locked image will not have overlays
		// this is because the overlay data is not bucketized
		*bstrOverlay = nullptr;

		// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
		// Product is still valid; 
		// Build the output images from the result data index in the product and the source images by name  (?)

		SVImageInfoClass l_ImageInfo;

		// put image in return array
		hr = ImageToBSTR( l_ImageInfo, SVaxls.m_ImageHandlePtr, bstrImage);

		break;
	} while (false);

	return hr;
}

HRESULT CSVCommand::SVUnlockImage(long Index)
{
	HRESULT hres = S_OK;
	if( Index < static_cast<long> (m_aSVActXLock.size()) )
	{
		SVActiveXLockStruct SVaxls;

		SVaxls = m_aSVActXLock[Index];
		if( !SVaxls.Valid() )
		{
			return SVMSG_IMAGE_NOT_LOCKED;
		}

		SVaxls.clear();
		m_aSVActXLock[Index] = SVaxls;
	}

	return hres;
}

HRESULT CSVCommand::SVUnlockAllImages()
{
	HRESULT hr = S_OK;

	for (int x = static_cast<int>(m_aSVActXLock.size() - 1); x > -1; x--)
	{
		SVActiveXLockStruct& rSVaxls = m_aSVActXLock[x];
		if(rSVaxls.Valid())
		{
			rSVaxls.clear();
		}
	}

	return hr;
}

STDMETHODIMP CSVCommand::SVGetRemoteInputCount(long *lCount)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
			if( nullptr != pInputObjectList && pInputObjectList->GetRemoteInputCount( *lCount ) )
			{
				bSuccess = true;
			}
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	if( !bSuccess )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		hrResult = S_FALSE;
	}

	return hrResult;
}// end SVGetRemoteInputCount

STDMETHODIMP CSVCommand::SVSetRemoteInput(long lIndex, VARIANT vtValue)
{
	HRESULT hrResult = S_OK;
	bool bSuccess = false;

	try
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList( );
			if( nullptr != pInputObjectList && pInputObjectList->SetRemoteInput( lIndex, _variant_t(vtValue) ) )
			{
				bSuccess = true;
			}
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	if( !bSuccess )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		hrResult = SVMSG_CMDCOMSRV_ERROR;
	}

	return hrResult;
}// end SVSetRemoteInput

// New functions to support Matrox Font Aalyzers - // Stage 1 runtime only, no setup yet.
SVMatroxBuffer CSVCommand::CreateImageFromBSTR( BSTR bstrImage )
{
	SVMatroxBuffer l_ImageBuf;
	BITMAPINFOHEADER* pbmhInfo;
	BITMAPINFO* pbmInfo;
	BYTE* pBits;
	long lTabSize;
	long lNumColor;

	// Make sure color table size is calculated
	pbmInfo  = (BITMAPINFO*) (BYTE*)bstrImage;
	pbmhInfo = (BITMAPINFOHEADER*) &pbmInfo->bmiHeader;

	lNumColor = pbmhInfo->biClrUsed;
	if( 0 == lNumColor )
	{
		if( pbmhInfo->biBitCount != 24 )
			lNumColor = 1 << pbmhInfo->biBitCount;
	}// end if

	lTabSize = lNumColor * sizeof( RGBQUAD );
	pBits    = ((BYTE*)bstrImage) + sizeof( BITMAPINFOHEADER ) + lTabSize;

	if ( pbmhInfo->biBitCount == 16 ||
		pbmhInfo->biBitCount == 32 )
	{
		return l_ImageBuf; // this is an error
	}// end if

	SVImageInfoClass oTempInfo;
	SvOi::SVImageBufferHandlePtr oTempHandle;

	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, pbmhInfo->biBitCount );
	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );
	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );

	oTempInfo.SetExtentProperty( SVExtentPropertyHeight, abs(pbmhInfo->biHeight) );
	oTempInfo.SetExtentProperty( SVExtentPropertyWidth, pbmhInfo->biWidth );

	if( pbmhInfo->biBitCount == 24 )
	{
		oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 );
		oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3 );
	}// end if

	if( S_OK != SVImageProcessingClass::CreateImageBuffer( oTempInfo, oTempHandle ) || nullptr == oTempHandle)
	{
		return l_ImageBuf;
	}

	// Copy the bits into the image object
	if( nullptr != pBits && nullptr != oTempHandle)
	{
		// Set buffer data...
		memcpy( oTempHandle->GetBufferAddress(), pBits, pbmhInfo->biSizeImage );
	}// end if

	return oTempHandle->GetBuffer();
};// end CreateImageFromBSTR

STDMETHODIMP CSVCommand::SVConnectFont(long* lFontIdentifier)
{
	long lIndentifier;
	HRESULT hr = SVMSG_FAILED_TO_ADD_FONT;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.AddFont( lIndentifier ) )
		{
			*lFontIdentifier = lIndentifier;
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SVConnectFont

STDMETHODIMP CSVCommand::SVDisconnectFont(long lFontIdentifier)
{
	HRESULT hr = SVMSG_FAILED_TO_REMOVE_FONT;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.RemoveFont( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SVDisconnectFont

STDMETHODIMP CSVCommand::SVGetCellSize(long lFontIdentifier, long* plWidth, long* plHeight, long* plSpacing)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetCellSize

STDMETHODIMP CSVCommand::SVSetCellSize(long lFontIdentifier, long lWidth, long lHeight)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetCellSize

STDMETHODIMP CSVCommand::SVGetSearchSettings(long lFontIdentifier, long* plLength, double* pdStringAcceptance, double* pdCharacterAcceptance, BSTR* pbstrInvalidCharacter, BSTR* pbstrSearchSpeed)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetSearchSettings

STDMETHODIMP CSVCommand::SVSetSearchSettings(long lFontIdentifier, long lLength, double dStringAcceptance, double dCharacterAcceptance, BSTR bstrInvalidCharacter, BSTR bstrSearchSpeed)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetSearchSettings

STDMETHODIMP CSVCommand::SVGetPositionVariance(long lFontIdentifier, double* pdXDirection, double* pdYDirection)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetPositionVariance

STDMETHODIMP CSVCommand::SVSetPositionVariance(long lFontIdentifier, double dXDirection, double dYDirection)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetPositionVariance

STDMETHODIMP CSVCommand::SVGetSearchAngleSettings(long lFontIdentifier, double* pdSearchAngle, long* plInterpolation, double* pdDeltaPositive, double* pdDeltaNegative)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetSearchAngleSettings

STDMETHODIMP CSVCommand::SVSetSearchAngleSettings(long lFontIdentifier, double dSearchAngle, long lInterpolation, double dDeltaPositive, double dDeltaNegative)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetSearchAngleSettings

STDMETHODIMP CSVCommand::SVGetCharacterEnhancementSettings(long lFontIdentifier, long* plMorphIterations, double* pdThickenCharacters, long* plBrokenCharacters, long* plTouchingCharacters)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetCharacterEnhancementSettings

STDMETHODIMP CSVCommand::SVSetCharacterEnhancementSettings(long lFontIdentifier, long lMorphIterations, double dThickenCharacters, long lBrokenCharacters, long lTouchingCharacters)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetCharacterEnhancementSettings

STDMETHODIMP CSVCommand::SVLoadFont(long lFontIdentifier, BSTR bstrFontFile, BSTR bstrFontControls, BSTR bstrFontConstraints)
{
	HRESULT hr = S_OK;
	SVMatroxOcr lFontHandle;
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			std::string FontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlffont.mfo"));
			std::string ControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfcont.mfo"));
			std::string ConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfstra.mfo"));
			CFile oFile;

			HRESULT MatroxCode;

			if( nullptr != bstrFontFile )
			{
				oFile.Open( FontFileName.c_str(), CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontFile, ::SysStringByteLen( bstrFontFile ) );
				oFile.Close();

				if ( !lFontHandle.empty() )
				{
					MatroxCode = SVMatroxOcrInterface::Destroy( lFontHandle );
				}
				std::string l_strFontFileName = FontFileName;

				MatroxCode = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strFontFileName, SVOcrRestore );

				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontHandle( lFontIdentifier, lFontHandle );

				if (S_OK != MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}// end if

			if( nullptr != bstrFontControls )
			{
				oFile.Open( ControlsFileName.c_str(), CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontControls, ::SysStringByteLen( bstrFontControls ) );
				oFile.Close();

				std::string l_strControlsFileName = ControlsFileName;
				MatroxCode = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strControlsFileName, SVOcrLoadControl );
				if (S_OK != MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}// end if

			if( nullptr != bstrFontConstraints )
			{
				oFile.Open( ConstraintsFileName.c_str(), CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontConstraints, ::SysStringByteLen( bstrFontConstraints ) );
				oFile.Close();
				MatroxCode = SVMatroxOcrInterface::RestoreFont( lFontHandle, ConstraintsFileName, SVOcrLoadConstraint );
				if (S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}// end if

			// fill the character's / ID mapping for the font
			double NbChar;
			MatroxCode = SVMatroxOcrInterface::Get( lFontHandle, SVCharNumberInFont, NbChar );
			std::string l_strCharacters;
			MatroxCode = SVMatroxOcrInterface::Get( lFontHandle, SVCharInFont, l_strCharacters );

			TheSVObserverApp.m_mgrRemoteFonts.CreateCharMapping(lFontIdentifier, CString(l_strCharacters.c_str()));
		}// end if
		else
		{
			hr = SVMSG_FONT_INVALID;
		}// end else
	}
	else
	{
		hr = SVMSG_FONT_INVALID;
	}

	return hr;
};// end SVLoadFont

STDMETHODIMP CSVCommand::SVSaveFont(long lFontIdentifier, BSTR* bstrFontFile, BSTR* bstrFontControls, BSTR* bstrFontConstraints)
{
	HRESULT hr = S_OK;
	SVMatroxOcr lFontHandle;
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			std::string strFontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsffont.mfo")).c_str();
			std::string strControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfcont.mfo")).c_str();
			std::string strConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfstra.mfo")).c_str();
			CFileStatus rStatus;
			CFile oFile;

			HRESULT MatroxCode;

			if( nullptr != *bstrFontFile )
			{
				MatroxCode = SVMatroxOcrInterface::SaveFont( lFontHandle, strFontFileName, SVOcrSave );
				if (S_OK != MatroxCode )
				{

					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				CFile::GetStatus( strFontFileName.c_str(), rStatus );
				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);
				*bstrFontFile = ::SysAllocStringByteLen( nullptr, strSize );

				oFile.Open( strFontFileName.c_str(), CFile::modeRead );
				oFile.Read( *bstrFontFile, ::SysStringByteLen( *bstrFontFile ) );
				oFile.Close();
			}// end if

			if( nullptr != *bstrFontControls )
			{
				MatroxCode = SVMatroxOcrInterface::SaveFont( lFontHandle,  strControlsFileName, SVOcrSaveControl );
				if (S_OK !=  MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				CFile::GetStatus( strControlsFileName.c_str(), rStatus );

				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);
				*bstrFontControls = ::SysAllocStringByteLen( nullptr, strSize );

				oFile.Open( strControlsFileName.c_str(), CFile::modeRead );
				oFile.Read( *bstrFontControls, ::SysStringByteLen( *bstrFontControls ) );
				oFile.Close();
			}// end if

			if( nullptr != *bstrFontConstraints )
			{
				MatroxCode = SVMatroxOcrInterface::SaveFont( lFontHandle, strConstraintsFileName, SVOcrSaveConstraint );
				if (S_OK != MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				CFile::GetStatus( strConstraintsFileName.c_str(), rStatus );
				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);

				*bstrFontConstraints = ::SysAllocStringByteLen( nullptr, strSize );

				oFile.Open( strConstraintsFileName.c_str(), CFile::modeRead );
				oFile.Read( *bstrFontConstraints, ::SysStringByteLen( *bstrFontConstraints ) );
				oFile.Close();
			}// end if
		}// end if
		else
		{
			hr = SVMSG_FONT_INVALID;
		}// end else
	}
	else
	{
		hr = SVMSG_FONT_INVALID;
	}
	return hr;
};// end SVSaveFont

STDMETHODIMP CSVCommand::SVCalibrateFont(long lFontIdentifier, 
										 BSTR bstrCalibrateString, 
										 BSTR bstrCalibrateImage, 
										 double dXMinSize, 
										 double dXMaxSize, 
										 double dXStepSize, 
										 double dYMinSize, 
										 double dYMaxSize, 
										 double dYStepSize )
{
	CString strCalibrate( bstrCalibrateString, ::SysStringLen( bstrCalibrateString ) );
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	HRESULT hr = SVSetStringLength( lFontIdentifier, strCalibrate.GetLength() );

	if ( S_OK == hr )
	{
		HRESULT MatroxCode(S_OK);
		SVMatroxOcr lFontHandle;

		if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
		{
			if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
			{
				SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrCalibrateImage );

				SVMatroxOcrCalibrateStruct l_CalStruct;
				l_CalStruct.m_ImageBuff = l_milImage;
				l_CalStruct.m_strCalString = strCalibrate;
				l_CalStruct.m_TargetCharSizeXMax = dXMaxSize;
				l_CalStruct.m_TargetCharSizeXMin = dXMinSize;
				l_CalStruct.m_TargetCharSizeYMax = dYMaxSize;
				l_CalStruct.m_TargetCharSizeYMin = dYMinSize;
				l_CalStruct.m_TargetCharSizeXStep = dXStepSize;
				l_CalStruct.m_TargetCharSizeYStep = dYStepSize;

				MatroxCode = SVMatroxOcrInterface::CalibrateFont( lFontHandle, l_CalStruct );
				if (S_OK !=  MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVCalibrateFont"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_45_CALIBRATEFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
					//return some mil error
				}
				l_milImage.clear();
			}// end if
			else
			{
				hr = SVMSG_FONT_INVALID;
			}// end else
		}
		else
		{
			hr = SVMSG_FONT_INVALID;
		}
	}

	return hr;
};// end SVCalibrateFont

STDMETHODIMP CSVCommand::SVReadString(long lFontIdentifier, BSTR* bstrFoundString, BSTR bstrReadImage, double* dMatchScore )
{
	HRESULT hr = S_OK;
	SVMatroxOcr lFontHandle;
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			HRESULT MatroxCode;
			SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrReadImage );
			SVMatroxIdentifier milResult = M_NULL;
			MatroxCode = SVMatroxOcrInterface::CreateResult( milResult );
			// BRW - This l_Code is never checked.

			lFontHandle.m_bVerify = false;
			MatroxCode = SVMatroxOcrInterface::Execute( milResult, lFontHandle, l_milImage );

			if (S_OK != MatroxCode)
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SvDef::StringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}

			// Process the OCR chars returned from MocrReadString();
			long l_lLength = 0L;

			MatroxCode = SVMatroxOcrInterface::GetResult( milResult, SVOcrResultStringSize, l_lLength );

			if( l_lLength != 0 )
			{
				MatroxCode = SVMatroxOcrInterface::GetResult(milResult, SVOcrStringScore, *dMatchScore );
				// BRW - This l_Code is never checked.
				std::string Text;
				MatroxCode = SVMatroxOcrInterface::GetResult(milResult, SVOcrString, Text );
				*bstrFoundString = _bstr_t(Text.c_str()).Detach();
			}// end if
			else
			{
				*dMatchScore = 0.0F;
				*bstrFoundString = _bstr_t(_T("")).Detach();
			}// end else

			l_milImage.clear();
			if (S_OK !=  MatroxCode)
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SvDef::StringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}

			MatroxCode = SVMatroxOcrInterface::DestroyResult( milResult );
			if (S_OK !=  MatroxCode)
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SvDef::StringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}// end if
		else
		{
			hr = SVMSG_FONT_INVALID;
		}// end else
	}
	else
	{
		hr = SVMSG_FONT_INVALID;
	}
	return hr;
};// end SVReadString

STDMETHODIMP CSVCommand::SVVerifyString(long lFontIdentifier, BSTR bstrVerifyString, BSTR bstrVerifyImage, double* dMatchScore )
{
	CString strVerify( bstrVerifyString, ::SysStringLen(bstrVerifyString));
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	HRESULT hr = SVSetStringLength( lFontIdentifier, strVerify.GetLength() );

	if ( S_OK == hr )
	{
		SVMatroxOcr lFontHandle;

		if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
		{
			if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
			{
				HRESULT MatroxCode(S_OK);
				SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrVerifyImage );
				SVMatroxIdentifier milResult = M_NULL;
				MatroxCode = SVMatroxOcrInterface::CreateResult( milResult );

				if (S_OK !=  MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				lFontHandle.m_bVerify = true;
				lFontHandle.m_VerifyString = strVerify;

				SVMatroxOcrInterface::Execute( milResult, lFontHandle, l_milImage );

				if (S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				// Process the OCV chars returned from MocrVerifyString();
				long l_lLength = 0L;

				MatroxCode = SVMatroxOcrInterface::GetResult( milResult, SVOcrResultStringSize, l_lLength);

				if( l_lLength != 0L )
				{
					MatroxCode = SVMatroxOcrInterface::GetResult( milResult, SVOcrStringScore, *dMatchScore );
				}// end if
				else
				{
					*dMatchScore = 0.0F;
				}// end else

				l_milImage.clear();

				if( S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				MatroxCode = SVMatroxOcrInterface::DestroyResult( milResult );

				if( S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SvDef::StringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}// end if
			else
			{
				hr = SVMSG_FONT_INVALID;
			}// end else
		}
		else
		{
			hr = SVMSG_FONT_INVALID;
		}
	}

	return hr;
};// end SVVerifyString

STDMETHODIMP CSVCommand::SVGetConstraints(long lFontIdentifier, long* plPosition, long* plType, BSTR* pbstrValues)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetConstraints

STDMETHODIMP CSVCommand::SVSetConstraints(long lFontIdentifier, long lPosition, long lType, BSTR bstrValues)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetConstraints

STDMETHODIMP CSVCommand::SVCreateNew(long lFontIdentifier, BSTR bstrImage)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end CreateNew

STDMETHODIMP CSVCommand::SVSetTrainingImage(long lFontIdentifier, BSTR bstrImage)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end SetTrainingImage

STDMETHODIMP CSVCommand::SVAddCharacter(long lFontIdentifier, long lXPosition, long lYPosition, long lHeight, long lWidth, BSTR bstrLabel, long *lCharId)
{
	HRESULT hr = SVMSG_FONT_INVALID; // Invalid Font
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			SVMatroxBuffer milFontImage;
			if ( TheSVObserverApp.m_mgrRemoteFonts.GetFontImage(lFontIdentifier, milFontImage) )
			{
				HRESULT MatroxCode;

				long lOcrWidth;
				long lOcrHeight;

				MatroxCode = SVMatroxOcrInterface::Get(lFontHandle, SVOcrCharSizeX, lOcrWidth );
				KeepPrevError( MatroxCode,SVMatroxOcrInterface::Get(lFontHandle, SVOcrCharSizeY, lOcrHeight ));

				SVMatroxBuffer milTmpID;
				SVMatroxBufferCreateChildStruct l_Create(milFontImage);
				l_Create.m_lOffX = lXPosition;
				l_Create.m_lOffY = lYPosition;
				l_Create.m_lSizeX = lWidth;
				l_Create.m_lSizeY = lHeight;

				SVMatroxBufferInterface::Create(milTmpID, l_Create );
				if (  !milTmpID.empty() )
				{
					try
					{
						SVMatroxOcr milNewFontID;
						if ( TheSVObserverApp.m_mgrRemoteFonts.CopyFont(lFontHandle,milNewFontID,1) )
						{
							long lId;
							std::string Text = SvUl::createStdString( _bstr_t(bstrLabel) );

							KeepPrevError( MatroxCode, SVMatroxOcrInterface::CopyFont(milNewFontID, milTmpID, SVOcrCopytoFont, Text));
							if ( S_OK != MatroxCode )
							{
								TheSVObserverApp.m_mgrRemoteFonts.UpdateFontHandle( lFontIdentifier,milNewFontID);
								TheSVObserverApp.m_mgrRemoteFonts.AddFontChar( lFontIdentifier, Text[0], &lId);

								*lCharId = lId;
								SVMatroxOcrInterface::Destroy( lFontHandle );
							}
							else
							{
								//there was an error.  
								hr = SVMSG_FONT_COPY_FAIL;
								return hr;
							}
						}
						else
						{
							hr = SVMSG_FONT_COPY_FAIL;
						}
					}
					catch(...)
					{
						hr = SVMSG_FONT_COPY_THREW_EXCEPTION;
					}
				}
				milTmpID.clear();

				hr = S_OK;
			}
			else
			{
				hr = SVMSG_FONT_IMAGE_FAIL;
			}
		}// end if
	}

	return hr;
};// end AddCharacter

STDMETHODIMP CSVCommand::SVAddCharacters(long lFontIdentifier, BSTR bstrLabelList)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end AddCharacters

STDMETHODIMP CSVCommand::SVDeleteCharacters(long lFontIdentifier, SAFEARRAY* psaCharIds)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			long lNumberOfChars = psaCharIds->rgsabound[0].cElements;

			for ( long l = 0; l < lNumberOfChars; l++ )
			{
				unsigned long ulVal;

				HRESULT hrStatus = SafeArrayGetElementNoCopy(psaCharIds, &l, &ulVal);
				if (FAILED(hrStatus))
				{
					hr = S_FALSE;
					break;
				}
				TheSVObserverApp.m_mgrRemoteFonts.DeleteFontChar(lFontIdentifier, lFontHandle, ulVal);
			}

			hr = S_OK;
		}// end if
	}
	return hr;
};// end DeleteCharacters

STDMETHODIMP CSVCommand::SVThresholdImage(long lFontIdentifier, BSTR bstrImage, long lThreshold, BSTR* pbstrThresholdImage)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end ThresholdImage

STDMETHODIMP CSVCommand::SVGetFontCharacterCount(long lFontIdentifier, long* plCharacterCount)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			hr = S_OK;
		}// end if
	}
	return hr;
};// end GetFontCharacterCount

STDMETHODIMP CSVCommand::SVGetFontCharacter(long lFontIdentifier, long  lCharID, BSTR* pbstrLabelImage)
{
	HRESULT hr = SVMSG_FONT_INVALID;
	SVMatroxOcr lFontHandle;
	HRESULT l_Code;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			SVMatroxBuffer lCharHandle;
			if ( TheSVObserverApp.m_mgrRemoteFonts.GetFontCharImage(lFontIdentifier, lFontHandle, lCharID, lCharHandle) )
			{
				SVImageInfoClass ImageInfo;

				long l_lValue = 0;

				ImageInfo.SetExtentProperty(SVExtentPropertyOutputPositionPoint, 0);
				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVSizeX, l_lValue );
				ImageInfo.SetExtentProperty(SVExtentPropertyWidth, l_lValue );

				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVSizeY, l_lValue );
				ImageInfo.SetExtentProperty(SVExtentPropertyHeight, l_lValue );

				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVSizeBand, l_lValue );
				ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lValue );

				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVType, l_lValue );
				ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, l_lValue );

				SvOi::SVImageBufferHandlePtr ImageBufferHandle{ new SVImageBufferHandleImage(lCharHandle) };

				ImageToBSTR( ImageInfo, ImageBufferHandle, pbstrLabelImage);
				lCharHandle.clear();

				hr = S_OK;
			}
			else
			{
				hr = SVMSG_NO_FONT_FOUND;
			}
		}// end if
	}
	return hr;
};// end GetFontCharacter

STDMETHODIMP CSVCommand::SVGetStringLength(long p_lFontIdentifier, double* p_plLength )
{
	HRESULT l_hr = S_FALSE;
	SVMatroxOcr l_lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, l_lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ))
		{
			SVMatroxOcrInterface::Get(l_lFontHandle, SVOcrStringSize, *p_plLength );

			l_hr = S_OK;
		}// end if
	}
	return l_hr;
};// end GetCellSize

STDMETHODIMP CSVCommand::SVSetStringLength(long p_lFontIdentifier, double p_dLength )
{
	HRESULT l_hr = S_FALSE;
	SVMatroxOcr l_lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, l_lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ))
		{
			double l_dMaxLength = 0.0;

			SVMatroxOcrInterface::Get(l_lFontHandle, SVOcrStringSizeMax, l_dMaxLength );

			if ( l_dMaxLength < p_dLength )
			{
				l_hr = SVMSG_43_FONT_STR_LEN_GREATER_THAN_MAX;
			}
			else
			{
				SVMatroxOcrInterface::Set( l_lFontHandle, SVOcrStringSize, p_dLength );

				l_hr = S_OK;
			}
		}// end if
	}
	return l_hr;
};// end SetCellSize

// New functions to support Matrox Font Aalyzers - // Stage 1 runtime only, no setup yet.

// New functions for OCR

STDMETHODIMP CSVCommand::SVLoadFontImage(long p_lFontIdentifier, BSTR bstrFontImage)
{
	HRESULT l_hr = S_OK;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ))
		{
			SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrFontImage );

			TheSVObserverApp.m_mgrRemoteFonts.AddFontImage(p_lFontIdentifier, l_milImage);
		}
	}

	return l_hr;
}

STDMETHODIMP CSVCommand::SVGetFontCharacterList(long p_lFontIdentifier, BSTR *bstrCharacterList, SAFEARRAY** ppsaCharIds)
{
	HRESULT l_hr = S_OK;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ) )
		{
			double NbChar;
			SVMatroxOcrInterface::Get(lFontHandle, SVCharNumberInFont, NbChar );
			std::string Characters;
			SVMatroxOcrInterface::Get( lFontHandle, SVCharInFont, Characters );

			*bstrCharacterList = _bstr_t( Characters.c_str() ).Detach();

			SAFEARRAYBOUND saBounds[1];
			saBounds[0].lLbound=0;
			saBounds[0].cElements = static_cast<long> (Characters.size());

			*ppsaCharIds  = ::SafeArrayCreate( VT_UI4, 1, saBounds);

			TheSVObserverApp.m_mgrRemoteFonts.GetCharIdSafeArray(p_lFontIdentifier, ppsaCharIds);
		}
		else
		{
			l_hr = S_FALSE;
		}
	}
	else
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

STDMETHODIMP CSVCommand::SVGetExpectedTargetCharacterSize(long p_lFontIdentifier, long *plWidth, long *plHeight)
{
	HRESULT l_hr = S_OK;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ) )
		{
			SVMatroxOcrInterface::Get( lFontHandle, SVTargetCharSizeX, *plWidth );
			SVMatroxOcrInterface::Get( lFontHandle, SVTargetCharSizeY, *plHeight );
		}
		else
		{
			l_hr = SVMSG_NO_FONT_FOUND;
		}
	}
	else
	{
		l_hr = SVMSG_NO_FONT_FOUND;
	}
	return l_hr;
}

STDMETHODIMP CSVCommand::SVGetFontCharacterSize(long p_lFontIdentifier, long *plWidth, long *plHeight) 
{
	HRESULT l_hr = S_OK;
	SVMatroxOcr lFontHandle;

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( p_lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( p_lFontIdentifier ) )
		{
			HRESULT l_Code;

			l_Code = SVMatroxOcrInterface::Get( lFontHandle, SVCharCellSizeX, *plWidth);
			l_Code = SVMatroxOcrInterface::Get( lFontHandle, SVCharCellSizeY, *plHeight);
		}
		else
		{
			l_hr = SVMSG_NO_FONT_FOUND;
		}
	}
	else
	{
		l_hr = SVMSG_NO_FONT_FOUND;
	}
	return l_hr;
}

HRESULT CSVCommand::RebuildStreamingDataList()
{
	HRESULT hr = S_OK;
	SVObjectClass* pTempObject;
	for(auto pStreamData : m_arStreamList)
	{
		if( nullptr != pStreamData )
		{
			if ( !pStreamData->m_InspectionID.empty() )
			{
				SVObjectNameInfo l_NameInfo;

				SVObjectClass* l_pInspection = SVObjectManagerClass::Instance().GetObject( pStreamData->m_InspectionID );

				hr = l_NameInfo.ParseObjectName( pStreamData->strValueName.c_str() );

				if( S_OK == hr && nullptr != l_pInspection && !( l_NameInfo.m_NameArray.empty() ) && l_NameInfo.m_NameArray[ 0 ] == l_pInspection->GetName() )
				{
					SVObjectManagerClass::Instance().GetObjectByDottedName(l_NameInfo.GetObjectName(), pTempObject);

					pStreamData->pValueObject = pTempObject;
					SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pStreamData->pValueObject);
					// Validate Array Objects
					if (pStreamData->arrayIndex > 0 && nullptr != pValueObject )
					{
						// Ensure valueObject is an Array
						if( pValueObject->isArray())
						{
							// Ensure index is within bounds
							if (pStreamData->arrayIndex >= pValueObject->getArraySize()) 
							{
								pStreamData->pValueObject = nullptr;
							}
						}
						else
						{
							pStreamData->pValueObject = nullptr;
						}
					}
				}
				else
				{
					pStreamData->pValueObject = nullptr;
				}
			}
			else
			{
				pStreamData->pValueObject = nullptr;
			}
		}// end if
	}// end for
	return hr;
}

void CSVCommand::ResetStreamingDataAndLockedImages()
{
	// Do what CSVCommand::SVUnlockAllImages does
	for( int x = static_cast<int> (m_aSVActXLock.size() - 1);  x > -1; x-- )
	{
		SVActiveXLockStruct& rSVaxls = m_aSVActXLock[x];
		if( rSVaxls.Valid() )
		{
			rSVaxls.clear();
		}// end if
	}// end for

	// Do what CSVCommand::SVUnRegisterStream does
	if ( m_hStopStreamEvent )
	{
		::SetEvent( m_hStopStreamEvent );
		::CloseHandle( m_hStopStreamEvent );
		m_hStopStreamEvent = nullptr;
	}// end if

	if ( m_hStreamingThread )
	{
		::CloseHandle( m_hStreamingThread );
		m_hStreamingThread = nullptr;
	}// end if

	m_bRegisteredStream = false;
	m_bRunStreamData = false;

	SVCommandStreamManager::Instance().EraseCommandCallback();
	m_dwStreamDataProcessId = 0;

	for(auto pStream : m_arStreamList)
	{
		delete pStream;
	}// end for
	m_arStreamList.clear();

	for(auto pProduct : m_arProductList)
	{
		delete pProduct;
	}// end for
	m_arProductList.clear();
}// end if

// This method is used to connect the event object to the application.
HRESULT CSVCommand::StoreEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver )
{
	return S_OK;
}

// This method is used to disconnect the event object to the application.
HRESULT CSVCommand::ReleaseEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver )
{
	return S_OK;
}

// Stub for SVGetTransferValueDefinitionList
STDMETHODIMP CSVCommand::SVGetTransferValueDefinitionList(BSTR bstrInspectionName, 
	long* p_plVersion,
	VARIANT* p_pvData )
{
	USES_CONVERSION;

	if( nullptr == p_pvData )
	{
		return S_FALSE;
	}

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		// Get Data Definition list from inspection
		SVToolSetClass* pToolSet = pInspection->GetToolSet();
		SVTaskObjectListClass* pTaskObjectList = static_cast <SVTaskObjectListClass*> ( pToolSet );

		SVObjectPtrVector SelectedObjects;

		SVOutputInfoListClass l_OutputList;

		pTaskObjectList->GetOutputList( l_OutputList );

		size_t nCount = l_OutputList.GetSize();

		for( int i = 0 ; i < static_cast< int >( nCount ); i++ )
		{
			// Get OutObjectInfoStruct...
			SVOutObjectInfoStruct* pInfoItem = nullptr;

			pInfoItem = l_OutputList.GetAt(i);

			SVObjectReference ObjectRef;
			if( pInfoItem )
			{
				ObjectRef = pInfoItem->GetObjectReference();
			}
			else
			{
				ASSERT(0);
				break;
			}

			SVObjectClass* pObject = ObjectRef.getObject();

			if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
			{
				if( pObject->ObjectAttributesSet() & SvDef::SV_DD_VALUE ) // if Data Definition List set.
				{
					SelectedObjects.push_back( pObject );
				}
			}
		}

		// Copy list to Safearray

		// Get Attributes and add to lists.
		// 1st Version of Data Transfer Definitions....January 2008..TRB

		SAFEARRAYBOUND l_saBounds[2];

		// First Dimension number of objects in list..
		l_saBounds[0].cElements = static_cast< ULONG >( SelectedObjects.size() );
		l_saBounds[0].lLbound = 0;

		// Second Dimension is the parts fo the Transfer Definition
		// 0 = Name, 1 = Writable, 2 = Data Type, 3 = EnumList
		l_saBounds[1].cElements = 4;
		l_saBounds[1].lLbound = 0;

		SAFEARRAY* l_psaData;	//( VT_VARIANT, l_saBounds);
		l_psaData = ::SafeArrayCreate( VT_VARIANT, 2, &l_saBounds[0] );
		long  l_Index[2];
		for( size_t i = 0 ; i < SelectedObjects.size() ; i++ )
		{
			l_Index[0] = static_cast< long >( i );
			// Name
			l_Index[1] = 0;
			_variant_t Value;
			Value.SetString( SelectedObjects[i]->GetCompleteName().c_str() );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value);

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (SelectedObjects[i]->ObjectAttributesAllowed() & SvDef::SV_REMOTELY_SETABLE) == SvDef::SV_REMOTELY_SETABLE;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = l_bWritable;
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );

			// Data Type
			l_Index[1] = 2;
			Value.Clear();
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (SelectedObjects[i]);
			if( nullptr != pValueObject )
			{
				Value.SetString( pValueObject->getTypeName().c_str() );
			}
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );

			// Enumeration List.
			l_Index[1] = 3;
			Value.Clear();
			if( SelectedObjects[i]->GetObjectSubType() == SvDef::SVEnumValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVEnumerateValueObjectClass* pEnumVO = dynamic_cast<SVEnumerateValueObjectClass*>(SelectedObjects[i]);
				if( nullptr != pEnumVO )
				{
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast< ULONG >( pEnumVO->GetEnumVector().size() );
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long i = 0; i < static_cast<long>(pEnumVO->GetEnumVector().size()) ; i++ )
					{
						_bstr_t bstTmp = pEnumVO->GetEnumVector()[i].first.c_str();
						SafeArrayPutElement(l_psaTemp, &i, bstTmp.Detach() );
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;
				}
			}
			else if( SelectedObjects[i]->GetObjectSubType() == SvDef::SVBoolValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<SVBoolValueObjectClass*>(SelectedObjects[i]);
				if( nullptr != l_pBoolVO )
				{
					SvDef::StringVector ValidTypes;
					l_pBoolVO->GetValidTypes( ValidTypes );
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast< ULONG >( ValidTypes.size() );
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long i = 0; i < static_cast<long>(ValidTypes.size()) ; i++ )
					{
						_bstr_t bstTmp = ValidTypes[i].c_str();
						SafeArrayPutElement(l_psaTemp, &i, bstTmp.GetBSTR() );
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;

				}
			}
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value);
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if( nullptr != p_pvData )
		{
			hr = VariantClear( p_pvData );
		}
		_variant_t Value;
		Value.vt = VT_ARRAY | VT_VARIANT ;
		Value.parray = l_psaData ;
		*p_pvData = Value.Detach();
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

// Stub for SVGetTransferImageDefinitionList
STDMETHODIMP CSVCommand::SVGetTransferImageDefinitionList(BSTR bstrInspectionName, 
	long* p_plVersion,
	VARIANT* p_pvData)
{
	USES_CONVERSION;

	if( nullptr == p_pvData )
	{
		return SVMSG_INVALID_VARIANT_PARAMETER;
	}

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		// Get Image list from the tool set.
		SVImageClassPtrVector ImageList;
		SVToolSetClass* pToolSet = pInspection->GetToolSet();
		pToolSet->GetImageList( ImageList );

		std::vector<SVImageClass*> objectList;

		int nCount = static_cast< int >( ImageList.size() );
		for( int i = 0; i < nCount; i++ )
		{
			SVImageClass* pImage = ImageList[i];

			if ( pImage )
			{
				//
				// Check for the required Output object attributes.
				//
				if ( (pImage->ObjectAttributesSet() & SvDef::SV_DD_IMAGE) != 0 )
				{
					objectList.push_back( pImage );
				}
			}
		}

		// Copy list to Safearray

		SAFEARRAYBOUND l_saBounds[2];

		// First Dimension number of objects in list..
		l_saBounds[0].cElements = static_cast< ULONG >( objectList.size() );
		l_saBounds[0].lLbound = 0;

		// Second Dimension is the parts fo the Transfer Definition
		// 0 = Name, 1 = Writable, 2 = Result is Published, 3 = Source Image Name
		l_saBounds[1].cElements = 4;
		l_saBounds[1].lLbound = 0;

		SAFEARRAY* l_psaData; //( VT_VARIANT, l_saBounds);
		l_psaData = ::SafeArrayCreate( VT_VARIANT, 2, l_saBounds );
		long l_Index[2];
		for( size_t i = 0 ; i < objectList.size() ; i++ )
		{
			l_Index[0] = static_cast< long >( i );
			// Name
			l_Index[1] = 0;
			_variant_t Value;
			Value.SetString( objectList[i]->GetCompleteName().c_str() );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value ); 

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (objectList[i]->ObjectAttributesAllowed() & SvDef::SV_REMOTELY_SETABLE) == SvDef::SV_REMOTELY_SETABLE;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = l_bWritable;
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );

			// Published Image
			l_Index[1] = 2;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = (objectList[i]->ObjectAttributesSet() & SvDef::SV_PUBLISH_RESULT_IMAGE) != 0;
			//l_saData.PutElement( l_Index, l_vTmp );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );

			// Fully Qualified Source Image Name
			SVToolClass* pTool = dynamic_cast<SVToolClass*>(objectList[i]->GetTool());
			l_Index[1] = 3;
			Value.Clear();
			if( nullptr != pTool )
			{
				SVStringValueObjectClass* pSourceNames = pTool->GetInputImageNames();
				if( nullptr != pSourceNames )
				{
					SAFEARRAYBOUND l_rgsabound[1];
					long l_lSize = pSourceNames->getArraySize();
					l_rgsabound[0].cElements = l_lSize;
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long l_lIndex = 0; l_lIndex < l_lSize ; l_lIndex++ )
					{
						std::string strTmp;
						HRESULT l_hr = pSourceNames->getValue( strTmp, l_lIndex );
						_bstr_t bstTmp = strTmp.c_str();
						SafeArrayPutElement(l_psaTemp, &l_lIndex, bstTmp.GetBSTR() );
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;
				}
			}
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if( nullptr != p_pvData )
		{
			hr = VariantClear( p_pvData );
		}
		_variant_t Value;
		Value.vt = VT_ARRAY | VT_VARIANT ;
		Value.parray = l_psaData ;
		*p_pvData = Value.Detach();
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP CSVCommand::SVConstructCommand( long p_CommandType, ISVRemoteCommand **p_ppCommand )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_ppCommand )
	{
		if( nullptr != *p_ppCommand )
		{
			( *p_ppCommand )->Release();

			*p_ppCommand = nullptr;
		}

		SVCommandTemplatePtr l_CommandPtr = SVMatroxCommandFactorySingleton::Instance().CreateCommand( p_CommandType );

		if(nullptr == l_CommandPtr)
		{
			l_CommandPtr = SVFileSystemCommandFactorySingleton::Instance().CreateCommand( p_CommandType );
		}

		if(nullptr == l_CommandPtr)
		{
			l_CommandPtr = SVStreamCommandFactorySingleton::Instance().CreateCommand( p_CommandType );
		}

		if(nullptr != l_CommandPtr )
		{
			CComPtr< ISVRemoteCommand > l_RemoteCommandPtr;

			l_Status = l_RemoteCommandPtr.CoCreateInstance( __uuidof( SVRemoteCommand ) );

			if( nullptr == l_RemoteCommandPtr.p )
			{
				if( S_OK == l_Status )
				{
					l_Status = E_FAIL;
				}
			}

			if( S_OK == l_Status )
			{
				// This has issues when using _ATL_DEBUG_INTERFACES...
				SVRemoteCommand* l_pRemoteCommand = dynamic_cast< SVRemoteCommand* >( l_RemoteCommandPtr.p );

				if( nullptr != l_pRemoteCommand )
				{
					l_pRemoteCommand->SetCommand( l_CommandPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			if( S_OK == l_Status )
			{
				l_Status = l_RemoteCommandPtr.QueryInterface( p_ppCommand );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP CSVCommand::SVIsAvailiable()
{
	return S_OK;
}

