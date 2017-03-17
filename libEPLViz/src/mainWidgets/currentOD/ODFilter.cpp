#include "ODFilter.hpp"
#include "CurODModelItem.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

ODFilter::ODFilter(QObject *parent, MainWindow *main)
    : QSortFilterProxyModel(parent), mw(main), filter(FilterType::EXCLUDE, "") {}

bool ODFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
  std::lock_guard<std::recursive_mutex> lock(*const_cast<std::recursive_mutex *>(&mutex));

  QModelIndex        index = sourceModel()->index(sourceRow, 0, sourceParent);
  TreeModelItemBase *it    = static_cast<TreeModelItemBase *>(index.internalPointer());
  CurODModelItem *   d     = dynamic_cast<CurODModelItem *>(it);

  if (!d) {
    return true;
  }

  return const_cast<CSViewFilters::Filter *>(&filter)->includeIndex(d->getIndex());
}

bool ODFilter::updateFilter() {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  std::string oldFilter = filter.getName();
  filter                = mw->getFilter();
  return filter.getName() != oldFilter;
}
