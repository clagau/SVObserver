//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlGetItemsView
//* .File Name       : $Workfile:   SVRemoteCtrlGetItemsView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:37:06  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Drawing;
using System.Drawing.Design;
using System.Reflection;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVRemoteCtrlGetItemsView : ISVComObjectView
    {
        #region Member Variables
        private List<String> m_items = new List<String>();
		private SVProductItemsProperties m_returnValue = new SVProductItemsProperties();
        #endregion

        #region Properties
        [Category("Input Parameters")]
		[Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
		[Description("List of Strings for Input")]
		public List<String> Items
		{
			get { return m_items; }
			set { m_items = value; }
		}

		[Category("Results")]
		[DisplayName("Return Value")]
		[Description("Result of operation")]
		[TypeConverter(typeof(SVProductItemsTypeConverter))]
		[ReadOnly(true)]
		public SVProductItemsProperties ReturnValue
		{
			get { return m_returnValue; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
			items.Add(m_items.ToArray());
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
			// Copy from Com to .Net Collection
			SVRemoteControlLib.ISVProductItems items = value as SVRemoteControlLib.ISVProductItems;

			m_returnValue = new SVProductItemsProperties();

			List<SVValue> valueList = new List<SVValue>();

			foreach (SVRemoteControlLib.ISVValueObject val in items.Values)
			{
				SVValue valueObj = new SVValue();
				valueObj.Name = val.Name;
				valueObj.TriggerCount = val.TriggerCount;
				valueObj.Status = val.Status;

				foreach (object o in val)
				{
					valueObj.Array.Add(o.ToString());
				}
				valueList.Add(valueObj);
			}
			m_returnValue.Values = valueList;

            List<SVImage> imageList = new List<SVImage>();

            foreach (SVRemoteControlLib.ISVImageObject img in items.Images)
            {
                SVImage imageObj = new SVImage();
                imageObj.Name = img.Name;
                imageObj.TriggerCount = img.TriggerCount;
                imageObj.Status = img.Status;

                //@Todo[MEC]  This is faster with BMP
                object o = img.GetImage(false, 1.0, SVRemoteControlLib.SVImageFormatsEnum.PNG);
				if (null != o)
				{
	                byte[] b = o as byte[];
	                imageObj.Picture = SVPictureConvertor.GetPictureFromByteArray(b, System.Drawing.Imaging.ImageFormat.Png);
	                imageList.Add(imageObj);
				}
            }
            m_returnValue.Images = imageList;

            List<SVError> errorList = new List<SVError>();

            foreach (SVRemoteControlLib.ISVErrorObject err in items.Errors)
            {
                SVError errorObj = new SVError();
                errorObj.Name = err.Name;
                errorObj.Status = err.Status;

                errorList.Add(errorObj);
            }
            m_returnValue.Errors = errorList;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlGetItemsView.cs_v  $
 * 
 *    Rev 1.1   14 May 2013 17:37:06   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:38   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

