using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Security.Permissions;

namespace GurcaTesterDotNet
{
    [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]
    public partial class frmGurcaTester : Form
    {
        #region Private Fields

        private readonly string TS_MACADDR_NAME = "TsClientMacAddr";
        private IntPtr ipChannelHandle;
        private GCHandle gchClientCallback;
        private GCHandle gchErrorCallback;

        #endregion

        public delegate void ClientCallbackDelegate(IntPtr callbackXml);
        public delegate void ErrorCallbackDelegate(IntPtr errorXml);

        [DllImport("GurcaHelperLib.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr InitializeVCAndRegisterCallback(string szVCName,
            ClientCallbackDelegate clientCallback, ErrorCallbackDelegate errorCallback);

        [DllImport("GurcaHelperLib.dll", CallingConvention=CallingConvention.Cdecl, CharSet=CharSet.Unicode)]
        private static extern bool SendVCMessage(IntPtr hChannelHandle, string wszMessage,
            ErrorCallbackDelegate errorCallback);

        public frmGurcaTester()
        {
            InitializeComponent();
        }

        private void frmGurcaTester_Load(object sender, EventArgs e)
        {
            webBrowser1.ObjectForScripting = this;

            webBrowser1.DocumentText =
                        "<html><head><script>" +
                        "function test(message) { alert(message); }" +
                        "</script></head><body><button " +
                        "onclick=\"window.external.Test('called from script code')\">" +
                        "call client code from script code</button>" +
                        "</body></html>";
        }

        private void pbInitialize_Click(object sender, EventArgs e)
        {
            ClientCallbackDelegate ccd = new ClientCallbackDelegate(ClientCallback);
            gchClientCallback = GCHandle.Alloc(ccd);

            ErrorCallbackDelegate ecd = new ErrorCallbackDelegate(ErrorCallback);
            gchErrorCallback = GCHandle.Alloc(ecd);

            ipChannelHandle = InitializeVCAndRegisterCallback(TS_MACADDR_NAME, ccd, ecd);
        }

        private void pbSendMessage_Click(object sender, EventArgs e)
        {
            bool bResult = SendVCMessage(ipChannelHandle, "<MacAddrServer><Request RequestType=\"MacAddress\" /></MacAddrServer>",
                new ErrorCallbackDelegate(ErrorCallback));
        }

        #region Callbacks

        private void ClientCallback(IntPtr callbackXml)
        {
            Trace.WriteLine("Hello from ClientCallback!!!");
            Trace.WriteLine(Marshal.PtrToStringUni(callbackXml));
        }

        private void ErrorCallback(IntPtr errorXml)
        {
            Trace.WriteLine("Hello from ErrorCallback!!!");
            Trace.WriteLine(Marshal.PtrToStringUni(errorXml));
        }

        #endregion

    }
}
