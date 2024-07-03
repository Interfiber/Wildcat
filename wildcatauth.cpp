#include "wildcatauth.h"
#include "SDL_messagebox.h"
#include <sys/types.h>
#include <unistd.h>

WildcatAuth::WildcatAuth() {}

bool WildcatAuth::isRoot() {
    return !getuid();
}

bool WildcatAuth::spawnAsRoot(const std::string &command) {
    if (std::system("which kdesu") == 0) {
        std::system(("kdesu -n -t --noignorebutton -c " + command).c_str());
    } else if (std::system("which pkexec") == 0) {
        std::system(("pkexec " + command).c_str());
    } else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Wildcat error", "Wildcat could not find a suitable polkit wrapper! Checked for: kdesu, and pkexec", nullptr);

        return false;
    }

    return true;
}
