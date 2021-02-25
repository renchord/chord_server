#include "../chord/log.h"
#include "../chord/config.h"
#include "yaml-cpp/yaml.h"

chord::ConfigVar<int>::ptr g_int_value_config = 
    chord::Config::Lookup("system.port", (int)8080, "system port"); //构建了映射

chord::ConfigVar<float>::ptr g_float_value_config = 
chord::Config::Lookup("system.value", (float)10.2f, "system value"); //构建了映射

chord::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
chord::Config::Lookup("system.int_vec", std::vector<int> {1,2}, "system int vec"); //构建了映射

chord::ConfigVar<std::list<int>>::ptr g_int_list_value_config = 
chord::Config::Lookup("system.int_list", std::list<int> {1,2}, "system int list"); //构建了映射

chord::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
chord::Config::Lookup("system.int_set", std::set<int> {1,2}, "system int set"); //构建了映射

chord::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = 
chord::Config::Lookup("system.int_uset", std::unordered_set<int> {1,2}, "system int unordered_set"); //构建了映射

chord::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config = 
chord::Config::Lookup("system.str_int_map", std::map<std::string, int> {{"k",2}}, "system str int map"); //构建了映射

chord::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config = 
chord::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int> {{"k",2}}, "system str int umap"); //构建了映射

void print_yaml(const YAML::Node& node, int level)
{
    if(node.IsScalar())
    {
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<std::string(level + 4, ' ') << node.Scalar() << " - "<< node.Type() << " - " << level;
    } 
    else if(node.IsNull())
    {
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<std::string(level + 4, ' ')  << "NULL - "<< " - "<< node.Type() << level;
    }
    else if(node.IsMap())
    {
        for(auto it = node.begin(); it != node.end(); ++it)
        {
            CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<std::string(level + 4, ' ')  << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }
    else if(node.IsSequence())
    {
        for(size_t i = 0; i < node.size(); ++i)
        {
            CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<std::string(level + 4, ' ')  << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/home/chord/chordPj/bin/conf/log.yml");

    print_yaml(root, 0);
    //
}

void test_config() {
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<"before : " << g_int_value_config->getValue();//依赖全局的构建
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<"before : " << g_float_value_config->toString();


#define XX(g_var, name, prefix) \
{ \
    auto v = g_var->getValue(); \
    for(auto& i : v) { \
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) << #prefix " " #name " : " << i; \
    } \
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();\
}
 

#define XX_M(g_var, name, prefix) \
{ \
    auto v = g_var->getValue(); \
    for(auto& i : v) { \
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) << #prefix " " #name " : {" << i.first << " - " << i.second << "}" ; \
    } \
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString();\
}
    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, list, before);
    XX(g_int_set_value_config, set, before);
    XX(g_int_uset_value_config, uset, before);
    XX_M(g_str_int_map_value_config, str_int_map, before);
    XX_M(g_str_int_umap_value_config, str_int_map, before);

    YAML::Node root = YAML::LoadFile("/home/chord/chordPj/bin/conf/log.yml"); //Load了Yaml文件的全集
    chord::Config::LoadFromYaml(root); //


    CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<"after : " << g_int_value_config->getValue();
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) <<"after : " << g_float_value_config->toString();
    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, list, after);
    XX(g_int_set_value_config, set, after);
    XX(g_int_uset_value_config, uset, before);
    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_umap_value_config, str_int_map, after);

#undef XX
#undef XX_M
}

int main(int argc, char** argv)
{


    //test_yaml();
    test_config();
    return 0;
}