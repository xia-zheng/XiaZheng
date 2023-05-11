/*
 * @Author       : XiaZheng <xiazheng.hhu@qq.com>
 * @Date         : 2023-04-07 18:48:52
 * @LastEditTime : 2023-05-11 21:21:41
 * @FilePath     : \XiaZheng\ParameterManager\parametermanager.cpp
 * @Description  : 
 */

#include "parametermanager.h"

QJsonObject enum_tables;

/**
 * @brief  加载参数权限Json文件
 * @param  {QString} file_name
 * @return {} 文件加载失败返回false，否则返回true
 */
bool ParameterManager::load_parameter_limit_file(QString file_name)
{   
    //读取文件的二进制数据
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    //解析二进制数据
    QJsonParseError json_error;
    QJsonDocument json_doc(QJsonDocument::fromJson(data, &json_error));
    //判断是否解析成功
    if(json_error.error != QJsonParseError::NoError){
        return false;
    }

    //解析json数据
    QJsonObject root_object = json_doc.object();

    //获取 enum_tables
    enum_tables = root_object["enum_tables"].toObject();

    //读取参数权限json文件
    load_limit_json_object(root_object.value("parameters").toObject(), "");

    return true;
    
}

/**
 * @brief  加载参数值Json文件
 * @param  {QString} file_name
 * @return {} 文件加载失败返回false，否则返回true
 */
bool ParameterManager::load_parameter_value_file(QString file_name)
{
    //读取文件的二进制数据
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    //解析二进制数据
    QJsonParseError json_error;
    QJsonDocument json_doc(QJsonDocument::fromJson(data, &json_error));
    //判断是否解析成功
    if(json_error.error != QJsonParseError::NoError){
        return false;
    }

    //解析json数据
    QJsonObject root_object = json_doc.object();

    //读取参数权限json文件
    load_value_json_object(root_object.value("parameters").toObject(), "");

    return true;
}
/**
 * @brief :  辅助解析函数，递归读取权限参数值json对象
 * @param  {QJsonObject} &obj
 * @param  {QString} current_path
 * @return {*}
 */
void ParameterManager::load_limit_json_object(const QJsonObject &obj, QString current_path)
{
    //遍历json对象
    for(auto iter = obj.begin(); iter != obj.end(); iter++)
    {
        QJsonObject value = iter.value().toObject();
        //若这是一个参数节点，则读取并保存此节点
        if(value.keys().contains("enum_id"))
        {
            Parameter para;
            para.para_id = value["enum_id"].toInt();
            para.name = iter.key();
            para.path = current_path+ "/" + para.name;
            para.limit = value["limit"].toVariant().toString().toInt(nullptr, 8);
            if(value.keys().contains("enum_table_name"))
            {
                para.enum_table = enum_tables[value["enum_table_name"].toString()].toObject().toVariantMap();
            }
            //保存参数
            //首先判断是否已经存在此参数，如果存在则更新，否则添加
            if(instance->parameter_map.find(para.para_id) != instance->parameter_map.end())
            {
                Parameter& old_para = instance->parameter_map[para.para_id];
                old_para.name = para.name;
                old_para.path = para.path;
                old_para.limit = para.limit;
                old_para.enum_table = para.enum_table;
            }
            else
            {
                instance->parameter_map.insert(para.para_id, para);
            }
        }
        //若这是一个父节点，则递归读取子节点
        else
        {
            load_limit_json_object(value, current_path + "/" + iter.key());
        }
    }
}

/**
 * @brief :  辅助解析函数，递归读取参数值json对象
 * @param  {QJsonObject} &obj
 * @param  {QString} current_path
 * @return {*}
 */
void ParameterManager::load_value_json_object(const QJsonObject &obj, QString current_path)
{
    //遍历json对象
    for(auto iter = obj.begin(); iter != obj.end(); iter++)
    {
        QJsonObject value = iter.value().toObject();
        //若这是一个参数节点，则读取并保存此节点
        if(value.keys().contains("enum_id"))
        {
            Parameter para;
            para.para_id = value["enum_id"].toInt();
            para.name = iter.key();
            para.path = current_path+ "/" + para.name;
            para.value = value["value"].toVariant();
            //保存参数
            //首先判断是否已经存在此参数，如果存在则更新，否则添加
            if(instance->parameter_map.find(para.para_id) != instance->parameter_map.end())
            {
                Parameter& old_para = instance->parameter_map[para.para_id];
                old_para.name = para.name;
                old_para.path = para.path;
                old_para.value = para.value;
            }
            else
            {
                instance->parameter_map.insert(para.para_id, para);
            }
        }
        //若这是一个父节点，则递归读取子节点
        else
        {
            load_value_json_object(value, current_path + "/" + iter.key());
        }
    }
}