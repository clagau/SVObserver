//*****************************************************************************
/// \copyright (c) 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the CTreeCtrl for the adjustment dialog for Draw-tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawTaskTree.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
extern const std::vector<std::pair<SvPb::ClassIdEnum, SvPb::SVObjectSubTypeEnum>> c_typeConvVec;

DrawTaskTree::DrawTaskTree(uint32_t inspectionId)
	: CTreeCtrl()
	, m_InspectionID(inspectionId)
{
}

DrawTaskTree::~DrawTaskTree()
{}

void DrawTaskTree::OnBeginLabelEdit(NMHDR*, LRESULT* pResult)
{
	auto item = GetSelectedItem();
	if (0 != item)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(item));
		if (nullptr != pData && DrawNodeType::BaseImage != pData->m_type && DrawNodeSubType::MainNode == pData->m_subType)
		{
			m_treeLabelEditing = true;
			*pResult = FALSE;
			return;
		}
	}

	//TRUE, stop editing.
	*pResult = TRUE;
}

void DrawTaskTree::OnEndLabelEdit(NMHDR*, LRESULT* pResult)
{
	*pResult = FALSE;
	m_treeLabelEditing = false;
	auto item = GetSelectedItem();
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(item));
	if (nullptr == pData || SvDef::InvalidObjectId == pData->m_objectId)
	{
		return;
	}

	CEdit* pEdit = GetEditControl();
	if (nullptr == pEdit)
	{
		return;
	}

	CString text;
	pEdit->GetWindowText(text);
	std::string newName = SvUl::Trim(text);
	SvUl::RemoveCharacters(newName, SvDef::cGeneralExcludeChars);

	if (newName.empty())
	{
		return;
	}

	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_setobjectnamerequest();
	pRequest->set_objectid(pData->m_objectId);
	pRequest->set_objectname(newName.c_str());

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	if (S_OK != hr)
	{
		return;
	}

	if (0 != newName.compare(text))
	{
		SetItemText(item, newName.c_str());
		UpdateData();
		return;
	}

	*pResult = true;
}

bool DrawTaskTree::OnDragTree(const NM_TREEVIEW& rNMHDR)
{
	HTREEITEM    hItem = rNMHDR.itemNew.hItem;
	if (!hItem)
	{
		return false;
	}

	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr == pData || SvDef::InvalidObjectId == pData->m_objectId || DrawNodeType::BaseImage == pData->m_type || DrawNodeSubType::MainNode != pData->m_subType)
	{
		return false;
	}

	SelectItem(hItem);

	//m_pDragImgList = m_treeCtrl.CreateDragImage(hItem);
	//if (!m_pDragImgList)
	//{
	//	return false;
	//}
	m_hDragItem = hItem;

	//	m_pDragImgList->BeginDrag(0, CPoint(0, 0));
	//	m_pDragImgList->DragEnter(this, pNMTreeView->ptDrag);


	m_hDragTarget = NULL;
	return true;
}

void DrawTaskTree::OnMouseMove(UINT, CPoint point)
{
	// highlight target
	TVHITTESTINFO tvHit;
	tvHit.pt = point;
	HTREEITEM hTarget = HitTest(&tvHit);

	if (hTarget)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hTarget));
		if (nullptr != pData && SvDef::InvalidObjectId != pData->m_objectId && DrawNodeType::BaseImage != pData->m_type && DrawNodeSubType::MainNode == pData->m_subType &&
			hTarget != m_hDragTarget)
		{                                                     // this test avoids flickering
			//m_pDragImgList->DragShowNolock(false);
			SelectDropTarget(hTarget);
			//m_pDragImgList->DragShowNolock(true);
			m_hDragTarget = hTarget;
		}
	}

	// move image being dragged
	//m_pDragImgList->DragMove(point);
}

bool DrawTaskTree::OnLButtonUp()
{
	SelectDropTarget(NULL);
	SelectItem(m_hDragItem);

	if (m_hDragTarget && (m_hDragTarget != m_hDragItem))
	{
		auto* pOldData = reinterpret_cast<TreeNodeData*>(GetItemData(m_hDragItem));
		auto* pNewData = reinterpret_cast<TreeNodeData*>(GetItemData(m_hDragTarget));
		if (nullptr != pOldData && nullptr != pNewData && SvDef::InvalidObjectId != pOldData->m_objectId && SvDef::InvalidObjectId != pNewData->m_objectId)
		{
			//@TODO[MZA][10.20][22.10.2021] if Task in different level (when temp image is added) moveobject will not longer work.
			auto hParentTaget = GetParentItem(m_hDragTarget);
			auto* pOwnerData = reinterpret_cast<TreeNodeData*>(GetItemData(hParentTaget));
			if (nullptr != pOwnerData && SvDef::InvalidObjectId != pOwnerData->m_objectId)
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pRequest = requestCmd.mutable_moveobjectrequest();
				pRequest->set_parentid(pOwnerData->m_objectId);
				pRequest->set_objectid(pOldData->m_objectId);
				pRequest->set_movepreid(pNewData->m_objectId);
				pRequest->set_listmode(SvPb::MoveObjectRequest_ListEnum_TaskObjectList);

				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
				assert(S_OK == hr);
				if (S_OK == hr)
				{
					auto hPos = GetNextItem(m_hDragTarget, TVGN_PREVIOUS);
					moveTreeItem(m_hDragItem, hParentTaget, hPos);
					return true;
				}
			}
		}
	}
	
	return false;
}


