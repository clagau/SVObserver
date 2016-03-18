//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLVFastOCR
//* .File Name       : $Workfile:   SVLVFastOCR.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:38:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLVFastOCR.h"
#include "SVContainerLibrary/SVVector.h"
#include "SVGlobal.h"
#include "SVOCRParamStruct.h"
#include "SVOCRGrayParamStruct.h"
#include "SVOMFCLibrary/SVTemplate.h"

#include "SVOLicenseManager/SVOLicenseManager.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
// Global Object Controlling Variables and Functions Section
//

//
// Logical Vision FastOCR Variables
//

//void *CorGuiInit(void *hWnd, int (*opStatusCallback)(CorOpRtn code, char *msg));
typedef void *(__cdecl *LVCorGuiInit)(void *, int (__cdecl *callback)(CorOpRtn code, char *msg));

// Boolean CorGuiExit(void *handle);
typedef Boolean (__cdecl *LVCorGuiExit )(void *);

// int CorOcrInit(void);
typedef int (__cdecl *LVCorOcrInit)(void);

// Boolean CorObjParseStr(char *defStr);
typedef Boolean (__cdecl *LVCorObjParseStr)(char *);

// CorObjType CorObjGetId(char *name);
typedef CorObjType (__cdecl *LVCorObjGetId) (char *);

// CorObj *CorObjCreate(CorObj *obj, CorObjType objType);
typedef CorObj *(__cdecl *LVCorObjCreate) (CorObj *, CorObjType);

//void *CorObjDataCreate(CorObjType type);
typedef void *(__cdecl *LVCorObjDataCreate) (CorObjType);

// Boolean CorStringAlloc(CorString *corStr, const int size, const char *value);
typedef Boolean (__cdecl *LVCorStringAlloc) (CorString *, const int, const char *);

// Boolean CorObjRelease(CorObj *obj);
typedef Boolean (__cdecl *LVCorObjRelease) (CorObj *);

// Boolean CorObjRelease1(CorObj *obj, Boolean freePtrAlso);
typedef Boolean (__cdecl *LVCorObjRelease1) (CorObj *, Boolean);

// Boolean CorObjDataRelease(void *datap, CorObjType type, CorObjType refType);
typedef Boolean (__cdecl *LVCorObjDataRelease) (void *, CorObjType, CorObjType);

// Boolean CorObjDataRelease1(void *datap, CorObjType type, CorObjType refType, Boolean freePtrAlso);
typedef Boolean (__cdecl *LVCorObjDataRelease1) (void *, CorObjType, CorObjType, Boolean);

// int CorVectorFree(CorVector *vec);
typedef int (__cdecl *LVCorVectorFree)(CorVector *);

// CorOpRtn cor_rdObj(CorObj *Out, CorFile filename);
typedef CorOpRtn (__cdecl *LVcor_rdObj) (CorObj *, CorFile);

// CorOpRtn cor_wrObj(CorObj *In, CorFile filename, int format);
typedef CorOpRtn (__cdecl *LVcor_wrObj) (CorObj *, CorFile, int);

// CorOpRtn cor_rdImage(CorImage *Out, CorFile filename, int type);
typedef CorOpRtn (__cdecl *LVcor_rdImage) (CorImage *, CorFile, int);

// CorOpRtn cor_extract(CorImage *In, CorObj *Rect, CorObj *Out);
typedef CorOpRtn (__cdecl *LVcor_extract) (CorImage *, CorObj *, CorObj *);

// Boolean CorObjDataCopy(void *src, void *dst, CorObjType type, CorObjType refType);
typedef Boolean (__cdecl *LVCorObjDataCopy) (void *, void *, CorObjType, CorObjType);

// CorOpRtn cor_addElem(CorObj *Vector, CorObj *NewElement, CorObj *NewVector, int method, int element);
typedef CorOpRtn (__cdecl *LVcor_addElem) (CorObj *, CorObj *, CorObj *, int, int);

// CorOpRtn cor_getElem(CorObj *Vector, CorObj *newElement, CorObj *newVector, int method, int element);
typedef CorOpRtn (__cdecl *LVcor_getElem) (CorObj *, CorObj *, CorObj *, int, int);

// Boolean CorObjCopy(CorObj *src, CorObj *dst);
typedef Boolean (__cdecl *LVCorObjCopy) (CorObj *, CorObj *);

