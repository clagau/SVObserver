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

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVFILENAMEMANAGERCLASS_3A4A12C60196_INCLUDED
#define _INC_SVFILENAMEMANAGERCLASS_3A4A12C60196_INCLUDED

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

#endif /* _INC_SVFILENAMEMANAGERCLASS_3A4A12C60196_INCLUDED */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileNameManagerClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:38:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 11:10:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upped beta 4.22 to 7 and fixed problem with .sec files being deleted from the run directory on a load.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 09:45:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Nov 2002 13:04:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SaveItems
 *                        Modified to fix array-out-of-bounds error
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Nov 2001 10:17:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Reverted to old code in ::LoadItem & ::SaveItem except for
 * one copy in ::LoadItem removed prior to saving
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2001 13:17:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to fix configuration save as problem by adding function ::RenameItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2001 14:39:06   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions:  FindItem, LoadItem.  Added the following functions: RemoveUnusedFiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2001 16:16:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Jan 2001 13:18:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision add the newly created classes for the file management added to SVObserver Project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/