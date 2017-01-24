#include "utils.hpp"

void Utils::fixQToolButtons(std::vector<QToolButton *> &btns) {
  for (auto i : btns) {
    if (!i)
      continue;
    i->setPopupMode(QToolButton::InstantPopup);
  }
}

void Utils::fixQToolButtons(std::vector<QAction *> &actions, QToolBar *bar) {
  std::vector<QToolButton *> btns;
  btns.reserve(actions.size());
  for (auto i : actions) {
    if (!i)
      continue;
    btns.emplace_back(dynamic_cast<QToolButton *>(bar->widgetForAction(i)));
  }
  Utils::fixQToolButtons(btns);
}
