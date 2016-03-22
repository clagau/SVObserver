//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandConcreteTemplate
//* .File Name       : $Workfile:   SVCommandConcreteTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:06  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVCommandAttributesTemplate.h"
#include "SVCommandTemplate.h"
#include "SVCommandDataHolder.h"
#include "SVCommandResultsTemplate.h"

typedef boost::function< HRESULT ( const SVCommandDataHolder&, SVCommandDataHolder& ) > SVCommandExecuteFunctor;

/*
*/
template< unsigned long p_CommandType >
class SVCommandConcreteTemplate : 
	public SVCommandTemplate,
	public SVCommandAttributesTemplate< p_CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap >,
	public SVCommandResultsTemplate< SVCommandDataHolder >
{
public:
	typedef SVCommandAttributesTemplate< p_CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap > SVCommandAttributes;
	typedef SVCommandResultsTemplate< SVCommandDataHolder > SVCommandResults;

	static unsigned long GetStaticCommandType();

	SVCommandConcreteTemplate( SVCommandExecuteFunctor p_ExecuteFunctor );
	virtual ~SVCommandConcreteTemplate();

	virtual HRESULT Execute();

protected:
	SVCommandExecuteFunctor m_ExecuteFunctor;

};

#include "SVCommandConcreteTemplate.inl"

