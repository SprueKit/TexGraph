#pragma once

/*
Defines for consistent stylistic use,
presently just for eliminating redundancy in property editors.

TODO: actually use these
*/

#define STYLING_DEFAULT_FONT QFont("Arial", 12)

#define STYLING_SET_SPINNER_RANGE(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX); SPINNER->setMaximumWidth(80);

#define STYLING_SET_SPINNER_MAX_RANGE(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX)

#define STYLING_SET_SPINNER(SPINNER, MINVAL, MAXVAL) SPINNER->setMinimum(MINVAL); SPINNER->setMaximum(MAXVAL)

#define STYLING_XStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #DD0000; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define STYLING_YStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #00AA00; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define STYLING_ZStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #3F3FDD; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define STYLING_AStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #AA007F; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"

#define STYLING_CREATE_LBL(VARNAME, TEXT, STYLE, TIP) VARNAME = new QLabel(TEXT); \
VARNAME->setMaximumWidth(18); \
VARNAME->setMinimumWidth(18); \
VARNAME->setMaximumHeight(18); \
VARNAME->setStyleSheet(STYLE); \
VARNAME->setAlignment(Qt::AlignCenter); \
VARNAME->setToolTip(TIP);

#define STYLING_CREATE_LBL_SIZED(VARNAME, TEXT, STYLE, SIZE, TIP) VARNAME = new QLabel(TEXT); \
VARNAME->setMaximumWidth(SIZE); \
VARNAME->setMinimumWidth(SIZE); \
VARNAME->setMaximumHeight(18); \
VARNAME->setStyleSheet(STYLE); \
VARNAME->setAlignment(Qt::AlignCenter); \
VARNAME->setToolTip(TIP);

#define STYLE_WIDGET_BG QColor("#3d3d3d")
// When using QDarkStyle this should be #31363b