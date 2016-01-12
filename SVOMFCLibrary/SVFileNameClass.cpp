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
#include <io.h>
#include "SVFileNameClass.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVMessage/SVMessage.h"
#include "SVMFCControls/SVDlgFolder.h"
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
	SetPathName( rhs.m_PathName );
	SetFileNameOnly( rhs.m_FileNameOnly );
	SetExtension( rhs.m_Extension );
	SetFileSelectDialogTitle( rhs.m_FileSelectDialogTitle );
	SetFileSaveDialogTitle( rhs.m_FileSaveDialogTitle );
	SetDefaultPathName( rhs.m_DefaultPathName );
	SetDefaultFileName( rhs.m_DefaultFileName );
	SetDefaultFileExtension( rhs.m_DefaultFileExtension );
	SetFileSelectFlags( rhs.m_FileSelectFlags );
	SetFileSaveFlags( rhs.m_FileSaveFlags );
	SetFileExtensionFilterList( rhs.m_FileExtensionFilterList );
	setExcludeCharacters( rhs.m_ExcludeChar );

	return *this;
}

DWORD SVFileNameClass::GetFileType() const
{
	return m_FileType;
}

LPCTSTR SVFileNameClass::GetPathName() const
{
	return m_PathName;
}

LPCTSTR SVFileNameClass::GetFileName()
{
	m_FileName = m_FileNameOnly + m_Extension;

	return m_FileName;
}

LPCTSTR SVFileNameClass::GetFileNameOnly() const
{
	return m_FileNameOnly;
}

LPCTSTR SVFileNameClass::GetExtension() const
{
	return m_Extension;
}

LPCTSTR SVFileNameClass::GetFullFileName()
{
	m_FullFileName.Empty();
	
	m_FullFileName = GetFileName();

	if ( !m_PathName.IsEmpty() )
	{
		m_FullFileName = m_PathName + "\\" + m_FullFileName;
	}

	return m_FullFileName;
}

LPCTSTR SVFileNameClass::GetFileSelectDialogTitle() const
{
	return m_FileSelectDialogTitle;
}

LPCTSTR SVFileNameClass::GetFileSaveDialogTitle() const
{
	return m_FileSaveDialogTitle;
}

LPCTSTR SVFileNameClass::GetDefaultPathName() const
{
	return m_DefaultPathName;
}

LPCTSTR SVFileNameClass::GetDefaultFileName() const
{
	return m_DefaultFileName;
}

LPCTSTR SVFileNameClass::GetDefaultFileExtension() const
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

LPCTSTR SVFileNameClass::GetFileExtensionFilterList() const
{
	return m_FileExtensionFilterList;
}

