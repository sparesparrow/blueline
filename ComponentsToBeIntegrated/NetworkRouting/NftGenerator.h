#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <nftables/libnftables.h>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <sys/un.h>
#include <sys/socket.h>
#include <thread>
#include <map>

/*! Base class for json data structure. */
class cmd_json
{
public:
    //! Constructs the object
    cmd_json(std::string keyroot): _keyroot(keyroot) { }
    //! Destructs the object
    virtual ~cmd_json() {}
    /*! \return stored value as a string */
    const std::string get_str() {
        Json::Value root;
        root[_keyroot] = *_proot;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::ostringstream ss;
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(root, &ss);
        return ss.str();

    }
    /*! \return stored value as a pretty-print string */
    const std::string get_pp() {
        Json::Value root;
        root[_keyroot] = *_proot;
        return root.toStyledString();
    }
    /*! \return stored value as JSON object/array */
    const Json::Value get_json() {
        Json::Value root;
        root[_keyroot] = *_proot;
        return root;
    }
protected:
    //! Stored commands
    std::shared_ptr<Json::Value> _proot;
private:
    std::string _keyroot;
};

class cmd_json_a : public cmd_json
{
public:
    //! Constructs the object
    cmd_json_a(std::string keyroot): cmd_json(keyroot) {
        _proot = std::make_shared<Json::Value>(Json::Value(Json::arrayValue));
    }
    //! Destructs the object
    virtual ~cmd_json_a() {}
    //! Adds command to the commands list
    void cmd_append(Json::Value cmd) {
        _proot->append(cmd);
    }
protected:
private:
};

class nft_root : public cmd_json_a
{
public:
    //! Constructs the object
    nft_root(): cmd_json_a("nftables") { }
    //! Destructs the object
    virtual ~nft_root() {}
    //! Adds testing command to dry run before running actual command
    void test_cmd_append(std::string cmd) {
        test_cmds.emplace_back(cmd);
    }
    //! Stored testing commands
    std::vector<std::string> test_cmds{};
};

/*! Holds data for creating various \c nft commands */
class nft_set : public nft_root
{
public:
    //! Constructs the object from given parameters
    nft_set(std::string family,
            std::string table,
            std::string name,
            std::string type,
            std::vector<std::string>& addresses): nft_root(),
        _family(family), _table(table), _name(name), _type(type), _addresses(addresses)  { }
    //! \return JSON command creating an empty set
    Json::Value cmd_add_set_json(bool append = false) {
        Json::Value set;
        set["family"] = _family;
        set["table"] = _table;
        set["name"] = _name;
        set["type"] = _type;
        Json::Value set_root;
        set_root["set"] = set;
        Json::Value add;
        add["add"] = set_root;
        if (append)
            _proot->append(add);
        return add;
    }
    //! \return JSON command flushing a set. Prints some errors if set does not exist
    Json::Value cmd_flush_set_json(bool append = false) {
        Json::Value set;
        set["family"] = _family;
        set["table"] = _table;
        set["name"] = _name;
        Json::Value set_root;
        set_root["set"] = set;
        Json::Value flush;
        flush["flush"] = set_root;
        if (append)
            _proot->append(flush);
        return flush;
    }
    //! \return JSON command filling the set with elements or creating such set if not exists
    Json::Value cmd_add_element_json(bool append = false) {
        Json::Value element;
        element["family"] = _family;
        element["table"] = _table;
        element["name"] = _name;
        Json::Value ele = Json::arrayValue;
        for (auto &a : _addresses)
            ele.append(a);
        if (ele.empty())
            ele.append("0.0.0.0");
        element["elem"] = ele;
        Json::Value element_root;
        element_root["element"] = element;
        Json::Value add;
        add["add"] = element_root;
        if (append)
            _proot->append(add);
        return add;
    }
    //! \return BISON command creating an empty set
    const std::string cmd_add_empty() {
        std::stringstream ss;
        ss << "add set " << _family << " " << _table << " " << _name << " { type " << _type << " ; }";
        return ss.str();
    }
    //! \return BISON command flushing a set. Prints some errors if set does not exist
    const std::string cmd_flush() {
        std::stringstream ss;
        ss << "flush set " << _family << " " << _table << " " << _name;
        return ss.str();
    }
    //! \return BISON command filling the set with elements or creating such set if not exists
    const std::string cmd_add() {
        std::stringstream ss;
        ss << "add set " << _family << " " << _table << " " << _name << " { type " << _type << " ; elements = { ";
        for (auto &a : _addresses)
            ss << a << ", ";
        ss << "} ; }";
        return ss.str();
    }
private:
    //! Address family (contains \c table )
    std::string _family;
    //! Table (contains set \c name )
    std::string _table;
    //! Set name (must specify \c type )
    std::string _name;
    //! Set type (currently only \c ipv4_addr )
    std::string _type;
    //! Set elements, must not be empty
    std::vector<std::string>& _addresses;
};
