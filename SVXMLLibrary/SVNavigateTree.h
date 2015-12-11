//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNavigateTreeClass
//* .File Name       : $Workfile:   SVNavigateTreeClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <comdef.h>
#pragma endregion Includes

class SVNavigateTree  
{
private:
	SVNavigateTree();
	~SVNavigateTree();

public:
	template< typename SVTreeType >
	static bool AddItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem = nullptr );

	template< typename SVTreeType >
	static bool SetItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem = nullptr );

	template< typename SVTreeType >
	static bool AddBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle* ppItem = nullptr );

	template< typename SVTreeType >
	static bool SetBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle* ppItem = nullptr );

	template< typename SVTreeType >
	static bool GetItemBranch( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVBranchHandle& rpItem );

	template< typename SVTreeType >
	static bool GetItemLeaf( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle& rpItem );

	template< typename SVTreeType >
	static bool GetItem( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, _variant_t& rVariant );

	template< typename SVTreeType >
	static bool DeleteItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pItem );

	template< typename SVTreeType >
	static bool DeleteItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle pItem );

	template< typename SVTreeType >
	static bool DeleteAllItems( SVTreeType &rTree );

	template< typename SVTreeType >
	static bool HasChildren(SVTreeType& rTree, typename SVTreeType::SVBranchHandle pItem);

	template< typename SVTreeType, typename SVPredicate >
	static bool FindBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle startBranch, const SVPredicate& rPredicate, typename SVTreeType::SVBranchHandle& rFoundItem );

	template< typename SVToTreeType, typename SVFromTreeType >
	static bool Copy( SVToTreeType &rToTree, typename SVToTreeType::SVBranchHandle toParent, const SVFromTreeType &rFromTree, typename SVFromTreeType::SVBranchHandle fromBranch );

};

#include "SVNavigateTree.inl"

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVNavigateTreeClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:47:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Sep 2012 15:21:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Sep 2012 18:24:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 2012 15:27:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Nov 2010 11:08:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:24:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:57:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