BOOL SVFileNameClass::SetFileType(DWORD dwFileType)
{
	BOOL bOk = FALSE;

	m_FileType = dwFileType;

	switch (m_FileType)
	{
		case SV_DEFAULT_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select File") );
			SetFileSaveDialogTitle( _T("Save File") );
			SetDefaultFileExtension( nullptr );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("All Files (*.*)|*.*||") );

			bOk = TRUE;
			break;
		}
		case SV_CAMERA_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Camera File") );
			SetFileSaveDialogTitle( _T("Save Camera File") );
			SetDefaultFileExtension( _T(".cca") );
			SetDefaultFileName( nullptr );
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
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\IMAGES") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Bitmap File Format (*.bmp)|*.bmp|MIL Image Format File (*.mim)|*.mim|TIFF Image Format (*.tif)|*.tif||") );

			bOk = TRUE;
			break;
		}
		case SV_PATTERN_MODEL_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Model File") );
			SetFileSaveDialogTitle( _T("Save Model File") );
			SetDefaultFileExtension( _T(".bmp") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( "Bitmap File Format (*.bmp)|*.bmp|MIL Image Format File (*.mim)|*.mim|TIFF Image Format (*.tif)|*.tif||" );

			bOk = TRUE;
			break;
		}
		case SV_MASK_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select SVObserver Mask File") );
			SetFileSaveDialogTitle( _T("Save SVObserver Mask File") );
			SetDefaultFileExtension( _T(".svm") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("SVObserver Mask Files (*.svm)|*.svm||") );

			bOk = TRUE;
			break;
		}
 		case SV_BAR_CODE_MATCH_STRING_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Bar Code Match String File") );
			SetFileSaveDialogTitle( _T("Save Bar Code Match String File") );
			SetDefaultFileExtension( _T(".txt") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||") );

			bOk = TRUE;
			break;
		}
		case SV_BAR_CODE_STORE_VALUE_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select File") );
			SetFileSaveDialogTitle( _T("Save Bar Code Value File") );
			SetDefaultFileExtension( _T(".txt") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
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
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||") );

			bOk = TRUE;
			break;
		}
		case SV_OCR_FONT_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select WiT Font Training File") );
			SetFileSaveDialogTitle( _T("Save WiT Font Training File") );
			SetDefaultFileExtension( _T(".wit") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T("WiT Font Files (*.wit)|*.wit||") );

			bOk = TRUE;
			break;
		}
		case SV_SVX_CONFIGURATION_FILE_TYPE:
		{
			SetFileSelectDialogTitle( _T("Select Configuration File") );
			SetFileSaveDialogTitle( _T( "Save Configuration File" ) );
			SetDefaultFileExtension( _T(".svx") );
			SetDefaultFileName( nullptr );
			SetDefaultPathName( _T("C:\\RUN") );
			SetFileSelectFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileSaveFlags( OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER );
			SetFileExtensionFilterList( _T( "SVResearch Configuration Files (*.svx)|*.svx||" ) );

			bOk = TRUE;
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
	CString csFullName = szFullName;

	if ( csFullName.IsEmpty() )
	{
		SetPathName( nullptr );
		SetFileNameOnly( nullptr );
		SetExtension( nullptr );
	}
	else
	{
		CString csPathName;

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( csFullName, drive, dir, fname, ext );

		csPathName = drive;
		csPathName += dir;

		SetFileNameOnly( fname );
		SetExtension( ext );
		SetPathName( csPathName );
	}

	return TRUE;
}

BOOL SVFileNameClass::SetPathName(LPCTSTR szPathName)
{
	m_PathName = szPathName;

	m_PathName.Replace('/','\\');
	m_PathName.TrimRight( '\\' );

	return TRUE;
}

BOOL SVFileNameClass::SetFileName(LPCTSTR szFileName)
{
	CString csFullFileName = GetPathName();

	if ( csFullFileName.IsEmpty() )
	{
		csFullFileName = szFileName;
	}
	else
	{
		csFullFileName += "\\";
		csFullFileName += szFileName;
	}

	return SetFullFileName( csFullFileName );
}

BOOL SVFileNameClass::SetFileNameOnly(LPCTSTR szFileName)
{
	m_FileNameOnly = szFileName;
	m_FileNameOnly.TrimRight();

	return TRUE;
}

BOOL SVFileNameClass::SetExtension(LPCTSTR szExtension)
{
	m_Extension = szExtension;

	return TRUE;
}

BOOL SVFileNameClass::SetFileSelectDialogTitle(LPCTSTR szTitle)
{
	m_FileSelectDialogTitle = szTitle;

	return TRUE;
}

BOOL SVFileNameClass::SetFileSaveDialogTitle(LPCTSTR szTitle)
{
	m_FileSaveDialogTitle = szTitle;

	return TRUE;
}

BOOL SVFileNameClass::SetDefaultFullFileName(LPCTSTR szFullName)
{
	CString csFullName = szFullName;

	if ( csFullName.IsEmpty() )
	{
		SetDefaultPathName( nullptr );
		SetDefaultFileName( nullptr );
		SetDefaultFileExtension( nullptr );
	}
	else
	{
		CString csPathName;

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( csFullName, drive, dir, fname, ext );

		csPathName = drive;
		csPathName += dir;

		SetDefaultFileName( fname );
		SetDefaultFileExtension( ext );
		SetDefaultPathName( csPathName );
	}

	return TRUE;
}

