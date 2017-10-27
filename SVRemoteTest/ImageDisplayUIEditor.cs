//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// UITypeEditor for Displaying/Loading an Image
//******************************************************************************
using System;
using System.ComponentModel;
using System.Drawing.Design;
using System.Windows.Forms;
using System.Text;

namespace SVRemoteTest
{
    public class ImageDisplayUIEditor : System.Drawing.Design.ImageEditor
    {
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.Modal;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            SVImage img = context.Instance as SVImage;
            string name = img.Name;

            // Strip off Inspections
            const string InspectionsTag = "Inspections.";
            if (name.StartsWith(InspectionsTag))
            {
                name = name.Substring(InspectionsTag.Length);
            }

            if (img is SVInputImage)
            {
                ImageDisplayForm imgForm = new ImageDisplayForm(name, value as System.Drawing.Image, true);
                imgForm.ShowDialog();
                value = imgForm.DisplayedImage;
            }
            else
            {
                if (null != value)
                {
                    ImageDisplayForm imgForm = new ImageDisplayForm(name, value as System.Drawing.Image);
                    imgForm.ShowDialog();
                }
            }
            return value;
        }
    }
}
