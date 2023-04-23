/**
 * @file parametermanager.cpp
 * @author Morphine
 */
#include "parametermanager.h"

//所有静态变量的声明
QJsonDocument ParameterManager::json_doc;
QJsonObject ParameterManager::enum_tables;
QList<ParameterManager::ParaStruct> ParameterManager::parameters;
//ParameterManager ParameterManager::manager("../parameters.json");
ParameterManager ParameterManager::manager("");

/**
 * @brief 构造函数
 * @param json_path json配置文件的路径，若为空字符串则不加载
 * @details 单例模式下，此函数会在 main() 之前运行
 * @warning 因为所有静态变量的初始化顺序可能是不确定的（不同编译器不同结果）
 *          所以可能会出现调用此构造函数前，类内静态变量仍未初始化的情况，导致崩溃（仅保证qt_5.15.2测试正常）
 *          因此，不建议在此函数中加载json，建议在主窗口的构造函数中手动调用 load_file 函数（也许是我单例模式写的有问题）
 */
ParameterManager::ParameterManager(QString json_path)
{
    if(!json_path.isEmpty())
    {
        load_file(json_path);
    }
}

/**
 * @brief 加载 json配置文件
 * @param json_path json配置文件的路径
 * @return 文件读失败或json格式不正确时返回false
 *          读取成功返回true
 * @details 解析后的 json 信息保存在了 json_doc 中
 */
bool ParameterManager::load_file(QString json_path)
{
    //读取文件的二进制数据
    QFile file(json_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    QByteArray file_data = file.readAll();
    file.close();

    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    json_doc = QJsonDocument::fromJson(file_data, &jsonError);

    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !json_doc.isNull()) {
        return false;
    }

    //读取根节点
    QJsonObject root_object = json_doc.object();

    //获取 enum_tables
    enum_tables = root_object["enum_tables"].toObject();

    //遍历读取所有参数
    parameter_clear();
    load_json_object(root_object.value("parameters").toObject(), "");
    //按枚举值排序
    std::sort(parameters.begin(), parameters.end(), para_struct_compare);

    return true;
}

/**
 * @brief ParameterManager::parameter_clear
 */
void ParameterManager::parameter_clear()
{
    parameters.clear();
}

/**
 * @brief 比较两个参数的先后顺序，用于 qSort 排序
 */
bool ParameterManager::para_struct_compare(const ParaStruct &para1, const ParaStruct &para2)
{
    return para1.enum_id < para2.enum_id;
}

/**
 * @brief 根据参数的enum值查找参数在QList中的索引（二分法）
 * @param target 目标参数
 * @return 参数在QList中的索引，未查找到则返回 -1
 */
int ParameterManager::parameter_find(Parameter target)
{
    int begin = 0;
    int end = parameters.length();
    int mid = begin;
    while(begin < end - 1)
    {
        mid = begin + (end - begin) / 2;
        if(parameters[mid].enum_id < target)
        {
            begin = mid + 1;
        }
        else if(parameters[mid].enum_id > target)
        {
            end = mid;
        }
        else
        {
            begin = mid;
            break;
        }
    }
    if(parameters[begin].enum_id == target)
        return begin;
    else
        return -1;
}

/**
 * @brief 保存json文件的辅助函数，内部进行了递归
 */
void ParameterManager::save_json_object(QJsonValueRef value)
{
    QJsonObject object = value.toObject();
    QStringList keys = object.keys();
    //若这是一个参数，则读取参数并修改
    if(keys.contains("enum_id"))
    {
        QVariant new_value;
        get_parameter(static_cast<Parameter>(object["enum_id"].toInt()), new_value);
        object["value"] = QJsonValue::fromVariant(new_value);
    }
    //若这是一个父节点，则遍历所有子节点
    else
    {
        foreach(QString key, keys)
        {
            save_json_object(object[key]);
        }
    }
    value = object;
}

/**
 * @brief 读取json文件的辅助函数，内部进行了递归
 */
