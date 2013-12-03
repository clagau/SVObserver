// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODeviceClass
// * .File Name       : $Workfile:   SVODeviceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:47:30  $
// ******************************************************************************

#ifndef SVODEVICECLASS_H
#define SVODEVICECLASS_H

#include <comdef.h>
#include <string>

#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"

#include "SVProcessFunctor.h"
#include "SVOCallbackClass.h"
#include "SVOResponseClass.h"
#include "SVQueueObject.h"

//typedef HRESULT ( CALLBACK *SVODeviceClassCallbackPtr )( void *, /* Owner */
//                                                         void *, /* Caller */
//                                                         SVOResponseClass * );

/*
This class represents the base functionality required for a device.
*/
class SVODeviceClass  
{
public:
	typedef std::string SVDeviceName;
	typedef SVProcessFunctor< SVODeviceClass > SVProcessFunctorImpl;

	friend class SVProcessFunctor< SVODeviceClass >;

	static VOID CALLBACK APCProc( ULONG_PTR dwParam );

	SVODeviceClass( LPCTSTR deviceName );
	virtual ~SVODeviceClass();

	virtual bool IsValid() const;
	virtual bool IsStarted() const;

	virtual void ClearDevice();

	virtual HRESULT Create( unsigned long ulSize = 50 );

	virtual HRESULT Destroy();

	virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, 
	                                  void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback,
	                                    void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterAllCallbacks();

	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Reset();
	virtual HRESULT Pulse();

	LPCTSTR GetDeviceName() const;
	virtual void SetDeviceName( LPCTSTR p_szName );

protected:
	typedef SVRingBuffer< SVOResponseClass > SVResponseQueue;

	/* SVODeviceClass Callback Functions */

	virtual HRESULT Process( bool& p_WaitForEvents );

	/* Device Callback Functions */

	virtual HRESULT Notify( SVOResponseClass& p_rResponse );

	virtual HRESULT GetUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );
	virtual HRESULT AddUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );

	virtual HRESULT ProcessResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );

	virtual HRESULT ExecuteCallback( SVCallbackClassPtrQueue &rCallbackList, SVOResponseClass& p_rResponse );

	virtual HRESULT GetIsValid( bool &rbIsValid )const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT TriggerDevice();

	//This attribute is a container that holds the list of registered recipients of the acquisition notifications.
	SVCallbackClassPtrQueue m_CallbackList;

	//This attribute holds the device name.
	SVDeviceName mDeviceName;

private:
	SVODeviceClass();

	typedef VOID ( CALLBACK* SVAPCFunctionPtr )( ULONG_PTR );

	HRESULT DestroyLocal();

	//This attribute holds the creation state of this object.
	bool mbIsCreated;
	//This attribute holds the validity state of this object.
	bool mbIsValid;
	//This attribute holds the processing state of this object.
	bool mbIsStarted;

	//This attribute holds the handle to the application thread.
	SVAsyncProcedure< SVAPCFunctionPtr, SVProcessFunctorImpl > m_Thread;

	//This attribute is a container that holds the unused resposes in the callback system.
	SVResponseQueue mUsedQueue;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOLibrary\SVODeviceClass.h_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:55:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 May 2011 15:37:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated device class to use the new Ring Buffer to commuicate to the thread using lockless mentality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Mar 2011 15:42:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 10:58:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jan 2010 14:55:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Sep 2009 09:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Aug 2009 16:43:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated new threading methodology to remove crash when closing a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2009 08:56:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 12:56:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes the name string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   14 Jul 2009 10:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Feb 2003 16:42:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added IsStarted method to the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
