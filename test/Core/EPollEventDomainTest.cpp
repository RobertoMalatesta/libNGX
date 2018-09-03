#include "Core/Core.h"

using namespace ngx::Core;

int EPollEventDomainTest() {

    EPollEventDomain Domain(40960, 4, 31723);


    return 0;
}