void ParameterManager::load_json_object(const QJsonObject object, QString current_path)
{
    for(auto it = object.begin(); it != object.end(); it++)
    {
        QJsonObject value = it.value().toObject();
        //若这是一个参数节点，则读取并保存此节点
        if(value.keys().contains("enum_id"))
        {
            ParaStruct para;
            para.name = it.key();
            para.path = current_path + "/" + para.name;
            para.enum_id = static_cast<Parameter>(value["enum_id"].toInt());
            para.value = value["value"].toVariant();
            para.limit = value["limit"].toVariant().toString().toInt(nullptr, 8);
            if(value.keys().contains("enum_table_name"))
            {
                para.enum_table = enum_tables[value["enum_table_name"].toString()].toObject().toVariantMap();
            }
            parameters.append(para);
        }
        //若这是一个父节点，则遍历其子节点
        else
        {
            load_json_object(value, current_path + "/" + it.key());
        }
    }
}

/**
 * @brief 将当前的参数设置保存到文件
 * @param json_path 要保存到的路径
 * @return 保存成功则返回true，否则返回false
 */
bool ParameterManager::save_file(QString json_path)
{
    QJsonObject root_object = json_doc.object();
    save_json_object(root_object["parameters"]);
    json_doc.setObject(root_object);
    QFile file(json_path);
    if(file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        file.write(json_doc.toJson());
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef ParameterManager_DEBUG
void ParameterManager::print_all_para_debug()
{
    foreach (ParaStruct para, parameters) {
        qDebug() << para.path <<  para.name << para.enum_id << para.limit << para.value << para.enum_table;
    }
}

void ParameterManager::to_tree_widget_debug(QTreeWidget *tree, QTreeWidgetItem *parent_item, const QJsonObject object)
{
    for(auto it = object.begin(); it != object.end(); it++)
    {
        QJsonObject value = it.value().toObject();
        //若这是一个参数节点，则读取并保存此节点
        if(value.keys().contains("enum_id"))
        {
            int enum_id = value["enum_id"].toInt();
            ParaStruct para_struct;
            get_parameter_struct(static_cast<Parameter>(enum_id), para_struct);

            QTreeWidgetItem *item = new QTreeWidgetItem(parent_item);
            item->setText(0, para_struct.name);
            item->setText(1, QString::number(para_struct.enum_id));
            item->setText(2, para_struct.value.typeName());
            item->setText(3, para_struct.value.toString());
            item->setText(4, QString::number(para_struct.limit, 8));
            if(value.keys().contains("enum_table_name"))
            {
                QJsonDocument doc_tem;
                doc_tem.setObject(enum_tables[value["enum_table_name"].toString()].toObject());
                item->setText(5, doc_tem.toJson(QJsonDocument::Compact));
            }
        }
        //若这是一个父节点，则遍历其子节点
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(parent_item);
            item->setText(0, it.key());
            to_tree_widget_debug(tree, item, value);
        }
    }
}

void ParameterManager::to_tree_widget_debug(QTreeWidget *tree)
{
    tree->clear();
    tree->setColumnCount(0);
    tree->setHeaderLabels(QStringList{"name", "enum_id", "type", "value", "limit", "enum_table_name"});
    QTreeWidgetItem *top_item = new QTreeWidgetItem(tree);
    top_item->setText(0, "parameters");
    QJsonObject root_object = json_doc.object();
    to_tree_widget_debug(tree, top_item, root_object["parameters"].toObject());
    tree->expandAll();
    tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
#endif

bool ParameterManager::get_parameter(Parameter para, QVariant &value)
{
    ParaStruct para_struct;
    if(get_parameter_struct(para, para_struct))
    {
        value = para_struct.value;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 读取某一参数的结构体，所有对参数的读操作最终都是调用此函数
 * @param para 参数enum值
 * @param para_struct 参数的结构体引用，读取出的内容存到这里
 * @return 读取成功返回true，否则返回false
 */
bool ParameterManager::get_parameter_struct(Parameter para, ParaStruct &para_struct)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;
    else
    {
        para_struct = parameters[index];
        return true;
    }
}

/**
 * @brief 设置某一参数的值，所有对参数的写操作最终都是调用此函数
 * @param para 参数enum值
 * @param value 参数的值，注意此函数不会检查参数的数据类型和之前是否一样
 * @return 写成功返回true，否则返回false
 */
bool ParameterManager::set_parameter(Parameter para, const QVariant &value)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;
    else
    {
        parameters[index].value = value;
        return true;
    }
}

/**
 * @brief 获取某个参数的值，获取前检查权限
 * @param para 参数enum值
 * @param value QVariant变量的引用，获取到的值存到这里
 * @param role 发出请求的角色
 * @return 获取成功（有权限+参数存在）则返回true，否则返回false
 */
bool ParameterManager::get_parameter(Parameter para, QVariant &value, Role role)
{
    if(check_permission(para, Read, role))
        return get_parameter(para, value);
    else
        return false;
}

/**
 * @brief 获取某个参数的结构体，获取前检查权限
 * @param para 参数enum值
 * @param value ParaStruct变量的引用，获取到的值存到这里
 * @param role 发出请求的角色
 * @return 获取成功（有权限+参数存在）则返回true，否则返回false
 */
bool ParameterManager::get_parameter_struct(Parameter para, ParaStruct &para_struct, Role role)
{
    if(check_permission(para, Read, role))
        return get_parameter_struct(para, para_struct);
    else
        return false;
}

/**
 * @brief 修改某个参数的值，修改前检查权限
 * @param para 参数enum值
 * @param value 参数的值
 * @param role 发出请求的角色
 * @return 写入成功（有权限+参数存在）则返回true，否则返回false
 * @warning 此函数只是修改内存中的参数值，并不会保存到文件,
 *              如果要保存到文件请调用 save_file()
 */
bool ParameterManager::set_parameter(Parameter para, const QVariant &value, Role role)
{
    if(check_permission(para, Write, role))
        return set_parameter(para, value);
    else
        return false;
}

/**
 * @brief 检查对某个参数是否有进行某种操作的权限
 */
bool ParameterManager::check_permission(Parameter para, RolePermission role_permission)
{
    ParaStruct para_struct;
    if(!get_parameter_struct(para, para_struct))
        return false;

    if(para_struct.limit & role_permission)
        return true;
    else
        return false;
}

/**
 * @brief 检查对某个参数是否有进行某种操作的权限
 */
bool ParameterManager::check_permission(Parameter para, Permission permission, Role role)
{
    if(role == SuperRoot)
        return true;
    else
        return check_permission(para, static_cast<RolePermission>(permission << role));
}

/**
 * @brief 绑定实际值的指针，可读可写
 * @param para  参数的枚举值
 * @param src  QVariant指针
 * @return  绑定成功则返回true，若参数不存在则返回false
 * @note  在绑定之前需要先 load_file 识别所有参数
 *          之前绑定过的指针会被覆盖
 * @warning 在尝试读写实际值时（若有权限）
 *              会先尝试使用 bind_parameter_get 和 bind_parameter_set 绑定的函数
 *               未绑定get或set时，才会尝试使用 bind_parameter_src 绑定的变量指针
 */
bool ParameterManager::bind_parameter_src(Parameter para, QVariant *src)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;

    parameters[index].src = src;
    return true;
}

/**
 * @brief 获取某个参数的传感器实际值
 * @param para 参数的枚举值
 * @param value 读取出的值的存放位置
 * @param role 访问角色
 * @return 读取成功则返回true。当权限不足、参数不存在、未绑定获取函数时返回false
 */
bool ParameterManager::get_parameter_real(Parameter para, QVariant &value, Role role)
{
    if(check_permission(para, Read, role))
        return get_parameter_real(para, value);
    else
        return false;
}

/**
 * @brief 设置某个参数的传感器实际值
 * @param para 参数的枚举值
 * @param value 读取出的值的存放位置
 * @param role 访问角色
 * @return 设置0成功则返回true。当权限不足、参数不存在、未绑定设置函数时返回false
 */
bool ParameterManager::set_parameter_real(Parameter para, QVariant &value, Role role)
{
    if(check_permission(para, Write, role))
        return set_parameter_real(para, value);
    else
        return false;
}

/**
 * @brief 获取某一参数的实际值
 * @param para 参数enum值
 * @param value 参数的值，注意此函数不会检查参数的数据类型和之前是否一样
 * @return 写成功返回true，否则返回false
 * @note 需要绑定获取实际值的函数，见bind_parameter_get函数
 */
bool ParameterManager::get_parameter_real(Parameter para, QVariant &value)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;
    else
    {
        if(parameters[index].get)
            return parameters[index].get(value);
        else if(parameters[index].src)
        {
            value = *parameters[index].src;
            return true;
        }
        else
            return false;
    }
}

