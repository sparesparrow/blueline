#include <string>
#include <cstdlib>
#include <sysexits.h>
#include <cstdio>
#include <iomanip>
#include <errno.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <chrono>
#include <fstream>
#include <signal.h>
#include <sys/time.h>
#include <getopt.h>
#include <optional>
#include <algorithm>
#include "Logger.h"
#include "Ipmon.h"


void ipmon::help()
{
    std::cout << R"( Usage: ipmon [-n[FILE] | --nftables[=FILE] [-f | --flush]]
                                  [-d DELAY | --delay=DELAY] [-m | --monitor]
                                  [-s | --start] [-h | --help]

    -h, --help         ... Displays a help message.
    -f, --flush        ... Used with -n, flushes the nftables ruleset and reloads them
    -d, --delay        ... Delay for updating the changes, 1-99 seconds or 100-999999 microseconds, defaults to 200000
    -m, --monitor      ... Monitors the netlink for address related events, writing them to stdout
    -s, --start        ... Updates addresses at program start

    Monitors interfaces for changes on IP addresses. When address is assigned or removed,
    the current addresses assigned to interfaces are updated to configuration files
    of programs specified in command line options, and the running instances of the programs are forced to
    reload the configuration files..

    After ipmon is started, configuration files are created with current addresses.

    Parameter --delay specifies the duration between event occured (address changed) and when it is updated.
    Multiple such events usually occur in a row, in which case the update should happen only once.
    Increase this delay if you find ipmon updating configuration multiple times in a very short time.
)" << std::endl;
}


ipmon::ipmon()
{
    _socket_server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (_socket_server_fd == -1) {
        std::cerr << "SOCKET ERROR: " <<  strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&_socket_server_addr, 0, sizeof(struct sockaddr_un));
    _socket_server_addr.sun_family = AF_UNIX;
    strcpy(_socket_server_addr.sun_path, _socket_server_path);
    int len = sizeof(_socket_server_addr);
    unlink(_socket_server_path);
    int rc = bind(_socket_server_fd, reinterpret_cast<struct sockaddr*> (&_socket_server_addr), len);
    if (rc == -1) {
        std::cerr << "BIND ERROR: " <<  strerror(errno) << std::endl;
        close(_socket_server_fd);
        exit(EXIT_FAILURE);
    }
}

bool ipmon::init_socket()
{
    return true;
}

