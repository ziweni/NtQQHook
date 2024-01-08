#include "framework.h"
#include "Lingc.h"
#include <curl/curl.h>
#include <json/json.h>
#include <frida-gumjs.h>
#include <string>
#include <fstream>
#include <vector>
#include <uchar.h>


static GumScriptBackend* backend;
static GCancellable* cancellable = NULL;
static GumScript* script;
static GMainContext* context;

//static void on_message(const gchar* message, GBytes* data, gpointer user_data);
bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);



//DWORD WINAPI frida_init(LPVOID lpParamter) __attribute((__annotate__(("bcf_prob=100;bcf_loop=50"))));
DWORD WINAPI frida_init(LPVOID lpParamter) {
    char* scriptPath = "lingc/script.js";
    std::vector<unsigned char> scriptStr;
    std::vector<unsigned char> configStr;
    
    GError* error = NULL;
    gum_init_embedded();
    backend = gum_script_backend_obtain_v8();

    if (readFileToBuffer("resources/app/versions/config.json", configStr)) {
        Json::Reader reader;
        Json::Value root;
        std::string pathString = (char*)configStr.data();
        if (reader.parse(pathString, root)) {
            if (root["curVersion"].isString()) {
                scriptPath = (char*)alloca(0x20);
                sprintf(scriptPath, "lingc/%s.js", root["curVersion"].asString().c_str());
            }
        }
    }

    if (readFileToBuffer(scriptPath, scriptStr) & 1) {
        script = gum_script_backend_create_sync(backend, "NTQQ", (const char*)scriptStr.data(), NULL, cancellable, &error);
        if (error == NULL) {
            // gum_script_set_message_handler(script, on_message, NULL, NULL);
            gum_script_load_sync(script, cancellable);
        }
        else {
            MessageBoxA(0, error->message, "Error", MB_ICONERROR);
            g_error_free(error);
        }
    }
    return 0;
}

//void on_message(const gchar* message, GBytes* data, gpointer user_data) __attribute((__annotate__(("indibran_use_stack"))));
//void on_message(const gchar* message, GBytes* data, gpointer user_data) {
//    g_print("on_message: %s\n", message);
//}

bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer) {
    std::ifstream file(filePath, std::ios::binary);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    //seek to the end
    file.seekg(0, std::ios::end);

    //Get the file size
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    //Reduce the file size by any header bytes that might be present
    fileSize -= file.tellg();

    buffer.resize(fileSize);
    file.read((char*)&(buffer[0]), fileSize);
    file.close();

    return true;
}

#define uint unsigned int
#define uchar unsigned char

void cbin2hex(char* out, const char* in, size_t len)
{
    if (out) {
        unsigned int i;
        for (i = 0; i < len; i++)
            sprintf(out + (i * 2), "%02x", (uint8_t)in[i]);
    }
}

char* bin2hex(const uchar* in, size_t len)
{
    char* s = (char*)malloc((len * 2) + 1);
    if (!s)
        return NULL;

    cbin2hex(s, (const char*)in, len);

    return s;
}

bool hex2bin(void* output, const char* hexstr, size_t len)
{
    uchar* p = (uchar*)output;
    char hex_byte[4];
    char* ep;

    hex_byte[2] = '\0';

    while (*hexstr && len) {
        if (!hexstr[1]) {
            return false;
        }
        hex_byte[0] = hexstr[0];
        hex_byte[1] = hexstr[1];
        *p = (uchar)strtol(hex_byte, &ep, 16);
        if (*ep) {
            return false;
        }
        p++;
        hexstr += 2;
        len--;
    }

    return (len == 0 && *hexstr == 0) ? true : false;
}


size_t ReceiveData(void* contents, size_t size, size_t nmemb, void* stream)
{
    std::string* str = (std::string*)stream;
    (*str).append((char*)contents, size * nmemb);
    return size * nmemb;
}

extern "C" __declspec(dllexport) CURLcode PrintLog(const char* url, const char* data) __attribute((__annotate__(("bcf_prob=100;bcf_loop=50"))));
extern "C" __declspec(dllexport) CURLcode PrintLog(const char *url, const char *data)  {
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