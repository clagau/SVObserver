#pragma once
#include "svmatroxbuffertemplate.h"
class MatroxSharedBuffer:public SVMatroxBufferTemplate
{
public:
	MatroxSharedBuffer(void);
	virtual ~MatroxSharedBuffer(void);
	friend class SVMatroxBufferInterface;
protected:
	MatroxSharedBuffer( SVMatroxIdentifier Identifier, const std::string& rCreatorName );

};

