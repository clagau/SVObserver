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

#include "SVCmnLib/SVXmlCmd.h"
#include "SVCmnLib/svxmlexception.h"
#include "SVCmnLib/Utilities.h"
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
#include "SVResultsWrapperClass.h"
#include "SVResultView.h"
#include "SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVImageBuffer.h"	//SVImageOverlayClass
#include "SVConfigurationPrint.h"
#include "SVConfigurationObject.h"
#include "SVInspectionProcess.h"
#include "SVImageProcessingClass.h"
#include "SVInputObjectList.h"
#include "SVStreamCommandFactory.h"
#include "SVInputStreamManager.h"
#include "SVTool.h"
#include "SVCommandStreamManager.h"
#include "SVCommandInspectionCollectImageData.h"
#include "GuiCommands/InspectionRunOnce.h"

#include "SVOlicenseManager/SVOLicenseManager.h"
#include "RemoteCommand.h"
#include "BasicValueObject.h"
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
volatile HANDLE CSVCommand::m_hStopStreamEvent = NULL;
volatile HANDLE CSVCommand::m_hStreamingThread = NULL;
CRITICAL_SECTION CSVCommand::m_hProductCritSect;
bool CSVCommand::m_bCriticalSectionInitialized = false;
CStringList CSVCommand::m_cslInspectionNames;

SVVector< SVActiveXLockStruct > CSVCommand::m_aSVActXLock;
SVVector< StreamDataStruct* > CSVCommand::m_arStreamList;
SVVector< ProductDataStruct* > CSVCommand::m_arProductList;
SVVector< SVInspectionProcess* > CSVCommand::m_arInspections;
#pragma endregion Declarations

#pragma region Constructor
CSVCommand::CSVCommand()
	: m_pStream( nullptr )
{
	if (!m_bCriticalSectionInitialized)
	{
		::InitializeCriticalSection( &m_hProductCritSect );
		m_bCriticalSectionInitialized = true;
	}
}
#pragma endregion Constructor

//@WARNING [gra][7.30][18.08.2015] Note the XML command come from a prior version of the SIAC interface the others are the current version (non XML)
STDMETHODIMP CSVCommand::GetSVIMState(BSTR *pszXMLData, BSTR *pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	ULONG ulState = 0; //set to 0 non test mode
	HRESULT hrResult = S_OK;
	BSTR XMLData;
	BSTR XMLError;

	//xml state data
	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.SetCommand(_T("GetSVIMState")))goto xmlerror;

		//more error checking required here!
		GlobalRCGetState(&ulState);

		//ENTER CURRENT STATE OF SVOBSERVER HERE as per the following function
		// i.e. Definition: BOOL GetSvimState(ULONG* state, SVException* svException)
		//					return FALSE if error
		// if(!GetSvimState(&ulState, &svException)break;

		if(!SvXmlCmd.SetSVIMstate(ulState))goto xmlerror;

		//save the xml doc into an XML BSTR
		if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

		*pszXMLData = SysAllocString(XMLData);

		if(XMLData)
		{
			SysFreeString(XMLData);
		}
		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SetSVIMState(BSTR szXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	ULONG ulState;
	HRESULT hrResult = S_OK;
	BSTR XMLError;

	//xml state data
	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(&szXMLData))goto xmlerror;

		SvXmlCmd.GetSVIMstate(&ulState);

		//more error checking required here!
		if(ulState)
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

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::GetSVIMConfig(BSTR* pszXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	DWORD dwFileLength = 0;
	BYTE * pBuf = NULL;
	CFile binFile; //just for testing
	BOOL rc = TRUE;
	CString szPackedFile;
	SVPackedFile PackedFile;
	char szConfigName[_MAX_PATH];
	memset(szConfigName,0,_MAX_PATH);
	HRESULT hrResult = S_OK;
	BSTR XMLData;
	BSTR XMLError;

	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.SetCommand(_T("GetSVIMConfig")))goto xmlerror;

		if( !GlobalRCGetConfigurationName( szConfigName ) ) { goto error; }

		szPackedFile = szConfigName;
		szPackedFile += _T(".svf");

		if ( GlobalRCSaveConfiguration() )
		{
			try
			{
				if (PackedFile.PackFiles (CString(szConfigName), szPackedFile))
				{
					//read the binary file data
					if(binFile.Open(szPackedFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone ))
					{
						// determine the file length
						dwFileLength = static_cast<unsigned long>(binFile.GetLength());
						//create a buffer to hold the file data
						pBuf = (BYTE *)malloc(dwFileLength);
						//transfer the file data into the buffer
						binFile.Read(pBuf,dwFileLength);
						// close the file
						binFile.Close();
						//delete the file
						CFile::Remove(szPackedFile);
					}
					else
					{
						goto error;
					}
				}
				else
				{
					goto error;
				}
			}
			catch (...)
			{
				goto error;
			}
		}
		else
		{
			goto error;
		}

		if(!SvXmlCmd.SetBinData(pBuf,dwFileLength))goto xmlerror;

		//save the xml doc into an XML BSTR
		//		if(!SvXmlCmd.GetXmlDoc(pszXMLData))goto xmlerror;

		//save the xml doc into an XML BSTR
		if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

		*pszXMLData = SysAllocString(XMLData);

		if(XMLData)
		{
			SysFreeString(XMLData);
		}

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	if(pBuf)free(pBuf);
	return hrResult;
}

STDMETHODIMP CSVCommand::PutSVIMConfig(BSTR szXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly ); //All exceptions are set to be logged
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	LPTSTR szSVIMFilename = NULL;
	DWORD dwFileLength = 0;
	BYTE * pBuf = NULL;
	CFile savFile; //testing only
	CString szPackedFile, szTemp, szConfigFile;
	int nRet = 0;
	SVPackedFile svPackedFile;
	HRESULT hrResult = S_OK;
	BSTR XMLError;
	HRESULT hrException = S_OK;

	// Check if we are in an allowed state first
	// Not allowed to perform if State is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}
	do
	{
		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Command_StartSvimConfig, StdMessageParams );

		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(&szXMLData))goto xmlerror;

		if(!SvXmlCmd.GetBinData(&pBuf,&dwFileLength))goto xmlerror;

		//For SVObserver implementation:
		//Unpacked the file and save all files under C:\Run  directory
		//then load the config



		//create confirm directory c:\temp
		if(!CreateDirPath(CString(SvStl::GlobalPath::Inst().GetTempPath().c_str())))
		{
			hrException = SVMSG_ERROR_CREATING_DIRECTORY;
			goto error;
		}

		//create a temporary filename and saved file to c:\temp
		szPackedFile =  SvStl::GlobalPath::Inst().GetTempPath(_T("temp.svf")).c_str() ; 

		if(savFile.Open(szPackedFile, CFile::shareDenyNone | CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			savFile.Write(pBuf,dwFileLength);
			savFile.Close();
		}
		else
		{
			hrException = SVMSG_ERROR_OPENING_PACKING_FILE; //Unable to open Pack File
			goto error;
		}

		//global function to close config and clean up c:\run dir
		if ( !GlobalRCCloseAndCleanConfiguration() )
		{ 
			hrException = SVMSG_ERROR_CLOSING_CLEANING_CONFIG;
			goto error; 
		}

		//unpack the files in the c:\run directory
		if(!svPackedFile.UnPackFiles (szPackedFile, SvStl::GlobalPath::Inst().GetRunPath().c_str()))
		{
			hrException = SVMSG_ERROR_UNPACKING_FILE;
			goto error;
		}

		//remove the temporary file
		CFile::Remove(szPackedFile);

		szConfigFile = svPackedFile.getConfigFilePath();
		if( szConfigFile.IsEmpty() )
		{
#ifdef _DEBUG
			ASSERT(false); //_T( "No config file found!" ) 
#endif
			hrException = SVMSG_NO_CONFIGURATION_FOUND; //No Config file in Packed File
			goto error;
		}

		//load the config
		LPTSTR pConfigFile = szConfigFile.GetBuffer();
		BOOL loadedConfig = GlobalRCOpenConfiguration( pConfigFile );
		if( !loadedConfig )
		{ 
			hrException = SVMSG_ERROR_LOADING_CONFIGURATION;
			goto error;
		}
		szConfigFile.ReleaseBuffer();

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Command_EndSvimConfig, StdMessageParams );
		break;
error:
		//set the exception for the correct reason for breaking
		Exception.setMessage( hrException, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	if( pBuf ) { free(pBuf); }

	return hrResult;
}

STDMETHODIMP CSVCommand::GetSVIMFile(BSTR * pszXMLData, BSTR* pXMLError)
{
	USES_CONVERSION;
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	DWORD dwFileLength = 0;
	BYTE * pBuf = NULL;
	CFile binFile;
	LPTSTR szFilename = NULL;
	HRESULT hrResult = S_OK;
	BSTR XMLData;
	BSTR XMLError;

	//file data
	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(pszXMLData))goto xmlerror;

		//read the binary file data
		if(!SvXmlCmd.GetSVIMfilename(&szFilename))goto xmlerror;

		if(binFile.Open(szFilename, CFile::shareDenyNone | CFile::modeRead ))
		{
			// determine the file length
			dwFileLength = static_cast<unsigned long>(binFile.GetLength());
			//create a buffer to hold the file data
			pBuf = (BYTE *)malloc(dwFileLength);
			//transfer the file data into the buffer
			binFile.Read(pBuf,dwFileLength);
			// close the file
			binFile.Close();
		}
		else goto error;
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.SetCommand(_T("GetSVIMFile")))goto xmlerror;

		if(!SvXmlCmd.SetBinData(pBuf,dwFileLength))goto xmlerror;

		//save the xml doc into an XML BSTR
		if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

		*pszXMLData = SysAllocString(XMLData);

		if(XMLData)
		{
			SysFreeString(XMLData);
		}

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	if(pBuf)free(pBuf);
	return hrResult;
}

STDMETHODIMP CSVCommand::PutSVIMFile(BSTR szXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	LPTSTR szSVIMFilename = NULL;
	DWORD dwFileLength = 0;
	BYTE * pBuf = NULL;
	CFile savFile; //testing only
	HRESULT hrResult = S_OK;
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	CString szPath, szTemp;
	int nBackCount = 0;
	int nBackIndex = 0;
	BSTR XMLError;

	//xml data
	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(&szXMLData))goto xmlerror;

		if(!SvXmlCmd.GetSVIMfilename(&szSVIMFilename))goto xmlerror;

		if(!SvXmlCmd.GetBinData(&pBuf,&dwFileLength))goto xmlerror;

		_tsplitpath (szSVIMFilename, szDrive, szDir, szFName, szExt);

		szPath = szDrive;
		szPath += szDir;

		if(szPath.IsEmpty())
		{
			szPath= SvStl::GlobalPath::Inst().GetRunPath().c_str();
			if(!CreateDirPath(szPath))goto error;
			szPath += _T("\\");
			szPath += szSVIMFilename;
			_tcscpy(szSVIMFilename,szPath.GetBuffer(szPath.GetLength()));
		}
		else
		{
			if(!CreateDirPath(szPath))goto error;
		}

		if(savFile.Open(szSVIMFilename,CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate))
		{
			savFile.Write(pBuf,dwFileLength);
			savFile.Close();
		}
		else goto error;

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	if(pBuf)free(pBuf);
	return hrResult;
}

