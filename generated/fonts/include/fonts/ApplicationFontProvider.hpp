/* DO NOT EDIT THIS FILE */
/* This file is autogenerated by the text-database code generator */

#ifndef APPLICATIONFONTPROVIDER_HPP
#define APPLICATIONFONTPROVIDER_HPP

#include <touchgfx/FontManager.hpp>

struct Typography
{
  static const touchgfx::FontId DIGITALCLOCK = 0;
};

class ApplicationFontProvider : public touchgfx::FontProvider
{
public:
  virtual touchgfx::Font* getFont(touchgfx::FontId fontId);
};

#endif /* APPLICATIONFONTPROVIDER_HPP */
