//*****************************************************************************
/// \copyright (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the CTreeCtrl for the adjustment dialog for Draw-tool
//*****************************************************************************
#pragma once

#pragma region Includes
#include "DrawToolHelperStructs.h"
#pragma endregion Includes


namespace SvOgu
{
class DrawTaskTree : public CTreeCtrl
{
	DECLARE_MESSAGE_MAP()

public:
	explicit DrawTaskTree(uint32_t inspectionId);
	virtual ~DrawTaskTree();

	enum class addPosEnum
	{
		After,
		Before,
		IntoBegin,
		IntoEnd
	};

public:
	void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnDragTree(const NM_TREEVIEW& rNMHDR);
	void OnMouseMove(UINT nFlags, CPoint point);
	bool OnLButtonUp();
	void OnRButtonDown(UINT Flags, CPoint Point);
	void OnExpandAll();
	void OnExpandSelectedItems();
	void OnCollapseAll();
	void OnCollapseTo2ndLevel();

	HTREEITEM InsertNode(uint32_t objectId, DrawNodeType type, DrawNodeSubType subTye, HTREEITEM hParent, const std::string& rName, HTREEITEM hInsertAfter);
	HTREEITEM InsertNode(TreeNodeData&& rData, HTREEITEM hParent, const std::string& rName, HTREEITEM hInsertAfter = TVI_LAST);
	std::pair< HTREEITEM, HTREEITEM> prepareAddInto(HTREEITEM item, SvPb::ClassIdEnum classId, addPosEnum posEnum, SvPb::CreateObjectRequest& rCmd);
	void addNodes(SvPb::SVObjectSubTypeEnum type, HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter = TVI_LAST);
	void clearItem(HTREEITEM item);
	void deleteItem(HTREEITEM item);
	bool isLableEditing() const { return m_treeLabelEditing; };

private:
	HTREEITEM moveTreeItem(HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemPos = TVI_LAST);
	void expandAllChild(HTREEITEM Item, UINT nCode);

	void addRectangleNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addOvalNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addSegmentNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addTriangleNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addLinesNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addPointsNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addPolygonNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addTextNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);
	void addBucketFillNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter);

private:
	uint32_t m_InspectionID;

	bool m_treeLabelEditing = false;
	HTREEITEM         m_hDragItem;
	HTREEITEM         m_hDragTarget;
	HTREEITEM m_RButtonItem = nullptr;

	std::vector<std::unique_ptr<TreeNodeData>> m_TreeDataVector;
};
} //namespace SvOgu
