// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file GuiHelpers.cpp
/// Functions taken out of the SVObserverApp that simplify access to GUI functionality

#include "stdafx.h"

#include "GuiHelpers.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h" 
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVOLibrary/SVHardwareManifest.h"

bool RecursivelyAddMenuItems(
	const MENUITEMINFO& rMenuItemInfo,
	const std::string& remainingText,
	bool bFoundSubMenu,
	UINT itemID,
	HMENU hTargetMenu,
	const std::string& rPopupMenuName);

bool AppendMenuIfNotFound(
	HMENU hTargetMenu,
	const std::string& rItemText,
	UINT itemID);

bool DoesPoupupMenuExist(
	HMENU hTargetMenu,
	const std::string& rItemText);


void UpdateRemoteInputTabs(SVIODoc* pIODoc)
{
	// Remote Inputs
	bool l_bHideIOTab = true;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		SVInputObjectList* pList = pConfig->GetInputObjectList();
		if (nullptr != pList)
		{
			if (pList->getRemoteInputCount() > 0)
			{
				l_bHideIOTab = false;
			}
		}
	}

	if (l_bHideIOTab && nullptr != pIODoc)
	{
		pIODoc->HideIOTab(SVIORemoteInputsViewID);
	}
}


void HideTabsAfterOpenSvxFile(SVIODoc* pIODoc)
{
	// Logic to remove unused IO Tabs Remote inputs.

	if (pIODoc != nullptr)
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			if (pConfig->GetRemoteOutputGroupCount() == 0)
			{
				pIODoc->HideIOTab(SVRemoteOutputsViewID);
			}

			const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
			if (!rList.size())
			{
				pIODoc->HideIOTab(SVRemoteMonitorListViewID);
			}
		}
		else
		{
			pIODoc->HideIOTab(SVRemoteOutputsViewID);
		}
		// Update Remote Inputs Tab
		UpdateRemoteInputTabs(pIODoc);
	}
}


void HideTabsAfterNewSVIODoc(SVIODoc* pIODoc)
{
	if (pIODoc != nullptr)
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			//Now that IO constructed check which tabs to hide
			if (false == SVHardwareManifest::isPlcSystem(pConfig->GetProductType()))
			{
				pIODoc->HideIOTab(SVIOPlcOutputsViewID);
			}
			if (false == SVHardwareManifest::isDiscreteIOSystem(pConfig->GetProductType()))
			{
				pIODoc->HideIOTab(SVIODiscreteInputsViewID);
				pIODoc->HideIOTab(SVIODiscreteOutputsViewID);
			}
		}
	}
}



// this is a recursive function which will attempt
// to add the item "itemText" to the menu with the
// given ID number. The "itemText" will be parsed for
// delimiting "\" characters for levels between
// popup menus. If a popup menu does not exist, it will
// be created and inserted at the end of the menu.
// ItemID of 0 will cause a separator to be added
bool AddMenuItem(
	HMENU hTargetMenu,
	const std::string& rItemText,
	UINT itemID)

{
	bool bSuccess = false;

	assert(rItemText.size() > 0);
	assert(nullptr != hTargetMenu);

	// first, does the menu item have
	// any required submenus to be found/created?
	size_t Pos = rItemText.find('\\');
	if (std::string::npos != Pos)
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		std::string popupMenuName = SvUl::Left(rItemText, Pos);

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		size_t RestSize = rItemText.size() - popupMenuName.size() - 1;
		std::string remainingText = SvUl::Right(rItemText, RestSize);

		// 3:See whether the popup menu already exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0;
			itemIndex < itemCount && !bFoundSubMenu; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu,
				itemIndex,
				TRUE,
				&menuItemInfo);
			if (menuItemInfo.hSubMenu != 0)
			{
				// this menu item is a popup menu (non popups give 0)
				TCHAR    buffer[MAX_PATH];
				::GetMenuString(
					hTargetMenu,
					itemIndex,
					buffer,
					MAX_PATH,
					MF_BYPOSITION);
				if (popupMenuName == buffer)
				{
					// this is the popup menu we have to add to
					bFoundSubMenu = true;
				}
			}
		}

		bSuccess = RecursivelyAddMenuItems(menuItemInfo, remainingText, bFoundSubMenu, itemID, hTargetMenu, popupMenuName);
	}
	else
	{
		if (AppendMenuIfNotFound(hTargetMenu, rItemText, itemID))
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}


bool RecursivelyAddMenuItems(
	const MENUITEMINFO& rMenuItemInfo,
	const std::string& remainingText,
	bool bFoundSubMenu,
	UINT itemID,
	HMENU hTargetMenu,
	const std::string& rPopupMenuName)
{
	// 4: If exists, do recursive call,
	// else create do recursive call
	// and then insert it
	if (bFoundSubMenu)
	{
		return  AddMenuItem(rMenuItemInfo.hSubMenu, remainingText, itemID);
	}
	else
	{
		// we need to create a new sub menu and insert it
		HMENU hPopupMenu = ::CreatePopupMenu();
		if (nullptr != hPopupMenu)
		{
			bool bSuccess = AddMenuItem(
				hPopupMenu,
				remainingText,
				itemID);

			if (bSuccess)
			{
				if (::AppendMenu(
					hTargetMenu,
					MF_POPUP,
					reinterpret_cast<UINT_PTR> (hPopupMenu),
					rPopupMenuName.c_str()) > 0)
				{
					bSuccess = true;
					// hPopupMenu now owned by hTargetMenu,
					// we do not need to destroy it
				}
				else
				{
					// failed to insert the popup menu
					bSuccess = false;
					// stop a resource leak
					::DestroyMenu(hPopupMenu);
				}
			}
			return bSuccess;
		}
	}
	return false;
}


