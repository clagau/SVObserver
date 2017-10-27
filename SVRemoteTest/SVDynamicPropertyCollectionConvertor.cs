//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVDynamicPropertyCollectionConvertor.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:14  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
//using System.Drawing.Design;
using System.Globalization;

namespace SeidenaderVision
{
	#region SVDynamicPropertyCollectionConverter class definition
	/// <summary>
	/// Class for creating Expandable Items.
	/// </summary>
	public class SVDynamicPropertyCollectionConverter : ExpandableObjectConverter
	{
		public override bool CanConvertTo(ITypeDescriptorContext context, System.Type destinationType)
		{
			if (destinationType == typeof(SVDynamicPropertyCollectionConverter))
				return true;

			return base.CanConvertTo(context, destinationType);
		}

		public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, System.Type destinationType)
		{
			if (destinationType == typeof(System.String) && value is SVDynamicPropertyCollection)
			{
				//SVSmartPropertyCollectionConverter so = (SVSmartPropertyCollectionConverter)value;

				return "Expand to see options for this item";
			}
			return base.ConvertTo(context, culture, value, destinationType);
		}
		/* optional
		public override bool CanConvertFrom(ITypeDescriptorContext context, System.Type sourceType)
		{
			if (sourceType == typeof(string))
				return true;

			return base.CanConvertFrom(context, sourceType);
		}

		// for editing base object items
		public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
		{
			if (value is string)
			{
				try
				{
					string s = (string)value;
					int colon = s.IndexOf(':');
					int comma = s.IndexOf(',');

					if (colon != -1 && comma != -1)
					{
						string checkWhileTyping = s.Substring(colon + 1,
														(comma - colon - 1));

						colon = s.IndexOf(':', comma + 1);
						comma = s.IndexOf(',', comma + 1);

						string checkCaps = s.Substring(colon + 1,
														(comma - colon - 1));

						colon = s.IndexOf(':', comma + 1);

						string suggCorr = s.Substring(colon + 1);

						SpellingOptions so = new SpellingOptions();

						so.SpellCheckWhileTyping = Boolean.Parse(checkWhileTyping);
						so.SpellCheckCAPS = Boolean.Parse(checkCaps);
						so.SuggestCorrections = Boolean.Parse(suggCorr);

						return so;
					}
				}
				catch
				{
					throw new ArgumentException(
						"Can not convert '" + (string)value +
										   "' to type SpellingOptions");
				}
			}
			return base.ConvertFrom(context, culture, value);
		}
		*/
	}
	#endregion
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVDynamicPropertyCollectionConvertor.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:14   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

