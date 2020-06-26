//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameClass
//* .File Name       : $Workfile:   SVFileNameClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   18 Sep 2014 13:37:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <io.h>
#include <commdlg.h>
#include "SVFileNameClass.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes

SVFileNameClass::SVFileNameClass()
{
	Init();
}

SVFileNameClass::~SVFileNameClass()
{
}

SVFileNameClass::SVFileNameClass(LPCTSTR szFullFileName)
{
	Init();

	SetFullFileName( szFullFileName );
}

SVFileNameClass::SVFileNameClass(SVFileNameClass * svFileName)
{
	if ( svFileName )
	{
		*this = *svFileName;
	}
	else
	{
		Init();
	}
}

SVFileNameClass::SVFileNameClass(const SVFileNameClass& orig)
{
	*this = orig;
}

const SVFileNameClass& SVFileNameClass::operator=(const SVFileNameClass& rhs)
{
	SetFileType( rhs.m_FileType );
	SetPathName( rhs.m_PathName.c_str() );
	SetFileNameOnly( rhs.m_FileNameOnly.c_str() );
	SetExtension( rhs.m_Extension.c_str() );
	SetFileSelectDialogTitle( rhs.m_FileSelectDialogTitle.c_str() );
	SetFileSaveDialogTitle( rhs.m_FileSaveDialogTitle.c_str() );
	SetDefaultPathName( rhs.m_DefaultPathName );
	SetDefaultFileName( rhs.m_DefaultFileName );
	SetDefaultFileExtension( rhs.m_DefaultFileExtension );
	SetFileSelectFlags( rhs.m_FileSelectFlags );
	SetFileSaveFlags( rhs.m_FileSaveFlags );
	SetFileExtensionFilterList( rhs.m_FileExtensionFilterList );
	setExcludeCharacters( rhs.m_ExcludeChar.c_str() );

	return *this;
}

DWORD SVFileNameClass::GetFileType() const
{
	return m_FileType;
}

const std::string& SVFileNameClass::GetPathName() const
{
	return m_PathName;
}

const std::string& SVFileNameClass::GetFileName() const
{
	m_FileName = m_FileNameOnly + m_Extension;

	return m_FileName;
}

const std::string& SVFileNameClass::GetFileNameOnly() const
{
	return m_FileNameOnly;
}

const std::string& SVFileNameClass::GetExtension() const
{
	return m_Extension;
}

const std::string SVFileNameClass::GetFullFileName() const
{
	std::string result;
	
	result = GetFileName();

	if (!m_PathName.empty())
	{
		result = m_PathName + "\\" + result;
	}

	return result;
}

const std::string& SVFileNameClass::GetFileSelectDialogTitle() const
{
	return m_FileSelectDialogTitle;
}

const std::string& SVFileNameClass::GetFileSaveDialogTitle() const
{
	return m_FileSaveDialogTitle;
}

const std::string& SVFileNameClass::GetDefaultPathName() const
{
	return m_DefaultPathName;
}

const std::string& SVFileNameClass::GetDefaultFileName() const
{
	return m_DefaultFileName;
}

const std::string& SVFileNameClass::GetDefaultFileExtension() const
{
	return m_DefaultFileExtension;
}

DWORD SVFileNameClass::GetFileSelectFlags() const
{
	return m_FileSelectFlags;
}

DWORD SVFileNameClass::GetFileSaveFlags() const
{
	return m_FileSaveFlags;
}

const std::string& SVFileNameClass::GetFileExtensionFilterList() const
{
	return m_FileExtensionFilterList;
}

