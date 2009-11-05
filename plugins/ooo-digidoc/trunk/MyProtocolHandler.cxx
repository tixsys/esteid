/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: MyProtocolHandler.cxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
/************************************************************************
* Modified By Mark Erlich, Smartlink OÜ
* 2009
*************************************************************************/

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include <string.h>
#include "MyBdocBridge.h"
#include "MyDebug.h"

#include "ListenerHelper.h"
#include "MyProtocolHandler.h"

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>
#include <com/sun/star/frame/ControlEvent.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XControlNotificationListener.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/container/XContainerQuery.hpp>
#include <com/sun/star/presentation/XPresentation.hpp>

#include <osl/file.hxx>
#include <osl/thread.h>

#include <cppuhelper/bootstrap.hxx> 
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/script/provider/XScript.hpp> 
#include <com/sun/star/frame/XDesktop.hpp>

#include <com/sun/star/frame/XDispatchHelper.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/util/XCloseable.hpp>

using namespace std;
using namespace rtl;

using namespace com::sun::star::util;
using namespace com::sun::star::lang;
using namespace com::sun::star::script::provider; 

using namespace com::sun::star::uno;
using namespace com::sun::star::frame;
using com::sun::star::lang::XMultiServiceFactory;
using com::sun::star::lang::XMultiComponentFactory;
using com::sun::star::lang::XComponent;
using com::sun::star::beans::PropertyValue;
using com::sun::star::util::URL;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::text::XTextViewCursor;
using com::sun::star::text::XTextCursor;
using com::sun::star::sheet::XSpreadsheetView;
using com::sun::star::sheet::XSpreadsheetDocument;
using com::sun::star::frame::XModel;
using com::sun::star::text::XTextRange;
using com::sun::star::text::XTextDocument;
using com::sun::star::beans::NamedValue;
using namespace com::sun::star::awt;
using com::sun::star::view::XSelectionSupplier;
using namespace com::sun::star::system;
using com::sun::star::presentation::XPresentation;



//--Global Variables--
ListenerHelper aListenerHelper;
OUString ousBDocFileURL;
OUString ousBDocContURL;
bool bContFlag;
bool bPrevContFlag;
Reference < XDesktop > rGlobalDesktop;
Reference < XDispatchHelper > rGlobalDispatchHelper;
const char* pcMessage;
string strSignData;
string strBuffer;
MyBdocBridge * m_BdocBridge;
//--------------------

void BaseDispatch::ShowMessageBox( const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText )
{
	if ( !mxToolkit.is() )
		mxToolkit = Reference< XToolkit > ( mxMSF->createInstance(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                            "com.sun.star.awt.Toolkit" ))), UNO_QUERY );
    if ( rFrame.is() && mxToolkit.is() )
    {
        // describe window properties.
        WindowDescriptor                aDescriptor;
        aDescriptor.Type              = WindowClass_MODALTOP;
        aDescriptor.WindowServiceName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "infobox" ));
        aDescriptor.ParentIndex       = -1;
        aDescriptor.Parent            = Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY );
        aDescriptor.Bounds            = Rectangle(100,100,300,200);
        aDescriptor.WindowAttributes  = WindowAttribute::BORDER | WindowAttribute::MOVEABLE | WindowAttribute::CLOSEABLE;

        Reference< XWindowPeer > xPeer = mxToolkit->createWindow( aDescriptor );
        if ( xPeer.is() )
        {
            Reference< XMessageBox > xMsgBox( xPeer, UNO_QUERY );
            if ( xMsgBox.is() )
            {
                xMsgBox->setCaptionText( aTitle );
                xMsgBox->setMessageText( aMsgText );
                xMsgBox->execute();
            }
        }
    }
}

void BaseDispatch::SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    Reference < XDispatch > xDispatch = 
            aListenerHelper.GetDispatch( mxFrame, aURL.Path );
    
    FeatureStateEvent aEvent;
    
    aEvent.FeatureURL = aURL;
    aEvent.Source     = xDispatch;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;
    
    aEvent.State <<= aCtrlCmd;
    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent ); 
}

void BaseDispatch::SendCommandTo( const Reference< XStatusListener >& xControl, const URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    FeatureStateEvent aEvent;
    
    aEvent.FeatureURL = aURL;
    aEvent.Source     = (::com::sun::star::frame::XDispatch*) this;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;
    
    aEvent.State <<= aCtrlCmd;
    xControl->statusChanged( aEvent );
}

