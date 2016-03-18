//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLVFastOCR
//* .File Name       : $Workfile:   SVLVFastOCR.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:25:44  $
//******************************************************************************
#pragma once
#define _wAlign_h  // remove once wAlignUsr.h exists
#define _corObjObsolete_h

#pragma region Includes
#include "wFast.h"
#pragma region Includes

//#undef freeData
struct SVOCRParamStruct;
struct SVOCRGrayParamStruct;

class SVLVFastOCR  
{
public:
	SVLVFastOCR();
	virtual ~SVLVFastOCR();

	CorOpRtn GetLastLVReturnCode();

	BOOL IsLVFastOCRLoaded();

	CorObjType GetLVObjectType(LPCTSTR pszType);


	CorObj *CreateLVObject(LPCTSTR pszType);
	CorObj *CreateLVObject(CorObjType lvType);
	void *CreateLVObjectData(LPCTSTR pszType);
	void *CreateLVObjectData(CorObjType lvType);
	CorObj *CreateLVStringObject(LPCTSTR pszString);
	BOOL InitializeLVObject(CorObj *plvObject, LPCTSTR pszType);
	BOOL InitializeLVObject(CorObj *plvObject, CorObjType lvType);
	BOOL InitializeLVStringObject(CorObj *plvObject, LPCTSTR pszString);
	BOOL ReleaseLVObject(CorObj *plvObject);
	BOOL ReleaseLVObjectData(void *plvObject, LPCTSTR pszType, LPCTSTR pszRefType);
	BOOL ReleaseLVObjectData(void *plvObject, CorObjType cotType, CorObjType cotRefType);
	BOOL ReleaseLVVector(CorVector *plvVector);
	BOOL DestroyLVObject(CorObj **plvObject);
	BOOL DestroyLVObjectData(void **plvObject, LPCTSTR pszType, LPCTSTR pszRefType);
	BOOL DestroyLVObjectData(void **plvObject, CorObjType cotType, CorObjType cotRefType);

	BOOL ReadLVImageFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL ReadLVObjectFromFile(CorObj *plvObject, LPCTSTR pszFileName);
	BOOL WriteLVObjectToFile(CorObj *plvObject, LPCTSTR pszFileName);

	BOOL CopyLVObject(CorObj *plvFromObject, CorObj *plvToObject);
	BOOL CopyLVObjectData(void *pvSource, void *pvDest, CorObjType cotType, CorObjType cotRefType);

	BOOL ExtractLVImage(CorObj *plvImage, CorObj *plvROI, CorObj *plvOut);

	BOOL AddLVObjectToVector(CorObj *plvObject, CorVector *plvVector, int iItem = 0);
	BOOL GetLVObjectFromVector(CorObj *plvObject, CorVector *plvVector, int iItem);

	void *AddLVOpStatusToThread();

	BOOL InitializeLVOCRGray(CorOcrFont *pFont, CorFontModelGray **pFontModel);
	BOOL InitializeLVOCRPerim(CorOcrFont *pFont, CorFontModel **pFontModel, int iSamples);
	BOOL RunLVOCRGray(CorImage *pTarget, CorObj *pROI, CorFontModelGray *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRGrayParamStruct *psvOCRParameters);
	BOOL RunLVOCRPerim(CorImage *pTarget, CorObj *pROI, CorFontModel *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRParamStruct *psvOCRParameters);

protected:

private:
	CorOpRtn mlvReturnCode;

	void *GetFunctionAddress( LPCSTR szFunctionName );

	bool loadDlls(LPCTSTR pDllString);
	bool LoadLVDLLs();
	BOOL LoadLVFunctionAddresses();

	BOOL UnloadLVDLLs();
	BOOL UnloadLVFunctionAddresses();

	BOOL ConnectToLVFastOCR();
	BOOL DisconnectFromLVFastOCR();
};

