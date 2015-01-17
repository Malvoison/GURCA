// TsClientMacAddr.cpp : Implementation of CTsClientMacAddr

#include "stdafx.h"
#include "TsClientMacAddr.h"
#include "Common.h"
#include "XmlHelper.h"

//
// CTsClientMacAddr
//
HRESULT CTsClientMacAddr::Initialize(IWTSVirtualChannelManager *pChannelMgr)
{
	HRESULT hr;

	try
	{
		OutputDebugString(L"CTsClientMacAddr::Initialize\n");
		//  Create the DVC Listener
		hr = pChannelMgr->CreateListener(TS_MACADDR_NAME,
			0, this, &_spListener);
	}
	catch (std::bad_alloc&)
	{
		hr = E_OUTOFMEMORY;
	}
	catch (std::exception&)
	{
		hr = E_UNEXPECTED;
	}
	
	return hr;
}

HRESULT CTsClientMacAddr::Connected()
{
	OutputDebugString(L"CTsClientMacAddr::Connected\n");
	return S_OK;
}

HRESULT CTsClientMacAddr::Disconnected(DWORD dwDisconnectCode)
{
	OutputDebugString(L"CTsClientMacAddr::Disconnected\n");
	UNREFERENCED_PARAMETER(dwDisconnectCode);
	return S_OK;
}

HRESULT CTsClientMacAddr::Terminated()
{
	OutputDebugString(L"CTsClientMacAddr::Terminated\n");
	return S_OK;
}

//
//  IWTSListenerCallback
//
HRESULT CTsClientMacAddr::OnNewChannelConnection(
	IWTSVirtualChannel *pChannel, BSTR data, BOOL *pbAccept, IWTSVirtualChannelCallback **ppCallback)
{
	OutputDebugString(L"CTsClientMacAddr::OnNewChannelConnection\n");

	UNREFERENCED_PARAMETER(data);

	*pbAccept = TRUE;  //  always accept connection

	*ppCallback = this;
	AddRef();

	_spChannel = pChannel;  //  save the channel

	return S_OK;
}

