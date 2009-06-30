using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace EstEIDNative
{
    public class StringP
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string str;
    }

    internal static class NativeMethods
    {
        [DllImport("EstEIDReader.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr OpenReader(string module);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern void CloseReader(IntPtr h);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern uint GetSlotCount(IntPtr h, int token_present);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern uint IsMechanismSupported(IntPtr h, uint slot, uint flag);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern uint Sign(IntPtr h, uint slot, string pwd, byte[] data, uint dlength,
            ref IntPtr hdata, ref uint hlength,
            ref IntPtr rdata, ref uint rlength);
    }

    public class Mechanisms
    {
        public const uint CKF_ENCRYPT = (1 << 8);
        public const uint CKF_DECRYPT = (1 << 9);
        public const uint CKF_DIGEST = (1 << 10);
        public const uint CKF_SIGN = (1 << 11);
    }

    class EstEIDReader
    {
        private IntPtr esteidHandler;

        public EstEIDReader()
        {
            esteidHandler = System.IntPtr.Zero;
        }

        public bool Open(string module)
        {            
            esteidHandler = NativeMethods.OpenReader(module);

            if (esteidHandler != null)
                return (true);

            return (false);
        }

        public void Close()
        {
            if (esteidHandler != null)
                NativeMethods.CloseReader(esteidHandler);
        }

        public uint GetSlotCount(int token_present)
        {
            if (esteidHandler != null)
                return NativeMethods.GetSlotCount(esteidHandler, token_present);

            return (0);
        }

        public bool IsMechanismSupported(uint slot, uint flag)
        {
            if (esteidHandler != null)
                return (NativeMethods.IsMechanismSupported(esteidHandler, slot, flag) == 0);

            return (false);
        }

        public uint Sign(uint slot, string pwd, byte[] data, uint dlength,
            ref IntPtr hdata, ref uint hlength,
            ref IntPtr rdata, ref uint rlength)
        {
            if (esteidHandler != null)
            {
                return NativeMethods.Sign(esteidHandler, slot, pwd, data, dlength,
                    ref hdata, ref hlength,
                    ref rdata, ref rlength);
            }

            return (1);
        }
    }
}
