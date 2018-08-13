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
#include "InspectionEngine/SVAnalyzer.h"
#include "SVLibrary/SVValueBaseNormalizerClass.h"
#pragma endregion Includes

class SVLinearEdgeProcessingClass;

class SVLinearAnalyzerClass : public SVAnalyzerClass  
{
	SV_DECLARE_CLASS( SVLinearAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVLinearAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARANALYZER );
	virtual ~SVLinearAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent ) override;

	SVLinearEdgeProcessingClass* GetEdgeA();
	SVLinearEdgeProcessingClass* GetEdgeB();

	HRESULT GetPixelDepth();

	SVBoolValueObjectClass m_svShowAllEdgeAOverlays;
	SVBoolValueObjectClass m_svShowAllEdgeBOverlays;

protected:
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructVector &p_MultiLineArray ) override;

	HRESULT GetProjectedExtent( long p_lIndex, long &p_rlBottom, SVImageExtentClass &p_rsvImageExtent );

	HRESULT GetInputProfileOrientation(long& rProfileOrientation);
	HRESULT GetInputUseRotationAngle(bool& rUseRotationAngle);
	bool ValidateEdgeA(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateEdgeB(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Set the parameter to the list, if available and valid.
	/// \param rName [in] Full dotted name of the parameter.
	/// \param inserter [in] backinsert iterator to added the parameter.
	/// \returns bool True if successfully.
	bool setParameterToList(const std::string& rName, std::back_insert_iterator<SvOi::ParametersForML> inserter) const;

	SvOl::SVInObjectInfoStruct m_InputImageObjectInfo;
	SvOl::SVInObjectInfoStruct m_InputProfileOrientation;
	SvOl::SVInObjectInfoStruct m_InputUseRotationAngle;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;
	DWORD m_dwColorNumber;

	bool m_bEnableDirection;
	bool m_bEnableEdgeSelect;
	bool m_bEnablePolarisation;
	bool m_bEnablePosition;
	bool m_bEnableThreshold;

	SVValueBaseNormalizerClass m_svNormalizer;
};