bool SVFileNameClass::SetFileType(DWORD dwFileType)
{
	bool bOk = false;

	m_FileType = dwFileType;

	switch (m_FileType)
	{
		case SV_DEFAULT_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select File") );
			SetFileSaveDialogTitle( _T("Save File") );
			SetDefaultFileExtension( _T("") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("All Files (*.*)|*.*||") );

			bOk = true;
			break;
		}
		case SV_CAMERA_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Camera File") );
			SetFileSaveDialogTitle( _T("Save Camera File") );
			SetDefaultFileExtension( _T(".cca") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( _T("C:\\CAM") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Digitizer Files (*.cca)|*.cca||") );

			bOk = true;
			break;
		}
		case SV_IMAGE_SOURCE_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Image File") );
			SetFileSaveDialogTitle( _T("Save Image File") );
			SetDefaultFileExtension( ".bmp" );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( _T("C:\\IMAGES") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Bitmap File Format (*.bmp)|*.bmp|MIL Image Format File (*.mim)|*.mim|TIFF Image Format (*.tif)|*.tif||") );

			bOk = true;
			break;
		}
		case SV_PATTERN_MODEL_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Model File") );
			SetFileSaveDialogTitle( _T("Save Model File") );
			SetDefaultFileExtension( _T(".bmp") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( "Bitmap File Format (*.bmp)|*.bmp|MIL Image Format File (*.mim)|*.mim|TIFF Image Format (*.tif)|*.tif||" );

			bOk = true;
			break;
		}
		case SV_MASK_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select SVObserver Mask File") );
			SetFileSaveDialogTitle( _T("Save SVObserver Mask File") );
			SetDefaultFileExtension( _T(".svm") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("SVObserver Mask Files (*.svm)|*.svm||") );

			bOk = true;
			break;
		}
 		case SV_BAR_CODE_MATCH_STRING_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Bar Code Match String File") );
			SetFileSaveDialogTitle( _T("Save Bar Code Match String File") );
			SetDefaultFileExtension( _T(".txt") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||") );

			bOk = true;
			break;
		}
		case SV_BAR_CODE_STORE_VALUE_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select File") );
			SetFileSaveDialogTitle( _T("Save Bar Code Value File") );
			SetDefaultFileExtension( _T(".txt") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||") );

			bOk = true;
			break;
		}
		case SV_OCR_MATCH_STRING_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select OCR Match String File") );
			SetFileSaveDialogTitle( _T("Save OCR Match String File") );
			SetDefaultFileExtension( _T(".txt") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||") );

			bOk = true;
			break;
		}
		case SV_OCR_FONT_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select WiT Font Training File") );
			SetFileSaveDialogTitle( _T("Save WiT Font Training File") );
			SetDefaultFileExtension( _T(".wit") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("WiT Font Files (*.wit)|*.wit||") );

			bOk = true;
			break;
		}
		case SV_SVZ_CONFIGURATION_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Configuration File") );
			SetFileSaveDialogTitle( _T( "Save Configuration File" ) );
			SetDefaultFileExtension(SvDef::cPackedConfigExtension);
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T( "SVResearch Configuration Files (*.svz)|*.svz;*.svx||" ) );

			bOk = true;
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

void SVFileNameClass::SetFullFileName(LPCTSTR szFullName)
{
	std::string sFullName;
	if (szFullName)
	{
		sFullName = szFullName;
	}

	if ( sFullName.empty() )
	{
		SetPathName( nullptr );
		SetFileNameOnly( nullptr );
		SetExtension( nullptr );
	}
	else
	{
		std::string sPathName;

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( sFullName.c_str(), drive, dir, fname, ext );

		sPathName = drive;
		sPathName += dir;

		SetFileNameOnly( fname );
		SetExtension( ext );
		SetPathName( sPathName.c_str() );
	}
}

void SVFileNameClass::SetPathName(LPCTSTR szPathName)
{
	if (szPathName)
	{
		m_PathName = szPathName;

		SvUl::searchAndReplace(m_PathName, "/", "\\");
		m_PathName = SvUl::TrimRight(m_PathName.c_str(), "\\" );
	}
	else
	{
		m_PathName.clear();
	}
}

void SVFileNameClass::SetFileName(LPCTSTR szFileName)
{
	std::string sFullFileName = GetPathName();

	if (szFileName)
	{
		if ( sFullFileName.empty() )
		{
			sFullFileName = szFileName;
		}
		else
		{
			sFullFileName += "\\";
			sFullFileName += szFileName;
		}
	}

	SetFullFileName( sFullFileName.c_str() );
}

