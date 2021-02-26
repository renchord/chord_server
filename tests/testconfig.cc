#include "../chord/log.h"
#include "../chord/config.h"
#include "yaml-cpp/yaml.h"

chord::ConfigVar<int>::ptr g_int_value_config = 
chord::Config::Lookup("system.port", (int)8080, "system port"); //构建了映射

//chord::ConfigVar<float>::ptr g_int_valuex_config = 
//chord::Config::Lookup("system.port", (float)8080, "system port"); //构建了映射

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

    YAML::Node root = YAML::LoadFile("/home/chord/workspace/chord_server/bin/conf/log.yml"); //Load了Yaml文件的全集
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

//自定义类型的配置处理
class Person {
public:
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;
    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name = " << m_name   
           << " age = " << m_age 
           << " sex = " << m_sex
           << "]";
        return ss.str();
    }
    bool operator==(const Person& rhs) const //这个fun的const非常重要 因为都是const成员去调用 ==方法
    //增加operator== :为了解决 重配置时 person类作为map的对比对象 必须要重载 == 操作符
    {
        return this->m_age == rhs.m_age && this->m_name == rhs.m_name && this->m_sex == rhs.m_sex;
    }
private:
    
};
namespace chord{

template<>//偏特化 Person
class LexicalCast <std::string, Person> {
public:
    Person operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};




template<>//偏特化 Person
class LexicalCast <Person, std::string> {
public:
    std::string operator() (const Person &p)
    {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}



chord::ConfigVar<Person>::ptr g_person = chord::Config::Lookup("class.person", Person(), "system person");

chord::ConfigVar<std::map<std::string, Person>>::ptr g_person_map = chord::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");


void test_calss(){
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "before:" << g_person->getValue().toString() << " - " << g_person->toString();
    
#define XX_PM(g_var, prefix)\
{\
     auto m = g_var->getValue();\
    for(auto& i : m)\
    {\
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) << prefix <<": "<< i.first << " - " << i.second.toString();\
    }\
    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << prefix <<": size = " << m.size();\
} 

    g_person->addListener(10, [](const Person& old_value, const Person& new_value){
        CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "old_value = " << old_value.toString() << " - " <<
        "new_value = " << new_value.toString();
    });

    XX_PM(g_person_map, "class.map before");
    YAML::Node root = YAML::LoadFile("/home/chord/workspace/chord_server/bin/conf/log.yml"); //Load了Yaml文件的全集
    chord::Config::LoadFromYaml(root); //

    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "after:" << g_person->getValue().toString() << " - " << g_person->toString();
    XX_PM(g_person_map, "class.map after");

#undef XX_PM
}

int main(int argc, char** argv)
{


    //test_yaml();
    //test_config();
    test_calss();
    return 0;
}