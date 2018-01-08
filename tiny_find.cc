#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <utility>
#include <cstdlib>
#include <exception>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

/* find files/dirs whose name match a given regex 
 *
 * linking flags: -lboost_system -bboost_program_options -lboost_filesystem
 *
 * usage:
 *  ./a.out --where .. --exclude ../a.out.dSYM -r ".*c"
 */

namespace sys_tools {

    class FindException
        : public std::exception 
    {
    public:
        FindException(std::string const &reason)
            : reason_(reason)
        {}

        const char *what() const noexcept {
            return reason_.c_str();
        }
    private:
        std::string reason_ = "Not specified.";
    };

    struct Requst {
        std::string                 where;
        std::string                 name_reg;
        std::vector<std::string>    to_excludes;
    };

    Requst
    parse_cmd(int argc, const char **argv)
    {
        namespace po = boost::program_options;
        po::options_description desc{"Find all files that match the regex"};

        try {
            desc.add_options()
                ("help,h", "Show this help")
                ("where,w", po::value<std::string>()->default_value("."),
                    "The starting dir to search")
                ("regex,r", po::value<std::string>()->default_value(".*"),
                    "The regex to match")
                ("exclude,e", po::value<std::vector<std::string>>()
                    ->default_value(std::vector<std::string>(), ""),
                    "The dir to be excluded")
                ;

            po::variables_map vm;
            po::store(po::parse_command_line(argc, argv, desc), vm);
    
            if ( vm.count("help") ) {
                std::cout << desc;
                std::exit(0);
            }

            return Requst {
                .where = vm["where"].as<std::string>(),
                .name_reg = vm["regex"].as<std::string>(),
                .to_excludes = std::move(vm["exclude"].as<std::vector<std::string>>()),
            };

        } catch (const po::error &e) {
            std::cerr << "Parse command line error: " << e.what() << "\n"
                      << "Usage: \n"
                      << desc 
                      ;
            std::exit(-1);
        }
    }

    // UnaryFunc: const char * -> auto 
    template<class UnaryFunc>
    void 
    do_find(const Requst &req, UnaryFunc func)
    {
        namespace fs = boost::filesystem;

        std::stack<fs::path>    paths_todo;
        std::vector<fs::path>   paths_to_exclude(req.to_excludes.size());
        std::regex reg;

        try {
            reg = req.name_reg;
        } catch ( const std::regex_error &e ) {
            throw FindException(std::string{"regex_error in "} + req.name_reg + ": " + e.what());
        }

        if ( !fs::is_directory(req.where) ) {
            throw FindException( req.where + " not a directory." );
        }

        std::copy(req.to_excludes.begin(), req.to_excludes.end(), paths_to_exclude.begin());

        paths_todo.push(req.where);
        std::cout << "Start finding regex: " << req.name_reg << " in " << req.where << ":\n";
        
        while ( !paths_todo.empty() ) {
            auto path_todo = paths_todo.top();
            paths_todo.pop();

            if ( std::find_if(paths_to_exclude.begin(), paths_to_exclude.end(), 
                    [&path_todo] (const fs::path &other) {
                        return fs::equivalent(path_todo, other);
                    }) != paths_to_exclude.end() ) {
                // should ignore this one
                continue;
            }

            if ( std::regex_match(path_todo.filename().c_str(), reg) ) {
                // this path satisfies the regex 
                func(path_todo.c_str());
            }

            if ( fs::is_directory(path_todo) ) {
                for ( auto const &p : fs::directory_iterator(path_todo) ) {
                    paths_todo.push(p);
                }
            }
        }
    }

} /* namespace sys_tools */

int
main(int argc, const char **argv)
{
    try {
        sys_tools::Requst req = sys_tools::parse_cmd(argc, argv);
        sys_tools::do_find(req, [] (const char *e) { std::cout << e << "\n"; });

    } catch ( const sys_tools::FindException &e ) {
        std::cerr << "Error: " << e.what() << "\n";
        std::exit(-1);
    }
}
