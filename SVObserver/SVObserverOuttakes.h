//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVObserverOuttakes.h
/// contains functions that were taken out of SVObserver in SVO-3442. 
/// These should be transferred to other source files when possible
//******************************************************************************

#pragma once

void StopSvo();
void StopAll();
bool AddSecurityNode(HMODULE hMessageDll, long lId, LPCTSTR NTGroup, bool bForcePrompt = false);
bool AddSecurityNode(HMODULE hMessageDll, long lId);
void executePreOrPostExecutionFile(const std::string& filepath, bool inRunMode = true);
HRESULT GetTriggersAndCounts(std::string& rTriggerCounts);
HRESULT ConnectToolsetBuffers();
HRESULT DisconnectToolsetBuffers();
HRESULT ConnectCameraBuffers();
HRESULT DisconnectCameraBuffers();
HRESULT ConnectCameras();
HRESULT DisconnectCameras();

class SVIPDoc* getCurrentDocument();
bool IsForcedImageUpdateActive();
unsigned char GetForcedImageUpdateTimeInSeconds();

long getMaxPreloadFileNumber();
long getPreloadTimeDelay();


long getSvoOfflineCount();

void StartTrigger(SVConfigurationObject* pConfig);///< can throw an exception!

void EnableTriggerSettings(bool enable);

bool OpenConfigFileFromMostRecentList(CRecentFileList* m_pRecentFileList, int nID);
bool SetStatusText(LPCTSTR pStatusText);

void StopRegression();

class CMultiDocTemplate* getIPDocTemplate();


