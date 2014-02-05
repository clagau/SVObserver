//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameClass
//* .File Name       : $Workfile:   SVFileNameClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Feb 2014 15:16:00  $
//******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVFILENAMECLASS_3A436B6802EE_INCLUDED
#define _INC_SVFILENAMECLASS_3A436B6802EE_INCLUDED

enum SVFileTypesEnum
{
	SV_DEFAULT_FILE_TYPE               = 0,
	SV_CAMERA_FILE_TYPE                = 1,
	SV_IMAGE_SOURCE_FILE_TYPE          = 2,
	SV_PATTERN_MODEL_FILE_TYPE         = 3,
	SV_MASK_FILE_TYPE                  = 4,
	SV_BAR_CODE_MATCH_STRING_FILE_TYPE = 5,
	SV_OCR_MATCH_STRING_FILE_TYPE      = 6,
	SV_OCR_FONT_FILE_TYPE              = 7,

	SV_SVX_CONFIGURATION_FILE_TYPE     = 9,
	SV_OCV_MATCH_STRING_FILE_TYPE      = 10,
	SV_OCV_FONT_FILE_TYPE              = 11,
	SV_OCV_CONSTRAINTS_FILE_TYPE       = 12,
	SV_OCV_CONTROLS_FILE_TYPE          = 13,
};

//This class maintain the type and information about a file.  
//This class can be used as a storage or selection class.  
//This class contains and manages information for only one 
//file or path.
//##ModelId=3A436B6802EE
class SVFileNameClass 
{
public:
	virtual ~SVFileNameClass();

	//This operator constructs the object with all of the 
	//default options except the path name, file name and 
	//extension.  These three pieces of information are 
	//collected from the full file name string pass as a 
	//parameter.
	//##ModelId=3A55D6E1003E
	SVFileNameClass(LPCTSTR szFullFileName);

	//This operator is the copy constructor.  This method 
	//will create an exact copy of the object supplied as the 
	//parameter to this constructor.
	//##ModelId=3A4CA55C0213
	SVFileNameClass(const SVFileNameClass& orig);

	//This operator is a constructor similiar to the copy 
	//constructor.  This method takes a pointer to a 
	//SVFileNameClass and copies the elements into the new 
	//object.
	//##ModelId=3A4B51590128
	SVFileNameClass(SVFileNameClass * svFileName);

	//This operator allows a SVFileNameClass to assign its 
	//values into this objects values.
	//##ModelId=3A4CA55C033C
	const SVFileNameClass& operator=(const SVFileNameClass& rhs);

	//This operator exposes the string contained in the 
	//mcsFileName attribute.
	//##ModelId=3A54E7370138
	LPCTSTR GetFileNameOnly();

	//This operator exposed the string contained in the 
	//mcsExtension attribute.
	//##ModelId=3A54E747000F
	LPCTSTR GetExtension();

	//This operator is the default constructor for the 
	//SVFileNameClass.  This method sets all object 
	//attributes to their default values.
	//##ModelId=3A4B50DE029F
	SVFileNameClass();

	//This operator exposes the values contained in the 
	//mdwFileType attribute.
	//##ModelId=3A4B5FC800EA
	DWORD GetFileType();

	//This operator exposes the string contained in the 
	//mcsPathName attribute.
	//##ModelId=3A436F020157
	LPCTSTR GetPathName();

	//This operator exposes the strings contained in the 
	//mcsFileName and mcsExtension attribute.
	//##ModelId=3A436F340186
	LPCTSTR GetFileName();

	//This operator exposed the strings contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes.
	//##ModelId=3A4B6AFD0148
	LPCTSTR GetFullFileName();

	//This operator exposes the string contained in the 
	//mcsFileSelectDialogTitle attribute.
	//##ModelId=3A4B60170290
	LPCTSTR GetFileSelectDialogTitle();

