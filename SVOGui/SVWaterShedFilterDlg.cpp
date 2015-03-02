// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWatershedFilterDlg
// * .File Name       : $Workfile:   SVWaterShedFilterDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   15 May 2014 15:01:46  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxLibrary\SVMatroxEnums.h"
#include "SVWatershedFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;

namespace Seidenader
{
	namespace SVOGui
	{

		SVWatershedFilterDlg::SVWatershedFilterDlg(SvOi::IWatershedFilter& rFilterClass, CWnd* pParent): 
				CDialog(SVWatershedFilterDlg::IDD, pParent)
				,m_rFilterClass(rFilterClass)
	{

		//{{AFX_DATA_INIT(SVWatershedFilterDlg)
		m_bUseBasin = FALSE;
		m_bSkipLastLevel = FALSE;
		m_lMinVariation = 0;
		m_bUseWatershed = FALSE;
		m_iEightWatershedLines = -1;
		m_iMinFillBasin = -1;
		m_iRegularWatershedLines = -1;
		m_iVariationType = -1;
		m_bUseMarker = FALSE;
		//}}AFX_DATA_INIT
	}

	HRESULT SVWatershedFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		long lControlFlag;
		long lMinVariation;

		lControlFlag =0;
		lControlFlag |= m_bSkipLastLevel ? SVImageSkipLastLevel : 0;
		lControlFlag |= m_bUseBasin ? SVImageWSBasin : 0;
		lControlFlag |= m_bUseWatershed ? SVImageWSWatershed : 0;

		lControlFlag |= m_iEightWatershedLines ? SVImage8Connected : 0;
		lControlFlag |= m_iMinFillBasin ? SVImageWSMaximaFill  : 0;
		lControlFlag |= m_iRegularWatershedLines ? SVImageWSStraight : 0;

		lMinVariation = m_iVariationType < 2 ? m_iVariationType : m_lMinVariation;

		l_hrOk = m_rFilterClass.addControlFlagRequest( lControlFlag );

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_rFilterClass.addMinVariationRequest( m_lMinVariation );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_rFilterClass.addMarkerUsedRequest( TRUE == m_bUseMarker );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_rFilterClass.AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = m_rFilterClass.RunOnce( m_rFilterClass.GetToolInterface() );
		}

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVWatershedFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVWatershedFilterDlg)
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_SVSourceImageCombo);
		DDX_Check(pDX, IDC_BASIN_CHECK, m_bUseBasin);
		DDX_Check(pDX, IDC_SKIP_LAST_CHECK, m_bSkipLastLevel);
		DDX_Text(pDX, IDC_VARIATION_EDIT, m_lMinVariation);
		DDX_Check(pDX, IDC_WATERSHED_CHECK, m_bUseWatershed);
		DDX_Radio(pDX, IDC_4CON_LINES_RADIO, m_iEightWatershedLines);
		DDX_Radio(pDX, IDC_MIN_FILL_RADIO, m_iMinFillBasin);
		DDX_Radio(pDX, IDC_REGULAR_RADIO, m_iRegularWatershedLines);
		DDX_Radio(pDX, IDC_OFF_VAR_RADIO, m_iVariationType);
		DDX_Check(pDX, IDC_MARKER_CHECK, m_bUseMarker);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(SVWatershedFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVWatershedFilterDlg)
		ON_BN_CLICKED(IDC_OFF_VAR_RADIO, OnSETVarRadio)
		ON_BN_CLICKED(IDC_MIN_VAR_RADIO, OnSETVarRadio)
		ON_BN_CLICKED(IDC_SET_VAR_RADIO, OnSETVarRadio)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVWatershedFilterDlg message handlers

	BOOL SVWatershedFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		long lControlFlag;
		long lMinVariation;

		lMinVariation = m_rFilterClass.getMinVariation();
		lControlFlag = m_rFilterClass.getControlFlag();

		// Check Boxes
		m_bSkipLastLevel = (lControlFlag & SVImageSkipLastLevel) == SVImageSkipLastLevel;
		m_bUseBasin = (lControlFlag & SVImageWSBasin) == SVImageWSBasin;
		m_bUseWatershed = (lControlFlag & SVImageWSWatershed) == SVImageWSWatershed;
		m_bUseMarker = m_rFilterClass.isMarkerUsed();

		// Radios
		m_iEightWatershedLines = (lControlFlag & SVImage8Connected)  == SVImage8Connected ;
		m_iMinFillBasin = (lControlFlag & SVImageWSMaximaFill  )  == SVImageWSMaximaFill   ;
		m_iRegularWatershedLines = (lControlFlag & SVImageWSStraight  )  == SVImageWSStraight;

		m_iVariationType = lMinVariation < 2 ? lMinVariation : 2 ;

		// Edit Box
		m_lMinVariation = lMinVariation;

		GetDlgItem( IDC_VARIATION_EDIT )->EnableWindow( m_iVariationType == 2 );

		std::vector<SVString> availableImages = m_rFilterClass.getAvailableMarkerImageNames();
		for (int i=0; i<availableImages.size(); ++i)
		{
			m_SVSourceImageCombo.AddString(availableImages[i].c_str());
		}

		SVString currentMarkerImageName = m_rFilterClass.getMarkerImage();
		m_SVSourceImageCombo.SelectString(-1, currentMarkerImageName.c_str());

		UpdateData( FALSE ); // set data to dialog

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}



	void SVWatershedFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVWatershedFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}


	void SVWatershedFilterDlg::OnSETVarRadio() 
	{
		UpdateData();
		GetDlgItem( IDC_VARIATION_EDIT )->EnableWindow( m_iVariationType == 2 );
		if( m_iVariationType < 2 )
		{
			m_lMinVariation = m_iVariationType;
			UpdateData(FALSE);
		}
	}


	void SVWatershedFilterDlg::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int iCurSel = m_SVSourceImageCombo.GetCurSel();
		CString currentImageName;
		m_SVSourceImageCombo.GetLBText(iCurSel, currentImageName);

		if( !currentImageName.IsEmpty() )
		{
			m_rFilterClass.setMarkerImage(currentImageName);
		}
	}
	}  //end namespace SVOGUI
}  //end namespace Seidenader
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVWaterShedFilterDlg.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 15:01:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:30:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Jul 2012 13:08:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   27 Jul 2012 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Jul 2012 17:52:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2012 14:28:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Mar 2011 14:01:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Feb 2011 12:27:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Jul 2007 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Sep 2005 14:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used new method in SVTaskObjectClass to handle the image connections properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2005 09:59:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:53:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 15:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new Reset Methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:01:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Watershed Filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
