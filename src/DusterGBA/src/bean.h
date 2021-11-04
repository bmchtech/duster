//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "dusk.h"
#include "wasm3.h"

#include "scn/bean.wasm.h"

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

void wasm_bean_init()
{
    result = m3Err_none;

    uint8_t* wasm = (uint8_t*)bean_wasm;
    size_t fsize = bean_wasm_len;

    tte_printf("#{P:12,12}#{ci:7}Loading Wasm");

    IM3Environment env = m3_NewEnvironment ();
    if (!env) FATAL("m3_NewEnvironment", "failed");

    IM3Runtime runtime = m3_NewRuntime (env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime", "failed");

    IM3Module module;
    result = m3_ParseModule (env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule", result);

    result = m3_LoadModule (runtime, module);
    if (result) FATAL("m3_LoadModule", result);

    result = m3_LinkRawFunction(module, "duster", "bean_printi", "v(i)", &bean_printi);
    if (result) FATAL("couldn't link bean_printi()", result);

    result = m3_LinkRawFunction(module, "duster", "bean_keydown", "i()", &bean_keydown);
    if (result) FATAL("couldn't link bean_keydown()", result);

    result = m3_FindFunction (&f_bean_start, runtime, "bean_start");
    if (result) FATAL("couldn't find bean_start()", result);

    result = m3_FindFunction (&f_bean_update, runtime, "bean_update");
    if (result) FATAL("couldn't find bean_update()", result);

    tte_printf("#{P:12,24}#{ci:7}Running");

    result = m3_CallV(f_bean_start);
    if (result) FATAL("m3_Call", result);

    //result = m3_GetResultsV (f, &value);
    //if (result) FATAL("m3_GetResults: %s", result);

    tte_printf("#{P:12,36}#{ci:7}Finished bean_start()");
}

void wasm_bean_start() {
    result = m3_CallV(f_bean_start);
    if (result) FATAL("m3_Call", result);
}

void wasm_bean_update() {
    result = m3_CallV(f_bean_update);
    if (result) FATAL("m3_Call", result);
}
