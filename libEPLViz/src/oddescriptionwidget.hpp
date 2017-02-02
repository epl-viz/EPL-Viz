#ifndef ODDESCRIPTIONWIDGET_HPP
#define ODDESCRIPTIONWIDGET_HPP

#include <QDockWidget>

class ODDescriptionWidget : public QDockWidget {
  Q_OBJECT

 public:
  ODDescriptionWidget(QWidget *parent = nullptr);
  ~ODDescriptionWidget()              = default;
};

#endif // ODDESCRIPTIONWIDGET_HPP
