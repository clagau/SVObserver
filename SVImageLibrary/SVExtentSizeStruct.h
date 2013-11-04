//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentSizeStruct
//* .File Name       : $Workfile:   SVExtentSizeStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:14  $
//******************************************************************************

#ifndef SVEXTENTSIZESTRUCT_H_
#define SVEXTENTSIZESTRUCT_H_
#pragma once

struct SVExtentSizeStruct
{
	SVExtentSizeStruct();
	SVExtentSizeStruct( const SVExtentSizeStruct& p_rsvValue );
	SVExtentSizeStruct( const SIZE& p_roValue );
	SVExtentSizeStruct( double p_dCX, double p_dCY );
	virtual ~SVExtentSizeStruct();

	const SVExtentSizeStruct& operator = ( const SVExtentSizeStruct& p_rsvValue );
	const SVExtentSizeStruct& operator = ( const SIZE& p_roValue );

	virtual HRESULT Initialize();

	bool operator == ( const SVExtentSizeStruct& p_rsvValue ) const;
	bool operator != ( const SVExtentSizeStruct& p_rsvValue ) const;

	operator SIZE() const;

	double m_dCX;
	double m_dCY;
};


#endif // #ifndef SVEXTENTSIZESTRUCT_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentSizeStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:44:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:38:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/