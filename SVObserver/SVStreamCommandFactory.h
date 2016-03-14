//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStreamCommandFactory
//* .File Name       : $Workfile:   SVStreamCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:58  $
//******************************************************************************

#ifndef SVSTREAMCOMMANDFACTORY_H
#define SVSTREAMCOMMANDFACTORY_H

#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVUtilityLibrary/SVGUID.h"

/*
*/
class SVStreamCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	static HRESULT QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVStreamCommandFactory();
	virtual ~SVStreamCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVStreamCommandFactory > SVStreamCommandFactorySingleton;

#endif

