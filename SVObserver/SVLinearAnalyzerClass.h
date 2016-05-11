// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearAnalyzerClass
// * .File Name       : $Workfile:   SVLinearAnalyzerClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   10 Jul 2014 17:44:52  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVAnalyzer.h"
#include "SVValueBaseNormalizerClass.h"
#pragma endregion Includes

class SVLinearEdgeProcessingClass;

class SVLinearAnalyzerClass : public SVAnalyzerClass  
{
	SV_DECLARE_CLASS( SVLinearAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVLinearAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARANALYZER );
	virtual ~SVLinearAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );

	SVLinearEdgeProcessingClass *GetEdgeA();
	SVLinearEdgeProcessingClass *GetEdgeB();

	SVImageClass *GetInputImage();

	HRESULT GetPixelDepth();

	SVBoolValueObjectClass m_svShowAllEdgeAOverlays;
	SVBoolValueObjectClass m_svShowAllEdgeBOverlays;

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	HRESULT GetProjectedExtent( long p_lIndex, long &p_rlBottom, SVImageExtentClass &p_rsvImageExtent );

	SVEnumerateValueObjectClass *GetInputProfileOrientation();
	SVBoolValueObjectClass *GetInputUseRotationAngle();

	SVInObjectInfoStruct m_svInputImageObjectInfo;
	SVInObjectInfoStruct m_svInputProfileOrientation;
	SVInObjectInfoStruct m_svInputUseRotationAngle;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;
	DWORD m_dwColorNumber;

	BOOL m_bEnableDirection;
	BOOL m_bEnableEdgeSelect;
	BOOL m_bEnablePolarisation;
	BOOL m_bEnablePosition;
	BOOL m_bEnableThreshold;

	SVValueBaseNormalizerClass m_svNormalizer;
};

