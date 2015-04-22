//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListSheet
//* .File Name       : $Workfile:   MonitorListSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   08 Jul 2014 09:19:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <set>
#include <algorithm>
#include <boost/function.hpp>
#include <boost/assign/list_of.hpp>
#include "MonitorListSheet.h"
#include "ISVPropertyPageDialog.h"
#include "MonitorListValuesPage.h"
#include "MonitorListImagesPage.h"
#include "SVConfigurationObject.h"
#include "SVToolSet.h"
#include "RemoteMonitorListHelper.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString ValuesTag(_T("Values"));
static const CString ImagesTag(_T("Images"));

typedef boost::function<bool(const SVObjectTypeInfoStruct& info)> AllowedFunc;

static bool AllowAll(const SVObjectTypeInfoStruct& info)
{
	return true;
}

static bool AllowOnly(const SVObjectTypeInfoStruct& info)
{
	typedef std::set<SVObjectTypeEnum> ObjectTypes;
	bool bRetVal = false;
	static ObjectTypes objectTypeList = boost::assign::list_of<>
	(SVDWordValueObjectType)
	(SVLongValueObjectType)
	(SVDoubleValueObjectType)
	(SVBoolValueObjectType)
	(SVByteValueObjectType)
//	(SVEnumValueObjectType)
	(SVInt64ValueObjectType)
	;

	ObjectTypes::const_iterator it = objectTypeList.find(info.ObjectType);
	if (it != objectTypeList.end())
	{
		bRetVal = true;
	}
	return bRetVal;
}

class MatchName
{
private:
	SVString m_name;

public:
	MatchName(const SVString& name)
	: m_name(name) 
	{
	}
	bool operator()(const SvUl::NameSelection& selection) const
	{
		return (selection.first == m_name);
	}
};

static void SelectItem(SvUl::NameSelectionList& rList, const SVString& name)
{
	// find name (full or partial) ?
	SvUl::NameSelectionList::iterator it = std::find_if(rList.begin(), rList.end(), MatchName(name));
	if (it != rList.end())
	{
		it->second = true;
	}
}

static SvUl::NameSelectionList BuildSelectionList(const SVString& PPQName, const MonitoredObjectList& rList, AllowedFunc Allowed, bool bAllowWholeArray, bool bImages=false)
{
	SvUl::NameSelectionList nameList;

	SVConfigurationObject* pConfig(NULL);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (pConfig)
	{
		SVPPQObject* pPPQ(NULL);
		pConfig->GetPPQByName(PPQName.c_str(), &pPPQ);
		if (pPPQ)
		{
			// Iterate over the Inspections and get the published data
			long numInspections(0);
			pPPQ->GetInspectionCount(numInspections);
			for (long i = 0;i < numInspections;i++)
			{
				SVInspectionProcess* pInspection(NULL);
				pPPQ->GetInspection(i, pInspection);
				if (pInspection)
				{
					if (!bImages)
					{
						// Get Available Items from the PPQ Published Value List
						SVPublishListClass& rPublishList = pInspection->GetPublishList();
						for (int i = 0;i < rPublishList.GetSize();i++)
						{
							SVOutObjectInfoStruct* pInfo = rPublishList.GetAt(i);
							if (pInfo)
							{
								// Get Object Name
								SVObjectReference objectRef = pInfo->GetObjectReference();
								if (Allowed(pInfo->ObjectTypeInfo))
								{
									SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*>(objectRef.Object());
									if (pValueObject)
									{
										if (pValueObject->IsArray())
										{
											// if want whole array
											if (bAllowWholeArray)
											{
												// stuff entire array specifier at the top of the list
												objectRef.SetEntireArray();
												SVString name = objectRef.GetCompleteOneBasedObjectName();
												nameList.push_back(std::make_pair(name, false));
											}
											// add array elements
											for (int i = 0;i < pValueObject->GetArraySize();i++)
											{
												objectRef.SetArrayIndex(i);
												SVString name = objectRef.GetCompleteOneBasedObjectName();
												nameList.push_back(std::make_pair(name, false));
											}
										}
										else
										{
											SVString name = objectRef.GetCompleteObjectName();
											nameList.push_back(std::make_pair(name, false));
										}
									}
								}
							}
						}
					}
					else
					{
						// Get Available Items from the PPQ Published Images List...
						// Get the Image List from the Toolset, include the Toolset Image and any Images that are published
						SVToolSetClass* pToolSet = pInspection->GetToolSet();
						if (pToolSet)
						{
							SVImageListClass imageList;
							pToolSet->GetImageList(imageList, SV_PUBLISH_RESULT_IMAGE);
						
							const SVImageClass* pMainImage = pToolSet->getCurrentImage();
							if (pMainImage)
							{
								SVString mainImageName = pMainImage->GetCompleteObjectName();
								nameList.push_back(std::make_pair(mainImageName, false));
							}
							for (SVImageListClass::const_iterator it = imageList.begin();it != imageList.end();++it)
							{
								const SVImageClass* pImage = (*it);
								if (pImage)
								{
									SVString name = pImage->GetCompleteObjectName();
									nameList.push_back(std::make_pair(name, false));
								}
							}
						}
					}
				}
			}
		}
	}
	// mark the selected items
	for (MonitoredObjectList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		// Get Name from MonitoredObject
		const SVString& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		SelectItem(nameList, name);
	}
	return nameList;
}

