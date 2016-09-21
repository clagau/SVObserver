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
#include "SVFileNameClass.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVMessage/SVMessage.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVStatusLibrary/GlobalPath.h"
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
	SetDefaultPathName( rhs.m_DefaultPathName.c_str() );
	SetDefaultFileName( rhs.m_DefaultFileName.c_str() );
	SetDefaultFileExtension( rhs.m_DefaultFileExtension.c_str() );
	SetFileSelectFlags( rhs.m_FileSelectFlags );
	SetFileSaveFlags( rhs.m_FileSaveFlags );
	SetFileExtensionFilterList( rhs.m_FileExtensionFilterList.c_str() );
	setExcludeCharacters( rhs.m_ExcludeChar.c_str() );

	return *this;
}

DWORD SVFileNameClass::GetFileType() const
{
	return m_FileType;
}

LPCTSTR SVFileNameClass::GetPathName() const
{
	return m_PathName.c_str();
}

LPCTSTR SVFileNameClass::GetFileName() const
{
	m_FileName = m_FileNameOnly + m_Extension;

	return m_FileName.c_str();
}

LPCTSTR SVFileNameClass::GetFileNameOnly() const
{
	return m_FileNameOnly.c_str();
}

LPCTSTR SVFileNameClass::GetExtension() const
{
	return m_Extension.c_str();
}

LPCTSTR SVFileNameClass::GetFullFileName() const
{
	m_FullFileName.clear();
	
	m_FullFileName = GetFileName();

	if ( !m_PathName.empty() )
	{
		m_FullFileName = m_PathName + "\\" + m_FullFileName;
	}

	return m_FullFileName.c_str();
}

LPCTSTR SVFileNameClass::GetFileSelectDialogTitle() const
{
	return m_FileSelectDialogTitle.c_str();
}

LPCTSTR SVFileNameClass::GetFileSaveDialogTitle() const
{
	return m_FileSaveDialogTitle.c_str();
}

LPCTSTR SVFileNameClass::GetDefaultPathName() const
{
	return m_DefaultPathName.c_str();
}

LPCTSTR SVFileNameClass::GetDefaultFileName() const
{
	return m_DefaultFileName.c_str();
}

LPCTSTR SVFileNameClass::GetDefaultFileExtension() const
{
	return m_DefaultFileExtension.c_str();
}

DWORD SVFileNameClass::GetFileSelectFlags() const
{
	return m_FileSelectFlags;
}

DWORD SVFileNameClass::GetFileSaveFlags() const
{
	return m_FileSaveFlags;
}

LPCTSTR SVFileNameClass::GetFileExtensionFilterList() const
{
	return m_FileExtensionFilterList.c_str();
}

BOOL SVFileNameClass::SetFileType(DWORD dwFileType)
{
	BOOL bOk = false;

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

			bOk = TRUE;
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

			bOk = TRUE;
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
		case SV_SVX_CONFIGURATION_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Configuration File") );
			SetFileSaveDialogTitle( _T( "Save Configuration File" ) );
			SetDefaultFileExtension( _T(".svx") );
			SetDefaultFileName( _T("") );
			SetDefaultPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T( "SVResearch Configuration Files (*.svx)|*.svx||" ) );

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

