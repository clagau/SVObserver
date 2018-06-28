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

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVCommandFactoryTemplate.h"
#pragma endregion Includes

class SVCommandAbstractFactoryTemplate
{
public:
	typedef std::map< unsigned long, SVCommandFactoryTemplatePtr > SVCommandFactoryMap;

	SVCommandAbstractFactoryTemplate() = delete;
	SVCommandAbstractFactoryTemplate(const SVCommandAbstractFactoryTemplate& rObject) = delete;

	const SVCommandAbstractFactoryTemplate& operator=(const SVCommandAbstractFactoryTemplate& p_rObject) = delete;
	virtual ~SVCommandAbstractFactoryTemplate();

	virtual bool IsValidCommandType( unsigned long p_CommandType ) const;

	virtual SvOi::ICommandPtr CreateCommand( unsigned long p_CommandType ) const;

protected:
	SVCommandAbstractFactoryTemplate(const SVCommandFactoryMap& rObject);

	SVCommandFactoryMap m_Factories;
};

typedef std::shared_ptr<SVCommandAbstractFactoryTemplate> SVCommandAbstractFactoryTemplatePtr;

