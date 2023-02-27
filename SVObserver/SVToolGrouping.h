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
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectWriter;
struct ObjectInfo;
typedef std::vector<ObjectInfo> ObjectInfoVector;
}

class ToolGroupData
{
public:
	enum Type
	{
		Tool = 1,
		StartOfGroup = 2,
		EndOfGroup = 3
	};
	std::string m_name;
	std::string m_endName;
	Type m_type;
	bool m_bCollapsed;
	std::string m_comment;

	ToolGroupData(Type rType, const std::string& rName, bool collapsed = false) : m_type(rType), m_name(rName), m_bCollapsed(collapsed) {}
	ToolGroupData(Type rType, const std::string& rName, const std::string& rEndName, bool bCollapsed) : m_type(rType), m_name(rName), m_endName(rEndName), m_bCollapsed(bCollapsed) {}
};

typedef std::pair<std::string, ToolGroupData> ToolGroup;
typedef std::list<ToolGroup> ToolGroupList;

class SVToolGrouping
{
public:
	ToolGroupList m_ToolGroups;
	bool Correct(const SvOi::ObjectInfoVector&  toolsetinfo, int& rNchanged);
	void SetCollapseFromOldConfig(const SvOi::ObjectInfoVector& toolsetinfo);

	std::string GetDefaultName() const;
	//************************************
	//! Searches the name in the list of all tool and group names case insensitive
	//! \param rName [in]
	//! \param pExclude [in] if not null and the name is found (case sensitive)  this is ignored 
	//! \returns bool true if the name is unique 
	//************************************
	bool IsNameUnique(const std::string& rName, const std::string& rExclude = _T("")) const;

	std::string makeNameUnique(const std::string& rOriginalName, const std::vector<std::string>& rAdditionalNames, bool useExplorerStyle = true) const;
	std::string GetToolToInsertBefore(const std::string& rName) const;

	void AddGroup(const std::string& rName, const std::string& rInsertBefore = std::string());
	bool AddEndGroup(const std::string& rGroup, const std::string& rInsertBefore);
	void AddTool(const std::string& rName, const std::string& rInsertBefore = std::string());

	bool RemoveGroup(const std::string& rName);
	bool RemoveTool(const std::string& rName);

	bool RenameItem(const std::string& rOldName, const std::string& rNewName);
	std::string FindCandidateStartGroup(const std::string& rName) const;
	std::string FindGroup(const std::string& rName) const;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;
	HRESULT SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent); // Load
	bool GetParameters(SvOi::IObjectWriter& rWriter); // Save

	bool IsStartTag(const std::string& rName, bool& bState) const;
	bool IsStartTag(const std::string& rName) const;
	bool IsEndTag(const std::string& rName) const;

	/// Return if object is collapsed. I
	/// If it is a real tool, use toolId, because unique. Only if it is not a tool (grouping) use name.)
	/// \param toolId [in]
	/// \param rName [in]
	/// \returns bool
	bool IsCollapsed(uint32_t toolId) const;
	bool IsCollapsed(const std::string& rName) const;
	/// Set the state if a object is collapsed or not.
	/// If it is a real tool, use toolId, because unique. Only if it is not a tool (grouping) use name.)
	/// \param toolId [in]
	/// \param rName [in]
	/// \param bCollapse [in]
	/// \returns bool Return if set succeeded.
	bool Collapse(uint32_t toolId, bool bCollapse);
	bool Collapse(const std::string& rName, bool bCollapse);

	typedef ToolGroupList::iterator iterator;
	iterator begin() ;
	iterator end() ;
	iterator erase(iterator it);

	iterator find(const std::string& rName) ;
	bool empty() const;
	size_t size() const;
	/// return the number of items in the view, whereby  loop tool with subtools is counted as one
	int GetNumberOfVisibleItems();

	//************************************
	/// Get the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \returns String - which is the associated comment
	//************************************
	std::string GetComment(const std::string& rName) const;

	//************************************
	/// Set the Comment for the Start or End Group
	/// \param rName [in] The name of the start or end group
	/// \param rComment [in] the comment to associate with the group
	//************************************
	void SetComment(const std::string& rName, const std::string& rComment);

	void clear() { m_ToolGroups.clear(); }
private:
	HRESULT LoadTools(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVToolGrouping& rGroupings);
	std::vector<uint32_t> m_CollapseToolIds;
};

