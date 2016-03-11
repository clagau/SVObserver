//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSaveToolSetImageDialog
//* .File Name       : $Workfile:   SVSaveToolSetImageDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:14:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSaveToolSetImageDialog.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Seidenader { namespace SVOGui
{
	SVSaveToolSetImageDialogClass::SVSaveToolSetImageDialogClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=NULL*/)
	: CDialog(SVSaveToolSetImageDialogClass::IDD, pParent)
	, m_ImageController(rInspectionID, rTaskObjectID)
	{
		//{{AFX_DATA_INIT(SVSaveToolSetImageDialogClass)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}

	void SVSaveToolSetImageDialogClass::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVSaveToolSetImageDialogClass)
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_currentSelectedImageCtrl);
		DDX_Control(pDX, IDC_AVAILABLE_IMAGES_COMBO, m_availableImagesComboCtrl);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVSaveToolSetImageDialogClass, CDialog)
		//{{AFX_MSG_MAP(SVSaveToolSetImageDialogClass)
		ON_BN_CLICKED(ID_SAVE_BUTTON, OnSaveButton)
		ON_CBN_SELCHANGE(IDC_AVAILABLE_IMAGES_COMBO, OnSelchangeAvailableImagesCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVSaveToolSetImageDialogClass message handlers

	BOOL SVSaveToolSetImageDialogClass::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		m_ImageController.Init();

		const SvUl::NameGuidList& availImages = m_ImageController.GetAvailableImageList();
		for (SvUl::NameGuidList::const_iterator it = availImages.begin();it != availImages.end();++it)
		{
			m_availableImagesComboCtrl.AddString(it->first.c_str());
		}
		m_availableImagesComboCtrl.SetCurSel(0);
		m_currentSelectedImageCtrl.AddTab(_T("Image")); 
		setImages();

		UpdateData( FALSE );
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVSaveToolSetImageDialogClass::OnSelchangeAvailableImagesCombo() 
	{
		UpdateData(true);	

		setImages();

		UpdateData(false);
	}

	void SVSaveToolSetImageDialogClass::OnSaveButton() 
	{
		UpdateData(true);

		int index = m_availableImagesComboCtrl.GetCurSel();
		if (index != CB_ERR)
		{
			CString imageName;
			m_availableImagesComboCtrl.GetLBText(index, imageName);
			if (!imageName.IsEmpty())
			{
				SVFileNameClass	svfncImageFile;
			
				CString csPathPlusFileName = _T("");
				CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"), _T("ImagesFilePath"), _T("C:\\Images"));	// Default
			
				svfncImageFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
				svfncImageFile.SetPathName(csPath);
			
				BOOL bResult = svfncImageFile.SaveFile(); // Show Save File Dialog
			
				if (bResult)
				{
					csPath = svfncImageFile.GetPathName();
					AfxGetApp()->WriteProfileString(_T("Settings"), _T("ImagesFilePath"), csPath);
				
					csPathPlusFileName = svfncImageFile.GetFullFileName();
					HRESULT hr = m_ImageController.SaveImage(SVString(imageName), SVString(csPathPlusFileName));
					if (S_OK != hr)
					{
						if (E_INVALIDARG == hr)
						{
							AfxMessageBox(IDS_UNKNOWN_FILE_FORMAT_STRING);
						}
						else
						{
							AfxMessageBox(_T("Unable to Save Image"));
						}
					}
				}
			}
		}	
	}

	void SVSaveToolSetImageDialogClass::setImages()
	{
		int index = m_availableImagesComboCtrl.GetCurSel();

		// Display current selected image...
		if (index != CB_ERR)
		{
			CString name;
			m_availableImagesComboCtrl.GetLBText(index, name);
		
			IPictureDisp* pImage = m_ImageController.GetImage(SVString(name));
			m_currentSelectedImageCtrl.setImage(pImage);
			m_currentSelectedImageCtrl.Refresh();
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSaveToolSetImageDialog.cpp_v  $
 * 
 *    Rev 1.2   26 Jun 2014 18:14:58   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2014 13:48:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:02:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   30 Jul 2012 13:05:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   27 Jul 2012 09:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   02 Jul 2012 17:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   07 Apr 2011 16:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   10 Feb 2011 15:13:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   27 Jan 2011 11:58:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   08 Dec 2010 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   16 Dec 2009 12:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   25 Jul 2007 07:48:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 15:16:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 15:12:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   30 Jan 2003 16:30:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnSaveButton method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   13 Jun 2001 10:02:52   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised file header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   06 Feb 2001 13:42:56   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.0   16 May 2001 14:11:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:37:32   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 23 1999 09:00:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New Provides Dialog to save any image from ToolSet..
 * Replaces old Save Image in IPDoc::OnFileSaveImage().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