//
//  IWTSChannelCallback
//
HRESULT CTsClientMacAddr::OnDataReceived(ULONG cbSize, BYTE *pBuffer)
{
	OutputDebugString(L"CTsClientMacAddr::OnDataReceived\n");
	HRESULT hr = S_OK;	

	OutputDebugString((wchar_t*)pBuffer);
	OutputDebugString(L"\n");

	//  Parse the server request
	BSTR bstrRequest = SysAllocString((wchar_t*)pBuffer);
	BSTR bstrErr = NULL;
	IXMLDOMDocument *pXMLDomRequest = NULL;	
	IXMLDOMParseError *pXMLErr = NULL;
	IXMLDOMNode *pRequestTypeNode = NULL;
	IXMLDOMNode *pAttributeNode = NULL;
	IXMLDOMNamedNodeMap *pIXMLDOMNamedNodeMap = NULL;
	IXMLDOMElement *pIXMLDOMElement = NULL;
	BSTR bstrAttributeName = SysAllocString(L"RequestType");
	BSTR bstrQueryString = SysAllocString(L"//MacAddrServer/Request");
	VARIANT_BOOL varStatus;
	wchar_t *wszRequestType = NULL;

	hr = CreateAndInitDOM(&pXMLDomRequest);
	hr = pXMLDomRequest->loadXML(bstrRequest, &varStatus);
	if (varStatus == VARIANT_TRUE)
	{
		//  Find the request type attribute
		VARIANT varValue;
				
		hr = pXMLDomRequest->selectSingleNode(bstrQueryString, &pRequestTypeNode);
		if (SUCCEEDED(hr) && pRequestTypeNode)
		{
			hr = pRequestTypeNode->get_attributes(&pIXMLDOMNamedNodeMap);
			if (SUCCEEDED(hr) && pIXMLDOMNamedNodeMap)
			{
				hr = pIXMLDOMNamedNodeMap->getNamedItem(bstrAttributeName, &pAttributeNode);
				if (SUCCEEDED(hr) && pAttributeNode)
				{
					hr = pAttributeNode->get_nodeValue(&varValue);
				}
				else
				{
					bstrMacAddr = SysAllocString(L"<Error ErrorReason=\"Malformed server request\" />");
				}
			}
			else
			{
				bstrMacAddr = SysAllocString(L"<Error ErrorReason=\"Malformed server request\" />");
			}
		}
		else
		{
			//  bad...
			bstrMacAddr = SysAllocString(L"<Error ErrorReason=\"Malformed server request\" />");
			goto ErrorExit;
		}

		UINT uiLength = SysStringLen(varValue.bstrVal);
		wszRequestType = new wchar_t[uiLength + 1];
		wcscpy_s(wszRequestType, uiLength + 1, (wchar_t*)varValue.bstrVal);	
	}
	else
	{
		hr = pXMLDomRequest->get_parseError(&pXMLErr);
		hr = pXMLErr->get_reason(&bstrErr);
		OutputDebugString((LPCWSTR)bstrErr);
		OutputDebugString(L"\n");
		SysFreeString(bstrErr);

		bstrMacAddr = SysAllocString(L"<Error ErrorReason=\"Malformed server request\" />");
		goto ErrorExit;
	}

	if (wcscmp(wszRequestType, L"MacAddress") == 0)
	{
		if (bstrMacAddr == NULL)
		{
			DWORD dwNumIf = 0;
			DWORD dwResult = GetNumberOfInterfaces(&dwNumIf);

			IP_ADAPTER_INFO* iai = new IP_ADAPTER_INFO[dwNumIf - 1];
			ULONG OutBufLen = sizeof(IP_ADAPTER_INFO)* (dwNumIf - 1);

			dwResult = GetAdaptersInfo(iai, &OutBufLen);

			wchar_t wszMacAddress[100];

			if (dwResult != ERROR_BUFFER_OVERFLOW)
			{
				wsprintf(wszMacAddress, L"%02X:%02X:%02X:%02X:%02X:%02X",
					iai[0].Address[0], iai[0].Address[1], iai[0].Address[2],
					iai[0].Address[3], iai[0].Address[4], iai[0].Address[5]);
				OutputDebugString(wszMacAddress);
				OutputDebugString(L"\n");
			}
			else
			{
				wsprintf(wszMacAddress, L"%02X:%02X:%02X:%02X:%02X:%02X",
					0, 0, 0, 0, 0, 0);
			}

			//  Create the XML document to send to the server
			IXMLDOMDocument *pXMLDom = NULL;
			IXMLDOMElement *pRoot = NULL;
			IXMLDOMElement *pNode = NULL;

			CHK_HR(CreateAndInitDOM(&pXMLDom));
			CHK_HR(CreateAndAddPINode(pXMLDom, L"xml", L"version='1.0'"));
			CHK_HR(CreateAndAddCommentNode(pXMLDom, L"Created with MacAddrPlgn v1.0"));
			CHK_HR(CreateElement(pXMLDom, L"MacAddrClnt", &pRoot));
			CHK_HR(CreateAndAddAttributeNode(pXMLDom, L"Mode", L"Root", pRoot));

			CHK_HR(CreateAndAddElementNode(pXMLDom, L"MacAddr", L"\n\t", pRoot, &pNode));
			CHK_HR(CreateAndAddAttributeNode(pXMLDom, L"Value", wszMacAddress, pNode));
			SAFE_RELEASE(pNode);

			CHK_HR(CreateAndAddTextNode(pXMLDom, L"\n", pRoot));
			CHK_HR(AppendChildToParent(pRoot, pXMLDom));

			CHK_HR(pXMLDom->get_xml(&bstrMacAddr));

		CleanUp:
			if (bstrMacAddr == NULL)
				bstrMacAddr = SysAllocString(L"<Error ErrorReason=\"Unknown\" />");
			SAFE_RELEASE(pXMLDom);
			SAFE_RELEASE(pRoot);
			SAFE_RELEASE(pNode);
		}
	}

ErrorExit:

	SysFreeString(bstrAttributeName);
	SysFreeString(bstrQueryString);
	SAFE_RELEASE(pIXMLDOMElement);
	SAFE_RELEASE(pIXMLDOMNamedNodeMap);
	SAFE_RELEASE(pAttributeNode);
	SAFE_RELEASE(pRequestTypeNode);
	SAFE_RELEASE(pXMLDomRequest);
	SAFE_RELEASE(pXMLErr);

	UINT uiLength = SysStringLen(bstrMacAddr);
	wchar_t *wszMacAddr = new wchar_t[uiLength + 1];
	wcscpy_s(wszMacAddr, uiLength + 1, (wchar_t*)bstrMacAddr);

	hr = _spChannel->Write((uiLength + 1) * sizeof(wchar_t), (PBYTE)wszMacAddr, NULL);

	delete wszMacAddr;
	if (wszRequestType)
		delete wszRequestType;
	
	return hr;
}

HRESULT CTsClientMacAddr::OnClose()
{
	OutputDebugString(L"CTsClientMacAddr::OnClose\n");

	if (bstrMacAddr != NULL)
	{
		SysFreeString(bstrMacAddr);
		bstrMacAddr = NULL;
	}
	return S_OK;
}
