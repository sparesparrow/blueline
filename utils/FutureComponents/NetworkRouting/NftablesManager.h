#ifndef NFTABLESMANAGER_H
#define NFTABLESMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <optional>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <jsoncpp/json/json.h>
#include "NftGenerator.h"
#include "IpInterfacesManager.h"


class NftablesManager : public QObject
{
    Q_OBJECT

public:
    explicit NftablesManager(QObject *parent = nullptr);
    virtual ~NftablesManager() = default;

public slots:
    void addRule(QString family, QString table, QString name, QString type, QStringList addresses);
    void removeRule(QString family, QString table, QString name);
    void addIpToBlacklist(QString ip);
    void removeIpFromBlacklist(QString ip);
    void help();
private:
    //! Stored interface names and addresses
    std::unordered_map<std::string, std::shared_ptr<struct addrs>> _ifaces;
    void runNftCommand(QString command);
    /*! Updates nftables IP addresses files \ref nft_outfile_sets() and \ref nft_outfile_vars. */
    void ifaces_filewrite_sets_vars();
    /*! Creates ip nat and inet filter sets for each interface in \ref _ifaces. */
    std::shared_ptr<nft_root>  create_sets_ifaces();
    /*! Updates nftables runtime configuration */
    bool runtime_update(std::shared_ptr<nft_root> cmd_json_nft);
    /*! Updates nftables configuration and configuration file with information in \c _ifaces
     *  Running configuration is updated by reseting named set values (accesed with @set)
     *  Persistent configuration is updated by overwriting file at \c nft_outfile_sets() and \c nft_outfile_vars()
     *  where values are put into variables (accesed with $variable) or sets (accesed with @set).
     *  Persistent configuration is reloaded on program start if \c _opt_start is set and if \c _opt_flush is set then
     *  it is reloaded every time netlink received an address related message.
     */
    void update_conf();
    /*! Removes named sets from nftables for every interface in \c _ifaces (not very useful)*/
    void rm_nft_sets();
    //! Stored interface names and addresses
    std::optional<std::string> atomic_write(const std::string& path, const std::string& content);

    //! Nftables persistent configuration file
    std::string _nft_outfile{"./ifacesAddrs"};
    std::string nft_outfile_sets() const { return _nft_outfile + ".sets"; }
    std::string nft_outfile_vars() const { return _nft_outfile + ".vars"; }
    //! Nftables flush-and-reload script
    static constexpr const char* _nft_conf_no_suricata = "./nftablesDefault.conf";
    static constexpr const char* _nft_conf_with_suricata = "./nftablesCustom.conf";    
    
    
    std::shared_ptr<nft_set> set;

};

#endif // NFTABLESMANAGER_H
