#include "pdf.h"

HPDF_Page add_page(HPDF_Doc* pdf){
    HPDF_Page page = HPDF_AddPage(*pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    return page;
}   

void set_font_and_size(HPDF_Doc* pdf, HPDF_Font* font, HPDF_Page* page, const char* font_name, int font_size){
    *font = HPDF_GetFont(*pdf, font_name, NULL);
    HPDF_Page_SetFontAndSize(*page, *font, font_size);
}

void write_to_pdf(HPDF_Doc* pdf,HPDF_Page* page, const char* txt, int w, int h){
    if (HPDF_Page_GetHeight(*page) - h < 50){
        *page = add_page(pdf);
    }
    HPDF_Page_BeginText(*page);
    HPDF_Page_TextOut(*page, w , h, txt);
    HPDF_Page_EndText(*page);
}

void save_pdf(HPDF_Doc* pdf, const char* file_name){
    HPDF_SaveToFile(*pdf, file_name);
}

void free_pdf(HPDF_Doc* pdf){
    HPDF_Free(*pdf);
}