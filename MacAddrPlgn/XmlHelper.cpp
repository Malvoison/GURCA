//  Filename:  XmlHelper.cpp

#include "stdafx.h"
#include "XmlHelper.h"

#include <MsXml6.h>


HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
	HRESULT hr = S_OK;
	BSTR bstr = SysAllocString(wszValue);
	CHK_ALLOC(bstr);

	V_VT(&Variant) = VT_BSTR;
	V_BSTR(&Variant) = bstr;

CleanUp:
	return hr;
}

HRESULT CreateAndInitDOM(IXMLDOMDocument **ppDoc)
{
	HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
	if (SUCCEEDED(hr))
	{
		//  these methods should not fail, so don't inspect result
		(*ppDoc)->put_async(VARIANT_FALSE);
		(*ppDoc)->put_validateOnParse(VARIANT_FALSE);
		(*ppDoc)->put_resolveExternals(VARIANT_FALSE);
		(*ppDoc)->put_preserveWhiteSpace(VARIANT_TRUE);
	}

	return hr;
}

HRESULT CreateElement(IXMLDOMDocument *pXMLDom, PCWSTR wszName, IXMLDOMElement **ppElement)
{
	HRESULT hr = S_OK;
	*ppElement = NULL;

	BSTR bstrName = SysAllocString(wszName);
	CHK_ALLOC(bstrName);
	CHK_HR(pXMLDom->createElement(bstrName, ppElement));

CleanUp:
	SysFreeString(bstrName);
	return hr;
}

HRESULT AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMNode *pChildOut = NULL;
	CHK_HR(pParent->appendChild(pChild, &pChildOut));

CleanUp:
	SAFE_RELEASE(pChildOut);
	return hr;
}

HRESULT CreateAndAddPINode(IXMLDOMDocument *pDom, PCWSTR wszTarget, PCWSTR wszData)
{
	HRESULT hr = S_OK;
	IXMLDOMProcessingInstruction *pPI = NULL;

	BSTR bstrTarget = SysAllocString(wszTarget);
	BSTR bstrData = SysAllocString(wszData);
	CHK_ALLOC(bstrTarget && bstrData);

	CHK_HR(pDom->createProcessingInstruction(bstrTarget, bstrData, &pPI));
	CHK_HR(AppendChildToParent(pPI, pDom));

CleanUp:
	SAFE_RELEASE(pPI);
	SysFreeString(bstrTarget);
	SysFreeString(bstrData);
	return hr;
}

HRESULT CreateAndAddCommentNode(IXMLDOMDocument *pDom, PCWSTR wszComment)
{
	HRESULT hr = S_OK;
	IXMLDOMComment *pComment = NULL;

	BSTR bstrComment = SysAllocString(wszComment);
	CHK_ALLOC(bstrComment);

	CHK_HR(pDom->createComment(bstrComment, &pComment));
	CHK_HR(AppendChildToParent(pComment, pDom));

CleanUp:
	SAFE_RELEASE(pComment);
	SysFreeString(bstrComment);
	return hr;
}

HRESULT CreateAndAddAttributeNode(IXMLDOMDocument *pDom, PCWSTR wszName, PCWSTR wszValue, IXMLDOMElement *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMAttribute *pAttribute = NULL;
	IXMLDOMAttribute *pAttributeOut = NULL;  // out param that is not used

	BSTR bstrName = NULL;
	VARIANT varValue;
	VariantInit(&varValue);

	bstrName = SysAllocString(wszName);
	CHK_ALLOC(bstrName);
	CHK_HR(VariantFromString(wszValue, varValue));

	CHK_HR(pDom->createAttribute(bstrName, &pAttribute));
	CHK_HR(pAttribute->put_value(varValue));
	CHK_HR(pParent->setAttributeNode(pAttribute, &pAttributeOut));

CleanUp:
	SAFE_RELEASE(pAttribute);
	SAFE_RELEASE(pAttributeOut);
	SysFreeString(bstrName);
	VariantClear(&varValue);
	return hr;
}

HRESULT CreateAndAddTextNode(IXMLDOMDocument *pDom, PCWSTR wszText, IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMText *pText = NULL;

	BSTR bstrText = SysAllocString(wszText);
	CHK_ALLOC(bstrText);

	CHK_HR(pDom->createTextNode(bstrText, &pText));
	CHK_HR(AppendChildToParent(pText, pParent));

CleanUp:
	SAFE_RELEASE(pText);
	SysFreeString(bstrText);
	return hr;
}

HRESULT CreateAndAddCDATANode(IXMLDOMDocument *pDom, PCWSTR wszCDATA, IXMLDOMNode *pParent)
{
	HRESULT hr = S_OK;
	IXMLDOMCDATASection *pCDATA = NULL;

	BSTR bstrCDATA = SysAllocString(wszCDATA);
	CHK_ALLOC(bstrCDATA);

	CHK_HR(pDom->createCDATASection(bstrCDATA, &pCDATA));
	CHK_HR(AppendChildToParent(pCDATA, pParent));

CleanUp:
	SAFE_RELEASE(pCDATA);
	SysFreeString(bstrCDATA);
	return hr;
}

HRESULT CreateAndAddElementNode(IXMLDOMDocument *pDom, PCWSTR wszName, PCWSTR wszNewLine,
	IXMLDOMNode *pParent, IXMLDOMElement **ppElement = NULL)
{
	HRESULT hr = S_OK;
	IXMLDOMElement *pElement = NULL;

	CHK_HR(CreateElement(pDom, wszName, &pElement));
	//  Add NEWLINE+TAB for indentation before this element.
	CHK_HR(CreateAndAddTextNode(pDom, wszNewLine, pParent));
	//  Append this element to parent
	CHK_HR(AppendChildToParent(pElement, pParent));

CleanUp:
	if (ppElement)
		*ppElement = pElement;  //  caller is responsible to release this element
	else
		SAFE_RELEASE(pElement);  //  caller is not interested in this element, so release it

	return hr;
}