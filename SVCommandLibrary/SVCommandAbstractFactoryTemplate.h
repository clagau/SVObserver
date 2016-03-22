//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactoryTemplate
//* .File Name       : $Workfile:   SVCommandAbstractFactoryTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:12  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVCommandFactoryTemplate.h"

class SVCommandAbstractFactoryTemplate
{
public:
	typedef std::map< unsigned long, SVCommandFactoryTemplatePtr > SVCommandFactoryMap;

	virtual ~SVCommandAbstractFactoryTemplate();

	virtual bool IsValidCommandType( unsigned long p_CommandType ) const;

	virtual SVCommandTemplatePtr CreateCommand( unsigned long p_CommandType ) const;

protected:
	SVCommandAbstractFactoryTemplate( const SVCommandFactoryMap& p_rObject );

	SVCommandFactoryMap m_Factories;

private:
	SVCommandAbstractFactoryTemplate();
	SVCommandAbstractFactoryTemplate( const SVCommandAbstractFactoryTemplate& p_rObject );

	const SVCommandAbstractFactoryTemplate& operator=( const SVCommandAbstractFactoryTemplate& p_rObject );

};

typedef SVSharedPtr< SVCommandAbstractFactoryTemplate > SVCommandAbstractFactoryTemplatePtr;

