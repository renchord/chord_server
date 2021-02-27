#include <list>
#include "config.h"


namespace chord{

//"A.B", 10  map
//A:
//   B: 10 
//   C: str

static void ListAllMember(const std::string& prefix, 
                          const YAML::Node& node, 
                          std::list<std::pair<std::string, const YAML::Node>>& output){
    if(prefix.find_first_not_of("abcefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
    {
        CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "Config invalid name : " << prefix << " : " << node;
        return;
    }
    output.emplace_back(std::make_pair(prefix, node));
    if(node.IsMap())
    {
        for(auto it = node.begin(); it != node.end(); ++it)
        {
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);//question
        }
    }
}




void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes); //该函数完成对YAML node中所有的对象的存储,每个pair key存string,带., val值存map的结果

    for(auto& i : all_nodes)
    {
        std::string key = i.first;//对所有非空的map节点
        if(key.empty())
        {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);//转成小写
        ConfigVarBase::ptr var = LookupBase(key);//根据key值，也就是string去查找ConfigVarBase的指针

        if(var != nullptr)
        {
            if(i.second.IsScalar())//将值转成string类型并
            {
                var->fromString(i.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

ConfigVarBase::ptr Config::LookupBase(const std::string& name)
{
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}
}