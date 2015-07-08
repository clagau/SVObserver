//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFilterPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogFilterPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   24 Oct 2014 11:45:54  $
//******************************************************************************

#include "stdafx.h"
#include "TADialogRingBufferParameterPage.h"

#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IToolSet.h"
#include "ObjectInterfaces/IRootObject.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "SVMessage/SVMessage.h"
#include "TextDefinesSvOg.h"

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
		BEGIN_MESSAGE_MAP(TADialogRingBufferParameterPage, CPropertyPage)
			//{{AFX_MSG_MAP(TADialogRingBufferParameterPage)
			//ON_BN_CLICKED(IDC_BUTTON_RING_DEPTH, OnButtonBufferDepth)
			ON_BN_CLICKED(IDC_BUTTON_IMAGE_INDEX1, OnButtonImageIndex1)
			ON_BN_CLICKED(IDC_BUTTON_IMAGE_INDEX2, OnButtonImageIndex2)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

#pragma region Constructor
		TADialogRingBufferParameterPage::TADialogRingBufferParameterPage( SvOi::IRingBufferTool& rTool) 
			: CPropertyPage(TADialogRingBufferParameterPage::IDD)
			,m_rTool(rTool)
		{
		}

		TADialogRingBufferParameterPage::~TADialogRingBufferParameterPage()
		{
		}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogRingBufferParameterPage::QueryAllowExit()
	{
		HRESULT hResult = SetPageData();
		return (S_OK == hResult);
	}
#pragma endregion Public Methods

#pragma region Protected Methods
		void TADialogRingBufferParameterPage::DoDataExchange(CDataExchange* pDX)
		{
			CPropertyPage::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
			DDX_Control(pDX, IDC_EDIT_RING_DEPTH, m_EditRingDepth);
			DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX1, m_EditImageIndex[0]);
			DDX_Control(pDX, IDC_EDIT_IMAGE_INDEX2, m_EditImageIndex[1]);
			DDX_Control(pDX, IDC_BUTTON_RING_DEPTH, m_ButtonRingDepth);
			DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX1, m_ButtonImageIndex1);
			DDX_Control(pDX, IDC_BUTTON_IMAGE_INDEX2, m_ButtonImageIndex2);
			//}}AFX_DATA_MAP
		}

		BOOL TADialogRingBufferParameterPage::OnInitDialog() 
		{
			CPropertyPage::OnInitDialog();

			// Put the Down Arrow on the Button
			m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

			//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
			m_ButtonRingDepth.SetBitmap( ( HBITMAP )m_downArrowBitmap );
			m_ButtonImageIndex1.SetBitmap( ( HBITMAP )m_downArrowBitmap );
			m_ButtonImageIndex2.SetBitmap( ( HBITMAP )m_downArrowBitmap );

			//set edit controls
			m_EditRingDepth.SetWindowText(m_rTool.getRingBufferDepthString().c_str());
			m_EditImageIndex[0].SetWindowText(m_rTool.getImageIndex(0).c_str());
			m_EditImageIndex[1].SetWindowText(m_rTool.getImageIndex(1).c_str());

			//[MZA]: this button is not used yet, but it will needed for the next version which can set the depth per global variables.
			m_ButtonRingDepth.ShowWindow( HIDE_WINDOW );

			SetInspectionData();

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
		}

		BOOL TADialogRingBufferParameterPage::OnKillActive()
		{
			HRESULT hResult = SetPageData();
			if (S_OK == hResult)
			{
				return CPropertyPage::OnKillActive();
			}
			else
			{
				return FALSE;
			}
		}

		//void TADialogRingBufferParameterPage::OnButtonBufferDepth()
		//{
		//	CString csText;
		//	m_EditRingDepth.GetWindowText(csText); 
		//	CString Title;
		//	Title.Format(IDS_OBJECTNAME_RINGBUFFER_DEPTH);
		//	if (ShowObjectSelector(csText, Title))
		//	{
		//		m_EditRingDepth.SetWindowText(csText);
		//		SetRingDepth();
		//	}
		//}

		void TADialogRingBufferParameterPage::OnButtonImageIndex1()
		{
			ObjectSelectorForIndex(0, IDS_OBJECTNAME_RINGBUFFER_INDEX1);
		}

		void TADialogRingBufferParameterPage::OnButtonImageIndex2()
		{
			ObjectSelectorForIndex(1, IDS_OBJECTNAME_RINGBUFFER_INDEX2);
		}

		void TADialogRingBufferParameterPage::ObjectSelectorForIndex(int index, int titleStringID)
		{
			CString csText;
			m_EditImageIndex[index].GetWindowText(csText); 
			CString Title;
			Title.Format(titleStringID);
			if (ShowObjectSelector(csText, Title))
			{
				m_EditImageIndex[index].SetWindowText(csText);
				SetImageIndex(index);
			}
		}

		bool TADialogRingBufferParameterPage::ShowObjectSelector(CString& name, const CString& Title)
		{
			bool result = false;
			SVString InspectionName = "";
			SVString PPQName = "";
			SvOi::IInspectionProcess* inspection = dynamic_cast<IInspectionProcess*>(m_rTool.GetAncestorInterface(SVInspectionObjectType));
			SvOi::IToolSet* pToolSet = dynamic_cast<IToolSet*>(m_rTool.GetAncestorInterface(SVToolSetObjectType));
			if (nullptr == pToolSet || nullptr == inspection)
			{
				return false;
			}
			
			InspectionName = inspection->GetName();
			IObjectClass *ppq = inspection->GetPPQInterface();
			if (nullptr != ppq)
			{
				PPQName = ppq->GetName();
			}

			SVStringArray nameArray;
			typedef std::insert_iterator<SVStringArray> Inserter;
	
			SvOsl::ObjectTreeGenerator::Instance().setAttributeFilters( SV_SELECTABLE_FOR_EQUATION );
			SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
			SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );
			SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
			SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::TypeSingleObject );

			SVStringArray objectNameList;
			SvOi::getRootChildNameList( objectNameList, _T(""), SV_SELECTABLE_FOR_EQUATION );
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects( objectNameList );

			SvOi::IOutputInfoListClassSmartPointer outputList = pToolSet->GetOutputList();
			SvOsl::ObjectTreeGenerator::Instance().insertOutputList( *(outputList.get()) );

			if(name.GetLength() > 0)
			{
				SVStringSet nameSet;
				nameSet.insert(name);
				SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
			}

			CString mainTabTitle;
			mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
			CString FilterTab;
			FilterTab.LoadString( IDS_FILTER );

			INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, this );

			if( IDOK == Result )
			{
				name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); 
				result = true;
			}

			return result;
		}
