//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonCmdBuilder
//* .File Name       : $Workfile:   SVJsonCmdBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Oct 2014 10:03:16  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVJsonCmdBuilder.h"
#include "SVRemoteControlConstants.h"
#pragma endregion Includes

long SVJsonCmdBuilder::m_cmdNo = 0;

SVJsonCmdBuilder::SVJsonCmdBuilder(const std::string & cmdName)
: m_name(cmdName)
, m_value(Json::objectValue)
{
	m_value[SVRC::cmd::name] = cmdName;
	m_value[SVRC::cmd::id] = ++m_cmdNo;
}

SVJsonCmdBuilder::~SVJsonCmdBuilder()
{
}

SVJsonCmdBuilder & SVJsonCmdBuilder::Argument(const std::string & name, const Json::Value & value)
{
	Json::Value & args = m_value[SVRC::cmd::arg];
	args[name] = value;
	return *this;
}