// CorOpRtn cor_ocrPerimInit(CorOcrFont *Font, CorFontModel **FontModel, int samples);
typedef CorOpRtn (__cdecl *LVcor_ocrPerimInit ) ( CorOcrFont *, CorFontModel ** , int );

// CorOpRtn cor_ocrPerimRun(CorImage *Target, CorObj *ROI, CorFontModel *FontModel, CorVector *Labels, CorVector *Scores, CorVector *Locations, int rotation, float sensitivity, float minAngle, float maxAngle, float featuresUsed, int scaleInvariant, float minScale, float maxScale, float threshold, int ignoreNoMatch, CorString noMatchLabel, int selection, int minWidth, int maxWidth, int minHeight, int maxHeight, int minPixels, int maxPixels);
typedef CorOpRtn (__cdecl *LVcor_ocrPerimRun ) ( CorImage *, CorObj *, CorFontModel *, CorVector *, CorVector *, CorVector *, int, float, float, float, float, int, float, float, float, int, CorString, int, int, int, int, int, int, int );

// CorOpRtn cor_ocrGrayInit(OcrFont *Font, FontModel2 **FontModel);
typedef CorOpRtn (__cdecl *LVcor_ocrGrayInit ) ( CorOcrFont *, CorFontModelGray ** );

// CorOpRtn cor_ocrGrayRun(CorImage *Target, CorObj *ROI, CorFontModelGray *FontModel, CorVector *Labels, CorVector *Scores, CorVector *Locations, int maxMatches, int xVicinity, int yVicinity, float rejectThresh, float acceptThresh, float relThresh, float minContrast, int scale, int maxCandidates, int candsInVicinity, float candThreshold, float candRelThresh, int output);
typedef CorOpRtn (__cdecl *LVcor_ocrGrayRun ) ( CorImage *, CorObj *, CorFontModelGray *, CorVector *, CorVector *, CorVector *, int, int, int, float, float, float, float, int, int, int, float, float, int );

// void *CorOpStatusAddThread(void);
typedef void *(__cdecl *LVCorOpStatusAddThread ) ( void );


void *gpvLVCorGuiHandle = NULL;

LVCorGuiInit gpfnCorGuiInit = NULL;
LVCorGuiExit gpfnCorGuiExit = NULL;
LVCorOcrInit gpfnCorOcrInit = NULL;
LVCorObjParseStr gpfnCorObjParseStr = NULL;
LVCorObjGetId gpfnCorObjGetId = NULL;
LVCorObjCreate gpfnCorObjCreate = NULL;
LVCorObjDataCreate gpfnCorObjDataCreate = NULL;
LVCorStringAlloc gpfnCorStringAlloc = NULL;
LVCorObjRelease gpfnCorObjRelease = NULL;
LVCorObjRelease1 gpfnCorObjRelease1 = NULL;
LVCorObjDataRelease gpfnCorObjDataRelease = NULL;
LVCorObjDataRelease1 gpfnCorObjDataRelease1 = NULL;
LVCorVectorFree gpfnCorVectorFree = NULL;
LVcor_rdObj gpfncor_rdObj = NULL;
LVcor_wrObj gpfncor_wrObj = NULL;
LVcor_rdImage gpfncor_rdImage = NULL;
LVcor_extract gpfncor_extract = NULL;
LVCorObjDataCopy gpfnCorObjDataCopy = NULL;
LVcor_addElem gpfncor_addElem = NULL;
LVcor_getElem gpfncor_getElem = NULL;
LVCorObjCopy gpfnCorObjCopy = NULL;
LVcor_ocrPerimInit gpfncor_ocrPerimInit = NULL;
LVcor_ocrPerimRun gpfncor_ocrPerimRun = NULL;
LVcor_ocrGrayInit gpfncor_ocrGrayInit = NULL;
LVcor_ocrGrayRun gpfncor_ocrGrayRun = NULL;
LVCorOpStatusAddThread gpfnCorOpStatusAddThread = NULL;

//
// WiT status message call back.
// All messages (errors, warnings, etc.) from any Cor... or cor_ functions
// will be directed here.
//
CorOpRtn glvcorLVLastStatusCode = COR_OP_OK;
char *gszLVLastStatusMessage = NULL;

