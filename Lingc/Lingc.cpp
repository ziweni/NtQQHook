#include "Lingc.h"
#include "util.h"
#include <curl/curl.h>
#include <json/json.h>
#include <Windows.h>
#include <string>
#include <vector>


DWORD WINAPI frida_init(LPVOID lpParamter) {
    
    std::vector<unsigned char> scriptStr;
    std::vector<unsigned char> configStr;
    char scriptPath[255] = { 0 };
    
    GError* error = NULL;
    gum_init_embedded();
    backend = gum_script_backend_obtain_v8();

    if (util::readFile("resources/app/versions/config.json", configStr)) {
        Json::Reader reader;
        Json::Value root;
        std::string pathString = (char*)configStr.data();
        if (reader.parse(pathString, root)) {
            if (root["curVersion"].isString()) {
                sprintf(scriptPath, "lingc/%s.js", root["curVersion"].asString().c_str());
            }
        }
    }

    if (util::readFile(scriptPath, scriptStr) & 1) {
        script = gum_script_backend_create_sync(backend, "NTQQ", (const char*)scriptStr.data(), NULL, cancellable, &error);
        if (error == NULL) {
            gum_script_load_sync(script, cancellable);
        }
        else {
            MessageBoxA(0, error->message, "Error", MB_ICONERROR);
            g_error_free(error);
        }
    }
    return 0;
}
