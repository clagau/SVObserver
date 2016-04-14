//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVTADlgPassFailPage.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\MessageTextEnum.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogPassFailPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogPassFailPageClass)
		ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect)
		ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations

	#pragma region Constructor
	SVToolAdjustmentDialogPassFailPageClass::SVToolAdjustmentDialogPassFailPageClass(const GUID& rInspectionID, const GUID& rTaskObjectID)
	: CPropertyPage( SVToolAdjustmentDialogPassFailPageClass::IDD )
	, RangeController(rInspectionID, rTaskObjectID)
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogPassFailPageClass)
		//}}AFX_DATA_INIT
		Init();
	}
	#pragma endregion Constructor

	SVToolAdjustmentDialogPassFailPageClass::~SVToolAdjustmentDialogPassFailPageClass()
	{
	}

	#pragma region Public Methods
	bool SVToolAdjustmentDialogPassFailPageClass::QueryAllowExit()
	{
		UpdateData(true);

		return UpdateRangeValues();
	}
	#pragma endregion Public Methods

	void SVToolAdjustmentDialogPassFailPageClass::SetInspectionData()
	{
		UpdateData(true); // get data from dialog

		// Validate Entered data for existance and if within bounds		
		IsFieldValid(SvOi::Tid_FailHigh, static_cast<LPCSTR>(m_FailHigh));
		IsFieldValid(SvOi::Tid_FailLow, static_cast<LPCSTR>(m_FailLow));
		IsFieldValid(SvOi::Tid_WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
		IsFieldValid(SvOi::Tid_WarnLow, static_cast<LPCSTR>(m_WarnLow));
		
		Set(FailHigh, static_cast<LPCSTR>(m_FailHigh));
		Set(FailLow, static_cast<LPCSTR>(m_FailLow));
		Set(WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
		Set(WarnLow, static_cast<LPCSTR>(m_WarnLow));
	}

	#pragma region Protected Methods
	void SVToolAdjustmentDialogPassFailPageClass::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(SVToolAdjustmentDialogPassFailPageClass)
		DDX_Text(pDX, IDC_EDIT_FAILHIGH, m_FailHigh);
		DDX_Text(pDX, IDC_EDIT_WARNHIGH, m_WarnHigh);
		DDX_Text(pDX, IDC_EDIT_WARNLOW, m_WarnLow);
		DDX_Text(pDX, IDC_EDIT_FAILLOW, m_FailLow);
		DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
		DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
		//}}AFX_DATA_MAP
	}

	void SVToolAdjustmentDialogPassFailPageClass::InitData()
	{
		try
		{
			m_FailHigh = Get(FailHigh).c_str();
			m_FailLow = Get(FailLow).c_str();
			m_WarnHigh = Get(WarnHigh).c_str();
			m_WarnLow  = Get(WarnLow).c_str();
		}
		catch (...) // find out what to catch here...
		{
		}
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);

		//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
		// And it is NOT needed as the implicit cast will call the casting operator.
		m_ButtonFailHigh.SetBitmap(m_downArrowBitmap);
		m_ButtonWarnHigh.SetBitmap(m_downArrowBitmap);
		m_ButtonWarnLow.SetBitmap(m_downArrowBitmap);
		m_ButtonFailLow.SetBitmap(m_downArrowBitmap);

		InitData();
		UpdateData(false); // set data to dialog

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnSetActive() 
	{
		InitData();
		return CPropertyPage::OnSetActive();
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnKillActive() 
	{
		UpdateData(true);
		bool bOk = UpdateRangeValues();

		if (bOk)
		{
			return CPropertyPage::OnKillActive();
		}
		return bOk;
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect()
	{
		setValuePerObjectSelector(m_FailHigh, RangeEnum::ER_FailHigh);
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect()
	{
		setValuePerObjectSelector(m_WarnHigh, RangeEnum::ER_WarnHigh);
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect()
	{
		setValuePerObjectSelector(m_WarnLow, RangeEnum::ER_WarnLow);
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect()
	{
		setValuePerObjectSelector(m_FailLow, RangeEnum::ER_FailLow);
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	bool SVToolAdjustmentDialogPassFailPageClass::UpdateRangeValues()
	{
		bool bRetVal = true;
	
		SVString errorMsg;

		try
		{
			SetInspectionData();
			Validate(AfxGetResourceHandle());
			Commit();
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( rSvE.getMessage() );
			bRetVal = false;
		}
		
		return bRetVal;
	}

	void SVToolAdjustmentDialogPassFailPageClass::setValuePerObjectSelector(CString& value, RangeEnum::ERange fieldEnum)
	{
		if (ShowObjectSelector(value, fieldEnum))
		{
			 UpdateData(false);
		}
	}

	bool SVToolAdjustmentDialogPassFailPageClass::ShowObjectSelector(CString& name, RangeEnum::ERange fieldEnum)
	{
		bool retValue = false;

		FillObjectSelector();

		if (name.GetLength() > 0)
		{
			SVStringSet nameSet;
			nameSet.insert(name);
			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
		}

		SVString Title = GetOwnerName();
		Title += _T(": ");
		Title += RangeEnum::ERange2String(AfxGetResourceHandle(), fieldEnum);

		CString mainTabTitle;
		mainTabTitle.LoadString(IDS_SELECT_TOOLSET_OUTPUT);
		CString FilterTab;
		FilterTab.LoadString(IDS_FILTER);

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), mainTabTitle, FilterTab, this );

		if (IDOK == Result)
		{
			name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); // @TODO:  Should we check the return values of getSingleObjectResult and getLocation?
			retValue = true;
		}
		return retValue;
	}

	#pragma endregion Private Methods
} /* namespace SVOGui */ } /* namespace Seidenader */

