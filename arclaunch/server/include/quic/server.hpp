#ifndef QUIC_SERVER_HPP_
#define QUIC_SERVER_HPP_

#include <event2/event.h>

#include "cli.hpp"

#include <quiche.h>

namespace quic
{
    class Server
    {
    private:
        cli::args_t *args;
        event_base *base;

        quiche_config *config = nullptr;

        static const size_t MAX_DATAGRAM_SIZE = 1350;
        static const char *ADDRESS;
        static const char *PORT;

        static void event_callback(evutil_socket_t sock, short what, void *arg);

        void handle(evutil_socket_t sock, short what);

    public:
        Server(cli::args_t *args, event_base *base);
        bool configure();
    };
}

#endif /* QUIC_SERVER_HPP_ */