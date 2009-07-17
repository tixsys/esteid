﻿/*
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
        internal static extern uint LocateCertificate(IntPtr h, uint slot, ref PKCS11_CERT cert);
    }

    /* --------------------------------------------------- */

    public class Mechanism
    {
        private uint mechanism;

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

        public uint CKF_WRITE_PROTECTED = (1 << 1);
        public uint CKF_LOGIN_REQUIRED = (1 << 2);
        public uint CKF_USER_PIN_INITIALIZED = (1 << 3);
        public uint CKF_RESTORE_KEY_NOT_NEEDED = (1 << 5);
        public uint CKF_CLOCK_ON_TOKEN = (1 << 6);
        public uint CKF_PROTECTED_AUTHENTICATION_PATH = (1 << 8);
        public uint CKF_DUAL_CRYPTO_OPERATIONS = (1 << 9);
        public uint CKF_TOKEN_INITIALIZED = (1 << 10);
        public uint CKF_SECONDARY_AUTHENTICATION = (1 << 11);
        public uint CKF_USER_PIN_COUNT_LOW = (1 << 16);
        public uint CKF_USER_PIN_FINAL_TRY = (1 << 17);
        public uint CKF_USER_PIN_LOCKED = (1 << 18);
        public uint CKF_USER_PIN_TO_BE_CHANGED = (1 << 19);
        public uint CKF_SO_PIN_COUNT_LOW = (1 << 20);
        public uint CKF_SO_PIN_FINAL_TRY = (1 << 21);
        public uint CKF_SO_PIN_LOCKED = (1 << 22);
        public uint CKF_SO_PIN_TO_BE_CHANGED = (1 << 23);

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
        private TokenInfo info;

        public PKCS11Signer(uint slot, X509Certificate2 cert, TokenInfo info)
        {
            this.slot = slot;
            this.cert = cert;
            this.info = info;
        }

        public uint Slot
        {
            get { return slot; }
        }

        public X509Certificate2 Cert
        {
            get { return cert; }
        }

        public TokenInfo Info
        {
            get { return info; }
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

    public class PKCS11Error
    {
        private string error;
        private static readonly Dictionary<uint, string> errors = new Dictionary<uint, string>();

        static PKCS11Error()
        {
            errors[1] = "CANCEL";
            errors[2] = "HOST_MEMORY";
            errors[3] = "SLOT_ID_INVALID";
            errors[5] = "GENERAL_ERROR";
            errors[6] = "FUNCTION_FAILED";            
            errors[7] = "ARGUMENTS_BAD";
            errors[8] = "NO_EVENT";
            errors[9] = "NEED_TO_CREATE_THREADS";
            errors[0xa] = "CANT_LOCK";
            errors[0x10] = "ATTRIBUTE_READ_ONLY";
            errors[0x11] = "ATTRIBUTE_SENSITIVE";
            errors[0x12] = "ATTRIBUTE_TYPE_INVALID";
            errors[0x13] = "ATTRIBUTE_VALUE_INVALID";
            errors[0x20] = "DATA_INVALID";
            errors[0x21] = "DATA_LEN_RANGE";
            errors[0x30] = "DEVICE_ERROR";
            errors[0x31] = "DEVICE_MEMORY";
            errors[0x32] = "DEVICE_REMOVED";
            errors[0x40] = "ENCRYPTED_DATA_INVALID";
            errors[0x41] = "ENCRYPTED_DATA_LEN_RANGE";
            errors[0x50] = "FUNCTION_CANCELED";
            errors[0x51] = "FUNCTION_NOT_PARALLEL";
            errors[0x54] = "FUNCTION_NOT_SUPPORTED";
            errors[0x60] = "KEY_HANDLE_INVALID";
            errors[0x62] = "KEY_SIZE_RANGE";
            errors[0x63] = "KEY_TYPE_INCONSISTENT";
            errors[0x64] = "KEY_NOT_NEEDED";
            errors[0x65] = "KEY_CHANGED";
            errors[0x66] = "KEY_NEEDED";
            errors[0x67] = "KEY_INDIGESTIBLE";
            errors[0x68] = "KEY_FUNCTION_NOT_PERMITTED";
            errors[0x68] = "KEY_FUNCTION_NOT_PERMITTED";
            errors[0x6a] = "KEY_UNEXTRACTABLE";
            errors[0x70] = "MECHANISM_INVALID";
            errors[0x71] = "MECHANISM_PARAM_INVALID";
            errors[0x82] = "OBJECT_HANDLE_INVALID";
            errors[0x90] = "OPERATION_ACTIVE";
            errors[0x91] = "OPERATION_NOT_INITIALIZED";
            errors[0xa0] = "PIN_INCORRECT";
            errors[0xa1] = "PIN_INVALID";
            errors[0xa2] = "PIN_LEN_RANGE";
            errors[0xa3] = "PIN_EXPIRED";
            errors[0xa4] = "PIN_LOCKED";
            errors[0xb0] = "SESSION_CLOSED";
            errors[0xb1] = "SESSION_COUNT";
            errors[0xb3] = "SESSION_HANDLE_INVALID";
            errors[0xb4] = "SESSION_PARALLEL_NOT_SUPPORTED";
            errors[0xb5] = "SESSION_READ_ONLY";
            errors[0xb6] = "SESSION_EXISTS";
            errors[0xb7] = "SESSION_READ_ONLY_EXISTS";
            errors[0xb8] = "SESSION_READ_WRITE_SO_EXISTS";
            errors[0xc0] = "SIGNATURE_INVALID";
            errors[0xc1] = "SIGNATURE_LEN_RANGE";
            errors[0xd0] = "TEMPLATE_INCOMPLETE";
            errors[0xd1] = "TEMPLATE_INCONSISTENT";
            errors[0xe0] = "TOKEN_NOT_PRESENT";
            errors[0xe1] = "TOKEN_NOT_RECOGNIZED";
            errors[0xe2] = "TOKEN_WRITE_PROTECTED";
            errors[0xf0] = "UNWRAPPING_KEY_HANDLE_INVALID";
            errors[0xf1] = "UNWRAPPING_KEY_SIZE_RANGE";
            errors[0xf2] = "UNWRAPPING_KEY_TYPE_INCONSISTENT";
            errors[0x100] = "USER_ALREADY_LOGGED_IN";
            errors[0x101] = "USER_NOT_LOGGED_IN";
            errors[0x102] = "USER_PIN_NOT_INITIALIZED";
            errors[0x103] = "USER_TYPE_INVALID";
            errors[0x104] = "USER_ANOTHER_ALREADY_LOGGED_IN";
            errors[0x105] = "USER_TOO_MANY_TYPES";
            errors[0x110] = "WRAPPED_KEY_INVALID";
            errors[0x112] = "WRAPPED_KEY_LEN_RANGE";
            errors[0x113] = "WRAPPING_KEY_HANDLE_INVALID";
            errors[0x114] = "WRAPPING_KEY_SIZE_RANGE";
            errors[0x115] = "WRAPPING_KEY_TYPE_INCONSISTENT";
            errors[0x120] = "RANDOM_SEED_NOT_SUPPORTED";
            errors[0x121] = "RANDOM_NO_RNG";
            errors[0x130] = "DOMAIN_PARAMS_INVALID";
            errors[0x150] = "BUFFER_TOO_SMALL";
            errors[0x160] = "SAVED_STATE_INVALID";
            errors[0x170] = "INFORMATION_SENSITIVE";
            errors[0x180] = "STATE_UNSAVEABLE";
            errors[0x190] = "CRYPTOKI_NOT_INITIALIZED";
            errors[0x191] = "CRYPTOKI_ALREADY_INITIALIZED";
            errors[0x1a0] = "MUTEX_BAD";
            errors[0x1a1] = "MUTEX_NOT_LOCKED";
            errors[0x200] = "FUNCTION_REJECTED";            
        }

        public PKCS11Error(uint code)
        {
            if (errors.ContainsKey(code))
                error = (string)errors[code];
            else
                error = string.Format("UNKNOWN_ERROR ({0})", code);
        }

        public static implicit operator string(PKCS11Error e)
        {
            ResourceManager resManager = new ResourceManager("EstEIDSigner.Properties.Resources",
                Assembly.GetExecutingAssembly());
            
            return string.Format("{0}: {1}", resManager.GetString("CARD_INTERNAL_ERROR"), e.error);
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

        public bool IsMechanismSupported(uint slot, Mechanism mech)
        {
            if (esteidHandler != IntPtr.Zero)
                return (NativeMethods.IsMechanismSupported(esteidHandler, slot, mech) == 0);

            return (false);
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