HTREEITEM DrawTaskTree::InsertNode(uint32_t objectId, DrawNodeType type, DrawNodeSubType subTye, HTREEITEM hParent, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto pValueControl = std::make_shared<ValueController>(SvOg::BoundValues {m_InspectionID, objectId});
	pValueControl->Init();
	return InsertNode({objectId, type, subTye, pValueControl}, hParent, rName, hInsertAfter);
}

HTREEITEM DrawTaskTree::InsertNode(TreeNodeData&& rNodeIndex, HTREEITEM hParent, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto& rData = m_TreeDataVector.emplace_back(std::make_unique<TreeNodeData>());
	*rData.get() = std::move(rNodeIndex);
	HTREEITEM hItem = InsertItem(rName.c_str(), hParent, hInsertAfter);
	SetItemData(hItem, reinterpret_cast<DWORD_PTR>(rData.get()));
	return hItem;
}

std::pair<HTREEITEM, HTREEITEM> DrawTaskTree::prepareAddInto(HTREEITEM item, SvPb::ClassIdEnum classId, addPosEnum posEnum, SvPb::CreateObjectRequest& rCmd)
{
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(item));
	if (nullptr != pData)
	{
		std::pair<HTREEITEM, HTREEITEM> hIntoAfterPair = {item, TVI_LAST};
		rCmd.set_classid(classId);
		switch (posEnum)
		{
			case addPosEnum::After:
			{
				hIntoAfterPair.second = item;
				hIntoAfterPair.first = GetParentItem(item);
				auto* pDataParent = reinterpret_cast<TreeNodeData*>(GetItemData(hIntoAfterPair.first));
				if (pDataParent)
				{
					rCmd.set_ownerid(pDataParent->m_objectId);
					auto nextItem = GetNextItem(item, TVGN_NEXT);
					TreeNodeData* pDataNext = nullptr;
					if (nullptr != nextItem && nullptr != (pDataNext = reinterpret_cast<TreeNodeData*>(GetItemData(nextItem))))
					{
						rCmd.set_taskobjectinsertbeforeid(pDataNext->m_objectId);
					}
					else
					{
						rCmd.set_taskobjectinsertbeforeid(SvDef::InvalidObjectId);
					}
				}
				else
				{
					assert(false);
					return {nullptr, nullptr};
				}
				break;
			}
			case addPosEnum::Before:
			{
				hIntoAfterPair.second = GetNextItem(item, TVGN_PREVIOUS);
				hIntoAfterPair.first = GetParentItem(item);
				auto* pDataParent = reinterpret_cast<TreeNodeData*>(GetItemData(hIntoAfterPair.first));
				if (pDataParent)
				{
					rCmd.set_ownerid(pDataParent->m_objectId);
					rCmd.set_taskobjectinsertbeforeid(pData->m_objectId);
				}
				else
				{
					assert(false);
					return {nullptr, nullptr};
				}
				break;
			}
			case addPosEnum::IntoBegin:
				rCmd.set_ownerid(pData->m_objectId);
				rCmd.set_taskobjectpos(0);
				hIntoAfterPair.second = pData->m_lastInternItem;
				break;
			case addPosEnum::IntoEnd:
				rCmd.set_ownerid(pData->m_objectId);
				rCmd.set_taskobjectpos(-1);
				break;
		}
		return hIntoAfterPair;
	}
	return {nullptr, nullptr};
}

void DrawTaskTree::addNodes(SvPb::SVObjectSubTypeEnum type, HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	switch (type)
	{
		case SvPb::DrawRectangleTaskType:
			addRectangleNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawOvalTaskType:
			addOvalNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawSegmentTaskType:
			addSegmentNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawTriangleTaskType:
			addTriangleNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawLineTaskType:
			addLinesNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawPointsTaskType:
			addPointsNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawPolygonTaskType:
			addPolygonNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawTextTaskType:
			addTextNodes(parentItem, objectId, rName, hInsertAfter);
			break;
		case SvPb::DrawBucketFillTaskType:
			addBucketFillNodes(parentItem, objectId, rName, hInsertAfter);
			break;
	}
}

