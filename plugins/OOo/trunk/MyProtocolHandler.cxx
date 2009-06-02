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

using namespace std;

using namespace com::sun::star::uno;
using namespace com::sun::star::frame;
using com::sun::star::lang::XMultiServiceFactory;
using com::sun::star::beans::PropertyValue;
using com::sun::star::util::URL;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::text::XTextViewCursor;
using com::sun::star::text::XTextCursor;
using com::sun::star::sheet::XSpreadsheetView;
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
        aDescriptor.Bounds            = Rectangle(0,0,300,200);
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
		Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
		if ( !xCursor.is() && !xView.is() )
			// ohne ein entsprechendes Dokument funktioniert der Handler nicht
			return xRet;

		if ( aURL.Path.equalsAscii("Command1" ) ||
			 aURL.Path.equalsAscii("Command2" ) ||
			 aURL.Path.equalsAscii("Command3" ) ||
			 aURL.Path.equalsAscii("Command4" ) ||
			 aURL.Path.equalsAscii("Command5" ) ||
			 aURL.Path.equalsAscii("Command6" ) ||
             aURL.Path.equalsAscii("Command7" ) )
		{
			xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
			if ( !xRet.is() )
			{
				xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxMSF, mxFrame ) :
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
			m_BdocBridge->teemingilollus1();
			
			m_BdocBridge->DigiInit();
			m_BdocBridge->DigiOpen();			
		}
		
		else if ( !aURL.Path.compareToAscii("Command2" ) )
		{
		    // remove the text if it's in our list
		    Sequence< NamedValue > aRemoveArgs( 1 );
		    aRemoveArgs[0].Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ));
		    aRemoveArgs[0].Value <<= maComboBoxText;
		    SendCommand( aURL, ::rtl::OUString::createFromAscii( "RemoveEntryText" ), aRemoveArgs, sal_True );

		    // add the new text to the start of the list
		    Sequence< NamedValue > aInsertArgs( 2 );
		    aInsertArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pos" ));
		    aInsertArgs[0].Value <<= sal_Int32( 0 );
		    aInsertArgs[1].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ));
		    aInsertArgs[1].Value <<= maComboBoxText;
		    SendCommand( aURL, ::rtl::OUString::createFromAscii( "InsertEntry" ), aInsertArgs, sal_True );
		}
	
		else if ( !aURL.Path.compareToAscii("Command3" ) )
		{
		    // Retrieve the text argument from the sequence property value
		    rtl::OUString aText;
		    for ( sal_Int32 i = 0; i < lArgs.getLength(); i++ )
		    {
		        if ( lArgs[i].Name.equalsAsciiL( "Text", 4 ))
		        {
		            lArgs[i].Value >>= aText;
		            break;
		        }
		    }

		    // create new URL to address the combox box
		    URL aCmdURL;
		    aCmdURL.Path = rtl::OUString::createFromAscii( "Command2" );
		    aCmdURL.Protocol = rtl::OUString::createFromAscii( "vnd.demo.complextoolbarcontrols.demoaddon:" );
		    aCmdURL.Complete = aCmdURL.Path + aCmdURL.Protocol;
		    
		    // set the selected item as text into the combobox
		    Sequence< NamedValue > aArgs( 1 );
		    aArgs[0].Name = rtl::OUString::createFromAscii( "Text" );
		    aArgs[0].Value <<= aText;
		    SendCommand( aCmdURL, ::rtl::OUString::createFromAscii( "SetText" ), aArgs, sal_True );
			}
			else if ( !aURL.Path.compareToAscii("Command4" ) )
			{
		    // Retrieve the text argument from the sequence property value
		    rtl::OUString aText;
		    for ( sal_Int32 i = 0; i < lArgs.getLength(); i++ )
		    {
		        if ( lArgs[i].Name.equalsAsciiL( "Text", 4 ))
		        {
		            lArgs[i].Value >>= aText;
		            break;
		        }
		    }
			
		    // just enable this command
			    
		    // set enable flag according to selection
		    if ( aText.equalsAscii( "Button Disabled" ))
		        mbButtonEnabled = sal_False;
		    else
		        mbButtonEnabled = sal_True;

		    // create new URL to address the image button
		    URL aCmdURL;
		    aCmdURL.Path = rtl::OUString::createFromAscii( "Command1" );
		    aCmdURL.Protocol = rtl::OUString::createFromAscii( "vnd.demo.complextoolbarcontrols.demoaddon:" );
		    aCmdURL.Complete = aCmdURL.Path + aCmdURL.Protocol;
		    
		    // create and initialize FeatureStateEvent with IsEnabled
		    ::com::sun::star::frame::FeatureStateEvent aEvent;
				aEvent.FeatureURL = aCmdURL;
			    aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
				aEvent.IsEnabled = mbButtonEnabled;
				aEvent.Requery = sal_False;
				aEvent.State <<= Any();
		    
		    // Notify listener about new state
		    Reference < XDispatch > xDispatch = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
		    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent ); 
		}
		else if ( !aURL.Path.compareToAscii("Command5" ) )
		{
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
        else if ( aURL.Path.equalsAscii("Command2" ) )
		{
			// just enable this command
		    ::com::sun::star::frame::FeatureStateEvent aEvent;
			aEvent.FeatureURL = aURL;
		    aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
			aEvent.IsEnabled = sal_True;
			aEvent.Requery = sal_False;
			aEvent.State <<= Any();
			xControl->statusChanged( aEvent );
		}
		else if ( aURL.Path.equalsAscii("Command3" ) )
		{
            // A toggle dropdown box is normally used for a group of commands
            // where the user can select the last issued command easily.
            // E.g. a typical command group would be "Insert shape"
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 3 );
            aContextMenu[0] = rtl::OUString::createFromAscii( "Command 1" );
            aContextMenu[1] = rtl::OUString::createFromAscii( "Command 2" );
            aContextMenu[2] = rtl::OUString::createFromAscii( "Command 3" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );
            
            // send command to check item on pos=0
            aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pos" ));
            aArgs[0].Value <<= sal_Int32( 0 );            
            SendCommandTo( xControl, aURL, ::rtl::OUString::createFromAscii( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command4" ) )
        {
            // A dropdown box is normally used for a group of dependent modes, where
            // the user can only select one. The modes cannot be combined.
            // E.g. a typical group would be left,right,center,block.
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 2 );
            aContextMenu[0] = rtl::OUString::createFromAscii( "Button Enabled" );
            aContextMenu[1] = rtl::OUString::createFromAscii( "Button Disabled" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );            

            // set position according to enable/disable state of button
            sal_Int32 nPos( mbButtonEnabled ? 0 : 1 );
            
            // send command to check item on pos=0
            aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pos" ));
            aArgs[0].Value <<= nPos;
            SendCommandTo( xControl, aURL, ::rtl::OUString::createFromAscii( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command5" ) )
        {
            // A spin button
            Sequence< NamedValue > aArgs( 5 );
            
            // send command to initialize spin button
            aArgs[0].Name = rtl::OUString::createFromAscii( "Value" );
            aArgs[0].Value <<= double( 0.0 );
            aArgs[1].Name = rtl::OUString::createFromAscii( "UpperLimit" );
            aArgs[1].Value <<= double( 10.0 );
            aArgs[2].Name = rtl::OUString::createFromAscii( "LowerLimit" );
            aArgs[2].Value <<= double( 0.0 );
            aArgs[3].Name = rtl::OUString::createFromAscii( "Step" );
            aArgs[3].Value <<= double( 0.1 );
            aArgs[4].Name = rtl::OUString::createFromAscii( "OutputFormat" );
            aArgs[4].Value <<= rtl::OUString::createFromAscii( "%.2f cm" );

            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetValues" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command7" ) )
        {
            // A dropdown box is normally used for a group of commands
            // where the user can select one of a defined set.
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aList( 10 );
            aList[0] = rtl::OUString::createFromAscii( "White" );
            aList[1] = rtl::OUString::createFromAscii( "Black" );
            aList[2] = rtl::OUString::createFromAscii( "Red" );
            aList[3] = rtl::OUString::createFromAscii( "Blue" );
            aList[4] = rtl::OUString::createFromAscii( "Green" );
            aList[5] = rtl::OUString::createFromAscii( "Grey" );
            aList[6] = rtl::OUString::createFromAscii( "Yellow" );
            aList[7] = rtl::OUString::createFromAscii( "Orange" );
            aList[8] = rtl::OUString::createFromAscii( "Brown" );
            aList[9] = rtl::OUString::createFromAscii( "Pink" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aList;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );
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
        if ( Event.aURL.Path.equalsAscii( "Command2" ))
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
    }
}
 
BaseDispatch::BaseDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame, const ::rtl::OUString& rServiceName )
        : mxMSF( rxMSF )
		, mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )
{
}


BaseDispatch::~BaseDispatch()
{
	mxFrame.clear();
	mxMSF.clear();
}

///================================================

