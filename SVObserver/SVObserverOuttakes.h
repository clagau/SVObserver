// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVObserverOuttakes.h
/// Various functions taken out of the SVObserverApp

#include "SVGlobal.h"

class SVIODoc;
class CMultiDocTemplate;
class SVMainFrame;


CDocTemplate* CreateMultiDocTemplate1();
CDocTemplate* CreateMultiDocTemplate2();

void AddFileToConfig(LPCTSTR FilePath);
void RemoveFileFromConfig(LPCTSTR FilePath);

HRESULT RebuildOutputObjectListHelper(SVIODoc* pIODoc);

HRESULT CheckDrive(const std::string& p_strDrive);

CMultiDocTemplate* getIPDocTemplate();

bool LoadSvxFilePart1(unsigned long& ulSVOConfigVersion, unsigned long svoVersion, std::string svxFilePath, SVTreeType& rXmlTree);
std::pair<bool, HRESULT> LoadSvxFilePart2(bool isGlobalInit, SVIMProductEnum SVIMType, SVTreeType& rXmlTree, SVMainFrame* pMainFrame);