void ipmon::listen_socket()
{
    while (true)
    {
        struct sockaddr_un peer_sock;
        memset(&peer_sock, 0, sizeof(struct sockaddr_un));
        socklen_t peer_len = sizeof(peer_sock);
        char buf[256];
        const auto& peer_sock_p = reinterpret_cast<struct sockaddr*> (&peer_sock);
        if (auto bytes_read = recvfrom(_socket_server_fd, buf, 256, MSG_WAITALL, peer_sock_p, &peer_len);
            bytes_read != -1)
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            if (auto buf_view = std::string_view(buf, bytes_read);
                buf_view == sockserver_cmds.at(sockserver_cmd::reload) ) {
                reload();
                if (sendto(_socket_server_fd, nullptr, 0, 0, peer_sock_p, sizeof(peer_sock)) == -1) {
                    Logger("Listening socket reply error(sendto): ");
                }
            } else if (buf_view == sockserver_cmds.at(sockserver_cmd::update) ) {
                update();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(250'000));
        } else if (errno !=  EAGAIN && errno != EWOULDBLOCK) {
            Logger("Listening socket error(recvfrom): ");
        }
    }
}

void ipmon::process_cmdline(int argc, char* argv[])
{
    struct option long_options[] =
    {
        {"help",     no_argument,       nullptr, 'h'},
        {"flush",    no_argument,       nullptr, 'f'},
        {"delay",    required_argument, nullptr, 'd'},
        {"monitor",  no_argument,       nullptr, 'm'},
        {"start",    no_argument,       nullptr, 's'},
        { nullptr, 0, nullptr, 0 }
    };
    static constexpr const char* short_options = "h::f::d:ms";
    int c, option_index;
    bool only_root = false;
    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
    {
        switch (c) {
        case 'h':
            help();
            exit(EXIT_SUCCESS);
        case 'f':
            _opt_flush = true;
            break;
        case 'd':
        {
            auto delay = std::stol(optarg);
            if (delay <= 0 || delay > 999999) {
                std::cerr << "Invalid value for parameter --delay: " << delay << std::endl;
                help();
                exit(EX_USAGE);
            } else if (delay < 100) {
                delay *= 1'000'000;
            }
            _delay = std::chrono::microseconds {delay};
            break;
        }
        case 'm':
            _opt_monitor = true;
            break;
        case 's':
            _opt_start = true;
            break;
        default:
            help();
            exit(EX_USAGE);
        }
    }
    if (!_opt_nftables && _opt_flush) {
            help();
            exit(EX_USAGE);
    }
    if (only_root && geteuid() != 0) {
        std::cerr << "You must be root." << std::endl;
        exit(EX_USAGE);
    }
}

void ipmon::update()
{
    get_if_addresses();
    if (_opt_nftables)
        tell_nftables();
    if (_opt_monitor)
        print();
}

void ipmon::reload()
{
    get_if_addresses();
    if (_opt_nftables)
        tell_nftables();
    if (_opt_monitor)
        print();
}

void ipmon::print()
{
    for (auto p = _ifaces.begin(); p != _ifaces.end(); p++) {
        std::cout << "Interface: " << p->first << " IPv4: ";
        for (auto& vec : p->second->ipv4)
            std::cout <<  vec << " ";
        std::cout << "IPv4_networks: ";
        for (auto& vec : p->second->ipv4_net)
            std::cout <<  vec << " ";
        std::cout << "IPv6: ";
        for (auto& vec : p->second->ipv6)
            std::cout <<  vec << " ";
        std::cout << std::endl;
    }
}

void ipmon::run()
{
    _netlink_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    while (_netlink_fd < 0) {
        Logger("Failed to create netlink socket: ");
        std::this_thread::sleep_for(std::chrono::microseconds(250'000));
        _netlink_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    }
    char buf[16384];
    struct iovec iov;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    struct sockaddr_nl  local;
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups = RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR;
    local.nl_pid = getpid();
    struct msghdr msg =
    {
        msg.msg_name = &local,
        msg.msg_namelen = sizeof(local),
        msg.msg_iov = &iov,
        msg.msg_iovlen = 1,
        msg.msg_control = nullptr,
        msg.msg_controllen = 0,
        msg.msg_flags = 0,
    };
    while (bind(_netlink_fd, reinterpret_cast<struct sockaddr*>(&local), sizeof(local)) < 0) {
        Logger("Failed to bind netlink socket.");
    }
    bool timer_ticking = false;
    std::chrono::_V2::steady_clock::time_point timer_delay;
    while (true)
    {
        ssize_t status = recvmsg(_netlink_fd, &msg, MSG_DONTWAIT);
        if (timer_ticking)
        {
            auto time_left = std::chrono::duration_cast<std::chrono::microseconds>
                            (std::chrono::steady_clock::now() - timer_delay).count();
            if (time_left >= _delay.count())
            {
                timer_ticking = false;
                socket_action(sockserver_cmd::update);
            }
        }
        if (status < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::microseconds(250'000));
                continue;
            }
            Logger("Error: netlink receive error: ");
            continue;
        } else if (status == 0) {
            Logger("Error: EOF on netlink.");
            continue;
        } else if (msg.msg_namelen != sizeof(local)) {
            Logger("Error: Invalid netlink sender address length = " + std::to_string(msg.msg_namelen));
            continue;
        } else {
            if (_opt_monitor)
                    parse_netlink_msg(status, reinterpret_cast<struct nlmsghdr*>(buf));
            if (!timer_ticking)
            {
                timer_ticking = true;
                timer_delay = std::chrono::steady_clock::now();
                continue;
            }
        }
    }
}

void ipmon::parse_netlink_msg(ssize_t status, struct nlmsghdr* buf)
{
    struct nlmsghdr *h;
    for (h = buf; status >= static_cast<ssize_t>(sizeof(*h)); ) {
        int len = h->nlmsg_len;
        int l = len - sizeof(*h);
        if ((l < 0) || (len > status)) {
            std::cerr << "Error: Invalid message length: " << len << std::endl;
            break;
        }
        struct ifaddrmsg *ifa;
        struct rtattr *tba[IFA_MAX+1];
        ifa = static_cast<struct ifaddrmsg*>(NLMSG_DATA(h));
        auto rta = IFA_RTA(ifa);
        memset(tba, 0, sizeof(struct rtattr *) * (IFA_MAX + 1));
        while (RTA_OK(rta, h->nlmsg_len)) {
            if (rta->rta_type <= IFA_MAX) {
                tba[rta->rta_type] = rta;
            }
            rta = RTA_NEXT(rta,h->nlmsg_len);
        }
        char if_addr[256];
        char if_name[IFNAMSIZ];
        if (ifa->ifa_index == 0 || if_indextoname(ifa->ifa_index, if_name) == nullptr) {
            std::cerr << "Error: No interface name." << std::endl;
            status -= NLMSG_ALIGN(len);
            h = reinterpret_cast<struct nlmsghdr *>(reinterpret_cast<char *>(h) + NLMSG_ALIGN(len));
            continue;
        }
        if (ifa->ifa_family == AF_INET) {
            if (!tba[IFA_ADDRESS]) {
                if (!tba[IFA_LOCAL]) {
                    std::cerr << "Error: No address obtained for interface " << if_name << std:: endl;
                    status -= NLMSG_ALIGN(len);
                    h = reinterpret_cast<struct nlmsghdr *>(reinterpret_cast<char *>(h) + NLMSG_ALIGN(len));
                    continue;
                }
                inet_ntop(AF_INET, RTA_DATA(tba[IFA_LOCAL]), if_addr, sizeof(if_addr));
            }
            inet_ntop(AF_INET, RTA_DATA(tba[IFA_ADDRESS]), if_addr, sizeof(if_addr));
        } else {            // AF_INET6
            if (!tba[IFA_ADDRESS]) {
                std::cerr << "Error: No address obtained for interface " << if_name << std::endl;
                status -= NLMSG_ALIGN(len);
                h = reinterpret_cast<struct nlmsghdr *>(reinterpret_cast<char *>(h) + NLMSG_ALIGN(len));
                continue;
            }
            inet_ntop(AF_INET6, RTA_DATA(tba[IFA_ADDRESS]), if_addr, sizeof(if_addr));
        }
        switch (h->nlmsg_type) {
            case RTM_NEWADDR:
                std::cout << "[NETLINK]: New address assigned to interface "
                                << if_name << ": " << if_addr << std:: endl;
                break;
            case RTM_DELADDR:
                std::cout << "[NETLINK]: Address was removed from interface "
                                << if_name << ": " << if_addr << std:: endl;
                break;
        }
        status -= NLMSG_ALIGN(len);
        h = reinterpret_cast<struct nlmsghdr *>(reinterpret_cast<char *>(h) + NLMSG_ALIGN(len));
    }
}

std::string network_addr_str(in_addr_t addr, in_addr_t mask) {
    char netAddrBuffer[INET_ADDRSTRLEN];
    auto addr_and_mask = addr & mask;
    inet_ntop(
        AF_INET,
        &addr_and_mask,
        netAddrBuffer,
        INET_ADDRSTRLEN);
    unsigned int count = 0;
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    return std::string(netAddrBuffer) + "/" + std::to_string(count);
}

void ipmon::get_if_addresses()
{
    struct ifaddrs * interface_list = nullptr;
    struct ifaddrs * ifa = nullptr;

    getifaddrs(&interface_list);
    _ifaces.clear();
    std::vector<char*> tmp_ifaces{};
    for (ifa = interface_list; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_name) {
            tmp_ifaces.emplace_back(ifa->ifa_name);
        }
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) {
            auto binary_addr_ptr= &reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr)->sin_addr;
            auto binary_mask_ptr = &reinterpret_cast<struct sockaddr_in *>(ifa->ifa_netmask)->sin_addr;
            auto net_addr_s = network_addr_str(binary_addr_ptr->s_addr, binary_mask_ptr->s_addr);
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, binary_addr_ptr, addressBuffer, INET_ADDRSTRLEN);
            if (auto find_it = _ifaces.find(ifa->ifa_name); find_it != _ifaces.end()) {
                find_it->second->ipv4.emplace_back(std::string(addressBuffer));
                auto& nets = find_it->second->ipv4_net;
                if (std::find(nets.begin(), nets.end(), net_addr_s ) == nets.end())
                    nets.emplace_back(net_addr_s);
            } else {
                struct addrs new_addrs = { {std::string(addressBuffer)}, {}, {net_addr_s} };
                _ifaces.emplace(std::make_pair(std::string(ifa->ifa_name),
                    std::make_shared<struct addrs>(new_addrs)));
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            auto binary_addr_ptr=&reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, binary_addr_ptr, addressBuffer, INET6_ADDRSTRLEN);
            if (auto find_it = _ifaces.find(ifa->ifa_name); find_it != _ifaces.end()) {
                find_it->second->ipv6.emplace_back(std::string(addressBuffer));
            } else {
                struct addrs new_addrs = { {}, {std::string(addressBuffer)}, {} };
                _ifaces.emplace(std::make_pair(std::string(ifa->ifa_name),
                    std::make_shared<struct addrs>(new_addrs)));
            }
        }
    }
    for (auto& tmp : tmp_ifaces)    // sets null addresses only if both IPv4 and IPv6 are empty
        if (_ifaces.find(tmp) == _ifaces.end()) {
            struct addrs empty_addrs = { {"0.0.0.0"}, {"::"}, {"0.0.0.0"} };
            _ifaces.emplace(std::make_pair(std::string(tmp), std::make_shared<struct addrs>(empty_addrs)));
        }
    if (interface_list != nullptr) freeifaddrs(interface_list);
}

