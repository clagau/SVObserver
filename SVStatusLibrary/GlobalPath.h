//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
#include <windows.h>
#include <winnt.h>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes


namespace Seidenader { namespace SVStatusLibrary
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
		SVString GetExternalToolPath(LPCTSTR filename = nullptr);
		SVString GetRunPath(LPCTSTR filename = nullptr);
		SVString GetBinPath(LPCTSTR filename = nullptr);
		SVString GetTempPath(LPCTSTR filename = nullptr);
		SVString GetLastValidPath(LPCTSTR filename = nullptr);
		SVString GetObserverPath(LPCTSTR filename = nullptr);
		SVString GetSecondObserverPath(LPCTSTR filename = nullptr);
#pragma endregion public functions 

	private: 

#pragma region private  constructors 
		GlobalPath();
		GlobalPath( const GlobalPath& );
#pragma endregion private  constructors 

#pragma region private  functions
		void InitializeIniFolder();
		void InitializePath();
#pragma endregion public  functions

#pragma region private members  
		bool m_IsInitializedIni;
		///Ini Files 
		SVString m_IniFolder;
		SVString  m_SVIM_ini;
		SVString  m_Hardware_ini;
		SVString  m_Utility_ini;

		///Svobserver Path 
		bool m_IsInitilizedPath;
		SVString m_SVObserverFolder;
		SVString m_SVSecondObserverFolder;
		SVString m_SVObserverBinFolder;
		SVString m_SVObserverExternalToolFolder;
		SVString m_SVObserverRunFolder;
		SVString m_TempFolder;
		SVString m_LastValidFolder;
#pragma endregion private members  
	};

} /* namespace SVStatusLibrary */ } /* namespace Seidenader */
namespace SvStl = Seidenader::SVStatusLibrary;
