#pragma once

#include <QAction>
#include <QToolBar>
#include <QToolButton>

#include <vector>

class Utils final {
 public:
  Utils() = delete; // You must not create an object from this class

  static void fixQToolButtons(std::vector<QToolButton *> &btns);
  static void fixQToolButtons(std::vector<QAction *> &actions, QToolBar *bar);
};
