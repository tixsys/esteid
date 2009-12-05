/*
 * Copyright (C) 2009  Manuel Matonin <manuel@smartlink.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */ 

using System;
using System.Collections.Generic;
using System.Resources;
using System.Reflection;
using System.Text;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;

using EstEIDSigner;
using EstEIDSigner.Properties;

namespace EstEIDNative
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CK_VERSION
    {
        public char major;
        public char minor;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CK_TOKEN_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] label;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] manufacturer_id;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public byte[] model;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public byte[] serial_number;
        public uint flags;
        public uint max_session_count;
        public uint session_count;
        public uint max_rw_session_count;
        public uint rw_session_count;
        public uint max_pin_len;
        public uint min_pin_len;
        public uint total_public_memory;
        public uint free_public_memory;
        public uint total_private_memory;
        public uint free_private_memory;
        public CK_VERSION hardware_version;
        public CK_VERSION firmware_version;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public byte[] utc_time;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CK_SLOT_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public byte[] description;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] manufacturer_id;
        public uint flags;
        public CK_VERSION hardware_version;
        public CK_VERSION firmware_version;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct PKCS11_CERT
    {
        public IntPtr certBuffer;
        public uint certBufferLength;
        public IntPtr certLabel;
        public uint certLabelLength;
    }

    internal static class NativeMethods
    {
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr OpenReader(string module);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void CloseReader(IntPtr h);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint GetSlotCount(IntPtr h, int token_present);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint IsMechanismSupported(IntPtr h, uint slot, uint flag);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint InitDigest(IntPtr h, uint algorithm);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint UpdateDigest(IntPtr h, byte[] data_buffer, uint data_length);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint FinalizeDigest(IntPtr h, ref IntPtr digest_buffer, ref uint digest_length);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint Sign(IntPtr h, uint slot, string pwd,
            byte[] digest_buffer, uint digest_length,
            ref IntPtr signature_buffer, ref uint signature_length);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint GetTokenInfo(IntPtr h, uint slot, ref CK_TOKEN_INFO info);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint GetSlotInfo(IntPtr h, uint slot, ref CK_SLOT_INFO info);
        [DllImport("EstEIDReader.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint LocateCertificate(IntPtr h, uint slot, ref PKCS11_CERT cert);
    }

    /* --------------------------------------------------- */

    public class Mechanism
    {
        private uint mechanism;

        public const uint CKF_HW = (1 << 0);
        public const uint CKF_ENCRYPT = (1 << 8);
        public const uint CKF_DECRYPT = (1 << 9);
        public const uint CKF_DIGEST = (1 << 10);
        public const uint CKF_SIGN = (1 << 11);

        public Mechanism(uint mechanism)
        {
            this.mechanism = mechanism;
        }

        public static implicit operator uint(Mechanism mechanism)
        {
            return mechanism.mechanism;
        }
    }

    /* --------------------------------------------------- */

    class Digest
    {
        private IntPtr esteidHandler;

        public enum HashAlgorithm : uint
        {
            SHA1,
            SHA256
        }
        public const uint SHA1_LENGTH = 20;
        public const uint SHA256_LENGTH = 32;

        public Digest(EstEIDReader handler)
        {
            esteidHandler = handler.Handler();
        }

        public uint InitDigest(HashAlgorithm algorithm)
        {
            return (NativeMethods.InitDigest(esteidHandler, (uint)algorithm));
        }

        public uint UpdateDigest(byte[] data_buffer, uint data_length)
        {
            return (NativeMethods.UpdateDigest(esteidHandler, data_buffer, data_length));
        }

        public uint FinalizeDigest(ref IntPtr diggest_buffer, ref uint diggest_length)
        {
            return (NativeMethods.FinalizeDigest(esteidHandler, ref diggest_buffer, ref diggest_length));
        }
    }

    /* --------------------------------------------------- */

    class Pkcs11Utf8Conversion
    {
        private byte[] b;

        public Pkcs11Utf8Conversion(byte[] bytes)
        {
            this.b = bytes;            
        }

        public static implicit operator string(Pkcs11Utf8Conversion conv)
        {
            int len;

            len = conv.b.Length;
            // PKCS#11 strings have #32 ending
            while (len > 0 && conv.b[len - 1] == ' ')
                len--;

            return Encoding.UTF8.GetString(conv.b, 0, len); 
        }
    }    

    /* --------------------------------------------------- */

    class TokenInfo
    {
        private IntPtr esteidHandler;
        private CK_TOKEN_INFO tokenInfo;

        public static uint CKF_TOKEN_PRESENT = (1 << 0);
        public static uint CKF_WRITE_PROTECTED = (1 << 1);
        public static uint CKF_LOGIN_REQUIRED = (1 << 2);
        public static uint CKF_USER_PIN_INITIALIZED = (1 << 3);
        public static uint CKF_RESTORE_KEY_NOT_NEEDED = (1 << 5);
        public static uint CKF_CLOCK_ON_TOKEN = (1 << 6);
        public static uint CKF_PROTECTED_AUTHENTICATION_PATH = (1 << 8);
        public static uint CKF_DUAL_CRYPTO_OPERATIONS = (1 << 9);
        public static uint CKF_TOKEN_INITIALIZED = (1 << 10);
        public static uint CKF_SECONDARY_AUTHENTICATION = (1 << 11);
        public static uint CKF_USER_PIN_COUNT_LOW = (1 << 16);
        public static uint CKF_USER_PIN_FINAL_TRY = (1 << 17);
        public static uint CKF_USER_PIN_LOCKED = (1 << 18);
        public static uint CKF_USER_PIN_TO_BE_CHANGED = (1 << 19);
        public static uint CKF_SO_PIN_COUNT_LOW = (1 << 20);
        public static uint CKF_SO_PIN_FINAL_TRY = (1 << 21);
        public static uint CKF_SO_PIN_LOCKED = (1 << 22);
        public static uint CKF_SO_PIN_TO_BE_CHANGED = (1 << 23);

        public TokenInfo(EstEIDReader handler)
        {
            esteidHandler = handler.Handler();
            tokenInfo = new CK_TOKEN_INFO();
        }

        public uint ReadInfo(uint slot)
        {
            return (NativeMethods.GetTokenInfo(esteidHandler, slot, ref tokenInfo));
        }        

        public string Label
        {
            get { return new Pkcs11Utf8Conversion(tokenInfo.label); }
        }

        public string Model
        {
            get { return new Pkcs11Utf8Conversion(tokenInfo.model); }
        }

        public string Serial
        {
            get { return new Pkcs11Utf8Conversion(tokenInfo.serial_number); }
        }

        public uint MinPin
        {
            get { return tokenInfo.min_pin_len; }
        }

        public uint MaxPin
        {
            get { return tokenInfo.max_pin_len; }
        }

        public string UtcTime
        {
            get { return new Pkcs11Utf8Conversion(tokenInfo.utc_time); }
        }

        public bool LoginRequired
        {
            get { return ((tokenInfo.flags & CKF_LOGIN_REQUIRED) != 0); }
        }

        public bool PinIsSet
        {
            get { return ((tokenInfo.flags & CKF_USER_PIN_INITIALIZED) != 0); }
        }

        public bool PinPadPresent
        {
            get { return ((tokenInfo.flags & CKF_PROTECTED_AUTHENTICATION_PATH) != 0); }
        }

        public bool TokenPresent
        {
            get { return ((tokenInfo.flags & CKF_TOKEN_PRESENT) != 0); }
        }

        public uint Flags
        {
            get { return (tokenInfo.flags); }
        }
    }

    /* --------------------------------------------------- */

    class SlotInfo
    {
        private IntPtr esteidHandler;
        private CK_SLOT_INFO slotInfo;

        public SlotInfo(EstEIDReader handler)
        {
            esteidHandler = handler.Handler();
            slotInfo = new CK_SLOT_INFO();
        }

        public uint ReadInfo(uint slot)
        {
            return (NativeMethods.GetSlotInfo(esteidHandler, slot, ref slotInfo));
        }

        public string Description
        {
            get { return new Pkcs11Utf8Conversion(slotInfo.description); }
        }

        public string Manufacturer
        {
            get { return new Pkcs11Utf8Conversion(slotInfo.manufacturer_id); }
        }

        public bool TokenPresent
        {
            get { return ((slotInfo.flags & TokenInfo.CKF_TOKEN_PRESENT) != 0); }
        }

        public uint Flags
        {
            get { return (slotInfo.flags); }
        }
    }

    /* --------------------------------------------------- */

    class PKCS11Cert
    {
        public PKCS11_CERT pkcs11Cert;

        public PKCS11Cert()
        {
            pkcs11Cert = new PKCS11_CERT();
            pkcs11Cert.certBufferLength = 2048;
            pkcs11Cert.certBuffer = (IntPtr)(new HGlobalSafeHandle((int)pkcs11Cert.certBufferLength));
            pkcs11Cert.certLabelLength = 128;            
            pkcs11Cert.certLabel = (IntPtr)(new HGlobalSafeHandle((int)pkcs11Cert.certLabelLength));            
        }

        public byte[] CertificateToByteArray()
        {
            byte[] certBuffer = new byte[pkcs11Cert.certBufferLength];
            Marshal.Copy(pkcs11Cert.certBuffer, certBuffer, 0, certBuffer.Length);

            return certBuffer;
        }

        public byte[] LabelToByteArray()
        {
            byte[] certLabel = new byte[pkcs11Cert.certLabelLength];
            Marshal.Copy(pkcs11Cert.certLabel, certLabel, 0, certLabel.Length);

            return certLabel;
        }
    }

    /* --------------------------------------------------- */

    class PKCS11Signer
    {
        private uint slot;
        private X509Certificate2 cert;
        private TokenInfo token;

        public PKCS11Signer(uint slot, X509Certificate2 cert, TokenInfo token)
        {
            this.slot = slot;
            this.cert = cert;
            this.token = token;
        }

        public uint Slot
        {
            get { return slot; }
        }

        public X509Certificate2 Cert
        {
            get { return cert; }
        }

        public TokenInfo Token
        {
            get { return token; }
        }
    }

    /* --------------------------------------------------- */

    class HGlobalSafeHandle : SafeHandle
    {
        private int length = 0;

        public HGlobalSafeHandle(IntPtr ptr)
            : base(ptr, true)
        {            
        }
     
        public HGlobalSafeHandle()
            : base(IntPtr.Zero, true)
        {         
        }
        
        public HGlobalSafeHandle(int size) :
            /* Mono has issues with inheritance ??? */
            /* base(Marshal.AllocHGlobal(size), true) */
            base(IntPtr.Zero, true)
        {
            length = size;            
            SetHandle(Marshal.AllocHGlobal(size));
        }
        
        public static implicit operator HGlobalSafeHandle(IntPtr ptr)
        {
            return new HGlobalSafeHandle(ptr);
        }
        
        public static implicit operator IntPtr(HGlobalSafeHandle h)
        {
            return h.handle;
        }
        
        override protected bool ReleaseHandle()
        {
            Marshal.FreeHGlobal(handle);
            return true;
        }

        public override bool IsInvalid
        {
            get { return (handle == IntPtr.Zero); }
        }

        public byte[] ToByteArray()
        {
            byte[] b = new byte[length];
            Marshal.Copy(handle, b, 0, b.Length);

            return b;
        }
    }    

    /* --------------------------------------------------- */

    public class PKCS11Exception
        : Exception
    {        
        public PKCS11Exception(string message)
            : base(message)
        {
        }

        public PKCS11Exception(uint code) 
            : base(new PKCS11Error(code))
        {            
        }

        public PKCS11Exception(string message, Exception exception)
            : base(message, exception)
        {
        }  
    }    

    /* --------------------------------------------------- */

    class EstEIDReader
    {
        private IntPtr esteidHandler;
        public const uint ESTEID_OK = 0;
        public const uint ESTEID_ERR = 1;
        
        /// <summary>Wrapper class that handles communication between PKCS11 driver and this application.</summary>
        /// <remarks>It needs EstEIDReader.dll for its communication.</remarks>
        public EstEIDReader()
        {
            esteidHandler = IntPtr.Zero;
        }

        ~EstEIDReader()
        {
            if (esteidHandler != IntPtr.Zero)
                NativeMethods.CloseReader(esteidHandler);
        }

        public IntPtr Handler()
        {
            return (esteidHandler);
        }

        public bool Open(string module)
        {
            esteidHandler = NativeMethods.OpenReader(module);

            if (esteidHandler != IntPtr.Zero)
                return (true);

            return (false);
        }

        public uint GetSlotCount(int token_present)
        {
            if (esteidHandler != IntPtr.Zero)
                return (NativeMethods.GetSlotCount(esteidHandler, token_present));

            return (0);
        }

        public uint IsMechanismSupported(uint slot, Mechanism mech)
        {
            if (esteidHandler != IntPtr.Zero)
                return (NativeMethods.IsMechanismSupported(esteidHandler, slot, mech));

            return (ESTEID_ERR);
        }

        public uint Sign(uint slot, string pwd,
            byte[] diggest_buffer, uint diggest_length,
            ref IntPtr signature_buffer, ref uint signature_length)
        {
            if (esteidHandler != IntPtr.Zero)
            {
                return (NativeMethods.Sign(esteidHandler, slot, pwd,
                    diggest_buffer, diggest_length,
                    ref signature_buffer, ref signature_length));
            }

            return (ESTEID_ERR);
        }

        public uint LocateCertificate(uint slot, PKCS11Cert cert)
        {
            if (esteidHandler != IntPtr.Zero)
            {
                return (NativeMethods.LocateCertificate(esteidHandler, slot, ref cert.pkcs11Cert));
            }

            return (ESTEID_ERR);
        }
    }
}
