//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "dusk.h"
#include "wasm3.h"

#include "scene/dlang/bean_dlang.wasm.h"

int liney = 96;

#define FATAL(func, msg) {           \
  tte_printf("#{P:12,%d}#{ci:7}FATAL: %s", liney, msg); \
  liney += 12; \
  return; \
}

m3ApiRawFunction(bean_printi) {
    m3ApiGetArg(int32_t, i);
    tte_erase_screen();
    tte_printf("#{P:12,140}#{ci:7}%li", i);
    m3ApiSuccess();
}


m3ApiRawFunction(bean_keydown) {
    m3ApiReturnType(int32_t);
    m3ApiReturn(key_is_down(KEY_A) ? 1 : 0);
}


IM3Function f_bean_start;
IM3Function f_bean_update;
M3Result result;
char err_msg[64];

void wasm_bean_init()
{
    result = m3Err_none;

    uint8_t* wasm = (uint8_t*)wasm_src;
    size_t fsize = wasm_src_len;

    tte_printf("#{P:12,12}#{ci:7}Loading Wasm");

    IM3Environment env = m3_NewEnvironment ();
    if (!env) FATAL("m3_NewEnvironment", "failed");

    IM3Runtime runtime = m3_NewRuntime (env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime", "failed");

    IM3Module module;
    result = m3_ParseModule (env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule", "bean");

    result = m3_LoadModule (runtime, module);
    if (result) FATAL("m3_LoadModule", "bean");

    result = m3_LinkRawFunction(module, "env", "bean_printi", "v(i)", &bean_printi);
    if (result) FATAL("m3_LinkRawFunction", "couldn't link bean_printi()");

    result = m3_LinkRawFunction(module, "env", "bean_keydown", "i()", &bean_keydown);
    if (result) FATAL("m3_LinkRawFunction", "couldn't link bean_keydown()");

    result = m3_FindFunction (&f_bean_start, runtime, "bean_start");
    if (result) FATAL("m3_FindFunction", "couldn't link bean_start()");

    result = m3_FindFunction (&f_bean_update, runtime, "bean_update");
    if (result) FATAL("m3_FindFunction", "couldn't link bean_update()");

    tte_printf("#{P:12,24}#{ci:7}Running");
}

void wasm_bean_start() {
    result = m3_CallV(f_bean_start);
    if (result) {
        sprintf(err_msg, "error during bean_start(): %s", result);
        FATAL("m3_CallV", err_msg);
    }
}

void wasm_bean_update() {
    result = m3_CallV(f_bean_update);
    if (result) FATAL("m3_CallV", "error during bean_update()");
}