/**
 * @brief 设置某一参数的实际值
 * @param para 参数enum值
 * @param value 参数的值，注意此函数不会检查参数的数据类型和之前是否一样
 * @return 写成功返回true，否则返回false
 * @note 需要绑定设置实际值的函数，见bind_parameter_set函数
 */
bool ParameterManager::set_parameter_real(Parameter para, QVariant &value)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;
    else
    {
        if(parameters[index].set)
            return parameters[index].set(value);
        else if(parameters[index].src)
        {
            *parameters[index].src = value;
            return true;
        }
        else
            return false;
    }
}

/**
 * @brief 设置 获取实际值 的回调函数
 * @param para enum值
 * @param get 由 std::function 封装的回调函数，返回值为设置成功与否的bool变量
 * @return 绑定成功返回true，参数不存在则返回false
 * @example 建议使用 lambda 表达式
 *      bind_parameter_get(para, [this](QVariant &value)->bool{
 *          value = ui->doubleSpinBox->value();
 *          return true;
 *      });
 * @note
 *      - 之前绑定过的回调函数会被覆盖
 *      - 除了lambda表达式以外，其他一切能被std::function封装的函数均可使用（只要返回值类型和参数类型符合要求）
 *          例如：类静态函数、普通函数、由std::bind组合成的对象
 */
