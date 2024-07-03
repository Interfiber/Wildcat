#ifndef WILDCATAUTH_H
#define WILDCATAUTH_H

#include <string>
class WildcatAuth
{
public:
    WildcatAuth();

    /**
     * @brief Check if the user is root
     * @return true if the user is root, false if otherwise
    */
    [[nodiscard]] static bool isRoot();

    /**
     * @brief Spawn the given command as root
     * @param command
     * @return true if the command was spawned, false if otherwise
     */
    static bool spawnAsRoot(const std::string &command);
};

#endif // WILDCATAUTH_H
