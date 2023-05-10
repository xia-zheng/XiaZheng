/*
 * @Author       : XiaZheng <xiazheng.hhu@qq.com>
 * @Date         : 2023-04-07 18:48:52
 * @LastEditTime : 2023-05-10 23:42:52
 * @FilePath     : \XiaZheng\ParameterManager\parametermanager.cpp
 * @Description  : 
 */

#include "parametermanager.h"

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
    QJsonObject root_obj = json_doc.object();
    
}