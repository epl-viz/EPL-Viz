#include "interfacepicker.hpp"
#include "ui_interfacepicker.h"
using namespace EPL_Viz;

InterfacePicker::InterfacePicker(QWidget *parent) : QDialog(parent), ui(new Ui::InterfacePicker) { ui->setupUi(this); }

InterfacePicker::~InterfacePicker() { delete ui; }
