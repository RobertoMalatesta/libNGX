/**
 *  @name Instance
 *
 *  @brief Abstract module instance type.
 * */
class Instance {

protected:
    /**Module name**/
    const char *ModuleName;
    /**Magic to identify module**/
    uint32_t MajorMagic;
    /**Magic to identify module detail**/
    uint32_t MinorMagic;
}

/**
 *  @name Module
 *
 *  @brief Module type.
 * */
class Moudle : DictionatyItem {

protected:
    /**Module name**/
    const char *Name;
    /**Module version**/
    uint32_t Version;
    /**Module author**/
    const char *Author;
    /**Module config manual**/
    const char *Manual;

public:
    /**Create a instance of this module**/
    RuntimeError Create(Instance &I) = 0;

    /**Config a instance of this module**/
    RuntimeError Config(Instance &I, Dictionaty &C) = 0;

    /**Destory a instance of this module**/
    RuntimeError Destroy(Instance &I) = 0;

    /**If this instance create from this module**/
    bool IsModuleInstance(Instance &I) = 0;
}

/**
 *  @name ModuleRegistry
 *
 *  @brief Module Registry to manage modules.
 * */
class MoudleRegistry : Dictionaty {

protected:
    /**Working directory to load dynamic load library**/
    char *WorkingDirectory = nullptr;

public:
    /**Register a known module type to this registry**/
    RuntimeError RegistModule(Module &M);

    /** Register a new dynamic load module to this registry**/
    RuntimeError RegistDynamicModule(char *ModuleName);

    /**Remove a module type from this registry**/
    RuntimeError UnregistModule(char *ModuleName);
}
// TODO 1
// HTTP Module Instance Map
//
// HTTP Server Module, Server Control Connection, HttpDns, PerfMon
// HTTP PreponeFilter: firewall, request mirror
// HTTP Mux: request route, add header, request limit, dynamic routing
// HTTP Service: static file cache, upstream, grpc and so on
// HTTP PostponeFilter: reslove mime, calculate content size, deny response

// TODO 2
// Move Timer from Socket to EvnetEntity