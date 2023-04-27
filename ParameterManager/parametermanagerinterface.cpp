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