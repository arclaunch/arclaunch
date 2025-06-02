#ifndef QUIC_SERVER_HPP_
#define QUIC_SERVER_HPP_

#include <event2/event.h>

#include "cli.hpp"

#include <uthash.h>

#include <quiche.h>
#include <event2/event_struct.h>

#define MAX_TOKEN_LEN                     \
    sizeof("quiche") - 1 +                \
        sizeof(struct sockaddr_storage) + \
        QUICHE_MAX_CONN_ID_LEN

namespace quic
{
    static const size_t LOCAL_CONN_ID_LEN = 16;

    struct connections
    {
        int sock;

        struct sockaddr *local_addr = NULL;
        socklen_t local_addr_len;

        struct conn_io *h;
    };

    struct conn_io
    {
        event timer;

        int sock;

        uint8_t cid[LOCAL_CONN_ID_LEN];

        quiche_conn *conn;

        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len;

        UT_hash_handle hh;
    };

    class Server
    {
    private:
        cli::args_t *args;
        event_base *base;

        // socket
        int sock;

        quiche_config *config = nullptr;

        static const size_t MAX_DATAGRAM_SIZE = 1350;
        static const char *ADDRESS;
        static const char *PORT;

        static void event_callback(evutil_socket_t sock, short what, void *arg);
        void handle(evutil_socket_t sock, short what);

        static void timeout_cb(evutil_socket_t sock, short what, void *data);

        void create_socket();
        void init_quiche();

        // from quiche examples (could be static)
        void mint_token(const uint8_t *dcid, size_t dcid_len,
                        struct sockaddr_storage *addr, socklen_t addr_len,
                        uint8_t *token, size_t *token_len);

        bool validate_token(const uint8_t *token, size_t token_len,
                            struct sockaddr_storage *addr, socklen_t addr_len,
                            uint8_t *odcid, size_t *odcid_len);

        uint8_t *gen_cid(uint8_t *cid, size_t cid_len);

        struct conn_io *create_conn(uint8_t *scid, size_t scid_len,
                                    uint8_t *odcid, size_t odcid_len,
                                    struct sockaddr *local_addr,
                                    socklen_t local_addr_len,
                                    struct sockaddr_storage *peer_addr,
                                    socklen_t peer_addr_len);

        // end quiche examples

    public:
        Server(cli::args_t *args, event_base *base);
        bool configure();
    };
}

#endif /* QUIC_SERVER_HPP_ */