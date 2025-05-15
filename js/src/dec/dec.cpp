#include "jsapi.h"
#include <iostream>
#include <fstream>
#include <vector>

// Função para carregar bytecode
std::vector<uint8_t> readFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " arquivo.jsc" << std::endl;
        return 1;
    }

    JSRuntime* rt = JS_NewRuntime(8L * 1024 * 1024);  // 8MB
    if (!rt) return 1;

    JSContext* cx = JS_NewContext(rt, 8192);
    if (!cx) return 1;

    JS_BeginRequest(cx);

    JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_METHODJIT | JSOPTION_TYPE_INFERENCE);
    JS_SetVersion(cx, JSVERSION_LATEST);

    JSObject* global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
    if (!global) return 1;

    JSAutoEnterCompartment ac;
    if (!ac.enter(cx, global)) return 1;

    JS_InitStandardClasses(cx, global);

    // Ler bytecode
    std::vector<uint8_t> data = readFile(argv[1]);
    if (data.empty()) {
        std::cerr << "Erro ao abrir arquivo.\n";
        return 1;
    }

    const uint8_t* buf = data.data();
    JSObject* scriptObj = NULL;

    // Deserializa script
    JSScript* script = JS_DecodeScript(cx, buf, data.size(), NULL);
    if (!script) {
        std::cerr << "Falha ao decodificar bytecode.\n";
        return 1;
    }

    // Decompila
    JSString* source = JS_DecompileScript(cx, script, "decompiled", 0);
    if (source) {
        char* utf8 = JS_EncodeString(cx, source);
        if (utf8) {
            std::cout << utf8 << std::endl;
            JS_free(cx, utf8);
        }
    } else {
        std::cerr << "Falha ao decompilar o script.\n";
    }

    JS_EndRequest(cx);
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();

    return 0;
}

// Classe global padrão mínima
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};
