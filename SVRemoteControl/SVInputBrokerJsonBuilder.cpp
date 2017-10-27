//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBrokerJsonBuilder
//* .File Name       : $Workfile:   SVInputBrokerJsonBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInputBrokerJsonBuilder.h"

#include "SVStringConversions.h"
#pragma endregion Includes



std::string SVInputBrokerJsonBuilder::BuildQueryListNamesCommand()
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[ "Command" ] = "QueryStreamNames";

	return l_CommandWriter.write( l_Command );
}

std::string SVInputBrokerJsonBuilder::BuildQueryListItemsCommand(const std::wstring& p_ListName)
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[ "Command" ] = "QueryStreamItems";
	l_Command[ "StreamName" ] = SVStringConversions::to_utf8( p_ListName );

	return l_CommandWriter.write( l_Command );
}

std::string SVInputBrokerJsonBuilder::BuildAddDataItemsCommand(const std::wstring& p_ListName, const SVMatchList& p_List)
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	Json::Value l_Array(Json::arrayValue);
	for (SVMatchList::const_iterator it = p_List.begin();it != p_List.end();++it)
	{
		Json::Value l_Element(Json::objectValue);

		l_Element[ "MatchString" ] = SVStringConversions::to_utf8( it->matchString );
		l_Element[ "MatchStringId" ] = it->transactionID;

		l_Array.append( l_Element );
	}
	l_Command[ "Command" ] = "AddDataItems";
	l_Command[ "StreamName" ] = SVStringConversions::to_utf8( p_ListName );
	l_Command[ "DataItems" ] = l_Array;

	return l_CommandWriter.write( l_Command );
}

std::string SVInputBrokerJsonBuilder::BuildClearDataItemsCommand(const std::wstring& p_ListName )
{
	Json::FastWriter l_CommandWriter;
	Json::Value l_Command(Json::objectValue);

	l_Command[ "Command" ] = "ClearDataItems";
	l_Command[ "StreamName" ] = SVStringConversions::to_utf8( p_ListName );
			
	return l_CommandWriter.write( l_Command );
}
