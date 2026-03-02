#ifndef _TFTINVERTTITLE_H_
#define _TFTINVERTTITLE_H_

  #if !DSP_INVERT_TITLE
    uint16_t newbg = config.theme.meta;
    config.theme.meta       = config.theme.metabg;
    config.theme.metabg     = newbg;
    config.theme.metafill   = config.theme.div;
  #endif 

#endif
