//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandFactory
//* .File Name       : $Workfile:   SVMatroxCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:12:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"
#pragma endregion Includes

class SVMatroxCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	SVMatroxCommandFactory();
	virtual ~SVMatroxCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVMatroxCommandFactory > SVMatroxCommandFactorySingleton;

