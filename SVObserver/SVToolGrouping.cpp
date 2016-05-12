//******************************************************************************
//* .Module Name     : SVToolGrouping
//* .File Name       : $Workfile:   SVToolGrouping.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.11  $
//* .Check In Date   : $Date:   12 Feb 2015 13:45:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <sstream>
#include "SVToolGrouping.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVGlobal.h"

#pragma endregion Includes

static const String scDefaultToolGroupName = _T("Group");
static const String scEndPrefix = _T("End ");

String SVToolGrouping::GetDefaultName() const
{
	String name = scDefaultToolGroupName;
	int cnt = 0;
	while (!IsNameUnique(name))
	{
		std::basic_stringstream<TCHAR> os;
		os << ++cnt;
		name = scDefaultToolGroupName + os.str();
	}
	return name;
}

bool SVToolGrouping::IsNameUnique(const String& rName, LPCTSTR pExclude) const
{
	String nameLower;
	std::transform(rName.begin(), rName.end(), std::back_inserter(nameLower), ::tolower);

	bool bRetVal = std::none_of(m_list.begin(), m_list.end(), 
		[&nameLower, pExclude, rName](const ToolGroup& rGroup)->bool 
	{ 
		if( nullptr != pExclude )
		{
			if( 0 == rGroup.first.compare(pExclude) )
			{
				return  false;
			}
		}
		// case insensitive compare
		String itemNameLower;
		std::transform(rGroup.first.begin(), rGroup.first.end(), std::back_inserter(itemNameLower), ::tolower);
		return (itemNameLower == nameLower); 
	});

	return bRetVal;
}

String SVToolGrouping::MakeNumericUniqueName(const String& rName) const
{
	String newName = rName;
	String nameLower;
	std::transform(rName.begin(), rName.end(), std::back_inserter(nameLower), ::tolower);
	
	//This strips any numbers at the end of the name
	size_t last_char_pos = nameLower.find_last_not_of(_T("0123456789"));
	if (last_char_pos != String::npos)
	{
		nameLower = nameLower.substr(0, last_char_pos + 1);
	}

	int num = 0;
	// find all names that start with this name (case insensitive)
	std::for_each(m_list.begin(), m_list.end(), [&nameLower, &num](const ToolGroup& rGroup)->void
	{ 
		String itemNameLower;
		std::transform(rGroup.first.begin(), rGroup.first.end(),std::back_inserter(itemNameLower), ::tolower);
	    size_t pos = itemNameLower.find(nameLower);
		if (0 == pos)
		{
			// check if the base name is augmented with numeric values
			if (itemNameLower == nameLower)
			{
				// check for trailing digits
				size_t last_char_pos = itemNameLower.find_last_not_of(_T("0123456789"));
				if (last_char_pos != String::npos)
				{
					String val = itemNameLower.substr(last_char_pos + 1);
					if (!val.empty())
					{
						int lastNum = atoi(val.c_str());
						num = std::max(num, lastNum + 1);
					}
					else
					{
						num = std::max(num, 1);
					}
				}
				else
				{
					num = std::max(num, 1);
				}
			}
			else
			{
				// check for trailing digits
				size_t last_char_pos = itemNameLower.find_last_not_of(_T("0123456789"));
				if (last_char_pos != String::npos)
				{
					String val = itemNameLower.substr(last_char_pos + 1);
					if (!val.empty())
					{
						// convert and set max
						int lastNum = atoi(val.c_str());
						num = std::max(num, lastNum + 1);
					}
				}
			}
		}
	});
	
	// build new name
	if (num)
	{
		std::stringstream ss;
		// Get Base Name (can't end in a number)
		String baseName = rName;
		size_t last_char_pos = rName.find_last_not_of(_T("0123456789"));
		if (last_char_pos != String::npos)
		{
			baseName = rName.substr(0, last_char_pos + 1);
		}
		ss << baseName << num;
		newName = ss.str().c_str();
	}
	return newName;
}

String SVToolGrouping::GetToolToInsertBefore(const String& rName) const
{
	String toolName;
	if (!rName.empty())
	{
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first; });
		if (it != m_list.end())
		{
			if (ToolGroupData::StartOfGroup == it->second.m_type || ToolGroupData::EndOfGroup == it->second.m_type)
			{
				// Start of Group or End of Group selected - get first tool after selection
				while (++it != m_list.end() && toolName.empty())
				{
					if (it->second.m_type == ToolGroupData::Tool)
					{
						toolName = it->first;
					}
				}
			}
			else
			{
				toolName = it->first;
			}
		}
	}
	return toolName;
}