	//This operator exposes the string contained in the 
	//mcsFileSaveDialogTitle attribute.
	//##ModelId=3A4B6046036B
	LPCTSTR GetFileSaveDialogTitle();

	LPCTSTR GetDefaultPathName();

	//This operator exposes the string contained in the 
	//mcsDefaultFileName attribute.
	//##ModelId=3A4B607C032C
	LPCTSTR GetDefaultFileName();

	//This operator exposes the string contained in the 
	//mcsFileExtensionFilterList attribute.
	//##ModelId=3A4B60D6006D
	LPCTSTR GetFileExtensionFilterList();

	//This operator exposes the value contained in the 
	//mdwFileSaveFlags attribute.
	//##ModelId=3A4B60B601C5
	DWORD GetFileSaveFlags();

	//This operator exposes the value contained in the 
	//mdwFileSelectFlags attribute.
	//##ModelId=3A4B6099003E
	DWORD GetFileSelectFlags();

	//This operator exposes the string contained in the 
	//mcsDefaultFileExtension attribute.
	//##ModelId=3A4B60600213
	LPCTSTR GetDefaultFileExtension();

	//This operator sets the mdwFileType attribute and also 
	//sets default values for the mcsFileSelectDialogTitle, 
	//mcsFileSaveDialogTitle, mcsDefaultFileExtension, 
	//mcsDefaultFileName, mdwFileSelectFlags, 
	//mdwFileSaveFlags, mcsFileExtensionFilterList attributes.
	//##ModelId=3A4B5FE50222
	BOOL SetFileType(DWORD dwFileType);

	//This operator changes the string contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes to 
	//the value contain in the operator parameter.
	//##ModelId=3A4B6B11006D
	BOOL SetFullFileName(LPCTSTR szFullName);

	//This operator changes the string contained in the 
	//mcsPathName attribute to the value contain in the 
	//operator parameter.
	//##ModelId=3A4373E400BB
	BOOL SetPathName(LPCTSTR szPathName);

