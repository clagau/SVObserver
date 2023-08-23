//*****************************************************************************
// \copyright COPYRIGHT (c) 2021, 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ConfigurationOuttakes.h
/// contains configuration-related functions that were taken out of SVObserver in SVO-3442. 
/// These should be transferred to other source files when possible
//******************************************************************************


#pragma once

#include "SVOGuiUtility/SVGlobal.h"

std::pair<bool, bool> OkToClose(bool AskForSavingOrClosing);

bool CreateAndUseConfigAssistant(bool newConfiguration);
bool PrepareForNewConfiguration();
HRESULT DestroyConfig(bool AskForSavingOrClosing = true, bool CloseWithoutHint = false);

void GetAndDestroyConfigurationObject();

const std::string& getConfigPathName();
const std::string& getConfigFileName();
const std::string getConfigFullFileName();
bool setConfigFullFileName(LPCTSTR csFullFileName, bool bLoadFile);
bool fileSaveAsSVXWrapper(const std::string& rFileName, bool resetAutoSave);
std::string GetSvxFileNameOnly();

void SaveConfig(bool saveAs = false);
HRESULT LoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType type);
HRESULT SavePackedConfiguration(LPCTSTR pFileName);

std::string GetConfigFileNameOnly();

void SetFullSvxFileName(const std::string& rName);
void LoadSvxFilenameFromManager();
void SetConfigFileNameOnly(const std::string& rOriginalFile);
void RemoveSvxFilenameFromManager();
std::string GetFullSvxFileName();
void AddSvxFilenameToManager();
