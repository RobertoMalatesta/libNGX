#include "HTTP/Mux/Router.h"
#include "HTTP/Mux/Service.h"
// Reference based service tree
// Hot reload router

class Mux {
protected:
    spin_lock Lock;
    // Router
    // ServiceDictionary
public:
    // Filter socket and create & config connection f(server, listening, socketfd, socketaddress)
    // Handle Request and Response f(server, connection, request, response)
};