void SVFileNameClass::SetFileNameOnly(LPCTSTR szFileName)
{
	if (szFileName)
	{
		m_FileNameOnly = SvUl::TrimRight(szFileName);
	}
	else
	{
		m_FileNameOnly.clear();
	}
}

void SVFileNameClass::SetExtension(LPCTSTR szExtension)
{
	if (szExtension)
	{
		m_Extension = szExtension;
	}
	else
	{
		m_Extension.clear();
	}
}

void SVFileNameClass::SetFileSelectDialogTitle(LPCTSTR szTitle)
{
	if (szTitle)
	{
		m_FileSelectDialogTitle = szTitle;
	}
	else
	{
		m_FileSelectDialogTitle.clear();
	}
}

void SVFileNameClass::SetFileSaveDialogTitle(LPCTSTR szTitle)
{
	if (szTitle)
	{
		m_FileSaveDialogTitle = szTitle;
	}
	else
	{
		m_FileSaveDialogTitle.clear();
	}
}

void SVFileNameClass::SetDefaultFullFileName(LPCTSTR FullName)
{
	std::string sFullName;

	if( nullptr != FullName)
	{
		sFullName = FullName;
	}

	if( sFullName.empty() )
	{
		SetDefaultPathName( std::string() );
		SetDefaultFileName( std::string() );
		SetDefaultFileExtension( std::string() );
	}
	else
	{
		std::string sPathName;

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( sFullName.c_str(), drive, dir, fname, ext );

		sPathName = drive;
		sPathName += dir;

		SetDefaultFileName( std::string(fname) );
		SetDefaultFileExtension( std::string(ext) );
		SetDefaultPathName( sPathName );
	}
}

void SVFileNameClass::SetDefaultPathName(const std::string& rPath)
{
	if( !rPath.empty() )
	{
		m_DefaultPathName = rPath;

		SvUl::searchAndReplace(m_DefaultPathName, "/", "\\");
		m_DefaultPathName = SvUl::TrimRight(m_DefaultPathName.c_str(), "\\" );
	}
	else
	{
		m_DefaultPathName.clear();
	}
}

void SVFileNameClass::SetDefaultFileName(const std::string& rName)
{
	m_DefaultFileName = rName;
}

void SVFileNameClass::SetDefaultFileExtension(const std::string& rExtension)
{
	m_DefaultFileExtension = rExtension;
}

void SVFileNameClass::SetFileSelectFlags(DWORD dwFlags)
{
	m_FileSelectFlags = dwFlags;
}

void SVFileNameClass::SetFileSaveFlags(DWORD dwFlags)
{
	m_FileSaveFlags = dwFlags;
}

void SVFileNameClass::SetFileExtensionFilterList(const std::string& rFilter)
{
	m_FileExtensionFilterList = rFilter;
}

bool SVFileNameClass::SelectPath()
{
	bool Result{ false };

	std::string Path = GetPathName();
	if (Path.empty())
	{
		Path = GetDefaultPathName();
	}

	if (IDOK == SvOi::OpenSVFolderDialog( Path, _T("Select Folder")))
	{
		SetPathName(Path.c_str());
		SetDefaultPathName(Path);
		Result = true;
	}

	return Result;
}

