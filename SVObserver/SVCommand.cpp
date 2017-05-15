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
#include "SVFileSystemLibrary/SVFileSystemCommandFactory.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleStruct.h"
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
#include "SVResultView.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVOCore/SVImageBuffer.h"	//SVImageOverlayClass
#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"
#include "SVInspectionProcess.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVInputObjectList.h"
#include "SVStreamCommandFactory.h"
#include "SVInputStreamManager.h"
#include "SVOCore/SVTool.h"
#include "SVCommandStreamManager.h"
#include "SVCommandInspectionCollectImageData.h"
#include "GuiCommands/InspectionRunOnce.h"

#include "SVOlicenseManager/SVOLicenseManager.h"
#include "RemoteCommand.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "SVStorageResult.h"
#include "SVVisionProcessorHelper.h"
#include "RangeClassHelper.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStatusLibrary/GlobalPath.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

volatile BOOL CSVCommand::m_bRunStreamData = FALSE;
volatile BOOL CSVCommand::m_bRegisteredStream = FALSE;
volatile DWORD CSVCommand::m_dwStreamDataProcessId = 0;
volatile long CSVCommand::m_lLastStreamedProduct = -1;
volatile HANDLE CSVCommand::m_hStopStreamEvent = nullptr;
volatile HANDLE CSVCommand::m_hStreamingThread = nullptr;
SVStringVector CSVCommand::m_InspectionNames;

SVVector< SVActiveXLockStruct > CSVCommand::m_aSVActXLock;
SVVector< StreamDataStruct* > CSVCommand::m_arStreamList;
SVVector< ProductDataStruct* > CSVCommand::m_arProductList;
SVVector< SVInspectionProcess* > CSVCommand::m_arInspections;
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
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			bSuccess = GlobalRCGetState( ulState );
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( !bSuccess )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}
	} while (0);

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
	HRESULT hrResult;
	BOOL bSuccess;

	if( SVSVIMStateClass::CheckState( SV_STATE_EDITING ) )
	{
		hrResult = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else
	{
		hrResult = S_OK;
		bSuccess = FALSE;

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
				bSuccess = GlobalRCGoOnline();
			}
			else
			{
				bSuccess = GlobalRCGoOffline();
			}
		}
		catch (...)
		{
			bSuccess = FALSE;
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
	SVString PackedFileName;
	SVString ConfigName;
	SVPackedFile PackedFile;
	HRESULT hrResult;
	BOOL bSuccess;
	CFile binFile;
	CFileStatus Status;
	long lSize = 0;
	long lBytesRead = 0;
	long lFileSize = 0;
	long lBytesToGo = 0;
	CFileException *ex;
	BOOL bHrSet = FALSE;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			ConfigName = GlobalRCGetConfigurationName();

			bSuccess = !ConfigName.empty();
			if( bSuccess )
			{
				PackedFileName = ConfigName + _T(".svf");
			}

			// check offset: if zero then it is first time in
			if (lOffset < 1)
			{
				// on first time
				// pack files to temp area before sending packets

				if( bSuccess )
				{
					bSuccess = GlobalRCSaveConfiguration();
				}

				if( bSuccess )
				{
					bSuccess = PackedFile.PackFiles( ConfigName.c_str(), PackedFileName.c_str() );
				}
			}//offset < 1  end of the fist time

			//send data back to control

			if (bSuccess)
			{
				ex = new CFileException;
				if( binFile.Open( PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, ex ) )
				{
					lFileSize = (long)binFile.GetLength();
					lBytesToGo = lFileSize - lOffset;
					if ( (*lBlockSize > lBytesToGo) || (*lBlockSize < 1) )
					{
						*lBlockSize = lBytesToGo;
						*bLastFlag = TRUE;
					}

					*bstrFileData = SysAllocStringByteLen(nullptr, *lBlockSize);
					if ( nullptr == (*bstrFileData) )
					{
						AfxThrowMemoryException( );
					}

					binFile.Seek( lOffset, CFile::begin );
					binFile.Read( *bstrFileData, *lBlockSize );
					// Close the file
					binFile.Close();
					ex->Delete();
				}// end if
				else
				{
					//unable to open file
					bSuccess = FALSE;
					throw (ex);
				}
			}//end if
		}
		catch (CMemoryException *memEx)
		{
			TCHAR szCause[255];
			memEx->GetErrorMessage(szCause,255);
#ifdef _DEBUG
			SVString Formatted = _T("ERROR - ");
			Formatted += szCause;
			ASSERT(false);// szFormatted
#endif
			hrResult = SVMSG_CMDCOMSRV_MEMORY_ERROR;
			bHrSet = TRUE;
			memEx->Delete();
		}
		catch (CFileException *ex)
		{
			TCHAR szCause[255];
			ex->GetErrorMessage(szCause,255);
#ifdef _DEBUG
			SVString Formatted = _T("ERROR - ");
			Formatted += szCause;
			ASSERT(false);//szFormatted
#endif
			hrResult = SVMSG_CMDCOMSRV_FILE_ERROR;
			bHrSet = TRUE;
			ex->Delete();
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( (!bSuccess) && (!bHrSet) )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}
		break;
	} while (0);

	return hrResult;
}// end SVGetSVIMConfig