void DrawTaskTree::clearItem(HTREEITEM item)
{
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(item));
	if (nullptr != pData)
	{
		auto nextItem = GetNextItem(pData->m_lastInternItem, TVGN_NEXT);
		while (nextItem)
		{
			deleteItem(nextItem);
			nextItem = GetNextItem(pData->m_lastInternItem, TVGN_NEXT);
		}
	}
}

void DrawTaskTree::deleteItem(HTREEITEM item)
{
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(item));
	if (nullptr != pData)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_deleteobjectrequest();
		pRequest->set_objectid(pData->m_objectId);

		/*HRESULT hr = */SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		DeleteItem(item);
	}
}

HTREEITEM DrawTaskTree::moveTreeItem(HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemPos)
{
	if (hItem == NULL || hItemTo == NULL)
	{
		return NULL;
	}

	if (hItem == hItemTo /*|| hItemTo == GetParentItem(hItem)*/)
	{
		return hItem;
	}

	// check we're not trying to move to a descendant
	HTREEITEM hItemParent = hItemTo;
	while (hItemParent != TVI_ROOT &&
		  (hItemParent = GetParentItem(hItemParent)) != NULL)
	{
		if (hItemParent == hItem)
		{
			return NULL;
		}
	}

	// copy items to new location, recursively, then delete old hierarchy
	// get text, and other info
	CString sText = GetItemText(hItem);
	TVINSERTSTRUCT tvis;
	tvis.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM |
		TVIF_SELECTEDIMAGE | TVIF_STATE;
	tvis.item.hItem = hItem;
	// we don't want to copy selection/expanded state etc
	tvis.item.stateMask = (UINT)-1 & ~(TVIS_DROPHILITED | TVIS_EXPANDED |
			  TVIS_EXPANDEDONCE | TVIS_EXPANDPARTIAL | TVIS_SELECTED);
	GetItem(&tvis.item);
	tvis.hParent = hItemTo;
	tvis.hInsertAfter = hItemPos;

	HTREEITEM hItemNew = InsertItem(&tvis);
	SetItemText(hItemNew, sText);

	// now move children to under new item
	HTREEITEM hItemChild = GetChildItem(hItem);
	while (hItemChild != NULL)
	{
		HTREEITEM hItemNextChild = GetNextSiblingItem(hItemChild);
		moveTreeItem(hItemChild, hItemNew);
		hItemChild = hItemNextChild;
	}

	if (GetSelectedItem() == hItem)
	{
		SelectItem(hItemNew);
	}
	// clear item data, so nothing tries to delete stuff based on lParam
	SetItemData(hItem, 0);
	// no (more) children, so we can safely delete top item
	DeleteItem(hItem);

	return hItemNew;
}

void DrawTaskTree::addRectangleNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Rectangle, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Rectangle, DrawNodeSubType::Position, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Rectangle, DrawNodeSubType::SizeData, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Rectangle, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Rectangle, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addOvalNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Oval, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Oval, DrawNodeSubType::Position, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Oval, DrawNodeSubType::SizeData, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Oval, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Oval, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addSegmentNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Segment, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Segment, DrawNodeSubType::Position, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Segment, DrawNodeSubType::SizeData, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Segment, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Segment, DrawNodeSubType::Angle, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Segment, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addTriangleNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Triangle, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Triangle, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Triangle, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Triangle, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addLinesNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Lines, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Lines, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Lines, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Lines, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addPointsNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Points, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Points, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Points, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addPolygonNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Polygon, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Polygon, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Polygon, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Polygon, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addTextNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::Text, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::Text, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		InsertNode({objectId, DrawNodeType::Text, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::Text, DrawNodeSubType::GeneralData, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}

void DrawTaskTree::addBucketFillNodes(HTREEITEM parentItem, uint32_t objectId, const std::string& rName, HTREEITEM hInsertAfter)
{
	auto hItem = InsertNode(objectId, DrawNodeType::BucketFill, DrawNodeSubType::MainNode, parentItem, rName, hInsertAfter);
	auto* pData = reinterpret_cast<TreeNodeData*>(GetItemData(hItem));
	if (nullptr != pData)
	{
		InsertNode({objectId, DrawNodeType::BucketFill, DrawNodeSubType::Points, pData->m_pValues}, hItem, "");
		auto hLastInterNode = InsertNode({objectId, DrawNodeType::BucketFill, DrawNodeSubType::Color, pData->m_pValues}, hItem, "");
		pData->m_lastInternItem = hLastInterNode;
	}
}
} //namespace SvOg