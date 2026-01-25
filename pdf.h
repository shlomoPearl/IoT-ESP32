#pragma once
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <hpdf.h>
#include "get_time.h"

#define TITLE "EVENT REPORT"
#define T_FONT "Helvetica-Bold"
#define T_SIZE 24
#define SUB_TITLE "Generated on: "
#define ST_FONT "Helvetica"
#define ST_SIZE 18
#define B_FONT "Helvetica"
#define B_SIZE 12
#define TIME_LEN 128
#define BORDER 50
#define ST_SPACE 30
#define NL_SPACE 20

typedef struct PDF{
    HPDF_Doc doc;
    HPDF_Font font;
    HPDF_Page page;
    int y_pos;
    int width;
    int border;
} PDF;

void add_page(PDF*);
void set_font_and_size(PDF*, const char*, int);
void write_to_pdf(PDF*, const char*, int);
void save_pdf(PDF*, const char*);
void free_pdf(PDF*);
void init_pdf(PDF*);