int __cdecl LVopStatusCallback(CorOpRtn lvcorStatusCode, char *szMessage)
{
	BOOL bOk = FALSE;

	char szBuffer[1024];

	glvcorLVLastStatusCode = lvcorStatusCode;
	gszLVLastStatusMessage = szMessage;

	switch (lvcorStatusCode)
	{
		case COR_OP_MSG:
		{
			bOk = TRUE;
			break;
		}
		case COR_OP_WARNING:
		{
			sprintf ( szBuffer, "FastOCR Warning: %s", szMessage );
			break;
 		}
		case COR_OP_ERROR:
		default:
		{
			sprintf ( szBuffer, "FastOCR Error: %s", szMessage );
			break;
		}
	}

	if ( !bOk )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, szBuffer, StdMessageParams, SvOi::Err_10061 );
	}

	return bOk;
}

//
// SVResearch Variables
//

int giSVObjectCount = 0;

CCriticalSection gccsSVLVFastOCRLock;

SVVector< HINSTANCE > gsvacDLLHandleArray;

//
// SVResearch Old WiT Definitions
//
// Object definitions for FastOCR package. This is a patch to maintain
// compatibility to old font files.  This string definition is passed
// to CorObjParseStr() function that adds the obsolete definition to
// the existing FastOCR types.
//
static char gszSVLVObjects[] = "object SVWitFontParams\n"
                               "{\n"
                               "  CorImage    imVec[];\n" 
                               "  CorString   strVec[];\n"
                               "}\n";

typedef struct 
{
  CorImageVector imVec;
	CorStringVector strVec;
} SVWitFontParams;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W;
} SVWitFontParamsVector;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W, H;
  struct
	{
    int x, y, w, h;
  } roi;
} SVWitFontParamsImage;

typedef struct
{
  CorObjType type;
  SVWitFontParams *data;
  int W, H, D;
} SVWitFontParamsVolume;

#define CorObj_SVWitFontParams(o) ((SVWitFontParams *) ((o)->val.data))
#define CorObj_SVWitFontParamsImVec(sv) ((CorImageVector *) &((sv)->imVec))
#define CorObj_SVWitFontParamsStrVec(sv) ((CorStringVector *) &((sv)->strVec))

//
// End of Global Section
//

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVLVFastOCR::SVLVFastOCR()
{
	mlvReturnCode = COR_OP_ERROR;

	if ( TheSVOLicenseManager().HasFastOCRLicense() )
	{
		if ( gccsSVLVFastOCRLock.Lock() )
		{
			giSVObjectCount++;

			if ( !IsLVFastOCRLoaded() )
			{
				if ( ConnectToLVFastOCR() )
				{
					mlvReturnCode = COR_OP_OK;
				}
			}
			else
			{
				mlvReturnCode = COR_OP_OK;
			}

			gccsSVLVFastOCRLock.Unlock();
		}
	}
}

SVLVFastOCR::~SVLVFastOCR()
{
	if ( TheSVOLicenseManager().HasFastOCRLicense() )
	{
		if ( gccsSVLVFastOCRLock.Lock() )
		{
			if ( 0 < giSVObjectCount )
			{
				giSVObjectCount--;
			}

			if ( giSVObjectCount <= 0 )
			{
				DisconnectFromLVFastOCR();
				giSVObjectCount = 0;
			}

			gccsSVLVFastOCRLock.Unlock();
		}
	}
}

CorOpRtn SVLVFastOCR::GetLastLVReturnCode()
{
	return mlvReturnCode;
}

BOOL SVLVFastOCR::IsLVFastOCRLoaded()
{
	BOOL bOk = 0 <= GetLVObjectType("SVWitFontParams");

	return bOk;
}

CorObjType SVLVFastOCR::GetLVObjectType(LPCTSTR pszType)
{
	CorObjType lvType = -1;

	if ( gpfnCorObjGetId )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		lvType = gpfnCorObjGetId((char *)pszType);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return lvType;
}

CorObj *SVLVFastOCR::CreateLVObject(LPCTSTR pszType)
{
	CorObj *plvObject = NULL;

	CorObjType lvType = GetLVObjectType(pszType);

	if ( 0 <= lvType )
	{
		plvObject = CreateLVObject( lvType );
	}

	return plvObject;
}

CorObj *SVLVFastOCR::CreateLVObject(CorObjType lvType)
{
	CorObj *plvObject = NULL;

	if ( IsLVFastOCRLoaded() && 0 <= lvType )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		plvObject = gpfnCorObjCreate(NULL,lvType);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return plvObject;
}

