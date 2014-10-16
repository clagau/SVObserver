//******************************************************************************
//* .Module Name     : SVToolGrouping
//* .File Name       : $Workfile:   SVToolGrouping.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   15 Oct 2014 17:27:52  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <algorithm>
#include <sstream>
#include "SVToolGrouping.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"

#pragma endregion Includes

static const String scDefaultToolGroupName = _T("Group");
static const String scEndPrefix = _T("End ");
static const String scDupSuffix = _T("DUP");

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

bool SVToolGrouping::IsNameUnique(const String& rName) const
{
	bool bRetVal = std::none_of(m_list.begin(), m_list.end(), 
		[&rName](const ToolGroup& rGroup)->bool { return rName == rGroup.first; });

	return bRetVal;
}

String SVToolGrouping::MakeNameUnique(const String& rName) const
{
	String newName = rName;
	while (!IsNameUnique(newName))
	{
		newName += scDupSuffix;
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
		while (!IsNameUnique(endName))
		{
			endName += scDupSuffix;
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
		HRESULT hrTemp = rTree.GetFirstLeaf(htiTools, htiLeaf);
		while (S_OK == hr && S_OK == hrTemp)
		{
			_bstr_t name;
			hr = rTree.GetLeafName(htiLeaf, name.GetBSTR());
			if (S_OK == hr)
			{
				String leafName = static_cast<LPCTSTR>(name);
				if (CTAG_TOOL == leafName)
				{
					_variant_t svValue;
					hr = rTree.GetLeafData(htiLeaf, svValue);
					if (S_OK == hr)
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
			}
			else
			{
				hr = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING;
			}
			hrTemp = rTree.GetNextLeaf(htiTools, htiLeaf);
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
	if (SVNavigateTreeClass::GetItemBranch(rTree, CTAG_TOOLGROUPINGS, htiParent, htiChild))
	{
		_variant_t svValue;
		SVTreeType::SVBranchHandle htiSubChild = nullptr;
		rTree.GetFirstBranch(htiChild, htiSubChild);
		while (S_OK == hr && nullptr != htiSubChild)
		{
			_bstr_t name;
			// Will be either Tools or Group
			hr = rTree.GetBranchName(htiSubChild, name.GetBSTR());
			if (S_OK == hr)
			{
				if (CTAG_GROUP == String(static_cast<LPCTSTR>(name)))
				{
					// Read the Group data
					if (SVNavigateTreeClass::GetItem(rTree, CTAG_STARTGROUP, htiSubChild, svValue))
					{
						String groupName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
						String endGroupName;
						bool bCollapsed = false;
						if (SVNavigateTreeClass::GetItem(rTree, CTAG_COLLAPSED, htiSubChild, svValue))
						{
							bCollapsed = (VARIANT_TRUE == svValue.boolVal);
							if (SVNavigateTreeClass::GetItem(rTree, CTAG_ENDGROUP, htiSubChild, svValue))
							{
								 endGroupName = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
							}
							groupings.m_list.insert(groupings.m_list.end(), std::make_pair(groupName, ToolGroupData(ToolGroupData::StartOfGroup, groupName, endGroupName, bCollapsed)));

							// Get The Tools...
							SVTreeType::SVBranchHandle htiTools = nullptr;
							rTree.GetFirstBranch(htiSubChild, htiTools);
							if (htiTools)
							{
								hr = LoadTools(rTree, htiTools, groupings);
							}
							if (S_OK == hr)
							{
								// End Group is optional
								if (!endGroupName.empty())
								{
									groupings.m_list.insert(groupings.m_list.end(), std::make_pair(endGroupName, ToolGroupData(ToolGroupData::EndOfGroup, groupName, endGroupName, false)));
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
				else if (CTAG_TOOLS == String(static_cast<LPCTSTR>(name)))
				{
					// Read The Tools
					hr = LoadTools(rTree, htiSubChild, groupings);
				}
			}
			rTree.GetNextBranch(htiChild, htiSubChild);
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
    <NODE Name="CTAG_TOOLS">
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	  <DATA Name=CTAG_TOOL Type="VT_BSTR">Dotted.Name</DATA>
	</NODE>
	<DATA Name=CTAG_ENDGROUP Type="VT_BSTR">EndGroup1</DATA>
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolGrouping.cpp_v  $
 * 
 *    Rev 1.7   15 Oct 2014 17:27:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  963
 * SCR Title:  Missing End Group When Saving and Loading Tool Group
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetParameters to construct the ToolGroupData for the end group correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Aug 2014 16:09:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetToolToInsertBefore method to correct an issue when inserting a tool before the Start of Group.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Jul 2014 14:18:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added MakeNameUnique method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jun 2014 13:17:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddGroup method to check and/or reassign end group if necessary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2014 10:42:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   modified order of saving end element for tool group and EndGroup name in GetParameters.
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
 *    Rev 1.1   18 Jun 2014 14:20:00   sjones
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