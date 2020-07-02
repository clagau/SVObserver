// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.cpp
// * .File Name       : $Workfile:   Utilities.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************
//DirectoryUtilities.h moved from SVCmnLib to SVLibrary 

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
//Moved to precompiled header: #include <tchar.h>
#include "DirectoryUtilities.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

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

