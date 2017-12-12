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
//Moved to precompiled header: #include <memory>
#include "SVMatroxTypedefs.h"
#pragma endregion Includes

class SVMatroxBufferTemplate
{
public:
	friend class SVMatroxDisplayInterface;

	virtual ~SVMatroxBufferTemplate();

	bool empty() const { return (m_Identifier == 0); };
	SVMatroxIdentifier GetIdentifier() const { return m_Identifier; };

protected:
	SVMatroxBufferTemplate();
	SVMatroxBufferTemplate( SVMatroxIdentifier p_Identifier, const std::string& p_rCreatorName );

	// Do not implement this method
	SVMatroxBufferTemplate(const SVMatroxBufferTemplate& p_rObject) = delete;

	// Do not implement this method
	const SVMatroxBufferTemplate& operator=(const SVMatroxBufferTemplate& p_rObject) = delete;

	virtual void freeBuffer();

private:
	SVMatroxIdentifier m_Identifier;
	std::string m_CreatorName;
};

typedef std::shared_ptr< SVMatroxBufferTemplate > SVMatroxBufferPtr;

