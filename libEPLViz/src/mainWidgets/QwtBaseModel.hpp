/* Copyright (c) 2017, EPL-Vizards
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the EPL-Vizards nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL EPL-Vizards BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 * \file QwtBaseModel.hpp
 */
#pragma once

#include "BaseModel.hpp"
#include "CSTimeSeriesPtr.hpp"
#include "EPLVizDefines.hpp"
#include "EventLog.hpp"
#include "TimeSeries.hpp"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>


#include "QCoreApplication"
#include <QEvent>
#include <QObject>

namespace EPL_Viz {

class MainWindow;

class QwtBaseModel : public QObject, public BaseModel {
  Q_OBJECT

 public:
  uint32_t maxXValue;

  QwtBaseModel(MainWindow *win, QwtPlot *widget);
  QwtBaseModel()  = delete;
  ~QwtBaseModel() = default;

  void    init() override;
  QString getName() override { return "QwtBaseModel"; }
  double  getViewportSize();

 protected:
  MainWindow *                                          window;
  QwtPlot *                                             plot;
  std::shared_ptr<QwtPlotCurve>                         curve;
  std::shared_ptr<EPL_DataCollect::plugins::TimeSeries> timeSeries;

  bool setupUsed;

  uint8_t     node;
  uint16_t    index;
  uint16_t    subindex;
  bool        odTS;
  std::string csName;

  bool created = false;

  virtual void update(ProtectedCycle &cycle) override;
  void initTS();

  template <typename F>
  static void postToThread(F &&fun, QObject *obj) {
    struct Event : public QEvent {
      typedef typename std::decay<F>::type Fun;
      Fun                                  funEl;
      Event(Fun &&funny) : QEvent(QEvent::None), funEl(std::move(funny)) {}
      Event(const Fun &funny) : QEvent(QEvent::None), funEl(funny) {}
      ~Event() { funEl(); }
    };
    QCoreApplication::postEvent(obj, new Event(std::forward<F>(fun)));
  }

 signals:
  void requestRedraw();
  void maxValueChanged(int notUsed, int nMax);
  void setNodes(uint8_t nodeID, uint16_t index, uint16_t subIndex);

 public slots:
  virtual void createPlot(uint8_t nodeID, uint16_t index, uint16_t subIndex);
  void setupPlotting();
  void replot();
  void reset();
};
}
