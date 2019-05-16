//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOBoardCapabilities
//* .File Name       : $Workfile:   SVIOBoardCapabilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   11 Jun 2013 15:26:18  $
//******************************************************************************

#pragma once

class SVIOBoardCapabilities  
{
public:
	SVIOBoardCapabilities();
	SVIOBoardCapabilities( const SVIOBoardCapabilities& p_rCapable );
	const SVIOBoardCapabilities& operator=( const SVIOBoardCapabilities& p_rCapable );
	virtual ~SVIOBoardCapabilities();

	int GetTriggerInverterCount() const;
	int GetStrobeInverterCount() const;
	int GetTriggerCount() const;
	int GetInputCount() const;
	int GetOutputCount() const;

	void SetInputCount( int p_iInputs );
	void SetOutputCount( int p_iOutputs );
	void SetTriggerCount( int p_iTriggers );
	void SetTriggerInverters( int p_iTrigInverters );
	void SetStrobeInverters( int p_iStrobeInverters );

	bool IsNonIOSVIM() const;
	void SetNonIOSVIM(int numTriggers);

private:
	int m_iTriggerInverters;
	int m_iStrobeInverters;
	int m_iNumTriggers;
	int m_iNumOutputs;
	int m_iNumInputs;
};
