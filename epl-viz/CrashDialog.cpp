#include "CrashDialog.hpp"
#include "ui_CrashDialog.h"
#include <QFontDatabase>
#include <QTextEdit>

// Escape sequence converter from here:
// https://stackoverflow.com/questions/26500429/qtextedit-and-colored-bash-like-output-emulation

// based on information: http://en.m.wikipedia.org/wiki/ANSI_escape_code
// http://misc.flogisoft.com/bash/tip_colors_and_formatting http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
void CrashDialog::parseEscapeSequence(int                     attribute,
                                      QListIterator<QString> &i,
                                      QTextCharFormat &       textCharFormat,
                                      QTextCharFormat const & defaultTextCharFormat) {
  switch (attribute) {
    case 0: { // Normal/Default (reset all attributes)
      textCharFormat = defaultTextCharFormat;
      break;
    }
    case 1: { // Bold/Bright (bold or increased intensity)
      textCharFormat.setFontWeight(QFont::Bold);
      break;
    }
    case 2: { // Dim/Faint (decreased intensity)
      textCharFormat.setFontWeight(QFont::Light);
      break;
    }
    case 3: { // Italicized (italic on)
      textCharFormat.setFontItalic(true);
      break;
    }
    case 4: { // Underscore (single underlined)
      textCharFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
      textCharFormat.setFontUnderline(true);
      break;
    }
    case 5: { // Blink (slow, appears as Bold)
      textCharFormat.setFontWeight(QFont::Bold);
      break;
    }
    case 6: { // Blink (rapid, appears as very Bold)
      textCharFormat.setFontWeight(QFont::Black);
      break;
    }
    case 7: { // Reverse/Inverse (swap foreground and background)
      QBrush foregroundBrush = textCharFormat.foreground();
      textCharFormat.setForeground(textCharFormat.background());
      textCharFormat.setBackground(foregroundBrush);
      break;
    }
    case 8: { // Concealed/Hidden/Invisible (usefull for passwords)
      textCharFormat.setForeground(textCharFormat.background());
      break;
    }
    case 9: { // Crossed-out characters
      textCharFormat.setFontStrikeOut(true);
      break;
    }
    case 10: { // Primary (default) font
      textCharFormat.setFont(defaultTextCharFormat.font());
      break;
    }
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19: {
      QFontDatabase fontDatabase;
      QString       fontFamily     = textCharFormat.fontFamily();
      QStringList   fontStyles     = fontDatabase.styles(fontFamily);
      int           fontStyleIndex = attribute - 11;
      if (fontStyleIndex < fontStyles.length()) {
        textCharFormat.setFont(
              fontDatabase.font(fontFamily, fontStyles.at(fontStyleIndex), textCharFormat.font().pointSize()));
      }
      break;
    }
    case 20: { // Fraktur (unsupported)
      break;
    }
    case 21: { // Set Bold off
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 22: { // Set Dim off
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 23: { // Unset italic and unset fraktur
      textCharFormat.setFontItalic(false);
      break;
    }
    case 24: { // Unset underlining
      textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
      textCharFormat.setFontUnderline(false);
      break;
    }
    case 25: { // Unset Blink/Bold
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 26: { // Reserved
      break;
    }
    case 27: { // Positive (non-inverted)
      QBrush backgroundBrush = textCharFormat.background();
      textCharFormat.setBackground(textCharFormat.foreground());
      textCharFormat.setForeground(backgroundBrush);
      break;
    }
    case 28: {
      textCharFormat.setForeground(defaultTextCharFormat.foreground());
      textCharFormat.setBackground(defaultTextCharFormat.background());
      break;
    }
    case 29: {
      textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
      textCharFormat.setFontUnderline(false);
      break;
    }
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37: {
      int    colorIndex = attribute - 30;
      QColor color;
      if (QFont::Normal < textCharFormat.fontWeight()) {
        switch (colorIndex) {
          case 0: {
            color = Qt::darkGray;
            break;
          }
          case 1: {
            color = Qt::red;
            break;
          }
          case 2: {
            color = Qt::green;
            break;
          }
          case 3: {
            color = Qt::yellow;
            break;
          }
          case 4: {
            color = Qt::blue;
            break;
          }
          case 5: {
            color = Qt::magenta;
            break;
          }
          case 6: {
            color = Qt::cyan;
            break;
          }
          case 7: {
            color = Qt::white;
            break;
          }
          default: { Q_ASSERT(false); }
        }
      } else {
        switch (colorIndex) {
          case 0: {
            color = Qt::black;
            break;
          }
          case 1: {
            color = Qt::darkRed;
            break;
          }
          case 2: {
            color = Qt::darkGreen;
            break;
          }
          case 3: {
            color = Qt::darkYellow;
            break;
          }
          case 4: {
            color = Qt::darkBlue;
            break;
          }
          case 5: {
            color = Qt::darkMagenta;
            break;
          }
          case 6: {
            color = Qt::darkCyan;
            break;
          }
          case 7: {
            color = Qt::lightGray;
            break;
          }
          default: { Q_ASSERT(false); }
        }
      }
      textCharFormat.setForeground(color);
      break;
    }
    case 38: {
      if (i.hasNext()) {
        bool ok       = false;
        int  selector = i.next().toInt(&ok);
        Q_ASSERT(ok);
        QColor color;
        switch (selector) {
          case 2: {
            if (!i.hasNext()) {
              break;
            }
            int red = i.next().toInt(&ok);
            Q_ASSERT(ok);
            if (!i.hasNext()) {
              break;
            }
            int green = i.next().toInt(&ok);
            Q_ASSERT(ok);
            if (!i.hasNext()) {
              break;
            }
            int blue = i.next().toInt(&ok);
            Q_ASSERT(ok);
            color.setRgb(red, green, blue);
            break;
          }
          case 5: {
            if (!i.hasNext()) {
              break;
            }
            int index = i.next().toInt(&ok);
            Q_ASSERT(ok);
            switch (index) {
              case 0x00:
              case 0x01:
              case 0x02:
              case 0x03:
              case 0x04:
              case 0x05:
              case 0x06:
              case 0x07: { // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x08:
              case 0x09:
              case 0x0A:
              case 0x0B:
              case 0x0C:
              case 0x0D:
              case 0x0E:
              case 0x0F: { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x10:
              case 0x11:
              case 0x12:
              case 0x13:
              case 0x14:
              case 0x15:
              case 0x16:
              case 0x17:
              case 0x18:
              case 0x19:
              case 0x1a:
              case 0x1b:
              case 0x1c:
              case 0x1d:
              case 0x1e:
              case 0x1f:
              case 0x20:
              case 0x21:
              case 0x22:
              case 0x23:
              case 0x24:
              case 0x25:
              case 0x26:
              case 0x27:
              case 0x28:
              case 0x29:
              case 0x2a:
              case 0x2b:
              case 0x2c:
              case 0x2d:
              case 0x2e:
              case 0x2f:
              case 0x30:
              case 0x31:
              case 0x32:
              case 0x33:
              case 0x34:
              case 0x35:
              case 0x36:
              case 0x37:
              case 0x38:
              case 0x39:
              case 0x3a:
              case 0x3b:
              case 0x3c:
              case 0x3d:
              case 0x3e:
              case 0x3f:
              case 0x40:
              case 0x41:
              case 0x42:
              case 0x43:
              case 0x44:
              case 0x45:
              case 0x46:
              case 0x47:
              case 0x48:
              case 0x49:
              case 0x4a:
              case 0x4b:
              case 0x4c:
              case 0x4d:
              case 0x4e:
              case 0x4f:
              case 0x50:
              case 0x51:
              case 0x52:
              case 0x53:
              case 0x54:
              case 0x55:
              case 0x56:
              case 0x57:
              case 0x58:
              case 0x59:
              case 0x5a:
              case 0x5b:
              case 0x5c:
              case 0x5d:
              case 0x5e:
              case 0x5f:
              case 0x60:
              case 0x61:
              case 0x62:
              case 0x63:
              case 0x64:
              case 0x65:
              case 0x66:
              case 0x67:
              case 0x68:
              case 0x69:
              case 0x6a:
              case 0x6b:
              case 0x6c:
              case 0x6d:
              case 0x6e:
              case 0x6f:
              case 0x70:
              case 0x71:
              case 0x72:
              case 0x73:
              case 0x74:
              case 0x75:
              case 0x76:
              case 0x77:
              case 0x78:
              case 0x79:
              case 0x7a:
              case 0x7b:
              case 0x7c:
              case 0x7d:
              case 0x7e:
              case 0x7f:
              case 0x80:
              case 0x81:
              case 0x82:
              case 0x83:
              case 0x84:
              case 0x85:
              case 0x86:
              case 0x87:
              case 0x88:
              case 0x89:
              case 0x8a:
              case 0x8b:
              case 0x8c:
              case 0x8d:
              case 0x8e:
              case 0x8f:
              case 0x90:
              case 0x91:
              case 0x92:
              case 0x93:
              case 0x94:
              case 0x95:
              case 0x96:
              case 0x97:
              case 0x98:
              case 0x99:
              case 0x9a:
              case 0x9b:
              case 0x9c:
              case 0x9d:
              case 0x9e:
              case 0x9f:
              case 0xa0:
              case 0xa1:
              case 0xa2:
              case 0xa3:
              case 0xa4:
              case 0xa5:
              case 0xa6:
              case 0xa7:
              case 0xa8:
              case 0xa9:
              case 0xaa:
              case 0xab:
              case 0xac:
              case 0xad:
              case 0xae:
              case 0xaf:
              case 0xb0:
              case 0xb1:
              case 0xb2:
              case 0xb3:
              case 0xb4:
              case 0xb5:
              case 0xb6:
              case 0xb7:
              case 0xb8:
              case 0xb9:
              case 0xba:
              case 0xbb:
              case 0xbc:
              case 0xbd:
              case 0xbe:
              case 0xbf:
              case 0xc0:
              case 0xc1:
              case 0xc2:
              case 0xc3:
              case 0xc4:
              case 0xc5:
              case 0xc6:
              case 0xc7:
              case 0xc8:
              case 0xc9:
              case 0xca:
              case 0xcb:
              case 0xcc:
              case 0xcd:
              case 0xce:
              case 0xcf:
              case 0xd0:
              case 0xd1:
              case 0xd2:
              case 0xd3:
              case 0xd4:
              case 0xd5:
              case 0xd6:
              case 0xd7:
              case 0xd8:
              case 0xd9:
              case 0xda:
              case 0xdb:
              case 0xdc:
              case 0xdd:
              case 0xde:
              case 0xdf:
              case 0xe0:
              case 0xe1:
              case 0xe2:
              case 0xe3:
              case 0xe4:
              case 0xe5:
              case 0xe6:
              case 0xe7: { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                index -= 0x10;
                int red = index % 6;
                index /= 6;
                int green = index % 6;
                index /= 6;
                int blue = index % 6;
                index /= 6;
                Q_ASSERT(index == 0);
                color.setRgb(red, green, blue);
                break;
              }
              case 0xe8:
              case 0xe9:
              case 0xea:
              case 0xeb:
              case 0xec:
              case 0xed:
              case 0xee:
              case 0xef:
              case 0xf0:
              case 0xf1:
              case 0xf2:
              case 0xf3:
              case 0xf4:
              case 0xf5:
              case 0xf6:
              case 0xf7:
              case 0xf8:
              case 0xf9:
              case 0xfa:
              case 0xfb:
              case 0xfc:
              case 0xfd:
              case 0xfe:
              case 0xff: { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                color.setRgbF(intensity, intensity, intensity);
                break;
              }
            }
            textCharFormat.setForeground(color);
            break;
          }
          default: { break; }
        }
      }
      break;
    }
    case 39: {
      textCharFormat.setForeground(defaultTextCharFormat.foreground());
      break;
    }
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47: {
      int    colorIndex = attribute - 40;
      QColor color;
      switch (colorIndex) {
        case 0: {
          color = Qt::darkGray;
          break;
        }
        case 1: {
          color = Qt::red;
          break;
        }
        case 2: {
          color = Qt::green;
          break;
        }
        case 3: {
          color = Qt::yellow;
          break;
        }
        case 4: {
          color = Qt::blue;
          break;
        }
        case 5: {
          color = Qt::magenta;
          break;
        }
        case 6: {
          color = Qt::cyan;
          break;
        }
        case 7: {
          color = Qt::white;
          break;
        }
        default: { Q_ASSERT(false); }
      }
      textCharFormat.setBackground(color);
      break;
    }
    case 48: {
      if (i.hasNext()) {
        bool ok       = false;
        int  selector = i.next().toInt(&ok);
        Q_ASSERT(ok);
        QColor color;
        switch (selector) {
          case 2: {
            if (!i.hasNext()) {
              break;
            }
            int red = i.next().toInt(&ok);
            Q_ASSERT(ok);
            if (!i.hasNext()) {
              break;
            }
            int green = i.next().toInt(&ok);
            Q_ASSERT(ok);
            if (!i.hasNext()) {
              break;
            }
            int blue = i.next().toInt(&ok);
            Q_ASSERT(ok);
            color.setRgb(red, green, blue);
            break;
          }
          case 5: {
            if (!i.hasNext()) {
              break;
            }
            int index = i.next().toInt(&ok);
            Q_ASSERT(ok);
            switch (index) {
              case 0x00:
              case 0x01:
              case 0x02:
              case 0x03:
              case 0x04:
              case 0x05:
              case 0x06:
              case 0x07: { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x08:
              case 0x09:
              case 0x0A:
              case 0x0B:
              case 0x0C:
              case 0x0D:
              case 0x0E:
              case 0x0F: { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x10:
              case 0x11:
              case 0x12:
              case 0x13:
              case 0x14:
              case 0x15:
              case 0x16:
              case 0x17:
              case 0x18:
              case 0x19:
              case 0x1a:
              case 0x1b:
              case 0x1c:
              case 0x1d:
              case 0x1e:
              case 0x1f:
              case 0x20:
              case 0x21:
              case 0x22:
              case 0x23:
              case 0x24:
              case 0x25:
              case 0x26:
              case 0x27:
              case 0x28:
              case 0x29:
              case 0x2a:
              case 0x2b:
              case 0x2c:
              case 0x2d:
              case 0x2e:
              case 0x2f:
              case 0x30:
              case 0x31:
              case 0x32:
              case 0x33:
              case 0x34:
              case 0x35:
              case 0x36:
              case 0x37:
              case 0x38:
              case 0x39:
              case 0x3a:
              case 0x3b:
              case 0x3c:
              case 0x3d:
              case 0x3e:
              case 0x3f:
              case 0x40:
              case 0x41:
              case 0x42:
              case 0x43:
              case 0x44:
              case 0x45:
              case 0x46:
              case 0x47:
              case 0x48:
              case 0x49:
              case 0x4a:
              case 0x4b:
              case 0x4c:
              case 0x4d:
              case 0x4e:
              case 0x4f:
              case 0x50:
              case 0x51:
              case 0x52:
              case 0x53:
              case 0x54:
              case 0x55:
              case 0x56:
              case 0x57:
              case 0x58:
              case 0x59:
              case 0x5a:
              case 0x5b:
              case 0x5c:
              case 0x5d:
              case 0x5e:
              case 0x5f:
              case 0x60:
              case 0x61:
              case 0x62:
              case 0x63:
              case 0x64:
              case 0x65:
              case 0x66:
              case 0x67:
              case 0x68:
              case 0x69:
              case 0x6a:
              case 0x6b:
              case 0x6c:
              case 0x6d:
              case 0x6e:
              case 0x6f:
              case 0x70:
              case 0x71:
              case 0x72:
              case 0x73:
              case 0x74:
              case 0x75:
              case 0x76:
              case 0x77:
              case 0x78:
              case 0x79:
              case 0x7a:
              case 0x7b:
              case 0x7c:
              case 0x7d:
              case 0x7e:
              case 0x7f:
              case 0x80:
              case 0x81:
              case 0x82:
              case 0x83:
              case 0x84:
              case 0x85:
              case 0x86:
              case 0x87:
              case 0x88:
              case 0x89:
              case 0x8a:
              case 0x8b:
              case 0x8c:
              case 0x8d:
              case 0x8e:
              case 0x8f:
              case 0x90:
              case 0x91:
              case 0x92:
              case 0x93:
              case 0x94:
              case 0x95:
              case 0x96:
              case 0x97:
              case 0x98:
              case 0x99:
              case 0x9a:
              case 0x9b:
              case 0x9c:
              case 0x9d:
              case 0x9e:
              case 0x9f:
              case 0xa0:
              case 0xa1:
              case 0xa2:
              case 0xa3:
              case 0xa4:
              case 0xa5:
              case 0xa6:
              case 0xa7:
              case 0xa8:
              case 0xa9:
              case 0xaa:
              case 0xab:
              case 0xac:
              case 0xad:
              case 0xae:
              case 0xaf:
              case 0xb0:
              case 0xb1:
              case 0xb2:
              case 0xb3:
              case 0xb4:
              case 0xb5:
              case 0xb6:
              case 0xb7:
              case 0xb8:
              case 0xb9:
              case 0xba:
              case 0xbb:
              case 0xbc:
              case 0xbd:
              case 0xbe:
              case 0xbf:
              case 0xc0:
              case 0xc1:
              case 0xc2:
              case 0xc3:
              case 0xc4:
              case 0xc5:
              case 0xc6:
              case 0xc7:
              case 0xc8:
              case 0xc9:
              case 0xca:
              case 0xcb:
              case 0xcc:
              case 0xcd:
              case 0xce:
              case 0xcf:
              case 0xd0:
              case 0xd1:
              case 0xd2:
              case 0xd3:
              case 0xd4:
              case 0xd5:
              case 0xd6:
              case 0xd7:
              case 0xd8:
              case 0xd9:
              case 0xda:
              case 0xdb:
              case 0xdc:
              case 0xdd:
              case 0xde:
              case 0xdf:
              case 0xe0:
              case 0xe1:
              case 0xe2:
              case 0xe3:
              case 0xe4:
              case 0xe5:
              case 0xe6:
              case 0xe7: { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                index -= 0x10;
                int red = index % 6;
                index /= 6;
                int green = index % 6;
                index /= 6;
                int blue = index % 6;
                index /= 6;
                Q_ASSERT(index == 0);
                color.setRgb(red, green, blue);
                break;
              }
              case 0xe8:
              case 0xe9:
              case 0xea:
              case 0xeb:
              case 0xec:
              case 0xed:
              case 0xee:
              case 0xef:
              case 0xf0:
              case 0xf1:
              case 0xf2:
              case 0xf3:
              case 0xf4:
              case 0xf5:
              case 0xf6:
              case 0xf7:
              case 0xf8:
              case 0xf9:
              case 0xfa:
              case 0xfb:
              case 0xfc:
              case 0xfd:
              case 0xfe:
              case 0xff: { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                color.setRgbF(intensity, intensity, intensity);
                break;
              }
            }
            textCharFormat.setBackground(color);
            break;
          }
          default: { break; }
        }
      }
      break;
    }
    case 49: {
      textCharFormat.setBackground(defaultTextCharFormat.background());
      break;
    }
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97: {
      int    colorIndex = attribute - 90;
      QColor color;
      switch (colorIndex) {
        case 0: {
          color = Qt::darkGray;
          break;
        }
        case 1: {
          color = Qt::red;
          break;
        }
        case 2: {
          color = Qt::green;
          break;
        }
        case 3: {
          color = Qt::yellow;
          break;
        }
        case 4: {
          color = Qt::blue;
          break;
        }
        case 5: {
          color = Qt::magenta;
          break;
        }
        case 6: {
          color = Qt::cyan;
          break;
        }
        case 7: {
          color = Qt::white;
          break;
        }
        default: { Q_ASSERT(false); }
      }
      color.setRedF(color.redF() * 0.8);
      color.setGreenF(color.greenF() * 0.8);
      color.setBlueF(color.blueF() * 0.8);
      textCharFormat.setForeground(color);
      break;
    }
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107: {
      int    colorIndex = attribute - 100;
      QColor color;
      switch (colorIndex) {
        case 0: {
          color = Qt::darkGray;
          break;
        }
        case 1: {
          color = Qt::red;
          break;
        }
        case 2: {
          color = Qt::green;
          break;
        }
        case 3: {
          color = Qt::yellow;
          break;
        }
        case 4: {
          color = Qt::blue;
          break;
        }
        case 5: {
          color = Qt::magenta;
          break;
        }
        case 6: {
          color = Qt::cyan;
          break;
        }
        case 7: {
          color = Qt::white;
          break;
        }
        default: { Q_ASSERT(false); }
      }
      color.setRedF(color.redF() * 0.8);
      color.setGreenF(color.greenF() * 0.8);
      color.setBlueF(color.blueF() * 0.8);
      textCharFormat.setBackground(color);
      break;
    }
    default: { break; }
  }
}

void CrashDialog::setTextTermFormatting(QTextEdit *textEdit, QString const &text) {
  QTextDocument *       document = textEdit->document();
  QRegExp const         escapeSequenceExpression(R"(\x1B\[([\d;]+)m)");
  QTextCursor           cursor(document);
  QTextCharFormat const defaultTextCharFormat = cursor.charFormat();
  cursor.beginEditBlock();
  int offset = escapeSequenceExpression.indexIn(text);
  cursor.insertText(text.mid(0, offset));
  QTextCharFormat textCharFormat = defaultTextCharFormat;
  while (!(offset < 0)) {
    int                    previousOffset = offset + escapeSequenceExpression.matchedLength();
    QStringList            capturedTexts  = escapeSequenceExpression.capturedTexts().back().split(';');
    QListIterator<QString> i(capturedTexts);
    while (i.hasNext()) {
      bool ok        = false;
      int  attribute = i.next().toInt(&ok);
      Q_ASSERT(ok);
      parseEscapeSequence(attribute, i, textCharFormat, defaultTextCharFormat);
    }
    offset = escapeSequenceExpression.indexIn(text, previousOffset);
    if (offset < 0) {
      cursor.insertText(text.mid(previousOffset), textCharFormat);
    } else {
      cursor.insertText(text.mid(previousOffset, offset - previousOffset), textCharFormat);
    }
  }
  cursor.setCharFormat(defaultTextCharFormat);
  cursor.endEditBlock();
  // cursor.movePosition(QTextCursor::Start);
  textEdit->setTextCursor(cursor);
}

CrashDialog::CrashDialog(QString text, QWidget *parent) : QDialog(parent), ui(new Ui::CrashDialog) {
  ui->setupUi(this);

  setTextTermFormatting(ui->textEdit, text);
}

CrashDialog::~CrashDialog() { delete ui; }
