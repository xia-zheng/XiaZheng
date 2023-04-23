
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
