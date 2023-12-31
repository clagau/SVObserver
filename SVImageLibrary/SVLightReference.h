//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLightReference
//* .File Name       : $Workfile:   SVLightReference.H  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:40  $
//******************************************************************************

#pragma  once

//Moved to precompiled header: #include <vector>
#include "ImageFileUtilities.h"


class SVLightReferenceAttributeInfo
{
public:
	std::string strName {};       // contains the text "Contrast", "Brightness", etc.
	int iIDCaption {0};         // contains the resource ID of "Contrast", etc. at the time the config was saved. Unfortunately, this ID can change.
	DWORD dwType {0};			// the Type of the Attribute (corresponds to the Device Parameter (SVDeviceParamEnum))
	long lValue {0L};
	long lMin {0L};
	long lMax {0L};
	long lStep {0L};
	
	SVLightReferenceAttributeInfo() = default;
	~SVLightReferenceAttributeInfo() = default;
};

class SVLightReferenceBand
{
public:
	SVLightReferenceBand() = default;
	~SVLightReferenceBand() = default;

	void Create( int iNumAttributes );
	void Destroy();
    inline int NumAttributes() const {return m_iNumAttributes;}
	SVLightReferenceAttributeInfo& Attribute( int iAttributeIndex ) { return m_aAttributeInfo[iAttributeIndex];}
	const SVLightReferenceAttributeInfo& Attribute( int iAttributeIndex ) const { return m_aAttributeInfo[iAttributeIndex];}
	SVLightReferenceAttributeInfo* AttributeByType( DWORD dwAttributeType );
	const SVLightReferenceAttributeInfo* AttributeByType( DWORD dwAttributeType ) const;
	SVLightReferenceAttributeInfo* AttributeByName( const std::string& sAttributeName );
	const SVLightReferenceAttributeInfo* AttributeByName( const std::string& sAttributeName ) const;
	int AddAttribute(SVLightReferenceAttributeInfo& attribute);

	std::vector< SVLightReferenceAttributeInfo > m_aAttributeInfo;

    unsigned long m_ulFileVersion{0UL};

protected:
	int	m_iNumAttributes {0};    // the number of "attributes", e.g. Contrast, Brightness
};

class SVLightReference
{
public:
	SVLightReference();
	SVLightReference( const SVLightReference& rhs );
	const SVLightReference& operator=( const SVLightReference& rhs );
	~SVLightReference();

	bool Create( int iNumBands );
	void Reset();
	void DeleteAll();
    bool SetBandData(const SVLightReference& rhs, unsigned int uiBand);
    bool IsSameDimensions( const SVLightReference& rhs );
    inline int NumBands() const {return m_iNumBands;}
	SVLightReferenceBand& Band(int iBand) {return m_aLightReferenceBand[iBand];}
	const SVLightReferenceBand& Band(int iBand) const {return m_aLightReferenceBand[iBand];}

	int AddAttribute(SVLightReferenceAttributeInfo& attribute);

    std::vector<SVLightReferenceBand>	m_aLightReferenceBand;

	unsigned long m_ulFileVersion {0UL};

protected:
	int	m_iNumBands {0};    // the number of bands (channels)
};

