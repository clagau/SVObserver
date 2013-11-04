//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBContent
//* .File Name       : $Workfile:   SVGigeCameraDBContent.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:16  $
//******************************************************************************
#pragma once

#include "SVSystemLibrary/SVSAXContentHandlerImpl.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#include "SVGigeCameraParametersLibrary/SVGigeFeatureStringEnumList.h"
#include "SVGigeCameraParametersLibrary/SVGigeFeature.h"
#include "SVGigeCameraParametersLibrary/SVGigeParameterAccessor.h"

// class to handle SAX parsing for GigeDeviceParameter database
class SVGigeCameraDBContent : public SVSAXContentHandlerImpl  
{
private:
	// GigeFeatureSelector Info
	struct SVGigeFeatureSelectorInfo
	{
		SVString m_GigeFeatureSelectorName;
		SVString m_GigeFeatureSelectorValue;
		
		void clear() 
		{ 
			m_GigeFeatureSelectorName.clear(); 
			m_GigeFeatureSelectorValue.clear(); 
		}
	};

	// GigeFeature Info
	struct SVGigeFeatureInfo
	{
		SVString m_GigeFeatureName;
		SVMatroxDigitizerFeature::SVFeatureTypeEnum m_GigeFeatureType;
		SVGigeFeature::FeatureSupportedEnum m_FeatureSupported;
		SVGigeFeature::FeatureAccessEnum m_FeatureAccess;

		SVGigeFeatureSelectorInfo m_GigeSelector;

		typedef SVBidirectionalMap<SVDeviceParamString, SVGigeFeatureString>::type GigeFeatureStringEnumList;
		GigeFeatureStringEnumList m_GigeFeatureStringEnumList;

		void clear() 
		{ 
			m_GigeFeatureName.clear(); 
			//m_GigeFeatureType = 
			m_FeatureSupported = SVGigeFeature::NotSupported; 
			m_FeatureAccess = SVGigeFeature::ReadOnly;

			m_GigeSelector.clear();

			m_GigeFeatureStringEnumList.clear();
		}
	};

	// GigeAccessor Info
	struct SVGigeAccessorInfo
	{
		SVString m_getterName;
		SVString m_setterName;

		void clear()
		{
			m_getterName.clear();
			m_setterName.clear();
		}
	};

	// GigeParameterAccessor Info
	struct SVGigeParameterAccessorInfo
	{
		SVGigeAccessorInfo m_accessor;
		SVGigeFeatureInfo m_feature;

		void clear()
		{
			m_accessor.clear();
			m_feature.clear();
		}
	};
	
	// GigeDeviceParameterStruct Info
	struct SVGigeDeviceParameterStructInfo
	{
		SVString m_GigeDeviceParameterName;
		VARTYPE m_GigeDeviceParameterDataType;

		SVGigeParameterAccessorInfo m_GigeFeatureAccessor;

		void clear()
		{
			m_GigeDeviceParameterName.clear();
			m_GigeDeviceParameterDataType = VT_EMPTY;
			m_GigeFeatureAccessor.clear();
		}
	};

	SVGigeDeviceParameterStructInfo m_GigeDeviceParameterInfo;
	SVGigeParameterEnum m_GigeParameterEnum; // SVGigeDeviceParameterMap Key
	SVGigeDeviceParameterMap m_GigeDeviceParameterMap;
	HRESULT m_errorCode;
	SVString m_errorText;
	
public:
    SVGigeCameraDBContent();
    virtual ~SVGigeCameraDBContent();
        
	virtual HRESULT STDMETHODCALLTYPE startElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName, MSXML2::ISAXAttributes* pAttributes);
    virtual HRESULT STDMETHODCALLTYPE endElement(unsigned short* pwchNamespaceUri, int cchNamespaceUri, unsigned short* pwchLocalName, int cchLocalName, unsigned short* pwchRawName, int cchRawName);
	
	void ReportError(wchar_t* pErrorMsg, HRESULT erorCode);

	const SVGigeDeviceParameterMap& GetGigeDeviceParameterMap() const;

private:
	void GetGigeParameterAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeDeviceParameterStructAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeAccessorAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureSelectorAttributes(MSXML2::ISAXAttributes* pAttributes);
	void GetGigeFeatureStringEnumAttributes(MSXML2::ISAXAttributes* pAttributes);

	SVGigeParameterAccessor ConstructGigeParameterAccessor();

	VARTYPE GetVarType(wchar_t* vartypeString) const;
	SVGigeParameterAccessor::ParamGetter GetGetterFunc();
	SVGigeParameterAccessor::ParamSetter GetSetterFunc();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeCameraDBContent.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:46:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2010 08:13:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
