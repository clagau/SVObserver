//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionTreeParser
//* .File Name       : $Workfile:   SVInspectionTreeParser.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:10:42  $
//******************************************************************************
#ifndef INCL_SVINSPECTIONTREEPARSER_H
#define INCL_SVINSPECTIONTREEPARSER_H

#include "SVObjectScriptParserBase.h"
#include "SVObjectLibrary/SVObjectScriptEnums.h"

class SVObjectClass;
class SVObjectScriptParserClass;

template< typename SVTreeType >
class SVInspectionTreeParser : public SVObjectScriptParserBase
{
private:
	SVTreeType& m_rTree;
	typename SVTreeType::SVBranchHandle m_rootItem;
	size_t m_count;
	size_t m_totalSize;
	bool m_ReplaceUniqueID;

public:
	SVInspectionTreeParser(SVTreeType& rTreeCtrl, typename SVTreeType::SVBranchHandle hItem, unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVInspectionTreeParser();
	virtual HRESULT DoParse();
	virtual size_t GetTotal() const;

	static HRESULT CreateInspectionObject(GUID& inspectionGuid, SVTreeType& p_rTree, typename SVTreeType::SVBranchHandle hItem);

private:
	HRESULT Process(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessChildren(typename SVTreeType::SVBranchHandle hParentItem, const GUID& ownerID);

	HRESULT ProcessFriend(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessFriends(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessEmbeddeds(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessEmbedded(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessEmbeddedValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID, const _variant_t& defaultValue, SVObjectScriptDataObjectTypeEnum dataType);

	HRESULT ProcessBranchObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID);
	HRESULT ProcessLeafObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID);

	HRESULT ProcessEquation(const GUID& ownerID, const _variant_t& equation);

	HRESULT ProcessMaskData(const GUID& ownerID, const _variant_t& maskData);

	HRESULT ProcessInputs(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessAttributes(const GUID& ownerID, const GUID& objectID, typename SVTreeType::SVBranchHandle hItem);

	bool GetItemValue(const SVString& tag, typename SVTreeType::SVBranchHandle hItem, _variant_t& value);
	bool GetValues(typename SVTreeType::SVBranchHandle hItem, const SVString& tag, SVVariantList& values);
	bool HasTag(typename SVTreeType::SVBranchHandle hItem, const SVString& tag);
};

#include "SVInspectionTreeParser.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInspectionTreeParser.h_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:10:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed parser to be able to parse a single tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:03:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Sep 2012 14:28:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Migrated object to templates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Sep 2012 15:19:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Sep 2012 17:13:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Process Shildren to handle the missing Mask Data processing and ass a new Process Mask Data method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2012 12:29:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Object Attribute filter
 * 
 * Added Object Attribute type table
 * 
 * Added new Process Object Values
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 14:00:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in of new files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
