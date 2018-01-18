#include <sys/param.h>
#include <sys/mount.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <exception>

/*
 * ./a.out filename 
 * show the filesystem name a file belonging to 
 */

class LowLevelError
    : public std::exception
{
public:
    LowLevelError(std::string const &msg)
        : msg_(msg)
    {}

    const char *what() const noexcept {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

std::string
get_fs_name(const std::string &filename) 
{
    struct statfs buf;

    if ( statfs(filename.c_str(), &buf) != 0 ) {
        throw LowLevelError{strerror(errno)};
    }

    return std::string(buf.f_fstypename);
}

void
usage()
{
    std::cout << "Usage: ./a.out filename\n";
}

int
main(int argc, char **argv)
{
    if ( argc != 2 ) {
        usage();
        return -1;
    }
    try {
        std::string fsname = get_fs_name(argv[1]);
        std::cout << "FS: " << fsname << "\n";
    } catch ( const LowLevelError &e ) {
        std::cout << "Error: " << e.what() << "\n";
    }
}
