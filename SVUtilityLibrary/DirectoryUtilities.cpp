// ******************************************************************************
// * COPYRIGHT (c) 2001 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.cpp
// * .File Name       : $Workfile:   Utilities.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
//Moved to precompiled header: #include <tchar.h>
#include "DirectoryUtilities.h"
#include "StringHelper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

namespace SvUl
{
bool CreateDirPath( LPCTSTR Path)
{
	int nBackCount = 0;
	size_t Pos = 0;

	if( nullptr == Path )
	{ 
		return false;
	}

	//Check if Path already exists
	if( 0 == _access( Path, 0 ) )
	{
		return true;
	}

	std::string PathToCreate( Path );
	//find the number of backslashes
	while( std::string::npos != (Pos = PathToCreate.find( _T("\\"), Pos )) )
	{
		Pos++;
		nBackCount++;
	}
	
	if(nBackCount > 1)
	{
		//check for all the directories, create dirs if they don't exist
		Pos = 0;
		for( int nIndex=0; nIndex < nBackCount; nIndex++ )
		{
			Pos = PathToCreate.find( _T("\\"), Pos );
			if(nIndex==0) 
			{
				Pos++;
				continue;
			}
			std::string Temp = SvUl::Left( PathToCreate, Pos );
			if( _tmkdir( Temp.c_str() ) )
			{
				if(errno != EEXIST) return false;
			}
			Pos++;
		}
	}
	else if( nBackCount == 1 )
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool ensureDirectoryExists(std::string& rDirectoryPath) //@TODO [Arvid][10.20][3.11.2021]: this function could be used to replace CreateDirPath()
{
	try
	{
		if (std::filesystem::is_directory(rDirectoryPath))
		{
			return true;
		}

		return std::filesystem::create_directories(rDirectoryPath);
	}
	catch (std::exception& e)
	{
		SvDef::StringVector msgList;
		msgList.push_back(e.what());
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}
}

}
