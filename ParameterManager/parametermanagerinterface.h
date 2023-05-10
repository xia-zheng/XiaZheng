/*
 * @Author: Xiazheng <xiazheng.hhu@qq.com>
 * @Date: 2023-04-07 18:48:52
 * @FilePath: parametermanagerinterface.h
 * @Description: 
 */
#ifndef PARAMETERMANAGERINTERFACE_H
#define PARAMETERMANAGERINTERFACE_H

#include <QVariant>
#include <QMap>

class ParameterManagerInterface
{
public:
    /**
     * @brief 角色与权限的枚举
     *    1.  RolePermission  是角色与权限的结合
     *    2.  Permission      是权限类型
     *    3.  Role            是角色类型
     *  其中 RolePermission 可以由后两者计算得到:
     *      (int)RolePermission = (int)Permission << (int)Role
     */
    enum Permission
    {
        Read = 0B100,
        Write = 0B010,
    };
    enum Role
    {
        Developer = 6,
        Engineer = 3,
        Operator = 0,
        SuperRoot = 9,
    };
    enum RolePermission
    {
        DeveloperRead   = Read  << Developer,
        DeveloperWrite  = Write << Developer,
        EngineerRead    = Read  << Engineer,
        EngineerWrite   = Write << Engineer,
        OperatorRead    = Read  << Operator,
        OperatorWrite   = Write << Operator,
    };

    //设定值的读写
    virtual bool get_parameter(int para_id, QVariant &value, Role role) = 0;
    virtual bool set_parameter(int para_id, QVariant value, Role role) = 0;

    //文件读写
    virtual bool load_parameter_limit_file(QString file_name) = 0;
    virtual bool load_parameter_value_file(QString file_name) = 0;
    virtual bool save_parameter_value_file(QString file_name) = 0;
    
private:
    class Parameter{
    public:
        int para_id;
        QString name;
        QString path;
        QVariant value;
        QVariant::Type type;
        int limit;
        QVariantMap enum_table;

        bool check_permission(Permission permission, Role role);
        // bool check_permission(RolePermission role_permission);
        bool get_set_parameter(QVariant &get_value, Role role);
        bool set_set_parameter(QVariant set_value, Role role);
        // bool bind_real_parameter_get(std::function<bool(QVariant &)> get_lambda, Role role);
        // bool bind_real_parameter_set(std::function<bool(QVariant &)> set_lambda, Role role);
        // bool get_real_parameter(QVariant &value, Role role);
        // bool set_real_parameter(QVariant &value, Role role);
    };
    QMap<int, Parameter> parameter_map;
};


#endif // PARAMETERMANAGERINTERFACE_H
