#ifndef NVIM_CPP_APP
#define NVIM_CPP_APP

#include <tuple>
#include "tree.h"
#include "nvim.hpp"


class App
{
public:
    App(nvim::Nvim *, int);

    void createTree(string &path);
    void handleNvimNotification(const string &method, const vector<nvim::Object> &args);
    void handleRequest(nvim::NvimRPC & rpc, uint64_t msgid, const string& method, const vector<nvim::Object> &args);

private:
    nvim::Nvim *m_nvim;
    int chan_id;

    Context m_ctx;
    Map m_cfgmap;

    // unordered_map<string, QVariant> resource;
    unordered_map<int, Tree*> trees;
    list<int> treebufs;  // Recently used order
};


#endif
