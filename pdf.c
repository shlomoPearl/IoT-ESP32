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

void init_pdf(PDF* pdf){
    pdf->doc = HPDF_New(NULL, NULL);
    if (!pdf->doc) {
        fprintf(stderr, "Error: cannot create PDF object\n");
        return;
    }
    add_page(pdf);
    pdf->border = BORDER;
    set_font_and_size(pdf, T_FONT, T_SIZE);
    float tw = HPDF_Page_TextWidth (pdf->page, TITLE);
    pdf->y_pos -= pdf->border;
    write_to_pdf(pdf, TITLE, (pdf->width - tw) / 2);
    
    char sub_titel_buffer[TIME_LEN];
    strncpy(sub_titel_buffer, SUB_TITLE, strlen(SUB_TITLE) + 1);
    get_current_time_str(sub_titel_buffer + strlen(SUB_TITLE), TIME_LEN - strlen(SUB_TITLE));
    set_font_and_size(pdf, ST_FONT, ST_SIZE);
    tw = HPDF_Page_TextWidth (pdf->page, sub_titel_buffer);
    pdf->y_pos -= ST_SPACE;
    write_to_pdf(pdf, sub_titel_buffer, (pdf->width - tw) / 2);
    set_font_and_size(pdf, B_FONT, B_SIZE);
    pdf->y_pos -= NL_SPACE;
}

void save_pdf(PDF* pdf, const char* file_name){
    HPDF_SaveToFile(pdf->doc, file_name);
}

void free_pdf(PDF* pdf){
    HPDF_Free(pdf->doc);
}