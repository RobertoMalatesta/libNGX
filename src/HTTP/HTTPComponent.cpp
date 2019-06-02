#include <dlfcn.h>
#include <memory>
#include "HTTP/HTTPComponent.h"

using namespace ngx::HTTP;

const std::string Component::type = "component";
const std::string Mux::type = "mux";
const std::string Service::type = "service";
const std::string Service::name = "abstract";

Component::~Component() {
    if (h_dl) dlclose(h_dl);
}

Component *Component::loadDynamicComponent(std::string &file, std::string &sym) {
    void *h_dl, *pComp;
    loadComponentFn loadFn;
    Component *component;

    h_dl = dlopen(file.c_str(), RTLD_GLOBAL | RTLD_NOW);
    if (h_dl== nullptr) {
        return nullptr;
    }

    loadFn = reinterpret_cast<loadComponentFn >(dlsym(h_dl, sym.c_str()));
    if (loadFn == nullptr)
        return nullptr;

    pComp = loadFn();
    if (pComp == nullptr)
        return nullptr;
    component = static_cast<Component *>(pComp);
    component->h_dl = h_dl;
    return component;
}

Mux *Mux::loadDynamicMux(std::string &file) {
    Component *component;

    component = Component::loadDynamicComponent(file, const_cast<std::string &>(muxSym));
    if(component == nullptr)
        return nullptr;

    if (component->getType() != Mux::type) {
        delete component;
        return nullptr;
    }
    return dynamic_cast<Mux *>(component);
}

Service *Service::loadDynamicService(std::string &file) {
    Component *component;

    component = Component::loadDynamicComponent(file, const_cast<std::string &>(serviceSym));
    if (!component)
        return nullptr;

    if (component->getType() !=Service::type) {
        delete component;
        return nullptr;
    }
    return dynamic_cast<Service *>(component);
}
