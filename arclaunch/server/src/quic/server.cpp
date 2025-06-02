#include "quic/server.hpp"

#include <fcntl.h>
#include <netdb.h>

#include <iostream>

namespace quic
{

    const char *Server::ADDRESS = "0";
    const char *Server::PORT = "40311";

    Server::Server(cli::args_t *args, event_base *base)
    {
        std::wcout << "Server constructor called" << std::endl;
        this->args = args;
        this->base = base;
    };

    bool Server::configure()
    {
        const struct addrinfo hints = {
            .ai_family = PF_UNSPEC,
            .ai_socktype = SOCK_DGRAM,
            .ai_protocol = IPPROTO_UDP,
        };

        struct addrinfo *local;
        if (getaddrinfo(ADDRESS, PORT, &hints, &local) != 0)
        {
            perror("failed to resolve host");
            return false;
        }

        int sock = socket(local->ai_family, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            perror("failed to create socket");
            return false;
        }

        if (fcntl(sock, F_SETFL, O_NONBLOCK) != 0)
        {
            perror("failed to make socket non-blocking");
            return false;
        }

        if (bind(sock, local->ai_addr, local->ai_addrlen) < 0)
        {
            perror("failed to connect socket");
            return false;
        }

        config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
        if (config == NULL)
        {
            fprintf(stderr, "failed to create config\n");
            return false;
        }

        quiche_config_load_cert_chain_from_pem_file(config, args->cert_file.c_str());
        quiche_config_load_priv_key_from_pem_file(config, args->key_file.c_str());

        quiche_config_set_application_protos(config, (uint8_t *)"arclaunch", 9);

        quiche_config_set_max_idle_timeout(config, 5000);
        quiche_config_set_max_recv_udp_payload_size(config, MAX_DATAGRAM_SIZE);
        quiche_config_set_max_send_udp_payload_size(config, MAX_DATAGRAM_SIZE);
        quiche_config_set_initial_max_data(config, 10000000);
        quiche_config_set_initial_max_stream_data_bidi_local(config, 1000000);
        quiche_config_set_initial_max_stream_data_bidi_remote(config, 1000000);
        quiche_config_set_initial_max_streams_bidi(config, 100);
        quiche_config_set_cc_algorithm(config, QUICHE_CC_RENO);

        // libevent2: create event
        struct event *event = event_new(base, sock, EV_READ | EV_PERSIST, event_callback, (void *)this);

        event_add(event, NULL); // no timeout

        return true;
    };

    void Server::event_callback(evutil_socket_t sock, short what, void *arg)
    {
        Server *inst = (Server *)arg;
        inst->handle(sock, what);
    };

    void Server::handle(evutil_socket_t sock, short what) {

    };
}