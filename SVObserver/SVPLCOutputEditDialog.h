//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputEditDialog
//* .File Name       : $Workfile:   SVPLCOutputEditDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:30:56  $
//******************************************************************************

#ifndef SVPLCOUTPUTEDITDIALOG_H
#define SVPLCOUTPUTEDITDIALOG_H

#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"
#include "SVInfoStructs.h"

class SVPLCOutputObject;

class SVPLCOutputEditDialog : public CDialog
{
	DECLARE_DYNAMIC(SVPLCOutputEditDialog)

	public:
	enum { IDD = IDD_PLC_OUTPUT_EDIT };

	public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeValueObjectNameCombo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	SVPLCOutputEditDialog(CWnd* pParent = NULL);
	virtual ~SVPLCOutputEditDialog();

	bool GetConvert();
	void SetConvert( bool p_bConvert );
	bool GetUseBitSize();
	void SetUseBitSize( bool p_bUseBitSize );

	GUID m_InputObjectGUID;
	CString m_strOutputSourceName;

	long     m_lSize;
	long     m_lElement;
	long     m_lOriginalIndex;

	CString  m_strPLCId;
	bool     m_bDisableElement;
	bool     m_bDisableObject;
	bool     m_bNewObject;

	boost::function<HRESULT (CString, GUID, long, long, long, long )> m_validateFunc;
	boost::function<long (CString, SVObjectTypeEnum)> m_CalcNewSizeCallBack;

	CComboBox m_ValueObjectNameCombo;

	long m_lBitNum;
	SVIOEntryHostStructPtr m_TriggerCount;
	BOOL m_bUseBitSize;

protected:
	void UpdateValueObjectFromCombo();

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;

};

#endif // SVPLCOUTPUTEDITDIALOG_H


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCOutputEditDialog.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:30:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Apr 2013 14:19:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:26:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2011 16:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2010 13:25:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