IMPLEMENT_DYNAMIC(MonitorListSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(MonitorListSheet, CPropertySheet)
	//{{AFX_MSG_MAP(MonitorListSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

MonitorListSheet::MonitorListSheet( LPCTSTR pszCaption, MonitorListType eListType, bool bImageTab, const RemoteMonitorNamedList& rList, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
, m_eListType(eListType)
, m_MonitorList(rList)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	CreatePages(bImageTab);
}

MonitorListSheet::~MonitorListSheet()
{
	DestroyPages();
}

const RemoteMonitorNamedList& MonitorListSheet::GetMonitorList() const
{
	return m_MonitorList;
}

HRESULT MonitorListSheet::CreatePages(bool bImageTab)
{
	switch (m_eListType)
	{
		case PRODUCT_OBJECT_LIST:
		{
			//SetDialogName = Product Object List
			const SvUl::NameSelectionList& valueSelectionList = BuildSelectionList(m_MonitorList.GetPPQName(), m_MonitorList.GetProductValuesList(), AllowAll, true);
			MonitorListValuesPage* pValuesDlg = new MonitorListValuesPage(valueSelectionList, true, this, ValuesTag);
			AddPage(pValuesDlg);

			const SvUl::NameSelectionList& imageSelectionList = BuildSelectionList(m_MonitorList.GetPPQName(), m_MonitorList.GetProductImagesList(), AllowAll, false, true);
			MonitorListImagesPage* pImagesDlg = new MonitorListImagesPage(imageSelectionList, this, ImagesTag);
			AddPage(pImagesDlg);

			//SetActivePage(m_initialActivePage);
			break;
		}
		case FAIL_STATUS_LIST:
		{
			//SetDialogName = Fail Status List
			const SvUl::NameSelectionList& valueSelectionList = BuildSelectionList(m_MonitorList.GetPPQName(), m_MonitorList.GetFailStatusList(), AllowOnly, false);
			MonitorListValuesPage* pValuesDlg = new MonitorListValuesPage(valueSelectionList, false, this, ValuesTag);
			AddPage(pValuesDlg);
			break;
		}
		case REJECT_CONDITION_LIST:
		{
			//SetDialogName = Reject Condition List
			const SvUl::NameSelectionList& valueSelectionList = BuildSelectionList(m_MonitorList.GetPPQName(), m_MonitorList.GetRejectConditionList(), AllowOnly, false);
			MonitorListValuesPage* pValuesDlg = new MonitorListValuesPage(valueSelectionList, false, this, ValuesTag);
			AddPage(pValuesDlg);
			break;
		}
	}
	return S_OK;
}

MonitoredObjectList MonitorListSheet::GetMonitoredObjectList(const SvUl::NameSelectionList& rList)
{
	MonitoredObjectList monitoredObjectList;
	for (SvUl::NameSelectionList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		if (it->second)
		{
			const MonitoredObject& monitoredObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(it->first);
			if (!monitoredObj.guid.empty())
			{
				monitoredObjectList.push_back(monitoredObj);
			}
		}
	}
	return monitoredObjectList;
}

void MonitorListSheet::SetMonitoredValues(MonitorListType eListType, const MonitoredObjectList& rList)
{
	switch (eListType)
	{
		case PRODUCT_OBJECT_LIST:
		{
			m_MonitorList.SetProductValuesList(rList);
			break;
		}
		case FAIL_STATUS_LIST:
		{
			m_MonitorList.SetFailStatusList(rList);
			break;
		}
		case REJECT_CONDITION_LIST:
		{
			m_MonitorList.SetRejectConditionList(rList);
			break;
		}
	}
}

void MonitorListSheet::SetMonitoredImages(MonitorListType eListType, const MonitoredObjectList& rList)
{
	switch (eListType)
	{
		case PRODUCT_OBJECT_LIST:
		{
			m_MonitorList.SetProductImagesList(rList);
			break;
		}
		case FAIL_STATUS_LIST:
		{
			break;
		}
		case REJECT_CONDITION_LIST:
		{
			break;
		}
	}
}

bool MonitorListSheet::CanCancel()
{
	return true;
}

HRESULT MonitorListSheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
}

