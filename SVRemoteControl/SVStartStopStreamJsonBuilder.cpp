//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStartStopStreamJsonBuilder
//* .File Name       : $Workfile:   SVStartStopStreamJsonBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Jan 2015 10:18:26  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVStartStopStreamJsonBuilder.h"
#include "SVRemoteControlConstants.h"
#include "jsonlib/include/json.h"

#include "SVStringConversions.h"
#pragma endregion Includes

using namespace SeidenaderVision;

std::string SVStartStopStreamJsonBuilder::BuildStartCommand(const std::wstring& p_ListName)
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[SVRC::cmd::command] = SVRC::stream::startStream;
	l_Command[SVRC::stream::streamName] = SVStringConversions::to_utf8(p_ListName);

	return l_CommandWriter.write( l_Command );
}

std::string SVStartStopStreamJsonBuilder::BuildStopCommand(const std::wstring& p_ListName)
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[SVRC::cmd::command] = SVRC::stream::stopStream;
	l_Command[SVRC::stream::streamName] = SVStringConversions::to_utf8(p_ListName);

	return l_CommandWriter.write(l_Command);
}
