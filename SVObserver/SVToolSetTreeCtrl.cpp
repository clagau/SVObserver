//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetTreeCtrl
//* .File Name       : $Workfile:   SVToolSetTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2014 16:15:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPDoc.h"
#include "SVToolSetTreeCtrl.h"
#include "SVTaskObjectList.h"
#include "SVToolSetTabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVToolSetTreeCtrl, CTreeCtrl)

SVToolSetTreeCtrl::SVToolSetTreeCtrl()
: CTreeCtrl()
, m_pTaskList(nullptr)
{
}

SVToolSetTreeCtrl::~SVToolSetTreeCtrl()
{
}

void SVToolSetTreeCtrl::SetHasButtonsStyle()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	style |= TVS_HASBUTTONS;
	SetWindowLong(m_hWnd, GWL_STYLE, style);
}

void SVToolSetTreeCtrl::SetNoEditLabelsStyle()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	style &= (~TVS_EDITLABELS);
	SetWindowLong(m_hWnd, GWL_STYLE, style);
}

bool SVToolSetTreeCtrl::SetTaskObjectList(SVTaskObjectListClass* pTaskList)
{
	bool bRetVal = false;
	if (nullptr != pTaskList)
	{
		m_pTaskList = pTaskList;
		// Rebuild list...
		Rebuild();
		bRetVal = true;
	}
	return bRetVal;
}

SVToolSetTabViewClass* SVToolSetTreeCtrl::GetView()
{
	return dynamic_cast<SVToolSetTabViewClass *>(GetParent());
}

const SVToolSetTabViewClass* SVToolSetTreeCtrl::GetView() const
{
	return dynamic_cast<SVToolSetTabViewClass *>(GetParent());
}

HTREEITEM SVToolSetTreeCtrl::InsertTaskItem(HTREEITEM HParent, SVTaskObjectClass* pTask)
{
	HTREEITEM hReturn = nullptr;
	if (nullptr != pTask && !pTask->GetClassID().empty())
	{
		if (SV_IS_KIND_OF(pTask, SVTaskObjectListClass))
		{
			// task object item is task object list...	 
			SVTaskObjectListClass* pTaskList = static_cast<SVTaskObjectListClass *>(pTask);
			// ...insert item
			hReturn = InsertItem(pTaskList->GetName(), 0, 0, HParent);
			if (hReturn)
			{
				SetItemData(hReturn, reinterpret_cast<DWORD_PTR>(pTaskList));

				//...insert list members
				for (int i = 0; i < pTaskList->GetSize(); ++i)
				{
					InsertTaskItem(hReturn, pTaskList->GetAt(i));
				}
				Expand(hReturn, TVE_EXPAND);
			}
		}
		else
		{
			// ...insert item
			hReturn = InsertItem(pTask->GetName(), 0, 0, HParent);
			if (hReturn)
			{
				SetItemData(hReturn, reinterpret_cast<DWORD_PTR>(pTask));
			}
		}
	}
	return hReturn;
}

HTREEITEM SVToolSetTreeCtrl::InsertToolSet(HTREEITEM HParent, SVTaskObjectListClass* pTask)
{
	HTREEITEM hReturn = nullptr;
	if (nullptr != pTask && !pTask->GetClassID().empty())
	{
		// ...insert item
		hReturn = InsertItem(pTask->GetName(), 0, 0, HParent);
		if (hReturn)
		{
			SetItemData(hReturn, reinterpret_cast<DWORD_PTR>(pTask));
		}
	}
	return hReturn;
}

static SVTaskObjectClass* GetTaskObject(SVTaskObjectListClass* pTaskObjectList, const CString& name)
{
	SVTaskObjectClass* pTaskObject = nullptr;
	for (int i = 0;i < pTaskObjectList->GetSize() && !pTaskObject;i++)
	{
		SVTaskObjectClass* pObject = pTaskObjectList->GetAt(i);
		if (pObject)
		{
			if (pObject->GetName() == name)
			{
				pTaskObject = pObject;
			}
		}
	}
	return pTaskObject;
}

void SVToolSetTreeCtrl::Rebuild()
{
	DeleteAllItems();

	if (nullptr != m_pTaskList)
	{
		HTREEITEM hRoot = InsertToolSet(nullptr, m_pTaskList);
		SVToolSetTabViewClass* pView = GetView();
		if (pView)
		{
			SVIPDoc* pDoc = pView->GetIPDoc();
			if (pDoc)
			{
				bool bStart = false;
				HTREEITEM hGroup = nullptr;
				HTREEITEM hInsertAfter = nullptr;
				const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
				for (SVToolGrouping::const_iterator it = rGroupings.begin();it != rGroupings.end();++it)
				{
					if (ToolGroupData::StartOfGroup == it->second.m_type)
					{
						// add group
						hGroup = InsertItem(it->first.c_str(), 0,0, hRoot, hInsertAfter);
						hInsertAfter = hGroup;
						bStart = true;
						Expand(hGroup, TVE_EXPAND);
					}
					else if (ToolGroupData::EndOfGroup == it->second.m_type)
					{
						// Add to group as last child mode
						bStart = false;
						InsertItem(it->first.c_str(), 0,0, hGroup, TVI_LAST);
					}
					else if (ToolGroupData::Tool == it->second.m_type)
					{
						SVTaskObjectClass* pTaskObject = GetTaskObject(m_pTaskList, it->first.c_str());
						if (bStart)
						{
							InsertTaskItem(hGroup, pTaskObject);
						}
						else
						{
							hInsertAfter = InsertTaskItem(hRoot, pTaskObject);
						}
					}
				}
				Expand(hRoot, TVE_EXPAND);
			}
		}
	}
}

BEGIN_MESSAGE_MAP(SVToolSetTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(SVToolSetTreeCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetTreeCtrl.cpp_v  $
 * 
 *    Rev 1.0   12 Jun 2014 16:15:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */