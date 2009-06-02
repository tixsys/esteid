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
#include <stdio.h>
#include <iostream.h>
#include "MyBdocBridge.h"
//#include "MyOOoParams.h"

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

#include <osl/file.hxx>
#include <cppuhelper/bootstrap.hxx> 
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/script/provider/XScript.hpp> 
#include <com/sun/star/frame/XDesktop.hpp>

#include <com/sun/star/frame/XDispatchHelper.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>


using namespace std;
using namespace rtl;

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


ListenerHelper aListenerHelper;

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

}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(	const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
				throw( RuntimeException )
{
	Reference < XDispatch > xRet;
	if ( !mxFrame.is() )
		return 0;

	Reference < XController > xCtrl = mxFrame->getController();
	if ( xCtrl.is() && !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
		//Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
		//if ( !xCursor.is() && !xView.is() )
			// ohne ein entsprechendes Dokument funktioniert der Handler nicht
		//	return xRet;

		if ( aURL.Path.equalsAscii("Command1" ))
		{
			xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
			if ( !xRet.is() )
			{
				xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxMSF, mxFrame ) :
					//(BaseDispatch*) new PresentationDispatch( mxMSF, mxFrame ) :
					(BaseDispatch*) new CalcDispatch( mxMSF, mxFrame );
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

	if ( !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		if ( !aURL.Path.compareToAscii("Command1" ))
		{
			MyBdocBridge * m_BdocBridge = MyBdocBridge::getInstance();
			//MyOOoParams::MyOOoParams mOOoParams;	
/*			m_BdocBridge->teemingilollus1();
			string str_targetPath = "/home/mark/Desktop/Juhan.bdoc";
			string str_inputPath = "/home/mark/Desktop/Juhan.txt";
			m_BdocBridge->DigiInit();
			m_BdocBridge->DigiSign();	
*/

//====================================================================================================
//-------------See jura käivitab olemasoleva factory kontrolleriga macro, kuid ei saa returni---------
//----------------------------------------------------------------------------------------------------
/*			 
			Reference < XDesktop > rDesktop (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);
 
			Reference< XDispatchHelper > rDispatchHelper = Reference < XDispatchHelper > ( mxMSF->createInstance(OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.DispatchHelper" ))), UNO_QUERY );

			Reference< XDispatchProvider > rDispatchProvider(rDesktop,UNO_QUERY);
 
			Any any=rDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii("macro:///Standard.HW.Munnid()"), OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());

			printf("any: %d\n",(int)any.pData);
*/			

//====================================================================================================
//----------------See jura loob uue componendi ja laeb avatud dokumendi sinna sisse-------------------
//----------------------------------------------------------------------------------------------------
			Reference < XDesktop > rDesktop (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);

			Reference <XComponent> xComp = rDesktop->getCurrentComponent();
//====================================================================================================
//----------------See jura loob uue componendi ja sellega hävitab ka vana dokumendi akna--------------
/*/----------------------------------------------------------------------------------------------------
			// Conditions: sURL = "private:factory/swriter" 
			// xSMGR = m_xServiceManager 
			// xFrame = reference to a frame
			// lProperties[] = new com.sun.star.beans.PropertyValue[0]
			OUString sURL(RTL_CONSTASCII_USTRINGPARAM("private:factory/swriter"));
			//OUString sURL(RTL_CONSTASCII_USTRINGPARAM("file:///home/mark/Desktop/Juhan.txt"));
			OUString sOldName = mxFrame->getName(); 
			OUString sTarget(RTL_CONSTASCII_USTRINGPARAM("odk_officedev_desk")); 
			mxFrame->setName(sTarget); 
			
			// Get access to the global component loader of the office 
			// for synchronous loading of documents. 
			Reference < ::com::sun::star::frame::XComponentLoader > xLoader (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );

			// Load the document into the target frame by using our unambigous name 
			// and special search flags. 
			Reference <XComponent> xComp = xLoader->loadComponentFromURL( sURL, sTarget, /*::com::sun::star::frame::FrameSearchFlag->CHILDREN*///4, lArgs); 


//====================================================================================================
//----------------See jura käivitab macro / scripti ning saab tagastatava return väärtuse-------------
//----------------------------------------------------------------------------------------------------
			Sequence <Any> outparam;
			Sequence <sal_Int16> indexes;
			OUString pParam;
			OString muff, ostrPath, ostrParam, ostrPin;
			int i_try = 1;
						
			Reference < XScriptProviderSupplier > xScriptPS(xComp, UNO_QUERY);
			Reference < XScriptProvider > xScriptProvider(xScriptPS->getScriptProvider(), UNO_QUERY);
			
			Reference < XModel> xMyModel (xComp, UNO_QUERY);
			pParam = xMyModel->getURL();
			muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
			printf("URL : %s\n",muff.pData->buffer);	
			//check which OOo service app is running
			//if ()
			//{						
				//Reference < XTextDocument> xDoc(xComp, UNO_QUERY);

			ostrPath = OUStringToOString(xMyModel->getURL(), RTL_TEXTENCODING_UTF8);

			//if file has not been saved
			if (!memcmp(ostrPath.pData->buffer, "", 1))
			{
				i_try = 0;
				::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Salvesamata Fail!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Palun salvestage fail!" )));
			}
			
			else
			{
				Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Init?language=Basic&location=application") ), UNO_QUERY);
				xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
				muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
								
//printf("macrost1: %s\n",muff.pData->buffer);			
				//--If Cacel button--
				if (!memcmp(muff.pData->buffer, "*", 1))
					i_try = 0;
			}
			
					

			while (i_try)
			{
				Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Signing?language=Basic&location=application") ), UNO_QUERY);
			
				xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
				ostrParam = OUStringToOString(pParam, RTL_TEXTENCODING_UTF8);

				//--If sign button--
				if (memcmp(ostrParam.pData->buffer, "*", 1))
				{
//printf("macrost1: %s\n",ostrParam.pData->buffer);
					Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.PIN?language=Basic&location=application") ), UNO_QUERY);
					xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					ostrPin = OUStringToOString(pParam, RTL_TEXTENCODING_UTF8);
			
					//--If OK button--
					if (memcmp(ostrPin.pData->buffer, "*", 1))
					{	//sign file --if not success retry					
						m_BdocBridge->DigiInit();						
						m_BdocBridge->DigiSign((char*)ostrPath.pData->buffer, (char*)ostrParam.pData->buffer, (char*)ostrPin.pData->buffer);	

						//printf("RETURN OLI: %d\n", m_BdocBridge->ret);
						if (!m_BdocBridge->ret)
						{ //If signing was successfull
							Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Success?language=Basic&location=application") ), UNO_QUERY);
							xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
							i_try = 0;
						}
						
						else if (m_BdocBridge->ret == 1)
						{ //NO card or cardreader
							Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.NoCard?language=Basic&location=application") ), UNO_QUERY);
							xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
						}

						else if (m_BdocBridge->ret == 10)
						{ //Wrong PIN2
							Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Failure?language=Basic&location=application") ), UNO_QUERY);
							xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
							muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
							
							if (!memcmp(muff.pData->buffer, "*", 1))
							{ //IF Canceled
								i_try = 0;
							}
						}
					}
				}
				else
					i_try = 0;

			}
//printf("macrost2: %s\n",muff.pData->buffer);

			// insert the returned string into the opened text document..
/*			Reference < ::com::sun::star::text::XTextDocument > xDoc(xComp, UNO_QUERY);
			Reference < ::com::sun::star::text::XText > xText(xDoc->getText(), UNO_QUERY);
			xText->setString(pname);
		
			Reference<XStorable> storeable(xComp, UNO_QUERY);

			if (storeable->hasLocation()) 
			{
				OString loc = OUStringToOString(storeable->getLocation(), RTL_TEXTENCODING_ASCII_US);

				printf("loc: %s\n",loc.pData->buffer);
			}
		
*/			// dont forget to restore old frame name ... 
			//mxFrame->setName(sOldName);
		}
		
	}
}


void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
	if ( aURL.Protocol.equalsAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
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
}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event ) throw (RuntimeException)
{
	if ( Event.aURL.Protocol.equalsAscii("vnd.demo.complextoolbarcontrols.demoaddon:" ))
	{
/***		if ( Event.aURL.Path.equalsAscii( "Command2" ))
		{
		    // We get notifications whenever the text inside the combobox has been changed.
		    // We store the new text into a member.
		    if ( Event.Event.equalsAscii( "TextChanged" ))
		    {
		        rtl::OUString aNewText;
		        sal_Bool      bHasText( sal_False );
		        for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
		        {
		            if ( Event.aInformation[i].Name.equalsAsciiL( "Text", 4 ))
		            {
		                bHasText = Event.aInformation[i].Value >>= aNewText;
		                break;
		            }
		        }
		        
		        if ( bHasText )
		            maComboBoxText = aNewText;
		    }
		}
***/	}
}
 
BaseDispatch::BaseDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame, const ::rtl::OUString& rServiceName )
        : mxMSF( rxMSF )
		, mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )//, mbButtonEnabled( sal_False )
{	

/*	
	OUString sURL(RTL_CONSTASCII_USTRINGPARAM("private:factory/swriter"));
	OUString sTarget(RTL_CONSTASCII_USTRINGPARAM("odk_officedev_desk")); 
	mxDesktopFrame->setName(sTarget); 
	
	// Get access to the global component loader of the office 
	// for synchronous loading of documents. 
	Reference < ::com::sun::star::frame::XComponentLoader > xLoader (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );

	// Load the document into the target frame by using our unambigous name 
	// and special search flags. 
	Reference <XComponent> xComp = xLoader->loadComponentFromURL( sURL, sTarget, /*::com::sun::star::frame::FrameSearchFlag->CHILDREN*///4, lArgs); 

}


BaseDispatch::~BaseDispatch()
{
	mxFrame.clear();
	mxMSF.clear();
}

///================================================

