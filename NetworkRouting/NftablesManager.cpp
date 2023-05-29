#include "NftablesManager.h"

NftablesManager::NftablesManager(QObject *parent) : QObject(parent)
{
}

void NftablesManager::help()
{
    std::cout << R"( 
    For nftables, the default file name (if not specified in FILE) is )" + _nft_outfile + R"( suffixed with .sets/.vars

     )" + nft_outfile_vars() + R"(
    #!/usr/sbin/nft -f
    define enp0s3 = { 192.168.150.52 }
    define enp0s8 = { 192.168.56.2, 192.168.56.3 }
    define lo = { 127.0.0.1 }

     )" + nft_outfile_sets() + R"(
    #!/usr/sbin/nft -f
    set enp0s3_ipv4_address { type ipv4_addr; elements = { 192.168.150.52,   } }
    set enp0s8_ipv4_address { type ipv4_addr; elements = { 192.168.56.2,   } }
    set lo_ipv4_address { type ipv4_addr; elements = { 127.0.0.1,   } }

    It is possible to update running nftables configuration without the need to flush the ruleset.
    This requires using named sets in rule definitions by calling @interface-name instead of $interface-name
    Before such update occurs, @interface-name has the value of $interface-name, taken from the file.
    After address update, named set value is updated, i.e. when enp0s3 will change to 192.168.150.101, 192.168.150.102:

        set enp0s3 {                                            set enp0s3 {
            type ipv4_addr                                          type ipv4_addr
            elements = { $enp0s3 }          ---------->             elements = { 192.168.150.101, 192.168.150.102 }
        }                                                       }

    If parameter --flush is specified, the nftables configuration is always flushed, reloading nftables
    configuration files including the one created by ipmon.
)" << std::endl;
}

void NftablesManager::addRule(QString family, QString table, QString name, QString type, QStringList addresses)
{
    std::vector<std::string> addressesStd = convertQStringListToStd(addresses);
    set = std::make_shared<nft_set>(family.toStdString(), table.toStdString(), name.toStdString(), type.toStdString(), addressesStd);
    runNftCommand(QString::fromStdString(set->cmd_add_empty()));
    runNftCommand(QString::fromStdString(set->cmd_add()));
}

void NftablesManager::removeRule(QString family, QString table, QString name)
{
    set = std::make_shared<nft_set>(family.toStdString(), table.toStdString(), name.toStdString(), "", {});
    runNftCommand(QString::fromStdString(set->cmd_flush()));
}

void NftablesManager::addIpToBlacklist(QString ip)
{
    // Assuming the blacklist is stored in a set named "blacklist" in the "filter" table of the "ip" family
    addRule("ip", "filter", "blacklist", "ipv4_addr", {ip});
}

void NftablesManager::removeIpFromBlacklist(QString ip)
{
    // Assuming the blacklist is stored in a set named "blacklist" in the "filter" table of the "ip" family
    // Remove the rule by flushing the set and then re-adding all elements except the one to be removed
    // TODO: Implement this functionality
}

void NftablesManager::runNftCommand(QString command)
{
    QProcess process;
    process.start("nft", QStringList() << command);
    process.waitForFinished();
}

std::vector<std::string> NftablesManager::convertQStringListToStd(QStringList list)
{
    std::vector<std::string> stdList;
    for (const QString &str : list)
        stdList.push_back(str.toStdString());
    return stdList;
}

std::optional<std::string> NftablesManager::atomic_write(const std::string& path, const std::string& content)
{
    std::ofstream ofs;

    try {
        ofs.open(path + ".tmp", std::ofstream::out | std::ofstream::trunc);
    } catch (const std::ios_base::failure& e)
    {
        return "Error opening file at " + path + ".tmp: " + e.what();
    }
    ofs << content;
    try {
        ofs.close();
    } catch (const std::ios_base::failure& e)
    {
        return "Error closing file at " + path + ".tmp: " + e.what();
    }
    if (rename((path + ".tmp").c_str(), path.c_str()) != 0)
        return "Error writing file at " + path + ": " + strerror(errno);
    return std::nullopt;
}

std::shared_ptr<nft_root> NftablesManager::create_sets_ifaces()
{
    nft_root cmd_json_nft;
    for (auto p = _ifaces.begin(); p != _ifaces.end(); p++)
    {
        std::vector<nft_set> sets;
        std::string set_4a = p->first + "_ipv4_address";
        sets.emplace_back("ip", "nat", set_4a, "ipv4_addr", p->second->ipv4);
        sets.emplace_back("inet", "filter", set_4a, "ipv4_addr", p->second->ipv4);
        for (auto& ns : sets) {
            cmd_json_nft.cmd_append(ns.cmd_add_set_json());
            cmd_json_nft.cmd_append(ns.cmd_flush_set_json());
            cmd_json_nft.cmd_append(ns.cmd_add_element_json());
            cmd_json_nft.test_cmd_append(ns.cmd_add_empty());
        }
    }
    return std::make_shared<nft_root>(cmd_json_nft);
}