bool AppendMenuIfNotFound(HMENU hTargetMenu, const std::string& rItemText, UINT itemID)
{
	// See whether the menu item already exists
	int itemCount = ::GetMenuItemCount(hTargetMenu);
	bool bFoundSubMenu = false;
	MENUITEMINFO menuItemInfo;

	memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
	menuItemInfo.cbSize = sizeof(MENUITEMINFO);
	menuItemInfo.fMask =
		MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
	for (int itemIndex = 0;
		itemIndex < itemCount && !bFoundSubMenu; itemIndex++)
	{
		::GetMenuItemInfo(
			hTargetMenu,
			itemIndex,
			TRUE,
			&menuItemInfo);
		if (menuItemInfo.wID == itemID)
		{
			// Set the inspection name.
			::ModifyMenuA(hTargetMenu, itemIndex, MF_BYPOSITION | MF_STRING, itemID, rItemText.c_str());
			bFoundSubMenu = true;
		}
	}

	// If not found then append menu.
	if (!bFoundSubMenu)
	{
		if (itemID != 0)
		{
			// its a normal menu command
			if (::AppendMenu(
				hTargetMenu,
				MF_ENABLED,
				itemID,
				rItemText.c_str()) > 0)
			{
				// we successfully added the item to the menu
				return true;
			}
		}
		else
		{
			// we are inserting a separator
			if (::AppendMenu(
				hTargetMenu,
				MF_SEPARATOR,
				itemID,
				rItemText.c_str()) > 0)
			{
				// we successfully added the separator to the menu
				return true;
			}
		}
	}
	return false;
}

// this is a recursive function which will attempt
// to remove the menu item with "itemText" from the menu with the
// target menu handle. "itemText" will be parsed for
// delimiting "\" characters for levels between
// popup menus. If the end popup menu exists, it will be deleted.
bool RemoveMenu(
	HMENU hTargetMenu,
	const std::string& rItemText)
{
	bool bSuccess = false;

	assert(rItemText.size() > 0);
	assert(nullptr != hTargetMenu);

	// first, does the menu item have
	// any required submenus to be found/created?
	size_t Pos = rItemText.find('\\');
	if (std::string::npos != Pos)
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		std::string popupMenuName = SvUl::Left(rItemText, Pos);

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		size_t RestSize = rItemText.size() - popupMenuName.size() - 1;
		std::string remainingText = SvUl::Right(rItemText, RestSize);

		// 3:See whether the popup menu already exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask =
			MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0;
			itemIndex < itemCount && !bFoundSubMenu; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu,
				itemIndex,
				TRUE,
				&menuItemInfo);
			if (menuItemInfo.hSubMenu != 0)
			{
				// this menu item is a popup menu (non popups give 0)
				TCHAR    buffer[MAX_PATH];
				::GetMenuString(
					hTargetMenu,
					itemIndex,
					buffer,
					MAX_PATH,
					MF_BYPOSITION);
				if (popupMenuName == buffer)
				{
					// this is the popup menu we are looking for.
					bFoundSubMenu = true;
				}
			}
		}
		// 4: If exists, do recursive call,
		if (bFoundSubMenu)
		{
			bSuccess = RemoveMenu(menuItemInfo.hSubMenu, remainingText);
		}
	}
	else
	{
		bSuccess = DoesPoupupMenuExist(hTargetMenu, rItemText);
	}

	return bSuccess;
}


bool DoesPoupupMenuExist(HMENU hTargetMenu, const std::string& rItemText)
{
	// See whether the popup menu exists
	int itemCount = ::GetMenuItemCount(hTargetMenu);
	MENUITEMINFO menuItemInfo;

	memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
	menuItemInfo.cbSize = sizeof(MENUITEMINFO);
	menuItemInfo.fMask =
		MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
	for (int itemIndex = 0; itemIndex < itemCount; itemIndex++)
	{
		::GetMenuItemInfo(
			hTargetMenu,
			itemIndex,
			TRUE,
			&menuItemInfo);
		if (menuItemInfo.hSubMenu != 0)
		{
			// this menu item is a popup menu (non popups give 0)
			TCHAR    buffer[MAX_PATH];
			::GetMenuString(
				hTargetMenu,
				itemIndex,
				buffer,
				MAX_PATH,
				MF_BYPOSITION);
			if (rItemText == buffer)
			{
				// this is the popup menu we have to remove
				return DeleteMenu(hTargetMenu, itemIndex, MF_BYPOSITION) ? true : false;
			}
		}
	}
	return false;
}


int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
	assert(Menu);
	assert(::IsMenu(Menu->GetSafeHmenu()));

	int count = Menu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		CString str;
		if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
			(strcmp(str, MenuString) == 0))
		{
			return i;
		}
	}

	return -1;
}