void *SVLVFastOCR::CreateLVObjectData(LPCTSTR pszType)
{
	void *pvObject = NULL;

	CorObjType lvType = GetLVObjectType(pszType);

	if ( 0 <= lvType )
	{
		pvObject = CreateLVObjectData( lvType );
	}

	return pvObject;
}

void *SVLVFastOCR::CreateLVObjectData(CorObjType lvType)
{
	void *pvObject = NULL;

	if ( IsLVFastOCRLoaded() && 0 <= lvType )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		pvObject = gpfnCorObjDataCreate(lvType);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return pvObject;
}

CorObj *SVLVFastOCR::CreateLVStringObject(LPCTSTR pszString)
{
	CorObj *plvObject = CreateLVObject(COR_OBJ_STRING);

	if (plvObject)
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		BOOL bOk = gpfnCorStringAlloc( &(CorObj_string(plvObject)), 0, (char *)pszString );

		mlvReturnCode = glvcorLVLastStatusCode;

		if ( !bOk )
		{
			//CorObj_type(plvObject) = COR_OBJ_GENERIC;  // JMS May note be needed
			DestroyLVObject(&plvObject);
		}
	}

	return plvObject;
}

BOOL SVLVFastOCR::InitializeLVObject(CorObj *plvObject, LPCTSTR pszType)
{
	BOOL bOk = FALSE;

	CorObjType lvType = GetLVObjectType(pszType);

	if ( plvObject && 0 <= lvType )
	{
		bOk = InitializeLVObject(plvObject,lvType);
	}

	return bOk;
}

BOOL SVLVFastOCR::InitializeLVObject(CorObj *plvObject, CorObjType lvType)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && plvObject )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		plvObject = gpfnCorObjCreate(plvObject,lvType);

		mlvReturnCode = glvcorLVLastStatusCode;
		bOk = plvObject != NULL;
	}

	return bOk;
}

BOOL SVLVFastOCR::InitializeLVStringObject(CorObj *plvObject, LPCTSTR pszString)
{
	BOOL bOk = FALSE;

	if ( plvObject )
	{
		if (InitializeLVObject(plvObject, COR_OBJ_STRING))
		{
			bOk = gpfnCorStringAlloc(&(CorObj_string(plvObject)), 0, (char *)pszString);
			if (!bOk)
			{
				ReleaseLVObject(plvObject);
			}
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::ReleaseLVObject(CorObj *plvObject)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && plvObject )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorObjRelease(plvObject);

		mlvReturnCode = glvcorLVLastStatusCode;

		memset(plvObject, 0, sizeof(CorObj));
	}

	return bOk;
}

BOOL SVLVFastOCR::ReleaseLVObjectData(void *plvObject, LPCTSTR pszType, LPCTSTR pszRefType)
{
	BOOL bOk = FALSE;

	CorObjType cotType = GetLVObjectType(pszType);
	CorObjType cotRefType = 0;

	if ( pszRefType )
	{
		cotRefType = GetLVObjectType(pszRefType);
	}

	if ( plvObject && 0 <= cotType && 0 <= cotRefType )
	{
		bOk = ReleaseLVObjectData(plvObject, cotType, cotRefType);
	}

	return bOk;
}

BOOL SVLVFastOCR::ReleaseLVObjectData(void *plvObject, CorObjType cotType, CorObjType cotRefType)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && plvObject && 0 <= cotType && 0 <= cotRefType )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorObjDataRelease(plvObject, cotType, cotRefType);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return bOk;
}

BOOL SVLVFastOCR::ReleaseLVVector(CorVector *plvVector)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && plvVector )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorVectorFree(plvVector);

		mlvReturnCode = glvcorLVLastStatusCode;

		memset(plvVector, 0, sizeof(CorVector));
	}

	return bOk;
}