STDMETHODIMP CSVCommand::SVPutSVIMConfig(long lOffset, long lBlockSize, BSTR *bstrFileData, BOOL bLastFlag)
{
	SVString configFileName;
	SVString PackedFileName;
	SVPackedFile svPackedFile;
	HRESULT hrResult;
	BOOL bSuccess;
	CFile binFile;
	BOOL bRet = FALSE;
	CFileException *ex;
	BOOL bHrSet = FALSE;

	// Check if we are in an allowed state first
	// Not allowed to perform if Mode is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVSVIMStateClass::AddState( SV_STATE_REMOTE_CMD );

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			if( CreateDirPath(SvStl::GlobalPath::Inst().GetTempPath().c_str()) ) 
			{
				PackedFileName = SvStl::GlobalPath::Inst().GetTempPath(_T("temp.svf"));
				ex = new CFileException;
				if (lOffset < 1)
				{
					if (binFile.Open(PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, ex) )
					{
						bRet = TRUE;
					}
					else
					{
						throw (ex);
					}
				}
				else
				{
					if(binFile.Open( PackedFileName.c_str(), CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate  | CFile::typeBinary, ex) )
					{
						bRet = TRUE;
					}
					else
					{
						throw (ex);
					}
				}

				if (bRet)
				{
					binFile.Seek( lOffset, CFile::begin );
					binFile.Write(*bstrFileData,lBlockSize);
					binFile.Close();
					ex->Delete();
					bSuccess = TRUE;
				}
			}

			if (bLastFlag)
			{
				// make sure file exists
				bSuccess = (0 == _access( PackedFileName.c_str(), 0 ) );

				// global function to close config and clean up c:\run dir
				bSuccess = GlobalRCCloseAndCleanConfiguration();

				if( bSuccess )
				{
					//unpack the files in the c:\run directory
					bSuccess = svPackedFile.UnPackFiles( PackedFileName.c_str(), SvStl::GlobalPath::Inst().GetRunPath().c_str() );
					if (!bSuccess)
					{
						hrResult = SVMSG_CMDCOMSRV_PACKEDFILE_ERROR;
						bHrSet = TRUE;
						SvStl::MessageMgrStd Exception( SvStl::LogOnly );
						Exception.setMessage( hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
					}
				}

				if( bSuccess )
				{
					// check for a good path on the config in the packed file
					configFileName = svPackedFile.getConfigFilePath();
					bSuccess = !( configFileName.empty() );
				}

				if( bSuccess )
				{
					// make sure file exists
					bSuccess = ( 0 == _access( configFileName.c_str(), 0 ) );
				}

				if( bSuccess )
				{
					//load the config
					bSuccess = GlobalRCOpenConfiguration( configFileName.c_str() );
				}
			}
		}
		catch (CFileException *theEx)
		{
			TCHAR szCause[255];
			theEx->GetErrorMessage(szCause,255);
#ifdef _DEBUG
			SVString Formatted = _T("Error - ");
			Formatted += szCause;
			ASSERT(false);//sFormatted
#endif
			hrResult = theEx->m_lOsError;
			theEx->Delete();
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( (!bSuccess) && (!bHrSet) )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}
		break;
	} while (0);

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
					*lLastPacketFlag = TRUE;
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
				SVString Formatted = _T("ERROR - ");
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
				SVString Formatted = _T("ERROR - ");
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

	SVString Path;
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
			SVString Formatted = _T("ERROR - ");
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
	SVString ConfigFile;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH];
	HRESULT hrResult;
	BOOL bSuccess;
	CFileException ex;
	BOOL bHrSet = TRUE;

	// Check the mode first - Not allowed to perform if Mode is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}

	SVSVIMStateClass::AddState( SV_STATE_REMOTE_CMD );

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			ConfigFile = SvUl_SF::createSVString( _bstr_t(bstrConfigFilename) );

			//split filename into peices
			_tsplitpath( ConfigFile.c_str(), szDrive, szDir, szFile, szExt );

			if( !_tcscmp( szDrive, _T("") ) )
			{ //just the file name, search the run directory for the filename
				if( 0 == _tcscmp( szExt, _T( ".svx" ) ) || 0 == _tcscmp( szExt, _T( "" ) ) )
				{
					_tmakepath( szPath, _T("C"), _T("\\Run\\"), szFile, _T("svx") );
					//check for existence of file first
					bSuccess = ( 0 == _access( szPath, 0 ) );

					if( bSuccess )
					{
						ConfigFile = szPath;
						//global function to close config and clean up c:\run dir
						bSuccess = GlobalRCCloseAndCleanConfiguration();
					}
				}
				else
				{
					bSuccess = FALSE;
				}
			}
			else if( 0 == _tcscmp( szExt, _T( ".svx" ) ) ) //fully qualified path with svx extension
			{
				//check for existence of file first
				bSuccess = ( 0 == _access( ConfigFile.c_str(), 0 ) );

				if( bSuccess )
				{
					//global function to close config and clean up c:\run dir
					bSuccess = GlobalRCCloseAndCleanConfiguration();
				}
				else
				{
					throw ( (CFileException*)(&ex) );
				}
			}
			else
			{
				bSuccess = FALSE;
			}

			if( bSuccess )
			{
				bSuccess = GlobalRCOpenConfiguration( ConfigFile.c_str() );
			}
		}
		catch (CFileException* &theEx)
		{
			hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;
			bHrSet = TRUE;
			Exception.setMessage( hrResult, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			theEx->Delete();
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( (!bSuccess) && (!bHrSet) )
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}
		break;
	} while (0);

	SVSVIMStateClass::RemoveState( SV_STATE_REMOTE_CMD );

	return hrResult;
}


