//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameClass
//* .File Name       : $Workfile:   SVFileNameClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Sep 2014 13:37:06  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"

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
	SV_BAR_CODE_STORE_VALUE_FILE_TYPE  = 8,
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
class SVFileNameClass 
{
public:
	//This operator is the default constructor for the 
	//SVFileNameClass.  This method sets all object 
	//attributes to their default values.
	SVFileNameClass();

	//This operator constructs the object with all of the 
	//default options except the path name, file name and 
	//extension.  These three pieces of information are 
	//collected from the full file name string pass as a 
	//parameter.
	SVFileNameClass(LPCTSTR szFullFileName);

	//This operator is the copy constructor.  This method 
	//will create an exact copy of the object supplied as the 
	//parameter to this constructor.
	SVFileNameClass(const SVFileNameClass& orig);

	//This operator is a constructor similiar to the copy 
	//constructor.  This method takes a pointer to a 
	//SVFileNameClass and copies the elements into the new 
	//object.
	SVFileNameClass(SVFileNameClass * svFileName);

	virtual ~SVFileNameClass();

	//This operator allows a SVFileNameClass to assign its 
	//values into this objects values.
	const SVFileNameClass& operator=(const SVFileNameClass& rhs);

	//This operator exposes the string contained in the 
	//mcsFileName attribute.
	LPCTSTR GetFileNameOnly() const;

	//This operator exposed the string contained in the 
	//mcsExtension attribute.
	LPCTSTR GetExtension() const;

	//This operator exposes the values contained in the 
	//mdwFileType attribute.
	DWORD GetFileType() const;

	//This operator exposes the string contained in the 
	//mcsPathName attribute.
	LPCTSTR GetPathName() const;

	//This operator exposes the strings contained in the 
	//mcsFileName and mcsExtension attribute.
	LPCTSTR GetFileName() const;

	//This operator exposed the strings contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes.
	LPCTSTR GetFullFileName() const;

	//This operator exposes the string contained in the 
	//mcsFileSelectDialogTitle attribute.
	LPCTSTR GetFileSelectDialogTitle() const;

	//This operator exposes the string contained in the 
	//mcsFileSaveDialogTitle attribute.
	LPCTSTR GetFileSaveDialogTitle() const;

	LPCTSTR GetDefaultPathName() const;

	//This operator exposes the string contained in the 
	//mcsDefaultFileName attribute.
	LPCTSTR GetDefaultFileName() const;

	//This operator exposes the string contained in the 
	//mcsFileExtensionFilterList attribute.
	LPCTSTR GetFileExtensionFilterList() const;

	//This operator exposes the value contained in the 
	//mdwFileSaveFlags attribute.
	DWORD GetFileSaveFlags() const;

	//This operator exposes the value contained in the 
	//mdwFileSelectFlags attribute.
	DWORD GetFileSelectFlags() const;

	//This operator exposes the string contained in the 
	//mcsDefaultFileExtension attribute.
	LPCTSTR GetDefaultFileExtension() const;

	//This operator sets the mdwFileType attribute and also 
	//sets default values for the mcsFileSelectDialogTitle, 
	//mcsFileSaveDialogTitle, mcsDefaultFileExtension, 
	//mcsDefaultFileName, mdwFileSelectFlags, 
	//mdwFileSaveFlags, mcsFileExtensionFilterList attributes.
	BOOL SetFileType(DWORD dwFileType);

	//This operator changes the string contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes to 
	//the value contain in the operator parameter.
	BOOL SetFullFileName(LPCTSTR szFullName);

	//This operator changes the string contained in the 
	//mcsPathName attribute to the value contain in the 
	//operator parameter.
	BOOL SetPathName(LPCTSTR szPathName);