BOOL SVFileNameClass::SetFullFileName(LPCTSTR szFullName)
{
	if (szFullName)
	{
		SVString sFullName = szFullName;

		if ( sFullName.empty() )
		{
			SetPathName( nullptr );
			SetFileNameOnly( nullptr );
			SetExtension( nullptr );
		}
		else
		{
			SVString sPathName;

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
		return true;
	}
	return false;
}

BOOL SVFileNameClass::SetPathName(LPCTSTR szPathName)
{
	if (szPathName)
	{
		m_PathName = szPathName;

		SvUl_SF::searchAndReplace(m_PathName, "/", "\\");
		SvUl_SF::TrimRight(m_PathName, "\\" );
	}
	else
	{
		m_PathName.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetFileName(LPCTSTR szFileName)
{
	SVString sFullFileName = GetPathName();

	if ( sFullFileName.empty() )
	{
		sFullFileName = szFileName;
	}
	else
	{
		sFullFileName += "\\";
		sFullFileName += szFileName;
	}

	return SetFullFileName( sFullFileName.c_str() );
}

BOOL SVFileNameClass::SetFileNameOnly(LPCTSTR szFileName)
{
	if (szFileName)
	{
		m_FileNameOnly = szFileName;
		SvUl_SF::TrimRight(m_FileNameOnly);
	}
	else
	{
		m_FileNameOnly.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetExtension(LPCTSTR szExtension)
{
	if (szExtension)
	{
		m_Extension = szExtension;
	}
	else
	{
		m_Extension.clear();
	}

	return true;
}

BOOL SVFileNameClass::SetFileSelectDialogTitle(LPCTSTR szTitle)
{
	if (szTitle)
	{
		m_FileSelectDialogTitle = szTitle;
	}
	else
	{
		m_FileSelectDialogTitle.clear();
	}

	return true;
}

BOOL SVFileNameClass::SetFileSaveDialogTitle(LPCTSTR szTitle)
{
	if (szTitle)
	{
		m_FileSaveDialogTitle = szTitle;
	}
	else
	{
		m_FileSaveDialogTitle.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetDefaultFullFileName(LPCTSTR szFullName)
{
	if (szFullName)
	{
		SVString sFullName = szFullName;

		if ( sFullName.empty() )
		{
			SetDefaultPathName( nullptr );
			SetDefaultFileName( nullptr );
			SetDefaultFileExtension( nullptr );
		}
		else
		{
			SVString sPathName;

			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			_splitpath( sFullName.c_str(), drive, dir, fname, ext );

			sPathName = drive;
			sPathName += dir;

			SetDefaultFileName( fname );
			SetDefaultFileExtension( ext );
			SetDefaultPathName( sPathName.c_str() );
		}

		return true;
	}
	return false;
}

BOOL SVFileNameClass::SetDefaultPathName(LPCTSTR szPathName)
{
	if (szPathName)
	{
		m_DefaultPathName = szPathName;

		SvUl_SF::searchAndReplace(m_DefaultPathName, "/", "\\");
		SvUl_SF::TrimRight(m_DefaultPathName, "\\" );
	}
	else
	{
		m_DefaultPathName.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetDefaultFileName(LPCTSTR szName)
{
	if (szName)
	{
		m_DefaultFileName = szName;
	}
	else
	{
		m_DefaultFileName.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetDefaultFileExtension(LPCTSTR szExtension)
{
	if (szExtension)
	{
		m_DefaultFileExtension = szExtension;
	}
	else
	{
		m_DefaultFileExtension.clear();
	}
	return true;
}

BOOL SVFileNameClass::SetFileSelectFlags(DWORD dwFlags)
{
	m_FileSelectFlags = dwFlags;

	return true;
}

BOOL SVFileNameClass::SetFileSaveFlags(DWORD dwFlags)
{
	m_FileSaveFlags = dwFlags;

	return true;
}

BOOL SVFileNameClass::SetFileExtensionFilterList(LPCTSTR szFilter)
{
	if (szFilter)
	{
		m_FileExtensionFilterList = szFilter;
	}
	else
	{
		m_FileExtensionFilterList .clear();
	}
	return true;
}

BOOL SVFileNameClass::SelectPath()
{
	BOOL bOk = false;
	SVString strPath;

	SVString initialDir = GetPathName();
	if (initialDir.empty())
	{
		initialDir = GetDefaultPathName();
	}
	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVDlgFolder dlg(bFullAccess, initialDir.c_str());

	dlg.InitDlgFolder(_T("Ok"), _T("Select Folder"));
	if ( IDOK == dlg.DoModal() )
	{
		strPath = dlg.GetPathName();
		bOk = SetPathName( strPath.c_str() );
		SetDefaultPathName( strPath.c_str() );
	}
	return bOk;
}

BOOL SVFileNameClass::SelectFile()
{
	BOOL bOk = false;

	if ( SVString( GetDefaultPathName() ).empty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( SVString( GetDefaultFileName() ).empty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( SVString( GetDefaultFileExtension() ).empty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}

	SVString sFileName = GetDefaultFileName();
    if (!sFileName.empty())
	{
	    sFileName += GetDefaultFileExtension();
	}
	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVFileDialog dlg( true,
					bFullAccess,  
	                GetDefaultFileExtension(),
					sFileName.c_str(), 
					GetFileSelectFlags(),
					GetFileExtensionFilterList(),
					nullptr );

	dlg.m_ofn.lpstrTitle = GetFileSelectDialogTitle();

	if ( SVString( GetPathName() ).empty() )
	{
		dlg.m_ofn.lpstrInitialDir = GetDefaultPathName();
	}
	else
	{
		dlg.m_ofn.lpstrInitialDir = GetPathName();
	}

	if ( IDOK == dlg.DoModal() )
	{
		bOk = SetFullFileName( dlg.GetPathName() );
		
		SetDefaultFullFileName( dlg.GetPathName() );
	}

	return bOk;
}

BOOL SVFileNameClass::SaveFile()
{
	BOOL bOk = false;
	BOOL bDone = false;

	if ( SVString( GetDefaultPathName() ).empty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( SVString( GetDefaultFileName() ).empty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( SVString( GetDefaultFileExtension() ).empty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}

	SVString csFileName = GetDefaultFileName();
	csFileName += GetDefaultFileExtension();

	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVFileDialog dlg( false, 
					 bFullAccess,
	                 GetDefaultFileExtension(),
					 csFileName.c_str(), 
					 GetFileSaveFlags(),
					 GetFileExtensionFilterList(),
					 nullptr );

	dlg.m_ofn.lpstrTitle = GetFileSaveDialogTitle();

	while ( ! bDone )
	{
		if ( SVString( GetPathName() ).empty() )
		{
			dlg.m_ofn.lpstrInitialDir = GetDefaultPathName();
		}
		else
		{
			dlg.m_ofn.lpstrInitialDir = GetPathName();
		}

		bOk = IDOK == dlg.DoModal();

		bDone = ! bOk;

		if ( ! bDone )
		{
			SVString PathName = checkFileName( dlg.GetPathName(), dlg.GetFileName() );

			bOk = SetFullFileName( PathName.c_str() );
			SetDefaultFullFileName( PathName.c_str() );

			bDone = GetFileType() != SV_SVX_CONFIGURATION_FILE_TYPE ;

			if ( ! bDone )
			{
				SVString csNewFullFileName = GetPathName();

				if ( 0 != SvUl_SF::CompareNoCase(SVString( GetExtension() ), _T( ".svx" ) ) )
				{
					SetExtension( _T( ".svx" ) );
				}

				bDone = (0 == SvUl_SF::CompareNoCase(csNewFullFileName, SvStl::GlobalPath::Inst().GetRunPath() ) ) ||
				        ( 0 == _access( GetFullFileName(), 0 ) );

				if ( ! bDone )
				{
					csNewFullFileName += _T( "\\" );
					csNewFullFileName += GetFileNameOnly();
					csNewFullFileName += _T( "\\" );
					csNewFullFileName += GetFileName();

					bOk = SetFullFileName( csNewFullFileName.c_str() );

					bDone = 0 != _access( GetFullFileName(), 0 );

					if ( ! bDone )
					{
						SVString csMessage = GetFullFileName();
						csMessage += _T( " already exists.\nDo you want to replace it?" );

						bDone = IDYES == dlg.MessageBox( csMessage.c_str(), GetFileSaveDialogTitle(), MB_ICONWARNING | MB_YESNO );
					}
				}
			}
		}
	}

	return bOk;
}

void SVFileNameClass::Init()
{
	m_FileType = 0;
	m_FullFileName.clear();
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

SVString SVFileNameClass::checkFileName( LPCTSTR PathName, LPCTSTR FileName ) const
{
	SVString Result( PathName );

	if( !m_ExcludeChar.empty() )
	{
		bool NameChanged( false );
		SVString NewFileName( FileName );

		SvUl_SF::RemoveCharacters(NewFileName, m_ExcludeChar.c_str());
		NameChanged = NewFileName == FileName;
		
		if( NewFileName.empty() || NewFileName == m_DefaultFileExtension )
		{
			NewFileName = GetDefaultFileName();
			NameChanged = true;
		}

		if( NameChanged )
		{
			SvUl_SF::searchAndReplace( Result, FileName, NewFileName.c_str() );
		}
	}

	return Result;
}
