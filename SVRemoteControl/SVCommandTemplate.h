//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandTemplate
//* .File Name       : $Workfile:   SVCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:19:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandStatus.h"
#pragma endregion Includes

class SVControlCommands;

class SVCommandTemplate
{
public:
	virtual ~SVCommandTemplate();

	virtual HRESULT Execute(CommandDispatcher disp) = 0;

	virtual HRESULT Cancel() = 0;

	const SVCommandStatus& GetStatus() const;

protected:
	SVCommandTemplate();

	SVCommandStatus m_Status;

private:
	SVCommandTemplate( const SVCommandTemplate& p_rObject );

	const SVCommandTemplate& operator=( const SVCommandTemplate& p_rObject );

};

typedef std::shared_ptr< SVCommandTemplate > SVCommandTemplatePtr;