void SAL_CALL MyProtocolHandler::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException)
{

	Reference < XFrame > xFrame;
	if ( aArguments.getLength() )
	{
		// das erste Argument ist immer der Frame, da ein ProtocolHandler den braucht um Zugriff
		// auf den Context zu haben, in dem er aufgerufen wird
		aArguments[0] >>= xFrame;
		mxFrame = xFrame;
	}

	bPrevContFlag = bContFlag;
	bContFlag = false;
	Reference < XController > xCtrl = mxFrame->getController();

	//----------------- Get File component and URL -------------------------
	Reference<XModel> xModel1(xCtrl->getModel(),UNO_QUERY);	
	
	Reference < XDesktop > pDesktop (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);

	Reference< XComponent > xCompMain = pDesktop->getCurrentComponent();

	ousBDocFileURL = xModel1->getURL();
	OString muffik = OUStringToOString(ousBDocFileURL, RTL_TEXTENCODING_ASCII_US);
PRINT_DEBUG ("file opening URL : %s",muffik.pData->buffer);
	string strContainerPath = muffik.pData->buffer;
	//----------------------------------------------------------------------
	//----------------- If Bdoc Container, open it -------------------------
	if (!memcmp(&strContainerPath[strContainerPath.size() - 5], ".bdoc", 5))
	{
		MyBdocBridge * m_BdocBridge1 = MyBdocBridge::getInstance();
		m_BdocBridge1->DigiInit();
/*		
*/		if (!m_BdocBridge1->ret)
		{
			m_BdocBridge1->DigiOpen(convertURItoPath(ousBDocFileURL, 0).pData->buffer);

	PRINT_DEBUG ("TempFile URL : %s",m_BdocBridge1->pRetPath);				
			string strTempFileUrl = UNO_URL_HEAD;
			strTempFileUrl += m_BdocBridge1->pRetPath;
			ousBDocContURL = ousBDocFileURL; //<-----Get access to the container in new frame!
			ousBDocFileURL = ::rtl::OUString(strTempFileUrl.data(),strTempFileUrl.size(), RTL_TEXTENCODING_UNICODE, 0);
			ousBDocFileURL = convertPathToURI(ousBDocFileURL);
			bContFlag = true;
			
			Sequence< PropertyValue > loadProps(1);
			loadProps[0] = PropertyValue();
			loadProps[0].Name = OUString::createFromAscii("ReadOnly");
			loadProps[0].Value <<= sal_True;

			OUString sTarget(RTL_CONSTASCII_USTRINGPARAM("odk_officedev_desk")); 
			OUString sOldName = mxFrame->getName(); 
			
			mxFrame->setName(sTarget); 

		
			// Get access to the global component loader of the office 
			// for synchronous loading of documents. 
			Reference < ::com::sun::star::frame::XComponentLoader > xLoader (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );

			// Load the document into the target frame by using our unambigous name 
			// and special search flags. 
			Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, 8, loadProps));
			mxFrame->setName(sOldName);

			m_BdocBridge1->Terminate();
			// dispose the local service manager
			//Reference< XComponent >::query( xMultiComponentFactoryClient )->dispose();
		}
		else
		{ // We got an error in init
			printf("Error: %s\n",m_BdocBridge1->pcErrMsg);
		}
		Reference <XCloseable> xCloseable( mxFrame, UNO_QUERY);
		xCloseable->close(true);
	}		
	//----------------------------------------------------------------------

	else if (bPrevContFlag) //-if it's a new frame after opening a bdoc container
	{
		Reference <XDesktop> rDesktop(mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);
		rGlobalDesktop = rDesktop;

		Reference< XDispatchHelper > rDispatchHelper = Reference < XDispatchHelper > ( mxMSF->createInstance(OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.DispatchHelper" ))), UNO_QUERY );
		rGlobalDispatchHelper = rDispatchHelper;

		getSignatures(false);
	}	
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
				throw( RuntimeException )
{
	Reference < XDispatch > xRet;
	if ( !mxFrame.is() )
		return 0;

	Reference < XController > xCtrl = mxFrame->getController();

	if (xCtrl.is() && !aURL.Protocol.compareToAscii("vnd.demo.digidoc.addon:") )
	{
		Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
		Reference <XPresentation> xPresentation ( xCtrl, UNO_QUERY );

		if ( aURL.Path.equalsAscii("Command1" ))
		{
			xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
			if ( !xRet.is() )
			{
				if (xPresentation.is())
					xRet = (BaseDispatch*) new PresentationDispatch( mxMSF, mxFrame );
				else
					xRet = /*xCursor.is() ?*/ (BaseDispatch*) new WriterDispatch( mxMSF, mxFrame );
					// :
					//(BaseDispatch*) new CalcDispatch( mxMSF, mxFrame );
				aListenerHelper.AddDispatch( xRet, mxFrame, aURL.Path );
			}
			
		}
		
	}


	return xRet;
	
}

Sequence < Reference< XDispatch > > SAL_CALL MyProtocolHandler::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
			throw( RuntimeException )
{
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}

::rtl::OUString MyProtocolHandler_getImplementationName ()
	throw (RuntimeException)
{
    return ::rtl::OUString::createFromAscii(MYPROTOCOLHANDLER_IMPLEMENTATIONNAME);
}

sal_Bool SAL_CALL MyProtocolHandler_supportsService( const ::rtl::OUString& ServiceName )
	throw (RuntimeException)
{
    return (
            ServiceName.equalsAscii(MYPROTOCOLHANDLER_SERVICENAME  ) ||
            ServiceName.equalsAscii("com.sun.star.frame.ProtocolHandler")
           );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
	throw (RuntimeException)
{
	Sequence < ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString::createFromAscii(MYPROTOCOLHANDLER_SERVICENAME);
    return aRet;
}

#undef SERVICE_NAME

Reference< XInterface > SAL_CALL MyProtocolHandler_createInstance( const Reference< XMultiServiceFactory > & rSMgr)
	throw( Exception )
{
	return (cppu::OWeakObject*) new MyProtocolHandler( rSMgr );
}

// XServiceInfo
::rtl::OUString SAL_CALL MyProtocolHandler::getImplementationName(  )
	throw (RuntimeException)
{
	return MyProtocolHandler_getImplementationName();
}

sal_Bool SAL_CALL MyProtocolHandler::supportsService( const ::rtl::OUString& rServiceName )
	throw (RuntimeException)
{
    return MyProtocolHandler_supportsService( rServiceName );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler::getSupportedServiceNames(  )
	throw (RuntimeException)
{
    return MyProtocolHandler_getSupportedServiceNames();
}

void SAL_CALL BaseDispatch::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs ) throw (RuntimeException)
{
	/* Its neccessary to hold this object alive, till this method finish.
	   May the outside dispatch cache (implemented by the menu/toolbar!)
	   forget this instance during de-/activation of frames (focus!).

		E.g. An open db beamer in combination with the My-Dialog
		can force such strange situation :-(
	 */
	
	Reference< XInterface > xSelfHold(static_cast< XDispatch* >(this), UNO_QUERY);

	if ( !aURL.Protocol.compareToAscii("vnd.demo.digidoc.addon:") )
	{
		if ( !aURL.Path.compareToAscii("Command1" ))
		{
			m_BdocBridge = MyBdocBridge::getInstance();

			Reference < XDesktop > rDesktop (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);

			Reference <XComponent> xComp = rDesktop->getCurrentComponent();

			Sequence <Any> outparam;
			Sequence <sal_Int16> indexes;
			OUString pParam;
			OString muff, ostrPath, ostrParam, ostrPin;
			int i_try = 1;
						
			Reference < XScriptProviderSupplier > xScriptPS(xComp, UNO_QUERY);
			Reference < XScriptProvider > xScriptProvider(xScriptPS->getScriptProvider(), UNO_QUERY);
			
			Reference < XModel> xMyModel (xComp, UNO_QUERY);
			
			ostrPath = convertURItoPath(xMyModel->getURL(), 0);

			//for bdoc container
			muff = OUStringToOString(ousLocBdocContUrl, RTL_TEXTENCODING_ASCII_US);
			string strBdocUrl;
			strBdocUrl = muff.pData->buffer;
			bool bPathIs = memcmp(ostrPath.pData->buffer, "", 1);
			//----------------------If We are dealing with BDoc container----------------------
			if (!memcmp(&strBdocUrl[strBdocUrl.size() - 5], ".bdoc", 5) && iLocPrevContFlag && bPathIs && i_try)
			{	
				getSignatures(true);
			}

			//---------------------if file has not been saved----------------------------------
			
			if (!bPathIs)
			{
				i_try = 0;
				::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Salvesamata Fail!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Palun salvestage fail!" )));
			}
			
			//---------------------check if card is in reader----------------------------------
			else 
			{				
				m_BdocBridge->DigiInit();
				if (m_BdocBridge->ret)
				{
					::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Digidoc Error!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( m_BdocBridge->pcErrMsg )));
					i_try = 0;
				}
				else
				{
					m_BdocBridge->DigiCheckCert();
					if (!m_BdocBridge->ret)
					{
						::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Kasutatav Sertifikaat!" )), convertPathToURI(::rtl::OUString::createFromAscii( m_BdocBridge->pSerialNr )));
					}
					else if (m_BdocBridge->ret == 1)
					{ //NO card or cardreader
						Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.NoCard?language=Basic&location=application") ), UNO_QUERY);
						xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
						i_try = 0;
					}
					else if(m_BdocBridge->ret == 2)
					{ //Invalid certificate						
						::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Sertifikaadi viga!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Teie sertifikaat on aegunud!\nPalun kasutage ID kaardi haldusvahendit,\net uuendata Teie allkirjastamise sertifikaati." )));
						i_try = 0;
					}
				}
			}
			
			//----------------------If We are dealing with BDoc container----------------------
			if (!memcmp(&strBdocUrl[strBdocUrl.size() - 5], ".bdoc", 5) && iLocPrevContFlag && bPathIs && i_try)
			{	
				m_BdocBridge->Terminate();
			}
			//---------------New File will be signed -> View a Warning------------------
			else if (bPathIs && i_try)
			{
				//check if container allready exists!
				string strTmp = ostrPath.pData->buffer;
				strTmp =+ ".bdoc";
				FILE* fp = fopen(strTmp.c_str(), "r");
				if (fp) 
				    fclose(fp);
				else
				{
					//message about existing container
					Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.ContErrorFunc?language=Basic&location=application") ), UNO_QUERY);
					xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
				
					//--If Cacel button--
					if (!memcmp(muff.pData->buffer, "*", 1))
						i_try = 0;
				}

				if (i_try)
				{
					strBdocUrl = "";		
					Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Init?language=Basic&location=application") ), UNO_QUERY);
					xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
				
					//--If Cacel button--
					if (!memcmp(muff.pData->buffer, "*", 1))
						i_try = 0;
				}
			}
			
					
			//----------------------Start the signing procedure---------------------------
			while (i_try)
			{
				Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Signing?language=Basic&location=application") ), UNO_QUERY);
			
				xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
				ostrParam = OUStringToOString(pParam, RTL_TEXTENCODING_UTF8);

				//--If sign button--
				if (memcmp(ostrParam.pData->buffer, "*", 1))
				{
					if (strBdocUrl.size()>1)
					{	//if its an open bdoc container			 			
						m_BdocBridge->DigiInit();
						m_BdocBridge->DigiOpen(convertURItoPath(ousLocBdocContUrl, 0).pData->buffer);
					}
					else
						m_BdocBridge->DigiInit();

					m_BdocBridge->iPinReq = 0;					
					//Sign
					m_BdocBridge->pPath = ostrPath.pData->buffer;
					m_BdocBridge->pParam = ostrParam.pData->buffer;

					oslWorkerFunction pFunc = (void (SAL_CALL *)(void*)) threadSign;
					oslThread hThreadShowSign = osl_createThread(pFunc,(void*) NULL);

					//Wait for PIN2 Request
					while(1)
					{
//PRINT_DEBUG("*** Value: %d", m_BdocBridge->iPinReq);
						if (m_BdocBridge->iPinReq == 100)
						{	//if we have regular card reader
							Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.PIN?language=Basic&location=application") ), UNO_QUERY);
							xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
							ostrPin = OUStringToOString(pParam, RTL_TEXTENCODING_UTF8);

							//--If OK button--
							if (memcmp(ostrPin.pData->buffer, "*", 1))
							{
								int c;
								for (c=0; c<ostrPin.pData->length; c++)
									m_BdocBridge->c_Pin[c] = ostrPin.pData->buffer[c];
								m_BdocBridge->c_Pin[c] = NULL;
								m_BdocBridge->iPinReq = 0;
							}
							else	//Cancel Signing process
								m_BdocBridge->iPinReq = 1;
					
							break;								
						}	
						else if (m_BdocBridge->iPinReq == 200)
						{	//if we have a cardreader with Pinpad	
							printf("PINPAD \n");
							::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PinPad" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Palun kasutage kaardilugeja klaviatuuri PIN2 sisestamiseks!" )));
							m_BdocBridge->iPinReq = 0;
							break;
						}
						else if (m_BdocBridge->iPinReq == 10)
							break;	//if signing procedure has been canceled for some error
					}
					//Wait until signing procedure ends
					//while (!m_BdocBridge->iPinReq); <-- while loop in this condition doesnt work
					for (int e=0; e<1; e++)
					{
						if (m_BdocBridge->iPinReq <= 1)
							e--;
					}

					if (!m_BdocBridge->ret)
					{ //If signing was successfull
						Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Success?language=Basic&location=application") ), UNO_QUERY);
						xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
						//--------------------------------------------------------------
						//---------------If New file --> Open bdoc container------------
						if(!strBdocUrl.size())
						{
							string strNewPath = ostrPath.pData->buffer;
							strNewPath += ".bdoc";
							m_BdocBridge->DigiInit();
							m_BdocBridge->DigiOpen(strNewPath.c_str());

//	PRINT_DEBUG ("TempFile URL : %s",m_BdocBridge->pRetPath);	
							string strTempFileUrl = UNO_URL_HEAD;
							strTempFileUrl += m_BdocBridge->pRetPath;
							strNewPath = UNO_URL_HEAD + strNewPath;
//	PRINT_DEBUG ("NewTempFile URL: %s\n",strTempFileUrl.c_str());
//	PRINT_DEBUG ("NewContFile URL: %s\n",strTempFileUrl.c_str());
							ousBDocContURL = ::rtl::OUString(strNewPath.data(),strNewPath.size(), RTL_TEXTENCODING_UNICODE, 0);; //<-----Get access to the container in new frame!
							ousBDocFileURL = ::rtl::OUString(strTempFileUrl.data(),strTempFileUrl.size(), RTL_TEXTENCODING_UNICODE, 0);

							ousBDocContURL = convertPathToURI(ousBDocContURL);
							ousBDocFileURL = convertPathToURI(ousBDocFileURL);

							bContFlag = true;
							bPrevContFlag = true;

							Sequence< PropertyValue > loadProps(1);
							loadProps[0] = PropertyValue();
							loadProps[0].Name = OUString::createFromAscii("ReadOnly");
							loadProps[0].Value <<= sal_True;

							OUString sTarget(RTL_CONSTASCII_USTRINGPARAM("odk_officedev_desk")); 
							OUString sOldName = mxFrame->getName(); 
							
							mxFrame->setName(sTarget); 

						
							// Get access to the global component loader of the office 
							// for synchronous loading of documents. 
							Reference < ::com::sun::star::frame::XComponentLoader > xLoader (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );

							// Load the document into the target frame by using our unambigous name 
							// and special search flags. 
							Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, 8, loadProps));
							mxFrame->setName(sOldName);

							xComp->dispose();
						}
						//----------------------------------------------------------------------
						else
							getSignatures(false);
						i_try = 0;
					}
					
					else if (m_BdocBridge->ret == 1)
					{ //NO card or cardreader
						Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.NoCard?language=Basic&location=application") ), UNO_QUERY);
						xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					}

					else if (m_BdocBridge->ret == 10)
					{ //OCSP Error
					/*	//Wrong PIN2 <-- use this when error codes from digidoc library are working
						Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Failure?language=Basic&location=application") ), UNO_QUERY);
						xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
						muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
						
						if (!memcmp(muff.pData->buffer, "*", 1))
						{ //IF Canceled
							i_try = 0;
						}
					*/								
						for (int ic=0; ic<m_BdocBridge->iCounter; ic++)
						{	// Show all errors								
							::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Exception!" )), convertPathToURI(::rtl::OUString::createFromAscii( m_BdocBridge->eMessages[ic].pcEMsg )));
							PRINT_DEBUG("Got Exception - %s", m_BdocBridge->eMessages[ic].pcEMsg);
						}
					}
					else if (m_BdocBridge->ret == 100)
					{	
						::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Korduv Allkiri!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Kuvatav fail on juba Teie poolt allkirjastatud!" )));
						i_try = 0;
					}

					if ( (!m_BdocBridge->ret) || (strBdocUrl.size()>1) )
						m_BdocBridge->Terminate();

				}
				else
					i_try = 0;

			}
		}		
	}
}


