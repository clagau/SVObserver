//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVUtilityLibrary/SVString.h"

class SVLightReferenceAttributeInfo
{
public:
	SVString strName;         // contains the text "Contrast", "Brightness", etc.
	int iIDCaption;         // contains the resource ID of "Contrast", etc. at the time the config was saved. Unfortunately, this ID can change.
	DWORD dwType;
	long lValue;
	long lMin;
	long lMax;
	long lStep;
	
	SVLightReferenceAttributeInfo();
	SVLightReferenceAttributeInfo( const SVLightReferenceAttributeInfo& rhs );
	const SVLightReferenceAttributeInfo& operator = ( const SVLightReferenceAttributeInfo& rhs );
};

//class SVLightReferenceClass
class SVLightReferenceBand
{
public:
	SVLightReferenceBand();
	SVLightReferenceBand( const SVLightReferenceBand& rhs );
	const SVLightReferenceBand& operator=( const SVLightReferenceBand& rhs );
	~SVLightReferenceBand();

	BOOL Create( int iNumAttributes );
	BOOL Destroy();
	void SetFileVersion( unsigned long ulVersion );
    inline int NumAttributes() const {return m_iNumAttributes;}
	SVLightReferenceAttributeInfo& Attribute( int iAttributeIndex ) { return m_aAttributeInfo[iAttributeIndex];}
	const SVLightReferenceAttributeInfo& Attribute( int iAttributeIndex ) const { return m_aAttributeInfo[iAttributeIndex];}
	SVLightReferenceAttributeInfo* AttributeByType( DWORD dwAttributeType );
	const SVLightReferenceAttributeInfo* AttributeByType( DWORD dwAttributeType ) const;
	SVLightReferenceAttributeInfo* AttributeByName( const SVString& sAttributeName );
	const SVLightReferenceAttributeInfo* AttributeByName( const SVString& sAttributeName ) const;
	int AddAttribute(SVLightReferenceAttributeInfo& attribute);


	std::vector< SVLightReferenceAttributeInfo > m_aAttributeInfo;

    unsigned long m_ulFileVersion;

protected:
    int			m_iNumAttributes;    // the number of "attributes", e.g. Contrast, Brightness

};




//class SVLightReferenceArrayClass
class SVLightReference
{
public:
	SVLightReference();
	SVLightReference( const SVLightReference& rhs );
	const SVLightReference& operator=( const SVLightReference& rhs );
	~SVLightReference();

	BOOL Create( int iNumBands );
	void Reset();
	void DeleteAll();
	void SetFileVersion( unsigned long ulVersion );
    bool SetBandData(const SVLightReference& rhs, unsigned int uiBand);
    bool IsSameDimensions( const SVLightReference& rhs );
    inline int NumBands() const {return m_iNumBands;}
	SVLightReferenceBand& Band(int iBand) {return m_aLightReferenceBand[iBand];}
	const SVLightReferenceBand& Band(int iBand) const {return m_aLightReferenceBand[iBand];}

	int AddAttribute(SVLightReferenceAttributeInfo& attribute);

    std::vector<SVLightReferenceBand>	m_aLightReferenceBand;

	unsigned long m_ulFileVersion;

protected:
	int						m_iNumBands;    // the number of bands (channels)
};

