#ifndef _MyProtocolHandler_HXX
#define _MyProtocolHandler_HXX

#include <stdio.h>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XControlNotificationListener.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <cppuhelper/implbase3.hxx>
#ifndef _CPPUHELPER_IMPLBASE2_HXX
#include <cppuhelper/implbase2.hxx>
#endif
#include <com/sun/star/awt/XToolkit.hpp>

#define MYPROTOCOLHANDLER_IMPLEMENTATIONNAME   "vnd.demo.Impl.ProtocolHandler"
#define MYPROTOCOLHANDLER_SERVICENAME          "vnd.demo.ProtocolHandler"

namespace com
{
	namespace sun
	{
		namespace star
		{
			namespace frame
			{
				class XModel;
				class XFrame;
			}
		}
	}
}

class MyProtocolHandler : public cppu::WeakImplHelper3
<
	com::sun::star::frame::XDispatchProvider,
	com::sun::star::lang::XInitialization,
	com::sun::star::lang::XServiceInfo
>
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;

public:
	MyProtocolHandler( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF)
        : mxMSF( rxMSF ) {}

	// XDispatchProvider
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
			SAL_CALL queryDispatch(	const ::com::sun::star::util::URL& aURL,
				const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
				throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence < ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > >
		SAL_CALL queryDispatches(
			const ::com::sun::star::uno::Sequence < ::com::sun::star::frame::DispatchDescriptor >& seqDescriptor )
			throw( ::com::sun::star::uno::RuntimeException );

	// XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments )
		throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  )
		throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
		throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  )
		throw (::com::sun::star::uno::RuntimeException);
};

::rtl::OUString MyProtocolHandler_getImplementationName()
	throw ( ::com::sun::star::uno::RuntimeException );

sal_Bool SAL_CALL MyProtocolHandler_supportsService( const ::rtl::OUString& ServiceName )
	throw ( ::com::sun::star::uno::RuntimeException );

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
	throw ( ::com::sun::star::uno::RuntimeException );

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
SAL_CALL MyProtocolHandler_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rSMgr)
	throw ( ::com::sun::star::uno::Exception );

class BaseDispatch : public cppu::WeakImplHelper2
<
	::com::sun::star::frame::XDispatch,
    ::com::sun::star::frame::XControlNotificationListener
>
{
protected:
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxDesktopFrame;//***********
    	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > mxToolkit;
    	::rtl::OUString msDocService;
    	::rtl::OUString maComboBoxText;
    	sal_Bool        mbButtonEnabled;
	
	::rtl::OUString ousLocBdocContUrl;
	int iLocPrevContFlag;
	
public:
	BaseDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame, const ::rtl::OUString& rServiceName );

	virtual ~BaseDispatch();

	//::rtl::OString SAL_CALL convertURItoPath(const ::rtl::OUString ousURI);

	void ShowMessageBox( const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText );
    void SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rArgs, sal_Bool bEnabled );
    void SendCommandTo( const com::sun::star::uno::Reference< com::sun::star::frame::XStatusListener >& xControl, const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rArgs, sal_Bool bEnabled );

	// XDispatch
    virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL& aURL,
		const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArgs )
		throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl,
		const ::com::sun::star::util::URL& aURL ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl,
		const ::com::sun::star::util::URL& aURL ) throw (::com::sun::star::uno::RuntimeException);
    
    // XControlNotificationListener
    virtual void SAL_CALL controlEvent( const ::com::sun::star::frame::ControlEvent& Event ) 
        throw (::com::sun::star::uno::RuntimeException); 
};

class WriterDispatch : public BaseDispatch
{
public:
	WriterDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame )
        : BaseDispatch( rxMSF, xFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextDocument" ) ) )
	{}
};
/*
class CalcDispatch : public BaseDispatch
{
public:
	CalcDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame )
        : BaseDispatch( rxMSF, xFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sheet.SpreadSheetDocument" ) ) )
	{}
};
*/
class PresentationDispatch : public BaseDispatch
{
public:
	PresentationDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame )
        : BaseDispatch( rxMSF, xFrame, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.presentation.PresentationDocument"/*"com.sun.star.drawing.GenericDrawingDocument" */) ) )
	{}
};

void SAL_CALL threadCallMacro(char *pMess);

::rtl::OString SAL_CALL convertURItoPath(const ::rtl::OUString ousURI, bool bToOO);
::rtl::OUString SAL_CALL convertPathToURI(const ::rtl::OUString ousPath);

int getTimeZoneDiff();

int convHexAsciiToInt(char cA, char cB);

void SAL_CALL threadGetPin2();

#endif