BOOL SVFileNameClass::SetDefaultPathName(LPCTSTR szPathName)
{
	m_DefaultPathName = szPathName;

	m_DefaultPathName.Replace('/','\\');
	m_DefaultPathName.TrimRight( '\\' );

	return TRUE;
}

BOOL SVFileNameClass::SetDefaultFileName(LPCTSTR szName)
{
	m_DefaultFileName = szName;

	return TRUE;
}

BOOL SVFileNameClass::SetDefaultFileExtension(LPCTSTR szExtension)
{
	m_DefaultFileExtension = szExtension;

	return TRUE;
}

BOOL SVFileNameClass::SetFileSelectFlags(DWORD dwFlags)
{
	m_FileSelectFlags = dwFlags;

	return TRUE;
}

BOOL SVFileNameClass::SetFileSaveFlags(DWORD dwFlags)
{
	m_FileSaveFlags = dwFlags;

	return TRUE;
}

BOOL SVFileNameClass::SetFileExtensionFilterList(LPCTSTR szFilter)
{
	m_FileExtensionFilterList = szFilter;

	return TRUE;
}

BOOL SVFileNameClass::SelectPath()
{
	BOOL bOk = FALSE;
	CString		strPath;

	CString initialDir = GetPathName();
	if (initialDir.IsEmpty())
	{
		initialDir = GetDefaultPathName();
	}
	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVDlgFolder dlg(bFullAccess, initialDir);

	dlg.InitDlgFolder(_T("Ok"), _T("Select Folder"));
	if ( dlg.DoModal() == IDOK )
	{
		strPath = dlg.GetPathName();
		bOk = SetPathName( strPath );
		SetDefaultPathName( strPath );
	}
	return bOk;
}

