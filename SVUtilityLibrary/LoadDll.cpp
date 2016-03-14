//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVUtilityLibrary
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

	HRESULT LoadDll::getDll( const SVString& DllName, HINSTANCE& Instance )
	{
		HRESULT Result = S_OK;
		Instance = nullptr;
		Instance = m_DllsLoaded[DllName];
		if( NULL == Instance )
		{
			Instance = ::LoadLibrary( DllName.c_str() );
			Sleep(0);
			if( nullptr != Instance )
			{
				m_DllsLoaded[DllName] = Instance;
			}
			else
			{
				Result = S_FALSE;
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
				//We are not setting the instance handle to NULL because the list is anyway being cleared after the loop
				//This Sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
				Sleep(0);
			}
			Iter++;
		}
		m_DllsLoaded.clear();
	}
#pragma endregion Public Methods
} /* namespace SVUtilityLibrary */ } /* namespace Seidenader */

