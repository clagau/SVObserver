//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectValueInterface
//* .File Name       : $Workfile:   SVTaskObjectValueInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   05 Nov 2014 05:13:02  $
//******************************************************************************

#ifndef SVTASKOBJECTVALUEINTERFACE_H
#define SVTASKOBJECTVALUEINTERFACE_H

#include "SVValueObjectReference.h"
#include "SVTaskObjectInterfaceInputRequestStruct.h"

class SVTaskObjectClass;
class SVToolClass;

class SVTaskObjectValueInterface
{
public:
	SVTaskObjectValueInterface();
	SVTaskObjectValueInterface( SVTaskObjectClass *p_psvTaskObject );
	virtual ~SVTaskObjectValueInterface();

	SVTaskObjectClass *GetTaskObject();
	void SetTaskObject( SVTaskObjectClass *p_psvTaskObject );

	virtual HRESULT GetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, const _variant_t& p_rVariantValue );

	virtual HRESULT GetValue( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, VARIANT& p_rValue ) const;

	template <class InIterator>
	HRESULT AddInputRequest(SVValueObjectReference ObjectRef, InIterator begin, InIterator end)
	{
		HRESULT Result( S_OK );

		ObjectRef->SetArraySize( std::distance(begin, end) );

		long Index = 0;
		InIterator Iter( begin );
		while( end != Iter && S_OK == Result )
		{
			ObjectRef.SetArrayIndex( Index );
			Result = AddInputRequest( ObjectRef, *Iter );
			++Index;
			++Iter;
		}

		return Result;
	}

	virtual HRESULT AddInputRequest( SVInputRequestStructMap p_map );
	virtual HRESULT AddInputRequest( SVValueObjectReference p_svObjectRef, LPCTSTR p_szValue );
	virtual HRESULT AddInputRequest( SVValueObjectReference p_svObjectRef, double p_dValue );
	virtual HRESULT AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, LPCTSTR p_szValue );
	virtual HRESULT AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, double p_dValue );
	virtual HRESULT AddInputRequestMarker();
	virtual HRESULT RunOnce( SVToolClass *p_psvTool = NULL );

private:
	SVTaskObjectClass *m_psvTaskObject;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTaskObjectValueInterface.h_v  $
 * 
 *    Rev 1.1   05 Nov 2014 05:13:02   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Bugfix: Kernel update problem in Tool Adjust dialog
 * Added Method: AddInputRequest for arrays
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:35:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2012 11:22:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
