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

#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDisplayImageSelect dialog


SVDisplayImageSelect::SVDisplayImageSelect(CWnd* pParent /*=nullptr*/)
	: CDialog(SVDisplayImageSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDisplayImageSelect)
	//}}AFX_DATA_INIT
	m_pDoc = nullptr;
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
		m_pCurrentImage = (SVImageClass*) m_ImageSelectList.GetItemData( index );
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
	
	CString szImageName;
	
    if( m_pDoc )
	{
		int index = m_ImageSelectList.AddString( _T("[None]") );
		m_ImageSelectList.SetItemData( index, static_cast<DWORD_PTR>(0) );

		SVObjectTypeInfoStruct info;

		info.ObjectType = SVImageObjectType;
		info.SubType = SVNotSetSubObjectType;

		SVGetObjectDequeByTypeVisitor l_Visitor( info );

		SVObjectManagerClass::Instance().VisitElements( l_Visitor, m_pDoc->GetInspectionID() );

		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

		for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if ( nullptr != pImage && !(pImage->ObjectAttributesAllowed() & SV_HIDDEN) )
			{
				szImageName = pImage->GetCompleteObjectName();
				index = m_ImageSelectList.AddString( szImageName );
				m_ImageSelectList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pImage) );
			}
		}// end while

		if( nullptr != m_pCurrentImage )
		{
			m_ImageSelectList.SelectString( -1, m_pCurrentImage->GetCompleteObjectName() );
		}
		else
		{
            m_ImageSelectList.SelectString( -1, _T("[None]") );
		}

		UpdateData( FALSE );

		return TRUE;
	}// end if

	return FALSE;
}// end OnInitDialog

