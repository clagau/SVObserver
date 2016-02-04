//******************************************************************************
//* .Module Name     : SVToolGrouping
//* .File Name       : $Workfile:   SVToolGrouping.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   12 Feb 2015 13:45:42  $
//******************************************************************************
#pragma once

#pragma region Includes
#include <list>
#include <string>
#include <utility>
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

typedef std::basic_string<TCHAR> String;

class ToolGroupData
{
public:
	enum Type
	{
		Tool = 1,
		StartOfGroup = 2,
		EndOfGroup = 3
	};
	String m_name;
	String m_endName;
	Type m_type;
	bool m_bCollapsed;
	String m_comment;

	ToolGroupData(Type rType, const String& rName) : m_type(rType), m_name(rName), m_bCollapsed(false) {}
	ToolGroupData(Type rType, const String& rName, const String& rEndName, bool bCollapsed) : m_type(rType), m_name(rName), m_endName(rEndName), m_bCollapsed(bCollapsed) {}
};

class SVToolGrouping
{
	typedef std::pair<String, ToolGroupData> ToolGroup;
	typedef std::list<ToolGroup> ToolGroupList;
	ToolGroupList m_list;

public:
	String GetDefaultName() const;
	//************************************
	//! Searches the name in the list of all tool and group names case insensitive
	//! \param rName [in]
	//! \param pExclude [in] if not null and the name is found (case sensitive)  this is ignored 
	//! \returns bool true if the name is unique 
	//************************************
	bool IsNameUnique(const String& rName, LPCTSTR pExclude = nullptr) const;

	String MakeNumericUniqueName(const String& rName) const;
	String GetToolToInsertBefore(const String& rName) const;

	void AddGroup(const String& rName, const String& rInsertBefore = String());
	bool AddEndGroup(const String& rGroup, const String& rInsertBefore);
	void AddTool(const String& rName, const String& rInsertBefore = String());

	bool RemoveGroup(const String& rName);
	bool RemoveTool(const String& rName);

	bool RenameItem(const String& rOldName, const String& rNewName);
	String FindCandidateStartGroup(const String& rName) const;
	String FindGroup(const String& rName) const;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;
	HRESULT SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent); // Load
	bool GetParameters(SVObjectWriter& rWriter); // Save

	bool IsStartTag(const String& rName, bool& bState) const;
	bool IsStartTag(const String& rNname) const;
	bool IsEndTag(const String& rName) const;
	bool HasEndTag(const String& rName) const;
	bool IsCollapsed(const String& rName) const;
	bool Collapse(const String& rName, bool bCollapse);

	typedef ToolGroupList::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	const_iterator find(const String& rName) const;
	bool empty() const;
	size_t size() const;

	//************************************
	/// Get the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \returns String - which is the associated comment
	//************************************
	String GetComment(const String& rName) const;

	//************************************
	/// Set the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \param rComment [in] the comment to associate with the group
	//************************************
	void SetComment(const String& rName, const String& rComment);

private:
	HRESULT LoadTools(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVToolGrouping& rGroupings);
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolGrouping.h_v  $
 * 
 *    Rev 1.5   12 Feb 2015 13:45:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  981
 * SCR Title:  Revise Tool Groupings to have comments
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetComment method
 * Added SetComment method
 * Revised  persistence methods to load and save group comments
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Nov 2014 16:41:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  969
 * SCR Title:  SVObserver Should Check the Name of Group Tools When Naming Other Tools
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed MakeNameUnique method.
 * Added MakeNumericUniqueName method.
 * Revised IsNameUnique to be case insensitive.
 * Revised AddEndGroup to call MakeNumericUniqueName if name is not unique.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Jul 2014 14:18:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added MakeNameUnique method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jun 2014 18:33:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FindCanfdidateStartGroup method to assist when adding an end group.
 * Revised AddEndGroup to update the start group with the end group name
 * Revised RemoveGroup to correct and issue with removing end groups.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jun 2014 14:20:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddEndGroup to allow adding end group before selection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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