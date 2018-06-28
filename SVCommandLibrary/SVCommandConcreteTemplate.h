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

#pragma region Includes
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVCommandAttributesTemplate.h"
#include "ObjectInterfaces/ICommand.h"
#include "SVCommandDataHolder.h"
#include "SVCommandResultsTemplate.h"
#pragma endregion Includes

typedef boost::function< HRESULT ( const SVCommandDataHolder&, SVCommandDataHolder& ) > SVCommandExecuteFunctor;

template<unsigned long CommandType>
class SVCommandConcreteTemplate : 
	public SvOi::ICommand,
	public SVCommandAttributesTemplate< CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap >,
	public SVCommandResultsTemplate< SVCommandDataHolder >
{
public:
	typedef SVCommandAttributesTemplate< CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap > SVCommandAttributes;
	typedef SVCommandResultsTemplate< SVCommandDataHolder > SVCommandResults;

	static unsigned long GetStaticCommandType();

	SVCommandConcreteTemplate( SVCommandExecuteFunctor ExecuteFunctor );
	virtual ~SVCommandConcreteTemplate();

	virtual unsigned long GetCommandType() const override {return m_CommandType;}
	virtual HRESULT Execute() override;

protected:
	SVCommandExecuteFunctor m_ExecuteFunctor;
	unsigned long m_CommandType{0UL};
};

#include "SVCommandConcreteTemplate.inl"

