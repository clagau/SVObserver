//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameClass
//* .File Name       : $Workfile:   SVFileNameClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Sep 2014 13:37:06  $
//******************************************************************************

#pragma once

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
	SV_SVZ_CONFIGURATION_FILE_TYPE     = 9,
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
	explicit SVFileNameClass(LPCTSTR FullFileName);

	//This operator is the copy constructor.  This method 
	//will create an exact copy of the object supplied as the 
	//parameter to this constructor.
	SVFileNameClass(const SVFileNameClass& orig);

	//This operator is a constructor similiar to the copy 
	//constructor.  This method takes a pointer to a 
	//SVFileNameClass and copies the elements into the new 
	//object.
	explicit SVFileNameClass(const SVFileNameClass* const svFileName);

	virtual ~SVFileNameClass();

	//This operator allows a SVFileNameClass to assign its 
	//values into this objects values.
	const SVFileNameClass& operator=(const SVFileNameClass& rhs);

	//This operator exposes the string contained in the 
	//mcsFileName attribute.
	const std::string& GetFileNameOnly() const;

	//This operator exposed the string contained in the 
	//mcsExtension attribute.
	const std::string& GetExtension() const;

	//This operator exposes the values contained in the 
	//mdwFileType attribute.
	DWORD GetFileType() const;

	//This operator exposes the string contained in the 
	//mcsPathName attribute.
	const std::string& GetPathName() const;

	//This operator exposes the strings contained in the 
	//mcsFileName and mcsExtension attribute.
	const std::string& GetFileName() const;

	//This operator exposed the strings contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes.
	const std::string GetFullFileName() const;

	//This operator exposes the string contained in the 
	//mcsFileSelectDialogTitle attribute.
	const std::string& GetFileSelectDialogTitle() const;

	//This operator exposes the string contained in the 
	//mcsFileSaveDialogTitle attribute.
	const std::string& GetFileSaveDialogTitle() const;

	const std::string& GetDefaultPathName() const;

	//This operator exposes the string contained in the 
	//mcsDefaultFileName attribute.
	const std::string& GetDefaultFileName() const;

	//This operator exposes the string contained in the 
	//mcsFileExtensionFilterList attribute.
	const std::string& GetFileExtensionFilterList() const;

	//This operator exposes the value contained in the 
	//mdwFileSaveFlags attribute.
	DWORD GetFileSaveFlags() const;

	//This operator exposes the value contained in the 
	//mdwFileSelectFlags attribute.
	DWORD GetFileSelectFlags() const;

	//This operator exposes the string contained in the 
	//mcsDefaultFileExtension attribute.
	const std::string& GetDefaultFileExtension() const;

	//This operator sets the mdwFileType attribute and also 
	//sets default values for the mcsFileSelectDialogTitle, 
	//mcsFileSaveDialogTitle, mcsDefaultFileExtension, 
	//mcsDefaultFileName, mdwFileSelectFlags, 
	//mdwFileSaveFlags, mcsFileExtensionFilterList attributes.
	bool SetFileType(DWORD dwFileType);

	//This operator changes the string contained in the 
	//mcsPathName, mcsFileName and mcsExtension attributes to 
	//the value contain in the operator parameter.
	void SetFullFileName(LPCTSTR szFullName);

	//This operator changes the string contained in the 
	//mcsPathName attribute to the value contain in the 
	//operator parameter.
	void SetPathName(LPCTSTR szPathName);

	//This operator changes the string contained in the 
	//mcsFileName attribute to the value contain in the 
	//operator parameter.
	void SetFileName(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsFileNameOnly attribute to the value contain in the 
	//operator parameter.
	void SetFileNameOnly(LPCTSTR szFileName);

	//This operator changes the string contained in the 
	//mcsExtension attribute to the value contain in the 
	//operator parameter.
	void SetExtension(LPCTSTR szExtension);

	//This operator changes the string contained in the 
	//mcsFileSelectDialogTitle attribute to the value contain 
	//in the operator parameter.
	void SetFileSelectDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileSaveDialogTitle attribute to the value contain 
	//in the operator parameter.
	void SetFileSaveDialogTitle(LPCTSTR szTitle);

	//This operator changes the string contained in the 
	//mcsFileExtensionFilterList attribute to the value 
	//contain in the operator parameter.
	void SetFileExtensionFilterList(const std::string& rFilter);

	//This operator changes the string contained in the 
	//mdwFileSaveFlags attribute to the value contain in the 
	//operator parameter.
	void SetFileSaveFlags(DWORD dwFlags);

	//This operator changes the string contained in the 
	//mdwFileSelectFlags attribute to the value contain in 
	//the operator parameter.
	void SetFileSelectFlags(DWORD dwFlags);

	void SetDefaultFullFileName(LPCTSTR FullName);

	void SetDefaultPathName(const std::string& rName);

	//This operator changes the string contained in the 
	//mcsDefaultFileName attribute to the value contain in 
	//the operator parameter.
	void SetDefaultFileName(const std::string& rName);

	//This operator changes the string contained in the 
	//mcsDefaultFileExtension attribute to the value contain 
	//in the operator parameter.
	void SetDefaultFileExtension(const std::string& rExtension);

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a path for the object.  A 
	//file and or path may be selected, but only the path 
	//information is stored within the object.  The 
	//mcsFileName attribute will remain empty.
	bool SelectPath();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object.  A file needs to be selected and it is stored 
	//within the object.
	bool SelectFile();

	//This operator uses the object's attributes to build a 
	//file dialog for selecting a full file name for the 
	//object. The file needs to be selected and it is stored 
	//within the object.
	bool SaveFile();

	//************************************
	//! Method to set a list of characters to exclude for the file name
	//! \param ExculdeChar [in] array of characters to exclude
	//************************************
	void setExcludeCharacters( LPCTSTR ExculdeChar ) { m_ExcludeChar = ExculdeChar; };

	bool operator == (const SVFileNameClass& rhs) const;

private:
	void Init();

	//************************************
	//! Method to check if the file name has any invalid characters
	//! \param PathName [in] the selected path name
	//! \param FileName [in] the selected file name
	//! \returns the valid PathName
//************************************
	std::string checkFileName( LPCTSTR PathName, LPCTSTR FileName ) const;

	//This attribute defines which kind of file is desired to 
	//be held within this object.
	DWORD m_FileType;

	//This attribute defines the path name attached to this 
	//object.
	std::string m_PathName;

	//This attribute defines the file name attached to this 
	//object with the extension.  This variable gets updated 
	//when the GetFileName operation is called.
	mutable std::string m_FileName;

	//This is a temporary storage container for the string 
	//defining the file name without the extension.
	std::string m_FileNameOnly;

	//This attribute defines the extension attached to this 
	//object.
	std::string m_Extension;

	//This attribute defines the title to be placed on the 
	//file selection dialog.
	std::string m_FileSelectDialogTitle;

	//This attribute defines the title to be placed on the 
	//file save dialog for this object.
	std::string m_FileSaveDialogTitle;

	//This attribute define the default file extension to be 
	//used for both the file selection and file saving dialog.
	std::string m_DefaultFileExtension;

	//This attribute defines the default file name to be used 
	//for the file selecting and file saving dialog for this 
	//object.
	std::string m_DefaultFileName;

	std::string m_DefaultPathName;

	//This attribute defines the flags needed for the file 
	//selection dialog for this object.
	DWORD m_FileSelectFlags;

	//This attribute define the flags for the file saving 
	//dialog for this object.
	DWORD m_FileSaveFlags;

	//This attribute defines the file extension filter list 
	//for both the file selection and the file saving dialog 
	//for this object.
	std::string m_FileExtensionFilterList;

	std::string m_ExcludeChar;			//The array of file name exclude characters
};

