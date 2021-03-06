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
 * \file BaseModel.hpp
 */

#pragma once

#include "CaptureInstance.hpp"
#include "Cycle.hpp"
#include "CycleContainer.hpp"
#include "EPLVizDefines.hpp"
#include "GUIState.hpp"
#include <QDebug>
#include <QLinkedList>
#include <QMainWindow>
#include <QTextEdit>
#include <QWidget>
#include <shared_mutex>

#include <QCoreApplication>
#include <QEvent>
#include <QObject>

namespace EPL_Viz {

class MainWindow;

class ProtectedCycle final {
 private:
  EPL_DataCollect::Cycle  c;
  std::shared_timed_mutex mut;

 public:
  ProtectedCycle()  = default;
  ~ProtectedCycle() = default;

  ProtectedCycle(const ProtectedCycle &) = delete;
  ProtectedCycle(ProtectedCycle &&)      = delete;

  ProtectedCycle &operator=(const ProtectedCycle &) = delete;
  ProtectedCycle &operator=(ProtectedCycle &&) = delete;

  inline void updateCycle(EPL_DataCollect::CaptureInstance *instance, uint32_t cycleNum) {
    std::unique_lock<std::shared_timed_mutex> lk(mut);

    auto *container = instance->getCycleContainer();

    if (cycleNum == UINT32_MAX)
      c = container->pollCycle();
    else
      c = container->getCycle(cycleNum);
  }

  inline std::shared_lock<std::shared_timed_mutex> getLock() { return std::shared_lock<std::shared_timed_mutex>(mut); }

  inline EPL_DataCollect::Cycle *getCycle() { return &c; }

  inline EPL_DataCollect::Cycle *operator*() noexcept { return getCycle(); }
  inline EPL_DataCollect::Cycle *operator->() noexcept { return getCycle(); }
};

class ModelThread;

class BaseModel {

 private:
  static QLinkedList<BaseModel *> registeredModels;
  static uint32_t                 appID;
  static ProtectedCycle           cycle;
  static bool                     forceUpdate;
  static std::mutex               updateLocker;
  MainWindow *                    mainWindow;

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif

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

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
#pragma clang diagnostic push
#endif

 public:
  BaseModel(MainWindow *mw, QWidget *widget);
  virtual ~BaseModel();

  inline bool operator==(const BaseModel &other);
  virtual QString getName() = 0;

  virtual bool needUpdateAlways();

  static std::unique_lock<std::mutex> getUpdateLock() { return std::unique_lock<std::mutex>(updateLocker); }
  MainWindow *                        getMainWindow();

 protected:
  virtual void update()       = 0;
  virtual void updateWidget() = 0;
  virtual void init()         = 0;

  static void reg(BaseModel *model);
  static void dereg(BaseModel *model);

  static void forceNextUpdate() { forceUpdate = true; }

  static ProtectedCycle &getCurrentCycle();

 private:
  static bool updateAll(MainWindow *mw, EPL_DataCollect::CaptureInstance *instance);
  static void updateAllWidgets(MainWindow *mw);

  static void initAll();

  friend MainWindow;  // Allow MainWindow to init
  friend ModelThread; // Allow ModelThread to update

 signals:
  void updateCompleted();
};
}
