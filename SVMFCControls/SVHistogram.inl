//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogram
//* .File Name       : $Workfile:   SVHistogram.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:08  $
//******************************************************************************

inline bool SVHistogram::SetScale(histogram::scale s)
{
	bool retval = SvUl::SVHistogramBase::SetScale(s);
	if (retval)
		UpdateAnchors(m_client, m_func);
	return retval;
}

