//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputBrokerJsonBuilder
//* .File Name       : $Workfile:   SVOutputBrokerJsonBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Jan 2015 10:36:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVOutputBrokerJsonBuilder.h"
#include "SVStartStopStreamJsonBuilder.h"
#include "SVRemoteControlConstants.h"
#include "jsonlib/include/json.h"

#include "SVStringConversions.h"
#pragma endregion Includes

using namespace SeidenaderVision;

std::string SVOutputBrokerJsonBuilder::BuildStartCommand(const std::wstring& p_ListName)
{
	return SVStartStopStreamJsonBuilder::BuildStartCommand(p_ListName);
}

std::string SVOutputBrokerJsonBuilder::BuildStopCommand(const std::wstring& p_ListName)
{
	return SVStartStopStreamJsonBuilder::BuildStopCommand(p_ListName);
}

std::string SVOutputBrokerJsonBuilder::BuildQueryListNamesCommand()
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[SVRC::cmd::command] = SVRC::iobroker::queryStreamNames;

	return l_CommandWriter.write( l_Command );
}

std::string SVOutputBrokerJsonBuilder::BuildQueryListItemsCommand(const std::wstring& p_ListName)
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[SVRC::cmd::command] = SVRC::iobroker::queryDataItems;
	l_Command[SVRC::stream::streamName] = SVStringConversions::to_utf8( p_ListName );

	return l_CommandWriter.write( l_Command );
}
