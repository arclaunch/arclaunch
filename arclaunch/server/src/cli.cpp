#include "cli.hpp"

#include <CLI/CLI.hpp>

namespace cli
{
    int parse(args_t &parsed, int argc, char **argv)
    {

        CLI::App app{DESCRIPTION};

        argv = app.ensure_utf8(argv);

        // define options
        CLI::Option *cfOpt = app.add_option(PARAM_CF_OPTION, parsed.cert_file, PARAM_CF_DESCRIPTION);
        cfOpt->required()->group(GROUP_1_NAME);

        CLI::Option *kfOpt = app.add_option(PARAM_KF_OPTION, parsed.key_file, PARAM_KF_DESCRIPTION);
        kfOpt->required()->group(GROUP_1_NAME)->needs(cfOpt);

        // based on CLI11_PARSE macro
        CLI11_PARSE(app, argc, argv);

        // valid
        return 0;
    };
};