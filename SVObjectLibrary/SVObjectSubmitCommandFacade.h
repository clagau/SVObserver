//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSubmitCommandFacade
//* .File Name       : $Workfile:   SVObjectSubmitCommandFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:48  $
//******************************************************************************

#pragma once

#include "SVCommandLibrary/SVCommandTemplate.h"

class SVObjectSubmitCommandFacade
{
	friend class SVObjectManagerClass;

protected:
	virtual HRESULT SubmitCommand( const SVCommandTemplatePtr& p_rCommandPtr ) = 0;

};

