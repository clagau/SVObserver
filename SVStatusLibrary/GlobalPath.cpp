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


namespace Seidenader { namespace SVStatusLibrary
{

	///defaultValue 
	static const TCHAR* const DefaultLastValidFolder =   _T( "C:\\Last Valid" );	
	static const TCHAR* const DefaultTempFolder = _T( "C:\\Temp" );
	static const TCHAR* const DefaultRunFolder = _T( "C:\\RUN" );
	static const TCHAR* const DefaultObserverBinPath = _T( "C:\\SVObserver\\bin" );
	static const TCHAR* const DefaultExternalToolPath = _T( "C:\\SVObserver\\ExternalTool" );	
	static const TCHAR* const DefaultObserverPath = _T( "C:\\SVObserver" );	
	static const TCHAR* const DefaultSecondObserverPath = _T( "D:\\SVObserver" );	

	///subdirectories and sub-subdirectories of DefaultSecondObserverPath 
	static const TCHAR* const AutoSaveRelativePath = _T( "Autosave" );	
	const TCHAR* const AutoSaveTempRelativePath = _T("Temp");///< the "temporary" autosave temp directory name
	const TCHAR* const AutoSaveTemp1RelativePath = _T("Temp1MostRecent");///< the first autosave temp directory name
	const TCHAR* const AutoSaveTemp2RelativePath = _T("Temp2");///< the second autosave temp directory name
	const TCHAR* const AutoSaveTemp3RelativePath = _T("Temp3");///< the third autosave temp directory name

	///TAGS 
	static const TCHAR* const SVIMDirectorySectionTag = _T("SVIM Directories");
	static const TCHAR* const SVIMObserverBinPathTag = _T("ObserverBinPath");
	static const TCHAR* const SVIMObserverExternalToolPathTag = _T("ObserverExternalToolPath");
	static const TCHAR* const SVIMSVObserverRunPathTag = _T("ObserverRunPath");
	static const TCHAR* const SVIMSVObserverTempPath = _T("ObserverTempPath");
	static const TCHAR* const SVIMSVObserverPath = _T("ObserverPath");
	static const TCHAR* const SVIMSVObserverSecondPath = _T("ObserverSecondPath");
	static const TCHAR* const SVIMSVObserverLastValidPath = _T("LastValidPath");


	GlobalPath::GlobalPath():
		m_IsInitializedIni(false),
		m_IsInitilizedPath(false),
		m_AutosaveTempDirectoryName(AutoSaveTempRelativePath),
		m_AutosaveTempDirectory1Name(AutoSaveTemp1RelativePath),
		m_AutosaveTempDirectory2Name(AutoSaveTemp2RelativePath),
		m_AutosaveTempDirectory3Name(AutoSaveTemp3RelativePath)
	{
		m_IniFolder = _T("");
		m_SVIM_ini = _T("");
		m_Hardware_ini = _T("");
		m_Utility_ini = _T("");
		m_SVObserverBinFolder =  DefaultObserverBinPath;
		m_SVObserverRunFolder =  DefaultRunFolder;
		m_TempFolder =   DefaultTempFolder;
		m_SVObserverExternalToolFolder = DefaultExternalToolPath;
	}


	GlobalPath::GlobalPath( const GlobalPath& )
	{
		;
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
	LPCTSTR GlobalPath::GetHardwareIniPath()
	{
		InitializeIniFolder();
		return m_Hardware_ini.c_str();
	}


	void AppendFilename(LPCTSTR filename, SVString& rpath )
	{
		if(nullptr != filename)
		{
			if(filename[0] !=  '\\')
			{
				rpath.append("\\");
			}
			rpath.append(filename);
		}
	}


	SVString GlobalPath::GetExternalToolPath(LPCTSTR filename)
	{
		InitializePath();
		SVString ret =  m_SVObserverExternalToolFolder;
		AppendFilename(filename,ret );

		return ret;
	}


	SVString GlobalPath::GetRunPath(LPCTSTR filename )
	{
		InitializePath();
		SVString ret =  m_SVObserverRunFolder;
		AppendFilename(filename,ret );
		return ret;
	}


	SVString GlobalPath::GetBinPath(LPCTSTR filename)
	{
		InitializePath();
		SVString ret =  m_SVObserverBinFolder;
		AppendFilename(filename,ret );
		return ret;
	}


	SVString GlobalPath::GetTempPath(LPCTSTR filename )
	{
		InitializePath();
		SVString ret =  m_TempFolder;
		AppendFilename(filename,ret );
		return ret;
	}


	SVString GlobalPath::GetObserverPath(LPCTSTR filename )
	{
		InitializePath();
		SVString ret =  m_SVObserverFolder;
		AppendFilename(filename,ret );
		return ret;
	}


	SVString GlobalPath::GetSecondObserverPath(LPCTSTR filename )
	{
		InitializePath();
		SVString ret =  m_SVSecondObserverFolder;
		AppendFilename(filename, ret);
		return ret;
	}


	SVString GlobalPath::GetAutoSaveRootPath(LPCTSTR filename)
	{
		SVString ret =  GetSecondObserverPath(AutoSaveRelativePath);
		AppendFilename(filename, ret);
		return ret;
	}


	SVString GlobalPath::GetAutoSaveTempPath(LPCTSTR filename)
	{
		SVString ret =  GetAutoSaveRootPath(AutoSaveTempRelativePath);
		AppendFilename(filename, ret);
		return ret;
	}


	SVString  GlobalPath::GetLastValidPath(LPCTSTR filename )
	{

		InitializePath();
		SVString ret =  m_LastValidFolder;
		AppendFilename(filename,ret );
		return ret;
	}


	SVString GetValueString( LPCTSTR p_szSection, LPCTSTR p_szKey, LPCTSTR p_szDefault, LPCTSTR p_szFileName )
	{
		SVString sValue;
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

			m_IsInitilizedPath = true;
		}

	}







}
}




