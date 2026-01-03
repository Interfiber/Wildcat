#include <Wildcat/driver/wrapper.h>
#include <stdexcept>
#include <cstring>

[[nodiscard]] std::string Wildcat_GetBootIDCookie(const std::string &file)
{
     // Read the 36-character boot ID

    FILE* bootIDFile = fopen(file.c_str(), "r");

    if (bootIDFile == nullptr)
    {
        throw std::runtime_error("Could not find boot ID at " + file + ": " + std::string(strerror(errno)));
    }

    char* bootID = (char* ) malloc(36 * sizeof(char));

    fread(bootID, 36 * sizeof(char), 1, bootIDFile);
    fclose(bootIDFile);

    auto result = std::string(bootID);

    free(bootID);

    return result;
}


