// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GuiHelpers.h
/// Functions taken out of the SVObserverApp that simplify access to GUI functionality

#include "SVOGuiUtility/SVGlobal.h"

class SVIODoc;

void HideTabsAfterOpenSvxFile(SVIODoc* pIODoc);
void HideTabsAfterNewSVIODoc(SVIODoc* pIODoc);

void UpdateRemoteInputTabs(SVIODoc* pIODoc);

int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);
bool AddMenuItem(HMENU hTargetMenu, const std::string& itemText, UINT itemID);
bool RemoveMenu(HMENU hTargetMenu, const std::string& itemText);