STDMETHODIMP CSVCommand::LoadSVIMConfig(BSTR szXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	LPTSTR szConfigFileName = NULL;
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	TCHAR path_buffer[_MAX_PATH];
	WIN32_FIND_DATA FindData;
	HRESULT hrResult = S_OK;
	BSTR XMLError;

	// Check if we are in an allowed state first
	// Not allowed to perform if State is Regression or Test
	if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_REGRESSION))
	{
		_bstr_t data(szXMLData, false);
		return SVMSG_63_SVIM_IN_WRONG_MODE;
	}
	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(&szXMLData))goto xmlerror;

		if(!SvXmlCmd.GetSVIMfilename(&szConfigFileName))goto xmlerror;

		//check for existence of config file
		_tsplitpath (szConfigFileName, szDrive, szDir, szFName, szExt);
		if(!_tcscmp(szDrive, _T("")))
		{ //just the file name, search the run directory for the filename
			if( 0 == _tcscmp( szExt,_T(".svx") ) || 0 == _tcscmp( szExt, _T("") ) )
			{
				_tmakepath( path_buffer, "C", "\\Run\\", szFName, "svx" );
				//check for existence of file first
				if(FindFirstFile(path_buffer,&FindData) == INVALID_HANDLE_VALUE)goto error;
				szConfigFileName = path_buffer;
				if( !GlobalRCCloseConfiguration() ) { goto error; }
			}
			else goto error;
		}
		else if( 0 == _tcscmp( szExt, _T( ".svx" ) ) )
		{ //fully qualified path with svx extension
			//check for existence of file first
			if(FindFirstFile(szConfigFileName,&FindData) == INVALID_HANDLE_VALUE)goto error;
			//global function to close config and clean up c:\run dir
			if( !GlobalRCCloseAndCleanConfiguration() ) { goto error; }
		}
		else goto error;

		//load the config
		if( !GlobalRCOpenConfiguration( szConfigFileName ) ) { goto error; }

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::GetSVIMInspectionResults(BSTR* pszXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	LPTSTR pszIPD;
	CString szIPD;
	CString szIpdTitle;
	HRESULT hrResult = S_OK;
	BSTR XMLData;
	BSTR XMLError;

	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.LoadDoc(pszXMLData))goto xmlerror;

		//the svimfilname element holds the IPD name
		if(!SvXmlCmd.GetSVIMfilename(&pszIPD))goto xmlerror;
		szIPD = pszIPD;

		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.SetSVIMfilename(pszIPD))goto xmlerror;

		if(!SvXmlCmd.SetCommand(_T("GetSVIMInspectionResults")))goto xmlerror;

		//ADD THE REQUIRED RESULTS DATA FROM ALL IPDS AND ALL PUBLISHED RESULTS
		//possibly in a loop using the following function
		//NOTE:  The order in which in the results data per IPD is added to the XML
		//Doc will determine the index number that must be used to retreive the data.
		//i.e first data item added to XML doc will be index zero

		if( ! SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
		{
			SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc( szIPD );
			if( pIPDoc )
			{
				SVResultsXmlWrapper * pSVRXW = new SVResultsXmlWrapper(&SvXmlCmd);

				if( pIPDoc->GetResults( pSVRXW ) != S_OK )
				{
					goto error;
				}
			}
			else goto error;
		}
		else goto error;

		//save the xml doc into an XML BSTR
		if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

		*pszXMLData = SysAllocString(XMLData);

		if(XMLData)
		{
			SysFreeString(XMLData);
		}

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::GetSVIMConfigName(BSTR * pszXMLData, BSTR* pXMLError)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	char szConfigName[_MAX_PATH];
	HRESULT hrResult = S_OK;
	BSTR XMLData;
	BSTR XMLError;

	do
	{
		if(!SvXmlCmd.InitXml())goto xmlerror;

		if(!SvXmlCmd.SetCommand(_T("GetSVIMConfigName")))goto xmlerror;

		if( !GlobalRCGetConfigurationName( szConfigName ) )
		{
			goto error;
		}

		if(!SvXmlCmd.SetSVIMfilename(szConfigName))goto xmlerror;

		//save the xml doc into an XML BSTR
		if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

		*pszXMLData = SysAllocString(XMLData);

		if(XMLData)
		{
			SysFreeString(XMLData);
		}

		Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
		break;
error:
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
		hrResult = S_FALSE;
		break;
xmlerror:
		//create an exception object for an XML error
		if(!SvXmlCmd.GetParserErrorCode())
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
		}
		else
		{
			Exception.setMessage( SvXmlCmd.GetParserError().getMessage() );
		}

		hrResult = S_FALSE;
		break;
	} while (0);

	if(( svXmlException = Exception.getMessageContainer() ) != TRUE)
	{ //create an exception object for an XML parse error
		SvStl::MessageContainer XmlException = svXmlException.GetParserError();
		svXmlException = XmlException;
	}
	svXmlException.GetXmlDoc(&XMLError);

	*pXMLError = SysAllocString(XMLError);

	if(XMLError)
	{
		SysFreeString(XMLError);
	}

	return hrResult;
}

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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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

		if( !(hrResult == S_OK) )
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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

	if( !(hrResult == S_OK) )
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
		}
	}

	return hrResult;
}// end SVSetSVIMState

STDMETHODIMP CSVCommand::SVGetSVIMConfig( long lOffset, long *lBlockSize, BSTR *bstrFileData, BOOL *bLastFlag)
{
	CString strPackedFile;
	SVPackedFile PackedFile;
	char szConfigName[_MAX_PATH];
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
		memset( szConfigName, 0, _MAX_PATH );

		try
		{
			bSuccess = GlobalRCGetConfigurationName( szConfigName );

			if( bSuccess )
			{
				strPackedFile = szConfigName;
				strPackedFile += _T(".svf");
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
					bSuccess = PackedFile.PackFiles( CString(szConfigName), strPackedFile );
				}
			}//offset < 1  end of the fist time

			//send data back to control

			if (bSuccess)
			{
				ex = new CFileException;
				if( binFile.Open( strPackedFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, ex ) )
				{
					lFileSize = (long)binFile.GetLength();
					lBytesToGo = lFileSize - lOffset;
					if ( (*lBlockSize > lBytesToGo) || (*lBlockSize < 1) )
					{
						*lBlockSize = lBytesToGo;
						*bLastFlag = TRUE;
					}

					*bstrFileData = SysAllocStringByteLen(NULL,*lBlockSize);
					if ( (*bstrFileData) == NULL)
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
			CString szFormatted = _T("ERROR - ");
			szFormatted += szCause;
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
			CString szFormatted = _T("ERROR - ");
			szFormatted += szCause;
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
		}
		break;
	} while (0);

	return hrResult;
}// end SVGetSVIMConfig

STDMETHODIMP CSVCommand::SVPutSVIMConfig(long lOffset, long lBlockSize, BSTR *bstrFileData, BOOL bLastFlag)
{
	CString strPackedFile;
	CString configFileName;
	SVPackedFile svPackedFile;
	HRESULT hrResult;
	BOOL bSuccess;
	CFile binFile;
	CString strFilename;
	CString strPath; 
	CString szPackedFile;
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
			if(CreateDirPath(CString(SvStl::GlobalPath::Inst().GetTempPath().c_str()))) 
			{
				szPackedFile = SvStl::GlobalPath::Inst().GetTempPath(_T("temp.svf")).c_str();
				ex = new CFileException;
				if (lOffset < 1)
				{
					if (binFile.Open(szPackedFile, CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, ex) )
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
					if(binFile.Open( szPackedFile, CFile::shareDenyNone | CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate  | CFile::typeBinary, ex) )
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
				bSuccess = (0 == _access( szPackedFile, 0 ) );

				// global function to close config and clean up c:\run dir
				bSuccess = GlobalRCCloseAndCleanConfiguration();

				if( bSuccess )
				{
					//unpack the files in the c:\run directory
					bSuccess = svPackedFile.UnPackFiles( szPackedFile, SvStl::GlobalPath::Inst().GetRunPath().c_str() );
					if (!bSuccess)
					{
						hrResult = SVMSG_CMDCOMSRV_PACKEDFILE_ERROR;
						bHrSet = TRUE;
						SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
						Exception.setMessage( hrResult, SvOi::Tid_Empty, StdMessageParams );
					}
				}

				if( bSuccess )
				{
					// check for a good path on the config in the packed file
					configFileName = svPackedFile.getConfigFilePath();
					bSuccess = !( configFileName.IsEmpty() );
				}

				if( bSuccess )
				{
					// make sure file exists
					bSuccess = ( 0 == _access( configFileName, 0 ) );
				}

				if( bSuccess )
				{
					//load the config
					LPTSTR pConfigFileName = configFileName.GetBuffer();
					bSuccess = GlobalRCOpenConfiguration( pConfigFileName );
					configFileName.ReleaseBuffer();
				}
			}
		}
		catch (CFileException *theEx)
		{
			TCHAR szCause[255];
			theEx->GetErrorMessage(szCause,255);
#ifdef _DEBUG
			CString sFormatted = _T("Error - ");
			sFormatted += szCause;
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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

				*bstrFileData = SysAllocStringByteLen( NULL, *lBlockSize );
				if (*bstrFileData == NULL)
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
				CString szFormatted = _T("ERROR - ");
				szFormatted += szCause;
#endif

				hrResult = SVMSG_CMDCOMSRV_FILE_ERROR;

				theException->Delete();
			}
			catch (CMemoryException* memEx)
			{
#ifdef _DEBUG
				TCHAR szCause[255];
				memEx->GetErrorMessage(szCause,255);
				CString szFormatted = _T("ERROR - ");
				szFormatted += szCause;
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( hrResult, SvOi::Tid_Empty, StdMessageParams );
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

	CString strPath;
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

	strPath = szDrive;
	strPath += szDir;
	strPath += strFilename;

	CreateDirPath( strPath );

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
			CString sFormatted = _T("ERROR - ");
			sFormatted += szCause;
#endif

			hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;

			theEx->Delete();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( hrResult, SvOi::Tid_Empty, StdMessageParams );
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
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	CString strConfigFile;
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
			strConfigFile = bstrConfigFilename;

			//split filename into peices
			_tsplitpath( strConfigFile, szDrive, szDir, szFile, szExt );

			if( !_tcscmp( szDrive, _T("") ) )
			{ //just the file name, search the run directory for the filename
				if( 0 == _tcscmp( szExt, _T( ".svx" ) ) || 0 == _tcscmp( szExt, _T( "" ) ) )
				{
					_tmakepath( szPath, "C", "\\Run\\", szFile, "svx" );
					//check for existence of file first
					bSuccess = ( 0 == _access( szPath, 0 ) );

					if( bSuccess )
					{
						strConfigFile = szPath;
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
				bSuccess = ( 0 == _access( strConfigFile, 0 ) );

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
				//load the config
				LPTSTR pConfigFile = strConfigFile.GetBuffer();
				bSuccess = GlobalRCOpenConfiguration( pConfigFile );
				strConfigFile.ReleaseBuffer();
			}
		}
		catch (CFileException* &theEx)
		{
			hrResult = SVMSG_CMDCOMCTRL_FILE_ERROR;
			bHrSet = TRUE;
			Exception.setMessage( hrResult, SvOi::Tid_Empty, StdMessageParams );
			theEx->Delete();
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( (!bSuccess) && (!bHrSet) )
		{
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
		}
		break;
	} while (0);

	SVSVIMStateClass::RemoveState( SV_STATE_REMOTE_CMD );

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMInspectionResults(BSTR bstrInspection, BSTR *bstrXMLResults)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	SVXmlException svXmlException;
	SVXmlCmd SvXmlCmd;
	CString szIPD;
	CString szIpdTitle;
	BSTR XMLData;
	unsigned long ulState;
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			szIPD = bstrInspection;

			if(!SvXmlCmd.InitXml())goto xmlerror;

			if(!SvXmlCmd.SetSVIMfilename(szIPD))goto xmlerror;

			if(!SvXmlCmd.SetCommand(_T("GetSVIMInspectionResults")))goto xmlerror;

			//ADD THE REQUIRED RESULTS DATA FROM ALL IPDS AND ALL PUBLISHED RESULTS
			//possibly in a loop using the following function
			//NOTE:  The order in which in the results data per IPD is added to the XML
			//Doc will determine the index number that must be used to retreive the data.
			//i.e first data item added to XML doc will be index zero

			bSuccess = GlobalRCGetState( &ulState );
			if( bSuccess && (0 == (ulState & SV_STATE_LOADING) ) )
			{
				SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc( szIPD );
				if( pIPDoc )
				{
					SVResultsXmlWrapper * pSVRXW = new SVResultsXmlWrapper(&SvXmlCmd);

					if( pIPDoc->GetResults( pSVRXW ) != S_OK )
					{
						goto error;
					}
				}
				else goto error;
			}
			else goto error;

			//save the xml doc into an XML BSTR
			if(!SvXmlCmd.GetXmlDoc(&XMLData))goto xmlerror;

			*bstrXMLResults = SysAllocString(XMLData);

			if(XMLData)
			{
				SysFreeString(XMLData);
			}

			Exception.setMessage( SVMSG_CMDCOMSRV_NO_ERROR, SvOi::Tid_Empty, StdMessageParams );
			break;
error:
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
			break;
xmlerror:
			// we had an XML error, return the right HRESULT
			Exception.setMessage( SVMSG_CMDCOMSRV_XML_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
			break;
		}
		catch (...)
		{
			bSuccess = FALSE;
		}
	} while (0);

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMConfigName(BSTR *bstrConfigFilename)
{
	CString strConfigName;
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			bSuccess = GlobalRCGetConfigurationName( strConfigName.GetBuffer( _MAX_PATH ) );
			strConfigName.ReleaseBuffer();

			if( bSuccess )
			{
				*bstrConfigFilename = strConfigName.AllocSysString();
			}
		}
		catch (...)
		{
			bSuccess = FALSE;
		}

		if( !bSuccess )
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
		}
	} while (0);

	return hrResult;
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
			hrResult = S_FALSE;
		}

	} while (0);

	return hrResult;
}

