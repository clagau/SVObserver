///*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jane Doe
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "GlobalPath.h"
#include "CommandLineArgs.h"
//Moved to precompiled header: #include <limits.h>
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


namespace SvStl
{
	///defaultValue 
	static const TCHAR* const DefaultLastValidFolder =   _T( "C:\\Last Valid" );	
	static const TCHAR* const DefaultTempFolder = _T( "C:\\Temp" );
	static const TCHAR* const DefaultRunFolder = _T( "C:\\RUN" );
	static const TCHAR* const DefaultObserverBinPath = _T( "C:\\SVObserver\\bin" );
	static const TCHAR* const DefaultExternalToolPath = _T( "C:\\SVObserver\\ExternalTool" );	
	static const TCHAR* const DefaultObserverPath = _T( "C:\\SVObserver" );	
	static const TCHAR* const DefaultSecondObserverPath = _T( "D:\\SVObserver" );	
	static const TCHAR* const DefaultRamDrive = _T("V:");

	///subdirectories and sub-subdirectories of DefaultSecondObserverPath 
	static const TCHAR* const AutoSaveRelativePath = _T( "Autosave" );	
	const TCHAR* const AutoSaveTempRelativePath = _T("Temp");///< the "temporary" autosave temp directory name
	const TCHAR* const AutoSaveTemp1FileName = _T("Temp1MostRecent.svz");///< the first autosave temp file name
	const TCHAR* const AutoSaveTemp2FileName = _T("Temp2.svz");///< the second autosave temp file name
	const TCHAR* const AutoSaveTemp3FileName = _T("Temp3.svz");///< the third autosave temp file name

	///TAGS 
	static const TCHAR* const SVIMDirectorySectionTag = _T("SVIM Directories");
	static const TCHAR* const SVIMObserverBinPathTag = _T("ObserverBinPath");
	static const TCHAR* const SVIMObserverExternalToolPathTag = _T("ObserverExternalToolPath");
	static const TCHAR* const SVIMSVObserverRunPathTag = _T("ObserverRunPath");
	static const TCHAR* const SVIMSVObserverTempPath = _T("ObserverTempPath");
	static const TCHAR* const SVIMSVObserverPath = _T("ObserverPath");
	static const TCHAR* const SVIMSVObserverSecondPath = _T("ObserverSecondPath");
	static const TCHAR* const SVIMSVObserverLastValidPath = _T("LastValidPath");
	static const TCHAR* const SVIMRamDrive = _T("RamDrive");


	GlobalPath::GlobalPath():
		m_IsInitializedIni{false},
		m_IsInitilizedPath{false},
		m_SVObserverBinFolder{DefaultObserverBinPath},
		m_SVObserverRunFolder{DefaultRunFolder},
		m_TempFolder{DefaultTempFolder},
		m_SVObserverExternalToolFolder{DefaultExternalToolPath},
		m_AutosaveTempDirectoryName{AutoSaveTempRelativePath},
		m_AutosaveTemp1FileName{AutoSaveTemp1FileName},
		m_AutosaveTemp2FileName{AutoSaveTemp2FileName},
		m_AutosaveTemp3FileName{AutoSaveTemp3FileName}
	{
	}

	GlobalPath& GlobalPath::Inst()
	{
		static GlobalPath Instance;
		return Instance;
	}

	void GlobalPath::InitializeIniFolder()
	{
		if(false == m_IsInitializedIni)
		{
			m_IniFolder = 	CommandLineArgs::Inst().GetIniDirectory(); 
			m_SVIM_ini = m_IniFolder;
			m_SVIM_ini.append(_T("\\SVIM.ini"));
			m_Hardware_ini = m_IniFolder;
			m_Hardware_ini.append(_T("\\Hardware.ini"));
			m_Utility_ini = m_IniFolder;
			m_Utility_ini.append(_T("\\SVUtility.ini"));
			m_SVOGateway_ini = m_IniFolder;
			m_SVOGateway_ini.append(_T("\\SVOGateway.ini"));

			m_IsInitializedIni = true;
		}
	}

	LPCTSTR GlobalPath::GetIniFolder()
	{
		InitializeIniFolder();
		return m_IniFolder.c_str();

	}

	LPCTSTR GlobalPath::GetSVUtilityIniPath()
	{
		InitializeIniFolder();
		return m_Utility_ini.c_str();
	}

	LPCTSTR GlobalPath::GetSVIMIniPath()
	{
		InitializeIniFolder();
		return m_SVIM_ini.c_str();
	}

	LPCTSTR GlobalPath::GetSVOGatewayIniPath()
	{
		InitializeIniFolder();
		return m_SVOGateway_ini.c_str();
	}

	LPCTSTR GlobalPath::GetHardwareIniPath()
	{
		InitializeIniFolder();
		return m_Hardware_ini.c_str();
	}

	std::string GlobalPath::GetExternalToolPath(LPCTSTR filename)
	{
		InitializePath();
		return AppendFilename(m_SVObserverExternalToolFolder, filename);
	}