void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
	if ( aURL.Protocol.equalsAscii("vnd.demo.digidoc.addon:") )
	{
		if ( aURL.Path.equalsAscii("Command1" ) )
		{
				// just enable this command
			::com::sun::star::frame::FeatureStateEvent aEvent;
			aEvent.FeatureURL = aURL;
			aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
			aEvent.IsEnabled = mbButtonEnabled;
			aEvent.Requery = sal_False;
			aEvent.State <<= Any();
			xControl->statusChanged( aEvent );
		}
		aListenerHelper.AddListener( mxFrame, xControl, aURL.Path );
	}
	
}

void SAL_CALL BaseDispatch::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
	aListenerHelper.RemoveListener( mxFrame, xControl, aURL.Path );
	
printf("BaseDispatch::removeStatusListener\n");
OString osBDocFilePath = OUStringToOString(ousBDocFileURL, RTL_TEXTENCODING_ASCII_US);
printf("URL in removeStatusListener: %s\n",osBDocFilePath.pData->buffer);

}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event ) throw (RuntimeException)
{
	if ( Event.aURL.Protocol.equalsAscii("vnd.demo.digidoc.addon:" ));
}
 
BaseDispatch::BaseDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame, const ::rtl::OUString& rServiceName )
        : mxMSF( rxMSF )
		, mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )//, mbButtonEnabled( sal_False )
{	
	ousLocBdocContUrl = ousBDocContURL;
	iLocPrevContFlag = bPrevContFlag;
}


