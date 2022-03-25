//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDynamicPropertySpecEventArgs
//* .File Name       : $Workfile:   SVDynamicPropertySpecEventArgs.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:14  $
//******************************************************************************
using System;
using System.ComponentModel;

namespace SeidenaderVision
{

	#region SVDynamicPropertySpecEventArgs class definintion
	/// <summary>
	/// Provides data for the GetValue and SetValue events .
	/// </summary>
	public class SVDynamicPropertySpecEventArgs : EventArgs
	{
		private SVDynamicProperty property;
		private object val;

		/// <summary>
		/// Initializes a new instance of the PropertySpecEventArgs class.
		/// </summary>
		/// <param name="property">The DynamicProperty that represents the property whose
		/// value is being requested or set.</param>
		/// <param name="val">The current value of the property.</param>
		public SVDynamicPropertySpecEventArgs(SVDynamicProperty property, object val)
		{
			this.property = property;
			this.val = val;
		}

		/// <summary>
		/// Gets the DynamicProperty that represents the property whose value is being
		/// requested or set.
		/// </summary>
		public SVDynamicProperty Property
		{
			get { return property; }
		}

		/// <summary>
		/// Gets or sets the current value of the property.
		/// </summary>
		public object Value
		{
			get { return val; }
			set { val = value; }
		}
	}
	#endregion
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVDynamicPropertySpecEventArgs.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:14   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

