/********************************************************************************
** Form generated from reading UI file 'myGui-designer.ui'
**
** Created: Fri Aug 31 16:29:35 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYGUI_2D_DESIGNER_H
#define UI_MYGUI_2D_DESIGNER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDial>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *labelA;
    QLineEdit *textOutput;
    QCheckBox *red1;
    QCheckBox *yellow1;
    QCheckBox *green1;
    QPushButton *pedastrian1;
    QCheckBox *green2;
    QCheckBox *yellow2;
    QCheckBox *red2;
    QPushButton *pedastrian2;
    QLineEdit *lineEdit;
    QLabel *labelA_2;
    QDial *dial;
    QSlider *horizontalSlider;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(434, 369);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        labelA = new QLabel(centralwidget);
        labelA->setObjectName(QString::fromUtf8("labelA"));
        labelA->setGeometry(QRect(40, 20, 111, 17));
        textOutput = new QLineEdit(centralwidget);
        textOutput->setObjectName(QString::fromUtf8("textOutput"));
        textOutput->setGeometry(QRect(30, 40, 181, 27));
        QPalette palette;
        QBrush brush(QColor(0, 170, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(133, 133, 133, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        textOutput->setPalette(palette);
        red1 = new QCheckBox(centralwidget);
        red1->setObjectName(QString::fromUtf8("red1"));
        red1->setGeometry(QRect(40, 90, 105, 22));
        red1->setAutoRepeatDelay(300);
        yellow1 = new QCheckBox(centralwidget);
        yellow1->setObjectName(QString::fromUtf8("yellow1"));
        yellow1->setGeometry(QRect(40, 120, 105, 22));
        yellow1->setAutoRepeatDelay(300);
        green1 = new QCheckBox(centralwidget);
        green1->setObjectName(QString::fromUtf8("green1"));
        green1->setGeometry(QRect(40, 150, 105, 22));
        green1->setAutoRepeatDelay(300);
        pedastrian1 = new QPushButton(centralwidget);
        pedastrian1->setObjectName(QString::fromUtf8("pedastrian1"));
        pedastrian1->setGeometry(QRect(280, 90, 88, 27));
        green2 = new QCheckBox(centralwidget);
        green2->setObjectName(QString::fromUtf8("green2"));
        green2->setGeometry(QRect(150, 150, 105, 22));
        green2->setAutoRepeatDelay(300);
        yellow2 = new QCheckBox(centralwidget);
        yellow2->setObjectName(QString::fromUtf8("yellow2"));
        yellow2->setGeometry(QRect(150, 120, 105, 22));
        yellow2->setAutoRepeatDelay(300);
        red2 = new QCheckBox(centralwidget);
        red2->setObjectName(QString::fromUtf8("red2"));
        red2->setGeometry(QRect(150, 90, 105, 22));
        red2->setAutoRepeatDelay(300);
        pedastrian2 = new QPushButton(centralwidget);
        pedastrian2->setObjectName(QString::fromUtf8("pedastrian2"));
        pedastrian2->setGeometry(QRect(280, 130, 88, 27));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 230, 241, 25));
        labelA_2 = new QLabel(centralwidget);
        labelA_2->setObjectName(QString::fromUtf8("labelA_2"));
        labelA_2->setGeometry(QRect(20, 200, 111, 17));
        dial = new QDial(centralwidget);
        dial->setObjectName(QString::fromUtf8("dial"));
        dial->setGeometry(QRect(300, 200, 50, 64));
        horizontalSlider = new QSlider(centralwidget);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(20, 270, 251, 25));
        horizontalSlider->setOrientation(Qt::Horizontal);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 434, 27));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        labelA->setText(QApplication::translate("MainWindow", "Any Text, eg. time", 0, QApplication::UnicodeUTF8));
        red1->setText(QApplication::translate("MainWindow", "Red", 0, QApplication::UnicodeUTF8));
        yellow1->setText(QApplication::translate("MainWindow", "Yellow", 0, QApplication::UnicodeUTF8));
        green1->setText(QApplication::translate("MainWindow", "Green", 0, QApplication::UnicodeUTF8));
        pedastrian1->setText(QApplication::translate("MainWindow", "pedastrian1", 0, QApplication::UnicodeUTF8));
        green2->setText(QApplication::translate("MainWindow", "Green", 0, QApplication::UnicodeUTF8));
        yellow2->setText(QApplication::translate("MainWindow", "Yellow", 0, QApplication::UnicodeUTF8));
        red2->setText(QApplication::translate("MainWindow", "Red", 0, QApplication::UnicodeUTF8));
        pedastrian2->setText(QApplication::translate("MainWindow", "pedastrian2", 0, QApplication::UnicodeUTF8));
        labelA_2->setText(QApplication::translate("MainWindow", "Type any text", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYGUI_2D_DESIGNER_H
