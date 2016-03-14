//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIProgress
//* .File Name       : $Workfile:   SVIProgress.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:46:04  $
//******************************************************************************

#ifndef SVIPROGRESS_H
#define SVIPROGRESS_H

class SVIProgress
{
public:
	SVIProgress() {}
	virtual ~SVIProgress() {}

	virtual HRESULT Start()=0;
	virtual HRESULT UpdateProgress(unsigned long p_Current, unsigned long p_Total) = 0;
	virtual HRESULT UpdateText(LPCTSTR text) = 0;
};

#endif

