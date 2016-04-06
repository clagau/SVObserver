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
//##ModelId=3A4A12C60196
class SVFileNameManagerClass 
{
public:
	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	//##ModelId=3A59E8470399
	LPCTSTR GetFileNameList();

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	//##ModelId=3A560337007D
	BOOL SaveItem(SVFileNameClass * svFileName);

	//This operator copies the file from the selected 
	//location to the configuration and run directory as 
	//necessary.
	//##ModelId=3A560FF50000
	BOOL LoadItem(SVFileNameClass * svFileName);

	//This operator renames the file from the selected 
	//location to the run directory as necessary.
	BOOL RenameItem(SVFileNameClass * svFileName);

	//This operator copies all the files contained in the 
	//application array from the selected location to the 
	//configuration and run directory as necessary.
	//##ModelId=3A55F7A9000F
	BOOL LoadItems();

	//This operator is the default constructor for the class. 
	// This method assigns the default values to the class's 
	//attributes.
	//##ModelId=3A4B960A0280
	SVFileNameManagerClass();

	//This operator exposes the string contained in the 
	//gcsConfigurationPathName application attribute.
	//##ModelId=3A4A3E72038A
	LPCTSTR GetConfigurationPathName();

	//This operator exposes the string contained in the 
	//gcsRunPathName application attribute.
	//##ModelId=3A4A41370251
	LPCTSTR GetRunPathName();

	//This operator sets the gcsRunPathName application 
	//attribute with the value passes as a parameter to the 
	//method.
	//##ModelId=3A4A4E0B0186
	BOOL SetRunPathName(LPCTSTR szPathName);

	//This operator sets the gcsConfigurationPathName 
	//application attribute with the value passes as a 
	//parameter to the method.
	//##ModelId=3A4A4F2200FA
	BOOL SetConfigurationPathName(LPCTSTR szPathName);

	//This operator will add a pointer to a SVFileNameClass 
	//object to the application file list.  The miCurrentItem 
	//index will be set to this element.
	//##ModelId=3A4B786E01C5
	BOOL AddItem(SVFileNameClass * svpFileName);

	//This operator will remove a pointer to a 
	//SVFileNameClass object from the application file list.  
	//The miCurrentItem index will be set to the proceding 
	//element.
	//##ModelId=3A52217C034B
	BOOL RemoveItem(SVFileNameClass * svpFileName);

	//This operator will search the global application list 
	//for a match for the pointer address.  It will reset the 
	//array index pointer to the appropriate array element.
	//##ModelId=3A522D32005D
	BOOL FindItem(SVFileNameClass * svpFileNameClass);

	//This operator will set the array index to the top of 
	//the array.
	//##ModelId=3A4B78E60138
	BOOL TopItem();

	//This operator will increment the array index by one.
	//##ModelId=3A4B79090138
	BOOL NextItem();

	//This operator will check to see if the array index is 
	//greater than the array size.
	//##ModelId=3A4B7924000F
	BOOL EndOfItems();

	//This operator will return the pointer address to the 
	//appropriate file object at the current array index.
	//##ModelId=3A4B796B01B5
	SVFileNameClass * GetItem();

	//This operator creates the full path tree if it does not 
	//exist.
	//##ModelId=3A4B7C70008C
	BOOL CreatePath(LPCTSTR szPathName);

	//This operator copies all the files contained in the 
	//application array from the selected location to the 
	//configuration and run directory as necessary.
	//##ModelId=3A4B7AF10251
	BOOL SaveItems();

	BOOL RemoveUnusedFiles(BOOL bCleanConfigDir = TRUE);

private:

	//This attribute contains the value of the current 
	//position in the application file name array.
	//##ModelId=3A5232EF00CB
	int miCurrentItem;

	// Global Configuration Path Name variable for DLL
	//static CString gcsConfigurationPathName;

	// Global Run Path Name variable for DLL
	//static CString gcsRunPathName;

	// Global File Name array for DLL
	//static SVFileNameLockablePointerArrayClass gsvFileNameArray;
};
