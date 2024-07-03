#include "wildcatuserdriver.h"
#include <cstdio>

int main() {
    printf("User Driver: Starting...\n");
    WildcatUserDriver *driver = new WildcatUserDriver(true);

    delete driver;
}
