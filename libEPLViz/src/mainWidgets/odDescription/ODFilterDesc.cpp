#include "ODFilterDesc.hpp"
#include "MainWindow.hpp"
#include "ODDescriptionItem.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

ODFilterDesc::ODFilterDesc(QObject *parent, MainWindow *main)
    : QSortFilterProxyModel(parent), mw(main), filter(FilterType::EXCLUDE, "") {}

bool ODFilterDesc::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
  std::lock_guard<std::recursive_mutex> lock(*const_cast<std::recursive_mutex *>(&mutex));

  QModelIndex        index = sourceModel()->index(sourceRow, 0, sourceParent);
  TreeModelItemBase *it    = static_cast<TreeModelItemBase *>(index.internalPointer());
  ODDescriptionItem *d     = dynamic_cast<ODDescriptionItem *>(it);

  if (!d) {
    return false;
  }

  return const_cast<CSViewFilters::Filter *>(&filter)->includeIndex(d->getIndex());
}

bool ODFilterDesc::updateFilter() {
  std::lock_guard<std::recursive_mutex> lock(mutex);

  std::string oldFilter = filter.getName();
  filter                = mw->getFilter();
  return filter.getName() != oldFilter;
}