bool ParameterManager::bind_parameter_get(Parameter para, std::function<bool(QVariant &)> get){
    int index = parameter_find(para);
    if(index == -1)
        return false;
    parameters[index].get = get;
    return true;
}

/**
 * @brief 设置 设置实际值 的回调函数
 * @param para enum值
 * @param set 由 std::function 封装的回调函数，返回值为设置成功与否的bool变量
 * @return 绑定成功返回true，参数不存在则返回false
 * @example 建议使用 lambda 表达式
 *      bind_parameter_get(para, [this](QVariant &value)->bool{
 *          ui->doubleSpinBox->setValue(value.toDouble());
 *          return true;
 *      });
 * @note
 *      - 之前绑定过的回调函数会被覆盖
 *      - 除了lambda表达式以外，其他一切能被std::function封装的函数均可使用（只要返回值类型和参数类型符合要求）
 *          例如：类静态函数、普通函数、由std::bind组合成的对象
 */
bool ParameterManager::bind_parameter_set(Parameter para, std::function<bool(QVariant &)> set){
    int index = parameter_find(para);
    if(index == -1)
        return false;
    parameters[index].set = set;
    return true;
}

/**
 * @brief 清除所有读写实际值的回调函数
 * @param para enum值
 * @return 清除成功返回true，参数不存在则返回false
 */
bool ParameterManager::bind_parameter_clear(Parameter para)
{
    int index = parameter_find(para);
    if(index == -1)
        return false;
    parameters[index].get = std::function<bool(QVariant &)>();
    parameters[index].set = std::function<bool(QVariant &)>();
    parameters[index].src = nullptr;
    return true;
}
