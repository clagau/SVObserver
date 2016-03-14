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
#include "stdafx.h"
#include "SVDisplayImageSelect.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDisplayImageSelect dialog


SVDisplayImageSelect::SVDisplayImageSelect(CWnd* pParent /*=NULL*/)
	: CDialog(SVDisplayImageSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDisplayImageSelect)
	//}}AFX_DATA_INIT
	m_pDoc = NULL;
}


void SVDisplayImageSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDisplayImageSelect)
	DDX_Control(pDX, IDC_IMAGE_SELECT_LIST, mImageSelectList);
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
	int index;
	
	index = mImageSelectList.GetCurSel();
	
	if (index != LB_ERR)
	{
		m_pCurrentImage = (SVImageClass*) mImageSelectList.GetItemData( index );
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
	
	//SVImageClass *pImage;
	CString szImageName;
	int index;
	
    if( m_pDoc )
	{
		index = mImageSelectList.AddString( _T("[None]") );
		mImageSelectList.SetItemData( index, static_cast<DWORD_PTR>(NULL) );

        // Get RGB Main Image only for Color Inpsection Document
		// BRW - No longer necessary.  l_Visitor will add this item to the list.
		/*if( m_pDoc->IsColorInspectionDocument() )
		{
			pImage = m_pDoc->GetHSIMainImage();
			if( pImage )
			{
				szImageName = pImage->GetCompleteObjectName();
				index = mImageSelectList.AddString( szImageName );
				mImageSelectList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pImage) );
			}
		}*/
		
		SVObjectTypeInfoStruct info;

		info.ObjectType = SVImageObjectType;
		info.SubType = SVNotSetSubObjectType;

		SVGetObjectDequeByTypeVisitor l_Visitor( info );

		SVObjectManagerClass::Instance().VisitElements( l_Visitor, m_pDoc->GetInspectionID() );

		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

		for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if ( pImage != NULL && !(pImage->ObjectAttributesAllowed() & SV_HIDDEN) )
			{
				szImageName = pImage->GetCompleteObjectName();
				index = mImageSelectList.AddString( szImageName );
				mImageSelectList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pImage) );
			}
		}// end while

		if( m_pCurrentImage != NULL )
			mImageSelectList.SelectString( -1, m_pCurrentImage->GetCompleteObjectName() );
		else
            mImageSelectList.SelectString( -1, _T("[None]") );

		UpdateData( FALSE );

		return TRUE;
	}// end if

	return FALSE;
}// end OnInitDialog

