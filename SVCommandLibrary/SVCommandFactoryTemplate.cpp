//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandFactoryTemplate
//* .File Name       : $Workfile:   SVCommandFactoryTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandFactoryTemplate.h"

SVCommandFactoryTemplate::~SVCommandFactoryTemplate()
{
}

unsigned long SVCommandFactoryTemplate::GetCommandType() const
{
	return m_CommandType;
}

SVCommandFactoryTemplate::SVCommandFactoryTemplate( unsigned long p_CommandType )
: m_CommandType( p_CommandType )
{
}

