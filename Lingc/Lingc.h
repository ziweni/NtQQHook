#ifndef LINGC_H
#define LINGC_H

#include <Windows.h>
#include <frida-gumjs.h>
#include <string>
#include <vector>

static GumScriptBackend* backend;
static GCancellable* cancellable = NULL;
static GumScript* script;
static GMainContext* context;

DWORD WINAPI frida_init(LPVOID lpParamter);

#endif