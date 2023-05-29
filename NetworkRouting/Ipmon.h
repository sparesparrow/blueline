#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <sys/un.h>
#include <sys/socket.h>
#include <thread>
#include <map>
#include "IpInterfacesManager.h"

struct addrs {
    std::vector<std::string> ipv4;
    std::vector<std::string> ipv6;
    std::vector<std::string> ipv4_net;
};

/*! The ipmon, of which only one instance should exist.
 *  Maybe will be transformed to singleon.
 */
class ipmon
{
public:
    enum class sockserver_cmd {
        update,
        reload,
        _size
    };
    const std::map<sockserver_cmd, const char*> sockserver_cmds {
        { sockserver_cmd::update, "update" },
        { sockserver_cmd::reload, "reload" },
    };
    //! Constructs the object
    ipmon();
    //! Destructs the object
    ~ipmon() {
        close(_netlink_fd);
        close(_socket_server_fd);
    }
    //! Prints a help message.
    void help();
    /*! Processes the cmdline arguments.
     * \param[in] argc from main()
     * \param[in] argv from main()*/
    void process_cmdline(int argc, char* argv[]);
    /*! Calls \c update() immediately if \c _opt_start is set. Should be called before \c run() */
    void start() {
        if (_opt_start) {
            update();
            _opt_start = false;
        }
    }
    /*! Starts monitoring netlink socket for address related messages. Upon message arrival,
     *  a timer with duration of \c _delay is set. After that time message is processed.
     *  Reading from socket continues, and if \c _opt_monitor is set, every message is parsed by \c parse_netlink_msg()
     *  Timer is reset in SIGALRM handling function \c update() */
    void run();
    /*! Prints information about network interfaces and addresses */
    void print();
    /*! \return get path to listening socket \c _socket_server_path */
    const auto& socket_server_path() { return _socket_server_path; }
    /*! \return get \c _socket_server_addr as a struct \ref sockaddr_un  */
    struct sockaddr_un& socket_server_addr() { return _socket_server_addr; }
    /*! \return get \c _socket_server_addr as a pointer to \ref sockaddr  */
    struct sockaddr* socket_server_addr_p() { return reinterpret_cast<struct sockaddr*>(&_socket_server_addr); }
    /*! \return whether the socket initialization at  \c _socket_server_path was successful or not.*/
    bool init_socket();
    /*! Unix socket server listening on \c _socket_server_path accepting requests defined in \c sockserver_cmds */
    void listen_socket();
    /*! Sends a message to unix domain socket listening on \ref _socket_server_addr.
     * \param[in] action message for the server
     * \return true on success, false otherwise */
    bool socket_action(sockserver_cmd action);
    /*! \param[in] ifname queried interface name
     *  \return whether the interface is loopback or not.*/
    bool is_iface_loopback(const std::string& ifname);
private:
    int _socket_server_fd;
    int _netlink_fd;
    struct sockaddr_un _socket_server_addr;
    //! Stored interface names and addresses
    std::unordered_map<std::string, std::shared_ptr<struct addrs>> _ifaces;
    static constexpr const char* _socket_server_path = "/run/ipmon.sock";
    //! Whether to flush and reload nftables configuration upon any address of any device had changed
    bool _opt_flush = false;
    //! Whether parse netlink messages and write various information to stdout
    bool _opt_monitor = false;
    //! Whether to update interface information on program start
    bool _opt_start = false;
    //! Time for which related messages should be ignored
    std::chrono::microseconds _delay {200'000};
    /*! Called from \c start() and after receiving IPv4/IPv6 address-related message from netlink.
     *  Obtains current interface information and updates served components. */
    void update();
    /*! Action performed based on \c sockserver_cmd::reload command coming from outside.
     *  Obtains current interface information and updates served components. Reloads the whole nft configuration */
    void reload();
    /*! Called from \c run() to parse netlink message for various information
     * \param[in] status message status
     * \param[in] buf message itself */
    void parse_netlink_msg(ssize_t status, struct nlmsghdr* buf);
    /*! Obtains information about network interfaces using getifaddrs()
     *  Interfaces with no IP addresses assigned will have 0.0.0.0 and ::
     *  Resets value of \c _ifaces */
    void get_if_addresses();
};