#pragma endregion Protected Methods

#pragma region Private Methods
		HRESULT TADialogRingBufferParameterPage::SetPageData()
		{
			HRESULT hResult = SetRingDepth();

			if (S_OK == hResult)
			{
				hResult = SetImageIndex(0);
			}
			if (S_OK == hResult)
			{
				hResult = SetImageIndex(1);
			}

			return hResult;
		}

		HRESULT TADialogRingBufferParameterPage::SetInspectionData()
		{
			HRESULT l_hrOk = S_OK;

			UpdateData( TRUE ); // get data from dialog

			if( l_hrOk == S_OK )
			{
				l_hrOk = m_rTool.RunOnce( &m_rTool );
			}

			UpdateData( FALSE );

			return l_hrOk;
		}

		HRESULT TADialogRingBufferParameterPage::SetRingDepth()
		{
			CString csText;
			m_EditRingDepth.GetWindowText(csText);
			HRESULT hResult = m_rTool.setRingDepth(csText);
			if(S_OK != hResult)
			{
				m_EditRingDepth.SetFocus();
				SvStl::ExceptionMgr1 Exception( SvStl::LogAndDisplay );
				CString strText;
				strText.Format(RingBuffer_Depth_Invalid_ValueString, SvOi::IRingBufferTool::m_minRingBufferDepth, SvOi::IRingBufferTool::m_maxRingBufferDepth, csText);
				Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, strText, StdExceptionParams, hResult, MB_OK | MB_ICONERROR );
			}
			return hResult;
		}

		HRESULT TADialogRingBufferParameterPage::SetImageIndex(int indexNumber)
		{
			CString csText;
			m_EditImageIndex[indexNumber].GetWindowText(csText);
			HRESULT hResult = m_rTool.setImageIndex(indexNumber, csText);
			if(S_OK != hResult)
			{
				m_EditImageIndex[indexNumber].SetFocus();
				SvStl::ExceptionMgr1 Exception( SvStl::LogAndDisplay );
				CString strText;
				strText.Format(RingBuffer_ImageIndex_Invalid_ValueString, indexNumber+1);
				Exception.setMessage( SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE, strText, StdExceptionParams, hResult, MB_OK | MB_ICONERROR );
			}
			return hResult;
		}
#pragma endregion Private Methods
	}  //end namespace SVOGUI
}  //end namespace Seidenader
