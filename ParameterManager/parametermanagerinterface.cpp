/*
 * @Author: Xiazheng <xiazheng.hhu@qq.com>
 * @Date: 2023-04-25 20:06:46
 * @FilePath: parametermanagerinterface.cpp
 * @Description: 
 */
#include "parametermanagerinterface.h"


/**
 * @description: 判断角色是否有权限
 * @param {Permission} permission：权限
 * @param {Role} role：角色
 * @return {*}：有权限返回true，否则返回false
 */
bool ParameterManagerInterface::Parameter::check_permission(Permission permission, Role role)
{
    int role_permission = (int)permission << (int)role;
    if (role_permission & limit)
        return true;
    else
        return false;
}


/**
 * @description: 判断是否读到了参数
 * @param {QVariant} &get_value： 读到的参数值
 * @param {Role} role ：角色
 * @return {*}  读到了返回true，否则返回false
 */
bool ParameterManagerInterface::Parameter::get_set_parameter(QVariant &get_value, Role role)
{
    if (check_permission(Read, role))
    {
        get_value = value;
        return true;
    }
    else
        return false;
}


/**
 * @description: 判断是否写入了参数
 * @param {QVariant} set_value：写入的参数值
 * @param {Role} role：角色
 * @return {*} 写入了返回true，否则返回false
 */
bool ParameterManagerInterface::Parameter::set_set_parameter(QVariant set_value, Role role)
{
    if (check_permission(Write, role))
    {
        value = set_value;
        return true;
    }
    else
        return false;
}