STDMETHODIMP CSVCommand::SVGetSVIMConfigPrint(long lOffset, long *lBlockSize, BSTR *bstrConfigPrint, BOOL *bLastFlag)
{
	CString strConfigPrint;
	HRESULT hrResult;
	BOOL bSuccess;

	do
	{
		hrResult = S_OK;
		bSuccess = FALSE;

		try
		{
			SVConfigurationPrint printConfig;
			printConfig.printConfigToStringBuffer( strConfigPrint );

			*bstrConfigPrint = strConfigPrint.AllocSysString();
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
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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

	typedef std::map< SVInspectionProcess*, SVCommandInspectionCollectImageData::SVImageIdSet > SVInspectionImageIdMap;
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

		ASSERT( ppsaStatus != NULL );
		ASSERT( *ppsaStatus != NULL );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( ppsaImages != NULL );
		ASSERT( *ppsaImages != NULL );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaImages)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( ppsaOverlays != NULL );
		ASSERT( *ppsaOverlays != NULL );// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( ppsaProcCounts != NULL );
		ASSERT( *ppsaProcCounts != NULL );// must provide allocated SafeArray(LONG)
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
			BSTR bstrName = NULL;
			SafeArrayGetElementNoCopy( psaNames, &i, &bstrName );
			SVString l_Name = SvUl_SF::createSVString(bstrName);

			if( nullptr != pConfig && pConfig->GetInspectionObject(l_Name.c_str(), &pInspection) && nullptr != pInspection )
			{
				SVImageClass* pImage=NULL;
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
				l_ImageNameIds.push_back( SVGetImageListImageInfo( bstrName, SVGUID(), NULL ) ); 

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
			if( l_InspectionIter->first != NULL )
			{
				SVCommandInspectionCollectImageDataPtr l_DataPtr = new SVCommandInspectionCollectImageData( l_InspectionIter->first->GetUniqueObjectID(), l_InspectionIter->second );
				SVObjectSynchronousCommandTemplate< SVCommandInspectionCollectImageDataPtr > l_Command( l_InspectionIter->first->GetUniqueObjectID(), l_DataPtr );

				if( l_Command.Execute( 120000 ) == S_OK )
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
			BSTR bstrImage = NULL;
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

					bstrImage = SysAllocStringByteLen( NULL, static_cast< UINT >( l_ImageIter->second.m_ImageDIB.size() ) );
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

					if ( hr == S_OK )
					{
						// Copy data to DIB memory locations
						memcpy( pDIB, &(l_ImageIter->second.m_ImageDIB[0]), l_ImageIter->second.m_ImageDIB.size() );
					}

					if ( hr == S_OK )
					{
						lProcessCount = l_DataPtr->GetProduct().m_TriggerCount;

						l_OverlayClass = l_ImageIter->second.m_OverlayData;
					}

					if ( hr == S_OK )
					{
						// Add the status to the outgoing array
						HRESULT hrItem = S_OK;
						hr = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrItem);
						if (hr != S_OK)
						{
							throw -1580;
						}

						// Add the image to the outgoing array
						SafeArrayPutElementNoCopy( *ppsaImages, &l, bstrImage );
						//::SysFreeString( bstrImage );

						hr = SafeArrayPutElement(*ppsaProcCounts, &l, (void*) &lProcessCount);

						BYTE* pBytes=NULL;
						long lSize;
						l_OverlayClass.GetBufferSize( lSize );
						l_OverlayClass.GetBuffer( pBytes );

						BSTR bstrOverlay = ::SysAllocStringByteLen( NULL, lSize );
						memcpy( bstrOverlay, pBytes, lSize );
						//delete pOverlayClass;

						// Add overlays to the outgoing array
						SafeArrayPutElementNoCopy( *ppsaOverlays, &l, bstrOverlay );
						//::SysFreeString( bstrOverlay );

						bImageOk = true;
					}
				}
			}

			if ( !bImageOk )
			{
				// Add NULL to the outgoing array
				lProcessCount = -1;
				::SafeArrayPutElement( *ppsaImages, &l, NULL );
				::SafeArrayPutElement( *ppsaOverlays, &l, NULL );
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
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
	}

	return hrResult;
}


STDMETHODIMP CSVCommand::SVRegisterStream(SAFEARRAY* psaName, VARIANT vtInterface, SAFEARRAY** ppsaStatus)
{
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	HRESULT hr = S_OK;
	HRESULT hrRet = S_OK;
	BSTR bstrName;
	CString sName;
	CString sInspectionName;
	BOOL bSamePPQ = TRUE;
	SVInspectionProcess* pInspection( nullptr );
	BOOL bRet = TRUE;
	BOOL bNotAllItemsFound=FALSE;
	CStringList cslItemsNotFound;
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
				Exception.setMessage( hrRet, SvOi::Tid_Empty, StdMessageParams );
			}

			return hrRet;
		}// end if

		hr = Fire_TestConnection();
		if ( hr != S_OK )
		{
			hrRet = SVMSG_SERVER_UNABLE_TO_CONNECT_TO_CONTROL;
			Exception.setMessage( hrRet, SvOi::Tid_Empty, StdMessageParams );

			return hrRet;
		}// end else

		if ( bRet )
		{
			CSVCommand* pThis = this;
			StreamDataStruct* pstData;

			m_cslInspectionNames.RemoveAll();
			m_arInspections.RemoveAll();

			//check list of names to make sure they are all on the same PPQ
			long lNumberOfElements = psaName->rgsabound[0].cElements;

			for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
			{
				SafeArrayGetElementNoCopy(psaName, &lIndex, &bstrName);
				sName = bstrName;
				sInspectionName = sName.Left(sName.Find('.'));

				if ( nullptr == m_cslInspectionNames.Find(sInspectionName) )
				{
					// add inspection name to list
					m_cslInspectionNames.AddHead(sInspectionName);
				}//end if
			}// end for

			//go through list of inspection names and make sure they are all on same PPQ
			int iInspectionCnt = static_cast< int >( m_cslInspectionNames.GetCount() );

			SVGUID l_PPQId;
			POSITION posIns;
			int iInsDel = 0;

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			for ( int i = 0; ( (i < iInspectionCnt) && (bSamePPQ) ); i++ )
			{
				CString sTmpInsName;

				posIns = m_cslInspectionNames.FindIndex(i-iInsDel);
				if ( posIns )
				{
					sTmpInsName = m_cslInspectionNames.GetAt(posIns);

					if ( nullptr != pConfig && pConfig->GetInspectionObject(sTmpInsName,&pInspection) && nullptr != pInspection )
					{
						if ( l_PPQId.empty() )
						{
							l_PPQId = pInspection->GetPPQIdentifier();
						}
						else
						{
							if ( l_PPQId != pInspection->GetPPQIdentifier() )
							{
								bSamePPQ = FALSE;
							}
						}
					}
					else
					{
						m_cslInspectionNames.RemoveAt(posIns);
						iInsDel++;
					}
				}
			}

			// requested values are not on same PPQ - return an error...
			if ( !bSamePPQ )
			{
				hrRet = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS;
				Exception.setMessage( hrRet, SvOi::Tid_Empty, StdMessageParams );
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
				CString sTmp;

				SafeArrayGetElementNoCopy(psaName,&x,&bstrName);
				sTmp = bstrName;

				sInspectionName = sTmp.Left(sTmp.Find('.'));
				BOOL bFound = FALSE;

				pstData = new StreamDataStruct;

				if ( nullptr != pConfig && pConfig->GetInspectionObject(sTmp,&pInspection) )
				{
					if ( sInspectionName == pInspection->GetName() )
					{
						bFound = TRUE;
					}

					if ( bFound )
					{
						l_bGoodInspection = TRUE;
						pstData->m_InspectionID = pInspection->GetUniqueObjectID();
						pstData->strValueName = sTmp;

						SVValueObjectReference ref;
						HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( sTmp ), ref );
						if( hrFind != S_OK )
						{
							pstData->pValueObject = NULL;
							m_arStreamList.Add( pstData );
							hrRet = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
						}// end if
						else
						{
							if ( ref.Object() )
							{
								hrStatus = S_OK;
								pstData->pValueObject = ref.Object();
								pstData->arrayIndex = ref.ArrayIndex();
								m_arStreamList.Add( pstData );
							}// end if
							else
							{
								pstData->pValueObject = NULL;
								m_arStreamList.Add( pstData );
								hrRet = SVMSG_OBJECT_NOT_PROCESSED;
								::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
								bNotAllItemsFound = TRUE;
							}// end else
						}// end else
					}// end if
				} // if ( pInspection != NULL )
				else
				{	// Inspection Not Found
					// put following error code in
					pstData->m_InspectionID.clear();
					pstData->pValueObject = NULL;
					pstData->strValueName = sTmp;
					m_arStreamList.Add( pstData );

					// to compile 
					hrRet = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
					::SafeArrayPutElement(*ppsaStatus,&x,&hrRet);
				}// end else
			}// end for

			for ( int iAdd = 0; iAdd < iInspectionCnt; iAdd++ )
			{
				POSITION pos = m_cslInspectionNames.FindIndex(iAdd);
				if ( pos != NULL )
				{
					CString sTmpIns = m_cslInspectionNames.GetAt(pos);
					pInspection = nullptr;
					if( nullptr != pConfig){ pConfig->GetInspectionObject(sTmpIns,&pInspection); }
					if (nullptr != pInspection && sTmpIns == pInspection->GetName() )
					{
						SVCommandStreamManager::Instance().EnableInspectionCallback( pInspection->GetUniqueObjectID() );

						// add inspections to list
						m_arInspections.Add( pInspection );
					}// end if
				}// end if
			}// end for

			RebuildStreamingDataList();

			m_bRunStreamData = TRUE;
			m_bRegisteredStream = TRUE;

			SVCommandStreamManager::Instance().InsertCommandCallback( 
				boost::bind( &CSVCommand::StreamingDataCallback, this, _1 ), 
				boost::bind( &CSVCommand::RebuildStreamingDataList, this ) );

			m_hStopStreamEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
			m_hStreamingThread = ::CreateThread( NULL, 0, SVStreamDataThread, this, 0, NULL);					
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
				m_hStopStreamEvent = NULL;
			}

			if( m_hStreamingThread )
			{
				::CloseHandle(m_hStreamingThread);
				SVThreadManager::Instance().Remove( m_hStreamingThread );
				m_hStreamingThread = NULL;
			}

			m_bRegisteredStream = FALSE;
			m_bRunStreamData = FALSE;

			SVCommandStreamManager::Instance().EraseCommandCallback();
			m_cslInspectionNames.RemoveAll();
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
		::EnterCriticalSection( &m_hProductCritSect );

		// search the list for this product
		bFound = FALSE;
		pProductData = NULL;
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

			if( l_Iter != p_rData.m_ProductInfo.m_svInspectionInfos.end() )
			{
				oPacketData.lState = l_Iter->second.oInspectedState;
				oPacketData.pValueObject = pStreamData->pValueObject;
				if( oPacketData.pValueObject )
				{
					if (pStreamData->arrayIndex > 0)
					{
						// Ensure valueObject is an Array
						if (oPacketData.pValueObject->IsArray())
						{
							// Ensure index is not ot of bounds
							if (pStreamData->arrayIndex < oPacketData.pValueObject->GetArraySize())
							{
								oPacketData.pValueObject->GetValue( p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), pStreamData->arrayIndex, oPacketData.strValue );
							}
							else
							{
								oPacketData.strValue.Empty();
							}
						}
						else
						{
							oPacketData.strValue.Empty();
						}
					}
					else
					{
						oPacketData.pValueObject->GetValue( p_rData.m_ProductInfo.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), oPacketData.strValue );
					}
				}// end if
				else
				{
					oPacketData.strValue.Empty();
				}// end else

				pProductData->arPacketData.SetAt( l, oPacketData );
			}// end if
		}// end for

		pProductData->lCallbackCount++;

		// Unprotect the to be streamed list
		::LeaveCriticalSection( &m_hProductCritSect );

		// if streaming thread is starved, temporarily boost it
		if( m_arProductList.GetSize() > 20 )
		{
			::SetThreadPriority( m_hStreamingThread, THREAD_PRIORITY_NORMAL );
		}// end if

		::QueueUserAPC( SVStreamingDataAPCProc, m_hStreamingThread, ( DWORD_PTR ) NULL );
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
	CString strValue;
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
	CoInitialize( NULL );    

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
				if( pProductData->lCallbackCount == m_cslInspectionNames.GetCount() )
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

				lProductCount = static_cast< long >( pThis->m_arProductList.GetSize() );
				for( l = 0; lStreamCount && l < lProductCount; l++ )
				{
					pProductData = pThis->m_arProductList.GetAt( l );

					// Verify again that this is one of the ready products
					// @TODO:  Should check pProductData before dereferencing.
					if( pProductData->lCallbackCount != m_cslInspectionNames.GetCount() )
					{
						continue;
					}// end if

					for( k = 0; k < lStreamSize; k++ )
					{
						lInspectIndex = -1;

						pStreamData = pThis->m_arStreamList.GetAt( k );
						oPacketData = pProductData->arPacketData.GetAt( k );

						if( pStreamData == NULL || pStreamData->pValueObject == NULL || 
							oPacketData.pValueObject == NULL || 
							( oPacketData.lState & PRODUCT_INSPECTION_NOT_RUN ) == PRODUCT_INSPECTION_NOT_RUN ||
							pStreamData->pValueObject != oPacketData.pValueObject )
						{
							// product not found or item not found or product not inspected. 
							// send -1 for process count to keep list same size, as what they requested.
							bstr = pStreamData->strValueName.AllocSysString();
							SafeArrayPutElementNoCopy( saNames, &lIndex, bstr );
							//::SysFreeString( bstr );

							strValue = _T( "" );
							bstr = strValue.AllocSysString();
							SafeArrayPutElementNoCopy( saValues, &lIndex, bstr );
							//::SysFreeString( bstr );

							::SafeArrayPutElement( saProcessCount, &lIndex, &lNoProduct );

							lIndex++;
						}// end if
						else
						{
							// product and item were found, add values to SAFEARRAYS
							bstr = pStreamData->strValueName.AllocSysString();
							SafeArrayPutElementNoCopy( saNames, &lIndex, bstr );
							//::SysFreeString( bstr );

							bstr = oPacketData.strValue.AllocSysString();
							SafeArrayPutElementNoCopy( saValues, &lIndex, bstr );
							//::SysFreeString( bstr );

							::SafeArrayPutElement( saProcessCount, &lIndex, &pProductData->lProductCount );

							lIndex++;
						}//end else

					} // end for

					m_lLastStreamedProduct = pProductData->lProductCount;
					lProductCount = static_cast< long >( pThis->m_arProductList.GetSize() );
					lStreamCount--;

					::EnterCriticalSection( &m_hProductCritSect );

					pThis->m_arProductList.RemoveAt( l );

					::LeaveCriticalSection( &m_hProductCritSect );
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
			lProductCount = static_cast< long >( pThis->m_arProductList.GetSize() );
			lStreamCount = 0;

			for( l = 0; l < lProductCount; l++ )
			{
				pProductData = pThis->m_arProductList.GetAt( l );
				if( pProductData->lCallbackCount == m_cslInspectionNames.GetCount() )
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
				m_cslInspectionNames.RemoveAll();
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
		pThis->m_hStopStreamEvent = NULL;
	}// end if

	if( pThis->m_hStreamingThread )
	{
		::CloseHandle( pThis->m_hStreamingThread );
		pThis->m_hStreamingThread = NULL;
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
	CStringArray saValueNames;
	SVInspectionProcessArray aInspections;
	SVValueObjectReferenceVector aValueObjects;
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

	ASSERT( ppsaStatus != nullptr );
	ASSERT( *ppsaStatus != nullptr );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

	ASSERT( ppsaData != nullptr );
	ASSERT( *ppsaData != nullptr );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaData)->rgsabound[0].cElements == lNumberOfElements );

	long lDefaultStatus = S_FALSE;

	// create String array of value names; set default Status
	for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
	{
		SafeArrayGetElementNoCopy(psaNames, &lIndex, &bstr);
		CString sName( bstr );
		saValueNames.Add( sName );

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

	for ( long i = 0; i < lNumberOfElements && SUCCEEDED( hrOK ); i++ )
	{
		SVInspectionProcess* pInspection( nullptr );
		//GetInspectionObject is only true if the pointer is valid
		if ( (nullptr != pConfig) && pConfig->GetInspectionObject(saValueNames[i], &pInspection))
		{
			aInspections.Add(pInspection);	// add inspection object to list

			SVObjectReference ref;
			SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( saValueNames[i] ), ref );
			if( ref.Object() )
			{
				aValueObjects.push_back(ref);	// add data object pointer to the list

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
				aValueObjects.push_back( SVValueObjectReference() );
				hrOK = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &i, (void*) &hrOK);
			}
		}
		else	// couldn't find inspection
		{
			aInspections.Add( NULL );
			aValueObjects.push_back( SVValueObjectReference() );
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
			SVValueObjectReference ref = aValueObjects[i];
			CString sValue;
			HRESULT hrStatus = S_OK;

			if ( ref.Object() != NULL )
			{
				SVDataManagerHandle	l_BucketHandle;

				l_ProductInfo.GetResultDataIndex( l_BucketHandle );

				if ( !ref.IsEntireArray() )
				{
					HRESULT hrGet = ref.GetValue(l_BucketHandle.GetIndex(), sValue);
					if ( hrGet == S_OK )
					{
						// put value in return array
						BSTR bstrTemp = sValue.AllocSysString();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTemp);
						//::SysFreeString( bstrTemp );

						HRESULT hrItem = S_OK;
						SafeArrayPutElement(*ppsaStatus, &i, &hrItem);
					}// if OK
					else if ( hrGet == SVMSG_SVO_33_OBJECT_INDEX_INVALID 
						|| hrGet == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
					{
						hrStatus = hrGet;
						// did not get value.  set value to default
						sValue = ref.DefaultValue();
						if ( sValue.IsEmpty() )
						{
							sValue.Format("%i",-1);
						}
						BSTR bstrTmpVal = sValue.AllocSysString();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal);
						//::SysFreeString(bstrTmpVal);
						::SafeArrayPutElement(*ppsaStatus, &i, &hr);
						l_bItemNotExist = TRUE;
					}// else invalid or out of range index
					else	// some generic error; currently should not get here
					{
						ASSERT( FALSE );
						hrStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						// did not get value.  set value to -1
						sValue.Empty();
						sValue.Format("%i",-1);
						BSTR bstrTmpVal = sValue.AllocSysString();
						SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal);
						//::SysFreeString(bstrTmpVal);
						::SafeArrayPutElement(*ppsaStatus, &i, &hr);
						l_bItemNotExist = TRUE;
					}
				}// if ( !ref.IsEntireArray() )
				else	// GET ENTIRE ARRAY
				{
					// get all results and put them into a parsable string
					int iNumResults = 0;
					ref.Object()->GetResultSize(l_BucketHandle.GetIndex(), iNumResults);
					CString sArrayValues;
					for ( int iArrayIndex = 0; iArrayIndex < iNumResults; iArrayIndex++ )
					{
						CString sValue;
						HRESULT hrGet = ref.Object()->GetValue( l_BucketHandle.GetIndex(), iArrayIndex, sValue );
						if ( hrGet == S_OK )
						{
							if ( iArrayIndex > 0 )
							{
								sArrayValues += _T(",");
							}

							sArrayValues += _T("`");
							sArrayValues += sValue;
							sArrayValues += _T("`");
						}
						else
						{
							break;
						}
					}

					hrStatus = S_OK;
					BSTR bstrTmpVal = sArrayValues.AllocSysString();
					SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTmpVal);
					::SafeArrayPutElement(*ppsaStatus, &i, &hrStatus);
				}// end if ( !ref.IsEntireArray() ) else
			}// end if ( ref.Object() != NULL )
			else
			{
				// sValue is empty at this point
				BSTR bstrTemp = sValue.AllocSysString();
				SafeArrayPutElementNoCopy(*ppsaData, &i, bstrTemp);
				//::SysFreeString( bstrTemp );

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

		ASSERT( ppsaStatus != NULL );
		ASSERT( *ppsaStatus != NULL );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaStatus)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( ppsaImages != NULL );
		ASSERT( *ppsaImages != NULL );	// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaImages)->rgsabound[0].cElements == lNumberOfElements );

		ASSERT( ppsaOverlays != NULL );
		ASSERT( *ppsaOverlays != NULL );// must provide allocated SafeArray(LONG)
		ASSERT( (*ppsaOverlays)->rgsabound[0].cElements == lNumberOfElements );

		BSTR bstr = NULL;
		SVImageClassArray aImageObjects;
		SVInspectionProcessArray aInspections;
		bool l_bItemNotFound = false;
		bool l_bInspectionNotFound = false;
		BSTR bstrOverlay = NULL;

		long lDefaultStatus = S_FALSE;

		// set default status for objects in list
		for ( long lIndex = 0; lIndex < lNumberOfElements; lIndex++ )
		{
			HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &lIndex, &lDefaultStatus);
			// No overlays for no for product source images
			// overlays are not bucketized
			bstr = NULL;
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
			BSTR bstrName = NULL;
			SafeArrayGetElementNoCopy( psaNames, &l, &bstrName );
			CString strName( bstrName );

			if (nullptr != pConfig && pConfig->GetInspectionObject(strName, &pInspection))
			{
				aInspections.Add( pInspection );
				SVImageClass* pImage=NULL;
				if( pInspection->GetChildObjectByName(strName, (SVObjectClass**) &pImage))
				{
					bool bImageOK = false;
					if( dynamic_cast< SVCameraImageTemplate* >( pImage ) ) // Source image
					{
						aImageObjects.Add(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else if ( pImage->ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE )	// Published result image
					{
						aImageObjects.Add(pImage);	// add data object pointer to the list
						bImageOK = true;
					}
					else
					{
						hrOK = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE;
						l_bItemNotFound = true;
						aImageObjects.Add( NULL );
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
					aImageObjects.Add( NULL );
					HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);

					// Add NULL to the outgoing array
					::SafeArrayPutElement( *ppsaImages, &l, NULL );
					::SafeArrayPutElement( *ppsaOverlays, &l, bstrOverlay );
				}
			}// if (TheSVObserverApp.m_pConfiguration->GetInspectionObject(strName, &pInspection))
			else	// couldn't find inspection
			{
				hrOK = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
				l_bInspectionNotFound = true;
				aImageObjects.Add( NULL );
				aInspections.Add( NULL );
				HRESULT hrTemp = SafeArrayPutElement(*ppsaStatus, &l, (void*) &hrOK);
			}
		}// end for ( long l = 0; l < lNumberOfElements && SUCCEEDED( hrOK ); l++ )

		// requested values are not valid - return an error...
		if ( ! SUCCEEDED( hrOK ) )
		{
			throw hrOK;
		}

		// Verify that the values are still alive on the PPQ (it has a TriggerCount that matches the specified ProcessCount)
		SVProductInfoStruct l_ProductInfo;

		SVPPQObject* pPPQ = dynamic_cast< SVPPQObject* >( SVObjectManagerClass::Instance().GetObject( l_PPQId ) );

		if ( nullptr != pPPQ )
		{
			if( pPPQ->GetProduct( l_ProductInfo, lProcessCount ) == S_OK )
			{
				// Product is still valid; 
				// Build the output images from the result data index in the product and the source images by name  (?)
				for (l=0; l < lNumberOfElements; l++)
				{
					HRESULT hrImage = S_OK;
					if ( aInspections[l] != NULL )
					{
						bool bImageOk = false;

						SVImageClass* pImage = aImageObjects[l];
						if ( pImage )
						{
							SVImageInfoClass svImageInfo;
							SVSmartHandlePointer svImageHandle;
							BSTR bstrImage = NULL;

							// this works for Source Images (SVMainImageClass) and Published Result images
							SVImageIndexStruct svIndex( pImage->GetSourceImageIndex( &l_ProductInfo ) );

							// put image in return array
							BSTR bstrTemp = NULL;

							HRESULT hr = SafeImageToBSTR( pImage, svIndex, &bstrTemp);

							if ( SUCCEEDED( hr ) )
							{
								// Add the status to the outgoing array
								HRESULT hrItem = S_OK;
								SafeArrayPutElement(*ppsaStatus, &l, &hrItem);

								// Add the image to the outgoing array
								SafeArrayPutElementNoCopy(*ppsaImages, &l, bstrTemp);
								//::SysFreeString( bstrTemp );

								bImageOk = true;
							}
							else
							{
								hrImage = SVMSG_INVALID_IMAGE_SOURCE;
							}
						}// end if ( aImageObjects[i] != NULL )

						if ( !bImageOk )
						{
							// put NULL image in return array
							SafeArrayPutElement(*ppsaImages, &l, NULL);

							if ( hrImage == S_OK )
								hrImage = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							SafeArrayPutElement(*ppsaStatus, &l, &hrImage);
							l_bItemNotFound = true;
						}// end if ( !bImageOk )
					}// end if ( aInspections[l] != NULL )
				}// end for (long i=0; i < lNumberOfElements; i++)
			}// end if (pPPQ->GetProductInfoStruct(lProcessCount, &pProductInfoStruct))
			else
			{
				hrResult = SVMSG_PRODUCT_NO_LONGER_AVAILABLE;	// product no longer alive
			}
		}// end if ( pPPQ != NULL )

		if ( l_bItemNotFound || l_bInspectionNotFound )
		{
			if ( hrResult == S_OK )
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
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
	}

	return hrResult;
}

