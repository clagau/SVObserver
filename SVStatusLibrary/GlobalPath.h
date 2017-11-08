//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <windows.h>
//Moved to precompiled header: #include <winnt.h>

#pragma endregion Includes


namespace SvStl
{
	/// \brief Singleton which holds all Paths
	/// the paths are read from SVIM.ini file the ini paths are from command line or current directory
	class GlobalPath
	{
	public:

#pragma region public functions 
		static GlobalPath& Inst();
		LPCTSTR GetIniFolder();
		LPCTSTR GetSVUtilityIniPath();
		LPCTSTR GetSVIMIniPath();
		LPCTSTR GetHardwareIniPath();
		std::string GetExternalToolPath(LPCTSTR filename = nullptr);
		std::string GetRunPath(LPCTSTR filename = nullptr);
		std::string GetBinPath(LPCTSTR filename = nullptr);
		std::string GetTempPath(LPCTSTR filename = nullptr);
		std::string GetLastValidPath(LPCTSTR filename = nullptr);
		std::string GetObserverPath(LPCTSTR filename = nullptr);
		std::string GetSecondObserverPath(LPCTSTR filename = nullptr);
		std::string GetAutoSaveRootPath(LPCTSTR filename = nullptr);
		std::string GetAutoSaveTempPath(LPCTSTR filename = nullptr);
		std::string GetAutosaveTempDirectoryName(){return m_AutosaveTempDirectoryName;}
		std::string GetAutosaveTempDirectory1Name(){return m_AutosaveTempDirectory1Name;}
		std::string GetAutosaveTempDirectory2Name(){return m_AutosaveTempDirectory2Name;}
		std::string GetAutosaveTempDirectory3Name(){return m_AutosaveTempDirectory3Name;}

#pragma endregion public functions 

	private: 

#pragma region private  constructors 
		GlobalPath();
		GlobalPath( const GlobalPath& );
#pragma endregion private  constructors 

#pragma region private  functions
		void InitializeIniFolder();
		void InitializePath();
#pragma endregion private  functions

#pragma region private members  
		bool m_IsInitializedIni;
		///Ini Files 
		std::string m_IniFolder;
		std::string  m_SVIM_ini;
		std::string  m_Hardware_ini;
		std::string  m_Utility_ini;

		///SVObserver Path 
		bool m_IsInitilizedPath;
		std::string m_SVObserverFolder;
		std::string m_SVSecondObserverFolder;
		std::string m_SVObserverBinFolder;
		std::string m_SVObserverExternalToolFolder;
		std::string m_SVObserverRunFolder;
		std::string m_TempFolder;
		std::string m_LastValidFolder;

		std::string m_AutosaveTempDirectoryName;
		std::string m_AutosaveTempDirectory1Name;
		std::string m_AutosaveTempDirectory2Name;
		std::string m_AutosaveTempDirectory3Name;
#pragma endregion private members
	};
} //namespace SvStl
