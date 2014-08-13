//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputInfoListTreeCtrl
//* .File Name       : $Workfile:   SVOutputInfoListTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Aug 2014 06:47:04  $
//******************************************************************************

#ifndef SVOUTPUTINFOLISTTREECTRL_H
#define SVOUTPUTINFOLISTTREECTRL_H

#include <map>
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVLoki/Functor.h"	// Loki
#include "SVLoki/LokiTypeInfo.h"

class SVImageListClass;
class SVObjectListClass;
class SVOutputInfoListClass;
class SVTaskObjectListClass;


// this callback occurs when the user attempts to select or deselect an item.
// the first parameter is the object, the second parameter is the current state
// of the item. The third parameter is the index of the item in the tree
// the return value indicates if the object can be selected/deselected.
typedef Loki::Functor<bool, TYPELIST_3(SVObjectReference, bool, int) > SVObjectTreeCanSelectObjectCallbackFn;
typedef Loki::Functor<void, TYPELIST_1( int ) > SVClickCallbackFn;


struct SVSelectObjectFilterInfoStruct
{
	bool bOnOff;
	UINT uAttributeFilter;
	CString strTextFilter;

	SVSelectObjectFilterInfoStruct() : bOnOff(false), uAttributeFilter(0) {}
};

/////////////////////////////////////////////////////////////////////////////
// SVOutputInfoListTreeCtrlClass window

class SVOutputInfoListTreeCtrlClass : public CTreeCtrl
{
	DECLARE_DYNCREATE(SVOutputInfoListTreeCtrlClass)

public:
	SVOutputInfoListTreeCtrlClass();
	virtual ~SVOutputInfoListTreeCtrlClass();
	BOOL InitOutputListTreeCtrl();

	void SetSingleSelect(bool bSingle);
	void SetAllowWholeArray();
	void SetSelection(const SVObjectReferenceVector& p_selectObjectList);

	void BuildOutputList(SVTaskObjectListClass* pTaskObjectList, UINT uAttributesDesired, UINT uAttributesAllowedFilter = 0 );
	void BuildImageListTree(SVImageListClass* pImageList, UINT uAttributesDesired, SVObjectTypeEnum objectTypeToInclude, UINT uAttributesAllowedFilter = 0  );

	void AllowNodeItemChecks( bool p_bAllow );
	BOOL UpdateAttributesInOutputList();

	SVObjectReference GetCheckedObject();	// use for single select
	BOOL GetCheckedObjects(SVObjectListClass* pObjectList);	// use for multiselect

	HRESULT SelectObjectFilter( SVSelectObjectFilterInfoStruct svFilter );

	void SetCanSelectObjectCallback( SVObjectTreeCanSelectObjectCallbackFn fn );
	void SetClickCallback( SVClickCallbackFn fn);

	HRESULT GetAddedObjects( std::set<SVObjectReference>& p_rAddedSet );
	HRESULT GetRemovedObjects( std::set<SVObjectReference>& p_rRemovedSet );
	HRESULT GetChangedObjects( std::set<SVObjectReference>& p_rChangedSet );
	HRESULT GetSelectedObjects( SVObjectReferenceVector& p_rvecSelectedObjects );
	HRESULT GetSelectedObjectsInTreeOrder( SVObjectReferenceVector& p_rvecSelectedObjects, std::vector<int>& p_rvecIndexes );

	// if either of these is empty, it is ignored in filtering
	HRESULT SetTypeFilter( const std::set<Loki::TypeInfo>& p_setFilterInclude, const std::set<Loki::TypeInfo>& p_setFilterExclude );

	bool SelectItemByIndex( int iIndex, bool bSelect );
	void SetBranchChecks( HTREEITEM hItem , bool p_bNewBranchState );

protected:
	SVObjectReference ObjectFromItem(HTREEITEM ht);
	void RebuildTree(const SVObjectReferenceVector& rObjectVector, SVObjectTypeEnum objectTypeToInclude );
	HTREEITEM GetCheckedItem();	// use for single select
	UINT TokenizeName(CString& strCompleteName, CStringArray& astrNames );

