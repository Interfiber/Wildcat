//
// Created by hstasonis on 12/25/25.
//

#include "Wildcat/driver/driver.h"
#include "Wildcat/driver/environ.h"

int main(int argc, char **argv)
{
    WildcatElevatable::elevate(new WildcatDriver());
}