BOOL SVLVFastOCR::DestroyLVObject(CorObj **plvObject)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() )
	{
		if (*plvObject)
		{
			glvcorLVLastStatusCode = COR_OP_OK;
			gszLVLastStatusMessage = NULL;

			bOk = gpfnCorObjRelease1(*plvObject, TRUE);

			mlvReturnCode = glvcorLVLastStatusCode;

			*plvObject = NULL;
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::DestroyLVObjectData(void **plvObject, LPCTSTR pszType, LPCTSTR pszRefType)
{
	BOOL bOk = FALSE;

	CorObjType cotType = GetLVObjectType(pszType);
	CorObjType cotRefType = 0;

	if ( pszRefType )
	{
		cotRefType = GetLVObjectType(pszRefType);
	}

	if ( 0 <= cotType && 0 <= cotRefType )
	{
		bOk = DestroyLVObjectData(plvObject, cotType, cotRefType);
	}

	return bOk;
}

BOOL SVLVFastOCR::DestroyLVObjectData(void **plvObject, CorObjType cotType, CorObjType cotRefType)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && 0 <= cotType && 0 <= cotRefType )
	{
		if (*plvObject)
		{
			glvcorLVLastStatusCode = COR_OP_OK;
			gszLVLastStatusMessage = NULL;

			try
			{
				bOk = gpfnCorObjDataRelease1(*plvObject, cotType, cotRefType, TRUE);
			}
			catch( ... )
			{
				bOk = FALSE;
			}

			mlvReturnCode = glvcorLVLastStatusCode;

			*plvObject = NULL;
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::ReadLVObjectFromFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if ( 0 <= GetLVObjectType("SVWitFontParams") &&
		   0 <= GetLVObjectType("CorOcrFont") )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_rdObj(plvObject, (char *)pszFileName);
		bOk = mlvReturnCode == COR_OP_OK;

		if (bOk)
		{
			if (CorObj_type(plvObject) == GetLVObjectType("SVWitFontParams"))
			{
				// This is the fastest, but dangerous method for converting old style font to
				// new style. It is dangerous because it relies on the internal structure alignment
				// of the two types being identical. This is the case, but since CorOcrFont is a
				// type defined by WiT, its internal structure is not guaranteed to remain the same.
				//
				CorObj_type(plvObject) = GetLVObjectType("CorOcrFont");  // JMS may need to change later

				//
				// JMS add WriteLVObject here if it is determined that conversion is neccessary
				// at finding an old font.
			}
			else 
			{
				bOk = CorObj_type(plvObject) == GetLVObjectType("CorOcrFont");
			}
		}
	}

	return bOk;
}

BOOL SVLVFastOCR::WriteLVObjectToFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_wrObj(plvObject, (char *)pszFileName, 0);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::ReadLVImageFromFile(CorObj *plvObject, LPCTSTR pszFileName)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_rdImage(CorObj_image(plvObject), (char *)pszFileName, 0);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::CopyLVObject(CorObj *plvFromObject, CorObj *plvToObject)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && plvFromObject && plvToObject )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorObjCopy(plvFromObject, plvToObject);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return bOk;
}

BOOL SVLVFastOCR::CopyLVObjectData(void *pvSource, void *pvDest, CorObjType cotType, CorObjType cotRefType)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && pvSource && pvDest && 0 <= cotType && 0 <= cotRefType)
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorObjDataCopy(pvSource, pvDest, cotType, cotRefType);

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return bOk;
}