BaseDispatch::~BaseDispatch()
{
	mxFrame.clear();
	mxMSF.clear();
}


void SAL_CALL threadCallMacro(char *pData)
{
	Reference < XDesktop > rLocDesktop = rGlobalDesktop;
	Reference < XDispatchHelper > rLocalDispatchHelper = rGlobalDispatchHelper;
PRINT_DEBUG("In the MacroCallerThread: %s",pData);
	//----------------------Open Macro--------------------------------
	Reference< XDispatchProvider > rDispatchProvider(rLocDesktop,UNO_QUERY);
	/*Any any = */rLocalDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii(pData), OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
//PRINT_DEBUG("Return From Macro: %d",any);	
	//----------------------------------------------------------------
}

//OString SAL_CALL ::BaseDispatch::convertURItoPath(OUString ousURI)
OString SAL_CALL convertURItoPath(OUString ousURI, bool bToOO)
{
	OString osPath;
	strBuffer = "";
	wchar_t utfChar;
	int iCD1, iCD2;

	osPath = OUStringToOString(ousURI, RTL_TEXTENCODING_ISO_8859_15);

	for(int i=sizeof(UNO_URL_HEAD)-1; i<osPath.getLength(); i++)
	{
		if (!memcmp(&osPath.pData->buffer[i], "%20", 3))
		{
			strBuffer += " ";
			i += 2;
		}

		else if (osPath.pData->buffer[i] == '%')
		{	//Some Special character -> change to utf8
			iCD1 = convHexAsciiToInt(osPath.pData->buffer[i+1],osPath.pData->buffer[i+2]);
			i += 3; 
			iCD2 = convHexAsciiToInt(osPath.pData->buffer[i+1],osPath.pData->buffer[i+2]);
			i += 2; 
			
			if (bToOO)
			{	// if string goes to OO
				utfChar = ((iCD1 - 192) * 64) + (iCD2 - 128);
				strBuffer += (char)utfChar;
			}
			else
			{
				strBuffer += (char)iCD1;
				strBuffer += (char)iCD2;
			}
		}
		else if ((osPath.pData->buffer[i] > 127) && (bToOO))
		{	// if its a utf8 and string goes to OO
			utfChar = ((osPath.pData->buffer[i] - 192) * 64) + (osPath.pData->buffer[i+1] - 128);
			strBuffer += (char)utfChar;
			i ++;
		}
		else
			strBuffer += osPath.pData->buffer[i];
	}
PRINT_DEBUG("Return URL: %s",strBuffer.c_str());
	return OString(strBuffer.c_str());
}