BOOL SVFileNameClass::SelectFile()
{
	BOOL bOk = FALSE;

	if ( CString( GetDefaultPathName() ).IsEmpty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( CString( GetDefaultFileName() ).IsEmpty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( CString( GetDefaultFileExtension() ).IsEmpty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}

	CString csFileName = GetDefaultFileName();
    if (!csFileName.IsEmpty())
	{
	    csFileName += GetDefaultFileExtension();
	}
	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVFileDialog dlg( true,
					bFullAccess,  
	                GetDefaultFileExtension(),
					csFileName, 
					GetFileSelectFlags(),
					GetFileExtensionFilterList(),
					nullptr );

	dlg.m_ofn.lpstrTitle = GetFileSelectDialogTitle();

	if ( CString( GetPathName() ).IsEmpty() )
	{
		dlg.m_ofn.lpstrInitialDir = GetDefaultPathName();
	}
	else
	{
		dlg.m_ofn.lpstrInitialDir = GetPathName();
	}

	if ( dlg.DoModal() == IDOK )
	{
		bOk = SetFullFileName( dlg.GetPathName() );
		
		SetDefaultFullFileName( dlg.GetPathName() );
	}

	return bOk;
}

BOOL SVFileNameClass::SaveFile()
{
	BOOL bOk = FALSE;
	BOOL bDone = FALSE;

	if ( CString( GetDefaultPathName() ).IsEmpty() )
	{
		SetDefaultPathName( GetPathName() );
	}

	if ( CString( GetDefaultFileName() ).IsEmpty() )
	{
		SetDefaultFileName( GetFileNameOnly() );
	}

	if ( CString( GetDefaultFileExtension() ).IsEmpty() )
	{
		SetDefaultFileExtension( GetExtension() );
	}

	CString csFileName = GetDefaultFileName();
	csFileName += GetDefaultFileExtension();

	bool bFullAccess = SvOi::isUnrestrictedFileAccess();
	SvMc::SVFileDialog dlg( false, 
					 bFullAccess,
	                 GetDefaultFileExtension(),
					 csFileName, 
					 GetFileSaveFlags(),
					 GetFileExtensionFilterList(),
					 nullptr );

	dlg.m_ofn.lpstrTitle = GetFileSaveDialogTitle();

	while ( ! bDone )
	{
		if ( CString( GetPathName() ).IsEmpty() )
		{
			dlg.m_ofn.lpstrInitialDir = GetDefaultPathName();
		}
		else
		{
			dlg.m_ofn.lpstrInitialDir = GetPathName();
		}

		bOk = dlg.DoModal() == IDOK ;

		bDone = ! bOk;

		if ( ! bDone )
		{
			CString PathName = checkFileName( dlg.GetPathName(), dlg.GetFileName() );

			bOk = SetFullFileName( PathName );
			SetDefaultFullFileName( PathName );

			bDone = GetFileType() != SV_SVX_CONFIGURATION_FILE_TYPE ;

			if ( ! bDone )
			{
				CString csNewFullFileName = GetPathName();

				if ( CString( GetExtension() ).CompareNoCase( _T( ".svx" ) ) != 0 )
				{
					SetExtension( _T( ".svx" ) );
				}

				bDone = csNewFullFileName.CompareNoCase( _T( "C:\\RUN" ) ) == 0 ||
					      _access( GetFullFileName(), 0 ) == 0;

				if ( ! bDone )
				{
					csNewFullFileName += _T( "\\" );
					csNewFullFileName += GetFileNameOnly();
					csNewFullFileName += _T( "\\" );
					csNewFullFileName += GetFileName();

					bOk = SetFullFileName( csNewFullFileName );

					bDone = _access( GetFullFileName(), 0 ) != 0;

					if ( ! bDone )
					{
						CString csMessage = GetFullFileName();
						csMessage += _T( " already exists.\nDo you want to replace it?" );

						bDone = dlg.MessageBox( csMessage, GetFileSaveDialogTitle(),
																		MB_ICONWARNING | MB_YESNO ) == IDYES;
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
	m_FullFileName.Empty();
	m_PathName.Empty();
	m_FileName.Empty();
	m_FileNameOnly.Empty();
	m_Extension.Empty();
	m_FileSelectDialogTitle = "Select File";
	m_FileSaveDialogTitle = "Save File";
	m_DefaultFileExtension.Empty();
	m_DefaultFileName.Empty();
	m_DefaultPathName.Empty();
	m_FileSelectFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER;
	m_FileSaveFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER;
	m_FileExtensionFilterList = "All Files (*.*)|*.*||";
}

CString SVFileNameClass::checkFileName( LPCTSTR PathName, LPCTSTR FileName ) const
{
	CString Result( PathName );

	if( !m_ExcludeChar.IsEmpty() )
	{
		bool NameChanged( false );
		CString NewFileName( FileName );

		for( int i=0; i < m_ExcludeChar.GetLength(); i++ )
		{
			if( 0 < NewFileName.Remove( m_ExcludeChar[i] ) )
			{
				NameChanged =  true;
			}
		}

		if( NewFileName.IsEmpty() || NewFileName == m_DefaultFileExtension )
		{
			NewFileName = GetDefaultFileName();
			NameChanged = true;
		}

		if( NameChanged )
		{
			Result.Replace( FileName, NewFileName );
		}
	}

	return Result;
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFileNameClass.cpp_v  $
 * 
 *    Rev 1.3   18 Sep 2014 13:37:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SV_BAR_CODE_STORE_VALUE_FILE_TYPE
 * Revised to use SVLibrary/SVFileDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Mar 2014 11:06:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  856
 * SCR Title:  Fix Bug with Saving config with space at the end of the name
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed white space at the end of the filename in the SetFileNameOnly function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Feb 2014 13:30:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Remove SEC case form method SetFileType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:36:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jun 2007 12:50:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 09:33:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Nov 2002 13:03:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SetFileType
 * Added support for SVX
 *             SelectFile
 *                         Don’t add extension for empty filename
 *             SaveFile
 *                         Changed to save SVX files only
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jul 2001 20:58:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  217
 * SCR Title:  Fix Dr. Watson errors in Patch Match Tool
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated default file name extension data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Jun 2001 14:32:20   ed
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
 *    Rev 1.4   19 Apr 2001 14:04:58   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Apr 2001 15:04:18   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Updated SelectPath member function to use CDlgFolder class instead of CFileDialog for selecting folders.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Jan 2001 16:16:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jan 2001 10:39:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixes return value for full file name method.  Fixes initial path and title for file dialog methods.
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
