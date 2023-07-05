//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayImageSelect
//* .File Name       : $Workfile:   SVDisplayImageSelect.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 11:14:52  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVDisplayImageSelect.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// SVDisplayImageSelect dialog


const int DependentColumnNumber = 1;

SVDisplayImageSelect::SVDisplayImageSelect(uint32_t inspectionId, uint32_t imageId, CWnd* pParent /*=nullptr*/)
	: CDialog(SVDisplayImageSelect::IDD, pParent)
	, m_inspectionId(inspectionId)
	, m_imageId(imageId)
	, m_WildcardPattern(_T(""))
{
	//{{AFX_DATA_INIT(SVDisplayImageSelect)
	//}}AFX_DATA_INIT


}

SVDisplayImageSelect::~SVDisplayImageSelect()
{
}

void SVDisplayImageSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDisplayImageSelect)
	DDX_Control(pDX, IDC_IMAGE_SELECT_LIST, m_ImageSelectList);
	DDX_Text(pDX, IDC_WILDCARD_PATTERN, m_WildcardPattern);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDisplayImageSelect, CDialog)
	//{{AFX_MSG_MAP(SVDisplayImageSelect)
	ON_EN_CHANGE(IDC_WILDCARD_PATTERN, OnReloadImageList)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDisplayImageSelect message handlers

void SVDisplayImageSelect::OnOK() 
{
	int index = m_ImageSelectList.GetCurSel();
	
	if (LB_ERR != index)
	{
		m_imageId = static_cast<uint32_t> (m_ImageSelectList.GetItemData( index ));
	}
	CDialog::OnOK();
}

void SVDisplayImageSelect::OnCancel() 
{
	CDialog::OnCancel();
}


void SVDisplayImageSelect::OnReloadImageList()
{
	UpdateData(TRUE);
	LoadImageList();
}

void SVDisplayImageSelect::OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo)
{
	// Set the Minimum Track Size. Used while resizing.
	pMinMaxInfo->ptMinTrackSize.x = m_minSize.cx;
	pMinMaxInfo->ptMinTrackSize.y = m_minSize.cy;
}


void SVDisplayImageSelect::LoadImageList()
{
	constexpr const char* c_NoImage = _T("[None]");

	m_ImageSelectList.ResetContent();

	int index = m_ImageSelectList.AddString(c_NoImage);
	m_ImageSelectList.SetItemData(index, static_cast<DWORD_PTR>(0));

	SvDef::SVObjectTypeInfoStruct info;

	info.m_ObjectType = SvPb::SVImageObjectType;
	info.m_SubType = SvPb::SVNotSetSubObjectType;

	// Differentiate between image owned by ToolSet and image owned by a tool
	auto getImageName = [](SvIe::SVImageClass* pImage) {
		std::string imageName;

		if (auto* pParent = pImage->GetParent();
			nullptr != pParent && SvPb::SVObjectTypeEnum::SVToolSetObjectType == pParent->GetObjectType())
		{
			imageName = pImage->GetName();
		}
		else
		{
			imageName = pImage->GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
		}
		return imageName;
	};

	std::vector<SvOi::IObjectClass*> list;
	fillObjectList(std::back_inserter(list), info, m_inspectionId);
	std::string selectedName;
	for (const auto pObject : list)
	{
		if (nullptr != pObject && SvPb::noAttributes != pObject->ObjectAttributesAllowed())
		{
			std::string name;
			SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (pObject);
			if (nullptr != pImage)
			{
				name = getImageName(pImage);
			}
			else
			{
				name = pObject->GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
			}

			auto wildcardPatternPlusAsterisk = m_WildcardPattern + _T("*");
			if (m_WildcardPattern == _T("") || PathMatchSpec(name.c_str(), wildcardPatternPlusAsterisk.GetString()))
			{
				index = m_ImageSelectList.AddString(name.c_str());
				m_ImageSelectList.SetItemData(index, pObject->getObjectId());
				if (pObject->getObjectId() == m_imageId)
				{
					selectedName = std::move(name);
				}
			}
		}
	}// end while

	if (false == selectedName.empty())
	{
		m_ImageSelectList.SelectString(-1, selectedName.c_str());
	}
	else
	{
		m_ImageSelectList.SelectString(-1, c_NoImage);
	}

	UpdateData(FALSE);
}


BOOL SVDisplayImageSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	SetTitle();

	LoadImageList();

	CRect wndRect;
	GetWindowRect(&wndRect);
	m_minSize = {wndRect.Width(), wndRect.Height()};

	return TRUE;
}// end OnInitDialog


void SVDisplayImageSelect::SetTitle()
{
	SVInspectionProcess* pInspection(dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(m_inspectionId)));
	if (nullptr != pInspection)
	{
		CString title;
		GetWindowText(title);
		title.Format(_T("%s - [%s]"), title.GetString(), pInspection->GetName());
		SetWindowText(title);
	}
}