	//This operator changes the string contained in the 
	//mcsFileName attribute to the value contain in the 
	//operator parameter.
	//##ModelId=3A437439001F
	BOOL SetFileName(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsFileNameOnly attribute to the value contain in the 
	//operator parameter.
	//##ModelId=3A71AAF90239
	BOOL SetFileNameOnly(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsExtension attribute to the value contain in the 
	//operator parameter.
	//##ModelId=3A54E9090119
	BOOL SetExtension(LPCTSTR szExtension);

	//This operator changes the string contained in the 
	//mcsFileSelectDialogTitle attribute to the value contain 
	//in the operator parameter.
	//##ModelId=3A4B6DD80186
	BOOL SetFileSelectDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileSaveDialogTitle attribute to the value contain 
	//in the operator parameter.
	//##ModelId=3A4B6E11031C
	BOOL SetFileSaveDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileExtensionFilterList attribute to the value 
	//contain in the operator parameter.
	//##ModelId=3A4B6EE7007D
	BOOL SetFileExtensionFilterList(LPCTSTR szFilter);

	//This operator changes the string contained in the 
	//mdwFileSaveFlags attribute to the value contain in the 
	//operator parameter.
	//##ModelId=3A4B6EC9036B
	BOOL SetFileSaveFlags(DWORD dwFlags);

	//This operator changes the string contained in the 
	//mdwFileSelectFlags attribute to the value contain in 
	//the operator parameter.
	//##ModelId=3A4B6EA502EE
	BOOL SetFileSelectFlags(DWORD dwFlags);

	BOOL SetDefaultFullFileName(LPCTSTR szFullName);

	BOOL SetDefaultPathName(LPCTSTR szName);

	//This operator changes the string contained in the 
	//mcsDefaultFileName attribute to the value contain in 
	//the operator parameter.
	//##ModelId=3A4B6E8002CE
	BOOL SetDefaultFileName(LPCTSTR szName);

	//This operator changes the string contained in the 
	//mcsDefaultFileExtension attribute to the value contain 
	//in the operator parameter.
	//##ModelId=3A4B6E4701D4
	BOOL SetDefaultFileExtension(LPCTSTR szExtension);

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a path for the object.  A 
	//file and or path may be selected, but only the path 
	//information is stored within the object.  The 
	//mcsFileName attribute will remain empty.
	//##ModelId=3A4B6F52004E
	BOOL SelectPath();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object.  A file needs to be selected and it is stored 
	//within the object.
	//##ModelId=3A4B6F2B002E
	BOOL SelectFile();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object. The file needs to be selected and it is stored 
	//within the object.
	//##ModelId=3A4B6F40034B
	BOOL SaveFile();

private:
	void Init();
	//This attribute defines which kind of file is desired to 
	//be held within this object.
	//##ModelId=3A4B5C15009C
	DWORD mdwFileType;

	//This is a temporary storage container for the string 
	//defining the fully qualified file name.  This name 
	//include the path, name, and the extension.
	//##ModelId=3A71AAF9021E
	CString mcsFullFileName;

	//This attribute defines the path name attached to this 
	//object.
	//##ModelId=3A436E68001F
	CString mcsPathName;

	//This attribute defines the file name attached to this 
	//object with the extension.  This variable gets updated 
	//when the GetFileName operation is called.
	//##ModelId=3A71AAF9021C
	CString mcsFileName;

	//This is a temporary storage container for the string 
	//defining the file name without the extension.
	//##ModelId=3A71AAF9021B
	CString mcsFileNameOnly;

	//This attribute defines the extension attached to this 
	//object.
	//##ModelId=3A54E6E90196
	CString mcsExtension;

	//This attribute defines the title to be placed on the 
	//file selection dialog.
	//##ModelId=3A4B5C4A02AF
	CString mcsFileSelectDialogTitle;

	//This attribute defines the title to be placed on the 
	//file save dialog for this object.
	//##ModelId=3A4B5CE20232
	CString mcsFileSaveDialogTitle;

	//This attribute define the default file extension to be 
	//used for both the file selection and file saving dialog.
	//##ModelId=3A4B5D2903A9
	CString mcsDefaultFileExtension;

	//This attribute defines the default file name to be used 
	//for the file selecting and file saving dialog for this 
	//object.
	//##ModelId=3A4B5D590251
	CString mcsDefaultFileName;

	CString mcsDefaultPathName;

	//This attribute defines the flags needed for the file 
	//selection dialog for this object.
	//##ModelId=3A4B5D7000BB
	DWORD mdwFileSelectFlags;

	//This attribute define the flags for the file saving 
	//dialog for this object.
	//##ModelId=3A4B5DDA007D
	DWORD mdwFileSaveFlags;

	//This attribute defines the file extension filter list 
	//for both the file selection and the file saving dialog 
	//for ths object.
	//##ModelId=3A4B5EAB0148
	CString mcsFileExtensionFilterList;

};

#endif /* _INC_SVFILENAMECLASS_3A436B6802EE_INCLUDED */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFileNameClass.h_v  $
 * 
 *    Rev 1.1   04 Feb 2014 15:16:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed SV_SEC_CONFIGURATION_FILE_TYPE from SVFileTypesEnum.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:36:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Jul 2004 10:23:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed #define to enum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 13:02:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 09:32:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Nov 2002 13:04:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SVX file type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 14:32:18   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised all constructors to use the Init function.  Modified the function SetFileType to add an SEC_CONFIGURATION_FILE_TYPE as well as defined a default path for all file types.
 * Modified the following functions:  operator=, SetFullFileName, SelectPath, SelectFile, & SaveFile.
 * Added the following functions: GetDefaultPathName, SetDefaultFullFileName, SetDefaultPathName,
 * Init.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 14:05:46   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2001 16:16:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Jan 2001 13:18:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision add the newly created classes for the file management added to SVObserver Project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/