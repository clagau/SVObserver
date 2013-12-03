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
//  ASSERT( rhs.m_iNumAttributes > 0 );
//	if(	iNumAttributes != rhs.iNumAttributes )
    if ( this != &rhs )
	{
		if ( Create( rhs.m_iNumAttributes ) )
		{
			m_aAttributeInfo = rhs.m_aAttributeInfo;
		}
	}

	return( *this );
}

BOOL SVLightReferenceBand::Create(int iSize)
{
//    ASSERT (iSize > 0);

	BOOL bOk = FALSE;

	bOk = Destroy();
	if ( bOk )
	{
		//bOk = 0 < iSize;	// we can now create 0 size LR's (for digital). The attributes are added with calls to AddAttribute
		if ( bOk )
		{
			m_iNumAttributes = iSize;
			m_aAttributeInfo.resize( iSize );

			bOk = TRUE;

			if ( !bOk )
			{
				Destroy();
			}
		}
	}

	return bOk;
}

BOOL SVLightReferenceBand::Destroy()
{
	BOOL bOk = TRUE;

	m_aAttributeInfo.clear();

	return bOk;
}

void SVLightReferenceBand::SetFileVersion(unsigned long ulVersion)
{
	m_ulFileVersion = ulVersion;
}

SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByType( DWORD dwAttributeType )
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].dwType == dwAttributeType )
			return &m_aAttributeInfo[i];
	}

	return NULL;
}

const SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByType( DWORD dwAttributeType ) const
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].dwType == dwAttributeType )
			return &m_aAttributeInfo[i];
	}

	return NULL;
}

SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByName( const SVString& sAttributeName )
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].strName == sAttributeName )
			return &m_aAttributeInfo[i];
	}

	return NULL;
}

const SVLightReferenceAttributeInfo* SVLightReferenceBand::AttributeByName( const SVString& sAttributeName ) const
{
	for ( int i = 0; i < m_iNumAttributes; i++)
	{
		if ( m_aAttributeInfo[i].strName == sAttributeName )
			return &m_aAttributeInfo[i];
	}

	return NULL;
}

int SVLightReferenceBand::AddAttribute(SVLightReferenceAttributeInfo& attribute)
{
	int iIndex = -1;

	iIndex = NumAttributes();
	m_aAttributeInfo.push_back( attribute );
	m_iNumAttributes = static_cast< int >( m_aAttributeInfo.size() );

	return iIndex;
}













//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLightReference
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


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

BOOL SVLightReference::Create( int iNewSize )
{
	// Rosco removed this to prevent annoying assert dialogs even though eric told him not to.
    // ASSERT (iNewSize > 0);

	BOOL bOk = FALSE;

	DeleteAll();

	bOk = 0 < iNewSize;
	if ( bOk )
	{
		m_aLightReferenceBand.resize( iNewSize );
		bOk = TRUE;
		if ( bOk )
		{
			m_iNumBands = iNewSize;
		}
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
	// Rosco removed this to prevent annoying assert dialogs even though eric told him not to.
    // ASSERT( rhs.m_iNumBands > 0 );
    
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
		ASSERT(FALSE);
	}

    return bResult;
}


void SVLightReference::SetFileVersion(unsigned long ulVersion)
{
	m_ulFileVersion = ulVersion;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLightReference.CPP_v  $
 * 
 *    Rev 1.1   01 Oct 2013 07:16:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:51:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Mar 2011 15:40:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Dec 2010 15:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jun 2007 11:09:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Dec 2003 14:44:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   allow for 0 size attribute array
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Nov 2003 16:29:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetAttributeByName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2003 10:41:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Removed asserts in the light reference class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Sep 2003 08:54:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added functions AddAttribute and AttributeByType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:01:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Jul 2003 10:01:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVLightReferenceClass to SVLightReferenceBand
 * renamed SVLightReferenceArrayClass to SVLightReference
 * cleaned up unnecessary comment blocks
 * changes made to facilitate integration with SVDeviceParam:
 * moved attribute info that was part of SVLightReferenceBand into new SVLightReferenceAttributeInfo
 * used std::vector instead of C-style arrays
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   22 Apr 2003 10:39:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
