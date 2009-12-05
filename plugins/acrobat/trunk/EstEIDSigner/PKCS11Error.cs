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
using System.Text;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    class PKCS11Error
    {
        private string error;
        private static readonly Dictionary<uint, string> errors = new Dictionary<uint, string>();

        public static readonly uint CKR_OK                          = 0;
        public static readonly uint CKR_CANCEL                      = 1;
        public static readonly uint CKR_HOST_MEMORY                 = 2;
        public static readonly uint CKR_SLOT_ID_INVALID	            = 3;
        public static readonly uint CKR_GENERAL_ERROR               = 5;
        public static readonly uint CKR_FUNCTION_FAILED			    = 6;
        public static readonly uint CKR_ARGUMENTS_BAD			    = 7;
        public static readonly uint CKR_NO_EVENT				    = 8;
        public static readonly uint CKR_NEED_TO_CREATE_THREADS		= 9;
        public static readonly uint CKR_CANT_LOCK				    = 0xa;
        public static readonly uint CKR_ATTRIBUTE_READ_ONLY			= 0x10;
        public static readonly uint CKR_ATTRIBUTE_SENSITIVE			= 0x11;
        public static readonly uint CKR_ATTRIBUTE_TYPE_INVALID		= 0x12;
        public static readonly uint CKR_ATTRIBUTE_VALUE_INVALID		= 0x13;
        public static readonly uint CKR_DATA_INVALID			    = 0x20;
        public static readonly uint CKR_DATA_LEN_RANGE			    = 0x21;
        public static readonly uint CKR_DEVICE_ERROR			    = 0x30;
        public static readonly uint CKR_DEVICE_MEMORY			    = 0x31;
        public static readonly uint CKR_DEVICE_REMOVED			    = 0x32;
        public static readonly uint CKR_ENCRYPTED_DATA_INVALID		= 0x40;
        public static readonly uint CKR_ENCRYPTED_DATA_LEN_RANGE	= 0x41;
        public static readonly uint CKR_FUNCTION_CANCELED			= 0x50;
        public static readonly uint CKR_FUNCTION_NOT_PARALLEL		= 0x51;
        public static readonly uint CKR_FUNCTION_NOT_SUPPORTED		= 0x54;
        public static readonly uint CKR_KEY_HANDLE_INVALID			= 0x60;
        public static readonly uint CKR_KEY_SIZE_RANGE			    = 0x62;
        public static readonly uint CKR_KEY_TYPE_INCONSISTENT		= 0x63;
        public static readonly uint CKR_KEY_NOT_NEEDED			    = 0x64;
        public static readonly uint CKR_KEY_CHANGED				    = 0x65;
        public static readonly uint CKR_KEY_NEEDED				    = 0x66;
        public static readonly uint CKR_KEY_INDIGESTIBLE			= 0x67;
        public static readonly uint CKR_KEY_FUNCTION_NOT_PERMITTED	= 0x68;
        public static readonly uint CKR_KEY_NOT_WRAPPABLE			= 0x69;
        public static readonly uint CKR_KEY_UNEXTRACTABLE			= 0x6a;
        public static readonly uint CKR_MECHANISM_INVALID			= 0x70;
        public static readonly uint CKR_MECHANISM_PARAM_INVALID		= 0x71;
        public static readonly uint CKR_OBJECT_HANDLE_INVALID		= 0x82;
        public static readonly uint CKR_OPERATION_ACTIVE			= 0x90;
        public static readonly uint CKR_OPERATION_NOT_INITIALIZED	= 0x91;
        public static readonly uint CKR_PIN_INCORRECT			    = 0xa0;
        public static readonly uint CKR_PIN_INVALID				    = 0xa1;
        public static readonly uint CKR_PIN_LEN_RANGE			    = 0xa2;
        public static readonly uint CKR_PIN_EXPIRED				    = 0xa3;
        public static readonly uint CKR_PIN_LOCKED				    = 0xa4;
        public static readonly uint CKR_SESSION_CLOSED			    = 0xb0;
        public static readonly uint CKR_SESSION_COUNT			    = 0xb1;
        public static readonly uint CKR_SESSION_HANDLE_INVALID		= 0xb3;
        public static readonly uint CKR_SESSION_PARALLEL_NOT_SUPPORTED = 0xb4;
        public static readonly uint CKR_SESSION_READ_ONLY			= 0xb5;
        public static readonly uint CKR_SESSION_EXISTS			    = 0xb6;
        public static readonly uint CKR_SESSION_READ_ONLY_EXISTS	= 0xb7;
        public static readonly uint CKR_SESSION_READ_WRITE_SO_EXISTS = 0xb8;
        public static readonly uint CKR_SIGNATURE_INVALID			= 0xc0;
        public static readonly uint CKR_SIGNATURE_LEN_RANGE			= 0xc1;
        public static readonly uint CKR_TEMPLATE_INCOMPLETE			= 0xd0;
        public static readonly uint CKR_TEMPLATE_INCONSISTENT		= 0xd1;
        public static readonly uint CKR_TOKEN_NOT_PRESENT			= 0xe0;
        public static readonly uint CKR_TOKEN_NOT_RECOGNIZED		= 0xe1;
        public static readonly uint CKR_TOKEN_WRITE_PROTECTED		= 0xe2;
        public static readonly uint CKR_UNWRAPPING_KEY_HANDLE_INVALID = 0xf0;
        public static readonly uint CKR_UNWRAPPING_KEY_SIZE_RANGE	= 0xf1;
        public static readonly uint CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT = 0xf2;
        public static readonly uint CKR_USER_ALREADY_LOGGED_IN		= 0x100;
        public static readonly uint CKR_USER_NOT_LOGGED_IN			= 0x101;
        public static readonly uint CKR_USER_PIN_NOT_INITIALIZED	= 0x102;
        public static readonly uint CKR_USER_TYPE_INVALID			= 0x103;
        public static readonly uint CKR_USER_ANOTHER_ALREADY_LOGGED_IN = 0x104;
        public static readonly uint CKR_USER_TOO_MANY_TYPES			= 0x105;
        public static readonly uint CKR_WRAPPED_KEY_INVALID			= 0x110;
        public static readonly uint CKR_WRAPPED_KEY_LEN_RANGE		= 0x112;
        public static readonly uint CKR_WRAPPING_KEY_HANDLE_INVALID	= 0x113;
        public static readonly uint CKR_WRAPPING_KEY_SIZE_RANGE		= 0x114;
        public static readonly uint CKR_WRAPPING_KEY_TYPE_INCONSISTENT = 0x115;
        public static readonly uint CKR_RANDOM_SEED_NOT_SUPPORTED	= 0x120;
        public static readonly uint CKR_RANDOM_NO_RNG			    = 0x121;
        public static readonly uint CKR_DOMAIN_PARAMS_INVALID		= 0x130;
        public static readonly uint CKR_BUFFER_TOO_SMALL			= 0x150;
        public static readonly uint CKR_SAVED_STATE_INVALID			= 0x160;
        public static readonly uint CKR_INFORMATION_SENSITIVE		= 0x170;
        public static readonly uint CKR_STATE_UNSAVEABLE			= 0x180;
        public static readonly uint CKR_CRYPTOKI_NOT_INITIALIZED	= 0x190;
        public static readonly uint CKR_CRYPTOKI_ALREADY_INITIALIZED = 0x191;
        public static readonly uint CKR_MUTEX_BAD				    = 0x1a0;
        public static readonly uint CKR_MUTEX_NOT_LOCKED			= 0x1a1;
        public static readonly uint CKR_FUNCTION_REJECTED			= 0x200;

        static PKCS11Error()
        {
            errors[CKR_CANCEL] = "CANCEL";
            errors[CKR_HOST_MEMORY] = "HOST_MEMORY";
            errors[CKR_SLOT_ID_INVALID] = "SLOT_ID_INVALID";
            errors[CKR_GENERAL_ERROR] = "GENERAL_ERROR";
            errors[CKR_FUNCTION_FAILED] = "FUNCTION_FAILED";
            errors[CKR_ARGUMENTS_BAD] = "ARGUMENTS_BAD";
            errors[CKR_NO_EVENT] = "NO_EVENT";
            errors[CKR_NEED_TO_CREATE_THREADS] = "NEED_TO_CREATE_THREADS";
            errors[CKR_CANT_LOCK] = "CANT_LOCK";
            errors[CKR_ATTRIBUTE_READ_ONLY] = "ATTRIBUTE_READ_ONLY";
            errors[CKR_ATTRIBUTE_SENSITIVE] = "ATTRIBUTE_SENSITIVE";
            errors[CKR_ATTRIBUTE_TYPE_INVALID] = "ATTRIBUTE_TYPE_INVALID";
            errors[CKR_ATTRIBUTE_VALUE_INVALID] = "ATTRIBUTE_VALUE_INVALID";
            errors[CKR_DATA_INVALID] = "DATA_INVALID";
            errors[CKR_DATA_LEN_RANGE] = "DATA_LEN_RANGE";
            errors[CKR_DEVICE_ERROR] = "DEVICE_ERROR";
            errors[CKR_DEVICE_MEMORY] = "DEVICE_MEMORY";
            errors[CKR_DEVICE_REMOVED] = "DEVICE_REMOVED";
            errors[CKR_ENCRYPTED_DATA_INVALID] = "ENCRYPTED_DATA_INVALID";
            errors[CKR_ENCRYPTED_DATA_LEN_RANGE] = "ENCRYPTED_DATA_LEN_RANGE";
            errors[CKR_FUNCTION_CANCELED] = Resources.SIGNING_CANCELED;
            errors[CKR_FUNCTION_NOT_PARALLEL] = "FUNCTION_NOT_PARALLEL";
            errors[CKR_FUNCTION_NOT_SUPPORTED] = "FUNCTION_NOT_SUPPORTED";
            errors[CKR_KEY_HANDLE_INVALID] = "KEY_HANDLE_INVALID";
            errors[CKR_KEY_SIZE_RANGE] = "KEY_SIZE_RANGE";
            errors[CKR_KEY_TYPE_INCONSISTENT] = "KEY_TYPE_INCONSISTENT";
            errors[CKR_KEY_NOT_NEEDED] = "KEY_NOT_NEEDED";
            errors[CKR_KEY_CHANGED] = "KEY_CHANGED";
            errors[CKR_KEY_NEEDED] = "KEY_NEEDED";
            errors[CKR_KEY_INDIGESTIBLE] = "KEY_INDIGESTIBLE";
            errors[CKR_KEY_FUNCTION_NOT_PERMITTED] = "KEY_FUNCTION_NOT_PERMITTED";
            errors[CKR_KEY_FUNCTION_NOT_PERMITTED] = "KEY_FUNCTION_NOT_PERMITTED";
            errors[CKR_KEY_UNEXTRACTABLE] = "KEY_UNEXTRACTABLE";
            errors[CKR_MECHANISM_INVALID] = "MECHANISM_INVALID";
            errors[CKR_MECHANISM_PARAM_INVALID] = "MECHANISM_PARAM_INVALID";
            errors[CKR_OBJECT_HANDLE_INVALID] = "OBJECT_HANDLE_INVALID";
            errors[CKR_OPERATION_ACTIVE] = "OPERATION_ACTIVE";
            errors[CKR_OPERATION_NOT_INITIALIZED] = "OPERATION_NOT_INITIALIZED";
            errors[CKR_PIN_INCORRECT] = Resources.UI_WRONG_PIN;
            errors[CKR_PIN_INVALID] = Resources.UI_INVALID_PIN;
            errors[CKR_PIN_LEN_RANGE] = "PIN_LEN_RANGE";
            errors[CKR_PIN_EXPIRED] = Resources.UI_EXPIRED_PIN;
            errors[CKR_PIN_LOCKED] = Resources.UI_LOCKED_PIN;
            errors[CKR_SESSION_CLOSED] = "SESSION_CLOSED";
            errors[CKR_SESSION_COUNT] = "SESSION_COUNT";
            errors[CKR_SESSION_HANDLE_INVALID] = "SESSION_HANDLE_INVALID";
            errors[CKR_SESSION_PARALLEL_NOT_SUPPORTED] = "SESSION_PARALLEL_NOT_SUPPORTED";
            errors[CKR_SESSION_READ_ONLY] = "SESSION_READ_ONLY";
            errors[CKR_SESSION_EXISTS] = "SESSION_EXISTS";
            errors[CKR_SESSION_READ_ONLY_EXISTS] = "SESSION_READ_ONLY_EXISTS";
            errors[CKR_SESSION_READ_WRITE_SO_EXISTS] = "SESSION_READ_WRITE_SO_EXISTS";
            errors[CKR_SIGNATURE_INVALID] = "SIGNATURE_INVALID";
            errors[CKR_SIGNATURE_LEN_RANGE] = "SIGNATURE_LEN_RANGE";
            errors[CKR_TEMPLATE_INCOMPLETE] = "TEMPLATE_INCOMPLETE";
            errors[CKR_TEMPLATE_INCONSISTENT] = "TEMPLATE_INCONSISTENT";
            errors[CKR_TOKEN_NOT_PRESENT] = "TOKEN_NOT_PRESENT";
            errors[CKR_TOKEN_NOT_RECOGNIZED] = "TOKEN_NOT_RECOGNIZED";
            errors[CKR_TOKEN_WRITE_PROTECTED] = "TOKEN_WRITE_PROTECTED";
            errors[CKR_UNWRAPPING_KEY_HANDLE_INVALID] = "UNWRAPPING_KEY_HANDLE_INVALID";
            errors[CKR_UNWRAPPING_KEY_SIZE_RANGE] = "UNWRAPPING_KEY_SIZE_RANGE";
            errors[CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT] = "UNWRAPPING_KEY_TYPE_INCONSISTENT";
            errors[CKR_USER_ALREADY_LOGGED_IN] = "USER_ALREADY_LOGGED_IN";
            errors[CKR_USER_NOT_LOGGED_IN] = "USER_NOT_LOGGED_IN";
            errors[CKR_USER_PIN_NOT_INITIALIZED] = "USER_PIN_NOT_INITIALIZED";
            errors[CKR_USER_TYPE_INVALID] = "USER_TYPE_INVALID";
            errors[CKR_USER_ANOTHER_ALREADY_LOGGED_IN] = "USER_ANOTHER_ALREADY_LOGGED_IN";
            errors[CKR_USER_TOO_MANY_TYPES] = "USER_TOO_MANY_TYPES";
            errors[CKR_WRAPPED_KEY_INVALID] = "WRAPPED_KEY_INVALID";
            errors[CKR_WRAPPED_KEY_LEN_RANGE] = "WRAPPED_KEY_LEN_RANGE";
            errors[CKR_WRAPPING_KEY_HANDLE_INVALID] = "WRAPPING_KEY_HANDLE_INVALID";
            errors[CKR_WRAPPING_KEY_SIZE_RANGE] = "WRAPPING_KEY_SIZE_RANGE";
            errors[CKR_WRAPPING_KEY_TYPE_INCONSISTENT] = "WRAPPING_KEY_TYPE_INCONSISTENT";
            errors[CKR_RANDOM_SEED_NOT_SUPPORTED] = "RANDOM_SEED_NOT_SUPPORTED";
            errors[CKR_RANDOM_NO_RNG] = "RANDOM_NO_RNG";
            errors[CKR_DOMAIN_PARAMS_INVALID] = "DOMAIN_PARAMS_INVALID";
            errors[CKR_BUFFER_TOO_SMALL] = "BUFFER_TOO_SMALL";
            errors[CKR_SAVED_STATE_INVALID] = "SAVED_STATE_INVALID";
            errors[CKR_INFORMATION_SENSITIVE] = "INFORMATION_SENSITIVE";
            errors[CKR_STATE_UNSAVEABLE] = "STATE_UNSAVEABLE";
            errors[CKR_CRYPTOKI_NOT_INITIALIZED] = "CRYPTOKI_NOT_INITIALIZED";
            errors[CKR_CRYPTOKI_ALREADY_INITIALIZED] = "CRYPTOKI_ALREADY_INITIALIZED";
            errors[CKR_MUTEX_BAD] = "MUTEX_BAD";
            errors[CKR_MUTEX_NOT_LOCKED] = "MUTEX_NOT_LOCKED";
            errors[CKR_FUNCTION_REJECTED] = "FUNCTION_REJECTED";
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
            return string.Format("{0}: {1}", Resources.CARD_INTERNAL_ERROR, e.error);
        }
    }
}
