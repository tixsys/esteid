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
#include <iostream>
#include <time.h>
#include <sstream>
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
//**********************************
printf("BaseDispatch::SendCommand\n");
//***********************************
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

	//Add an information to the statusbar
	
	bPrevContFlag = bContFlag;
	bContFlag = false;
	Reference < XController > xCtrl = mxFrame->getController();

	//----------------- Get File component and URL -------------------------
	Reference<XModel> xModel1(xCtrl->getModel(),UNO_QUERY);	
	
	Reference < XDesktop > pDesktop (mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);

	Reference< XComponent > xCompMain = pDesktop->getCurrentComponent();

	ousBDocFileURL = xModel1->getURL();
	OString muffik = OUStringToOString(ousBDocFileURL, RTL_TEXTENCODING_ASCII_US);
PRINT_DEBUG ("URL : %s",muffik.pData->buffer);
	string strContainerPath = muffik.pData->buffer;
	//----------------------------------------------------------------------
	//----------------- If Bdoc Container, open it -------------------------
	if (!memcmp(&strContainerPath[strContainerPath.size() - 5], ".bdoc", 5))
	{
		MyBdocBridge * m_BdocBridge1 = MyBdocBridge::getInstance();
		m_BdocBridge1->DigiInit();
		m_BdocBridge1->DigiOpen(convertURItoPath(ousBDocFileURL).pData->buffer);
		//m_BdocBridge1->DigiOpen(&strContainerPath[sizeof(UNO_URL_HEAD)]);
PRINT_DEBUG ("TempFile URL : %s",m_BdocBridge1->pRetPath);				
		string strTempFileUrl = UNO_URL_HEAD;
		strTempFileUrl += m_BdocBridge1->pRetPath;
		ousBDocContURL = ousBDocFileURL; //<-----Get access to the container in new frame!
		ousBDocFileURL = ::rtl::OUString(strTempFileUrl.data(),strTempFileUrl.size(), RTL_TEXTENCODING_UNICODE, 0);

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
		//Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, /*frame::FrameSearchFlag->CHILDREN*/4, loadProps)); 
		Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, 8, loadProps));
		//Reference <XStatusbarController> xStatusBar(xComp2, UNO_QUERY);
		mxFrame->setName(sOldName);

		Reference <XCloseable> xCloseable( mxFrame, UNO_QUERY);
		xCloseable->close(true);

	}		
	//----------------------------------------------------------------------

	else if (bPrevContFlag) //-if it's a new frame after opening a bdoc container
	{
		//muffik = OUStringToOString(ousBDocContURL, RTL_TEXTENCODING_ASCII_US);
		//strContainerPath = muffik.pData->buffer;

		MyBdocBridge * m_BdocBridge1 = MyBdocBridge::getInstance();
		m_BdocBridge1->DigiInit();
		m_BdocBridge1->DigiOpen(convertURItoPath(ousBDocContURL).pData->buffer);
		//m_BdocBridge1->DigiOpen(&strContainerPath[sizeof(UNO_URL_HEAD)]);

		Reference <XDesktop> rDesktop(mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))),UNO_QUERY);
		rGlobalDesktop = rDesktop;

		Reference< XDispatchHelper > rDispatchHelper = Reference < XDispatchHelper > ( mxMSF->createInstance(OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.DispatchHelper" ))), UNO_QUERY );
		rGlobalDispatchHelper = rDispatchHelper;
PRINT_DEBUG("Return from FileOpen: %d", m_BdocBridge1->ret);
		if (m_BdocBridge1->ret < 100)// && (m_BdocBridge1->ret < m_BdocBridge1->iSignCnt))//check for possible errors
		{
			
			//------------Fix sign data string from returned data---------		
			strSignData = "";
			//char* cTemp = NULL;
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
						wchar_t utfChar = ((iCD1 - 192) * 64) + (iCD2 - 128);
						
PRINT_DEBUG("utfChar: %c \nNum1: %d\nNum2: %d\nutf as nr: %d", utfChar, iCD1, iCD2, utfChar);
						strSignData += (char)utfChar;

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
			//OUString ousTemp(RTL_CONSTASCII_USTRINGPARAM(strSignData.c_str()));
			OString oTemp(strSignData.c_str(), RTL_TEXTENCODING_ISO_8859_15);
			strSignData	= oTemp.pData->buffer; 
			strSignData = "macro:///HW.HW.GetCert(*" + strSignData + ")";
			
			
			//Amazing converting to proper ascii
			//OString osSignData = OUStringToOString((OUString::createFromAscii(strSignData.c_str())), RTL_TEXTENCODING_ISO_8859_15);// 
			//----------------------------------------------------------------
PRINT_DEBUG("Signature Data: %s",strSignData.c_str());
			//oslWorkerFunction type : void (SAL_CALL *oslWorkerFunction)(void*); in osl/thread.h
			oslWorkerFunction pFunc1 = (void (SAL_CALL *)(void*)) threadCallMacro;
			// create and start the hThreadShowSign with pcSignData as a parameter value		
			oslThread hThreadShowSign = osl_createThread(pFunc1,(void *) strSignData.c_str());
			//----------------------------------------------------------------
			
			if (m_BdocBridge1->ret)	//validation failed
			{
			//Change Statusbar
				pcMessage = "macro:///HW.HW.StatusBarCtrl(KEHTETU ALLKIRI!  -- Fail mida soovite avada sisaldab kehtetuid või aegunud allkirju!)";
			}

			else//Change Statusbar			
				pcMessage = "macro:///HW.HW.StatusBarCtrl(ALLKIRJASTATUD Fail!  -- faili muutmine blokeeritud!)";
			//oslWorkerFunction pFunc2 = (void (SAL_CALL *)(void*)) threadChangeStatusBar;
			//oslThread hThreadChangeStatusBar = osl_createThread(pFunc2,(void *) pcMessage);

			// dispose the local service manager
			//Reference< XComponent >::query( xMultiComponentFactoryClient )->dispose();

		//----------------------------------------------------------------
		}
		else if (m_BdocBridge1->ret >= 10000)
		{
			//Change Statusbar
			pcMessage = "macro:///HW.HW.StatusBarCtrl(ALLKIRJASTATUD Fail!  -- faili muutmine blokeeritud! -- Allkirja lugemisel tekkis viga!)";
			//oslWorkerFunction pFunc2 = (void (SAL_CALL *)(void*)) threadChangeStatusBar;
			//oslThread hThreadChangeStatusBar = osl_createThread(pFunc2,(void *) pcMessage);
		}
		
		//oslWorkerFunction pFunc2 = (void (SAL_CALL *)(void*)) threadCallMacro;
		//oslThread hThreadChangeStatusBar = osl_createThread(pFunc2,(void *) pcMessage);		
	}	
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
				throw( RuntimeException )
{
	Reference < XDispatch > xRet;
	if ( !mxFrame.is() )
		return 0;

	Reference < XController > xCtrl = mxFrame->getController();

	if (xCtrl.is() && !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
		Reference <XPresentation> xPresentation ( xCtrl, UNO_QUERY );
		//Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
		//if ( !xCursor.is() && !xView.is() )
			// ohne ein entsprechendes Dokument funktioniert der Handler nicht
		//	return xRet;

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

	if ( !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		if ( !aURL.Path.compareToAscii("Command1" ))
		{
			MyBdocBridge * m_BdocBridge = MyBdocBridge::getInstance();

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
			//pParam = xMyModel->getURL();
			//muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
			//printf("URL : %s\n",muff.pData->buffer);	
			//check which OOo service app is running
			//if ()
			//{						
				//Reference < XTextDocument> xDoc(xComp, UNO_QUERY);

			ostrPath = convertURItoPath(xMyModel->getURL());
			//ostrPath = ::BaseDispatch::convertURItoPath(xMyModel->getURL());


//PRINT_DEBUG( "After Converter File OstrPath: %s", ostrPath.pData->buffer);
		
			//for bdoc container
			muff = OUStringToOString(ousLocBdocContUrl, RTL_TEXTENCODING_ASCII_US);
			string strBdocUrl;
			strBdocUrl = muff.pData->buffer;

			//---------------------if file has not been saved----------------------------------
			bool bPathIs = memcmp(ostrPath.pData->buffer, "", 1);
			if (!bPathIs)
			{
				i_try = 0;
				::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Salvesamata Fail!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Palun salvestage fail!" )));
			}
			
			//---------------------check if card is in reader----------------------------------
			else 
			{				
				m_BdocBridge->DigiInit();
				m_BdocBridge->DigiCheckCert();
				if (m_BdocBridge->ret == 1)
				{ //NO card or cardreader
					Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.NoCard?language=Basic&location=application") ), UNO_QUERY);
					xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					i_try = 0;
				}
			}	
			//----------------------If We are dealing with BDoc container----------------------
			if (!memcmp(&strBdocUrl[strBdocUrl.size() - 5], ".bdoc", 5) && iLocPrevContFlag && bPathIs)
			{	
				int iHour;
				m_BdocBridge->DigiInit();
				m_BdocBridge->DigiOpen(convertURItoPath(ousLocBdocContUrl).pData->buffer);
				//m_BdocBridge->DigiOpen(&strBdocUrl[sizeof(UNO_URL_HEAD)]);
				
				//			string strTempFileUrl = "file://";
				//			strTempFileUrl += m_BdocBridge->pRetPath;
				
				//------------Fix sign data string from returned data---------			
				if (m_BdocBridge->ret < 100)// && (m_BdocBridge->ret < m_BdocBridge->iSignCnt))	
				{// Container has at least one valid signature
					strSignData = "";
					int k,l,m,n,o,p,q,r;
					k=l=m=n=o=p=q=r=0;
					for (int cnt=0; cnt<m_BdocBridge->iSignCnt; cnt++)
					{	//Signer Name
						while (m_BdocBridge->pSignName[k] != '#')
						{
							if ((m_BdocBridge->pSignName[k] == '\\') && (m_BdocBridge->pSignName[k+1] == ','))
							{
								strSignData += ";";
								k++;
							}
							else
								strSignData += m_BdocBridge->pSignName[k];
							k++;
						}
						k++;
						strSignData += ";";

						//Signer Role
						if (sizeof(m_BdocBridge->pSignRole) > m_BdocBridge->iSignCnt)
						{
							//strSignData += "Roll: ";
							while (m_BdocBridge->pSignRole[l] != '#')
							{
								strSignData += m_BdocBridge->pSignRole[l];
								l++;
							}
							l++;
						}
						strSignData += ";";

						//Signer AddRole
						if (sizeof(m_BdocBridge->pSignAddRole) > m_BdocBridge->iSignCnt)
						{
							//strSignData += "Resolutsioon: ";
							while (m_BdocBridge->pSignAddRole[m] != '#')
							{
								strSignData += m_BdocBridge->pSignAddRole[m];
								m++;
							}
							m++;
						}
						strSignData += ";";

						//Signing Time
						while (m_BdocBridge->pSignTime[n] != '#')
						{
							if (m_BdocBridge->pSignTime[n] == 'T')
							{
								stringstream out;
								strSignData += " ";
								iHour = 10*((int)m_BdocBridge->pSignTime[++n] - 48);
								iHour += (int)m_BdocBridge->pSignTime[++n] - 48;
								iHour += getTimeZoneDiff();						
								out << iHour;
								strSignData += out.str();
							}
							else if (m_BdocBridge->pSignTime[n] != 'Z')
								strSignData += m_BdocBridge->pSignTime[n];
													
							n++;
						}
						n++;
						strSignData += ";";
					
						//Signing City
						if (sizeof(m_BdocBridge->pSignCity) > m_BdocBridge->iSignCnt)
						{
							//strSignData += "Linn: ";
							while (m_BdocBridge->pSignCity[o] != '#')
							{
								strSignData += m_BdocBridge->pSignCity[o];
								o++;
							}
							o++;
						}
						strSignData += ";";

						//Signing Region
						while (m_BdocBridge->pSignState[p] != '#')
						{
							strSignData += m_BdocBridge->pSignState[p];
							p++;
						}
						p++;
						strSignData += ";";

						//Signing Postal nr
						while (m_BdocBridge->pSignPostal[q] != '#')
						{
							strSignData += m_BdocBridge->pSignPostal[q];
							q++;
						}
						q++;
						strSignData += ";";

						//Signing Postal nr
						while (m_BdocBridge->pSignCountry[r] != '#')
						{
							strSignData += m_BdocBridge->pSignCountry[r];
							r++;
						}
						r++;
						strSignData += ";";

						strSignData += "_____________________;";
					}
					strSignData = "macro:///HW.HW.GetCert(#" + strSignData + ")";
					//------------------------------------------------------------
/*					//OString osSignData = OUStringToOString((OUString::createFromAscii(strSignData.c_str())), RTL_TEXTENCODING_ASCII_US);//
					oslWorkerFunction pFunc1 = (void (SAL_CALL *)(void*)) threadCallMacro;
					//oslThread hThreadShowSign = osl_createThread(pFunc1,(void *) osSignData.pData->buffer);
					oslThread hThreadShowSign = osl_createThread(pFunc1,(void *) strSignData.c_str());
*/					//OBS! Call in function instead of thread. Threads may delay and mess with dialog order					
					//-------------Open Signature Viewer Macro------------------------
					Reference< XDispatchHelper > rDispatchHelper = Reference < XDispatchHelper > ( mxMSF->createInstance(OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.DispatchHelper" ))), UNO_QUERY );
					Reference< XDispatchProvider > rDispatchProvider(rDesktop,UNO_QUERY);
	 
					Any any=rDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii(strSignData.data()), OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
				//----------------------------------------------------------------
				}
							
//printf("macro returns: %s\n",muff.pData->buffer);				
				
				//i_try = 0;
				
			}
			//---------------New File will be signed -> View a Warning------------------
			else if (bPathIs && i_try)
			{	
				strBdocUrl = "";		
				Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.Init?language=Basic&location=application") ), UNO_QUERY);
				xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
				muff = OUStringToOString(pParam, RTL_TEXTENCODING_ASCII_US);
							
//printf("macro returns: %s\n",muff.pData->buffer);				
				//--If Cacel button--
				if (!memcmp(muff.pData->buffer, "*", 1))
					i_try = 0;			
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
//printf("macrost1: %s\n",ostrParam.pData->buffer);
					Reference < XScript > xScript(xScriptProvider->getScript( OUString::createFromAscii("vnd.sun.star.script:HW.HW.PIN?language=Basic&location=application") ), UNO_QUERY);
					xScript->invoke(Sequence <Any>(), indexes, outparam) >>= pParam;
					ostrPin = OUStringToOString(pParam, RTL_TEXTENCODING_UTF8);
			
					//--If OK button--
					if (memcmp(ostrPin.pData->buffer, "*", 1))
					{	//sign file --if not success retry					
						if (strBdocUrl.size()>1)
						//if (!memcmp (ostrPath.pData->buffer, &strBdocUrl, strBdocUrl.size()))
						{	//- tõsta see hiljem else sisse!!!
							//if its an open bdoc container	
		 			
							m_BdocBridge->DigiInit();
							//m_BdocBridge->DigiOpen(&strBdocUrl[sizeof(UNO_URL_HEAD)]);
							m_BdocBridge->DigiOpen(convertURItoPath(ousLocBdocContUrl).pData->buffer);
						}

						m_BdocBridge->DigiSign(ostrPath.pData->buffer, ostrParam.pData->buffer, ostrPin.pData->buffer);	

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
								//m_BdocBridge1->DigiOpen(&strContainerPath[sizeof(UNO_URL_HEAD)]);
//	PRINT_DEBUG ("TempFile URL : %s",m_BdocBridge->pRetPath);	
								string strTempFileUrl = UNO_URL_HEAD;
								strTempFileUrl += m_BdocBridge->pRetPath;
								strNewPath = UNO_URL_HEAD + strNewPath;
//	PRINT_DEBUG ("NewTempFile URL: %s\n",strTempFileUrl.c_str());
//	PRINT_DEBUG ("NewContFile URL: %s\n",strTempFileUrl.c_str());
								ousBDocContURL = ::rtl::OUString(strNewPath.data(),strNewPath.size(), RTL_TEXTENCODING_UNICODE, 0);; //<-----Get access to the container in new frame!
								ousBDocFileURL = ::rtl::OUString(strTempFileUrl.data(),strTempFileUrl.size(), RTL_TEXTENCODING_UNICODE, 0);

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
								//Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, /*frame::FrameSearchFlag->CHILDREN*/4, loadProps)); 
								Reference <XComponent> xComp2 (xLoader->loadComponentFromURL( ousBDocFileURL, sTarget, 8, loadProps));
								//Reference <XStatusbarController> xStatusBar(xComp2, UNO_QUERY);
								mxFrame->setName(sOldName);
								xComp->dispose();
							}
							//----------------------------------------------------------------------
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
						else if (m_BdocBridge->ret == 100)
						{
							::BaseDispatch::ShowMessageBox(mxFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Korduv Allkiri!" )), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Kuvatav fail on juba Teie poolt allkirjastatud!" )));
							i_try = 0;
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
	
printf("BaseDispatch::removeStatusListener\n");
OString osBDocFilePath = OUStringToOString(ousBDocFileURL, RTL_TEXTENCODING_ASCII_US);
printf("URL in removeStatusListener: %s\n",osBDocFilePath.pData->buffer);

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
	ousLocBdocContUrl = ousBDocContURL;
	iLocPrevContFlag = bPrevContFlag;
}


BaseDispatch::~BaseDispatch()
{
	mxFrame.clear();
	mxMSF.clear();
}

///================================================

/*void SAL_CALL threadShowSign(char *pData)
{
	Reference < XDesktop > rLocDesktop = rGlobalDesktop;
	Reference < XDispatchHelper > rLocalDispatchHelper = rGlobalDispatchHelper;

	//-------------Open Signature Viewer Macro------------------------
	Reference< XDispatchProvider > rDispatchProvider(rLocDesktop,UNO_QUERY);
	Any any = rLocalDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii(pData), OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
	//----------------------------------------------------------------
}*/
///================================================

void SAL_CALL threadCallMacro(char *pData)
{
	Reference < XDesktop > rLocDesktop = rGlobalDesktop;
	Reference < XDispatchHelper > rLocalDispatchHelper = rGlobalDispatchHelper;
PRINT_DEBUG("In the MacroCallerThread: %s",pData);
	//----------------------Open Macro--------------------------------
	Reference< XDispatchProvider > rDispatchProvider(rLocDesktop,UNO_QUERY);
	Any any = rLocalDispatchHelper->executeDispatch(rDispatchProvider, OUString::createFromAscii(pData), OUString::createFromAscii(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
	//----------------------------------------------------------------
}

//OString SAL_CALL ::BaseDispatch::convertURItoPath(OUString ousURI)
OString SAL_CALL convertURItoPath(OUString ousURI)
{
	OString osPath;
	strBuffer = "";
	osPath = OUStringToOString(ousURI, RTL_TEXTENCODING_ISO_8859_15);
	//osPath = OUStringToOString(ousURI, RTL_TEXTENCODING_ASCII_US);

	for(int i=sizeof(UNO_URL_HEAD)-1; i<osPath.getLength(); i++)
	{
		if (!memcmp(&osPath.pData->buffer[i], "%20", 3))
		{
			strBuffer += " ";
			i += 3;
		}
		/*if (osPath.pData->buffer[i] == '\\')
		{
			strBuffer += "/";
			i++;
		}*/
		strBuffer += osPath.pData->buffer[i];
	}

	return OString(strBuffer.c_str());
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
