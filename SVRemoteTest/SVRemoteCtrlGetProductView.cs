//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlGetProductView
//* .File Name       : $Workfile:   SVRemoteCtrlGetProductView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   28 Oct 2014 11:54:26  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Drawing;
using System.Drawing.Design;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVRemoteCtrlGetProductView : ISVComObjectView
	{
		#region Member Variables
		private Int32 m_imageScale = 0;
		private Int32 m_triggerCount = -1;
		private SVProductItemsProperties m_returnValue = new SVProductItemsProperties();
		private SVRemoteControlLib.ISVProductItems m_LatestItems = null;
		#endregion

		#region Properties
		[Category("Input Parameters")]
		[Description("Name of Monitor List")]
		public String ListName
		{
			get { return SVRemoteTestHelper.MonitorListName; }
			set { SVRemoteTestHelper.MonitorListName = value; }
		}
		[Category("Input Parameters")]
		[Description("ID of Product to retrieve")]
		public Int32 ProductID
		{
			get { return m_triggerCount; }
			set { m_triggerCount = value; }
		}
		[Category("Input Parameters")]
		[Description("Image Scale")]
		public Int32 ImageScale
		{
			get { return m_imageScale; }
			set { m_imageScale = value; }
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
			items.Add(ListName);
			items.Add(m_triggerCount);
			items.Add(m_imageScale);
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
			// Copy from Com to .Net Collection
			m_LatestItems = value as SVRemoteControlLib.ISVProductItems;

			m_returnValue = new SVProductItemsProperties();

			List<SVValue> valueList = new List<SVValue>();

			foreach (SVRemoteControlLib.ISVValueObject val in m_LatestItems.Values)
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

			bool bGetAllImages = Program.m_MainForm.ShouldAllImagesLoaded();

			foreach (SVRemoteControlLib.ISVImageObject img in m_LatestItems.Images)
			{
				SVImage imageObj = null;

				if (!bGetAllImages)
				{
					imageObj = new SVImage();
					imageObj.Name = img.Name;
					imageObj.TriggerCount = img.TriggerCount;
					imageObj.Status = img.Status;
				}
				else
				{
					imageObj = GetPicture(img.Name);
				}

				imageList.Add(imageObj);
			}
			m_returnValue.Images = imageList;

			List<SVError> errorList = new List<SVError>();

			foreach (SVRemoteControlLib.ISVErrorObject err in m_LatestItems.Errors)
			{
				SVError errorObj = new SVError();
				errorObj.Name = err.Name;
				errorObj.Status = err.Status;

				errorList.Add(errorObj);
			}
			m_returnValue.Errors = errorList;
		}
		#endregion

		#region GetPicture
		/// <summary>
		/// This method may throw an exception.
		/// </summary>
		/// <param name="imageName"></param>
		/// <returns></returns>
		public SVImage GetPicture(string imageName)
		{
			SVImage imageObj = new SVImage();
			imageObj.TriggerCount = -1;
			imageObj.Status = -1;

			try
			{
				foreach (SVRemoteControlLib.ISVImageObject img in m_LatestItems.Images)
				{
					if (img.Name == imageName)
					{
						imageObj.TriggerCount = img.TriggerCount;
						imageObj.Status = img.Status;
						imageObj.Name = img.Name;
		///@Todo[mec] check with bmp				
		object o = img.GetImage(false, 1.0, SVRemoteControlLib.SVImageFormatsEnum.PNG);
						if (null != o)
						{
							byte[] b = o as byte[];
							imageObj.Picture = SVPictureConvertor.GetPictureFromByteArray(b, System.Drawing.Imaging.ImageFormat.Png);
							break;
						}
						else
						{
							System.Diagnostics.Trace.WriteLine("Error getting image:  " + img.Name + "; Trigger Count:  " + img.TriggerCount);
							break;
						}
					}
				}
			}
			catch (Exception e)
			{
				System.Diagnostics.Trace.WriteLine("Trigger Count:  " + imageObj.TriggerCount + "; Exception in GetPicture:  " + e.ToString());
				throw new Exception("Error getting image for trigger count " + imageObj.TriggerCount + ".", e);
			}

			return imageObj;
		}
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlGetProductView.cs_v  $
 * 
 *    Rev 1.5   28 Oct 2014 11:54:26   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised ListName property to use referenec the MonitorList name in SVRemoteTestHelper
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.4   11 Sep 2014 18:12:28   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: sJones; Steve Jones
 * Change Description: Changed GetPicture to set the image name.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   08 Sep 2014 18:30:58   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Added Comment
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   05 Sep 2014 15:20:30   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Added m_LatestItems to store the most recent product so images can be requested later.
 * Changed SetResult method to only fetch all the images at once when the Continual Results tab is not the selected tab.
 * When Continual Results tab is selected, only one image at a time should be fetched.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   08 May 2014 19:29:20   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changed ProductID and ImageSCale from long to Int32.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2014 13:53:40   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial checkin.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