	std::string GlobalPath::GetRunPath(LPCTSTR filename )
	{
		InitializePath();
		return AppendFilename(m_SVObserverRunFolder, filename);
	}


	std::string GlobalPath::GetBinPath(LPCTSTR filename)
	{
		InitializePath();
		return AppendFilename(m_SVObserverBinFolder, filename);
	}


	std::string GlobalPath::GetTempPath(LPCTSTR filename )
	{
		InitializePath();
		return AppendFilename(m_TempFolder, filename);
	}


	std::string GlobalPath::GetObserverPath(LPCTSTR filename )
	{
		InitializePath();
		return AppendFilename(m_SVObserverFolder, filename);
	}


	std::string GlobalPath::GetSecondObserverPath(LPCTSTR filename )
	{
		InitializePath();
		return AppendFilename(m_SVSecondObserverFolder, filename);
	}

	std::string GlobalPath::GetAutoSaveRootPath(LPCTSTR filename)
	{
		return AppendFilename(GetSecondObserverPath(AutoSaveRelativePath), filename);
	}

	std::string GlobalPath::GetAutoSaveTempPath(LPCTSTR filename)
	{
		return AppendFilename(GetAutoSaveRootPath(AutoSaveTempRelativePath), filename);
	}

	std::string  GlobalPath::GetLastValidPath(LPCTSTR filename )
	{

		InitializePath();
		return AppendFilename(m_LastValidFolder, filename);
	}

	std::string GlobalPath::GetRamDrive(LPCTSTR filename)
	{
		InitializePath();
		return AppendFilename(m_SvimRamDrive, filename);
	}


	std::string GetValueString( LPCTSTR p_szSection, LPCTSTR p_szKey, LPCTSTR p_szDefault, LPCTSTR p_szFileName )
	{
		std::string sValue;
		TCHAR szResult[ 1024 ];

		unsigned long written  = ::GetPrivateProfileString( p_szSection, p_szKey, 
			p_szDefault,
			szResult, sizeof szResult,
			p_szFileName );

		sValue = szResult;

		return sValue;
	}
  
	void GlobalPath::InitializePath()
	{
		if(false == m_IsInitilizedPath)
		{
			m_SVObserverBinFolder  = GetValueString(SVIMDirectorySectionTag,SVIMObserverBinPathTag, DefaultObserverBinPath,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMObserverBinPathTag,  m_SVObserverBinFolder.c_str() , GetSVIMIniPath() );
  
			m_SVObserverExternalToolFolder = GetValueString(SVIMDirectorySectionTag,SVIMObserverExternalToolPathTag,DefaultExternalToolPath,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMObserverExternalToolPathTag, m_SVObserverExternalToolFolder.c_str(), GetSVIMIniPath() );
  
			m_SVObserverRunFolder = GetValueString(SVIMDirectorySectionTag,SVIMSVObserverRunPathTag,DefaultRunFolder,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMSVObserverRunPathTag, m_SVObserverRunFolder.c_str(), GetSVIMIniPath() );
  
			m_TempFolder  =  GetValueString(SVIMDirectorySectionTag,SVIMSVObserverTempPath,DefaultTempFolder,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMSVObserverTempPath, m_TempFolder.c_str(), GetSVIMIniPath());

			m_SVObserverFolder = GetValueString(SVIMDirectorySectionTag,SVIMSVObserverPath,DefaultObserverPath,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMSVObserverPath, m_SVObserverFolder.c_str(), GetSVIMIniPath());

			m_SVSecondObserverFolder = GetValueString(SVIMDirectorySectionTag,SVIMSVObserverSecondPath,DefaultSecondObserverPath,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMSVObserverSecondPath, m_SVSecondObserverFolder.c_str(), GetSVIMIniPath());

			m_LastValidFolder =  GetValueString(SVIMDirectorySectionTag,SVIMSVObserverLastValidPath,DefaultLastValidFolder,GetSVIMIniPath());
			::WritePrivateProfileString( SVIMDirectorySectionTag, SVIMSVObserverLastValidPath, m_LastValidFolder.c_str(), GetSVIMIniPath());

			m_SvimRamDrive = GetValueString(SVIMDirectorySectionTag, SVIMRamDrive, DefaultRamDrive, GetSVIMIniPath());
			::WritePrivateProfileString(SVIMDirectorySectionTag, SVIMSVObserverLastValidPath, m_LastValidFolder.c_str(), GetSVIMIniPath());

			m_IsInitilizedPath = true;
		}

	}

	std::string GlobalPath::AppendFilename(const std::string& rPath, LPCTSTR filename)
	{
		std::string Result{rPath};

		if (nullptr != filename)
		{
			if (filename[0] != '\\' && Result.size() > 0 && Result[Result.size() - 1] != '\\')
			{
				Result.append("\\");
			}
			Result.append(filename);
		}

		return Result;
	}

} //namespace SvStl