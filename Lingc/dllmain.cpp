#include <Windows.h>
#include <curl/curl.h>
#include "Lingc.h"

#define LINGC_EXPORT __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, frida_init, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

size_t ReceiveData(void* contents, size_t size, size_t nmemb, std::string* str)
{
    str->append((char*)contents, size * nmemb);
    return size * nmemb;
}

extern "C" {
    /* 对外发送数据;用于提供给Js调用 */
    LINGC_EXPORT CURLcode PrintLog(const char* url, const char* data) {
        CURLcode res;
        CURL* curl = curl_easy_init();
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
        if (curl == NULL)
        {
            return CURLE_FAILED_INIT;
        }
        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 200);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReceiveData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return res;
    }
}