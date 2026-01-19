#include "pdf.h"

HPDF_Page get_pdf_report(){
    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    if (!pdf) {
        printf("Error: Cannot create PDF\n");
        return 1;
    }
    
    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    
    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
    HPDF_Page_SetFontAndSize(page, font, 24);
    test_pdf(page, "HI THERE THIS IS a TEST PDF",0);
    font = HPDF_GetFont(pdf, "Helvetica", NULL);
    HPDF_Page_SetFontAndSize(page, font, 12);
    
}
void write_event(HPDF_Page page, const char* txt){

}
void save(HPDF_Doc pdf, const char* file_name){
    HPDF_SaveToFile(pdf, file_name);
}
void free_pdf(HPDF_Doc pdf){
    HPDF_Free(pdf);
}