OUString SAL_CALL convertPathToURI(OUString ousPath)
{
	wchar_t utfChar;
	strBuffer = "";

	for(int i=0; i<ousPath.getLength(); i++)
	{
		if (ousPath.pData->buffer[i] > 127)
		{
			utfChar = ((ousPath.pData->buffer[i] - 192) * 64) + (ousPath.pData->buffer[i+1] - 128);
			strBuffer += (char)utfChar;

			i ++;
		}
		else
			strBuffer += ousPath.pData->buffer[i];
	}

	return OUString(strBuffer.data(), strBuffer.size(), RTL_TEXTENCODING_UNICODE, 0);
}

int getTimeZoneDiff()
{
	tm *gmtTime;
	time_t locTime;
	double  dif;

	time(&locTime);
	gmtTime = gmtime ( &locTime );
	dif = difftime (locTime, mktime(gmtTime));

	return int(dif / 3600);
}

int convHexAsciiToInt(char cA, char cB)
{
	int num;
	switch (cA)
	{
	case 'A':
		num = 10;
		break;
	case 'B':
		num = 11;
		break;
	case 'C':
		num = 12;
		break;
	case 'D':
		num = 13;
		break;
	case 'E':
		num = 14;
		break;
	case 'F':
		num = 15;
		break;
	default:
		num = (int)cA - 48;		
	}
	num *= 16; 
	switch (cB)
	{
	case 'A':
		num += 10;
		break;
	case 'B':
		num += 11;
		break;
	case 'C':
		num += 12;
		break;
	case 'D':
		num += 13;
		break;
	case 'E':
		num += 14;
		break;
	case 'F':
		num += 15;
		break;
	default:
		num += (int)cB - 48;
	}
	return num;
}

