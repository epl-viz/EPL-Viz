#ifndef TIMELINESLIDER_H
#define TIMELINESLIDER_H

#include <QAbstractSlider>

namespace Ui {
class TimelineSlider;
}

class TimelineSlider : public QAbstractSlider {
  Q_OBJECT

 public:
  TimelineSlider( QWidget *parent = nullptr ) : QAbstractSlider( parent ) {} //!\todo fix
  ~TimelineSlider()               = default;                                 //!\todo fix
};

#endif // QTIMELINESLIDER_H
