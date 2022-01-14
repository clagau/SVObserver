// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVXFunctions.h 
/// Various functions concerned with SVX and SVZ files that were taken out of the SVObserverApp class

HRESULT LoadSvxFile(unsigned long svoVersion, const std::string& rSvxFilePath, const std::string& rSvzFilePath, SVIMProductEnum SVIMType, SVMainFrame* pMainFrame, SvOi::TRC_RAIIPtr globalInitRAII);
bool LoadSvxFilePart1(unsigned long& ulSVOConfigVersion, unsigned long svoVersion, std::string svxFilePath, SVTreeType& rXmlTree);
std::pair<bool, HRESULT> LoadSvxFilePart2(SvOi::TRC_RAIIPtr globalInitRAII, SVIMProductEnum SVIMType, SVTreeType& rXmlTree, SVMainFrame* pMainFrame);