void SVToolGrouping::AddGroup(const String& rName, const String& rInsertBefore)
{
	ToolGroupList::iterator it = m_list.end();
	if (!rInsertBefore.empty())
	{
		it = std::find_if(m_list.begin(), m_list.end(),
		[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
	}
	ToolGroupList::iterator curIt = m_list.insert(it, std::make_pair(rName, ToolGroupData(ToolGroupData::StartOfGroup, rName)));

	// check if end group needs to be reassigned to this newly added group (split group)
	it = std::find_if(curIt, m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName != rGroup.first && (ToolGroupData::EndOfGroup == rGroup.second.m_type || ToolGroupData::StartOfGroup == rGroup.second.m_type); });
	if (it != m_list.end())
	{
		if (ToolGroupData::EndOfGroup == it->second.m_type)
		{
			String endName = it->first;
			// find current start group for end group and unassign
			it = std::find_if(m_list.begin(), m_list.end(),
				[&endName](const ToolGroup& rGroup)->bool { return endName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
			if (it != m_list.end())
			{
				it->second.m_endName.clear();
			}
			// reassign the end to the newly inserted group
			curIt->second.m_endName = endName;
		}
	}
}

bool SVToolGrouping::AddEndGroup(const String& rGroupName, const String& rInsertBefore)
{
	bool bRetVal = false;
	
	if (!rGroupName.empty())
	{
		String endName = scEndPrefix + rGroupName;
		if (!IsNameUnique(endName))
		{
			endName = MakeNumericUniqueName(endName);
		}
		ToolGroupList::iterator it = m_list.end();
		if (!rInsertBefore.empty())
		{
			it = std::find_if(m_list.begin(), m_list.end(),
			[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
			m_list.insert(it, std::make_pair(endName, ToolGroupData(ToolGroupData::EndOfGroup, rGroupName, endName, false)));

			// Find Start Group
			it = std::find_if(m_list.begin(), m_list.end(),
			[&rGroupName](const ToolGroup& rGroup)->bool { return ToolGroupData::StartOfGroup == rGroup.second.m_type && rGroupName == rGroup.first; } );
			if (it != m_list.end())
			{
				// Update Start group with End group name
				it->second.m_endName = endName;
			}
		}
		else
		{
			// Find End of this group
			it = std::find_if(m_list.begin(), m_list.end(),
			[&rGroupName](const ToolGroup& rGroup)->bool { return rGroupName == rGroup.first; } );
			if (it != m_list.end())
			{
				// Update Start group with End group name
				it->second.m_endName = endName;
				// Find the End of the group (next Start, or End of the list)
				it = std::find_if(it, m_list.end(),[&rGroupName](const ToolGroup& rGroup)->bool { return ToolGroupData::StartOfGroup == rGroup.second.m_type && rGroupName != rGroup.first; } );
				m_list.insert(it, std::make_pair(endName, ToolGroupData(ToolGroupData::EndOfGroup, rGroupName, endName, false)));
			}
		}
	}
	return bRetVal;
}

void SVToolGrouping::AddTool(const String& rName, const String& rInsertBefore)
{
	ToolGroupList::iterator it = m_list.end();
	if (!rInsertBefore.empty())
	{
		it = std::find_if(m_list.begin(), m_list.end(),
			[&rInsertBefore](const ToolGroup& rGroup)->bool { return rInsertBefore == rGroup.first; });
	}
	m_list.insert(it, std::make_pair(rName, ToolGroupData(ToolGroupData::Tool, rName)));
}

bool SVToolGrouping::RemoveGroup(const String& rName)
{
	bool bRetVal = false;
	if (IsEndTag(rName))
	{
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			// Remove End 
			m_list.erase(it);
			bRetVal = true;
		
			it = std::find_if(m_list.begin(), m_list.end(),
			[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
			if (it != m_list.end())
			{
				// Update Start
				it->second.m_endName.clear();
			}
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			String endName = it->second.m_endName;
			m_list.erase(it);
			it = std::find_if(m_list.begin(), m_list.end(),
			[&endName](const ToolGroup& rGroup)->bool { return (endName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
			if (it != m_list.end())
			{
				m_list.erase(it);
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool SVToolGrouping::RemoveTool(const String& rName)
{
	bool bRetVal = false;
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::Tool == rGroup.second.m_type); });
	if (it != m_list.end())
	{
		m_list.erase(it);
	}
	return bRetVal;
}

bool SVToolGrouping::RenameItem(const String& rOldName, const String& rNewName)
{
	bool bRetVal = false;
	if (IsEndTag(rOldName))
	{
		// Update End name in Start Group
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rOldName](const ToolGroup& rGroup)->bool { return (rOldName == rGroup.second.m_endName && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			// Update End name in Start Group
			it->second.m_endName = rNewName;
		}
	}
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
	[&rOldName](const ToolGroup& rGroup)->bool { return rOldName == rGroup.first; });
	if (it != m_list.end())
	{
		it->first = rNewName;
		it->second.m_name = rNewName;
		bRetVal = true;
	}
	return bRetVal;
}

String SVToolGrouping::FindCandidateStartGroup(const String& rName) const
{
	String group;
	ToolGroupList::const_reverse_iterator it = m_list.rbegin();
	if (!rName.empty())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return rName == rToolGroup.first; } );
	}
	if (it != m_list.rend())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return ToolGroupData::StartOfGroup == rToolGroup.second.m_type && rName != rToolGroup.first; } );
		if (it != m_list.rend() && it->second.m_endName.empty())
		{
			group = it->first;
		}
	}
	return group;
}

String SVToolGrouping::FindGroup(const String& rName) const
{
	String group;
	ToolGroupList::const_reverse_iterator it = m_list.rbegin();
	if (!rName.empty())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return rName == rToolGroup.first; } );
	}
	if (it != m_list.rend())
	{
		it = std::find_if(it, m_list.rend(), [&rName](const ToolGroup& rToolGroup)->bool { return ToolGroupData::StartOfGroup == rToolGroup.second.m_type && rName != rToolGroup.first; } );
		if (it != m_list.rend())
		{
			group = it->first;
		}
	}
	return group;
}

