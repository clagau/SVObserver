//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandFactoryTemplate
//* .File Name       : $Workfile:   SVCommandFactoryTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandTemplate.h"
#pragma endregion Includes

class SVCommandFactoryTemplate
{
public:
	virtual ~SVCommandFactoryTemplate();

	unsigned long GetCommandType() const;

	virtual SVCommandTemplatePtr CreateCommand() const = 0;

protected:
	SVCommandFactoryTemplate( unsigned long p_CommandType );

	const unsigned long m_CommandType;

private:
	SVCommandFactoryTemplate();
	SVCommandFactoryTemplate( const SVCommandFactoryTemplate& p_rObject );

	const SVCommandFactoryTemplate& operator=( const SVCommandFactoryTemplate& p_rObject );

};

typedef std::shared_ptr< SVCommandFactoryTemplate > SVCommandFactoryTemplatePtr;

