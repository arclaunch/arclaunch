#include "quic/server.hpp"

#include <fcntl.h>
#include <netdb.h>

#include <iostream>
#include <string.h>
#include <unistd.h>  // read
#include <cinttypes> // PRIu64

namespace quic
{

    struct connections c;

    static struct connections *conns = nullptr;

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

        create_socket();

        init_quiche();

        // libevent2: create event
        struct event *event = event_new(base, sock, EV_READ | EV_PERSIST, event_callback, (void *)this);

        event_add(event, NULL); // no timeout

        return true;
    };

    void Server::create_socket()
    {
        const struct addrinfo hints = {
            .ai_family = PF_UNSPEC,
            .ai_socktype = SOCK_DGRAM,
            .ai_protocol = IPPROTO_UDP,
        };

        int port = atoi(PORT);

        if (port <= 0 || port > 65535)
        {
            std::wcout << "Invalid port\n";
            return;
        }

        // internet socket

        struct sockaddr_in sin;

        /* Clear the sockaddr before using it, in case there are extra
         * platform-specific fields that can mess us up. */
        memset(&sin, 0, sizeof(sin));
        /* This is an INET address */
        sin.sin_family = AF_INET;
        /* Listen on 0.0.0.0 */
        sin.sin_addr.s_addr = htonl(0);
        /* Listen on the given port. */
        sin.sin_port = htons(port);

        sock = socket(sin.sin_family, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            perror("failed to create socket");
            return;
        }

        // make socket non blocking
        if (evutil_make_socket_nonblocking(sock) < 0)
        {
            perror("failed to make socket non blocking");
            return;
        }

        if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
            perror("failed to connect socket");
            return;
        }

        c.sock = sock;
        c.h = nullptr;
        c.local_addr = (struct sockaddr *)&sin;
        c.local_addr_len = sizeof(sin);

        conns = &c;
    };

    void Server::init_quiche()
    {

        config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
        if (config == NULL)
        {
            fprintf(stderr, "failed to create config\n");
            return;
        }

        // TODO: handle nonexistent
        quiche_config_load_cert_chain_from_pem_file(config, args->cert_file.c_str());
        quiche_config_load_priv_key_from_pem_file(config, args->key_file.c_str());

        // quiche_config_set_application_protos(config, (uint8_t *)"arclaunch", 9);
        quiche_config_set_application_protos(config,
                                             (uint8_t *)"\x0ahq-interop\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 38);

        quiche_config_set_max_idle_timeout(config, 5000);
        quiche_config_set_max_recv_udp_payload_size(config, MAX_DATAGRAM_SIZE);
        quiche_config_set_max_send_udp_payload_size(config, MAX_DATAGRAM_SIZE);
        quiche_config_set_initial_max_data(config, 10000000);
        quiche_config_set_initial_max_stream_data_bidi_local(config, 1000000);
        quiche_config_set_initial_max_stream_data_bidi_remote(config, 1000000);
        quiche_config_set_initial_max_streams_bidi(config, 100);
        quiche_config_set_cc_algorithm(config, QUICHE_CC_RENO);
    };

    void
    Server::event_callback(evutil_socket_t sock, short what, void *arg)
    {
        Server *inst = (Server *)arg;
        inst->handle(sock, what);
    };

    void Server::mint_token(const uint8_t *dcid, size_t dcid_len,
                            struct sockaddr_storage *addr, socklen_t addr_len,
                            uint8_t *token, size_t *token_len)
    {
        memcpy(token, "quiche", sizeof("quiche") - 1);
        memcpy(token + sizeof("quiche") - 1, addr, addr_len);
        memcpy(token + sizeof("quiche") - 1 + addr_len, dcid, dcid_len);

        *token_len = sizeof("quiche") - 1 + addr_len + dcid_len;
    };

    bool Server::validate_token(const uint8_t *token, size_t token_len,
                                struct sockaddr_storage *addr, socklen_t addr_len,
                                uint8_t *odcid, size_t *odcid_len)
    {
        if ((token_len < sizeof("quiche") - 1) ||
            memcmp(token, "quiche", sizeof("quiche") - 1))
        {
            return false;
        }

        token += sizeof("quiche") - 1;
        token_len -= sizeof("quiche") - 1;

        if ((token_len < addr_len) || memcmp(token, addr, addr_len))
        {
            return false;
        }

        token += addr_len;
        token_len -= addr_len;

        if (*odcid_len < token_len)
        {
            return false;
        }

        memcpy(odcid, token, token_len);
        *odcid_len = token_len;

        return true;
    }

    uint8_t *Server::gen_cid(uint8_t *cid, size_t cid_len)
    {
        int rng = open("/dev/urandom", O_RDONLY);
        if (rng < 0)
        {
            perror("failed to open /dev/urandom");
            return NULL;
        }

        ssize_t rand_len = read(rng, cid, cid_len);
        if (rand_len < 0)
        {
            perror("failed to create connection ID");
            return NULL;
        }

        return cid;
    };

    struct conn_io *Server::create_conn(uint8_t *scid, size_t scid_len,
                                        uint8_t *odcid, size_t odcid_len,
                                        struct sockaddr *local_addr,
                                        socklen_t local_addr_len,
                                        struct sockaddr_storage *peer_addr,
                                        socklen_t peer_addr_len)
    {
        struct conn_io *conn_io = (struct conn_io *)calloc(1, sizeof(*conn_io));
        if (conn_io == NULL)
        {
            fprintf(stderr, "failed to allocate connection IO\n");
            return NULL;
        }

        if (scid_len != LOCAL_CONN_ID_LEN)
        {
            fprintf(stderr, "failed, scid length too short\n");
        }

        memcpy(conn_io->cid, scid, LOCAL_CONN_ID_LEN);

        quiche_conn *conn = quiche_accept(conn_io->cid, LOCAL_CONN_ID_LEN,
                                          odcid, odcid_len,
                                          local_addr,
                                          local_addr_len,
                                          (struct sockaddr *)peer_addr,
                                          peer_addr_len,
                                          config);

        if (conn == NULL)
        {
            fprintf(stderr, "failed to create connection\n");
            return NULL;
        }

        conn_io->sock = sock;
        conn_io->conn = conn;

        memcpy(&conn_io->peer_addr, peer_addr, peer_addr_len);
        conn_io->peer_addr_len = peer_addr_len;

        event_assign(&conn_io->timer, base, sock, EV_READ, timeout_cb, (void *)conn_io);

        HASH_ADD(hh, conns->h, cid, LOCAL_CONN_ID_LEN, conn_io);

        fprintf(stderr, "new connection\n");

        return conn_io;
    }

    void Server::handle(evutil_socket_t sock, short what)
    {
        // try reading from socket
        struct conn_io *tmp, *conn_io = nullptr;

        static uint8_t buf[65535];
        static uint8_t out[MAX_DATAGRAM_SIZE];

        while (1)
        {
            struct sockaddr_storage peer_addr;
            socklen_t peer_addr_len = sizeof(peer_addr);
            memset(&peer_addr, 0, peer_addr_len);

            ssize_t read = recvfrom(sock, buf, sizeof(buf), 0,
                                    (struct sockaddr *)&peer_addr,
                                    &peer_addr_len);

            if (read < 0)
            {
                if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
                {
                    fprintf(stderr, "recv would block\n");
                    break;
                }

                perror("failed to read");
                return;
            }

            // quiche
            uint8_t type;
            uint32_t version;

            uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
            size_t scid_len = sizeof(scid);

            uint8_t dcid[QUICHE_MAX_CONN_ID_LEN];
            size_t dcid_len = sizeof(dcid);

            uint8_t odcid[QUICHE_MAX_CONN_ID_LEN];
            size_t odcid_len = sizeof(odcid);

            uint8_t token[MAX_TOKEN_LEN];
            size_t token_len = sizeof(token);

            int rc = quiche_header_info(buf, read, LOCAL_CONN_ID_LEN, &version,
                                        &type, scid, &scid_len, dcid, &dcid_len,
                                        token, &token_len);
            if (rc < 0)
            {
                fprintf(stderr, "failed to parse header: %d\n", rc);
                continue;
            }

            HASH_FIND(hh, conns->h, dcid, dcid_len, conn_io);

            if (conn_io == NULL)
            {
                if (!quiche_version_is_supported(version))
                {
                    fprintf(stderr, "version negotiation\n");

                    ssize_t written = quiche_negotiate_version(scid, scid_len,
                                                               dcid, dcid_len,
                                                               out, sizeof(out));

                    if (written < 0)
                    {
                        fprintf(stderr, "failed to create vneg packet: %zd\n",
                                written);
                        continue;
                    }

                    ssize_t sent = sendto(conns->sock, out, written, 0,
                                          (struct sockaddr *)&peer_addr,
                                          peer_addr_len);
                    if (sent != written)
                    {
                        perror("failed to send");
                        continue;
                    }

                    fprintf(stderr, "sent %zd bytes\n", sent);
                    continue;
                }

                if (token_len == 0)
                {
                    fprintf(stderr, "stateless retry\n");

                    mint_token(dcid, dcid_len, &peer_addr, peer_addr_len,
                               token, &token_len);

                    uint8_t new_cid[LOCAL_CONN_ID_LEN];

                    if (gen_cid(new_cid, LOCAL_CONN_ID_LEN) == NULL)
                    {
                        continue;
                    }

                    ssize_t written = quiche_retry(scid, scid_len,
                                                   dcid, dcid_len,
                                                   new_cid, LOCAL_CONN_ID_LEN,
                                                   token, token_len,
                                                   version, out, sizeof(out));

                    if (written < 0)
                    {
                        fprintf(stderr, "failed to create retry packet: %zd\n",
                                written);
                        continue;
                    }

                    ssize_t sent = sendto(conns->sock, out, written, 0,
                                          (struct sockaddr *)&peer_addr,
                                          peer_addr_len);
                    if (sent != written)
                    {
                        perror("failed to send");
                        continue;
                    }

                    fprintf(stderr, "sent %zd bytes\n", sent);
                    continue;
                }

                if (!validate_token(token, token_len, &peer_addr, peer_addr_len,
                                    odcid, &odcid_len))
                {
                    fprintf(stderr, "invalid address validation token\n");
                    continue;
                }

                conn_io = create_conn(dcid, dcid_len, odcid, odcid_len,
                                      conns->local_addr, conns->local_addr_len,
                                      &peer_addr, peer_addr_len);

                if (conn_io == NULL)
                {
                    continue;
                }
            }

            quiche_recv_info recv_info = {
                (struct sockaddr *)&peer_addr,
                peer_addr_len,

                conns->local_addr,
                conns->local_addr_len,
            };

            ssize_t done = quiche_conn_recv(conn_io->conn, buf, read, &recv_info);

            if (done < 0)
            {
                fprintf(stderr, "failed to process packet: %zd\n", done);
                continue;
            }

            fprintf(stderr, "recv %zd bytes\n", done);

            if (quiche_conn_is_established(conn_io->conn))
            {
                uint64_t s = 0;

                quiche_stream_iter *readable = quiche_conn_readable(conn_io->conn);

                while (quiche_stream_iter_next(readable, &s))
                {
                    fprintf(stderr, "stream %" PRIu64 " is readable\n", s);

                    bool fin = false;
                    uint64_t error_code;
                    ssize_t recv_len = quiche_conn_stream_recv(conn_io->conn, s,
                                                               buf, sizeof(buf),
                                                               &fin, &error_code);
                    if (recv_len < 0)
                    {
                        break;
                    }

                    if (fin)
                    {
                        static const char *resp = "byez\n";
                        uint64_t error_code;
                        quiche_conn_stream_send(conn_io->conn, s, (uint8_t *)resp,
                                                5, true, &error_code);
                    }
                }

                quiche_stream_iter_free(readable);
            }
        };

        // established conns
        std::wcout << "hello" << std::endl;

        HASH_ITER(hh, conns->h, conn_io, tmp)
        {
            // flush_egress(loop, conn_io);

            if (quiche_conn_is_closed(conn_io->conn))
            {
                quiche_stats stats;
                quiche_path_stats path_stats;

                quiche_conn_stats(conn_io->conn, &stats);
                quiche_conn_path_stats(conn_io->conn, 0, &path_stats);

                fprintf(stderr, "connection closed, recv=%zu sent=%zu lost=%zu rtt=%" PRIu64 "ns cwnd=%zu\n",
                        stats.recv, stats.sent, stats.lost, path_stats.rtt, path_stats.cwnd);

                HASH_DELETE(hh, conns->h, conn_io);

                event_remove_timer(&conn_io->timer);
                quiche_conn_free(conn_io->conn);
                free(conn_io);
            }
        }
    };

    void Server::timeout_cb(evutil_socket_t sock, short what, void *data)
    {
        struct conn_io *conn_io = (struct conn_io *)data;
        quiche_conn_on_timeout(conn_io->conn);

        fprintf(stderr, "timeout\n");

        // flush_egress(loop, conn_io);

        if (quiche_conn_is_closed(conn_io->conn))
        {
            quiche_stats stats;
            quiche_path_stats path_stats;

            quiche_conn_stats(conn_io->conn, &stats);
            quiche_conn_path_stats(conn_io->conn, 0, &path_stats);

            fprintf(stderr, "connection closed, recv=%zu sent=%zu lost=%zu rtt=%" PRIu64 "ns cwnd=%zu\n",
                    stats.recv, stats.sent, stats.lost, path_stats.rtt, path_stats.cwnd);

            HASH_DELETE(hh, conns->h, conn_io);

            event_del(&conn_io->timer);
            quiche_conn_free(conn_io->conn);
            free(conn_io);

            return;
        }
    }
}