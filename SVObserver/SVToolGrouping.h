//******************************************************************************
//* .Module Name     : SVToolGrouping
//* .File Name       : $Workfile:   SVToolGrouping.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   12 Feb 2015 13:45:42  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <list>
//Moved to precompiled header: #include <utility>
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

class ToolGroupData
{
public:
	enum Type
	{
		Tool = 1,
		StartOfGroup = 2,
		EndOfGroup = 3
	};
	SVString m_name;
	SVString m_endName;
	Type m_type;
	bool m_bCollapsed;
	SVString m_comment;

	ToolGroupData(Type rType, const SVString& rName) : m_type(rType), m_name(rName), m_bCollapsed(false) {}
	ToolGroupData(Type rType, const SVString& rName, const SVString& rEndName, bool bCollapsed) : m_type(rType), m_name(rName), m_endName(rEndName), m_bCollapsed(bCollapsed) {}
};

class SVToolGrouping
{
	typedef std::pair<SVString, ToolGroupData> ToolGroup;
	typedef std::list<ToolGroup> ToolGroupList;
	ToolGroupList m_list;

public:
	SVString GetDefaultName() const;
	//************************************
	//! Searches the name in the list of all tool and group names case insensitive
	//! \param rName [in]
	//! \param pExclude [in] if not null and the name is found (case sensitive)  this is ignored 
	//! \returns bool true if the name is unique 
	//************************************
	bool IsNameUnique(const SVString& rName, LPCTSTR pExclude = nullptr) const;

	SVString MakeNumericUniqueName(const SVString& rName) const;
	SVString GetToolToInsertBefore(const SVString& rName) const;

	void AddGroup(const SVString& rName, const SVString& rInsertBefore = SVString());
	bool AddEndGroup(const SVString& rGroup, const SVString& rInsertBefore);
	void AddTool(const SVString& rName, const SVString& rInsertBefore = SVString());

	bool RemoveGroup(const SVString& rName);
	bool RemoveTool(const SVString& rName);

	bool RenameItem(const SVString& rOldName, const SVString& rNewName);
	SVString FindCandidateStartGroup(const SVString& rName) const;
	SVString FindGroup(const SVString& rName) const;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;
	HRESULT SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent); // Load
	bool GetParameters(SVObjectWriter& rWriter); // Save

	bool IsStartTag(const SVString& rName, bool& bState) const;
	bool IsStartTag(const SVString& rName) const;
	bool IsEndTag(const SVString& rName) const;
	bool HasEndTag(const SVString& rName) const;
	bool IsCollapsed(const SVString& rName) const;
	bool Collapse(const SVString& rName, bool bCollapse);

	typedef ToolGroupList::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	const_iterator find(const SVString& rName) const;
	bool empty() const;
	size_t size() const;

	//************************************
	/// Get the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \returns String - which is the associated comment
	//************************************
	SVString GetComment(const SVString& rName) const;

	//************************************
	/// Set the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \param rComment [in] the comment to associate with the group
	//************************************
	void SetComment(const SVString& rName, const SVString& rComment);

private:
	HRESULT LoadTools(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVToolGrouping& rGroupings);
};

