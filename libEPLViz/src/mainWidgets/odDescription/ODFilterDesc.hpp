#pragma once

#include "CSViewFilters.hpp"
#include <QSortFilterProxyModel>
#include <mutex>

namespace EPL_Viz {

class MainWindow;

class ODFilterDesc : public QSortFilterProxyModel {
 private:
  MainWindow *                           mw;
  std::recursive_mutex                   mutex;
  EPL_DataCollect::CSViewFilters::Filter filter;

 protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

 public:
  ODFilterDesc() = delete;
  ODFilterDesc(QObject *parent, MainWindow *main);

  bool updateFilter();
};
}