bool SVToolGrouping::IsStartTag(const String& rName, bool& bState) const
{
	bool bRetVal = false;
	bState = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bState = it->second.m_bCollapsed;
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::IsStartTag(const String& rName) const
{
	bool bRetVal = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::IsEndTag(const String& rName) const
{
	bool bRetVal = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVToolGrouping::HasEndTag(const String& rName) const
{
	bool bRetVal = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bRetVal = !it->second.m_endName.empty();
	}
	return bRetVal;
}

bool SVToolGrouping::IsCollapsed(const String& rName) const
{
	bool bCollapsed = false;
	ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		bCollapsed = it->second.m_bCollapsed;
	}
	return bCollapsed;
}

bool SVToolGrouping::Collapse(const String& rName, bool bCollapse)
{
	bool bRetVal = false;

	// mark them all as collapsed ? or just add a function to get the tools/end nodes
	ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type; });
	if (it != m_list.end())
	{
		it->second.m_bCollapsed = bCollapse;
		bRetVal = true;
	}
	return bRetVal;
}

HRESULT SVToolGrouping::LoadTools(SVTreeType& rTree, SVTreeType::SVBranchHandle htiTools, SVToolGrouping& rGroupings)
{
	HRESULT hr = S_OK;
	if (htiTools)
	{
		// Get the Tools
		SVToolGrouping::SVTreeType::SVLeafHandle htiLeaf;
		htiLeaf = rTree.getFirstLeaf( htiTools );
		while (S_OK == hr && rTree.isValidLeaf( htiTools, htiLeaf ) )
		{
			String leafName = rTree.getLeafName( htiLeaf );
			if (CTAG_TOOL == leafName)
			{
				_variant_t svValue;
				svValue = rTree.getLeafData( htiLeaf );
				if( VT_EMPTY != svValue.vt )
				{
					String toolName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
					if (!toolName.empty())
					{
						rGroupings.m_list.insert(rGroupings.m_list.end(), std::make_pair(toolName, ToolGroupData(ToolGroupData::Tool, toolName)));
					}
					else
					{
						hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
					}
				}
				else
				{
					hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
				}
			}
			else
			{
				hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
			}
			htiLeaf = rTree.getNextLeaf(htiTools, htiLeaf);
		}
	}
	return  hr;
}

