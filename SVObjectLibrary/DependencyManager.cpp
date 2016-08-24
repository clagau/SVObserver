//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file DependencyManager.cpp
//*****************************************************************************
/// Dependency manager using BGL
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DependencyManager.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVObjectLibrary
{
	#pragma region Constructor
	DependencyManager::DependencyManager() 
	{
	}

	DependencyManager::~DependencyManager() 
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	DependencyManager& DependencyManager::Instance()
	{
		static DependencyManager DependencyMgr;

		return DependencyMgr;
	}
	#pragma endregion Public Methods
} /* namespace SVObjectLibrary */ } /* namespace Seidenader */
