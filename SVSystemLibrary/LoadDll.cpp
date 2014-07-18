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
using namespace Seidenader::SVSystemLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

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
	Instance = NULL;
	Instance = m_DllsLoaded[DllName];
	if( NULL == Instance )
	{
		Instance = ::LoadLibrary( DllName.c_str() );
		if( NULL != Instance )
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

	BOOL Unloaded;

	while( m_DllsLoaded.end() != Iter )
	{
		Unloaded = ::FreeLibrary( Iter->second );
		Iter++;
	}
	m_DllsLoaded.clear();
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\LoadDll.cpp_v  $
 * 
 *    Rev 1.0   17 Jul 2014 16:05:36   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
