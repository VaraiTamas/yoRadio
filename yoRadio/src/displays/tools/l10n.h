//Módositva! "multi_language"
#include "../../core/options.h"
#ifndef _display_l10n_h
#define _display_l10n_h
namespace LANG{
//==================================================
#if L10N_LANGUAGE == RU
#define L10N_PATH "../../../locale/displayL10n_ru.h"
#elif L10N_LANGUAGE == EN
#define L10N_PATH "../../../locale/displayL10n_en.h"
#elif L10N_LANGUAGE == HU
#define L10N_PATH "../../../locale/displayL10n_hu.h"
#elif L10N_LANGUAGE == PL
#define L10N_PATH "../../../locale/displayL10n_pl.h"
#elif L10N_LANGUAGE == NL
#define L10N_PATH "../../../locale/displayL10n_nl.h"
#elif L10N_LANGUAGE == GR
#define L10N_PATH "../../../locale/displayL10n_gr.h"
#elif L10N_LANGUAGE == SK
#define L10N_PATH "../../../locale/displayL10n_sk.h"
#elif L10N_LANGUAGE == UA
#define L10N_PATH "../../../locale/displayL10n_ua.h"
#elif L10N_LANGUAGE == DE
#define L10N_PATH "../../../locale/displayL10n_de.h"
#endif

#if __has_include("../../../locale/displayL10n_custom.h")
#include "../../../locale/displayL10n_custom.h"
#else
#include L10N_PATH
#endif
//==================================================
}
#endif
