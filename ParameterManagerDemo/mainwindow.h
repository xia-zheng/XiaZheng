/*
 * @Author       : XiaZheng <xiazheng.hhu@qq.com>
 * @Date         : 2023-05-01 21:11:33
 * @LastEditTime : 2023-05-10 14:53:50
 * @FilePath     : \XiaZheng\ParameterManagerDemo\mainwindow.h
 * @Description  : 
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
