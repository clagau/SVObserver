//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LoadDll
//* .File Name       : $Workfile:   LoadDll.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 16:05:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LoadDll.h"

#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvUl
{
	#pragma region Constructor
	LoadDll::LoadDll()
	{
	}

	LoadDll::~LoadDll()
	{
		freeDlls();
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	LoadDll& LoadDll::Instance()
	{
		static LoadDll Object;

		return Object;
	}

	HRESULT LoadDll::getDll( const std::string& rDllName, HINSTANCE& Instance )
	{
		HRESULT Result = S_OK;
		Instance = nullptr;
		Instance = m_DllsLoaded[rDllName];
		if( nullptr == Instance )
		{
			Instance = ::LoadLibrary( rDllName.c_str() );
			if(nullptr == Instance && !m_DefaultPath.empty())
			{
				std::string filePath{m_DefaultPath + rDllName};
				Instance = ::LoadLibrary(filePath.c_str());
			}
			Sleep(0);
			if( nullptr != Instance )
			{
				m_DllsLoaded[rDllName] = Instance;
			}
			else
			{
				Result = E_FAIL;
			}
		}
	
		return Result;
	}

	void LoadDll::freeDlls()
	{
		DllMap::const_iterator Iter( m_DllsLoaded.begin() );

		while( m_DllsLoaded.end() != Iter )
		{
			if( ::FreeLibrary( Iter->second ) )
			{
				//We are not clearing the instance handle because the list is being cleared after the loop
				//This Sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
				Sleep(0);
			}
			Iter++;
		}
		m_DllsLoaded.clear();
	}
	#pragma endregion Public Methods
} // namespace SvUl