bool NftablesManager::runtime_update(std::shared_ptr<nft_root> cmd_json_nft)
{
    std::unique_ptr<struct nft_ctx, void (*)(struct nft_ctx*)> nft = { nft_ctx_new(NFT_CTX_DEFAULT), nft_ctx_free };
    nft_ctx_output_set_flags(&*nft, NFT_CTX_OUTPUT_JSON);
    if (!nft) {
        Logger("Failed to obtain nftables context.");
        return;
    }
    // need to test that set can be added to table (permissions, table exists etc.)
    nft_ctx_set_dry_run(&*nft, true);
    bool cmd_ok = true;
    for (auto& cmd : cmd_json_nft->test_cmds) {
        if (nft_run_cmd_from_buffer(&*nft, cmd.c_str()) != 0)
            cmd_ok = false;
    }
    nft_ctx_set_dry_run(&*nft, false);
    if (cmd_ok)
        if (nft_run_cmd_from_buffer(&*nft, cmd_json_nft->get_pp().c_str()) != 0)
            Logger("Error running nft command: " + cmd_json_nft->get_pp() + " : ");
    //std::cout << cmd_json_nft->get_pp().c_str() << std::endl;
    return cmd_ok;
}

void NftablesManager::ifaces_filewrite_sets_vars()
{
    static constexpr const char* shebang_nft{"#!/usr/sbin/nft -f\n"};
    std::stringstream filecontent_vars, filecontent_sets;
    filecontent_vars << shebang_nft;
    filecontent_sets << shebang_nft;
    for (auto p = _ifaces.begin(); p != _ifaces.end(); p++)
    {
        // prepare update file with constant definitions (unnamed vars)
        filecontent_vars << "redefine " << p->first << "_ipv4_address  = { ";
        for (auto &addr : p->second->ipv4)
            filecontent_vars << addr << ", ";
        if (p->second->ipv4.empty())
            filecontent_vars << "0.0.0.0";
        filecontent_vars << " }\n";

        // IPv4 network addresses  (unnamed vars)
        filecontent_vars << "redefine " << p->first << "_ipv4_network  = { ";
        for (auto &addr : p->second->ipv4_net)
            filecontent_vars << addr << ", ";
        if (p->second->ipv4_net.empty())
            filecontent_vars << "0.0.0.0";
        filecontent_vars << " }\n";

        // prepare update file with named sets
        filecontent_sets << "set " << p->first << "_ipv4_address { type ipv4_addr; elements = { ";
        for (auto &addr : p->second->ipv4)
            filecontent_sets << addr << ", ";
        if (p->second->ipv4.empty())
            filecontent_sets << "0.0.0.0";
        filecontent_sets << " } }\n";

        // IPv4 network addresses (named sets)
        filecontent_sets << "set " << p->first << "_ipv4_network { type ipv4_addr; elements = { ";
        for (auto &addr : p->second->ipv4_net)
            filecontent_sets << addr << ", ";
        if (p->second->ipv4_net.empty())
            filecontent_sets << "0.0.0.0";
        filecontent_sets << " } }\n";
    }
    auto err = atomic_write(nft_outfile_vars(), filecontent_vars.str());
    if (err == std::nullopt) {
        if (_opt_monitor)
            std::cout << "--Written file " << nft_outfile_vars() << std::endl;
    } else Logger(*err);

    err = atomic_write(nft_outfile_sets(), filecontent_sets.str());
    if (err == std::nullopt) {
        if (_opt_monitor)
            std::cout << "--Written file " << nft_outfile_sets() << std::endl;
    } else Logger(*err);
}

void NftablesManager::update_conf()
{
    // Always write up-to-date values into file, no matter if updating running conf or reflushing from file
    ifaces_filewrite_sets_vars();
    if (_opt_flush || _opt_start) {
        std::unique_ptr<struct nft_ctx, void (*)(struct nft_ctx*)> nft = { nft_ctx_new(NFT_CTX_DEFAULT), nft_ctx_free };
        if (!nft) {
            Logger("Failed to obtain nftables context.");
            return;
        }
        nft_run_cmd_from_filename(&*nft, nft_conf_file());
    } else {
        runtime_update(create_sets_ifaces());
    }
}

void NftablesManager::rm_nft_sets ()
{
    std::unique_ptr<struct nft_ctx, void (*)(struct nft_ctx*)> nft = { nft_ctx_new(NFT_CTX_DEFAULT), nft_ctx_free };
    if (!nft)
        return;
    for (auto p = _ifaces.begin(); p != _ifaces.end(); p++) {
        nft_run_cmd_from_buffer(&*nft, ("delete set ip nat " + p->first).c_str());
        nft_run_cmd_from_buffer(&*nft, ("delete set inet filter " + p->first).c_str());
    }
}
