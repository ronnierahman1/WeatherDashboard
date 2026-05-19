#pragma once
#include "globals.h"

// Vector font sizes via setTextSize() — same technique as Salah timetable

inline int glyphWidth(int textSize)  { return 6 * textSize; }   // 6x8 classic
inline int glyphHeight(int textSize) { return 8 * textSize; }

inline int textWidth(const String& s, int textSize) {
  return (int)s.length() * glyphWidth(textSize);
}
