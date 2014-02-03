//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputTool
//* .File Name       : $Workfile:   SVRemoteInputTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 12:03:56  $
//******************************************************************************

#ifndef SVREMOTEINPUTTOOL_H
#define SVREMOTEINPUTTOOL_H

#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVObjectLibrary/SVObjectNotifyTemplate.h"
#include "SVOLibrary/SVQueueObject.h"

#include "SVObjectCommandDataJson.h"
#include "SVTool.h"
#include "SVVariantValueObjectClass.h"

class SVRemoteInputTool : 
	public SVToolClass,
	public SVObjectNotifyTemplate< SVObjectCommandDataJsonPtr >
{
	SV_DECLARE_CLASS( SVRemoteInputTool )

public:
	SVRemoteInputTool( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVREMOTEINPUTTOOL );
	virtual ~SVRemoteInputTool();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	virtual HRESULT ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr );

	SVObjectClass* GetInputObject() const;
	HRESULT ClearInputObject();
	HRESULT SetInputObject( const SVGUID& p_rObjectId );

protected:
	typedef SVTQueueObject< SVObjectCommandDataJsonPtr > SVCommandQueue;

	class SVCommandQueueElement : public SVCommandTemplate
	{
	public:
		SVCommandQueueElement( const SVGUID& p_rObjectId, SVObjectCommandDataJsonPtr& p_rJsonCommandPtr );

		virtual ~SVCommandQueueElement();

		virtual HRESULT Execute();

	private:
		SVCommandQueueElement();

		SVGUID m_ObjectId;
		SVObjectCommandDataJsonPtr m_JsonCommandPtr;

	};

	struct SVDataElement
	{
		SVDataElement();
		SVDataElement( const SVString& p_rElementData, long p_ElementIdentifier );
		SVDataElement( const SVString& p_rElementData );
		SVDataElement( const SVDataElement& p_rObject );

		virtual ~SVDataElement();

		bool operator==( const SVDataElement& p_rObject ) const;
		bool operator<( const SVDataElement& p_rObject ) const;

		SVString m_ElementData;
		long m_ElementIdentifier;

	};

	typedef SVBiUniqueMap< SVDataElement, long >::type SVDataContainer;

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );

	HRESULT ProcessCommandQueue();

	SVInObjectInfoStruct m_InputObjectInfo;

	SVVariantValueObjectClass m_MatchedValueId;
	SVVariantValueObjectClass m_MatchedValue;

	long m_ElementIdentifier;
	SVCommandQueue m_Commands;
	SVDataContainer m_Elements;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteInputTool.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 12:03:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:48:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:36:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new remote functionality to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/