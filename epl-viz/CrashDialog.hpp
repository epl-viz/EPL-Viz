#pragma once

#include <QDialog>
#include <QTextEdit>

namespace Ui {
class CrashDialog;
}

class CrashDialog : public QDialog {
  Q_OBJECT
 private:
  Ui::CrashDialog *ui;

  void setTextTermFormatting(QTextEdit *textEdit, QString const &text);
  void parseEscapeSequence(int                     attribute,
                           QListIterator<QString> &i,
                           QTextCharFormat &       textCharFormat,
                           QTextCharFormat const & defaultTextCharFormat);

 public:
  explicit CrashDialog(QString text, QWidget *parent = 0);
  ~CrashDialog();
};
