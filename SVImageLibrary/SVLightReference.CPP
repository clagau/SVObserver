//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLightReference
//* .File Name       : $Workfile:   SVLightReference.CPP  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 07:16:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVLightReference.h"

SVLightReferenceAttributeInfo::SVLightReferenceAttributeInfo()
{
	iIDCaption=0;
	dwType=0;
	lValue=0;
	lMin=0;
	lMax=0;
	lStep=0;
}

SVLightReferenceAttributeInfo::~SVLightReferenceAttributeInfo()
{
}

SVLightReferenceAttributeInfo::SVLightReferenceAttributeInfo( const SVLightReferenceAttributeInfo& rhs )
{
	*this = rhs;
}

const SVLightReferenceAttributeInfo& SVLightReferenceAttributeInfo::operator = ( const SVLightReferenceAttributeInfo& rhs )
{
	if ( this != &rhs )
	{
		strName    = rhs.strName;
		iIDCaption = rhs.iIDCaption;
		dwType     = rhs.dwType;
		lValue     = rhs.lValue;
		lMin       = rhs.lMin;
		lMax       = rhs.lMax;
		lStep      = rhs.lStep;
		
	}
	return *this;
}


SVLightReferenceBand::SVLightReferenceBand()
{
	m_iNumAttributes        = 0;
}

SVLightReferenceBand::SVLightReferenceBand( const SVLightReferenceBand& rhs )
{
	m_iNumAttributes         = 0;

	*this = rhs;
}

SVLightReferenceBand::~SVLightReferenceBand()
{
	Destroy();
}

const SVLightReferenceBand& SVLightReferenceBand::operator=( const SVLightReferenceBand& rhs )
{
    if ( this != &rhs )
	{
		Create(rhs.m_iNumAttributes);
		m_aAttributeInfo = rhs.m_aAttributeInfo;
	}

	return( *this );
}

void SVLightReferenceBand::Create(int iSize)
{
	Destroy();

	// we can now create 0 size LR's (for digital). The attributes are added with calls to AddAttribute
	m_iNumAttributes = iSize;
	m_aAttributeInfo.resize(iSize);
}

void SVLightReferenceBand::Destroy()
{
	m_aAttributeInfo.clear();
}

SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByType( DWORD dwAttributeType )
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].dwType == dwAttributeType )
			return &m_aAttributeInfo[i];
	}

	return nullptr;
}

const SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByType( DWORD dwAttributeType ) const
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].dwType == dwAttributeType )
			return &m_aAttributeInfo[i];
	}

	return nullptr;
}

SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByName( const SVString& sAttributeName )
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].strName == sAttributeName )
			return &m_aAttributeInfo[i];
	}

	return nullptr;
}

const SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByName( const SVString& sAttributeName ) const
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].strName == sAttributeName )
			return &m_aAttributeInfo[i];
	}

	return nullptr;
}

int SVLightReferenceBand::AddAttribute(SVLightReferenceAttributeInfo& attribute)
{
	int iIndex = -1;

	iIndex = NumAttributes();
	m_aAttributeInfo.push_back( attribute );
	m_iNumAttributes = static_cast< int >( m_aAttributeInfo.size() );

	return iIndex;
}

SVLightReference::SVLightReference()
{
	m_iNumBands = 0;
}

SVLightReference::SVLightReference( const SVLightReference& rhs )
{
	m_iNumBands = 0;

	*this = rhs;
}

SVLightReference::~SVLightReference()
{
	DeleteAll();
}

bool SVLightReference::Create( int iNewSize )
{
	// Rosco removed this to prevent annoying assert dialogs even though eric told him not to.
    // assert (iNewSize > 0);

	DeleteAll();

	bool bOk = 0 < iNewSize;
	if ( bOk )
	{
		m_aLightReferenceBand.resize( iNewSize );
		m_iNumBands = iNewSize;
	}

	return bOk;
}

void SVLightReference::Reset()
{
	m_aLightReferenceBand.clear();
	m_aLightReferenceBand.resize( m_iNumBands );
}

void SVLightReference::DeleteAll()
{
	m_aLightReferenceBand.clear();
	
	m_iNumBands = 0;
}

const SVLightReference& SVLightReference::operator = ( const SVLightReference& rhs )
{
    if (this != &rhs)
    {
	    if(	Create( rhs.m_iNumBands ) )
	    {
			m_aLightReferenceBand = rhs.m_aLightReferenceBand;
	    }
    }

	return( *this );
}

bool SVLightReference::IsSameDimensions( const SVLightReference& rhs )
{
    bool bSame = false;

    if (m_iNumBands == rhs.NumBands())
    {
        bSame = true;
        for (int i = 0; i < m_iNumBands && bSame == true; i++)
        {
            bSame = bSame && (m_aLightReferenceBand[i].NumAttributes() == rhs.m_aLightReferenceBand[i].NumAttributes());
        }
    }

    return bSame;
}

bool SVLightReference::SetBandData(const SVLightReference& rhs, unsigned int uiBand)
{
    bool bResult = false;

    if (IsSameDimensions(rhs))
    {
        if (uiBand < (unsigned int) m_iNumBands)
        {
            m_aLightReferenceBand[uiBand] = rhs.m_aLightReferenceBand[uiBand];

            bResult = true;
        }
    }
	else
	{
		assert(false);
	}

    return bResult;
}

int SVLightReference::AddAttribute(SVLightReferenceAttributeInfo& attribute)
{
	int iIndex = -1;

	for( unsigned int i=0; i < m_aLightReferenceBand.size(); i++)
	{
		iIndex = m_aLightReferenceBand[i].NumAttributes();
		m_aLightReferenceBand[i].AddAttribute( attribute );
	}

	return iIndex;
}

