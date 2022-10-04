//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageExtentClass
//* .File Name       : $Workfile:   SVImageExtentClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:38  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVExtentFigureStruct.h"
#include "SVExtentLineStruct.h"
#include "SVExtentMultiLineStruct.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes


typedef std::map<SvPb::SVExtentPropertyEnum, std::string> SVExtentPropertyStringMap;
typedef std::map<SvPb::SVExtentPropertyEnum, double> SVExtentPropertyValueMap;
typedef std::map<SvPb::SVExtentPropertyEnum, std::pair<SvPb::SVExtentPropertyEnum, SvPb::SVExtentPropertyEnum>> SVExtentPropertyPointMap;

struct SVExtentOffsetStruct;

class SVImageExtentClass
{
public:
	SVImageExtentClass();
	SVImageExtentClass(const SVImageExtentClass& rRhs) =default;
	~SVImageExtentClass() = default;

	SVImageExtentClass& operator=(const SVImageExtentClass& rRhs);

	//Set all values to zero
	void Initialize();

	//type of transformation 
	SvPb::SVExtentTranslationEnum GetTranslation() const;
	void SetTranslation( SvPb::SVExtentTranslationEnum p_eTranslation );

	void DisableExtentProperty( SvPb::SVExtentPropertyEnum eProperty );

	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, long &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, double &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, POINT &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue ) const;

	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData = true);
	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData = true);

	//Calculate output data from input
	HRESULT UpdateDataRecalculateOutput();

	//type of output data 
	bool hasFigure() const;
	const SVExtentFigureStruct& GetFigure() const { return m_figure; }


	/// GetRectangle  
	///  Retrieves the rect values for the input/parent image that is 
	///  referenced by the current Extents.
	HRESULT GetRectangle( RECT &p_roRect ) const;

	/// GetOutputRectangle  
	///  Retrieves the rect values for the output image that is referenced by 
	///  the current Extents.  The translation value is used in determining 
	///  these dimensions.
	HRESULT GetOutputRectangle( RECT &p_roRect ) const;
	
	/// GetLogicalRectangle  
	///  Retrieves the rect values based on the ROI of the current extents.
	HRESULT GetLogicalRectangle( RECT &p_roRect ) const;



	///Note that the value in TranslateFromOutputSpace are not allowed to be references
	///because if the input value and result use the same object this causes problems
	///as the result changes the input value during the function execution!
	//Where is the point in the source image?
	HRESULT TranslateFromOutputSpace(SVPoint<double> value, SVPoint<double>& rResult) const;
	HRESULT TranslateFromOutputSpace( SVExtentFigureStruct value, SVExtentFigureStruct& rResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentLineStruct value, SVExtentLineStruct& rResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentMultiLineStruct value, SVExtentMultiLineStruct& rResult ) const;

	HRESULT TranslateLineFromOutputSpace(const SVPoint<double>& rRadiusPoint, const SVPoint<double>& rRotatePoint, double dLineAngle, SVPoint<double> value, SVPoint<double>& rResult) const;

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	//move to toolextent
	HRESULT GetTitlePoint(POINT& rTitlePoint) const;
	HRESULT GetTitlePoint(SVPoint<double>& rTitlePoint) const;

	bool operator==(const SVImageExtentClass& rRhs) const;
	bool operator!=(const SVImageExtentClass& rRhs) const;

	HRESULT UpdateSourceOffset( SVExtentOffsetStruct& rOffsetData ) const;

	/// Set the image extent data depending of an image file.
	/// \param pFileName [in] Filename of the image.
	/// \returns HRESULT S_OK on success.
	HRESULT SetDataFromFile(LPCTSTR pFileName);

	void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const;

	bool OutputDebugInformationOnExtent(const char* pDescription, const SVImageExtentClass * pReference=nullptr, long deltaThreshold=0) const ;
	void setIsUpdated(bool isupdate) { m_isUpdated = isupdate; };
	bool getIsUpdated() const  { return m_isUpdated; }
	void setTransfermatrix(const std::vector<double>& rMatrix  );
private:

	void ClearOutputData();

	HRESULT BuildOutputDimensions();
	HRESULT BuildFigure();

	HRESULT setProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue);

	bool isEnabled(SvPb::SVExtentPropertyEnum eProperty) const;

	// Input Attributes
	SvPb::SVExtentTranslationEnum m_eTranslation;
	// Output Attributes
	SVExtentFigureStruct m_figure;

	int m_properties{0L};

	SVExtentPropertyValueMap m_extentValues;
	bool m_isUpdated = false;
	double m_TransferAngle=0.0;
	std::array<std::array<double, 3>, 2> m_TransferMatrix = {{{1.0, 0.0,0.0},{0.0, 1.0,0.0}}};
	bool m_useTransferMatrix {false};
};

