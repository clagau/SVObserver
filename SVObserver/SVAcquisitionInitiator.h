//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionInitiator
//* .File Name       : $Workfile:   SVAcquisitionInitiator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:22:54  $
//******************************************************************************
#ifndef SVACQUISITIONINITIATOR_H
#define SVACQUISITIONINITIATOR_H

#include <vector>
#include <map>

struct SVCallbackStruct;
class SVDigitizerLoadLibraryClass;

typedef std::vector<unsigned long> SVAcquisitionInitiatorHandleList;
typedef std::map<SVDigitizerLoadLibraryClass*, SVAcquisitionInitiatorHandleList> SVAcquisitionInitiatorList;

class SVAcquisitionInitiator
{
private:
	SVAcquisitionInitiatorList m_initiatorList;

public:
	SVAcquisitionInitiator();
	SVAcquisitionInitiator(const SVAcquisitionInitiatorList& initatorList);
	
	void Add(SVDigitizerLoadLibraryClass* pAcqDLL, unsigned long handle);

	HRESULT EnableInternalTrigger();
	HRESULT Exec();
	HRESULT RegisterCallback(SVCallbackStruct& p_rCallback );
	HRESULT UnRegisterCallback(SVCallbackStruct& p_rCallback );
	HRESULT UnRegisterAllCallbacks();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAcquisitionInitiator.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:22:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 11:19:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