// Load the Groupings
HRESULT SVToolGrouping::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	HRESULT hr = S_OK;
	SVToolGrouping groupings;
	SVTreeType::SVBranchHandle htiChild = nullptr;
	if (SVNavigateTree::GetItemBranch(rTree, CTAG_TOOLGROUPINGS, htiParent, htiChild))
	{
		_variant_t svValue;
		SVTreeType::SVBranchHandle htiSubChild( rTree.getFirstBranch( htiChild ) );
		;
		while (S_OK == hr && nullptr != htiSubChild)
		{
			// Will be either Tools or Group
			String name = rTree.getBranchName( htiSubChild );
			if (CTAG_GROUP == name)
			{
				// Read the Group data
				if (SVNavigateTree::GetItem(rTree, CTAG_STARTGROUP, htiSubChild, svValue))
				{
					String groupName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
					String endGroupName;
					String startGroupComment;
					String endGroupComment;
					bool bCollapsed = false;
					if (SVNavigateTree::GetItem(rTree, CTAG_COLLAPSED, htiSubChild, svValue))
					{
						bCollapsed = (VARIANT_TRUE == svValue.boolVal);

						if (SVNavigateTree::GetItem(rTree, CTAG_STARTGROUP_COMMENT, htiSubChild, svValue))
						{
							CString tmp(static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue)));
							::SVRemoveEscapedSpecialCharacters(tmp, true);
							startGroupComment = static_cast<LPCTSTR>(tmp);
						}
						if (SVNavigateTree::GetItem(rTree, CTAG_ENDGROUP, htiSubChild, svValue))
						{
							endGroupName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
							if (SVNavigateTree::GetItem(rTree, CTAG_ENDGROUP_COMMENT, htiSubChild, svValue))
							{
								CString tmp(static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue)));
								::SVRemoveEscapedSpecialCharacters(tmp, true);
								endGroupComment = static_cast<LPCTSTR>(tmp);
							}
						}
						groupings.m_list.insert(groupings.m_list.end(), std::make_pair(groupName, ToolGroupData(ToolGroupData::StartOfGroup, groupName, endGroupName, bCollapsed)));
						// Update Comments
						if (!startGroupComment.empty())
						{
							groupings.SetComment(groupName, startGroupComment);
						}
						// Get The Tools...
						SVTreeType::SVBranchHandle htiTools( rTree.getFirstBranch( htiSubChild ) );
						if (nullptr != htiTools)
						{
							hr = LoadTools(rTree, htiTools, groupings);
						}
						if (S_OK == hr)
						{
							// End Group is optional
							if (!endGroupName.empty())
							{
								groupings.m_list.insert(groupings.m_list.end(), std::make_pair(endGroupName, ToolGroupData(ToolGroupData::EndOfGroup, groupName, endGroupName, false)));
								if (!endGroupComment.empty())
								{
									groupings.SetComment(endGroupName, endGroupComment);
								}
							}
						}
					}
					else
					{
						hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
					}
				}
				else
				{
					hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
				}
			}
			else if (CTAG_TOOLS == name)
			{
				// Read The Tools
				hr = LoadTools(rTree, htiSubChild, groupings);
			}
			htiSubChild = rTree.getNextBranch(htiChild, htiSubChild);
		}
		if (S_OK == hr)
		{
			m_list.swap(groupings.m_list);
		}
	}
	return hr;
}

