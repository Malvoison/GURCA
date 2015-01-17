// TsClientMacAddr.h : Declaration of the CTsClientMacAddr

#pragma once
#include "resource.h"       // main symbols



#include "MacAddrPlgn_i.h"

using namespace ATL;

// CTsClientMacAddr

class ATL_NO_VTABLE CTsClientMacAddr :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTsClientMacAddr, &CLSID_TsClientMacAddr>,
	public IDispatchImpl<ITsClientMacAddr, &IID_ITsClientMacAddr, &LIBID_MacAddrPlgnLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IWTSPlugin,
	public IWTSListenerCallback,
	public IWTSVirtualChannelCallback
{
public:
	CTsClientMacAddr()
	{
		bstrMacAddr = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TSCLIENTMACADDR)


BEGIN_COM_MAP(CTsClientMacAddr)
	COM_INTERFACE_ENTRY(ITsClientMacAddr)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IWTSPlugin)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	//  IWTSPlugin
public:	
	STDMETHOD(Initialize)(IWTSVirtualChannelManager *pChannelMgr);
	STDMETHOD(Connected)();
	STDMETHOD(Disconnected)(DWORD dwDisconnectCode);
	STDMETHOD(Terminated)();

	//  IWTSListenerCallback
public:
	STDMETHOD(OnNewChannelConnection)(IWTSVirtualChannel *pChannel, BSTR data, BOOL *pAccept, IWTSVirtualChannelCallback **ppCallback);

	//  IWTSVirtualChannelCallback
public:
	STDMETHOD(OnDataReceived)(ULONG cbSize, BYTE *pBuffer);
	STDMETHOD(OnClose)();

private:
	CComPtr<IWTSListener> _spListener;
	CComPtr<IWTSVirtualChannel> _spChannel;
	BSTR bstrMacAddr;
};

OBJECT_ENTRY_AUTO(__uuidof(TsClientMacAddr), CTsClientMacAddr)
