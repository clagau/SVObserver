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

#ifndef SVLIGHTREFERENCE_H
#define SVLIGHTREFERENCE_H

#include <vector>
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

#endif //SVLIGHTREFERENCE_H

//******************************************************************************

//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLightReference.H_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Mar 2011 15:40:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Nov 2003 16:29:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetAttributeByName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Sep 2003 08:54:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added functions AddAttribute and AttributeByType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:01:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Jul 2003 10:01:50   ebeyeler
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
 * 
 *    Rev 3.2   22 Apr 2003 10:39:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
