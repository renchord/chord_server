#ifndef __CHORD_CONFIG_H__
#define __CHORD_CONFIG_H__


//配置系统的原则： 约定优于配置
#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "yaml-cpp/yaml.h"
#include "log.h"
#include "thread.h"


#define CHORD_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        chord::LogEventWrap(chord::LogEvent::ptr(new chord::LogEvent(logger,\
        level, __FILE__, __LINE__, 0, chord::GetThreadId(), chord::GetFiberId(), \
        time(0)))).getSS()

#define CHORD_LOG_DEBUG(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::DEBUG)
#define CHORD_LOG_INFO(logger)    CHORD_LOG_LEVEL(logger,   chord::LogLevel::INFO)
#define CHORD_LOG_WARN(logger)    CHORD_LOG_LEVEL(logger,   chord::LogLevel::WARN)
#define CHORD_LOG_ERROR(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::ERROR)
#define CHORD_LOG_FATAL(logger)   CHORD_LOG_LEVEL(logger,   chord::LogLevel::FATAL)

#define CHORD_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        chord::LogEventWrap(chord::LogEvent::ptr(new chord::LogEvent(logger, \
        level, __FILE__, __LINE__, 0, chord::GetThreadId(), chord::GetFiberId(), \
        time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define CHORD_LOG_FMT_DEBUG(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_INFO(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::INFO, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_WARN(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::WARN, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_ERROR(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::ERROR, fmt, __VA_ARGS__)
#define CHORD_LOG_FMT_FATAL(logger, fmt, ...)    CHORD_LOG_FMT_LEVEL(logger, chord::LogLevel::FATAL, fmt, __VA_ARGS__)

#define CHORD_LOG_ROOT()   chord::LoggerMgr::GetInstance()->getRoot()             
#define CHORD_LOG_NAME(name) chord::LoggerMgr::GetInstance()->getLogger(name)    //获取logger的方式 by name

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
    virtual std::string getTypeName() const = 0; //如果派生类特有 无法访问
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
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;
    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        :ConfigVarBase(name, description)
        ,m_val(default_value){}
    
    std::string toString() override {
        try {
            RWMutexType::ReadLock lock(m_mutex);
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
                << e.what() << "convert: " << typeid(val).name()
                << " - " << val;
        }
        return false;
    }
    
    const T getValue() 
    {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val; 
    }

    void setValue(const T& val) 
    {
        {
        RWMutexType::ReadLock lock(m_mutex);
        //m_val == val //一开始没注释掉这个 出问题了
        if(val == m_val) //判断原值和新值是否发生变化
            return;
        else
        {
            for(auto& i : m_cbs)
            {
                i.second(m_val, val); //回调
            }
            
        }
        }//ReadLock Destroy
        RWMutexType::WriteLock lock(m_mutex);
        m_val= val;

    }

    std::string getTypeName() const override {return typeid(T).name(); }
    uint64_t addListener (on_change_cb cb)
    {
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock (m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    } 

    void delListener (uint64_t key)
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key)
    {
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener()
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }
private:
    RWMutexType m_mutex;
    T m_val;
    //变更回调函数组, uint64_t key unique 一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};


class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,  //声明两个函数
            const T& default_value, const std::string& description = ""){
            RWMutexType::WriteLock lock(GetMutex()); //add write lock
            auto it = GetDatas().find(name);
            if(it != GetDatas().end())
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
                                    << typeid(T).name() << "real_type = " << it->second->getTypeName() << " " << it->second->toString();
                    return nullptr;
                }
            }

            /* Maybe useless code
            auto temp = Lookup<T>(name) ;
            if(temp != nullptr)
            {
                CHORD_LOG_INFO(CHORD_LOG_ROOT()) << "Lookup name = " << name << "exists";
                return temp;
            } */
            if(name.find_first_not_of("abcefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
            {
                CHORD_LOG_ERROR(CHORD_LOG_ROOT()) << "Lookup name invalid" << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            GetDatas()[name] = v; //构建映射对
            return v;
            }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        RWMutexType::ReadLock lock(GetMutex()); //add read lock
        auto it = GetDatas().find(name);
        if(it == GetDatas().end())
        {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    }

    static void LoadFromYaml(const YAML::Node& node);
    static ConfigVarBase::ptr LookupBase(const std::string& name); //不允许返回抽象类
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);  //

private: //staic方法可以是private的
    static ConfigVarMap& GetDatas()
    {
        static ConfigVarMap s_datas; //用方法来返回静态变量,这样不同cpp文件可以保证s_datas初始化好了

        return s_datas;
    }
    static RWMutexType& GetMutex() 
    {
        static RWMutexType s_mutex; // lock the same , make sure that lock is inited
        return s_mutex;
    }
};


}

#endif