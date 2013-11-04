//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterials
//* .File Name       : $Workfile:   SVMaterials.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:38  $
//******************************************************************************

#ifndef SVMATERIALS_H
#define SVMATERIALS_H

#include <deque>
#include "SVContainerLibrary/SVMap.h"

#include "SVMaterialsLibrary/SVMaterialData.h"

#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

/**
@SVObjectName Materials

@SVObjectOverview This file defines the Materials class and the types associated with Materials.

@SVObjectOperations Available operations include multiple constructors; a virtual destructor; multiple begins, ends, finds, gets, and sets; emtpy, clear, and erase operations; an assignment operator.

*/
class SVMaterials
{
public:
	typedef SVString SVIdentifier;
	typedef SVMaterialData SVData;
	typedef std::pair<SVIdentifier, SVData> SVMapData;
	//typedef SVMap< SVIdentifier, SVData > SVMaterialMap;
	typedef std::deque< SVMapData > SVMaterialMap;
	typedef SVMaterialMap::const_iterator const_iterator;
	typedef SVMaterialMap::iterator iterator;
	typedef SVMaterialMap::size_type size_type;

	SVMaterials();
	SVMaterials( const SVMaterials& p_rMaterials );

	virtual ~SVMaterials();

	bool empty() const;

	void clear();

	size_type size() const;

	const_iterator begin() const;
	iterator begin();

	const_iterator end() const;
	iterator end();

	const_iterator find( const SVIdentifier& p_Identifier ) const;
	iterator find( const SVIdentifier& p_Identifier );

	//size_type erase( const SVIdentifier& p_rKey );
	iterator erase( iterator p_Where );

	HRESULT GetMaterial( const SVIdentifier& p_Identifier, SVData& p_rData ) const;
	HRESULT GetMaterial( const SVIdentifier& p_Identifier, _variant_t& p_rVariant ) const;

	HRESULT AddMaterial( const SVIdentifier& p_Identifier, const _variant_t& p_rVariant );
	HRESULT UpdateMaterial( const SVIdentifier& p_Identifier, const _variant_t& p_rVariant );

	HRESULT AddMaterials( iterator p_First, iterator p_Last );
	HRESULT UpdateMaterials( iterator p_First, iterator p_Last );

	const SVMaterials& operator=( const SVMaterials& p_rMaterials );

	template<class Iter>
	SVMaterials(Iter p_First, Iter p_Last)
	{
		for (Iter it = p_First; it != p_Last; ++it)
		{
			AddMaterial((*it).first, (*it).second);
		}
	}
protected:
	SVMaterialMap m_Materials;

};

typedef SVSharedPtr< SVMaterials > SVMaterialsPtr;

#endif // #ifndef SVMATERIALS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterials.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed storage from map to deque for materials class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

