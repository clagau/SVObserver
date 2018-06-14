//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameManagerClass
//* .File Name       : $Workfile:   SVFileNameManagerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:16  $
//******************************************************************************

#pragma once

class SVFileNameClass;

#include "SVFileNameLockablePointerArrayClass.h"

//This class maintains a list of object global to the 
//application.  This class is responsible for copying the 
//appropriate files to the configuration and run directories.
class SVFileNameManagerClass 
{
private:
	// This method assigns the default values to the class's attributes.
	SVFileNameManagerClass();

public:
	static SVFileNameManagerClass& Instance();

	//list of file names contained within the vector.
	SvDef::StringVector GetFileNameList();

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	bool LoadItem(SVFileNameClass* pFileName);

	//This operator exposes the string contained in the 
	//gcsConfigurationPathName application attribute.
	const std::string& GetConfigurationPathName() const { return m_ConfigurationPathName; };

	//This operator exposes the string contained in the 
	//gcsRunPathName application attribute.
	const std::string& GetRunPathName() const { return m_RunPathName; };

	//This operator sets the gcsRunPathName application 
	//attribute with the value passes as a parameter to the 
	//method.
	bool SetRunPathName(LPCTSTR PathName);

	//This operator sets the gcsConfigurationPathName 
	//application attribute with the value passes as a 
	//parameter to the method.
	bool SetConfigurationPathName(LPCTSTR PathName);

	//This operator will add a pointer to a SVFileNameClass 
	//object to the application file list.  The miCurrentItem 
	//index will be set to this element.
	bool AddItem(SVFileNameClass* pFileName);

	//This operator will remove a pointer to a 
	//SVFileNameClass object from the application file list.  
	//The miCurrentItem index will be set to the proceeding 
	//element.
	bool RemoveItem(SVFileNameClass* pFileName);

	//This operator will search the global application list 
	//for a match for the pointer address.  It will reset the 
	//array index pointer to the appropriate array element.
	static bool FindItem(SVFileNameClass* pFileName);

	//This operator will return the pointer address to the 
	//appropriate file object at the current array index.
	static SVFileNameClass* GetItem();

	//This operator creates the full path tree if it does not 
	//exist.
	bool CreatePath(LPCTSTR PathName);

	bool RemoveUnusedFiles();

private:
	bool RemoveUnusedFiles(const std::string& rPath);

	std::string m_ConfigurationPathName; // Configuration Path Name variable
	std::string m_RunPathName; // Run Path Name variable
	SVFileNameLockablePointerArrayClass m_svFileNameList; // File Name List
	int m_iCurrentItem; //This attribute contains the value of the current position in the file name list.
};
