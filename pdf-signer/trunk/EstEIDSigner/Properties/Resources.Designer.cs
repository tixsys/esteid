﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.4200
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
        ///   Looks up a localized string similar to Allkirjastamine katkestati.
        /// </summary>
        internal static string ACTION_CANCELED {
            get {
                return ResourceManager.GetString("ACTION_CANCELED", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjastan dokumenti ....
        /// </summary>
        internal static string ADD_SIGNATURE {
            get {
                return ResourceManager.GetString("ADD_SIGNATURE", resourceCulture);
            }
        }
        
        internal static System.Drawing.Bitmap background {
            get {
                object obj = ResourceManager.GetObject("background", resourceCulture);
                return ((System.Drawing.Bitmap)(obj));
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
        ///   Looks up a localized string similar to Valitud sertifikaat ja Id-kaardil paiknev sertifikaat ei ole identsed.
        /// </summary>
        internal static string CERT_DONT_MATCH {
            get {
                return ResourceManager.GetString("CERT_DONT_MATCH", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjaandja sertifikaat puudulik: väljaandja väli puudub.
        /// </summary>
        internal static string CERT_ISSUER_MISSING {
            get {
                return ResourceManager.GetString("CERT_ISSUER_MISSING", resourceCulture);
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
        ///   Looks up a localized string similar to ID-kaardilt ei leitud vajalikke sertifikaate.
        /// </summary>
        internal static string CERTS_MISSING {
            get {
                return ResourceManager.GetString("CERTS_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Ühendan kaardilugejaga ....
        /// </summary>
        internal static string CONNECTING_SMARTCARD {
            get {
                return ResourceManager.GetString("CONNECTING_SMARTCARD", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to &lt;?xml version=&quot;1.0&quot; encoding=&quot;utf-8&quot;?&gt;
        ///&lt;configuration&gt;
        /// &lt;appSettings&gt;
        ///  &lt;add key=&quot;debug&quot; value=&quot;false&quot; /&gt;
        ///  &lt;add key=&quot;enable_tsa&quot; value=&quot;False&quot; /&gt;
        ///  &lt;add key=&quot;visible_signature&quot; value=&quot;True&quot; /&gt;
        ///  &lt;add key=&quot;tsa_url&quot; value=&quot;http://www.edelweb.fr/cgi-bin/service-tsp&quot; /&gt;
        ///  &lt;add key=&quot;tsa_user&quot; value=&quot;&quot; /&gt;
        ///  &lt;add key=&quot;tsa_password&quot; value=&quot;&quot; /&gt;
        ///  &lt;add key=&quot;help_url&quot; value=&quot;http://www.esteid.ee/&quot; /&gt;
        ///  &lt;add key=&quot;language&quot; value=&quot;&quot; /&gt;
        ///  &lt;add key=&quot;signature_page&quot; value=&quot;1&quot; /&gt;
        ///  &lt;add key=&quot;signature_use_sect [rest of string was truncated]&quot;;.
        /// </summary>
        internal static string DEFAULT_CONFIG {
            get {
                return ResourceManager.GetString("DEFAULT_CONFIG", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sertifikaatide lugemine ebaõnnestus kataloogist: .
        /// </summary>
        internal static string DIR_CERT_ERROR {
            get {
                return ResourceManager.GetString("DIR_CERT_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Dokumendi allkirja kontroll ebaõnnestus, allkirja andja: .
        /// </summary>
        internal static string DOC_VERIFY_FAILED {
            get {
                return ResourceManager.GetString("DOC_VERIFY_FAILED", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirja kontroll ebaõnnestus.
        /// </summary>
        internal static string DOC_VERIFY_FAILURE {
            get {
                return ResourceManager.GetString("DOC_VERIFY_FAILURE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Dokument allkirjastatud!.
        /// </summary>
        internal static string DOCUMENT_SIGNED {
            get {
                return ResourceManager.GetString("DOCUMENT_SIGNED", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Viga.
        /// </summary>
        internal static string ERROR {
            get {
                return ResourceManager.GetString("ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Tekkis viga ....
        /// </summary>
        internal static string GENERAL_EXCEPTION {
            get {
                return ResourceManager.GetString("GENERAL_EXCEPTION", resourceCulture);
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
        ///   Looks up a localized string similar to Vigane kehtivuspäringu vastus.
        /// </summary>
        internal static string INVALID_OCSP_RESPONSE {
            get {
                return ResourceManager.GetString("INVALID_OCSP_RESPONSE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivuspäringu vastus sisaldas rohkem kui 1 tulemit: .
        /// </summary>
        internal static string INVALID_OCSP_RESPONSE_COUNT {
            get {
                return ResourceManager.GetString("INVALID_OCSP_RESPONSE_COUNT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivuspäringu staatus vigane: .
        /// </summary>
        internal static string INVALID_OCSP_STATUS {
            get {
                return ResourceManager.GetString("INVALID_OCSP_STATUS", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivuspäringu ajakontroll ebaõnnestus.
        /// </summary>
        internal static string INVALID_OCSP_TIMESLOT {
            get {
                return ResourceManager.GetString("INVALID_OCSP_TIMESLOT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vigane PKCS#12 juudrepääsutõend: privaatvõti puudub.
        /// </summary>
        internal static string INVALID_PKCS12_CERT_PRI_KEY_MISSING {
            get {
                return ResourceManager.GetString("INVALID_PKCS12_CERT_PRI_KEY_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vigane PKCS#12 juurdepääsutõend: avalik võti puudub.
        /// </summary>
        internal static string INVALID_PKCS12_CERT_PUB_KEY_MISSING {
            get {
                return ResourceManager.GetString("INVALID_PKCS12_CERT_PUB_KEY_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Issuer certificate missing: .
        /// </summary>
        internal static string ISSUER_CERT_MISSING {
            get {
                return ResourceManager.GetString("ISSUER_CERT_MISSING", resourceCulture);
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
        ///   Looks up a localized string similar to Konfiguratsiooni faili ei leitud: .
        /// </summary>
        internal static string MISSING_CONFIG_FILE {
            get {
                return ResourceManager.GetString("MISSING_CONFIG_FILE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjastamiseks on vaja valida olemasolev PDF dokument!.
        /// </summary>
        internal static string MISSING_INPUT {
            get {
                return ResourceManager.GetString("MISSING_INPUT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjastamiseks on vaja valida väljund PDF dokument!.
        /// </summary>
        internal static string MISSING_OUTPUT {
            get {
                return ResourceManager.GetString("MISSING_OUTPUT", resourceCulture);
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
        ///   Looks up a localized string similar to Kehtivussertifikaadi viga!.
        /// </summary>
        internal static string OCSP_CERT_ERROR {
            get {
                return ResourceManager.GetString("OCSP_CERT_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sertifikaat: {0}, kehtivuspäringu vastus: {1}.
        /// </summary>
        internal static string OCSP_ERROR {
            get {
                return ResourceManager.GetString("OCSP_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivuspäringu URL määramata.
        /// </summary>
        internal static string OCSP_URL_MISSING {
            get {
                return ResourceManager.GetString("OCSP_URL_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivuspäringu kontroll ebaõnnestus.
        /// </summary>
        internal static string OCSP_VERIFY_FAILED {
            get {
                return ResourceManager.GetString("OCSP_VERIFY_FAILED", resourceCulture);
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
        ///   Looks up a localized string similar to PKCS#11 avamine ebaõnnestus.
        /// </summary>
        internal static string PKCS11_OPEN {
            get {
                return ResourceManager.GetString("PKCS11_OPEN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Juurdepääsutõendi faili (PKCS#12) ei leitud: .
        /// </summary>
        internal static string PKCS12_FILE_MISSING {
            get {
                return ResourceManager.GetString("PKCS12_FILE_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Juurdepääsutõendi faili nimi määramata!.
        /// </summary>
        internal static string PKCS12_FILENAME_MISSING {
            get {
                return ResourceManager.GetString("PKCS12_FILENAME_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Juurdepääsutõendi muutujate lugemine ebaõnnestus: .
        /// </summary>
        internal static string PKCS12_LOAD_ERROR {
            get {
                return ResourceManager.GetString("PKCS12_LOAD_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Loen sertifikaate ....
        /// </summary>
        internal static string READ_CERTS {
            get {
                return ResourceManager.GetString("READ_CERTS", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Loen seadeid ....
        /// </summary>
        internal static string READ_SETTINGS {
            get {
                return ResourceManager.GetString("READ_SETTINGS", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Eesti ID-kaardi registri seaded määramata.
        /// </summary>
        internal static string REGISTRY_SETTING_MISSING {
            get {
                return ResourceManager.GetString("REGISTRY_SETTING_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Puudub responderi sertifikaat:.
        /// </summary>
        internal static string RESPONDER_CET_MISSING {
            get {
                return ResourceManager.GetString("RESPONDER_CET_MISSING", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjastamine katkestati ....
        /// </summary>
        internal static string SIGNING_CANCELED {
            get {
                return ResourceManager.GetString("SIGNING_CANCELED", resourceCulture);
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
        
        /// <summary>
        ///   Looks up a localized string similar to {0} vastuse lugemine ebaõnnestus.
        /// </summary>
        internal static string TSA_READ_ERROR {
            get {
                return ResourceManager.GetString("TSA_READ_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Teostan ajatempli päringut ....
        /// </summary>
        internal static string TSA_REQUEST {
            get {
                return ResourceManager.GetString("TSA_REQUEST", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to {0} tagastas veakoodi: {1}.
        /// </summary>
        internal static string TSA_URL_ERROR {
            get {
                return ResourceManager.GetString("TSA_URL_ERROR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Autor:.
        /// </summary>
        internal static string UI_AUTHOR {
            get {
                return ResourceManager.GetString("UI_AUTHOR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Väljastaja.
        /// </summary>
        internal static string UI_CERT_ISSUED_BY {
            get {
                return ResourceManager.GetString("UI_CERT_ISSUED_BY", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Väljastatud.
        /// </summary>
        internal static string UI_CERT_ISSUED_TO {
            get {
                return ResourceManager.GetString("UI_CERT_ISSUED_TO", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Kehtivusaeg.
        /// </summary>
        internal static string UI_CERT_VALID_UNTIL {
            get {
                return ResourceManager.GetString("UI_CERT_VALID_UNTIL", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sertifikaadid.
        /// </summary>
        internal static string UI_CERTIFICATES {
            get {
                return ResourceManager.GetString("UI_CERTIFICATES", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vali allkirjastatav PDF dokument.
        /// </summary>
        internal static string UI_CHOOSE_INPUT {
            get {
                return ResourceManager.GetString("UI_CHOOSE_INPUT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Salvesta nimega.
        /// </summary>
        internal static string UI_CHOOSE_OUTPUT {
            get {
                return ResourceManager.GetString("UI_CHOOSE_OUTPUT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Looja:.
        /// </summary>
        internal static string UI_CREATOR {
            get {
                return ResourceManager.GetString("UI_CREATOR", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sisesta {0} sõrmistikult.
        /// </summary>
        internal static string UI_ENTER_PIN_ONTHE_PINPAD {
            get {
                return ResourceManager.GetString("UI_ENTER_PIN_ONTHE_PINPAD", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PIN on kehtetu.
        /// </summary>
        internal static string UI_EXPIRED_PIN {
            get {
                return ResourceManager.GetString("UI_EXPIRED_PIN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF allkirjastaja.
        /// </summary>
        internal static string UI_FORM_NAME {
            get {
                return ResourceManager.GetString("UI_FORM_NAME", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF allkirjastaja.
        /// </summary>
        internal static string UI_FORM_TITLE {
            get {
                return ResourceManager.GetString("UI_FORM_TITLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Abi.
        /// </summary>
        internal static string UI_HELP_LINK {
            get {
                return ResourceManager.GetString("UI_HELP_LINK", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Sisend:.
        /// </summary>
        internal static string UI_INPUT {
            get {
                return ResourceManager.GetString("UI_INPUT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF failid (*.pdf)|*.pdf.
        /// </summary>
        internal static string UI_INPUT_FILEDIALOG_FILTER {
            get {
                return ResourceManager.GetString("UI_INPUT_FILEDIALOG_FILTER", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vali fail.
        /// </summary>
        internal static string UI_INPUT_FILEDIALOG_TITLE {
            get {
                return ResourceManager.GetString("UI_INPUT_FILEDIALOG_TITLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vigane PIN.
        /// </summary>
        internal static string UI_INVALID_PIN {
            get {
                return ResourceManager.GetString("UI_INVALID_PIN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Märksõnad:.
        /// </summary>
        internal static string UI_KEYWORDS {
            get {
                return ResourceManager.GetString("UI_KEYWORDS", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PIN on lukus.
        /// </summary>
        internal static string UI_LOCKED_PIN {
            get {
                return ResourceManager.GetString("UI_LOCKED_PIN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Väljund:.
        /// </summary>
        internal static string UI_OUTPUT {
            get {
                return ResourceManager.GetString("UI_OUTPUT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF failid (*.pdf)|*.pdf.
        /// </summary>
        internal static string UI_OUTPUT_FILEDIALOG_FILTER {
            get {
                return ResourceManager.GetString("UI_OUTPUT_FILEDIALOG_FILTER", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Salvesta PDF File.
        /// </summary>
        internal static string UI_OUTPUT_FILEDIALOG_TITLE {
            get {
                return ResourceManager.GetString("UI_OUTPUT_FILEDIALOG_TITLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF Dokument.
        /// </summary>
        internal static string UI_PDF_DOC {
            get {
                return ResourceManager.GetString("UI_PDF_DOC", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to PDF Metaandmed.
        /// </summary>
        internal static string UI_PDF_METADATA {
            get {
                return ResourceManager.GetString("UI_PDF_METADATA", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vali sertifikaat digitaalallkirja lisamiseks.
        /// </summary>
        internal static string UI_PICK_CERTIFICATE {
            get {
                return ResourceManager.GetString("UI_PICK_CERTIFICATE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Seaded.
        /// </summary>
        internal static string UI_SETTINGS_LINK {
            get {
                return ResourceManager.GetString("UI_SETTINGS_LINK", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkirjasta ID-kaardiga.
        /// </summary>
        internal static string UI_SIGN_BUTTON {
            get {
                return ResourceManager.GetString("UI_SIGN_BUTTON", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Allkiri.
        /// </summary>
        internal static string UI_SIGNATURE {
            get {
                return ResourceManager.GetString("UI_SIGNATURE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Linn.
        /// </summary>
        internal static string UI_SIGNATURE_CITY {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_CITY", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Riik.
        /// </summary>
        internal static string UI_SIGNATURE_COUNTRY {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_COUNTRY", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Maakond.
        /// </summary>
        internal static string UI_SIGNATURE_COUNTY {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_COUNTY", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Indeks.
        /// </summary>
        internal static string UI_SIGNATURE_INDEX {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_INDEX", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Resolutsioon.
        /// </summary>
        internal static string UI_SIGNATURE_RESOLUTION {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_RESOLUTION", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Roll.
        /// </summary>
        internal static string UI_SIGNATURE_ROLE {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_ROLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Soovid nähtavat allkirja.
        /// </summary>
        internal static string UI_SIGNATURE_VISIBLE {
            get {
                return ResourceManager.GetString("UI_SIGNATURE_VISIBLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Tarkvara:.
        /// </summary>
        internal static string UI_SOFTWARE {
            get {
                return ResourceManager.GetString("UI_SOFTWARE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Staatus:.
        /// </summary>
        internal static string UI_STATUS {
            get {
                return ResourceManager.GetString("UI_STATUS", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Teema:.
        /// </summary>
        internal static string UI_SUBJECT {
            get {
                return ResourceManager.GetString("UI_SUBJECT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Pealkiri:.
        /// </summary>
        internal static string UI_TITLE {
            get {
                return ResourceManager.GetString("UI_TITLE", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Korraga saab allkirjastada ainult 1 faili.
        /// </summary>
        internal static string UI_TOO_MANY_FILES {
            get {
                return ResourceManager.GetString("UI_TOO_MANY_FILES", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Vale PIN.
        /// </summary>
        internal static string UI_WRONG_PIN {
            get {
                return ResourceManager.GetString("UI_WRONG_PIN", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Dokumendi kontroll ....
        /// </summary>
        internal static string VERIFYING_DOCUMENT {
            get {
                return ResourceManager.GetString("VERIFYING_DOCUMENT", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Teostan OCSP kehtivuspäringut ....
        /// </summary>
        internal static string VERIFYING_OCSP {
            get {
                return ResourceManager.GetString("VERIFYING_OCSP", resourceCulture);
            }
        }
    }
}
