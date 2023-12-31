//******************************************************************************
//* COPYRIGHT (c) 2011 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

class SVMatroxBufferTemplate
{
public:
	friend class SVMatroxDisplayInterface;

	virtual ~SVMatroxBufferTemplate();

	bool empty() const { return (m_Identifier == 0); };
	__int64 GetIdentifier() const { return m_Identifier; };

protected:
	SVMatroxBufferTemplate();
	SVMatroxBufferTemplate( __int64 p_Identifier, const std::string& p_rCreatorName );

	// Do not implement this method
	SVMatroxBufferTemplate(const SVMatroxBufferTemplate& p_rObject) = delete;

	// Do not implement this method
	const SVMatroxBufferTemplate& operator=(const SVMatroxBufferTemplate& p_rObject) = delete;

	virtual void freeBuffer();

private:
	__int64 m_Identifier;
	std::string m_CreatorName;
};

typedef std::shared_ptr< SVMatroxBufferTemplate > SVMatroxBufferPtr;