STDMETHODIMP CSVCommand::SVSetLUT(BSTR bstrCameraName, SAFEARRAY* paulLUTTable)
{
	HRESULT hr = S_OK;

	CString sCameraName( bstrCameraName );
	SVVirtualCamera* pCamera( nullptr );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( nullptr != pConfig && pConfig->GetChildObjectByName(sCameraName, &pCamera) && nullptr != pCamera )
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

	CString sCameraName( bstrCameraName );
	SVVirtualCamera* pCamera( nullptr );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && pConfig->GetChildObjectByName(sCameraName, &pCamera) && nullptr != pCamera )
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
		SVMatroxBufferInterface::SVStatusCode l_Code;

		SVImageInfoClass oChildInfo;
		SVSmartHandlePointer oChildHandle;
		BITMAPINFOHEADER* pbmhInfo = NULL;
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

		long l_lType = SVImageTypeUnknown;
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

		oChildInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );
		oChildInfo.GetImageProperty( SVImagePropertyBandLink, l_lBandLink );

		if ( IsColor && l_lType == SVImageTypePhysical && l_lBandNumber == 3)
		{
			bDestroyHandle = TRUE;

			oChildInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

			HRESULT hrImage = SVImageProcessingClass::Instance().CreateImageBuffer( oChildInfo, oChildHandle );

			SVImageBufferHandleImage l_ChildMilBuffer;

			if( !( oChildHandle.empty() ) )
			{
				oChildHandle->GetData( l_ChildMilBuffer );
			}

			// Copy input image to output image
			l_Code = SVMatroxBufferInterface::CopyBuffer(l_ChildMilBuffer.GetBuffer(), l_MilBuffer.GetBuffer(), l_lBandLink );
		} 

		if ((l_lType == SVImageTypeLogicalAndPhysical) ||
			(l_lType == SVImageTypeLogical))
		{
			bDestroyHandle = TRUE;

			HRESULT hrImage = SVImageProcessingClass::Instance().CreateImageBuffer( oChildInfo, oChildHandle );

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

		if ( hr == S_OK )
		{
			// Copy data to DIB memory locations
			memcpy( pDIB, l_BitmapInfo.GetBitmapInfo(), sizeof( BITMAPINFOHEADER ) + lTabSize );
			memcpy( pDIB + sizeof( BITMAPINFOHEADER ) + lTabSize, 
				oChildHandle->GetBufferAddress(), 
				pbmhInfo->biSizeImage );
		}

		//--- For the case where compression is not zero the pDIB value can no longer
		//--- be used because the Global Memory Block has been unlocked. When 
		//--- alCompression equals zero it is set to NULL only for compatabillity. 
		//--- We no longer need this value for reference. We still have the BSTR       
		//--- reference.
		pDIB = NULL;
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

		SVImageProcessingClass::Instance().CreateImageBuffer( oChildInfo, oChildHandle );

		if( SV_IS_KIND_OF( p_pImage, SVRGBMainImageClass ) )
		{
			p_pImage->SafeImageConvertToHandle( p_svIndex, oChildHandle, SVImageHLSToRGB );
		}
		else
		{
			p_pImage->SafeImageCopyToHandle( p_svIndex, oChildHandle );
		}

		hr = ImageToBSTR( oChildInfo, oChildHandle, pbstr);
	}
	else
	{
		hr = -1578;
	}

	return hr;
}

