﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.3074
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace EstEIDSigner.Properties {
    using System;
    
    
    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    // This class was auto-generated by the StronglyTypedResourceBuilder
    // class via a tool like ResGen or Visual Studio.
    // To add or remove a member, edit your .ResX file then rerun ResGen
    // with the /str option, or rebuild your VS project.
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "2.0.0.0")]
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
    [global::System.Runtime.CompilerServices.CompilerGeneratedAttribute()]
    internal class Resources {
        
        private static global::System.Resources.ResourceManager resourceMan;
        
        private static global::System.Globalization.CultureInfo resourceCulture;
        
        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal Resources() {
        }
        
        /// <summary>
        ///   Returns the cached ResourceManager instance used by this class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Resources.ResourceManager ResourceManager {
            get {
                if (object.ReferenceEquals(resourceMan, null)) {
                    global::System.Resources.ResourceManager temp = new global::System.Resources.ResourceManager("EstEIDSigner.Properties.Resources", typeof(Resources).Assembly);
                    resourceMan = temp;
                }
                return resourceMan;
            }
        }
        
        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture {
            get {
                return resourceCulture;
            }
            set {
                resourceCulture = value;
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Viga krüptoräsi sulgemisel.
        /// </summary>
        internal static string CARD_HASH_FINALIZE {
            get {
                return ResourceManager.GetString("CARD_HASH_FINALIZE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Viga krüptoräsi initsialiseerimisel.
        /// </summary>
        internal static string CARD_HASH_INIT {
            get {
                return ResourceManager.GetString("CARD_HASH_INIT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Viga krüptoräsi uuendamisel.
        /// </summary>
        internal static string CARD_HASH_UPDATE {
            get {
                return ResourceManager.GetString("CARD_HASH_UPDATE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Viga ID-kaardiga suhtlemisel .
        /// </summary>
        internal static string CARD_INTERNAL_ERROR {
            get {
                return ResourceManager.GetString("CARD_INTERNAL_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to ID-kaarti ei leitud.
        /// </summary>
        internal static string CARD_MISSING {
            get {
                return ResourceManager.GetString("CARD_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to GetTokenInfo {0} ebaõnnestus.
        /// </summary>
        internal static string CARD_TOKEN_INFO {
            get {
                return ResourceManager.GetString("CARD_TOKEN_INFO", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Puudub sertifikaat.
        /// </summary>
        internal static string CERT_MISSING {
            get {
                return ResourceManager.GetString("CERT_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sertifikaadi lugemine ebaõnnestus.
        /// </summary>
        internal static string CERT_READ_FAILURE {
            get {
                return ResourceManager.GetString("CERT_READ_FAILURE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Valitud sertifikaati ei saa kasutada allkirjastamiseks.
        /// </summary>
        internal static string INVALID_CERT {
            get {
                return ResourceManager.GetString("INVALID_CERT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Mälupuhver on täis.
        /// </summary>
        internal static string MEMORY_ERROR {
            get {
                return ResourceManager.GetString("MEMORY_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PIN-kood on tühi.
        /// </summary>
        internal static string NO_PIN_SUPPLIED {
            get {
                return ResourceManager.GetString("NO_PIN_SUPPLIED", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Mälu on otsas.
        /// </summary>
        internal static string OUT_OF_MEMORY {
            get {
                return ResourceManager.GetString("OUT_OF_MEMORY", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to opensc-pkcs11 avamine ebaõnnestus.
        /// </summary>
        internal static string PKCS11_OPEN {
            get {
                return ResourceManager.GetString("PKCS11_OPEN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Ajatempli viga: .
        /// </summary>
        internal static string TSA_ERROR {
            get {
                return ResourceManager.GetString("TSA_ERROR", resourceCulture);
            }
        }
    }
}