STDMETHODIMP CSVCommand::SVGetSVIMConfigName(BSTR *bstrConfigFilename)
{
	HRESULT Result( S_OK );

	try
	{
		SVString ConfigName = GlobalRCGetConfigurationName();

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
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			*ulOfflineCount = TheSVObserverApp.getOfflineCount();
			bSuccess = TRUE;
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( !bSuccess )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}

	} while (0);

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMVersion(unsigned long *ulVersion)
{
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			*ulVersion = TheSVObserverApp.getCurrentVersion();
			bSuccess = TRUE;
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( !bSuccess )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}

	} while (0);

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMConfigPrint(long lOffset, long *lBlockSize, BSTR *bstrConfigPrint, BOOL *bLastFlag)
{
	SVString ConfigPrint;
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			SVConfigurationPrint printConfig;
			printConfig.printConfigToStringBuffer( ConfigPrint );

			*bstrConfigPrint = _bstr_t(ConfigPrint.c_str()).Detach();
			*lBlockSize = ::SysStringByteLen( *bstrConfigPrint );
			*bLastFlag = TRUE;

			bSuccess = TRUE;
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( !bSuccess )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			hrResult = S_FALSE;
		}

	} while (0);

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
			SVString l_Name = SvUl_SF::createSVString(bstrName);

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
				SVCommandInspectionCollectImageDataPtr l_DataPtr = new SVCommandInspectionCollectImageData( l_InspectionIter->first->GetUniqueObjectID(), l_InspectionIter->second );
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

			if( !( l_DataPtr.empty() ) )
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
	SVString sName;
	SVString sInspectionName;
	BOOL bSamePPQ = TRUE;
	SVInspectionProcess* pInspection( nullptr );
	BOOL bRet = TRUE;
	BOOL bNotAllItemsFound=FALSE;
	BOOL l_bGoodItem = FALSE;
	BOOL l_bGoodInspection = FALSE;

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

		if ( bRet )
		{
			CSVCommand* pThis = this;
			StreamDataStruct* pstData;

			m_InspectionNames.clear();
			m_arInspections.RemoveAll();

			//check list of names to make sure they are all on the same PPQ
			long lNumberOfElements = psaName->rgsabound[0].cElements;

			for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
			{
				SafeArrayGetElementNoCopy(psaName, &lIndex, &bstrName);
				sName = SvUl_SF::createSVString( _bstr_t(bstrName) );
				sInspectionName = SvUl_SF::Left( sName,  sName.find('.'));
				SVStringVector::iterator Iter = std::find( m_InspectionNames.begin(), m_InspectionNames.end(), sInspectionName );
				if ( m_InspectionNames.end() == Iter )
				{
					// add inspection name to list
					m_InspectionNames.push_back(sInspectionName);
				}//end if
			}// end for

			SVGUID PPQId;

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			SVStringVector::iterator Iter( m_InspectionNames.begin() );
			for ( ; m_InspectionNames.end() != Iter && bSamePPQ; ++Iter )
			{
				if ( nullptr != pConfig && pConfig->GetInspectionObject(Iter->c_str(), &pInspection) && nullptr != pInspection )
				{
					if ( PPQId.empty() )
					{
						PPQId = pInspection->GetPPQIdentifier();
					}
					else
					{
						if ( PPQId != pInspection->GetPPQIdentifier() )
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
			if ( !bSamePPQ )
			{
				hrRet = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
				Exception.setMessage( hrRet, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
				// go through all items in psaStatus and set to hrRet;
				lNumberOfElements = (*ppsaStatus)->rgsabound[0].cElements;
				for (long l = 0; l < lNumberOfElements; l++)
				{
					::SafeArrayPutElement(*ppsaStatus,&l,&hrRet);
				}
				m_bRegisteredStream = FALSE;
				hrRet = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				return hrRet;
			}

			HRESULT hrStatus = S_OK;
			//create list of value objects
			for ( long x = 0; x < lNumberOfElements; x++ )
			{
				SVString Temp;

				SafeArrayGetElementNoCopy(psaName,&x,&bstrName);
				Temp = SvUl_SF::createSVString( _bstr_t( bstrName ) );

				sInspectionName = SvUl_SF::Left( Temp, Temp.find('.') );
				BOOL bFound = FALSE;

				pstData = new StreamDataStruct;

				if ( nullptr != pConfig && pConfig->GetInspectionObject(Temp.c_str(), &pInspection) )
				{
					if ( sInspectionName == pInspection->GetName() )
					{
						bFound = TRUE;
					}

					if ( bFound )
					{
						l_bGoodInspection = TRUE;
						pstData->m_InspectionID = pInspection->GetUniqueObjectID();
						pstData->strValueName = Temp;

						SVObjectReference ObjectRef;
						HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( Temp.c_str(), ObjectRef );
						if( S_OK != hrFind )
						{
							pstData->pValueObject = nullptr;
							m_arStreamList.Add( pstData );
							hrRet = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
						}// end if
						else
						{
							if( nullptr != ObjectRef.getValueObject() )
							{
								hrStatus = S_OK;
								pstData->pValueObject = ObjectRef.getObject();
								pstData->arrayIndex = ObjectRef.ArrayIndex();
								m_arStreamList.Add( pstData );
							}// end if
							else
							{
								pstData->pValueObject = nullptr;
								m_arStreamList.Add( pstData );
								hrRet = SVMSG_OBJECT_NOT_PROCESSED;
								::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
								bNotAllItemsFound = TRUE;
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
					m_arStreamList.Add( pstData );

					// to compile 
					hrRet = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
				}// end else
			}// end for

			Iter = m_InspectionNames.begin();
			for ( ; m_InspectionNames.end() != Iter && bSamePPQ; ++Iter )
			{
				pInspection = nullptr;
				if( nullptr != pConfig){ pConfig->GetInspectionObject( Iter->c_str(), &pInspection); }
				if (nullptr != pInspection && *Iter == pInspection->GetName() )
				{
					SVCommandStreamManager::Instance().EnableInspectionCallback( pInspection->GetUniqueObjectID() );

					// add inspections to list
					m_arInspections.Add( pInspection );
				}
			}

			RebuildStreamingDataList();

			m_bRunStreamData = TRUE;
			m_bRegisteredStream = TRUE;

			SVCommandStreamManager::Instance().InsertCommandCallback( 
				boost::bind( &CSVCommand::StreamingDataCallback, this, _1 ), 
				boost::bind( &CSVCommand::RebuildStreamingDataList, this ) );

			m_hStopStreamEvent = ::CreateEvent( nullptr, false, false, nullptr );
			m_hStreamingThread = ::CreateThread( nullptr, 0, SVStreamDataThread, this, 0, nullptr);
			SVThreadManager::Instance().Add( m_hStreamingThread, "SIAC Streaming Data" );
		}// end if
	}//try
	catch(...)
	{
		// send error message
		hr = S_FALSE;
		m_bRegisteredStream = FALSE;
		m_bRunStreamData = FALSE;
		m_dwStreamDataProcessId = 0;
	}

	if ( !(l_bGoodInspection) )
	{
		m_bRegisteredStream = FALSE;
		m_dwStreamDataProcessId = 0;
		bNotAllItemsFound = TRUE;
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
	StreamDataStruct *pStream;
	ProductDataStruct *pProduct;
	long l;
	long lSize;

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

			m_bRegisteredStream = FALSE;
			m_bRunStreamData = FALSE;

			SVCommandStreamManager::Instance().EraseCommandCallback();
			m_InspectionNames.clear();
			m_dwStreamDataProcessId = 0;

			lSize = static_cast< long >( m_arStreamList.GetSize() );
			for( l = 0; l < lSize; l++ )
			{
				pStream = m_arStreamList.GetAt( l );
				delete pStream;
			}// end for
			m_arStreamList.RemoveAll();

			lSize = static_cast< long >( m_arProductList.GetSize() );
			for( l = 0; l < lSize; l++ )
			{
				pProduct = m_arProductList.GetAt( l );
				delete pProduct;
			}// end for
			m_arProductList.RemoveAll();
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
	ProductDataStruct *pProductData;
	PacketDataStruct oPacketData;
	StreamDataStruct *pStreamData;
	BOOL bFound;
	long l;
	long lSize;

	if( m_bRegisteredStream )
	{
		// Protect the to be streamed list
		::EnterCriticalSection( CProductCriticalSection::Get());

		// search the list for this product
		bFound = FALSE;
		pProductData = nullptr;
		lSize = static_cast< long >( m_arProductList.GetSize() );

		for( l = 0; l < lSize; l++ )
		{
			if( pProductData = m_arProductList.GetAt( l ) )
			{
				if( pProductData->lProductCount == p_rData.m_ProductInfo.ProcessCount() )
				{
					bFound = TRUE;
					break;
				}// end if
			}// end if
		}// end for

		// it is not in the list then we need to add it
		if( !bFound )
		{
			pProductData = new ProductDataStruct;
			pProductData->lProductCount = p_rData.m_ProductInfo.ProcessCount();
			pProductData->arPacketData.SetSize( static_cast< int >( m_arStreamList.GetSize() ) );

			// add the product to the outgoing list
			m_arProductList.Add( pProductData );
		}// end if

		// We will go ahead and make a copy of the data now
		lSize = static_cast< long >( m_arStreamList.GetSize() );
		for( l = 0; l < lSize; l++ )
		{
			pStreamData = m_arStreamList.GetAt( l );

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
								pValueObject->getValue( oPacketData.strValue, p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), pStreamData->arrayIndex );
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
						pValueObject->getValue( oPacketData.strValue, p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex() );
					}
				}// end if
				else
				{
					oPacketData.strValue.clear();
				}// end else

				pProductData->arPacketData.SetAt( l, oPacketData );
			}// end if
		}// end for

		pProductData->lCallbackCount++;

		// Unprotect the to be streamed list
		::LeaveCriticalSection( CProductCriticalSection::Get() );

		// if streaming thread is starved, temporarily boost it
		if( m_arProductList.GetSize() > 20 )
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
	SVString strValue;
	BOOL bRunning;
	DWORD dwResult;
	BSTR bstr;
	long lSize;
	long lIndex;
	long lStreamSize;
	long lStreamCount;
	long lInspectIndex;
	long lProductCount;
	long lPacketCount;
	long lNoProduct;
	long l;
	long k;

	CSVCommand *pThis = (CSVCommand*) lpParam; //@TODO:  Should check pThis before dereferencing.  Avoid c-style casts.
	lStreamSize = static_cast< long >( pThis->m_arStreamList.GetSize() );
	lStreamCount = 0;
	lNoProduct = -1;
	bRunning = TRUE;

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
			dwResult = ::WaitForSingleObjectEx( pThis->m_hStopStreamEvent, INFINITE, TRUE );
		}// end else

		switch( dwResult )
		{
		case WAIT_OBJECT_0 :
			bRunning = FALSE;
			break;
		case WAIT_IO_COMPLETION :
			// pass one - check for products to stream now that we are awake
			lProductCount = static_cast< long >( pThis->m_arProductList.GetSize() );
			lStreamCount = 0;
			hr = S_OK;

			for( l = 0; l < lProductCount; l++ )
			{
				pProductData = pThis->m_arProductList.GetAt( l ); // @TODO:  Should check pProductData before dereferencing.
				if( pProductData->lCallbackCount == m_InspectionNames.size() )
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

				lProductCount = static_cast<long> (pThis->m_arProductList.GetSize());
				for( l = 0; lStreamCount && l < lProductCount; l++ )
				{
					pProductData = pThis->m_arProductList.GetAt( l );

					// Verify again that this is one of the ready products
					// @TODO:  Should check pProductData before dereferencing.
					if( pProductData->lCallbackCount != m_InspectionNames.size() )
					{
						continue;
					}// end if

					for( k = 0; k < lStreamSize; k++ )
					{
						lInspectIndex = -1;

						pStreamData = pThis->m_arStreamList.GetAt( k );
						oPacketData = pProductData->arPacketData.GetAt( k );

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
					lProductCount = static_cast<long> (pThis->m_arProductList.GetSize());
					lStreamCount--;

					::EnterCriticalSection(CProductCriticalSection::Get());

					pThis->m_arProductList.RemoveAt( l );

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
			lProductCount = static_cast<long> (pThis->m_arProductList.GetSize());
			lStreamCount = 0;

			for( l = 0; l < lProductCount; l++ )
			{
				pProductData = pThis->m_arProductList.GetAt( l );
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

				lSize = static_cast< long >( m_arStreamList.GetSize() );
				for( l = 0; l < lSize; l++ )
				{
					pStreamData = m_arStreamList.GetAt( l );
					delete pStreamData;
				}// end for
				m_arStreamList.RemoveAll();

				lSize = static_cast< long >( m_arProductList.GetSize() );
				for( l = 0; l < lSize; l++ )
				{
					pProductData = m_arProductList.GetAt( l );
					delete pProductData;
				}// end for
				m_arProductList.RemoveAll();

				bRunning = FALSE;
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
	m_bRegisteredStream = FALSE;
	m_bRunStreamData = FALSE;

	// Uninitialize COM
	CoUninitialize();

	return 0L;
}

STDMETHODIMP CSVCommand::SVGetProductDataList(long lProcessCount, SAFEARRAY* psaNames, SAFEARRAY** ppsaData, SAFEARRAY** ppsaStatus)
{
	HRESULT hr = S_OK;

	BSTR bstr;
	SVStringVector ValueNames;
	SVInspectionProcessVector aInspections;
	SVObjectReferenceVector aValueObjects;
	BOOL l_bItemNotExist = FALSE;

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
		SVString sName = SvUl_SF::createSVString( _bstr_t(bstr) );
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
			aInspections.Add(pInspection);	// add inspection object to list

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
			aInspections.Add( nullptr );
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
			SVString Value;
			HRESULT hrStatus = S_OK;

			if ( nullptr != ObjectRef.getObject() )
			{
				SVDataManagerHandle	l_BucketHandle;

				l_ProductInfo.GetResultDataIndex( l_BucketHandle );

				if ( !ObjectRef.isEntireArray() )
				{
					HRESULT hrGet = ObjectRef.getValueObject()->getValue(Value, l_BucketHandle.GetIndex(), ObjectRef.getValidArrayIndex() );
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
						l_bItemNotExist = TRUE;
					}// else invalid or out of range index
					else	// some generic error; currently should not get here
					{
						ASSERT( FALSE );
						hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						// did not get value.  set value to -1
						Value = _T("-1");
						_bstr_t bstrTmpVal = Value.c_str();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal.Detach());
						//::SysFreeString(bstrTmpVal);
						::SafeArrayPutElement(*ppsaStatus, &i, &hr);
						l_bItemNotExist = TRUE;
					}
				}// if ( !ref.IsEntireArray() )
				else	// GET ENTIRE ARRAY
				{
					// get all results and put them into a parsable string
					int NumResults = ObjectRef.getValueObject()->getResultSize( l_BucketHandle.GetIndex() );
					SVString ArrayValues;
					for ( int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++ )
					{
						HRESULT hrGet = ObjectRef.getValueObject()->getValue( Value, l_BucketHandle.GetIndex(), iArrayIndex );
						if ( S_OK == hrGet )
						{
							if ( iArrayIndex > 0 )
							{
								ArrayValues += _T(",");
							}

							ArrayValues += SvUl_SF::Format( _T("`%s`"), Value.c_str() );
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
				l_bItemNotExist = TRUE;
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
		SVImageClassPtrVector aImageObjects;
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
			SVString strName = SvUl_SF::createSVString( _bstr_t(bstrName) );

			if (nullptr != pConfig && pConfig->GetInspectionObject(strName.c_str(), &pInspection))
			{
				aInspections.Add( pInspection );
				SVImageClass* pImage = nullptr;
				if( pInspection->GetChildObjectByName(strName.c_str(), (SVObjectClass**) &pImage))
				{
					bool bImageOK = false;
					if( dynamic_cast< SVCameraImageTemplate* >( pImage ) ) // Source image
					{
						aImageObjects.Add(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else if ( pImage->ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE )	// Published result image
					{
						aImageObjects.Add(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else
					{
						hrOK = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE;
						l_bItemNotFound = true;
						aImageObjects.Add( nullptr );
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
					aImageObjects.Add( nullptr );
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
				aImageObjects.Add( nullptr );
				aInspections.Add( nullptr );
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

						SVImageClass* pImage = aImageObjects[l];
						if ( pImage )
						{
							SVImageInfoClass svImageInfo;
							SVSmartHandlePointer svImageHandle;
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

	SVString CameraName = SvUl_SF::createSVString( _bstr_t(bstrCameraName) );
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

	SVString CameraName = SvUl_SF::createSVString( _bstr_t(bstrCameraName) );
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

HRESULT CSVCommand::ImageToBSTR(SVImageInfoClass&  rImageInfo, SVSmartHandlePointer rImageHandle, BSTR* pbstr)
{
	HRESULT hr = S_OK;

	SVImageBufferHandleImage l_MilBuffer;
	if ( !( rImageHandle.empty() ) )
	{
		rImageHandle->GetData( l_MilBuffer );
	}

	if ( l_MilBuffer.empty() )
	{
		hr = -1578;
	}

	if ( S_OK == hr )
	{
		HRESULT l_Code;

		SVImageInfoClass oChildInfo;
		SVSmartHandlePointer oChildHandle;
		BITMAPINFOHEADER* pbmhInfo = nullptr;
		SVBitmapInfo l_BitmapInfo;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;
		bool IsColor( false );

		char* pDIB = nullptr;

		BOOL bDestroyHandle = FALSE;

		oChildInfo = rImageInfo;
		oChildHandle = rImageHandle;

		long l_lType = SvOi::SVImageTypeEnum::SVImageTypeUnknown;
		long l_lBandNumber = 1;
		long l_lBandLink = 0;

		SVImageClass* pImage( nullptr );

		oChildInfo.GetOwnerImage( pImage );

		if( nullptr != pImage )
		{
			l_lType = pImage->GetImageType();
			SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( pImage->GetAncestor( SVInspectionObjectType ) );
			if( nullptr != pInspection )
			{
				IsColor = pInspection->IsColorCamera();
			}
		}

		oChildInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber );
		oChildInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandLink, l_lBandLink );

		if ( IsColor && l_lType == SvOi::SVImageTypeEnum::SVImageTypePhysical && l_lBandNumber == 3)
		{
			bDestroyHandle = TRUE;

			oChildInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

			HRESULT hrImage = SVImageProcessingClass::CreateImageBuffer( oChildInfo, oChildHandle );

			SVImageBufferHandleImage l_ChildMilBuffer;

			if( !( oChildHandle.empty() ) )
			{
				oChildHandle->GetData( l_ChildMilBuffer );
			}

			// Copy input image to output image
			l_Code = SVMatroxBufferInterface::CopyBuffer(l_ChildMilBuffer.GetBuffer(), l_MilBuffer.GetBuffer(), l_lBandLink );
		} 

		if ((l_lType == SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical) ||
			(l_lType == SvOi::SVImageTypeEnum::SVImageTypeLogical))
		{
			bDestroyHandle = TRUE;

			HRESULT hrImage = SVImageProcessingClass::CreateImageBuffer( oChildInfo, oChildHandle );

			SVImageBufferHandleImage l_ChildMilBuffer;

			if( !( oChildHandle.empty() ) )
			{
				oChildHandle->GetData( l_ChildMilBuffer );
			}

			// Copy input image to output image
			l_Code = SVMatroxBufferInterface::CopyBuffer(l_ChildMilBuffer.GetBuffer(), l_MilBuffer.GetBuffer() );
		}

		// Get the BITMAPINFO from MIL
		l_BitmapInfo = oChildHandle->GetBitmapInfo();
		if( l_BitmapInfo.empty() )
		{
			SVImageBufferHandleImage l_ChildMilBuffer;

			if( !( oChildHandle.empty() ) )
			{
				oChildHandle->GetData( l_ChildMilBuffer );
			}

			l_Code = SVMatroxBufferInterface::GetBitmapInfo( l_BitmapInfo, l_ChildMilBuffer.GetBuffer() );
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

		if( oChildHandle.empty() )
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

		SVSmartHandlePointer oChildHandle;

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
	BOOL    ItemNotFound = FALSE;
	BOOL    InspectionNotFound = FALSE;

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
		SVString Name;
		SVString Value;
		BSTR bstrName = nullptr;
		long ProcessCount = -1;
		SVObjectReference ObjectRef;
		SVInspectionProcess* pInspection( nullptr );

		Status = S_OK;

		// Get name of requested value out of the safearray
		SafeArrayGetElementNoCopy(psaNames,&l,&bstrName);
		Name = SvUl_SF::createSVString( bstrName );
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
						ItemNotFound = TRUE;
					}
				}
				else
				{
					ItemNotFound = TRUE;
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
						HRESULT hrGet = ObjectRef.getValueObject()->getValue( Value, -1, ObjectRef.getValidArrayIndex() );
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
							ItemNotFound = TRUE;
						}// else invalid or out of range index
						else	// some generic error; currently should not get here
						{
							ASSERT( FALSE );
							Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							// did not get value.  set value to -1
							Value = _T("-1");
							ItemNotFound = TRUE;
						} //end else generic error
					}// if ( !ValueObjectRef.IsEntireArray() )
					else	// GET ENTIRE ARRAY
					{
						// get all results and put them into a parsable string
						int NumResults = ObjectRef.getValueObject()->getResultSize();
						SVString ArrayValues;
						for ( int iArrayIndex = 0; iArrayIndex < NumResults; iArrayIndex++ )
						{
							HRESULT hrGet = ObjectRef.getValueObject()->getValue( Value, -1, iArrayIndex  );
							if ( S_OK == hrGet )
							{
								if ( iArrayIndex > 0 )
								{
									ArrayValues += _T(",");
								}
								ArrayValues += SvUl_SF::Format( _T("`%s`"), Value.c_str() );
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
					ItemNotFound = TRUE;
				} //else could not find object
			}// else inspection object
		}//end if object or inspection
		else
		{ //inspection not found
			Status = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			// did not get value.  set value to -1
			Value = _T("-1");
			InspectionNotFound = TRUE;
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
			SvCmd::InspectionRunOncePtr l_CommandPtr = new SvCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< SvCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

			hrResult = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
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

	HRESULT                hr = S_OK;
	HRESULT                hrStatus = S_OK;
	SVInspectionProcess*   pInspection( nullptr );
	SVObjectReference	   ObjectRef;
	BSTR                   bstrName;
	BSTR                   bstrValue;
	SVString               TmpName;
	SVString			   TmpVal;
	SVVector< SVInspectionProcess* > l_arInspections;

	do
	{
		long lNumberOfElements1 = psaNames->rgsabound[0].cElements;
		long lNumberOfElements2 = psaValues->rgsabound[0].cElements;

		if ((lNumberOfElements1 == 0) || (lNumberOfElements1 != lNumberOfElements2))
		{
			hr = SVMSG_TOO_MANY_REQUESTED_ITEMS;
			break;
		}// end if

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		for (long l = 0; l < lNumberOfElements1; l++)
		{
			BOOL bAddRequest = FALSE;

			hrStatus = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
			if ( FAILED( hrStatus ) ) { break; }

			TmpName = SvUl_SF::createSVString( _bstr_t(bstrName) );

			hrStatus = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
			if ( FAILED( hrStatus ) ) { break; }

			TmpVal = SvUl_SF::createSVString( _bstr_t(bstrValue) );

			//GetInspectionObject is only true if the pointer is valid
			if ( nullptr != pConfig && pConfig->GetInspectionObject(TmpName.c_str(), &pInspection) )
			{
				//got the inspection.
				SVObjectManagerClass::Instance().GetObjectByDottedName( TmpName.c_str(), ObjectRef );
				if( nullptr != ObjectRef.getValueObject() )
				{
					SVObjectClass* pOwnerObject = ObjectRef.getObject()->GetOwner();

					if( SV_IS_KIND_OF(pOwnerObject, SVInspectionProcess) )
					{
						// this object was found and is an input
						bAddRequest = TRUE;
					}// end if
					else
					{
						// item not able to be set online.
						hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_NOT_AN_INPUT;
						::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
					}// end else
				}// end if
				else
				{
					// object not found.  send back status
					hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
				}// end else
			}// end if
			else
			{
				//did not find inspection.
				//put an error back into the list
				hrStatus = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
			} // end else

			if ( bAddRequest )
			{
				//add request to inspection process
				pInspection->AddInputRequest( ObjectRef, W2T( bstrValue ) );

				bool l_bFound = false;
				long lSize = static_cast< long >( l_arInspections.GetSize() );
				for( long k = 0; k < lSize; k++ )
				{
					if( pInspection == l_arInspections[k] )
					{
						l_bFound = true;
						break;
					}// end if
				}// end if

				if( !l_bFound )
				{
					l_arInspections.Add( pInspection );
				}// end if
			}// end if
		}// end for	

		// New delimiter added after each SVSetToolParameterList call
		// This breaks the list into pieces and we are only processing
		// 1 piece of the list per inspection iteration
		long lSize = static_cast< long >( l_arInspections.GetSize() );
		for( long j = 0; j < lSize; j++ )
		{
			//add request to inspection process
			l_arInspections[j]->AddInputRequestMarker();
		}// end for					

	} while(0); // end while

	hr = hrStatus;

	return hr;
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
	SVString TmpName;
	BOOL l_bItemNotFound = FALSE;

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
			BOOL bAddRequest = FALSE;

			hrStatus = ::SafeArrayGetElement(psaNames, &l, &bstrName);
			if ( FAILED( hrStatus ) ) { break; }

			TmpName = SvUl_SF::createSVString( _bstr_t(bstrName) );
			::SysFreeString(bstrName);

			hrStatus = SafeArrayGetElementNoCopy(psaImages, &l, &bstrImage);
			if ( FAILED( hrStatus ) ) { break; }

			//GetInspectionObject is only true if the pointer is valid
			if ( nullptr != pConfig &&  pConfig->GetInspectionObject(TmpName.c_str(), &pInspection)  && nullptr != pInspection )
			{
				SVObjectClass *l_pObject = nullptr;
				//got the inspection.
				if ( pInspection->GetChildObjectByName( TmpName.c_str(), &l_pObject ))
				{
					l_pImageObject = dynamic_cast< SVImageClass* >( l_pObject );

					if ( nullptr != l_pImageObject &&
						(l_pImageObject->ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE) == SvOi::SV_REMOTELY_SETABLE )
					{
						// currently all SvOi::SV_REMOTELY_SETABLE parameters are also SvOi::SV_SETABLE_ONLINE
						// if this changes, this code needs updated
						bAddRequest = TRUE;
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
					l_bItemNotFound = TRUE;
				}// end else
			}// end if
			else
			{
				//did not find inspection.
				//put an error back into the list
				hrStatus = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus,&l,&hrStatus);
				l_bItemNotFound = TRUE;
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
					pInspection->AddInputImageRequest(pInRequest);
				}
				else
				{
					//error
					l_bItemNotFound = TRUE;
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
	inline SVString StripBrackets(const SVString& rValue)
	{
		SVString Result( rValue );
		Result = SvUl_SF::Trim( Result );
		size_t Pos = Result.rfind('[');
		if( SVString::npos != Pos )
		{
			Result = SvUl_SF::Left(Result, Pos);
		}
		return Result;
	}

	inline SVString StripQuotes(const SVString& rValue)
	{
		SVString Result( rValue );
		Result = SvUl_SF::Trim( Result );
		Result = SvUl_SF::Trim( Result, _T("`\'") ); // strip single quotes or back-ticks
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

	int						ItemErrorCount = 0;
	bool					StateOnline;
	SVNameSet				ParameterNames;
	SVConfigurationObject*	pConfig = nullptr;
	SVNameStorageMap		ParameterObjects;
	SVVector< SVInspectionProcess* > Inspections;

	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	StateOnline = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	for ( long l = 0; l < NumberOfElements; l++ )
	{
		SVString				Name;
		SVString				Value;
		SVInspectionProcess*	pInspection = nullptr;
		SVObjectReference		ObjectRef;

		BOOL AddRequest = FALSE;

		BSTR bstrName = nullptr;
		Status = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		if ( FAILED( Status ) ) { break; }

		Name = SvUl_SF::createSVString( bstrName );

		BSTR bstrValue = nullptr;
		Status = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
		if ( FAILED( Status ) ) { break; }

		Value = SvUl_SF::createSVString( bstrValue );
		if(nullptr != pConfig)
		{
			pConfig->GetInspectionObject( Name.c_str(), &pInspection );
		}
		HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), ObjectRef );

		if ( nullptr != ObjectRef.getObject() || nullptr != pInspection )
		{
			if ( S_OK == hrFind && ObjectRef.ArrayIndex() < 0 && !ObjectRef.isArray())
			{
				Name = StripBrackets(Name);
				hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), ObjectRef );
				if (S_OK == hrFind)
				{
					Value = StripQuotes(Value);
				}
			}

			if ( S_OK == hrFind )
			{
				// Check if item is already in the list
				if( nullptr != ObjectRef.getObject() )
				{
					HRESULT hres = S_OK;
					SVString strCompleteObjectName( ObjectRef.GetCompleteName() );
					if ( ParameterNames.find( strCompleteObjectName ) != ParameterNames.end() )
					{
						// Item is already in the list
						Status = SVMSG_OBJECT_ALREADY_SET_IN_THIS_LIST;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if ( (ObjectRef.ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE) != SvOi::SV_REMOTELY_SETABLE )
					{
						// Item is not allowed to be set remotely
						Status = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if (StateOnline && ((ObjectRef.ObjectAttributesAllowed() & SvOi::SV_SETABLE_ONLINE) != SvOi::SV_SETABLE_ONLINE)  ) 
					{
						Status = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if(RangeClassHelper::IsOwnedByRangeObject((*ObjectRef.getObject())) && !RangeClassHelper::IsAllowedToSet(*ObjectRef.getObject(),Value,StateOnline, hres))
					{
						Status = hres;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else
					{
						AddRequest = TRUE;
						ParameterNames.insert( strCompleteObjectName );
					}


				}// end if ( nullptr != ObjectRef.Object() )	// object not a value object
				else
				{
					// object not found.  send back status
					Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus, &l, &Status);
					ItemErrorCount++;
				}
			}// end if found value object
			else
			{
				// object not found.  send back status
				Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				::SafeArrayPutElement(*ppsaStatus, &l, &Status);
				ItemErrorCount++;
			}// end else
		}// end if found inspection
		else
		{
			//did not find inspection.
			//put an error back into the list
			Status = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			::SafeArrayPutElement(*ppsaStatus, &l, &Status);
			ItemErrorCount++;
		} // end else

		if ( AddRequest )
		{
			//Check if the parameter is for inspection or not
			if(nullptr == pInspection)
			{
				SVStorage	Parameter;
				SVSAFEARRAY	Values;

				//Values must be in array format
				Values.Add(_variant_t( bstrValue ));
				Parameter.m_StorageType = SVVisionProcessor::SVStorageValue;
				Parameter.m_Variant = Values;
				ParameterObjects[SVString(Name)] = Parameter;
			}
			else
			{
				//add request to inspection process
				pInspection->AddInputRequest( ObjectRef, W2T( bstrValue ) );

				bool Found = false;
				long Size = static_cast< long >( Inspections.GetSize() );
				for( long k = 0; k < Size; k++ )
				{
					if( pInspection == Inspections[k] )
					{
						Found = true;
						break;
					}// end if
				}// end if

				if( !Found )
				{
					Inspections.Add( pInspection );
				}// end if
			} //end else nullptr == pInspection
		}// end if ( bAddRequest )
	}// end for ( long l = 0; l < lNumberOfElements; l++ )

	//Check if there are parameters other than for the inspection 
	if(0 != ParameterObjects.size())
	{
		SVNameStatusMap SetItemsResult;
		///SVO785
		bool IsOneBased(false);

		SVVisionProcessorHelper::Instance().SetItems(ParameterObjects, SetItemsResult, IsOneBased);
	}
	// New delimiter added after each SVSetToolParameterList call
	// This breaks the list into pieces and we are only processing
	// 1 piece of the list per inspection iteration
	long Size = static_cast< long >( Inspections.GetSize() );
	for( long j = 0; j < Size; j++ )
	{
		//add request to inspection process
		Inspections[j]->AddInputRequestMarker();
	}// end for

	if ( ItemErrorCount > 0 ) //some error happened
	{
		Result = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
	}
	else
	{
		Result = S_OK;
	}

	return Result;
}

HRESULT CSVCommand::SVLockImage(long p_lProcessCount, long p_lIndex, BSTR p_bsName)
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

	if(p_lIndex < m_aSVActXLock.GetSize())	// Check Lock Structure.
	{

		SVaxls = m_aSVActXLock.GetAt(p_lIndex);

		if(SVaxls.Valid())	// Image is Locked so Release
		{
			SVaxls.clear();
			m_aSVActXLock.SetAt(p_lIndex, SVaxls);
		}
	}

	SVImageClass* pImage = nullptr;
	SVImageIndexStruct l_svImageIndex;
	SVDataManagerHandle l_DMImageIndexHandle;

	SVInspectionProcess* pInspection = nullptr;

	SVString TmpName = SvUl_SF::createSVString( _bstr_t(p_bsName) );
	//GetInspectionObject is only true if the pointer is valid
	if ( nullptr != pConfig && pConfig->GetInspectionObject( TmpName.c_str(), &pInspection) )
	{
		if (pInspection->GetChildObjectByName(TmpName.c_str(), (SVObjectClass**) &pImage))
		{
			SVPPQObject* pPPQ = pInspection->GetPPQ();	// inspection can be part of only one PPQ

			if( nullptr != pPPQ )
			{
				SVProductInfoStruct ProductInfo;

				if( S_OK == pPPQ->GetProduct( ProductInfo, p_lProcessCount ) )
				{
					SVaxls.m_Name = TmpName;
					SVaxls.m_ProcessCount = p_lProcessCount;
					if( SVCameraImageTemplate* pMainImage = dynamic_cast<SVCameraImageTemplate*>( pImage ) )
					{
						l_svImageIndex = pMainImage->GetSourceImageIndex( &ProductInfo.oPPQInfo.m_ResultImagePublishedDMIndexHandle, ProductInfo.m_svCameraInfos );
						l_DMImageIndexHandle.Assign( l_svImageIndex.m_CameraDMIndexHandle, SV_DCOM );
					}
					else if ( pImage->ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE )
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
			SVSmartHandlePointer l_svImageHandle;

			if( pImage->GetImageHandle( l_svImageIndex, l_svImageHandle ) && !( l_svImageHandle.empty() ) )
			{
				SVImageBufferHandleImage l_MilBuffer;
				l_svImageHandle->GetData( l_MilBuffer );

				SVImageInfoClass l_ImageInfo = l_svImageInfo;

				if ( S_OK == SVImageProcessingClass::CreateImageBuffer( l_ImageInfo, SVaxls.m_ImageHandlePtr ) && !( SVaxls.m_ImageHandlePtr.empty() ) )
				{
					SVImageBufferHandleImage l_AxlsMilBuffer;
					SVaxls.m_ImageHandlePtr->GetData( l_AxlsMilBuffer );

					HRESULT l_Code;
					l_Code = SVMatroxBufferInterface::CopyBuffer( l_AxlsMilBuffer.GetBuffer(), l_MilBuffer.GetBuffer() );

					// Add locked image to Lock array
					m_aSVActXLock.SetAtGrow(p_lIndex, SVaxls);// Store Info in lock array
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

HRESULT CSVCommand::SVGetLockedImage(long p_lIndex, long p_lCompression, BSTR* p_pbstrImage, BSTR* p_pbstrOverlay)
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

		if( p_lIndex >= m_aSVActXLock.GetSize() )
		{
			hr = SVMSG_IMAGE_NOT_LOCKED;
			break;
		}

		SVaxls = m_aSVActXLock.GetAt(p_lIndex);
		if( !SVaxls.Valid() )
		{
			hr = SVMSG_IMAGE_NOT_LOCKED;
			break;
		}

		// for right now locked image will not have overlays
		// this is because the overlay data is not bucketized
		*p_pbstrOverlay = nullptr;

		// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
		// Product is still valid; 
		// Build the output images from the result data index in the product and the source images by name  (?)

		SVImageInfoClass l_ImageInfo;

		// put image in return array
		hr = ImageToBSTR( l_ImageInfo, SVaxls.m_ImageHandlePtr, p_pbstrImage);

		break;
	} while (false);

	return hr;
}

HRESULT CSVCommand::SVUnlockImage(long p_lIndex)
{
	HRESULT hres = S_OK;
	if( p_lIndex < m_aSVActXLock.GetSize() )
	{
		SVActiveXLockStruct SVaxls;

		SVaxls = m_aSVActXLock.GetAt( p_lIndex );
		if( !SVaxls.Valid() )
		{
			return SVMSG_IMAGE_NOT_LOCKED;
		}

		SVaxls.clear();
		m_aSVActXLock.SetAt(p_lIndex, SVaxls);
	}

	return hres;
}

HRESULT CSVCommand::SVUnlockAllImages()
{
	SVActiveXLockStruct SVaxls;
	HRESULT hr = S_OK;

	for( int x = static_cast< int >( m_aSVActXLock.GetSize() - 1 );  x > -1; x-- )
	{
		SVaxls = m_aSVActXLock.GetAt(x);
		if(SVaxls.Valid())
		{
			SVaxls.clear();
			m_aSVActXLock.SetAt(x, SVaxls);
		}
	}

	return hr;
}

STDMETHODIMP CSVCommand::SVGetRemoteInputCount(long *lCount)
{
	HRESULT hrResult = S_OK;
	BOOL bSuccess = FALSE;

	try
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList();
			if( nullptr != pInputObjectList && pInputObjectList->GetRemoteInputCount( *lCount ) )
			{
				bSuccess = TRUE;
			}
		}
	}
	catch (...)
	{
		bSuccess = FALSE;
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
	BOOL bSuccess = FALSE;

	try
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList( );
			if( nullptr != pInputObjectList && pInputObjectList->SetRemoteInput( lIndex, _variant_t(vtValue) ) )
			{
				bSuccess = TRUE;
			}
		}
	}
	catch (...)
	{
		bSuccess = FALSE;
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
	SVSmartHandlePointer oTempHandle;

	oTempInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, pbmhInfo->biBitCount );
	oTempInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );
	oTempInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );

	oTempInfo.SetExtentProperty( SVExtentPropertyHeight, abs(pbmhInfo->biHeight) );
	oTempInfo.SetExtentProperty( SVExtentPropertyWidth, pbmhInfo->biWidth );

	if( pbmhInfo->biBitCount == 24 )
	{
		oTempInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 );
		oTempInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, 3 );
	}// end if

	if( S_OK != SVImageProcessingClass::CreateImageBuffer( oTempInfo, oTempHandle ) || oTempHandle.empty() )
	{
		return l_ImageBuf;
	}

	// Copy the bits into the image object
	if( nullptr != pBits && !oTempHandle.empty() )
	{
		// Set buffer data...
		memcpy( oTempHandle->GetBufferAddress(), pBits, pbmhInfo->biSizeImage );
	}// end if

	SVImageBufferHandleImage l_MilBuffer;
	oTempHandle->GetData( l_MilBuffer );

	return l_MilBuffer.GetBuffer();
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
			SVString FontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlffont.mfo"));
			SVString ControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfcont.mfo"));
			SVString ConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfstra.mfo"));
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
				SVString l_strFontFileName = FontFileName;

				MatroxCode = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strFontFileName, SVOcrRestore );

				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontHandle( lFontIdentifier, lFontHandle );

				if (S_OK != MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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

				SVString l_strControlsFileName = ControlsFileName;
				MatroxCode = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strControlsFileName, SVOcrLoadControl );
				if (S_OK != MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
					SVStringVector msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}// end if

			// fill the character's / ID mapping for the font
			double NbChar;
			MatroxCode = SVMatroxOcrInterface::Get( lFontHandle, SVCharNumberInFont, NbChar );
			SVString l_strCharacters;
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
			SVString strFontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsffont.mfo")).c_str();
			SVString strControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfcont.mfo")).c_str();
			SVString strConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfstra.mfo")).c_str();
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
					SVStringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
					SVStringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
					SVStringVector msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
					SVStringVector msgList;
					msgList.push_back(_T("SVCalibrateFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
			SVMatroxOcrResult l_milResult;
			MatroxCode = SVMatroxOcrInterface::Create( l_milResult );
			// BRW - This l_Code is never checked.

			lFontHandle.m_bVerify = false;
			MatroxCode = SVMatroxOcrInterface::Execute( l_milResult, lFontHandle, l_milImage );

			if (S_OK != MatroxCode)
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SVStringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}

			// Process the OCR chars returned from MocrReadString();
			long l_lLength = 0L;

			MatroxCode = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrResultStringSize, l_lLength );

			if( l_lLength != 0 )
			{
				MatroxCode = SVMatroxOcrInterface::GetResult(l_milResult, SVOcrStringScore, *dMatchScore );
				// BRW - This l_Code is never checked.
				SVString Text;
				MatroxCode = SVMatroxOcrInterface::GetResult(l_milResult, SVOcrString, Text );
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
				SVStringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}

			MatroxCode = SVMatroxOcrInterface::Destroy( l_milResult );
			if (S_OK !=  MatroxCode)
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SVStringVector msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
				SVMatroxOcrResult l_milResult;
				MatroxCode = SVMatroxOcrInterface::Create( l_milResult );

				if (S_OK !=  MatroxCode)
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				lFontHandle.m_bVerify = true;
				lFontHandle.m_VerifyString = strVerify;

				SVMatroxOcrInterface::Execute( l_milResult, lFontHandle, l_milImage );

				if (S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				// Process the OCV chars returned from MocrVerifyString();
				long l_lLength = 0L;

				MatroxCode = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrResultStringSize, l_lLength);

				if( l_lLength != 0L )
				{
					MatroxCode = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrStringScore, *dMatchScore );
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
					SVStringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvStl::Tid_ErrorMIL, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}

				MatroxCode = SVMatroxOcrInterface::Destroy( l_milResult );

				if( S_OK != MatroxCode )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringVector msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), MatroxCode));
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
							SVString Text = SvUl_SF::createSVString( _bstr_t(bstrLabel) );

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
				ImageInfo.SetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lValue );

				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVType, l_lValue );
				ImageInfo.SetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, l_lValue );

				SVSmartHandlePointer ImageBufferHandle = new SVImageBufferHandleStruct( lCharHandle );

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
			SVString Characters;
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
	StreamDataStruct *pStreamData;
	SVObjectClass* pTempObject;
	long l;
	long lSize = static_cast< long >( m_arStreamList.GetSize() );
	for( l = 0; l < lSize; l++ )
	{
		pStreamData = m_arStreamList.GetAt( l );
		if( nullptr != pStreamData )
		{
			if ( !( pStreamData->m_InspectionID.empty() ) )
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

BOOL CSVCommand::ResetStreamingDataAndLockedImages()
{
	// Do what CSVCommand::SVUnlockAllImages does
	SVActiveXLockStruct SVaxls;
	StreamDataStruct *pStream;
	ProductDataStruct *pProduct;
	long l;
	long lSize;

	for( int x = static_cast< int >( m_aSVActXLock.GetSize() - 1 );  x > -1; x-- )
	{
		SVaxls = m_aSVActXLock.GetAt( x );
		if( SVaxls.Valid() )
		{
			SVaxls.clear();
			m_aSVActXLock.SetAt( x, SVaxls );
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

	m_bRegisteredStream = FALSE;
	m_bRunStreamData = FALSE;

	SVCommandStreamManager::Instance().EraseCommandCallback();
	m_dwStreamDataProcessId = 0;

	lSize = static_cast< long >( m_arStreamList.GetSize() );
	for( l = 0; l < lSize; l++ )
	{
		pStream = m_arStreamList.GetAt( l );
		delete pStream;
	}// end for
	m_arStreamList.RemoveAll();

	lSize = static_cast< long >( m_arProductList.GetSize() );
	for( l = 0; l < lSize; l++ )
	{
		pProduct = m_arProductList.GetAt( l );
		delete pProduct;
	}// end for
	m_arProductList.RemoveAll();

	return TRUE;
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
				if( pObject->ObjectAttributesSet() & SvOi::SV_DD_VALUE ) // if Data Definition List set.
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
			bool l_bWritable = (SelectedObjects[i]->ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE) == SvOi::SV_REMOTELY_SETABLE;
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
			if( SelectedObjects[i]->GetObjectType() == SVEnumValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVEnumerateValueObjectClass* l_pEnumVO = dynamic_cast<SVEnumerateValueObjectClass*>(SelectedObjects[i]);
				if( nullptr != l_pEnumVO )
				{
					SVEnumerateVector l_enumVect;
					l_pEnumVO->GetEnumTypes( l_enumVect );
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast< ULONG >( l_enumVect.size() );
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long i = 0; i < static_cast<long>(l_enumVect.size()) ; i++ )
					{
						_bstr_t bstTmp = l_enumVect[i].first.c_str();
						SafeArrayPutElement(l_psaTemp, &i, bstTmp.Detach() );
					}
					// Put the Safearray in the Variant.
					Value.vt = VT_ARRAY | VT_BSTR;
					Value.parray = l_psaTemp;
				}
			}
			else if( SelectedObjects[i]->GetObjectType() == SVBoolValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<SVBoolValueObjectClass*>(SelectedObjects[i]);
				if( nullptr != l_pBoolVO )
				{
					SVStringVector ValidTypes;
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
		SVImageListClass l_ImageList;
		SVToolSetClass* pToolSet = pInspection->GetToolSet();
		pToolSet->GetImageList( l_ImageList );

		std::vector<SVImageClass*> objectList;

		int nCount = static_cast< int >( l_ImageList.GetSize() );
		for( int i = 0; i < nCount; i++ )
		{
			SVImageClass* pImage = l_ImageList.GetAt(i);

			if ( pImage )
			{
				//
				// Check for the required Output object attributes.
				//
				if ( (pImage->ObjectAttributesSet() & SvOi::SV_DD_IMAGE) != 0 )
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
			bool l_bWritable = (objectList[i]->ObjectAttributesAllowed() & SvOi::SV_REMOTELY_SETABLE) == SvOi::SV_REMOTELY_SETABLE;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = l_bWritable;
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &Value );

			// Published Image
			l_Index[1] = 2;
			Value.Clear();
			Value.ChangeType(VT_BOOL);
			Value = (objectList[i]->ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE) != 0;
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
						SVString strTmp;
						HRESULT l_hr = pSourceNames->getValue( strTmp, -1, l_lIndex );
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

		if( l_CommandPtr.empty() )
		{
			l_CommandPtr = SVFileSystemCommandFactorySingleton::Instance().CreateCommand( p_CommandType );
		}

		if( l_CommandPtr.empty() )
		{
			l_CommandPtr = SVStreamCommandFactorySingleton::Instance().CreateCommand( p_CommandType );
		}

		if( ! l_CommandPtr.empty() )
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

