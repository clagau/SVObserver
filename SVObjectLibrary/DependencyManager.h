//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file DependencyManager.h
//*****************************************************************************
/// Dependency manager using BGL
//******************************************************************************
#pragma once

#pragma region Includes
#include "JoinType.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVContainerLibrary/ObjectGraph.h"
#pragma endregion Includes

namespace Seidenader { namespace SVObjectLibrary
{
	class DependencyManager : public SvCl::ObjectGraph<SVGUID, JoinType>
	{

	typedef SvCl::ObjectGraph<SVGUID, JoinType>::DependencyInserter DependencyInserter;

	#pragma region Constructor
	public:
		DependencyManager();

		~DependencyManager();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
	static DependencyManager& Instance();

	#pragma endregion Public Methods
	};
} /* namespace SVObjectLibrary */ } /* namespace Seidenader */

namespace SvOl = Seidenader::SVObjectLibrary;
