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

//This class maintains a list of object global to the 
//application.  This class is responsible for copying the 
//appropriate files to the configuration and run directories.
class SVFileNameManagerClass 
{
public:
	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	LPCTSTR GetFileNameList();

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	BOOL SaveItem(SVFileNameClass * svFileName);

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	BOOL LoadItem(SVFileNameClass * svFileName);

	//This operator renames the file from the selected 
	//location to the run directory as necessary.
	BOOL RenameItem(SVFileNameClass * svFileName);

	//This operator is the default constructor for the class. 
	// This method assigns the default values to the class's 
	//attributes.
	SVFileNameManagerClass();

	//This operator exposes the string contained in the 
	//gcsConfigurationPathName application attribute.
	LPCTSTR GetConfigurationPathName();

	//This operator exposes the string contained in the 
	//gcsRunPathName application attribute.
	LPCTSTR GetRunPathName();

	//This operator sets the gcsRunPathName application 
	//attribute with the value passes as a parameter to the 
	//method.
	BOOL SetRunPathName(LPCTSTR szPathName);

	//This operator sets the gcsConfigurationPathName 
	//application attribute with the value passes as a 
	//parameter to the method.
	BOOL SetConfigurationPathName(LPCTSTR szPathName);

	//This operator will add a pointer to a SVFileNameClass 
	//object to the application file list.  The miCurrentItem 
	//index will be set to this element.
	BOOL AddItem(SVFileNameClass * svpFileName);

	//This operator will remove a pointer to a 
	//SVFileNameClass object from the application file list.  
	//The miCurrentItem index will be set to the proceding 
	//element.
	BOOL RemoveItem(SVFileNameClass * svpFileName);

	//This operator will search the global application list 
	//for a match for the pointer address.  It will reset the 
	//array index pointer to the appropriate array element.
	BOOL FindItem(SVFileNameClass * svpFileNameClass);

	//This operator will return the pointer address to the 
	//appropriate file object at the current array index.
	SVFileNameClass * GetItem();

	//This operator creates the full path tree if it does not 
	//exist.
	BOOL CreatePath(LPCTSTR szPathName);

	//This operator copies all the files contained in the 
	//application array from the selected location to the 
	//configuration and run directory as necessary.
	BOOL SaveItems();

	BOOL RemoveUnusedFiles(BOOL bCleanConfigDir = TRUE);

private:

	//This attribute contains the value of the current 
	//position in the application file name array.
	int miCurrentItem;

	// Global Configuration Path Name variable for DLL
	//static CString gcsConfigurationPathName;

	// Global Run Path Name variable for DLL
	//static CString gcsRunPathName;

	// Global File Name array for DLL
	//static SVFileNameLockablePointerArrayClass gsvFileNameArray;
};
