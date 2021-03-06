###############################################################################
# This file is part of the TouchGFX 4.9.3 distribution.
# Copyright (C) 2017 Draupner Graphics A/S <http://www.touchgfx.com>.
###############################################################################
# This is licensed software. Any use hereof is restricted by and subject to 
# the applicable license terms. For further information see "About/Legal
# Notice" in TouchGFX Designer or in your TouchGFX installation directory.
###############################################################################
class ApplicationFontProviderHpp < Template
  def input_path
    File.join(root_dir,'Templates','ApplicationFontProvider.hpp.temp')
  end
  def output_path
    '/include/fonts/ApplicationFontProvider.hpp'
  end
end
