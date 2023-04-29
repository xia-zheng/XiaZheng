/*
 * @Author: Xiazheng <xiazheng.hhu@qq.com>
 * @Date: 2023-04-07 18:48:52
 * @FilePath: parametermanager.h
 * @Description: 
 */

#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "parametermanagerinterface.h"

class ParameterManager : public ParameterManagerInterface
{
private:
    ParameterManager();
    ~ParameterManager();
    static ParameterManager& instance;

public:
    //单例模式
    static ParameterManager& getInstance(){
        return instance;
    }
    //设定值的读写
    bool get_parameter(int para_id, QVariant &value, Role role) override;
    bool set_parameter(int para_id, QVariant &value, Role role) override;
    //文件读写
    bool load_parameter_limit_file(QString file_name) override;
    bool load_parameter_value_file(QString file_name) override;
    bool save_parameter_value_file(QString file_name) override;
};

#endif // PARAMETERMANAGER_H
