//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteIOAdjustDialog
//* .File Name       : $Workfile:   SVRemoteIOAdjustDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:49:12  $
//******************************************************************************

#ifndef SVREMOTEIOADJUSTDIALOG_H
#define SVREMOTEIOADJUSTDIALOG_H

class SVRemoteIOAdjustDialog : public CDialog
{
	DECLARE_DYNAMIC(SVRemoteIOAdjustDialog)

public:
	SVRemoteIOAdjustDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVRemoteIOAdjustDialog();

	enum { IDD = IDD_REMOTE_IOADJUST_DIALOG };

	void SetIOName( LPCTSTR p_szName );

	void SetIOValue( const _variant_t& p_rValue );
	void GetIOValue( _variant_t& p_rValue ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	CStatic m_IOName;
	CEdit m_ValueEditControl;

	CString m_Name;
	_variant_t m_Value;
	double m_EditValue;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteIOAdjustDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:49:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Oct 2010 11:20:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.  Added new dialog to edit the Remote Input Value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/