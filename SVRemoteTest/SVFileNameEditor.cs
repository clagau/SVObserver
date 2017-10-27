//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameEditor
//* .File Name       : $Workfile:   SVFileNameEditor.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:10  $
//******************************************************************************
using System;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Design;
using System.Windows.Forms.Design;

// FileName Editor class
namespace SeidenaderVision.Gui
{
	public class SVFileNameEditor : System.Drawing.Design.UITypeEditor
	{
		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
		{
			if (context != null)
				return UITypeEditorEditStyle.Modal;

			return UITypeEditorEditStyle.None;
		}

		[RefreshProperties(RefreshProperties.All)]

		public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
		{
			FileDialog fileDlg;

			if (context == null || provider == null || context.Instance == null)
				return base.EditValue(provider, value);

			fileDlg = new System.Windows.Forms.OpenFileDialog();
			fileDlg.Title = "Select Filename";
			fileDlg.FileName = (value as string);
			fileDlg.Filter = "All Files (any)|*.*";
			fileDlg.CheckFileExists = false;
			fileDlg.CheckPathExists = false;

			if (fileDlg.ShowDialog() == DialogResult.OK)
				value = fileDlg.FileName;

			return value;
		}
	}

	/// This class represents a filename, as well as all of the options for the
	/// OpenFileDialog that is opened by the FileTypeConverter.
	[EditorAttribute(typeof(SVFileTypeConverter), typeof(System.Drawing.Design.UITypeEditor))]
	public class SVFileType : ICloneable
	{
		// the most important thing is the string itself, this is what is shown
		public string FileName = "";
		public override string ToString() { return FileName; }

		// This function sets all of the Properties
		public virtual void InitializeDialog(OpenFileDialog openFileDialog)
		{
			if (openFileDialog == null)
				throw new System.ArgumentNullException("openFileDialog");

			openFileDialog.AddExtension = AddExtension;
			openFileDialog.CheckFileExists = CheckFileExists;
			openFileDialog.CheckPathExists = CheckPathExists;
			openFileDialog.DefaultExt = DefaultExt;
			openFileDialog.DereferenceLinks = DereferenceLinks;
			openFileDialog.Filter = Filter;
			openFileDialog.FilterIndex = FilterIndex;
			openFileDialog.InitialDirectory = InitialDirectory;
			openFileDialog.Multiselect = Multiselect;
			openFileDialog.ReadOnlyChecked = ReadOnlyChecked;
			openFileDialog.RestoreDirectory = RestoreDirectory;
			openFileDialog.ShowHelp = ShowHelp;
			openFileDialog.ShowReadOnly = ShowReadOnly;
			openFileDialog.Title = Title;
			openFileDialog.ValidateNames = ValidateNames;
		}

		// These Properties Match the public properties of OpenFileDialog and defaults
		public bool AddExtension = true;
		public bool CheckFileExists = true;
		public bool CheckPathExists = true;
		public string DefaultExt = "";
		public bool DereferenceLinks = true;
		public string Filter = "All files (*.*)|*.*";
		public int FilterIndex = 1;
		public string InitialDirectory = "";
		public bool Multiselect = false;
		public bool ReadOnlyChecked = false;
		public bool RestoreDirectory = false;
		public bool ShowHelp = false;
		public bool ShowReadOnly = false;
		public string Title = "";
		public bool ValidateNames = true;

		#region ICloneable Members
		public object Clone()
		{
			return this.MemberwiseClone();
		}
		#endregion
	}

	/// This type converter opens up a FileOpen dialog box with the options provided by the FileType
	public class SVFileTypeConverter : System.Windows.Forms.Design.FileNameEditor
	{
		private SVFileType m_fileType = null;

		//! Grab the FileType from the value temporarily and let the base class do the rest
		public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
		{
			m_fileType = value as SVFileType;
			if (m_fileType != null)
				value = m_fileType.FileName;

			value = base.EditValue(context, provider, value);
			
			if (m_fileType != null)
			{
				if (m_fileType.FileName != value.ToString())
				{
					m_fileType = (SVFileType)m_fileType.Clone();
					m_fileType.FileName = value.ToString();
				}
				value = m_fileType;
				m_fileType = null;
			}
			return value;
		}

		//! If I have a m_fileType, let it initialize the OpenFileDialog
		protected override void InitializeDialog(OpenFileDialog openFileDialog)
		{
			if (m_fileType != null)
				m_fileType.InitializeDialog(openFileDialog);
			else
				base.InitializeDialog(openFileDialog);
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVFileNameEditor.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:10   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
