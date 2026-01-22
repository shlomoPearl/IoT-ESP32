#include "pdf.h"

void add_page(PDF* pdf){
    pdf->page = HPDF_AddPage(pdf->doc);
    HPDF_Page_SetSize(pdf->page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    pdf->y_pos = HPDF_Page_GetHeight(pdf->page);
    pdf->width = HPDF_Page_GetWidth(pdf->page);
}   

void set_font_and_size(PDF* pdf, const char* font_name, int font_size){
    pdf->font = HPDF_GetFont(pdf->doc, font_name, NULL);
    HPDF_Page_SetFontAndSize(pdf->page, pdf->font, font_size);
}

void write_to_pdf(PDF* pdf, const char* txt, int x){
    if (pdf->y_pos < 50){
        const char* font_name = HPDF_Font_GetFontName(pdf->font);
        int font_size = HPDF_Page_GetCurrentFontSize(pdf->page);
        add_page(pdf);
        set_font_and_size(pdf, font_name, font_size);
        pdf->y_pos-= pdf->border;
    }
    HPDF_Page_BeginText(pdf->page);
    HPDF_Page_TextOut(pdf->page, x , pdf->y_pos, txt);
    HPDF_Page_EndText(pdf->page);
}

void save_pdf(PDF* pdf, const char* file_name){
    HPDF_SaveToFile(pdf->doc, file_name);
}

void free_pdf(PDF* pdf){
    HPDF_Free(pdf->doc);
}