	//This operator changes the string contained in the 
	//mcsFileName attribute to the value contain in the 
	//operator parameter.
	BOOL SetFileName(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsFileNameOnly attribute to the value contain in the 
	//operator parameter.
	BOOL SetFileNameOnly(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsExtension attribute to the value contain in the 
	//operator parameter.
	BOOL SetExtension(LPCTSTR szExtension);

	//This operator changes the string contained in the 
	//mcsFileSelectDialogTitle attribute to the value contain 
	//in the operator parameter.
	BOOL SetFileSelectDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileSaveDialogTitle attribute to the value contain 
	//in the operator parameter.
	BOOL SetFileSaveDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileExtensionFilterList attribute to the value 
	//contain in the operator parameter.
	BOOL SetFileExtensionFilterList(LPCTSTR szFilter);

	//This operator changes the string contained in the 
	//mdwFileSaveFlags attribute to the value contain in the 
	//operator parameter.
	BOOL SetFileSaveFlags(DWORD dwFlags);

	//This operator changes the string contained in the 
	//mdwFileSelectFlags attribute to the value contain in 
	//the operator parameter.
	BOOL SetFileSelectFlags(DWORD dwFlags);

	BOOL SetDefaultFullFileName(LPCTSTR szFullName);

	BOOL SetDefaultPathName(LPCTSTR szName);

	//This operator changes the string contained in the 
	//mcsDefaultFileName attribute to the value contain in 
	//the operator parameter.
	BOOL SetDefaultFileName(LPCTSTR szName);

	//This operator changes the string contained in the 
	//mcsDefaultFileExtension attribute to the value contain 
	//in the operator parameter.
	BOOL SetDefaultFileExtension(LPCTSTR szExtension);

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a path for the object.  A 
	//file and or path may be selected, but only the path 
	//information is stored within the object.  The 
	//mcsFileName attribute will remain empty.
	BOOL SelectPath();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object.  A file needs to be selected and it is stored 
	//within the object.
	BOOL SelectFile();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object. The file needs to be selected and it is stored 
	//within the object.
	BOOL SaveFile();

	//************************************
	//! Method to set a list of characters to exclude for the file name
	//! \param ExculdeChar [in] array of characters to exclude
	//************************************
	void setExcludeCharacters( LPCTSTR ExculdeChar ) { m_ExcludeChar = ExculdeChar; };

private:
	void Init();

	//************************************
	//! Method to check if the file name has any invalid characters
	//! \param PathName [in] the selected path name
	//! \param FileName [in] the selected file name
	//! \returns the valid PathName
//************************************
	SVString checkFileName( LPCTSTR PathName, LPCTSTR FileName ) const;

	//This attribute defines which kind of file is desired to 
	//be held within this object.
	DWORD m_FileType;

	//This is a temporary storage container for the string 
	//defining the fully qualified file name.  This name 
	//include the path, name, and the extension.
	mutable SVString m_FullFileName;

	//This attribute defines the path name attached to this 
	//object.
	SVString m_PathName;

	//This attribute defines the file name attached to this 
	//object with the extension.  This variable gets updated 
	//when the GetFileName operation is called.
	mutable SVString m_FileName;

	//This is a temporary storage container for the string 
	//defining the file name without the extension.
	SVString m_FileNameOnly;

	//This attribute defines the extension attached to this 
	//object.
	SVString m_Extension;

	//This attribute defines the title to be placed on the 
	//file selection dialog.
	SVString m_FileSelectDialogTitle;

	//This attribute defines the title to be placed on the 
	//file save dialog for this object.
	SVString m_FileSaveDialogTitle;

	//This attribute define the default file extension to be 
	//used for both the file selection and file saving dialog.
	SVString m_DefaultFileExtension;

	//This attribute defines the default file name to be used 
	//for the file selecting and file saving dialog for this 
	//object.
	SVString m_DefaultFileName;

	SVString m_DefaultPathName;

	//This attribute defines the flags needed for the file 
	//selection dialog for this object.
	DWORD m_FileSelectFlags;

	//This attribute define the flags for the file saving 
	//dialog for this object.
	DWORD m_FileSaveFlags;

	//This attribute defines the file extension filter list 
	//for both the file selection and the file saving dialog 
	//for this object.
	SVString m_FileExtensionFilterList;

	SVString m_ExcludeChar;			//The array of file name exclude characters
};

