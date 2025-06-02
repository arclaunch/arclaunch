#ifndef INCLUDE_CLI_H_
#define INCLUDE_CLI_H_

#include <string>

namespace cli
{
    const std::string DESCRIPTION = "Server binary";

    const std::string GROUP_1_NAME = "QUIC";

    const std::string PARAM_CF_OPTION = "--cert-file";
    const std::string PARAM_CF_DESCRIPTION = "Path to server certificate.";
    const std::string PARAM_KF_OPTION = "--key-file";
    const std::string PARAM_KF_DESCRIPTION = "Path to server key.";

    typedef struct
    {
        std::string key_file;
        std::string cert_file;
    } args_t;

    int parse(args_t &parsed, int argc, char **argv);
};

#endif /* INCLUDE_CLI_H_ */