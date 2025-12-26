//
// Created by hstasonis on 12/25/25.
//

#include "Wildcat/driver/environ.h"
#include <unistd.h>

void WildcatElevatable::elevate(WildcatElevatable* pElevatable)
{
    if (getuid() != 0)
    {
        return pElevatable->onElevationFailure();
    }

    return pElevatable->onElevationComplete();
}