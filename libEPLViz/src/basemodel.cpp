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
 * \file basemodel.cpp
 */

#include "basemodel.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

QLinkedList<BaseModel *> *BaseModel::registeredModels = new QLinkedList<BaseModel *>;

BaseModel::BaseModel() { reg(this); }

BaseModel::~BaseModel() { dereg(this); }

void BaseModel::updateAll(GUIState *state) {
  (void)state;
  // TODO get current Cycle
  Cycle *                          c = nullptr;
  QLinkedListIterator<BaseModel *> iterator(*registeredModels);
  while (iterator.hasNext()) {
    iterator.next()->update(c);
  }
}

void BaseModel::reg(BaseModel *model) {
  if (!registeredModels->contains(model)) {
    qDebug() << "Registered a model";
    registeredModels->append(model);
  } else
    throw std::runtime_error("Cannot add a model twice!");
}

void BaseModel::dereg(BaseModel *model) {
  registeredModels->removeOne(model);
  qDebug() << "Deregistered a model";
}

bool BaseModel::operator==(const BaseModel &other) { return this == &other; }