HRESULT MonitorListSheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// MonitorListSheet message handlers

BOOL MonitorListSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( pWnd )
	{
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );
	}
	return bResult;
}

void MonitorListSheet::OnOK() 
{
	CWnd* pActivePage = GetActivePage();
	int iNumPages = GetPageCount();
	int i=0;

	// can we exit?
	for( i = 0; i < iNumPages; i++ )
	{
		if ( ISVPropertyPageDialog* pIDlg = dynamic_cast <ISVPropertyPageDialog*> ( GetPage(i) ) )
		{
			if ( pIDlg->QueryAllowExit() == false )	// exit not allowed
			{
				return;
			}
		}
	}

	// OK to exit; call OnOK for all pages

	bool bChanges = false;

	for( i = 0; i < iNumPages; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			if ( pPage == pActivePage )
			{
				pPage->OnKillActive();
			}
			pPage->OnOK();
			if ( MonitorListBasePage* pCHPage = dynamic_cast <MonitorListBasePage*> (pPage) )
			{
				bChanges = bChanges || pCHPage->m_bObjectsChanged;

				const SvUl::NameSelectionList& rList = pCHPage->GetNameSelectionList();
				const MonitoredObjectList& selectionList = GetMonitoredObjectList(rList);

				if (pCHPage->IsKindOf(RUNTIME_CLASS(MonitorListValuesPage)))
				{
					SetMonitoredValues(m_eListType, selectionList);
				}
				else if (pCHPage->IsKindOf(RUNTIME_CLASS(MonitorListImagesPage)))
				{
					SetMonitoredImages(m_eListType, selectionList);
				}
			}
		}
	}
	EndDialog(IDOK);
}

void MonitorListSheet::DestroyPages()
{
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		CPropertyPage* pPage = GetPage( i );
		RemovePage( i );
		if( pPage )
		{
			delete pPage;
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListSheet.cpp_v  $
 * 
 *    Rev 1.3   08 Jul 2014 09:19:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed GetObjectName function.
 * Removed GetGuidFromObjectName function.
 * Revised GetGuidFromObjectName to use RemoteMonitorListHelper.
 * Revised CreatePages to allow wholeArraySelection.
 * Revised GetMonitoredObjectList to use RemoteMonitorListHelper.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 May 2014 11:23:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   change CreatePages method - only all certain elements for the Fail Status List.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 May 2014 14:47:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised BuildSelectionList to use a functor to determine if an item should be added.
 * Revised CreatePages to restrict selection for the RejectConditionList.
 * Added AllowAll and AllowOnly functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:24:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

