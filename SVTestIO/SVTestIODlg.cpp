//******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestIODlg
//* .File Name       : $Workfile:   SVTestIODlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
//******************************************************************************

#include "stdafx.h"
#include <functional>
#include <algorithm>
#include "SVTestIO.h"
#include "SVTestIODlg.h"

class SVTestIOTriggerFind : public std::binary_function<SVTestIOTriggerStruct, SVTestIOCallbackPtr, bool>
{
public:
	bool operator()(const SVTestIOTriggerStruct& triggerStruct, const SVTestIOCallbackPtr& pCallback) const
	{
		return (triggerStruct.pCallback == pCallback);
	}
};


// SVTestIODlg dialog

IMPLEMENT_DYNAMIC(SVTestIODlg, CDialog)

SVTestIODlg::SVTestIODlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVTestIODlg::IDD, pParent)
	, m_lInputs(0)
	, m_bResetStart(true)
{
	::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 10000);
	m_pCallBacks.resize(4);
}

SVTestIODlg::~SVTestIODlg()
{
	::DeleteCriticalSection( &m_CriticalSection );
}

void SVTestIODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT1, m_Input1);
	DDX_Control(pDX, IDC_INPUT2, m_Input2);
	DDX_Control(pDX, IDC_INPUT3, m_Input3);
	DDX_Control(pDX, IDC_INPUT4, m_Input4);
	DDX_Control(pDX, IDC_INPUT5, m_Input5);
	DDX_Control(pDX, IDC_INPUT6, m_Input6);
	DDX_Control(pDX, IDC_INPUT7, m_Input7);
	DDX_Control(pDX, IDC_INPUT8, m_Input8);
	DDX_Control(pDX, IDC_OUTPUT1, m_Output1);
	DDX_Control(pDX, IDC_OUTPUT2, m_Output2);
	DDX_Control(pDX, IDC_OUTPUT3, m_Output3);
	DDX_Control(pDX, IDC_OUTPUT4, m_Output4);
	DDX_Control(pDX, IDC_OUTPUT5, m_Output5);
	DDX_Control(pDX, IDC_OUTPUT6, m_Output6);
	DDX_Control(pDX, IDC_OUTPUT7, m_Output7);
	DDX_Control(pDX, IDC_OUTPUT8, m_Output8);
	DDX_Control(pDX, IDC_OUTPUT9, m_Output9);
	DDX_Control(pDX, IDC_OUTPUT10, m_Output10);
	DDX_Control(pDX, IDC_OUTPUT11, m_Output11);
	DDX_Control(pDX, IDC_OUTPUT12, m_Output12);
	DDX_Control(pDX, IDC_OUTPUT13, m_Output13);
	DDX_Control(pDX, IDC_OUTPUT14, m_Output14);
	DDX_Control(pDX, IDC_OUTPUT15, m_Output15);
	DDX_Control(pDX, IDC_OUTPUT16, m_Output16);
	DDX_Control(pDX, IDC_LOG_LIST, m_LogList);
	DDX_Control(pDX, IDC_LOG_CHECK, m_LogBtn);
}

BEGIN_MESSAGE_MAP(SVTestIODlg, CDialog)
	ON_STN_CLICKED(IDC_INPUT1, &SVTestIODlg::OnStnClickedInput1)
	ON_STN_CLICKED(IDC_INPUT2, &SVTestIODlg::OnStnClickedInput2)
	ON_STN_CLICKED(IDC_INPUT3, &SVTestIODlg::OnStnClickedInput3)
	ON_STN_CLICKED(IDC_INPUT4, &SVTestIODlg::OnStnClickedInput4)
	ON_STN_CLICKED(IDC_INPUT5, &SVTestIODlg::OnStnClickedInput5)
	ON_STN_CLICKED(IDC_INPUT6, &SVTestIODlg::OnStnClickedInput6)
	ON_STN_CLICKED(IDC_INPUT7, &SVTestIODlg::OnStnClickedInput7)
	ON_STN_CLICKED(IDC_INPUT8, &SVTestIODlg::OnStnClickedInput8)
	ON_BN_CLICKED(IDC_LOG_BUTTON, &SVTestIODlg::OnBnClickedLogButton)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &SVTestIODlg::OnBnClickedClearButton)
	ON_BN_CLICKED(IDC_TRIGGER_BUTTON, &SVTestIODlg::OnBnClickedTriggerButton)
	ON_BN_CLICKED(IDC_TRIGGER_2, &SVTestIODlg::OnBnClickedTrigger2)
	ON_BN_CLICKED(IDC_TRIGGER_3, &SVTestIODlg::OnBnClickedTrigger3)
	ON_BN_CLICKED(IDC_TRIGGER_4, &SVTestIODlg::OnBnClickedTrigger4)
END_MESSAGE_MAP()


// SVTestIODlg message handlers

void SVTestIODlg::OnStnClickedInput1()
{
	if( m_lInputs & 1 )
	{
		m_lInputs &= ~1 ;
		m_Input1.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 1 ;
		m_Input1.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput2()
{
	if( m_lInputs & 2 )
	{
		m_lInputs &= ~2 ;
		m_Input2.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 2 ;
		m_Input2.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput3()
{
	if( m_lInputs & 4 )
	{
		m_lInputs &= ~4 ;
		m_Input3.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 4 ;
		m_Input3.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput4()
{
	if( m_lInputs & 8 )
	{
		m_lInputs &= ~8 ;
		m_Input4.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 8 ;
		m_Input4.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput5()
{
	if( m_lInputs & 16 )
	{
		m_lInputs &= ~16 ;
		m_Input5.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 16 ;
		m_Input5.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput6()
{
	if( m_lInputs & 32 )
	{
		m_lInputs &= ~32 ;
		m_Input6.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 32 ;
		m_Input6.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput7()
{
	if( m_lInputs & 64 )
	{
		m_lInputs &= ~64 ;
		m_Input7.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 64 ;
		m_Input7.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::OnStnClickedInput8()
{
	if( m_lInputs & 128 )
	{
		m_lInputs &= ~128 ;
		m_Input8.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= 128 ;
		m_Input8.SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::SetOutput( unsigned long p_iChannel, bool p_bState)
{

	HICON l_hState = NULL;

	::EnterCriticalSection( &m_CriticalSection );
	if( p_bState )
	{
		l_hState = AfxGetApp()->LoadIconA( IDI_ICON1 );
	}
	else
	{
		l_hState = AfxGetApp()->LoadIconA( IDI_ICON4 );
	}

	switch( p_iChannel )
	{
		case 0:
		{
			m_Output1.SetIcon( l_hState );
			break;
		}
		case 1:
		{
			m_Output2.SetIcon( l_hState );
			break;
		}
		case 2:
		{
			m_Output3.SetIcon( l_hState );
			break;
		}
		case 3:
		{
			m_Output4.SetIcon( l_hState );
			break;
		}
		case 4:
		{
			m_Output5.SetIcon( l_hState );
			break;
		}
		case 5:
		{
			m_Output6.SetIcon( l_hState );
			break;
		}
		case 6:
		{
			m_Output7.SetIcon( l_hState );
			break;
		}
		case 7:
		{
			m_Output8.SetIcon( l_hState );
			break;
		}
		case 8:
		{
			m_Output9.SetIcon( l_hState );
			break;
		}
		case 9:
		{
			m_Output10.SetIcon( l_hState );
			break;
		}
		case 10:
		{
			m_Output11.SetIcon( l_hState );
			break;
		}
		case 11:
		{
			m_Output12.SetIcon( l_hState );
			break;
		}
		case 12:
		{
			m_Output13.SetIcon( l_hState );
			break;
		}
		case 13:
		{
			m_Output14.SetIcon( l_hState );
			break;
		}
		case 14:
		{
			m_Output15.SetIcon( l_hState );
			break;
		}
		case 15:
		{
			m_Output16.SetIcon( l_hState );
			break;
		}
	}
	if( m_LogBtn.GetCheck() != 0 )
	{
		CString l_strTmp;
		unsigned __int64 l_i64Check;
		unsigned __int64 l_iTime;

		if( m_bResetStart )
		{
			::QueryPerformanceCounter( (LARGE_INTEGER*)&m_i64Start);
			::QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
			m_i64Frequency/=1000;
			m_bResetStart = false;
		}

		::QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Check);
		l_iTime = (l_i64Check - m_i64Start)/m_i64Frequency;
		l_strTmp.Format( "%010I64u Ch %02d %s", l_iTime, p_iChannel + 1, p_bState? "Off" : "On");
		m_LogList.AddString(l_strTmp);
	}
	::LeaveCriticalSection(&m_CriticalSection);
}

BOOL SVTestIODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVTestIODlg::OnBnClickedLogButton()
{
	m_bResetStart = true;
	m_LogList.AddString("*** Reset TS ****");
}


void SVTestIODlg::OnBnClickedClearButton()
{
	m_LogList.ResetContent();

}

void SVTestIODlg::OnBnClickedTriggerButton()
{

	for( TriggerList::iterator it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it)
	{
		TriggerCallbackList& list = it->second;
		if( it->first == 1) // Trigger 1....1 based handle / index.
		{
			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					(list[i].pCallback)(list[i].pOwner, list[i].pData);
				}
			}
		}
	}
}


void SVTestIODlg::OnBnClickedTrigger2()
{
	for( TriggerList::iterator it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it)
	{
		TriggerCallbackList& list = it->second;
		if( it->first == 2) // Trigger 2. 1 based handle / index.
		{
			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					(list[i].pCallback)(list[i].pOwner, list[i].pData);
				}
			}
		}
	}
}

void SVTestIODlg::OnBnClickedTrigger3()
{
	for( TriggerList::iterator it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it)
	{
		TriggerCallbackList& list = it->second;
		if( it->first == 3) // Trigger 3.  1 based handle / index.
		{
			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					(list[i].pCallback)(list[i].pOwner, list[i].pData);
				}
			}
		}
	}
}

void SVTestIODlg::OnBnClickedTrigger4()
{
	for( TriggerList::iterator it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it)
	{
		TriggerCallbackList& list = it->second;
		if( it->first == 4) // trigger 4.  1 based handle / index.
		{
			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					(list[i].pCallback)(list[i].pOwner, list[i].pData);
				}
			}
		}
	}
}


HRESULT SVTestIODlg::AddTriggerCallback(unsigned long handle, SVTestIOCallbackPtr pCallback, void* pOwner, void* pData)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;

		// check for dups
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVTestIOTriggerFind(), pCallback));
		
		if (callbackIt != list.end())
		{
			// DUPLICATE Entry!!!
		}
		else
		{
			// add it
			SVTestIOTriggerStruct triggerStruct;
			triggerStruct.bStarted = false;
			triggerStruct.pCallback = pCallback;
			triggerStruct.pOwner = pOwner;
			triggerStruct.pData = pData;

			list.push_back(triggerStruct);
			hr = S_OK;
		}
	}
	else
	{
		// add it
		TriggerCallbackList list;
		SVTestIOTriggerStruct triggerStruct;
		triggerStruct.bStarted = false;
		triggerStruct.pCallback = pCallback;
		triggerStruct.pOwner = pOwner;
		triggerStruct.pData = pData;

		list.push_back(triggerStruct);
		m_triggerList.insert(std::make_pair(handle, list));
			
		hr = S_OK;
	}
	return hr;
}

HRESULT SVTestIODlg::RemoveTriggerCallback(unsigned long handle, SVTestIOCallbackPtr pCallback)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		// check if it is in the list
		TriggerCallbackList& list = it->second;
		
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVTestIOTriggerFind(), pCallback));
		if (callbackIt != list.end())
		{
			list.erase(callbackIt);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVTestIODlg::RemoveAllTriggerCallbacks(unsigned long handle)
{
	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		m_triggerList.erase(it);
	}
	return S_OK;
}

HRESULT SVTestIODlg::StartTrigger(unsigned long handle)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = true;
		}
		hr = S_OK;
	}

	return hr;
}

HRESULT SVTestIODlg::StopTrigger(unsigned long handle)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		hr = S_OK;
	}
	if( hr == S_OK )
	{
		bool l_bDisableIrq = true;
		for( it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it )
		{
			TriggerCallbackList& list = it->second;

			if( 0 == list.size() || list[0].bStarted )
			{
				l_bDisableIrq = false;
			}
		}
	}

	return hr;
}

void SVTestIODlg::OnCancel()
{
	return;

	CDialog::OnCancel();
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTestIO\SVTestIODlg.cpp_v  $
 * 
 *    Rev 1.2   16 Jul 2013 13:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  846
 * SCR Title:  Update SVTestIO Dll to include triggers
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Trigger button functionallity.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 11:48:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 13:10:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:29:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/