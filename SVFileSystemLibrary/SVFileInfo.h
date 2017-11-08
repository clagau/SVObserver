//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileInfo
//* .File Name       : $Workfile:   SVFileInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:42  $
//******************************************************************************

#pragma once



struct SVFileInfo
{
	std::string filename;
	UINT64 fileSize;
	FILETIME creationDate;
	FILETIME modifiedDate;
};