/* Has to be an object value {} */
std::optional<Json::Value> json_from_str(const std::string& str)
{
    Json::CharReaderBuilder builder;
    builder["failIfExtra"] = true;
    std::unique_ptr<Json::CharReader> char_reader(builder.newCharReader());
    std::string error;
    Json::Value row_object;
    const auto str_len = str.length();
    auto first_paren = str.find_first_of('{');
    auto str_c_rec = str.c_str();
    auto ret = char_reader->parse(str_c_rec + first_paren, str_c_rec + str_len, &row_object, &error);

    if (ret) {
        std::cout << "--Read " << str << " GOOD" << std::endl;
    } else {
        std::cout << "--Read " << str << " FAILED" << std::endl;
    }

    if (!ret || first_paren == std::string::npos)
    {
        std::cerr << "json_from_str(str): Failed to parse str=|" << str << "| error=" << error << std::endl;
        return std::nullopt;
    }
    return row_object;
}

bool ipmon::is_iface_loopback(const std::string& ifname)
{
    if (ifname == "lo")
        return true;
    return false;
}

std::string get_hostname()
{
    const long hostname_len = sysconf(_SC_HOST_NAME_MAX) + 1;
    std::string hostname(hostname_len, ' ');
    if (gethostname(&hostname[0], hostname_len) == 0) {
        hostname.resize(hostname.find_first_of('\0'));
        return hostname;
    }
    else std::cerr << "gethostname() failed with error: " << strerror(errno) << std::endl;
    return "";
}

bool ipmon::socket_action(sockserver_cmd action)
{
    auto client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        Logger("SOCKET ERROR: ");
        return false;
    }
    const auto& cmd = sockserver_cmds.at(action);
    if (sendto(client_socket, cmd, strlen(cmd), 0, socket_server_addr_p(), sizeof(socket_server_addr())) == -1) {
        Logger("Temporary socket(socket_action) error on sendto(): ");
        close(client_socket);
        return false;
    }
    close(client_socket);
    return true;
}

int main(int argc, char* argv[]) {
    ipmon ipm;
    ipm.process_cmdline(argc, argv);
    ipm.start();
    auto listen_thread = std::thread(&ipmon::listen_socket, &ipm);
    listen_thread.detach();
    ipm.run();

    return 0;
}
