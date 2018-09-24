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
		LPCTSTR GetSVOGatewayIniPath();
		LPCTSTR GetHardwareIniPath();
		LPCTSTR GetSVObserverIniPath();
		std::string GetExternalToolPath(LPCTSTR filename = nullptr);
		std::string GetRunPath(LPCTSTR filename = nullptr);
		std::string GetBinPath(LPCTSTR filename = nullptr);
		std::string GetTempPath(LPCTSTR filename = nullptr);
		std::string GetLastValidPath(LPCTSTR filename = nullptr);
		std::string GetObserverPath(LPCTSTR filename = nullptr);
		std::string GetSecondObserverPath(LPCTSTR filename = nullptr);
		std::string GetAutoSaveRootPath(LPCTSTR filename = nullptr);
		std::string GetAutoSaveTempPath(LPCTSTR filename = nullptr);
		std::string GetRamDrive(LPCTSTR filename = nullptr);
		std::string GetAutosaveTempDirectoryName(){return m_AutosaveTempDirectoryName;}
		std::string GetAutosaveTemp1FileName(){return m_AutosaveTemp1FileName;}
		std::string GetAutosaveTemp2FileName(){return m_AutosaveTemp2FileName;}
		std::string GetAutosaveTemp3FileName(){return m_AutosaveTemp3FileName;}

#pragma endregion public functions 

	private: 

#pragma region private  constructors 
		GlobalPath();
#pragma endregion private  constructors 

#pragma region private  functions
		void InitializeIniFolder();
		void InitializePath();
		std::string AppendFilename(const std::string& rPath, LPCTSTR filename);
#pragma endregion private  functions

#pragma region private members  
		bool m_IsInitializedIni;
		///Ini Files 
		std::string m_IniFolder;
		std::string  m_SVIM_ini;
		std::string  m_SVOGateway_ini;
		std::string m_SVObserver_ini;
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
		std::string m_SvimRamDrive;

		std::string m_AutosaveTempDirectoryName;
		std::string m_AutosaveTemp1FileName;
		std::string m_AutosaveTemp2FileName;
		std::string m_AutosaveTemp3FileName;
#pragma endregion private members
	};
} //namespace SvStl