	bool GetCheckState( HTREEITEM hItem );
	bool SetCheckState( HTREEITEM hItem, bool bCheck, bool bSelect = true );

	bool Click( HTREEITEM hItem );

	void ProcessItemAttributes( HTREEITEM hItem, void* );
	typedef Loki::Functor< void, TYPELIST_2(HTREEITEM, void*) > TreeCallbackFn;
	void WalkTheTree( HTREEITEM hItem, void* pData, TreeCallbackFn fnItemProc );

	void SelectObjectFilterItemProc( HTREEITEM hItem, void* pData );

	void UpdateNodeStateColor( HTREEITEM hItem, long& p_rlFlags);

// Attributes
	SVTaskObjectListClass* m_pTaskObjectList;	
	SVOutputInfoListClass* m_pOutputInfoList;
	UINT m_uiAttributesSetFilter;
	UINT m_uiAttributesAllowedFilter;
	SVObjectReferenceVector m_ObjectList;

	std::set<SVObjectReference> m_setOrigSelected;
	std::set<SVObjectReference> m_setNewSelected;

	std::set<Loki::TypeInfo> m_setTypeFilterInclude;
	std::set<Loki::TypeInfo> m_setTypeFilterExclude;

	CImageList m_StateImageList;

	typedef std::map <int, HTREEITEM> MapIndexToHandleType;
	MapIndexToHandleType m_mapIndexes;

	typedef std::map <HTREEITEM, bool> MapSelectedType;
	MapSelectedType m_mapSelected;
	HTREEITEM m_hLastChecked;
	bool m_bSingleSelect;
	bool m_bAllowWholeArray;
	bool m_bAllowBranchCheck;

	SVObjectTreeCanSelectObjectCallbackFn m_fnCanSelectObject;
	SVClickCallbackFn m_fnClick;

	SVObjectReferenceVector m_selectObjectList; // used to provide what should be selected
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOutputInfoListTreeCtrlClass)
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
protected:
	//{{AFX_MSG(SVOutputInfoListTreeCtrlClass)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOutputInfoListTreeCtrl.h_v  $
 * 
 *    Rev 1.1   12 Aug 2014 06:47:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added click callback.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:15:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   21 Feb 2012 13:40:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated selection functionality to fix a multi/single select issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   28 Oct 2010 14:08:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   16 Dec 2009 14:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   07 Mar 2008 07:48:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to support branch node check boxes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   30 Jan 2006 09:44:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made changed to allow the Set attributes and Allowed attributes to be specified independently
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   25 Jan 2006 12:39:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetSelectedObjectsInTreeOrder, SelectItemByIndex, Click
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   17 Jan 2006 09:56:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetSelectedObjects, SetTypeFilter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   02 Nov 2005 09:10:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added functions GetAddedObjects, GetRemovedObjects, GetChangedObjects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   25 Aug 2005 13:46:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added text filter capabilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   10 Aug 2005 15:59:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added AllowWholeArray
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   08 Aug 2005 14:04:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented "can select" callback
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   27 Jun 2005 14:56:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Jun 2005 08:57:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented interface for array value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   09 Mar 2005 09:40:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed SVHRESULT #include
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 15:09:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 14:53:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   12 Jul 2004 12:31:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  429
 * SCR Title:  Fix Select Object tree control
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed selection bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 13:37:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2003 15:30:12   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated header information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   20 Nov 2002 10:31:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:22   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 13 1999 14:42:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to archive images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 30 1999 17:27:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repaired bug fix to check for potential crashes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 30 1999 12:14:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixed a bug in that the Tool Set was always used as the 'root' in the tree.    The 'root' is now specified by the type of the object passed to the tree control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Aug 25 1999 10:31:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The software code that implements the tree control for the result picker dialog and possible other uses.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
