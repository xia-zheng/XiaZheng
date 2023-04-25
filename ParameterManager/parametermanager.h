/*
 * @Author: Xiazheng <xiazheng.hhu@qq.com>
 * @Date: 2023-04-07 18:48:52
 * @FilePath: parametermanager.h
 * @Description: 
 */

#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include "parametermanagerinterface.h"

class ParameterManager : public ParameterManagerInterface
{
private:
    ParameterManager();
    ~ParameterManager();
    static ParameterManager *instance;

public:
    //单例模式
    static ParameterManager *getInstance(){
        if(instance == nullptr){
            instance = new ParameterManager();
        }
        return instance;
    }
};

#endif // PARAMETERMANAGER_H
