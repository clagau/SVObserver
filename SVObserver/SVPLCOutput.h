//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputObject
//* .File Name       : $Workfile:   SVPLCOutput.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:26:18  $
//******************************************************************************

#ifndef SVPLCOUTPUT_H_INCLUDED
#define SVPLCOUTPUT_H_INCLUDED

#include <vector>
#include "svp_src\plcbaseclass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObject.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

class SVPLCOutputObject : public SVObjectClass
{
public:
	SV_DECLARE_CLASS( SVPLCOutputObject );

	SVPLCOutputObject(void);
	virtual ~SVPLCOutputObject(void);
	
	// Get Functions
	CString GetInputValueObjectName();
	HRESULT GetInputValueObjectGUID( GUID& p_rGUID ) const;
	HRESULT GetPLCName( CString& p_strPLCID ) const ;
	CString GetPLCName( ) const ;
	long GetBit() const;
	const long GetElement() const;
	long GetSize() const ;

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL GetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent ) const;

	HRESULT GetMaterials( SVMaterialsTreeAdapter& p_rMaterials, SVMaterialsTreeAdapter::SVTreeContainer* p_pParent );
	HRESULT Update( SVMaterialsTreeAdapter& p_rMaterials );

	SVPLCDataTypeEnum GetDataType() const;
	HRESULT SetDataType( SVPLCDataTypeEnum p_eType );


	// Set Functions
	HRESULT SetInputObject( SVValueObjectClass* p_pObject );
	HRESULT SetInputObjectId( GUID p_ObjectId );
	HRESULT SetPLCName( CString p_strPLCID );
	HRESULT SetBit( const long p_lBit );
	bool SetElement( const long p_lElement );

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	HRESULT SetSize( long p_lSize );
	bool GetConvert() const;
	HRESULT SetConvert(const bool p_bConvert);
	HRESULT SetSizeInBits( bool p_bSizeInBits );
	bool SizeInBits();

private:
	// Member Variables

	// flag used to determine wether to convert data or just copy data.
	bool m_bConvert;
	// The PLC address
	CString m_strAddress;

	// What type of Data
	SVPLCDataTypeEnum m_eDataType;

	// The number of PLC elements
	long m_lSize;

	// The PLC Element This is what we use to determine the PLC address
	long m_lPLCElement;

	long m_lBit;

	bool m_bSizeIsInBits;

	// The Value Objects name.
	CString m_strObjectName;
	// The value object
	SVValueObjectClass* m_pValueObject;	

	// Guid of the value object used to output
	SVGUID m_InputObjectId;	

	// This replaces the GUID of the PPQ. PLC_1 will connect to PPQ_1. PLC_2 - PPQ_2...
	CString m_strPLCId;	// Which plc to connect to for a specific PPQ.

};

typedef std::vector<SVPLCOutputObject*> SVPLCOutputObjectList;

#endif


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCOutput.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:26:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2012 11:25:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Sep 2012 15:24:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 12:25:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

