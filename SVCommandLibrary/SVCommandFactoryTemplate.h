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
#include "ObjectInterfaces/ICommand.h"
#pragma endregion Includes

class SVCommandFactoryTemplate
{
public:
	SVCommandFactoryTemplate() = delete;
	SVCommandFactoryTemplate(const SVCommandFactoryTemplate& p_rObject) =delete;

	const SVCommandFactoryTemplate& operator=(const SVCommandFactoryTemplate& p_rObject) =delete;

	virtual ~SVCommandFactoryTemplate();

	virtual unsigned long GetCommandType() const;

	virtual SvOi::ICommandPtr CreateCommand() const = 0;

protected:
	explicit SVCommandFactoryTemplate(unsigned long CommandType);

	const unsigned long m_CommandType;
};

typedef std::shared_ptr<SVCommandFactoryTemplate> SVCommandFactoryTemplatePtr;

