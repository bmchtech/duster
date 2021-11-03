//
//  Wasm3 - high performance WebAssembly interpreter written in C.
//
//  Copyright © 2019 Steven Massey, Volodymyr Shymanskyy.
//  All rights reserved.
//

#include "wasm3.h"

#include "extra/fib32.wasm.h"

#define FATAL(func, msg) {           \
  tte_printf("#{P:12,140}#{ci:7}FATAL: %s", msg); \
  return; \
}

void run_wasm()
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)fib32_wasm;
    size_t fsize = fib32_wasm_len;

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

    IM3Function f;
    result = m3_FindFunction (&f, runtime, "fib");
    if (result) FATAL("m3_FindFunction", result);

    tte_printf("#{P:12,24}#{ci:7}Running");

    result = m3_CallV(f, 24);
    if (result) FATAL("m3_Call", result);

    uint32_t value = 0;
    result = m3_GetResultsV (f, &value);
    if (result) FATAL("m3_GetResults: %s", result);

    tte_printf("#{P:12,36}#{ci:7}Result: %lu", value);
}
