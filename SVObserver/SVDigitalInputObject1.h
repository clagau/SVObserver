//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitalInputObject
//* .File Name       : $Workfile:   SVDigitalInputObject1.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:12  $
//******************************************************************************

#ifndef SVDIGITALINPUTOBJECT_H
#define SVDIGITALINPUTOBJECT_H

#include "SVInputObject.h"

class SVDigitalInputObject : public SVInputObject
{
public:

	SVDigitalInputObject( LPCSTR strObjectName );
	SVDigitalInputObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVDIGITALINPUTOBJECT );

	virtual ~SVDigitalInputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();
	virtual BOOL IsCreated();

	virtual HRESULT Read( _variant_t& p_rValue );

	BOOL Force( bool bForce, bool bForcedValue );
	BOOL Invert( bool bInvert );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool GetValue() const;

	BOOL SetChannel( long lChannel );
	long GetChannel() const;

private:

	// Values for this digital input
	long m_lChannel;
	bool m_bForced;
	bool m_bInverted;
	bool m_bLastValue;
	bool m_bDefaultValue;
	bool m_bForcedValue;
};

#endif /* _INC_SVDIGITALINPUTOBJECT_INCLUDED */

