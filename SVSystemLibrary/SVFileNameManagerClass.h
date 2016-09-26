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
#include "SVUtilityLibrary\SVString.h"

//This class maintains a list of object global to the 
//application.  This class is responsible for copying the 
//appropriate files to the configuration and run directories.
class SVFileNameManagerClass 
{
private:
	SVString m_ConfigurationPathName; // Configuration Path Name variable
	SVString m_RunPathName; // Run Path Name variable
	SVFileNameLockablePointerArrayClass m_svFileNameList; // File Name List
	int m_iCurrentItem; //This attribute contains the value of the current position in the file name list.

	static SVFileNameManagerClass& Instance();
	
	// This method assigns the default values to the class's attributes.
	SVFileNameManagerClass();

public:
	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	static LPCTSTR GetFileNameList();

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	static BOOL SaveItem(SVFileNameClass* svFileName);

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	static BOOL LoadItem(SVFileNameClass* svFileName);

	//This operator renames the file from the selected 
	//location to the run directory as necessary.
	static BOOL RenameItem(SVFileNameClass* svFileName);

	//This operator exposes the string contained in the 
	//gcsConfigurationPathName application attribute.
	static LPCTSTR GetConfigurationPathName();

	//This operator exposes the string contained in the 
	//gcsRunPathName application attribute.
	static LPCTSTR GetRunPathName();

	//This operator sets the gcsRunPathName application 
	//attribute with the value passes as a parameter to the 
	//method.
	static BOOL SetRunPathName(LPCTSTR szPathName);

	//This operator sets the gcsConfigurationPathName 
	//application attribute with the value passes as a 
	//parameter to the method.
	static BOOL SetConfigurationPathName(LPCTSTR szPathName);

	//This operator will add a pointer to a SVFileNameClass 
	//object to the application file list.  The miCurrentItem 
	//index will be set to this element.
	static BOOL AddItem(SVFileNameClass* svpFileName);

	//This operator will remove a pointer to a 
	//SVFileNameClass object from the application file list.  
	//The miCurrentItem index will be set to the proceding 
	//element.
	static BOOL RemoveItem(SVFileNameClass* svpFileName);

	//This operator will search the global application list 
	//for a match for the pointer address.  It will reset the 
	//array index pointer to the appropriate array element.
	static BOOL FindItem(SVFileNameClass* svpFileNameClass);

	//This operator will return the pointer address to the 
	//appropriate file object at the current array index.
	static SVFileNameClass* GetItem();

	//This operator creates the full path tree if it does not 
	//exist.
	static BOOL CreatePath(LPCTSTR szPathName);

	//This operator copies all the files contained in the 
	//application array from the selected location to the 
	//configuration and run directory as necessary.
	static BOOL SaveItems();

	static BOOL RemoveUnusedFiles(BOOL bCleanConfigDir = true);
};