//void BaseDispatch::threadSign(MyBdocBridge * th_BdocBridge)
void SAL_CALL threadSign()
{
	m_BdocBridge->DigiSign();
	m_BdocBridge->iPinReq = 10;
}

int getSignatures(bool bButton)
{
	MyBdocBridge * m_BdocBridge1 = MyBdocBridge::getInstance();
	m_BdocBridge1->DigiInit();
	m_BdocBridge1->DigiOpen(convertURItoPath(ousBDocContURL, 0).pData->buffer);

	m_BdocBridge1->Terminate();

PRINT_DEBUG("Return from FileOpen: %d", m_BdocBridge1->ret);
	if (m_BdocBridge1->ret < 100)// && (m_BdocBridge1->ret < m_BdocBridge1->iSignCnt))//check for possible errors
	{		
		//------------Fix sign data string from returned data---------		
		strSignData = "";
		int k,l,m,n,o,p,q,r, iHour;
		
		k=l=m=n=o=p=q=r=0;
		for (int cnt=0; cnt<m_BdocBridge1->iSignCnt; cnt++)
		{	
			//Signer Name
			while (m_BdocBridge1->pSignName[k] != '#')
			{
				if ((m_BdocBridge1->pSignName[k] == '\\') && (m_BdocBridge1->pSignName[k+1] == ','))
				{
					strSignData += ";";
					k++;
				}
				else if ((m_BdocBridge1->pSignName[k] == '\\') && (m_BdocBridge1->pSignName[k+1] != ','))
				{	//convert utf-8 ascii string to unicode char
					int iCD1, iCD2;
					iCD1 = convHexAsciiToInt(m_BdocBridge1->pSignName[k+1],m_BdocBridge1->pSignName[k+2]);
					k += 3; 
					iCD2 = convHexAsciiToInt(m_BdocBridge1->pSignName[k+1],m_BdocBridge1->pSignName[k+2]);
					k += 2; 
					strSignData += (char)iCD1;
					strSignData += (char)iCD2;
				}
				else
					strSignData += m_BdocBridge1->pSignName[k];
				k++;
			}
			k++;
			strSignData += ";";

PRINT_DEBUG("name string: %s", m_BdocBridge1->pSignName);

			//Signer Role
			if (sizeof(m_BdocBridge1->pSignRole) > m_BdocBridge1->iSignCnt)
			{
				//strSignData += "Roll: ";
				while (m_BdocBridge1->pSignRole[l] != '#')
				{
					strSignData += m_BdocBridge1->pSignRole[l];
					l++;
				}
				l++;
			}
			strSignData += ";";

			//Signer AddRole
			if (sizeof(m_BdocBridge1->pSignAddRole) > m_BdocBridge1->iSignCnt)
			{
				//strSignData += "Resolutsioon: ";
				while (m_BdocBridge1->pSignAddRole[m] != '#')
				{
					strSignData += m_BdocBridge1->pSignAddRole[m];
					m++;
				}
				m++;
			}
			strSignData += ";";

			//Signing Time
			while (m_BdocBridge1->pSignTime[n] != '#')
			{
				if (m_BdocBridge1->pSignTime[n] == 'T')
				{
					stringstream out;
					strSignData += " ";
					iHour = 10*((int)m_BdocBridge1->pSignTime[++n] - 48);
					iHour += (int)m_BdocBridge1->pSignTime[++n] - 48;
					iHour += getTimeZoneDiff();						
					out << iHour;
					strSignData += out.str();
				}
				else if (m_BdocBridge1->pSignTime[n] != 'Z')
					strSignData += m_BdocBridge1->pSignTime[n];
										
				n++;
			}
			n++;
			strSignData += ";";
		
			//Signing City
			if (sizeof(m_BdocBridge1->pSignCity) > m_BdocBridge1->iSignCnt)
			{
				//strSignData += "Linn: ";
				while (m_BdocBridge1->pSignCity[o] != '#')
				{
					strSignData += m_BdocBridge1->pSignCity[o];
					o++;
				}
				o++;
			}
			strSignData += ";";

			//Signing Region
			while (m_BdocBridge1->pSignState[p] != '#')
			{
				strSignData += m_BdocBridge1->pSignState[p];
				p++;
			}
			p++;
			strSignData += ";";

			//Signing Postal nr
			while (m_BdocBridge1->pSignPostal[q] != '#')
			{
				strSignData += m_BdocBridge1->pSignPostal[q];
				q++;
			}
			q++;
			strSignData += ";";

			//Signing Postal nr
			while (m_BdocBridge1->pSignCountry[r] != '#')
			{
				strSignData += m_BdocBridge1->pSignCountry[r];
				r++;
			}
			r++;
			strSignData += ";";

			strSignData += "_____________________;";

		}
		if (!bButton)
		{
			strSignData = "macro:///HW.HW.GetCert(*" + strSignData + ")";
			//-------------------Open Macro in new thread---------------------
			//oslWorkerFunction type : void (SAL_CALL *oslWorkerFunction)(void*); in osl/thread.h
			oslWorkerFunction pFunc1 = (void (SAL_CALL *)(void*)) threadCallMacro;
			// create and start the hThreadShowSign with pcSignData as a parameter value		
			oslThread hThreadShowSign = osl_createThread(pFunc1,(void *) strSignData.c_str());
			//----------------------------------------------------------------
		}
		else
		{
			strSignData = "macro:///HW.HW.GetCert(#" + strSignData + ")";
			
			//-------------Open Signature Viewer Macro------------------------
			Reference< XDispatchProvider > rDispatchProvider(rGlobalDesktop,UNO_QUERY);
			rGlobalDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii(strSignData.data()), 
				OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
			//----------------------------------------------------------------
		}
		
		
/*****************************************
//some preparations for using statusbar
		if (m_BdocBridge1->ret)	//validation failed
		{
		//Change Statusbar
			pcMessage = "macro:///HW.HW.StatusBarCtrl(KEHTETU ALLKIRI!  -- Fail mida soovite avada sisaldab kehtetuid või aegunud allkirju!)";
		}

		else//Change Statusbar			
			pcMessage = "macro:///HW.HW.StatusBarCtrl(ALLKIRJASTATUD Fail!  -- faili muutmine blokeeritud!)";

	//----------------------------------------------------------------
	}
	else if (m_BdocBridge1->ret >= 10000)
	{
		//Change Statusbar
		pcMessage = "macro:///HW.HW.StatusBarCtrl(ALLKIRJASTATUD Fail!  -- faili muutmine blokeeritud! -- Allkirja lugemisel tekkis viga!)";
	}
	
	//oslWorkerFunction pFunc2 = (void (SAL_CALL *)(void*)) threadCallMacro;
	//oslThread hThreadChangeStatusBar = osl_createThread(pFunc2,(void *) pcMessage);
****************************************/
		return 0;
	}
	else
		return 1;
}
