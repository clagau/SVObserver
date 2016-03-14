//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:00:04  $
//******************************************************************************

#ifndef SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_H
#define SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_H

#include "SVObjectCommandWrapperTemplate.h"

template< typename SVCommandPtr >
class SVObjectAsynchronousCommandTemplate
{
public:
	SVObjectAsynchronousCommandTemplate();
	SVObjectAsynchronousCommandTemplate( const SVObjectAsynchronousCommandTemplate& p_rObject );
	SVObjectAsynchronousCommandTemplate( SVObjectClass& p_rObject, const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectAsynchronousCommandTemplate();

	HRESULT SubmitCommand();
	HRESULT IsCommandComplete();
	HRESULT WaitForCommandToComplete( DWORD p_TimeoutInMilliseconds );

	const SVCommandPtr& GetCommandPtr() const;

private:
	typedef typename SVObjectCommandWrapperTemplate< SVCommandPtr >::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

	SVObjectClass *m_Object;
	SVCommandPtr m_CommandPtr;
	SVObjectCommandWrapperPtr m_WrapperPtr;

};

#include "SVObjectAsynchronousCommandTemplate.inl"

#endif