bool SVFileNameClass::SelectFile()
{
	bool bOk = false;

	if ( GetDefaultPathName().empty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( GetDefaultFileName().empty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( GetDefaultFileExtension().empty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}

	std::string FileName = GetDefaultFileName();
	if (!FileName.empty())
	{
	    FileName += GetDefaultFileExtension();
	}

	std::string Path = GetPathName();

	if (Path.empty())
	{
		Path = GetDefaultPathName();
	}

	//The dot is removed from the defaultFileExtension, because that can lead too two dots after the filename in specific use cases
	std::string defaultFileExtension = GetDefaultFileExtension();
	size_t positionOfDot = defaultFileExtension.find(".");

	if (0 == positionOfDot)
	{
		defaultFileExtension = defaultFileExtension.substr(1);
	}

	if (IDOK == SvOi::OpenSVFileDialog(true, defaultFileExtension.c_str(), FileName, GetFileSelectFlags(), GetFileExtensionFilterList().c_str(), Path, GetFileSelectDialogTitle().c_str()))
	{
		SetFullFileName(Path.c_str());
		
		SetDefaultFullFileName(Path.c_str());
		bOk = true;
	}

	return bOk;
}

bool SVFileNameClass::SaveFile()
{
	bool bOk = false;
	bool bDone = false;

	if ( GetDefaultPathName().empty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( GetDefaultFileName().empty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( GetDefaultFileExtension().empty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}



	while ( ! bDone )
	{
		std::string FileName = GetDefaultFileName();
		if (!FileName.empty())
		{
			FileName += GetDefaultFileExtension();
		}

		std::string Path = GetPathName();
		if (Path.empty())
		{
			Path = GetDefaultPathName();
		}

		bOk = IDOK == SvOi::OpenSVFileDialog(false, GetDefaultFileExtension().c_str(), FileName, GetFileSaveFlags(), GetFileExtensionFilterList().c_str(), Path, GetFileSaveDialogTitle().c_str());

		bDone = ! bOk;

		if ( ! bDone )
		{
			std::string PathName = checkFileName( Path.c_str(), FileName.c_str() );

			SetFullFileName( PathName.c_str() );
			SetDefaultFullFileName( PathName.c_str() );

			bDone = GetFileType() != SV_SVZ_CONFIGURATION_FILE_TYPE ;

			if ( ! bDone )
			{
				std::string csNewFullFileName = GetPathName();
				//! Make sure for configurations the extension is svz
				SetExtension(SvDef::cPackedConfigExtension);

				bDone = (0 == SvUl::CompareNoCase(csNewFullFileName, SvStl::GlobalPath::Inst().GetRunPath() ) ) ||
				        ( 0 != _access( GetFullFileName().c_str(), 0 ) );

				
				//If the name of the file changed, because specific special chracters were removed, check if the file with the new name already exists
				//and ask if the user wants to overwrite it, if it does
				if ( ! bDone && PathName != Path)
				{
					csNewFullFileName += _T( "\\" );
					csNewFullFileName += GetFileName();

					SetFullFileName( csNewFullFileName.c_str() );

					bDone = 0 != _access( GetFullFileName().c_str(), 0 );

					if ( ! bDone )
					{
						SvDef::StringVector msgList;
						msgList.push_back(GetFullFileName());
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
						bDone = IDYES == Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_FileName_Exists, msgList, SvStl::SourceFileParams(StdMessageParams), 0, SvDef::InvalidObjectId, MB_YESNO);
					}
				}
				else
				{
					bDone = true;
				}
			}
		}
	}

	return bOk;
}

void SVFileNameClass::Init()
{
	m_FileType = 0;
	m_PathName.clear();
	m_FileName.clear();
	m_FileNameOnly.clear();
	m_Extension.clear();
	m_FileSelectDialogTitle = "Select File";
	m_FileSaveDialogTitle = "Save File";
	m_DefaultFileExtension.clear();
	m_DefaultFileName.clear();
	m_DefaultPathName.clear();
	m_FileSelectFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER;
	m_FileSaveFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER;
	m_FileExtensionFilterList = "All Files (*.*)|*.*||";
}

std::string SVFileNameClass::checkFileName( LPCTSTR PathName, LPCTSTR FileName ) const
{
	std::string Result( PathName );

	if( !m_ExcludeChar.empty() )
	{
		std::string NewFileName( FileName );

		SvUl::RemoveCharacters(NewFileName, m_ExcludeChar.c_str());
		bool NameChanged = NewFileName != FileName;
		
		if( NewFileName.empty() || NewFileName == m_DefaultFileExtension )
		{
			NewFileName = GetDefaultFileName();
			NameChanged = true;
		}

		if( NameChanged )
		{
			SvUl::searchAndReplace( Result, FileName, NewFileName.c_str() );
		}
	}

	return Result;
}
