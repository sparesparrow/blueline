#include <optional>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Ipmon.h"

class IpInterface
{
public:
    IpInterface(const std::string& name, std::vector<std::string> ipv4, std::vector<std::string> ipv6, std::vector<std::string> ipv4_net)
        : name(name), ipv4(ipv4), ipv6(ipv6), ipv4_net(ipv4_net)
    {}
    virtual ~IpInterface() override = default;
private:
    std::string name{};
    std::vector<std::string> ipv4{};
    std::vector<std::string> ipv6{};
    std::vector<std::string> ipv4_net{};
};

class IpInterfacesManager
{
public:
    IpInterfacesManager(std::unordered_map<std::string, std::shared_ptr<IpInterface> > ifaces): _ifaces(ifaces)
    {}
    virtual ~IpInterfacesManager() = default;
    void update(std::string iface, std::shared_ptr<IpInterface> ip)
    {
        
        _ifaces[iface] = ip;
    }
    std::shared_ptr<IpInterface> get(std::string iface)
    {
        if(_ifaces.find(iface) != _ifaces.end())
            return _ifaces[iface];
        else
            return nullptr;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<IpInterface> > _ifaces;
};