HRESULT CSVCommand::BSTRToImage(bool bCreateNew, const BSTR bstr, SVImageInfoClass& rImageInfo, SVSmartHandlePointer &rImageHandle)
{
	HRESULT hr = ERROR_CALL_NOT_IMPLEMENTED;
	ASSERT( hr != ERROR_CALL_NOT_IMPLEMENTED );
	return hr;
}

HRESULT CSVCommand::SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv)
	// Does a blind copy of the requested element.
	// E.G., if the element is a BSTR (wchar_t*), only the value of the pointer is copied,
	//      not the contents of the BSTR.
{
	void* pElement;
	HRESULT hr = SafeArrayGetElementPointer(psa, rgIndices, &pElement);
	if (hr == S_OK)
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
	if (hr == S_OK)
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

	ASSERT( ppsaStatus != NULL );
	ASSERT( *ppsaStatus != NULL );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaStatus)->rgsabound[0].cElements == Size );

	ASSERT( ppsaValues != NULL );
	ASSERT( *ppsaValues != NULL );	// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaValues)->rgsabound[0].cElements == Size );

	ASSERT( ppsaProcCounts != NULL );
	ASSERT( *ppsaProcCounts != NULL );// must provide allocated SafeArray(LONG)
	ASSERT( (*ppsaProcCounts)->rgsabound[0].cElements == Size );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	for ( long l = 0; l < Size; l++ )
	{
		CString Name;
		CString Value;
		BSTR bstrName = NULL;
		long ProcessCount = -1;
		SVObjectReference ObjectRef;
		SVInspectionProcess* pInspection( nullptr );

		Status = S_OK;

		// Get name of requested value out of the safearray
		SafeArrayGetElementNoCopy(psaNames,&l,&bstrName);
		Name = bstrName;
		ProcessCount = -1;
		if( nullptr != pConfig )
		{
			pConfig->GetInspectionObject(Name,&pInspection);
		}

		SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( Name ), ObjectRef );

		if ( nullptr != ObjectRef.Object() || nullptr != pInspection )
		{
			//If inspection  is nullptr then object is of type BasicValueObject
			if( nullptr == pInspection )
			{
				BasicValueObject* pValueObject = dynamic_cast< BasicValueObject* >( ObjectRef.Object() );
				if( nullptr != pValueObject && !ObjectRef.IsEntireArray() )
				{
					SVString TempValue;

					HRESULT hrGet = pValueObject->getValue( TempValue );
					Value = TempValue.c_str();

					if ( S_OK != hrGet )
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
				SVValueObjectReference ValueObjectRef;
				SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( Name ), ValueObjectRef );

				if( NULL	!= ValueObjectRef.Object() )
				{
					int iBucket = ValueObjectRef->GetLastSetIndex();
					ProcessCount = pInspection->LastProductGet( SV_OTHER ).ProcessCount();

					if ( !ValueObjectRef.IsEntireArray() )
					{
						// was able to find the object
						HRESULT hrGet = ValueObjectRef.GetValue(iBucket, Value);
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
							Value = ValueObjectRef.DefaultValue();
							if ( Value.IsEmpty() )
							{
								Value.Format("%i",-1);
							}
							ItemNotFound = TRUE;
						}// else invalid or out of range index
						else	// some generic error; currently should not get here
						{
							ASSERT( FALSE );
							Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
							// did not get value.  set value to -1
							Value.Empty();
							Value.Format("%i",-1);
							ItemNotFound = TRUE;
						} //end else generic error
					}// if ( !ValueObjectRef.IsEntireArray() )
					else	// GET ENTIRE ARRAY
					{
						// get all results and put them into a parsable string
						int iNumResults = 0;
						ValueObjectRef->GetResultSize(iBucket, iNumResults);
						CString sArrayValues;
						for ( int iArrayIndex = 0; iArrayIndex < iNumResults; iArrayIndex++ )
						{
							CString sValue;
							HRESULT hrGet = ValueObjectRef->GetValue( iBucket, iArrayIndex, sValue );
							if ( hrGet == S_OK )
							{
								if ( iArrayIndex > 0 )
									sArrayValues += _T(",");
								sArrayValues += _T("`");
								sArrayValues += sValue;
								sArrayValues += _T("`");
							}
							else
							{
								break;
							}
						}

						Status = S_OK;
						Value = sArrayValues;
					}// end if ( !ValueObjectRef.IsEntireArray() ) else
				} // end if found object
				else
				{
					// could not find object
					Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
					// did not get value.  set value to -1
					Value.Empty();
					Value.Format("%i",-1);
					ItemNotFound = TRUE;
				} //else could not find object
			}// else inspection object
		}//end if object or inspection
		else
		{ //inspection not found
			Status = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			// did not get value.  set value to -1
			Value.Empty();
			Value.Format("%i",-1);
			InspectionNotFound = TRUE;
		}// else inspection not found

		//Results have been prepared so can place them in the list
		BSTR bstrTmpVal = Value.AllocSysString();
		SafeArrayPutElementNoCopy(*ppsaValues,&l,bstrTmpVal);
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
			GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
			SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

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
			if( pInspection->AddInputImageRequest( pMainImage, bstrImage ) != S_OK )
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
	SVValueObjectClass*    pValueObject( nullptr );
	SVValueObjectReference ref;
	BSTR                   bstrName;
	BSTR                   bstrValue;
	CString                sTmpName;
	CString                sTmpVal;
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

			sTmpName = bstrName;

			hrStatus = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
			if ( FAILED( hrStatus ) ) { break; }

			sTmpVal = bstrValue;

			//GetInspectionObject is only true if the pointer is valid
			if ( nullptr != pConfig && pConfig->GetInspectionObject(sTmpName,&pInspection) )
			{
				//got the inspection.
				SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( sTmpName ), ref );
				pValueObject = ref.Object();
				if( pValueObject != NULL )
				{
					SVObjectClass* pOwnerObject = pValueObject->GetOwner();

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
				pInspection->AddInputRequest( ref, W2T( bstrValue ) );

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
	BSTR bstrName = NULL;
	BSTR bstrImage = NULL;
	CString sTmpName;
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

			sTmpName = bstrName;
			::SysFreeString(bstrName);

			hrStatus = SafeArrayGetElementNoCopy(psaImages, &l, &bstrImage);
			if ( FAILED( hrStatus ) ) { break; }

			//GetInspectionObject is only true if the pointer is valid
			if ( nullptr != pConfig &&  pConfig->GetInspectionObject(sTmpName,&pInspection)  && nullptr != pInspection )
			{
				SVObjectClass *l_pObject = NULL;
				//got the inspection.
				if ( pInspection->GetChildObjectByName( sTmpName, &l_pObject ))
				{
					l_pImageObject = dynamic_cast< SVImageClass* >( l_pObject );

					if ( l_pImageObject != NULL &&
						(l_pImageObject->ObjectAttributesAllowed() & SV_REMOTELY_SETABLE) == SV_REMOTELY_SETABLE )
					{
						// currently all SV_REMOTELY_SETABLE parameters are also SV_SETABLE_ONLINE
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
				pInRequest->m_strObjectName = sTmpName;

				l_ImageInfo = l_pImageObject->GetImageInfo();

				if ( S_OK == SVImageProcessingClass::Instance().LoadImageBuffer( (void*)bstrImage, 
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
	inline CString StripBrackets(const CString & str)
	{
		CString ret = str;
		ret.Trim();
		int idx = ret.ReverseFind(_T('['));
		if( -1 < idx )
		{
			ret.Truncate(idx);
		}
		return ret;
	}

	inline CString StripQuotes(const CString & str)
	{
		CString ret = str;
		ret.Trim();
		ret.Trim(_T("`\'")); // strip single quotes or back-ticks
		return ret;
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
		CString					Name;
		CString					Value;
		SVInspectionProcess*	pInspection = nullptr;
		SVObjectReference		ObjectRef;

		BOOL AddRequest = FALSE;

		BSTR bstrName = nullptr;
		Status = SafeArrayGetElementNoCopy(psaNames, &l, &bstrName);
		if ( FAILED( Status ) ) { break; }

		Name = bstrName;

		BSTR bstrValue = nullptr;
		Status = SafeArrayGetElementNoCopy(psaValues, &l, &bstrValue);
		if ( FAILED( Status ) ) { break; }

		Value = bstrValue;
		if(nullptr != pConfig)
		{
			pConfig->GetInspectionObject(Name, &pInspection);
		}
		HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( Name ), ObjectRef );

		if ( nullptr != ObjectRef.Object() || nullptr != pInspection )
		{
			if ( hrFind == S_OK && ObjectRef.ArrayIndex() < 0 && !ObjectRef->IsArray())
			{
				Name = StripBrackets(Name);
				hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( Name ), ObjectRef );
				if (S_OK == hrFind)
				{
					Value = StripQuotes(Value);
				}
			}

			if ( S_OK == hrFind )
			{
				// Check if item is already in the list
				if( nullptr != ObjectRef.Object() )
				{
					HRESULT hres = S_OK;
					SVString strCompleteObjectName( ObjectRef.GetCompleteObjectName() );
					if ( ParameterNames.find( strCompleteObjectName ) != ParameterNames.end() )
					{
						// Item is already in the list
						Status = SVMSG_OBJECT_ALREADY_SET_IN_THIS_LIST;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if ( (ObjectRef.ObjectAttributesAllowed() & SV_REMOTELY_SETABLE) != SV_REMOTELY_SETABLE )
					{
						// Item is not allowed to be set remotely
						Status = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if (StateOnline && ((ObjectRef.ObjectAttributesAllowed() & SV_SETABLE_ONLINE) != SV_SETABLE_ONLINE)  ) 
					{
						Status = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
						::SafeArrayPutElement(*ppsaStatus, &l, &Status);
						ItemErrorCount++;
					}
					else if(RangeClassHelper::IsOwnedByRangeObject((*ObjectRef.Object())) && !RangeClassHelper::IsAllowedToSet(*ObjectRef.Object(),Value,StateOnline, hres))
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

	SVImageClass* pImage = NULL;
	SVImageIndexStruct l_svImageIndex;
	SVDataManagerHandle l_DMImageIndexHandle;

	SVInspectionProcess* pInspection = nullptr;

	//GetInspectionObject is only true if the pointer is valid
	if ( nullptr != pConfig && pConfig->GetInspectionObject(CString(p_bsName), &pInspection) )
	{
		if (pInspection->GetChildObjectByName(CString(p_bsName), (SVObjectClass**) &pImage))
		{
			SVPPQObject* pPPQ = pInspection->GetPPQ();	// inspection can be part of only one PPQ

			if( nullptr != pPPQ )
			{
				SVProductInfoStruct l_ProductInfo;

				if( pPPQ->GetProduct( l_ProductInfo, p_lProcessCount ) == S_OK )
				{
					SVaxls.strName = CString(p_bsName);
					SVaxls.lProcessCount = p_lProcessCount;
					if( SVCameraImageTemplate* pMainImage = dynamic_cast< SVCameraImageTemplate* >( pImage ) )
					{
						l_svImageIndex = pMainImage->GetSourceImageIndex( &l_ProductInfo );
						l_DMImageIndexHandle.Assign( l_svImageIndex.m_CameraDMIndexHandle, SV_DCOM );
					}
					else if ( pImage->ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE )
					{
						l_svImageIndex = pImage->GetSourceImageIndex( &l_ProductInfo );
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
				// BRW - Why should we do nothing here?  If we should do nothing, then remove the else, but we should probably be setting an error.
				NULL;//	hr = SVMSG_NO_PPQ_FOUND;
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
	if ( hr == S_OK )
	{
		if ( pImage != nullptr && 
			!l_svImageIndex.IsNull() )
		{
			SVImageInfoClass l_svImageInfo = pImage->GetImageInfo();
			SVSmartHandlePointer l_svImageHandle;

			if( pImage->GetImageHandle( l_svImageIndex, l_svImageHandle ) && !( l_svImageHandle.empty() ) )
			{
				SVImageBufferHandleImage l_MilBuffer;
				l_svImageHandle->GetData( l_MilBuffer );

				SVImageInfoClass l_ImageInfo = l_svImageInfo;

				if ( S_OK == SVImageProcessingClass::Instance().CreateImageBuffer( l_ImageInfo, SVaxls.m_ImageHandlePtr ) && !( SVaxls.m_ImageHandlePtr.empty() ) )
				{
					SVImageBufferHandleImage l_AxlsMilBuffer;
					SVaxls.m_ImageHandlePtr->GetData( l_AxlsMilBuffer );

					SVMatroxBufferInterface::SVStatusCode l_Code;
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
		*p_pbstrOverlay = NULL;

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
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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
			if( nullptr != pInputObjectList && pInputObjectList->SetRemoteInput( lIndex, vtValue ) )
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
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_CMDCOMSRV_ERROR, SvOi::Tid_Empty, StdMessageParams );
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

	oTempInfo.SetImageProperty( SVImagePropertyPixelDepth, pbmhInfo->biBitCount );
	oTempInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
	oTempInfo.SetImageProperty( SVImagePropertyBandLink, 0 );

	oTempInfo.SetExtentProperty( SVExtentPropertyHeight, abs(pbmhInfo->biHeight) );
	oTempInfo.SetExtentProperty( SVExtentPropertyWidth, pbmhInfo->biWidth );

	if( pbmhInfo->biBitCount == 24 )
	{
		oTempInfo.SetImageProperty( SVImagePropertyPixelDepth, 8 );
		oTempInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );
	}// end if

	if( S_OK != SVImageProcessingClass::Instance().CreateImageBuffer( oTempInfo, oTempHandle ) || oTempHandle.empty() )
	{
		return l_ImageBuf;
	}

	// Copy the bits into the image object
	if( pBits != NULL && !oTempHandle.empty() )
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
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ))
		{
			CString strFontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlffont.mfo")).c_str();
			CString strControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfcont.mfo")).c_str();
			CString strConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svlfstra.mfo")).c_str();
			CFile oFile;

			SVMatroxOcrInterface::SVStatusCode l_Code;

			if( bstrFontFile != nullptr )
			{
				oFile.Open( strFontFileName, CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontFile, ::SysStringByteLen( bstrFontFile ) );
				oFile.Close();

				if ( !lFontHandle.empty() )
				{
					l_Code = SVMatroxOcrInterface::Destroy( lFontHandle );
				}
				SVMatroxString l_strFontFileName = strFontFileName;

				l_Code = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strFontFileName, SVOcrRestore );

				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontHandle( lFontIdentifier, lFontHandle );

				if ( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}
			}// end if

			if( bstrFontControls != nullptr )
			{
				oFile.Open( strControlsFileName, CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontControls, ::SysStringByteLen( bstrFontControls ) );
				oFile.Close();

				SVMatroxString l_strControlsFileName = strControlsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strControlsFileName, SVOcrLoadControl );
				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}
			}// end if

			if( bstrFontConstraints != nullptr )
			{
				oFile.Open( strConstraintsFileName, CFile::modeCreate | CFile::modeWrite );
				oFile.Write( bstrFontConstraints, ::SysStringByteLen( bstrFontConstraints ) );
				oFile.Close();
				SVMatroxString l_strConstraintsFileName = strConstraintsFileName;
				l_Code = SVMatroxOcrInterface::RestoreFont( lFontHandle, l_strConstraintsFileName, SVOcrLoadConstraint );
				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVLoadFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_46_LOADFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}
			}// end if

			// fill the character's / ID mapping for the font
			double NbChar;
			l_Code = SVMatroxOcrInterface::Get( lFontHandle, SVCharNumberInFont, NbChar );
			SVMatroxString l_strCharacters;
			l_Code = SVMatroxOcrInterface::Get( lFontHandle, SVCharInFont, l_strCharacters );

			CString sTmp(l_strCharacters.c_str());
			TheSVObserverApp.m_mgrRemoteFonts.CreateCharMapping(lFontIdentifier, sTmp);
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
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			SVMatroxString strFontFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsffont.mfo")).c_str();
			SVMatroxString strControlsFileName		= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfcont.mfo")).c_str();
			SVMatroxString strConstraintsFileName	= SvStl::GlobalPath::Inst().GetTempPath(_T("svsfstra.mfo")).c_str();
			CFileStatus rStatus;
			CFile oFile;

			SVMatroxOcrInterface::SVStatusCode l_Code;

			if( *bstrFontFile != nullptr )
			{
				l_Code = SVMatroxOcrInterface::SaveFont( lFontHandle, strFontFileName, SVOcrSave );
				if ( l_Code != SVMEE_STATUS_OK )
				{

					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				CFile::GetStatus( strFontFileName.c_str(), rStatus );
				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);
				*bstrFontFile = ::SysAllocStringByteLen( NULL, strSize );

				oFile.Open( strFontFileName.c_str(), CFile::modeRead );
				oFile.Read( *bstrFontFile, ::SysStringByteLen( *bstrFontFile ) );
				oFile.Close();
			}// end if

			if( *bstrFontControls != nullptr )
			{
				l_Code = SVMatroxOcrInterface::SaveFont( lFontHandle,  strControlsFileName, SVOcrSaveControl );
				if ( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				CFile::GetStatus( strControlsFileName.c_str(), rStatus );

				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);
				*bstrFontControls = ::SysAllocStringByteLen( NULL, strSize );

				oFile.Open( strControlsFileName.c_str(), CFile::modeRead );
				oFile.Read( *bstrFontControls, ::SysStringByteLen( *bstrFontControls ) );
				oFile.Close();
			}// end if

			if( *bstrFontConstraints != nullptr )
			{
				l_Code = SVMatroxOcrInterface::SaveFont( lFontHandle, strConstraintsFileName, SVOcrSaveConstraint );
				if ( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVSaveFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_47_SAVEFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				CFile::GetStatus( strConstraintsFileName.c_str(), rStatus );
				unsigned long strSize = static_cast<unsigned long>(rStatus.m_size);

				*bstrFontConstraints = ::SysAllocStringByteLen( NULL, strSize );

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
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	HRESULT hr = SVSetStringLength( lFontIdentifier, strCalibrate.GetLength() );

	if ( hr == S_OK )
	{
		SVMatroxOcrInterface::SVStatusCode l_Code;
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

				l_Code = SVMatroxOcrInterface::CalibrateFont( lFontHandle, l_CalStruct );
				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVCalibrateFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_45_CALIBRATEFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
					//return some mil error
				}
				l_milImage.clear();

				// BRW - Who could have changed l_Code since the previous if?
				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVCalibrateFont"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_45_CALIBRATEFONT_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
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
};// end SVCalibrateFont

STDMETHODIMP CSVCommand::SVReadString(long lFontIdentifier, BSTR* bstrFoundString, BSTR bstrReadImage, double* dMatchScore )
{
	HRESULT hr = S_OK;
	SVMatroxOcr lFontHandle;
	CString strRead;
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			SVMatroxOcrInterface::SVStatusCode l_Code;
			SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrReadImage );
			SVMatroxOcrResult l_milResult;
			l_Code = SVMatroxOcrInterface::Create( l_milResult );
			// BRW - This l_Code is never checked.

			lFontHandle.m_bVerify = false;
			l_Code = SVMatroxOcrInterface::Execute( l_milResult, lFontHandle, l_milImage );

			if( l_Code != SVMEE_STATUS_OK )
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SVStringArray msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
			}

			// Process the OCR chars returned from MocrReadString();
			long l_lLength = 0L;

			l_Code = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrResultStringSize, l_lLength );

			if( l_lLength != 0 )
			{
				l_Code = SVMatroxOcrInterface::GetResult(l_milResult, SVOcrStringScore, *dMatchScore );
				// BRW - This l_Code is never checked.
				SVMatroxString l_strRead;
				l_Code = SVMatroxOcrInterface::GetResult(l_milResult, SVOcrString, l_strRead );
				strRead = l_strRead.c_str();
				*bstrFoundString = strRead.AllocSysString();
			}// end if
			else
			{
				*dMatchScore = 0.0F;
				*bstrFoundString = strRead.AllocSysString();
			}// end else

			l_milImage.clear();
			if( l_Code != SVMEE_STATUS_OK )
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SVStringArray msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
			}

			l_Code = SVMatroxOcrInterface::Destroy( l_milResult );
			if( l_Code != SVMEE_STATUS_OK )
			{
				SVMatroxStatusInformation l_info;
				SVMatroxApplicationInterface::GetLastStatus( l_info );
				SVStringArray msgList;
				msgList.push_back(_T("SVReadString"));
				msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
				msgList.push_back(l_info.m_StatusString);
				hr = SVMSG_48_READSTRING_ERROR;
				Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
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
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	HRESULT hr = SVSetStringLength( lFontIdentifier, strVerify.GetLength() );

	if ( hr == S_OK )
	{
		SVMatroxOcr lFontHandle;

		if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
		{
			if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
				TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
			{
				SVMatroxOcrInterface::SVStatusCode l_Code;
				SVMatroxBuffer l_milImage = CreateImageFromBSTR( bstrVerifyImage );
				SVMatroxOcrResult l_milResult;
				l_Code = SVMatroxOcrInterface::Create( l_milResult );

				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				lFontHandle.m_bVerify = true;
				lFontHandle.m_VerifyString = strVerify;

				SVMatroxOcrInterface::Execute( l_milResult, lFontHandle, l_milImage );

				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				// Process the OCV chars returned from MocrVerifyString();
				long l_lLength = 0L;

				l_Code = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrResultStringSize, l_lLength);

				if( l_lLength != 0L )
				{
					l_Code = SVMatroxOcrInterface::GetResult( l_milResult, SVOcrStringScore, *dMatchScore );
				}// end if
				else
				{
					*dMatchScore = 0.0F;
				}// end else

				l_milImage.clear();

				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
				}

				l_Code = SVMatroxOcrInterface::Destroy( l_milResult );

				if( l_Code != SVMEE_STATUS_OK )
				{
					SVMatroxStatusInformation l_info;
					SVMatroxApplicationInterface::GetLastStatus( l_info );
					SVStringArray msgList;
					msgList.push_back(_T("SVVerifyString"));
					msgList.push_back(SvUl_SF::Format(_T("%d"), l_Code));
					msgList.push_back(l_info.m_StatusString);
					hr = SVMSG_49_VERIFYSTRING_ERROR;
					Exception.setMessage( hr, SvOi::Tid_ErrorMIL, msgList, StdMessageParams );
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

	CString sTmp(bstrLabel);
	char cTmp = sTmp.GetAt(0);

	if (TheSVOLicenseManager().HasMatroxIdentificationLicense())
	{
		if( TheSVObserverApp.m_mgrRemoteFonts.IsValidFont( lFontIdentifier, lFontHandle ) &&
			TheSVObserverApp.m_mgrRemoteFonts.UpdateFontTime( lFontIdentifier ) )
		{
			SVMatroxBuffer milFontImage;
			if ( TheSVObserverApp.m_mgrRemoteFonts.GetFontImage(lFontIdentifier, milFontImage) )
			{
				SVMatroxOcrInterface::SVStatusCode l_Code;

				long lOcrWidth;
				long lOcrHeight;

				l_Code = SVMatroxOcrInterface::Get(lFontHandle, SVOcrCharSizeX, lOcrWidth );
				KeepPrevError( l_Code,SVMatroxOcrInterface::Get(lFontHandle, SVOcrCharSizeY, lOcrHeight ));

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
							SVMatroxString Str;
							Str = cTmp;

							KeepPrevError( l_Code, SVMatroxOcrInterface::CopyFont(milNewFontID, milTmpID, SVOcrCopytoFont, Str));
							if ( l_Code == SVMEE_STATUS_OK )
							{
								TheSVObserverApp.m_mgrRemoteFonts.UpdateFontHandle( lFontIdentifier,milNewFontID);
								TheSVObserverApp.m_mgrRemoteFonts.AddFontChar( lFontIdentifier, Str[0], &lId);

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
	SVMatroxBufferInterface::SVStatusCode l_Code;

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
				ImageInfo.SetImageProperty(SVImagePropertyBandNumber, l_lValue );

				l_Code = SVMatroxBufferInterface::Get(lCharHandle, SVType, l_lValue );
				ImageInfo.SetImageProperty(SVImagePropertyPixelDepth, l_lValue );

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
			SVMatroxString Characters;
			SVMatroxOcrInterface::Get( lFontHandle, SVCharInFont, Characters );

			CString sTmp(Characters.c_str());

			*bstrCharacterList = sTmp.AllocSysString();

			SAFEARRAYBOUND saBounds[1];
			saBounds[0].lLbound=0;
			saBounds[0].cElements = sTmp.GetLength();

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
			SVMatroxOcrInterface::SVStatusCode l_Code;

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
		if( pStreamData != NULL )
		{
			if ( !( pStreamData->m_InspectionID.empty() ) )
			{
				SVObjectNameInfo l_NameInfo;

				SVObjectClass* l_pInspection = SVObjectManagerClass::Instance().GetObject( pStreamData->m_InspectionID );

				hr = l_NameInfo.ParseObjectName( static_cast< LPCTSTR >( pStreamData->strValueName ) );

				if( hr == S_OK && l_pInspection != NULL && !( l_NameInfo.m_NameArray.empty() ) && l_NameInfo.m_NameArray[ 0 ] == l_pInspection->GetName() )
				{
					pTempObject = reinterpret_cast <SVObjectClass*> (::SVSendMessage( SVObjectManagerClass::Instance().GetObject( pStreamData->m_InspectionID ),
						( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD | SVM_NOTIFY_FRIENDS ) & ~SVM_NOTIFY_ONLY_THIS, 
						reinterpret_cast <DWORD_PTR> (static_cast<LPCSTR>(l_NameInfo.GetObjectName().c_str())), NULL ) );

					pStreamData->pValueObject = dynamic_cast <SVValueObjectClass*> ( pTempObject );

					// Validate Array Objects
					if (pStreamData->arrayIndex > 0 && pStreamData->pValueObject)
					{
						// Ensure valueObject is an Array
						if (pStreamData->pValueObject->IsArray())
						{
							// Ensure index is within bounds
							if (pStreamData->arrayIndex >= pStreamData->pValueObject->GetArraySize()) 
							{
								pStreamData->pValueObject = NULL;
							}
						}
						else
						{
							pStreamData->pValueObject = NULL;
						}
					}
				}
				else
				{
					pStreamData->pValueObject = NULL;
				}
			}
			else
			{
				pStreamData->pValueObject = NULL;
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
		m_hStopStreamEvent = NULL;
	}// end if

	if ( m_hStreamingThread )
	{
		::CloseHandle( m_hStreamingThread );
		m_hStreamingThread = NULL;
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

// Conditional History functions

namespace local
{
	HRESULT SafeArrayToScalarValueVector( SAFEARRAY* psaNames, SAFEARRAY* psaValues, SVScalarValueVector& rvec )
	{
		HRESULT hr = S_FALSE;
		long lNumberOfElements = psaNames->rgsabound[0].cElements;
		ASSERT( psaValues == NULL || lNumberOfElements == psaValues->rgsabound[0].cElements );
		if ( psaValues == NULL || lNumberOfElements == psaValues->rgsabound[0].cElements )
		{
			for ( long l = 0; l < lNumberOfElements; ++l )
			{
				BSTR bstrName=NULL;
				BSTR bstrValue=NULL;
				CSVCommand::SafeArrayGetElementNoCopy( psaNames,  &l, &bstrName  );
				if ( psaValues != NULL )
				{
					CSVCommand::SafeArrayGetElementNoCopy( psaValues, &l, &bstrValue );
				}

				rvec.push_back( SVScalarValue( CString(bstrName), CString(bstrValue) ) );
			}
			hr = S_OK;
		}
		return hr;
	}// end SafeArrayToScalarValueVector

	HRESULT ScalarValueVectorToSafeArray( SVScalarValueVector& rvec, SAFEARRAY** ppsaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus )
	{
		HRESULT hr = S_FALSE;

		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = static_cast< ULONG >( rvec.size() );

		if ( ppsaNames )
		{
			*ppsaNames  = ::SafeArrayCreate( VT_BSTR, 1, saBounds);
		}

		if ( ppsaValues )
		{
			*ppsaValues = ::SafeArrayCreate( VT_BSTR, 1, saBounds);
		}

		if ( ppsaStatus )
		{
			*ppsaStatus = ::SafeArrayCreate( VT_I4,   1, saBounds);
		}

		for ( long l = 0; l < static_cast<long>(rvec.size()); ++l )
		{
			SVScalarValue& rValue = rvec.at(l);
			if ( ppsaNames )
			{
				BSTR bstrName  = rValue.strName.AllocSysString();
				CSVCommand::SafeArrayPutElementNoCopy( *ppsaNames,  &l, bstrName );
			}

			if ( ppsaValues )
			{
				BSTR bstrValue = rValue.strValue.AllocSysString();
				CSVCommand::SafeArrayPutElementNoCopy( *ppsaValues, &l, bstrValue );
			}

			if ( ppsaStatus )
			{
				long lStatus = rValue.status;
				::SafeArrayPutElement( *ppsaStatus, &l, (void*) &lStatus );
			}
		}
		hr = S_OK;
		return hr;
	}

	HRESULT VectorLongToSafeArray( std::vector<long>& rvec, SAFEARRAY** ppsaProcessCount )
	{
		HRESULT hr = S_FALSE;

		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = static_cast< ULONG >( rvec.size() );

		if ( ppsaProcessCount )
		{
			*ppsaProcessCount  = ::SafeArrayCreate( VT_I4, 1, saBounds);
		}

		for ( long l = 0; l < static_cast<long>(rvec.size()); ++l )
		{
			if ( ppsaProcessCount )
			{
				long lProcessCount = rvec.at(l);
				SafeArrayPutElement( *ppsaProcessCount, &l, &lProcessCount );
			}
		}
		hr = S_OK;
		return hr;
	}

	HRESULT ScalarValueVectorToSafeArray( std::vector < SVScalarValueVector >& rvec, SAFEARRAY** ppsaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus )
	{
		HRESULT hr = S_FALSE;

		long lNumEntries = static_cast< long >( rvec.size() );
		long lNumValues = lNumEntries > 0 ? static_cast< long >( rvec.at(0).size() ) : 0;	// all entries have same num values

		SAFEARRAYBOUND saBounds[2];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = lNumValues;
		saBounds[1].lLbound=0;
		saBounds[1].cElements = lNumEntries;

		if ( ppsaNames )
		{
			*ppsaNames  = ::SafeArrayCreate( VT_BSTR, 2, saBounds);
		}

		if ( ppsaValues )
		{
			*ppsaValues = ::SafeArrayCreate( VT_BSTR, 2, saBounds);
		}

		if ( ppsaStatus )
		{
			*ppsaStatus = ::SafeArrayCreate( VT_I4,   2, saBounds);
		}

		long alDimIndex[2]={0};
		for ( long lEntry = 0; lEntry < lNumEntries; ++lEntry )
		{
			alDimIndex[1] = lEntry;
			SVScalarValueVector& rvecValues = rvec.at(lEntry);
			ASSERT( rvecValues.size() == lNumValues );

			for ( long lIndex = 0; lIndex < static_cast<long>(rvecValues.size()); ++lIndex )
			{
				SVScalarValue& rValue = rvecValues.at(lIndex);
				alDimIndex[0] = lIndex;

				if ( ppsaNames )
				{
					BSTR bstrName  = rValue.strName.AllocSysString();
					CSVCommand::SafeArrayPutElementNoCopy( *ppsaNames, alDimIndex, bstrName );
				}

				if ( ppsaValues )
				{
					BSTR bstrValue = rValue.strValue.AllocSysString();
					CSVCommand::SafeArrayPutElementNoCopy( *ppsaValues, alDimIndex, bstrValue );
				}

				if ( ppsaStatus )
				{
					long lStatus = rValue.status;
					::SafeArrayPutElement( *ppsaStatus, alDimIndex, &lStatus );
				}
			}// end for ( long lIndex = 0; lIndex < rvecValues.size(); ++lIndex )
		}// end for ( long lEntry = 0; lEntry < lNumEntries; ++lEntry )
		hr = S_OK;
		return hr;
	}

	HRESULT ImageBufferVectorToSafeArray( std::vector<SVImageBufferStruct>& rvec, long lCompression, SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus )
	{
		HRESULT hr = S_FALSE;

		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = static_cast< ULONG >( rvec.size() );

		if ( ppsaImageNames )
		{
			*ppsaImageNames  = ::SafeArrayCreate( VT_BSTR, 1, saBounds);
		}

		if ( ppsaImages )
		{
			*ppsaImages = ::SafeArrayCreate( VT_BSTR, 1, saBounds);
		}

		if ( ppsaOverlays )
		{
			*ppsaOverlays = ::SafeArrayCreate( VT_BSTR, 1, saBounds);
		}

		if ( ppsaStatus )
		{
			*ppsaStatus = ::SafeArrayCreate( VT_I4,   1, saBounds);
		}

		for ( long l = 0; l < static_cast<long>(rvec.size()); ++l )
		{
			SVImageBufferStruct& rImage = rvec.at(l);

			if ( ppsaImageNames )
			{
				BSTR bstrName  = rImage.strName.AllocSysString();
				CSVCommand::SafeArrayPutElementNoCopy( *ppsaImageNames, &l, bstrName );
			}

			if ( ppsaImages )
			{
				BSTR bstrImage = nullptr;
				CSVCommand::ImageToBSTR( rImage.info, rImage.handle, &bstrImage);
				CSVCommand::SafeArrayPutElementNoCopy( *ppsaImages, &l, bstrImage );
			}

			if ( ppsaOverlays )
			{
				BYTE* pBytes = nullptr;
				long lSize = 0;
				rImage.overlay.GetBufferSize( lSize );
				rImage.overlay.GetBuffer( pBytes );

				BSTR bstrOverlay = ::SysAllocStringByteLen( nullptr, lSize );
				memcpy( bstrOverlay, pBytes, lSize );

				CSVCommand::SafeArrayPutElementNoCopy( *ppsaOverlays, &l, bstrOverlay );
			}

			if ( ppsaStatus )
			{
				long lStatus = 0;
				::SafeArrayPutElement( *ppsaStatus, &l, (void*) &lStatus );
			}
		}
		hr = S_OK;
		return hr;
	}// end ImageBufferVectorToSafeArray ( std::vector<SVImageBufferStruct>& rvec )

	HRESULT ImageBufferVectorToSafeArray( std::vector < std::vector<SVImageBufferStruct> >& rvec, long lCompression, SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus )
	{
		HRESULT hr = S_FALSE;

		long lNumEntries = static_cast< long >( rvec.size() );
		long lNumValues = lNumEntries > 0 ? static_cast< long >( rvec.at( 0 ).size() ) : 0;	// all entries have same num values

		SAFEARRAYBOUND saBounds[2];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = lNumValues;
		saBounds[1].lLbound=0;
		saBounds[1].cElements = lNumEntries;

		if ( ppsaImageNames )
		{
			*ppsaImageNames  = ::SafeArrayCreate( VT_BSTR, 2, saBounds);
		}

		if ( ppsaImages )
		{
			*ppsaImages = ::SafeArrayCreate( VT_BSTR, 2, saBounds);
		}

		if ( ppsaOverlays )
		{
			*ppsaOverlays = ::SafeArrayCreate( VT_BSTR, 2, saBounds);
		}

		if ( ppsaStatus )
		{
			*ppsaStatus = ::SafeArrayCreate( VT_I4,   2, saBounds);
		}

		long alDimIndex[2]={0};
		for ( long lEntry = 0; lEntry < lNumEntries; ++lEntry )
		{
			alDimIndex[1] = lEntry;
			std::vector<SVImageBufferStruct>& rvecImages = rvec.at(lEntry);
			ASSERT( rvecImages.size() == lNumValues );

			for ( long lIndex = 0; lIndex < static_cast<long>(rvecImages.size()); ++lIndex )
			{
				SVImageBufferStruct& rImage = rvecImages.at(lIndex);
				alDimIndex[0] = lIndex;

				if ( ppsaImageNames )
				{
					BSTR bstrName  = rImage.strName.AllocSysString();
					CSVCommand::SafeArrayPutElementNoCopy( *ppsaImageNames, alDimIndex, bstrName );
				}

				if ( ppsaImages )
				{
					BSTR bstrImage = nullptr;
					CSVCommand::ImageToBSTR( rImage.info, rImage.handle, &bstrImage);
					CSVCommand::SafeArrayPutElementNoCopy( *ppsaImages, alDimIndex, bstrImage );
				}

				if ( ppsaOverlays )
				{
					BYTE* pBytes = nullptr;
					long lSize = 0;
					rImage.overlay.GetBufferSize( lSize );
					rImage.overlay.GetBuffer( pBytes );

					BSTR bstrOverlay = ::SysAllocStringByteLen( nullptr, lSize );
					memcpy( bstrOverlay, pBytes, lSize );

					CSVCommand::SafeArrayPutElementNoCopy( *ppsaOverlays, alDimIndex, bstrOverlay );
				}

				if ( ppsaStatus )
				{
					//long lStatus = rImage.status;
					long lStatus = 0;
					::SafeArrayPutElement( *ppsaStatus, alDimIndex, &lStatus );
				}
			}// end for ( long lIndex = 0; lIndex < rvecValues.size(); ++lIndex )
		}// end for ( long lEntry = 0; lEntry < lNumEntries; ++lEntry )
		hr = S_OK;
		return hr;
	}// end ImageBufferVectorToSafeArray ( std::vector < std::vector<SVImageBufferStruct> >& rvec )

	HRESULT SetStatus( SVScalarValueVector& rvec, SAFEARRAY* psaStatus )
	{
		HRESULT hr = S_FALSE;
		long lNumberOfElements = psaStatus->rgsabound[0].cElements;
		ASSERT( lNumberOfElements == rvec.size() );
		if ( lNumberOfElements == rvec.size() )
		{
			for ( long l = 0; l < lNumberOfElements; ++l )
			{
				HRESULT hrTemp = ::SafeArrayPutElement( psaStatus, &l, &(rvec.at(l).status) );
			}
			hr = S_OK;
		}
		return hr;
	}// end SetStatus
}// end namespace local

STDMETHODIMP CSVCommand::SVSetConditionalHistoryProperties(BSTR bstrInspectionName, SAFEARRAY*  psaNames,  SAFEARRAY*  psaValues,  SAFEARRAY** ppsaStatus)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		SVScalarValueVector vecProperties;
		hr = local::SafeArrayToScalarValueVector( psaNames, psaValues, vecProperties );
		if ( hr == S_OK )
		{
			hr = pInspection->SetConditionalHistoryProperties( vecProperties );

			if ( hr == S_OK )
			{
				local::SetStatus( vecProperties, *ppsaStatus );
			}
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}

	return hr;
}

STDMETHODIMP CSVCommand::SVGetConditionalHistoryProperties(BSTR bstrInspectionName, SAFEARRAY** ppsaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		SVScalarValueVector vecProperties;

		hr = pInspection->GetConditionalHistoryProperties( vecProperties );

		if ( hr == S_OK )
		{
			HRESULT hrTemp = local::ScalarValueVectorToSafeArray( vecProperties, ppsaNames, ppsaValues, ppsaStatus );
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP CSVCommand::SVSetConditionalHistoryList(BSTR bstrInspectionName, SAFEARRAY*  psaValueNames,  SAFEARRAY*  psaImageNames,  SAFEARRAY*  psaConditionalNames,  SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus)
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		SVScalarValueVector vecValues, vecImages, vecConditionals;

		// These params are optional, so pass NULL in SetConditionalHistoryList if not supplied
		SVScalarValueVector* pvecValues = NULL;
		SVScalarValueVector* pvecImages = NULL;
		SVScalarValueVector* pvecConditionals = NULL;

		if ( psaValueNames )
		{
			pvecValues = &vecValues;
			hr = local::SafeArrayToScalarValueVector( psaValueNames, NULL, vecValues );
		}

		if ( psaImageNames )
		{
			pvecImages = &vecImages;
			hr = local::SafeArrayToScalarValueVector( psaImageNames, NULL, vecImages );
		}

		if ( psaConditionalNames )
		{
			pvecConditionals = &vecConditionals;
			hr = local::SafeArrayToScalarValueVector( psaConditionalNames, NULL, vecConditionals );
		}

		hr = pInspection->SetConditionalHistoryList( pvecValues, pvecImages, pvecConditionals );

		if ( hr == S_OK )
		{
			if ( ppsaValueStatus )
			{
				local::SetStatus( vecValues, *ppsaValueStatus );
			}

			if ( ppsaImageStatus )
			{
				local::SetStatus( vecImages, *ppsaImageStatus );
			}

			if ( ppsaConditionalStatus )
			{
				local::SetStatus( vecConditionals, *ppsaConditionalStatus );
			}
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP CSVCommand::SVGetConditionalHistoryList( BSTR bstrInspectionName, SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus )
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		SVScalarValueVector vecValues, vecImages, vecConditionals;

		hr = pInspection->GetConditionalHistoryList( vecValues, vecImages, vecConditionals );

		if ( hr == S_OK )
		{
			HRESULT hrTemp = local::ScalarValueVectorToSafeArray( vecValues, ppsaValueNames, NULL, ppsaValueStatus );
			hrTemp = local::ScalarValueVectorToSafeArray( vecImages, ppsaImageNames, NULL, ppsaImageStatus );
			hrTemp = local::ScalarValueVectorToSafeArray( vecConditionals, ppsaConditionalNames, NULL, ppsaConditionalStatus );
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP CSVCommand::SVGetConditionalHistoryAndClear(
	BSTR bstrInspectionName, long lCompression,
	SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
	SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
	SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
	SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
	SAFEARRAY** ppsaProcessCount )
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		// check online status
		bool bOnline = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );
		if ( bOnline )
		{
			std::vector<SVScalarValueVector> vecValues, vecConditionals;
			std::vector<SVImageBufferStructVectorType> vecImages;
			std::vector<long> vecProcessCount;

			hr = pInspection->GetConditionalHistoryAndClear( vecValues, vecImages, vecConditionals, vecProcessCount );

			if ( hr == S_OK )
			{
				HRESULT hrTemp;
				hrTemp = local::VectorLongToSafeArray( vecProcessCount, ppsaProcessCount );
				hrTemp = local::ScalarValueVectorToSafeArray( vecValues, ppsaValueNames, ppsaValues, ppsaValueStatus );
				hrTemp = local::ScalarValueVectorToSafeArray( vecConditionals, ppsaConditionalNames, ppsaConditionalValues, ppsaConditionalStatus );
				hrTemp = local::ImageBufferVectorToSafeArray( vecImages, lCompression, ppsaImageNames, ppsaImages, ppsaOverlays, ppsaImageStatus );
			}

			// clean up image buffers
			for ( int i=0; i < static_cast<int>(vecImages.size()); ++i )
			{
				SVImageBufferStructVectorType& rvec = vecImages.at(i);
				for ( int j = 0; j < static_cast<int>(rvec.size()); ++j )
				{
					SVImageBufferStruct& rImage = rvec.at(j);
					rImage.DestroyBuffer();
				}
			}
		}// end if online
		else
		{
			hr = SVMSG_53_SVIM_NOT_IN_RUN_MODE;
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

STDMETHODIMP CSVCommand::SVGetMostRecentConditionalHistory(
	BSTR bstrInspectionName, long lCompression,
	SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
	SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
	SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
	SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
	long* plProcessCount )
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	SVInspectionProcess* pInspection( nullptr );

	if ( SVConfigurationObject::GetInspection( W2T(bstrInspectionName), pInspection ) )
	{
		// check online status
		bool bOnline = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );
		if ( bOnline )
		{
			SVScalarValueVector vecValues, vecConditionals;
			SVImageBufferStructVectorType vecImages;
			long lProcessCount = -1;

			hr = pInspection->GetMostRecentConditionalHistory( vecValues, vecImages, vecConditionals, lProcessCount );

			if ( hr == S_OK )
			{
				*plProcessCount = lProcessCount;
				HRESULT hrTemp = local::ScalarValueVectorToSafeArray( vecValues, ppsaValueNames, ppsaValues, ppsaValueStatus );
				hrTemp = local::ScalarValueVectorToSafeArray( vecConditionals, ppsaConditionalNames, ppsaConditionalValues, ppsaConditionalStatus );
				hrTemp = local::ImageBufferVectorToSafeArray( vecImages, lCompression, ppsaImageNames, ppsaImages, ppsaOverlays, ppsaImageStatus );
			}

			// clean up image buffers
			for ( int i=0; i < static_cast<int>(vecImages.size()); ++i )
			{
				SVImageBufferStruct& rImage = vecImages.at(i);
				rImage.DestroyBuffer();
			}
		}// end if online
		else
		{
			hr = SVMSG_53_SVIM_NOT_IN_RUN_MODE;
		}
	}
	else
	{
		hr = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
	}
	return hr;
}

// Stub for SVGetTransferValueDefinitionList
STDMETHODIMP CSVCommand::SVGetTransferValueDefinitionList(BSTR bstrInspectionName, 
	long* p_plVersion,
	VARIANT* p_pvData )
{
	USES_CONVERSION;

	if( p_pvData == NULL )
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

		std::vector<SVValueObjectClass*> l_pSelectedObjects;

		SVOutputInfoListClass l_OutputList;

		pTaskObjectList->GetOutputList( l_OutputList );

		size_t nCount = l_OutputList.GetSize();

		for( int i = 0 ; i < static_cast< int >( nCount ); i++ )
		{
			// Get OutObjectInfoStruct...
			SVOutObjectInfoStruct* pInfoItem = NULL;

			pInfoItem = l_OutputList.GetAt(i);

			SVObjectReference object;
			if( pInfoItem )
			{
				object = pInfoItem->GetObjectReference();
			}
			else
			{
				ASSERT(0);
				break;
			}

			SVObjectClass* pObject = object.Object();

			if( pObject )
			{
				if( pObject->ObjectAttributesSet() & SV_DD_VALUE ) // if Data Definition List set.
				{
					SVValueObjectClass* l_pValueObject = dynamic_cast<SVValueObjectClass*>(pObject);
					if( l_pValueObject != NULL )
					{
						l_pSelectedObjects.push_back( l_pValueObject );
					}
				}
			}
		}

		// Copy list to Safearray

		// Get Attributes and add to lists.
		// 1st Version of Data Transfer Definitions....January 2008..TRB

		SAFEARRAYBOUND l_saBounds[2];

		// First Dimension number of objects in list..
		l_saBounds[0].cElements = static_cast< ULONG >( l_pSelectedObjects.size() );
		l_saBounds[0].lLbound = 0;

		// Second Dimension is the parts fo the Transfer Definition
		// 0 = Name, 1 = Writable, 2 = Data Type, 3 = EnumList
		l_saBounds[1].cElements = 4;
		l_saBounds[1].lLbound = 0;

		SAFEARRAY* l_psaData;	//( VT_VARIANT, l_saBounds);
		l_psaData = ::SafeArrayCreate( VT_VARIANT, 2, &l_saBounds[0] );
		long  l_Index[2];
		for( size_t i = 0 ; i < l_pSelectedObjects.size() ; i++ )
		{
			l_Index[0] = static_cast< long >( i );
			// Name
			l_Index[1] = 0;
			_variant_t l_vTmp;
			l_vTmp = l_pSelectedObjects[i]->GetCompleteObjectName();
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp);

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (l_pSelectedObjects[i]->ObjectAttributesAllowed() & SV_REMOTELY_SETABLE) == SV_REMOTELY_SETABLE;
			l_vTmp.Clear();
			l_vTmp.ChangeType(VT_BOOL);
			l_vTmp = l_bWritable;
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp );

			// Data Type
			l_Index[1] = 2;
			l_vTmp.Clear();
			l_vTmp.ChangeType(VT_BSTR);
			CString l_strTmp;
			l_pSelectedObjects[i]->GetTypeName(l_strTmp);
			l_vTmp = l_strTmp;
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp );

			// Enumeration List.
			l_Index[1] = 3;
			l_vTmp.Clear();
			if( l_pSelectedObjects[i]->GetObjectType() == SVEnumValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVEnumerateValueObjectClass* l_pEnumVO = dynamic_cast<SVEnumerateValueObjectClass*>(l_pSelectedObjects[i]);
				if( l_pEnumVO != NULL )
				{
					SVEnumerateVector l_enumVect;
					l_pEnumVO->GetEnumTypes( l_enumVect );
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast< ULONG >( l_enumVect.size() );
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long i = 0; i < static_cast<long>(l_enumVect.size()) ; i++ )
					{
						BSTR bstTmp = l_enumVect[i].first.AllocSysString();
						SafeArrayPutElement(l_psaTemp, &i, bstTmp );
						::SysFreeString( bstTmp );
					}
					// Put the Safearray in the Variant.
					l_vTmp.vt = VT_ARRAY | VT_BSTR;
					l_vTmp.parray = l_psaTemp;
				}
			}
			else if( l_pSelectedObjects[i]->GetObjectType() == SVBoolValueObjectType)
			{
				// Get the strings from the enumeration value object class.
				SVBoolValueObjectClass* l_pBoolVO = dynamic_cast<SVBoolValueObjectClass*>(l_pSelectedObjects[i]);
				if( l_pBoolVO != NULL )
				{
					std::vector<CString> l_StringVect;
					l_pBoolVO->GetValidTypes( l_StringVect );
					SAFEARRAYBOUND l_rgsabound[1];
					l_rgsabound[0].cElements = static_cast< ULONG >( l_StringVect.size() );
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long i = 0; i < static_cast<long>(l_StringVect.size()) ; i++ )
					{
						BSTR bstTmp = l_StringVect[i].AllocSysString();
						SafeArrayPutElement(l_psaTemp, &i, bstTmp );
						::SysFreeString( bstTmp );
					}
					// Put the Safearray in the Variant.
					l_vTmp.vt = VT_ARRAY | VT_BSTR;
					l_vTmp.parray = l_psaTemp;

				}
			}
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp);
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if( p_pvData != NULL )
		{
			hr = VariantClear( p_pvData );
		}
		_variant_t l_vTemp;
		l_vTemp.vt = VT_ARRAY | VT_VARIANT ;
		l_vTemp.parray = l_psaData ;
		*p_pvData = l_vTemp.Detach();
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

	if( p_pvData == NULL )
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
				if ( (pImage->ObjectAttributesSet() & SV_DD_IMAGE) != 0 )
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
			_variant_t l_vTmp;
			l_vTmp = objectList[i]->GetCompleteObjectName();
			//l_saData.PutElement( l_Index, l_vTmp );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp ); 

			// Writable
			l_Index[1] = 1;
			bool l_bWritable = (objectList[i]->ObjectAttributesAllowed() & SV_REMOTELY_SETABLE) == SV_REMOTELY_SETABLE;
			l_vTmp.Clear();
			l_vTmp.ChangeType(VT_BOOL);
			l_vTmp = l_bWritable;
			//l_saData.PutElement( l_Index, l_vTmp );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp );

			// Published Image
			l_Index[1] = 2;
			l_vTmp.Clear();
			l_vTmp.ChangeType(VT_BOOL);
			l_vTmp = (objectList[i]->ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE) != 0;
			//l_saData.PutElement( l_Index, l_vTmp );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp );

			// Fully Qualified Source Image Name
			CString l_strNames;
			SVToolClass* l_pTool = objectList[i]->GetTool();
			l_Index[1] = 3;
			l_vTmp.Clear();
			if( l_pTool )
			{
				SVStaticStringValueObjectClass* l_psvSourceNames = l_pTool->GetInputImageNames();
				if( l_psvSourceNames )
				{
					SAFEARRAYBOUND l_rgsabound[1];
					long l_lSize = l_psvSourceNames->GetArraySize();
					l_rgsabound[0].cElements = l_lSize;
					l_rgsabound[0].lLbound = 0;
					SAFEARRAY *l_psaTemp = SafeArrayCreate( VT_BSTR, 1, l_rgsabound );
					for( long l_lIndex = 0; l_lIndex < l_lSize ; l_lIndex++ )
					{
						CString l_strTmp;
						HRESULT l_hr = l_psvSourceNames->GetValue( l_psvSourceNames->GetLastSetIndex(),
							l_lIndex,
							l_strTmp );
						BSTR bstTmp = l_strTmp.AllocSysString();
						SafeArrayPutElement(l_psaTemp, &l_lIndex, bstTmp );
						::SysFreeString( bstTmp );
					}
					// Put the Safearray in the Variant.
					l_vTmp.vt = VT_ARRAY | VT_BSTR;
					l_vTmp.parray = l_psaTemp;
				}
			}
			//l_saData.PutElement( l_Index, l_vTmp );
			hr = ::SafeArrayPutElement( l_psaData, l_Index, &l_vTmp );
		} // end for( int i

		// stuff SVSAFEARRAY into the safearray pointer
		if( p_pvData != NULL )
		{
			hr = VariantClear( p_pvData );
		}
		_variant_t l_vTemp;
		l_vTemp.vt = VT_ARRAY | VT_VARIANT ;
		l_vTemp.parray = l_psaData ;
		*p_pvData = l_vTemp.Detach();
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

	if( p_ppCommand != NULL )
	{
		if( *p_ppCommand != NULL )
		{
			( *p_ppCommand )->Release();

			*p_ppCommand = NULL;
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

			if( l_RemoteCommandPtr.p == NULL )
			{
				if( l_Status == S_OK )
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
			{
				// This has issues when using _ATL_DEBUG_INTERFACES...
				SVRemoteCommand* l_pRemoteCommand = dynamic_cast< SVRemoteCommand* >( l_RemoteCommandPtr.p );

				if( l_pRemoteCommand != NULL )
				{
					l_pRemoteCommand->SetCommand( l_CommandPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
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