BOOL SVLVFastOCR::ExtractLVImage(CorObj *plvImage, CorObj *plvROI, CorObj *plvOut)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_extract(CorObj_image(plvImage), plvROI, plvOut);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::AddLVObjectToVector(CorObj *plvObject, CorVector *plvVector, int iItem)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() )
	{
		CorObj *plvObjInVec = CreateLVObject(COR_OBJ_VECTOR);
		CorObj *plvObjOutVec = CreateLVObject(COR_OBJ_VECTOR);

		if (plvObjInVec && plvObjOutVec)
		{
			bOk = CopyLVObjectData(plvVector,
			                       CorObj_vector(plvObjInVec),
													   COR_OBJ_VECTOR,
													   CorObj_mdType(plvVector));

			if (bOk)
			{
				CorObjType cotTypeObj = CorObj_mdType(CorObj_vector(plvObjInVec));
				CorObjType cotTypeVec = CorObj_mdType(plvVector);

				glvcorLVLastStatusCode = COR_OP_OK;
				gszLVLastStatusMessage = NULL;

				mlvReturnCode = gpfncor_addElem(plvObjInVec, plvObject, plvObjOutVec, 1, iItem);
				bOk = mlvReturnCode == COR_OP_OK;
			}

			if (bOk)
			{
				bOk = CopyLVObjectData(CorObj_vector(plvObjOutVec),
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

	if ( IsLVFastOCRLoaded() )
	{
		CorObj *plvObjInVec = CreateLVObject(COR_OBJ_VECTOR);
		CorObj *plvObjOutVec = CreateLVObject(COR_OBJ_VECTOR);

		if (plvObjInVec && plvObjOutVec)
		{
			bOk = CopyLVObjectData(plvVector,
			                       CorObj_vector(plvObjInVec),
													   COR_OBJ_VECTOR,
													   CorObj_mdType(plvVector));

			if (bOk)
			{
				glvcorLVLastStatusCode = COR_OP_OK;
				gszLVLastStatusMessage = NULL;

				mlvReturnCode = gpfncor_getElem(plvObjInVec, plvObject, plvObjOutVec, 2, iItem);
				bOk = mlvReturnCode == COR_OP_OK;
			}

			if (bOk)
			{
				bOk = CopyLVObjectData(CorObj_vector(plvObjOutVec),
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

void *SVLVFastOCR::AddLVOpStatusToThread()
{
	void *pvHandle = NULL;

	if ( IsLVFastOCRLoaded() )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		pvHandle = gpfnCorOpStatusAddThread();

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	return pvHandle;
}


BOOL SVLVFastOCR::InitializeLVOCRGray(CorOcrFont *pFont, CorFontModelGray **pFontModel)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && pFont && pFontModel )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_ocrGrayInit(pFont, pFontModel);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::InitializeLVOCRPerim(CorOcrFont *pFont, CorFontModel **pFontModel, int iSamples)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && pFont && pFontModel )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_ocrPerimInit(pFont, pFontModel, iSamples);
		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::RunLVOCRGray(CorImage *pTarget, CorObj *pROI, CorFontModelGray *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRGrayParamStruct *psvOCRParameters)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && 
	     pTarget && pROI && pFontModel && 
       pLabels && pScores && pLocations &&
			 psvOCRParameters )
	{
		DWORD threadId = GetCurrentThreadId();
		TRACE1("Thread %d -- cor_ocrGrayRun\n",threadId);

		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_ocrGrayRun( pTarget, pROI, pFontModel, 
		                                    *pLabels, *pScores, *pLocations, 
																			  psvOCRParameters->maxMatches,
																			  psvOCRParameters->xVicinity,
																			  psvOCRParameters->yVicinity,
																			  psvOCRParameters->rejectThresh,
																			  psvOCRParameters->acceptThresh,
																				psvOCRParameters->relThresh,
																				psvOCRParameters->minContrast,
																				psvOCRParameters->scale,
																				psvOCRParameters->maxCandidates,
																				psvOCRParameters->candsInVicinity,
																				psvOCRParameters->candidateThresh,
																				psvOCRParameters->candidateRelThresh,
																				psvOCRParameters->output );

		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

BOOL SVLVFastOCR::RunLVOCRPerim(CorImage *pTarget, CorObj *pROI, CorFontModel *pFontModel, CorVector **pLabels, CorVector **pScores, CorVector **pLocations, SVOCRParamStruct *psvOCRParameters)
{
	BOOL bOk = FALSE;

	if ( IsLVFastOCRLoaded() && 
	     pTarget && pROI && pFontModel && 
       pLabels && pScores && pLocations &&
			 psvOCRParameters )
	{
		DWORD threadId = GetCurrentThreadId();
		TRACE1("Thread %d -- cor_ocrPerimRun\n",threadId);

		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		mlvReturnCode = gpfncor_ocrPerimRun( pTarget, pROI, pFontModel, 
		                                     *pLabels, *pScores, *pLocations, 
																				 psvOCRParameters->rotation,
																				 psvOCRParameters->forientationSensitivity,
																				 psvOCRParameters->fminAngle,
																				 psvOCRParameters->fmaxAngle,
																				 psvOCRParameters->fFeaturesUsed,
																				 psvOCRParameters->scaleInvariant,
																				 psvOCRParameters->fminScale,
																				 psvOCRParameters->fmaxScale,
																				 psvOCRParameters->fthreshold,
																				 psvOCRParameters->ignoreNoMatch,
																				 (char *)(LPCTSTR)psvOCRParameters->STRnoMatchLabel,
																				 psvOCRParameters->selection,
																				 psvOCRParameters->minWidth,
																				 psvOCRParameters->maxWidth,
																				 psvOCRParameters->minHeight,
																				 psvOCRParameters->maxHeight,
																				 psvOCRParameters->nMinPixelsInBlob,
																				 psvOCRParameters->nMaxPixelsInBlob );

		bOk = mlvReturnCode == COR_OP_OK;
	}

	return bOk;
}

//
// Private Methods
//

void *SVLVFastOCR::GetFunctionAddress(LPCSTR szFunctionName)
{
	void *pvFunctionAddress = NULL;
	
	for ( int i = 0; pvFunctionAddress == NULL && i < gsvacDLLHandleArray.GetSize(); i++ )
	{
		pvFunctionAddress = ::GetProcAddress( gsvacDLLHandleArray[i], szFunctionName );
	}

	if ( pvFunctionAddress == NULL )
	{
		SVString csMessage = SvUl_SF::Format(SvO::FastOCR_functionAddressNotFound, szFunctionName);
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, csMessage.c_str(), StdMessageParams, SvOi::Err_10062 );
	}

	return pvFunctionAddress;
}

bool SVLVFastOCR::loadDlls(LPCTSTR pDllString)
{
	HINSTANCE hDLL = SVLoadLibrary( pDllString );
	bool bOk = (nullptr != hDLL);
	// This sleep(0) was added after the LoadLibrary to fix a bug where the system ran out of resources.
	Sleep(0);
	if ( bOk )
	{
		gsvacDLLHandleArray.Add( hDLL );
	}
	else
	{
		SVString csMessage = SvUl_SF::Format(SvO::FastOCR_LoadDllFailed, pDllString);
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, csMessage.c_str(), StdMessageParams, SvOi::Err_10063 );
	}
	return bOk;
}

bool SVLVFastOCR::LoadLVDLLs()
{
	bool bOk = loadDlls(_T( "wobj.dll" ));
	if ( bOk )
	{
		bOk = loadDlls(_T( "wobjman.dll" ));
	}

	if ( bOk )
	{
		bOk = loadDlls(_T( "wsystem.dll" ));
	}

	if ( bOk )
	{
		bOk = loadDlls(_T( "wocr.dll" ));
	}

	if ( !bOk )
	{
		UnloadLVDLLs();
	}

	return bOk;
}

BOOL SVLVFastOCR::LoadLVFunctionAddresses()
{
	BOOL bOk = ( gpfnCorGuiInit = (LVCorGuiInit) GetFunctionAddress( _T( "CorGuiInit" ) ) ) != NULL;

	if ( bOk )
	{
		bOk = ( gpfnCorGuiExit = (LVCorGuiExit) GetFunctionAddress( _T( "CorGuiExit" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorOcrInit = (LVCorOcrInit) GetFunctionAddress( _T( "CorOcrInit" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjParseStr = (LVCorObjParseStr) GetFunctionAddress( _T( "CorObjParseStr" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjGetId = (LVCorObjGetId) GetFunctionAddress( _T( "CorObjGetId" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjCreate = (LVCorObjCreate) GetFunctionAddress( _T( "CorObjCreate" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjDataCreate = (LVCorObjDataCreate) GetFunctionAddress( _T( "CorObjDataCreate" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorStringAlloc = (LVCorStringAlloc) GetFunctionAddress( _T( "CorStringAlloc" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjRelease = (LVCorObjRelease) GetFunctionAddress( _T( "CorObjRelease" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjRelease1 = (LVCorObjRelease1) GetFunctionAddress( _T( "CorObjRelease1" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjDataRelease = (LVCorObjDataRelease) GetFunctionAddress( _T( "CorObjDataRelease" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjDataRelease1 = (LVCorObjDataRelease1) GetFunctionAddress( _T( "CorObjDataRelease1" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorVectorFree = (LVCorVectorFree) GetFunctionAddress( _T( "CorVectorFree" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_rdObj = (LVcor_rdObj) GetFunctionAddress( _T( "cor_rdObj" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_wrObj = (LVcor_wrObj) GetFunctionAddress( _T( "cor_wrObj" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_rdImage = (LVcor_rdImage) GetFunctionAddress( _T( "cor_rdImage" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_extract = (LVcor_extract) GetFunctionAddress( _T( "cor_extract" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjDataCopy = (LVCorObjDataCopy) GetFunctionAddress( _T( "CorObjDataCopy" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_addElem = (LVcor_addElem) GetFunctionAddress( _T( "cor_addElem" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_getElem = (LVcor_getElem) GetFunctionAddress( _T( "cor_getElem" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorObjCopy = (LVCorObjCopy) GetFunctionAddress( _T( "CorObjCopy" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_ocrPerimInit = (LVcor_ocrPerimInit) GetFunctionAddress( _T( "cor_ocrPerimInit" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_ocrPerimRun = (LVcor_ocrPerimRun) GetFunctionAddress( _T( "cor_ocrPerimRun" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_ocrGrayInit = (LVcor_ocrGrayInit) GetFunctionAddress( _T( "cor_ocrGrayInit" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfncor_ocrGrayRun = (LVcor_ocrGrayRun) GetFunctionAddress( _T( "cor_ocrGrayRun" ) ) ) != NULL;
	}

	if ( bOk )
	{
		bOk = ( gpfnCorOpStatusAddThread = (LVCorOpStatusAddThread) GetFunctionAddress( _T( "CorOpStatusAddThread" ) ) ) != NULL;
	}

	if ( !bOk )
	{
		UnloadLVFunctionAddresses();
	}

	return bOk;
}

BOOL SVLVFastOCR::UnloadLVDLLs()
{
	BOOL bOk = TRUE;

	for ( int i = gsvacDLLHandleArray.GetSize() - 1; 0 <= i; i-- )
	{
		SVFreeLibrary( gsvacDLLHandleArray[i] );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
		gsvacDLLHandleArray[i] = NULL;
	}

	gsvacDLLHandleArray.RemoveAll();

	return bOk;
}

BOOL SVLVFastOCR::UnloadLVFunctionAddresses()
{
	BOOL bOk = TRUE;

	gpfnCorGuiInit = NULL;
	gpfnCorGuiExit = NULL;
	gpfnCorOcrInit = NULL;
	gpfnCorObjParseStr = NULL;
	gpfnCorObjGetId = NULL;
	gpfnCorObjCreate = NULL;
	gpfnCorObjDataCreate = NULL;
	gpfnCorStringAlloc = NULL;
	gpfnCorObjRelease = NULL;
	gpfnCorObjRelease1 = NULL;
	gpfnCorObjDataRelease = NULL;
	gpfnCorObjDataRelease1 = NULL;
	gpfnCorVectorFree = NULL;
 	gpfncor_rdObj = NULL;
	gpfncor_wrObj = NULL;
	gpfncor_rdImage = NULL;
	gpfncor_extract = NULL;
	gpfnCorObjDataCopy = NULL;
	gpfncor_addElem = NULL;
	gpfncor_getElem = NULL;
	gpfnCorObjCopy = NULL;
	gpfncor_ocrPerimInit = NULL;
	gpfncor_ocrPerimRun = NULL;
	gpfncor_ocrGrayInit = NULL;
	gpfncor_ocrGrayRun = NULL;
	gpfnCorOpStatusAddThread = NULL;

	return bOk;
}

BOOL SVLVFastOCR::ConnectToLVFastOCR()
{
	BOOL bOk = LoadLVDLLs();

	if ( bOk )
	{
		bOk = LoadLVFunctionAddresses();
	}

	if ( bOk )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = ( gpvLVCorGuiHandle = gpfnCorGuiInit( AfxGetApp()->GetMainWnd(), LVopStatusCallback ) ) != NULL;

		mlvReturnCode = glvcorLVLastStatusCode;

		if ( bOk )
		{
			glvcorLVLastStatusCode = COR_OP_OK;
			gszLVLastStatusMessage = NULL;

			bOk = gpfnCorOcrInit();

			mlvReturnCode = glvcorLVLastStatusCode;

			if ( bOk )
			{
				glvcorLVLastStatusCode = COR_OP_OK;
				gszLVLastStatusMessage = NULL;

				bOk = gpfnCorObjParseStr( gszSVLVObjects );

				mlvReturnCode = glvcorLVLastStatusCode;
			}
		}
	}

	if ( !bOk )
	{
		DisconnectFromLVFastOCR();
	}

	return bOk;
}

BOOL SVLVFastOCR::DisconnectFromLVFastOCR()
{
	BOOL bOk = FALSE;

	if ( gpvLVCorGuiHandle )
	{
		glvcorLVLastStatusCode = COR_OP_OK;
		gszLVLastStatusMessage = NULL;

		bOk = gpfnCorGuiExit( gpvLVCorGuiHandle );
		gpvLVCorGuiHandle = NULL;

		mlvReturnCode = glvcorLVLastStatusCode;
	}

	bOk = UnloadLVFunctionAddresses() && bOk;
	bOk = UnloadLVDLLs() && bOk;

	return bOk;
}

