//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLVFastOCR
//* .File Name       : $Workfile:   SVLVFastOCR.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:36:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
//abb
//
// Object definitions for FastOCR package. This is a temporary patch
// that can be eliminated with next update to wOCR.dll.
// This string definition, and the CorObjParseStr() function call that
// uses it should both be deleted.  All that is required is the
// currently commented out CorOcrInit(...) function call.//
//
static char OldSVOCRObjects[] =
"object SVWitFontParams\n"
"{\n"
"  CorImage    imVec[];\n"
"  CorString   strVec[];\n"
"}\n";

/////////////////////////////////////////////////////////////////////////////
//
//
// WiT status message call back.
// All messages (errors, warnings, etc.) from any Cor... or cor_ functions
// will be directed here.
//
//
// abb: this new format of message callback gives the message type in the
// first parameter, so it is unnecessary to scan the msg string to find that out.
CorOpRtn glvLastCorOpRtn;

int SVLVopStatusCallback(CorOpRtn code, char *msg)
{
	BOOL bOk = TRUE;

	glvLastCorOpRtn = code;

	switch (code)
	{
		case COR_OP_ERROR:
		{
			bOk = FALSE;

			AfxMessageBox ( msg );
			break;
		}
		case COR_OP_WARNING:
		{
			bOk = FALSE;

			AfxMessageBox ( msg );
			break;
 		}
		case COR_OP_MSG:
		{
			AfxMessageBox ( msg );
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVLVFastOCR::SVLVFastOCR()
{
	mhWnd = NULL;
	mplvCorGuiHandle = NULL;
	mlvReturnCode = COR_OP_OK;
}

SVLVFastOCR::~SVLVFastOCR()
{
	if (mplvCorGuiHandle)
	{
//		CorGuiExit(mplvCorGuiHandle);
	}
}

CorOpRtn SVLVFastOCR::GetLastLVReturnCode()
{
	return mlvReturnCode;
}

BOOL SVLVFastOCR::Init(void *hWnd, int (*opStatusCallback)(CorOpRtn code, char *msg))
{
	BOOL bOk = FALSE;

//  int n = CorObjGetId("CorGraphic");
	if (hWnd)
	{
		bOk = Destroy();

		if (bOk)
		{
			mhWnd = hWnd;
//			mplvCorGuiHandle = CorGuiInit(mhWnd, opStatusCallback);
			mplvCorGuiHandle = (void *) 1;
			bOk = mplvCorGuiHandle != NULL;
		}

/*
		if (bOk)
		{
			// 
			// Update the CorObject system to include the fastOCR objects
			bOk = CorOcrInit();
			//
			// Patch (for now, since CorOcrInit is not up to date...)
			// When the wOcr.dll is updated, this call (and the ocrObjects 
			// definition should be deleted, and the above CorOcrInit() function 
			// call should be uncommented.
			if (bOk)
			{
				bOk = CorObjParseStr(OldSVOCRObjects);
			}
		}
*/
	}

	return bOk;
}

BOOL SVLVFastOCR::Destroy()
{
	BOOL bOk = TRUE;

	if (mplvCorGuiHandle)
	{
//		bOk = CorGuiExit(mplvCorGuiHandle);

		mplvCorGuiHandle = NULL;
		mhWnd = NULL;
	}

	return bOk;
}

CorObj *SVLVFastOCR::CreateLVObject(LPCTSTR pszType)
{
	return CreateLVObject(CorObjGetId((char *)pszType));
}

CorObj *SVLVFastOCR::CreateLVObject(CorObjType lvType)
{
	CorObj *plvObject = NULL;

	if (mplvCorGuiHandle)
	{
		plvObject = CorObjCreate(NULL,lvType);
	}

	return plvObject;
}

CorObj *SVLVFastOCR::CreateLVStringObject(LPCTSTR pszString)
{
	CorObj *plvObject = NULL;

	plvObject = CreateLVObject(COR_OBJ_STRING);

	if (plvObject)
	{
		if (CorStringAlloc(&(CorObj_string(plvObject)),0,(char *)pszString))
		{
			//CorObj_type(plvObject) = COR_OBJ_GENERIC;  // JMS May note be needed
			DestroyLVObject(&plvObject);
		}
	}

	return plvObject;
}

BOOL SVLVFastOCR::InitializeLVObject(CorObj *plvObject, LPCTSTR pszType)
{
	return InitializeLVObject(plvObject,CorObjGetId((char *)pszType));
}

BOOL SVLVFastOCR::InitializeLVObject(CorObj *plvObject, CorObjType lvType)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		plvObject = CorObjCreate(plvObject,lvType);
		bOk = plvObject != NULL;
	}

	return bOk;
}

BOOL SVLVFastOCR::InitializeLVStringObject(CorObj *plvObject, LPCTSTR pszString)
{
	BOOL bOk = FALSE;

	if (InitializeLVObject(plvObject,COR_OBJ_STRING))
	{
		bOk = CorStringAlloc(&(CorObj_string(plvObject)),0,(char *)pszString);
		if (!bOk)
		{
			ReleaseLVObject(plvObject);
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::ReleaseLVObject(CorObj *plvObject)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		bOk = CorObjRelease(plvObject);
		memset(plvObject, 0, sizeof(CorObj));
	}

	return bOk;
}

BOOL SVLVFastOCR::DestroyLVObject(CorObj **plvObject)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		if (*plvObject)
		{
			bOk = CorObjRelease1(*plvObject,1);

			*plvObject = NULL;
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::ReadLVObjectFromFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		mlvReturnCode = cor_rdObj(plvObject, (char *)pszFileName);
		bOk = mlvReturnCode == COR_OP_OK;

		if (bOk)
		{
			if (CorObj_type(plvObject) == CorObjGetId("SVWitFontParams"))
			{
			  // This is the fastest, but dangerous method for converting old style font to
				// new style. It is dangerous because it relies on the internal structure alignment
				// of the two types being identical. This is the case, but since CorOcrFont is a
				// type defined by WiT, its internal structure is not guaranteed to remain the same.
				//
				CorObj_type(plvObject) = CorObjGetId("CorOcrFont");  // JMS may need to change later
/*
				CorImage *pImage, *pIm;
				CorString *pString, *pStr;

				CorOcrFont *svWFP = (CorOcrFont *)CorObj_OcrFont(plvObject);
				CorImageVector *svWFPIV = &svWFP->images;
				CorStringVector *svWFPSV = &svWFP->labels;
				if (CorObj_mdType(svWFPIV) != COR_OBJ_VECTOR) {
				}
				if (CorObj_mdType(svWFPSV) != COR_OBJ_VECTOR) {
				}
				if (CorObj_size(svWFPIV) != CorObj_size(svWFPSV)) {
				  ASSERT(0);
				}
				pImage = (CorImage *) CorObj_mdData(svWFPIV);
        pString = (CorString *) CorObj_mdData(svWFPSV);
				for (int i = 0; i < CorObj_size(svWFPSV); i ++) {
				  pIm = &pImage[i];
					pStr = &pString[i];
//				  pIm = pImage+i;
//					pStr = pString+i;
				}
*/
				//
				// JMS add WriteLVObject here if it is determined that conversion is neccessary
				// at finding an old font.
			}
			else if (CorObj_type(plvObject) != CorObjGetId("CorOcrFont"))
			{
			}

/*  Old Methods of extracting data

				SVWitFontParams *svWFP = CorObj_SVWitFontParams(&obj);
				CorImageVector *svWFPIV = CorObj_SVWitFontParamsImVec(svWFP);
				CorStringVector *svWFPSV = CorObj_SVWitFontParamsStrVec(svWFP);
				if (CorObj_mdType(svWFPIV) != COR_OBJ_VECTOR) {
				}
				if (CorObj_mdType(svWFPSV) != COR_OBJ_VECTOR) {
				}
				if (CorObj_size(svWFPIV) != CorObj_size(svWFPSV)) {
				  ASSERT(0);
				}
				pImage = (CorImage *) CorObj_mdData(svWFPIV);
        pString = (CorString *) CorObj_mdData(svWFPSV);
				for (int i = 0; i < CorObj_size(svWFPSV); i ++) {
				  pIm = &pImage[i];
					pStr = &pString[i];
//				  pIm = pImage+i;
//					pStr = pString+i;
				}
*/
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::WriteLVObjectToFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		mlvReturnCode = cor_wrObj(plvObject, (char *)pszFileName, 0);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}


BOOL SVLVFastOCR::ReadLVImageFromFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		mlvReturnCode = cor_rdImage(CorObj_image(plvObject), (char *)pszFileName, 0);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::ExtractLVImage(CorObj *plvImage, CorObj *plvROI, CorObj *plvOut)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		mlvReturnCode = cor_extract(CorObj_image(plvImage), plvROI, plvOut);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::AddLVObjectToVector(CorObj *plvObject, CorVector *plvVector, int iItem)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		CorObj *plvObjInVec = CreateLVObject(COR_OBJ_VECTOR);
		CorObj *plvObjOutVec = CreateLVObject(COR_OBJ_VECTOR);

//		CorObj *plvObjInVec = CreateLVObject(CorObj_mdType(plvVector));
//		CorObj *plvObjOutVec = CreateLVObject(CorObj_mdType(plvVector));

		if (plvObjInVec && plvObjOutVec)
		{
			bOk = CorObjDataCopy(plvVector,
			                     CorObj_vector(plvObjInVec),
													 COR_OBJ_VECTOR,
													 CorObj_mdType(plvVector));

			if (bOk)
			{
				CorObjType cotTypeObj = CorObj_mdType(CorObj_vector(plvObjInVec));
				CorObjType cotTypeVec = CorObj_mdType(plvVector);

				mlvReturnCode = cor_addElem(plvObjInVec, plvObject, plvObjOutVec, 1, iItem);
				bOk = mlvReturnCode == COR_OP_OK;
			}

			if (bOk)
			{
				bOk = CorObjDataCopy(CorObj_vector(plvObjOutVec),
														 plvVector,
														 COR_OBJ_VECTOR,
														 CorObj_mdType(CorObj_vector(plvObjOutVec)));
			}
		}

		if (plvObjInVec)
		{
			bOk = DestroyLVObject(&plvObjInVec) && bOk;
		}

		if (plvObjOutVec)
		{
			bOk = DestroyLVObject(&plvObjOutVec) && bOk;
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::GetLVObjectFromVector(CorObj *plvObject, CorVector *plvVector, int iItem)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		CorObj *plvObjInVec = CreateLVObject(COR_OBJ_VECTOR);
		CorObj *plvObjOutVec = CreateLVObject(COR_OBJ_VECTOR);

		if (plvObjInVec && plvObjOutVec)
		{
			bOk = CorObjDataCopy(plvVector,
			                     CorObj_vector(plvObjInVec),
													 COR_OBJ_VECTOR,
													 CorObj_mdType(plvVector));

			if (bOk)
			{
				mlvReturnCode = cor_getElem(plvObjInVec, plvObject, plvObjOutVec, 2, iItem);
				bOk = mlvReturnCode == COR_OP_OK;
			}

			if (bOk)
			{
				bOk = CorObjDataCopy(CorObj_vector(plvObjOutVec),
														 plvVector,
														 COR_OBJ_VECTOR,
														 CorObj_mdType(CorObj_vector(plvObjOutVec)));
			}
		}

		if (plvObjInVec)
		{
			bOk = DestroyLVObject(&plvObjInVec) && bOk;
		}

		if (plvObjOutVec)
		{
			bOk = DestroyLVObject(&plvObjOutVec) && bOk;
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::CopyLVObject(CorObj *plvFromObject, CorObj *plvToObject)
{
	BOOL bOk = FALSE;

	if (mplvCorGuiHandle)
	{
		bOk = CorObjCopy(plvFromObject, plvToObject);
	}

	return bOk;
}

