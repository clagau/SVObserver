//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemCommandFactory
//* .File Name       : $Workfile:   SVFileSystemCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:52  $
//******************************************************************************

#pragma once

#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"

/*
*/
class SVFileSystemCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	static HRESULT DoesFileExist( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVFileSystemCommandFactory();
	virtual ~SVFileSystemCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVFileSystemCommandFactory > SVFileSystemCommandFactorySingleton;


