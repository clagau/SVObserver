//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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


SVDisplayImageSelect::SVDisplayImageSelect(uint32_t inspectionId, uint32_t imageId, CWnd* pParent /*=nullptr*/)
	: CDialog(SVDisplayImageSelect::IDD, pParent)
	, m_inspectionId(inspectionId)
	, m_imageId(imageId)
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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDisplayImageSelect, CDialog)
	//{{AFX_MSG_MAP(SVDisplayImageSelect)
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

BOOL SVDisplayImageSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	constexpr TCHAR* c_NoImage = _T("[None]");

	SetTitle();

	int index = m_ImageSelectList.AddString(c_NoImage);
	m_ImageSelectList.SetItemData(index, static_cast<DWORD_PTR>(0));

	SvDef::SVObjectTypeInfoStruct info;

	info.m_ObjectType = SvPb::SVImageObjectType;
	info.m_SubType = SvPb::SVNotSetSubObjectType;

	// Differentiate between image owned by ToolSet and image owned by a tool
	auto getImageName = [](SvIe::SVImageClass* pImage) {
		std::string imageName;

		if (pImage->GetOwnerInfo().CheckExistence() &&
			(pImage->GetOwnerInfo().getObject() != nullptr) &&
			(SvPb::SVObjectTypeEnum::SVToolSetObjectType == pImage->GetOwnerInfo().getObject()->GetObjectType())
			)
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
			index = m_ImageSelectList.AddString(name.c_str());
			m_ImageSelectList.SetItemData(index, pObject->getObjectId());
			if (pObject->getObjectId() == m_imageId)
			{
				selectedName = std::move(name);
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
