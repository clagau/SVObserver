//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferTemplate
//* .File Name       : $Workfile:   SVMatroxBufferTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:02:28  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxBufferFacade.h"
#pragma endregion Includes

class SVMatroxBufferTemplate : public SVMatroxBufferFacade
{
public:
	friend class SVMatroxDisplayInterface;

	virtual ~SVMatroxBufferTemplate();

	virtual bool empty() const override;

protected:
	SVMatroxBufferTemplate();
	SVMatroxBufferTemplate( SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName );

	virtual SVMatroxIdentifier GetIdentifier() const override;

	SVMatroxIdentifier m_StartIdentifier;
	SVMatroxIdentifier m_Identifier;
	SVString m_CreatorName;

};

