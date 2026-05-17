#ifndef THEME_H
#define THEME_H

#include "raylib.h"

/* Palette — Guardiões do Mangue.
 * Hex codes kept in comments so the source values are easy to verify. */

/* #0e3a3a — deep mangrove water (background) */
#define COR_FUNDO     ((Color){ 14,  58,  58, 255 })

/* #d97706 — caranguejo orange (primary action) */
#define COR_PRIMARIA  ((Color){217, 119,   6, 255 })

/* Hover tint: primary lightened ~15% for button hover feedback */
#define COR_PRIMARIA_HOVER ((Color){242, 153,  46, 255 })

/* #84cc16 — mangrove green (accent / secondary action) */
#define COR_ACENTO    ((Color){132, 204,  22, 255 })
#define COR_ACENTO_HOVER ((Color){164, 222,  74, 255 })

/* Text on dark background */
#define COR_TEXTO     ((Color){255, 255, 255, 255 })

/* Subtle surface for panels / text-field backgrounds */
#define COR_PAINEL    ((Color){ 24,  78,  78, 255 })
#define COR_PAINEL_HOVER ((Color){ 36, 100, 100, 255 })

/* Danger color for destructive actions (reset ranking confirm) */
#define COR_PERIGO    ((Color){220,  60,  60, 255 })
#define COR_PERIGO_HOVER ((Color){240,  92,  92, 255 })

#endif /* THEME_H */
