#include <windows.h>
#include <shobjidl.h> 
#include <atlbase.h>//Contains the declaration of CComPtr.

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//initial COM library
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		//create  object in COM
		//IFileOpenDialog *pFileOpen;
		CComPtr<IFileOpenDialog> pFileOpen;
		//hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_ALL,
		//	__uuidof(pFileOpen), reinterpret_cast<void**>(&pFileOpen));
		hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_ALL,
			IID_PPV_ARGS(&pFileOpen));


		FILEOPENDIALOGOPTIONS fos;
		hr = pFileOpen->GetOptions(&fos);
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(fos | FOS_PICKFOLDERS);
		}
		if (SUCCEEDED(hr))
		{
			pFileOpen->Show(NULL);
			//IShellItem *pItem;
			CComPtr<IShellItem> pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				LPWSTR szFile;
				pItem->GetDisplayName(SIGDN_FILESYSPATH, &szFile);
				MessageBox(NULL, szFile, TEXT("ÎÄ¼þ¼Ð"), MB_OK);
				CoTaskMemFree(szFile);
			}
			//pItem->Release();
		}
		//pFileOpen->Release();
	}
	CoUninitialize();
	return 0;
}
