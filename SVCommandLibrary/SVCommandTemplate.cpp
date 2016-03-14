//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandTemplate
//* .File Name       : $Workfile:   SVCommandTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandTemplate.h"

SVCommandTemplate::~SVCommandTemplate()
{
}

unsigned long SVCommandTemplate::GetCommandType() const
{
	return m_CommandType;
}

SVCommandTemplate::SVCommandTemplate( unsigned long p_CommandType )
: m_CommandType( p_CommandType )
{
}