// Save the Groupings
/*
<NODE Name="CTAG_TOOLGROUPINGS">
  <NODE Name="CTAG_TOOLS">
	<DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	<DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
  </NODE>
  <NODE Name="CTAG_GROUP">
	<DATA Name=CTAG_STARTGROUP Type="VT_BSTR">Group1</DATA>
    <DATA Name=CTAG_COLLAPSED Type="VT_BOOL">TRUE</DATA>
	<DATA Name=CTAG_STARTGROUP_COMMENT Type="VT_BSTR">Optional Comment</DATA>
    <NODE Name="CTAG_TOOLS">
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	</NODE>
	<DATA Name=CTAG_ENDGROUP Type="VT_BSTR">EndGroup1</DATA>
	<DATA Name=CTAG_ENDGROUP_COMMENT Type="VT_BSTR">Optional Comment</DATA>
  </NODE>
  <NODE Name="CTAG_GROUP">
	<DATA Name=CTAG_NAME Type="VT_BSTR">Group2</DATA>
    <DATA Name=CTAG_COLLAPSED Type="VT_BOOL">TRUE</DATA>
    <NODE Name="CTAG_TOOLS">
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	</NODE>
  <NODE Name="CTAG_GROUP">
	<DATA Name=CTAG_STARTGROUP Type="VT_BSTR">Group3</DATA>
    <DATA Name=CTAG_COLLAPSED Type="VT_BOOL">FALSE</DATA>
    <NODE Name="CTAG_TOOLS">
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
  </NODE>
</NODE>

*/
bool SVToolGrouping::GetParameters(SVObjectWriter& rWriter)
{
	bool bRetVal = true;
	if (m_list.size())
	{
		bool bGroupActive = false;
		bool bToolListActive = false;
		rWriter.StartElement(CTAG_TOOLGROUPINGS);
		for (ToolGroupList::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
		{
			if (ToolGroupData::StartOfGroup == it->second.m_type)
			{
				if (bToolListActive)
				{
					rWriter.EndElement();
					bToolListActive = false;
				}
				if (bGroupActive)
				{
					rWriter.EndElement();
					bGroupActive = false;
				}
				rWriter.StartElement(CTAG_GROUP);
				bGroupActive = true;

				_bstr_t name(it->first.c_str());
				_variant_t value(name);
				rWriter.WriteAttribute(CTAG_STARTGROUP, value);

				CString tmp(it->second.m_comment.c_str());
				::SVAddEscapeSpecialCharacters(tmp, true);
				_bstr_t comment(tmp);
				_variant_t commentValue(comment);
				rWriter.WriteAttribute(CTAG_STARTGROUP_COMMENT, commentValue);
				
				_variant_t collapsedValue(it->second.m_bCollapsed);
				rWriter.WriteAttribute(CTAG_COLLAPSED, collapsedValue);
			}
			else if (ToolGroupData::EndOfGroup == it->second.m_type)
			{
				if (bToolListActive)
				{
					rWriter.EndElement();
					bToolListActive = false;
				}

				_bstr_t name(it->second.m_endName.c_str());
				_variant_t value(name);
				rWriter.WriteAttribute(CTAG_ENDGROUP, value);

				CString tmp(it->second.m_comment.c_str());
				::SVAddEscapeSpecialCharacters(tmp, true);
				_bstr_t comment(tmp);
				_variant_t commentValue(comment);
				rWriter.WriteAttribute(CTAG_ENDGROUP_COMMENT, commentValue);

				if (bGroupActive)
				{
					rWriter.EndElement();
					bGroupActive = false;
				}
			}
			else if (ToolGroupData::Tool == it->second.m_type)
			{
				if (!bToolListActive)
				{
					rWriter.StartElement(CTAG_TOOLS);
					bToolListActive = true;
				}
				_bstr_t name(it->first.c_str());
				_variant_t value(name);
				rWriter.WriteAttribute(CTAG_TOOL, value);
			}
		}
		if (bToolListActive)
		{
			rWriter.EndElement();
			bToolListActive = false;
		}
		if (bGroupActive)
		{
			rWriter.EndElement();
			bGroupActive = false;
		}
		// End Element for Toolgroupings
		rWriter.EndElement();
	}
	return bRetVal;
}

SVToolGrouping::const_iterator SVToolGrouping::begin() const
{
	return m_list.begin();
}

SVToolGrouping::const_iterator SVToolGrouping::end() const
{
	return m_list.end();
}

SVToolGrouping::const_iterator SVToolGrouping::find(const String& rName) const
{
	SVToolGrouping::const_iterator it = std::find_if(begin(), end(), 
		[&rName](const ToolGroup& rGroup)->bool { return rGroup.first == rName; });
	return it;
}

bool SVToolGrouping::empty() const
{
	return m_list.empty();
}

size_t SVToolGrouping::size() const
{
	return m_list.size();
}

String SVToolGrouping::GetComment(const String& rName) const
{
	String comment;
	if (IsEndTag(rName))
	{
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			comment = it->second.m_comment;
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::const_iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			comment = it->second.m_comment;
		}
	}
	return comment;
}

void SVToolGrouping::SetComment(const String& rName, const String& rComment)
{
	if (IsEndTag(rName))
	{
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::EndOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			it->second.m_comment = rComment;
		}
	}
	else if (IsStartTag(rName))
	{
		// Remove Start and End
		ToolGroupList::iterator it = std::find_if(m_list.begin(), m_list.end(),
		[&rName](const ToolGroup& rGroup)->bool { return (rName == rGroup.first && ToolGroupData::StartOfGroup == rGroup.second.m_type); });
		if (it != m_list.end())
		{
			it->second.m_comment = rComment;
		}
	}
}

