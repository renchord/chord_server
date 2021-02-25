#ifndef __CHORD_CONFIG_H__
#define __CHORD_CONFIG_H__


//配置系统的原则： 约定优于配置
#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include "yaml-cpp/yaml.h"
#include "log.h"
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace chord
{

class ConfigVarBase
{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description){
        std::transform(m_name.begin(), m_name.end(), m_name.begin(),::tolower);//大写转成小写
    }

    virtual ~ConfigVarBase(){}

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const {return m_description; }

    virtual std::string toString() = 0; //纯虚函数
    virtual bool fromString(const std::string& val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};


// F From_type, T To_type
// 仿函数 实现从 F->T的转换
// 默认类型都用boost库进行处理
template<class F, class T>
class LexicalCast {
public:
    T operator() (const F & v){
        return boost::lexical_cast<T>(v);
    }
};

template<class T>//偏特化 针对vector
class LexicalCast <std::string, std::vector<T>> {
public:
    std::vector<T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::vector<T> vec; //告诉编译器 std::vector<T>是个类型名
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i)
        {   
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str())); //递归处理,因为 T有可能是vector<U>,需要递归判断
        }
        return vec;
    }
};


template<class T>//偏特化 针对vector
class LexicalCast <std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i))); //变成一个YAML节点再pushback
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};



template<class T>//偏特化 list
class LexicalCast <std::string, std::list<T>> {
public:
    std::list<T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::list<T> l; //告诉编译器 std::list<T>是个类型名
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i)
        {   
            ss.str("");
            ss << node[i];
            l.push_back(LexicalCast<std::string, T>()(ss.str())); //递归处理,因为 list<U>,需要递归判断
        }
        return l;
    }
};


template<class T>//偏特化 list
class LexicalCast <std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i))); //变成一个YAML节点再pushback
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};

template<class T>//偏特化 set
class LexicalCast <std::string, std::set<T>> {
public:
    std::set<T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::set<T> s; //告诉编译器 std::set<T>是个类型名
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i)
        {   
            ss.str("");
            ss << node[i];
            s.insert(LexicalCast<std::string, T>()(ss.str())); //递归处理,因为 set<U>,需要递归判断
        }
        return s;
    }
};

template<class T>//偏特化 set
class LexicalCast <std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i))); //变成一个YAML节点再pushback
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};



template<class T>//偏特化 unordered_set
class LexicalCast <std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i))); //变成一个YAML节点再pushback
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};


template<class T>//偏特化 unordered_set
class LexicalCast <std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::unordered_set<T> s; //告诉编译器 std::unordered_set<T>是个类型名
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i)
        {   
            ss.str("");
            ss << node[i];
            s.insert(LexicalCast<std::string, T>()(ss.str())); //递归处理,因为 unordered_set<U>,需要递归判断
        }
        return s;
    }
};



template<class T>//偏特化 map
class LexicalCast <std::string, std::map<std::string ,T>> {   
public:
    std::map<std::string, T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::map<std::string, T> m; //告诉编译器 std::map<T>是个类型名
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it)
        {   
            ss.str("");
            ss << it->second;
            m.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return m;
    }
};

template<class T>//偏特化 map
class LexicalCast <std::map<std::string, T>, std::string> {
public:
    std::string operator() (const std::map<std::string,T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second)); 
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};


template<class T>//偏特化 unordered_map
class LexicalCast <std::string, std::unordered_map<std::string ,T>> {   
public:
    std::unordered_map<std::string, T> operator() (const std::string &v)
    {
        YAML::Node node = YAML::Load(v);//如果不是数组会解析失败
        typename std::unordered_map<std::string, T> m; //告诉编译器 std::unordered_map<T>是个类型名
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it)
        {   
            ss.str("");
            ss << it->second;
            m.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return m;
    }
};

template<class T>//偏特化 unordered_map
class LexicalCast <std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string,T> &v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second)); 
        }
        std::stringstream ss;
        ss << node;
       
        return ss.str();
    }
};



//FromStr   T operator() (const std::string& input) 
//ToStr     std::string operator() (const T& input)
template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>> // From Str 和 ToStr是 functors
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        :ConfigVarBase(name, description)
        ,m_val(default_value){}
    
    std::string toString() override {
        try {
            return ToStr()(m_val);
            //return boost::lexical_cast<std::string>(m_val); //泛化
        } catch (std::exception& e)
        {
            CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "ConfigVar::toString exception" 
                << e.what() << "convert: " << typeid(m_val).name() << "to string";
        }
        return "";
    }
    bool fromString(const std::string& val) override {
        try {
            //m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        } catch (std::exception& e)
        {
            CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "ConfigVar::toString exception" 
                << e.what() << "convert: " << typeid(m_val).name();
        }
        return false;
    }
    
    const T getValue() const {return m_val; }
    void setValue(const T& val) {m_val = val; } 
private:
    T m_val;
};


class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,  //声明两个函数
            const T& default_value, const std::string& description = ""){
            
            auto it = s_datas.find(name);
            if(it != s_datas.end())
            {
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
                if(tmp != nullptr)
                {
                    CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "Lookup name = " << name << "exists";
                    return tmp;
                }
                else
                {
                    CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "Lookup name = " << name << "exists but type not"
                                    << typeid(T) <<endl;
                    return nullptr;
                }
            }

            auto temp = Lookup<T>(name) ;
            if(temp != nullptr)
            {
                CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "Lookup name = " << name << "exists";
                return temp;
            }
            if(name.find_first_not_of("abcefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
            {
                CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "Lookup name invalid" << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            s_datas[name] = v; //构建映射对
            return v;
            }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end())
        {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    }

    static void LoadFromYaml(const YAML::Node& node);
    static ConfigVarBase::ptr LookupBase(const std::string& name); //不允许返回抽象类
private:
    static ConfigVarMap s_datas;

};

}

#endif