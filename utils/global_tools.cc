//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include <TStyle.h>
#include <TColor.h>
#include <TH1.h>

TStyle* SetpCTStyle(Int_t WhichStyle, TString styleName = "pCT") {
    TStyle *pCTstyle= new TStyle(styleName, "pCT approved plots style");

    // -- WhichStyle --
    // 1 = presentation large fonts
    // 2 = presentation small fonts
    // 3 = publication/paper

    Int_t FontStyle;// = 22;
    Float_t FontSizeLabel;// = 0.035;
    Float_t FontSizeTitle;// = 0.05;
    Float_t YOffsetTitle;// = 1.3;

    switch(WhichStyle) {
    case 1:
        //std::cout << "Setting Presentation Style 1." << std::endl;
        FontStyle = 42;
        FontSizeLabel = 0.055;
        FontSizeTitle = 0.055;
        YOffsetTitle = 1.19;
        break;
    case 2:
        //std::cout << "Setting Presentation Style 2." << std::endl;
        FontStyle = 42;
        FontSizeLabel = 0.045;
        FontSizeTitle = 0.045;
        YOffsetTitle = 0.9;
        break;
    case 3:
        //std::cout << "Setting Publication Style." << std::endl;
        FontStyle = 132;
        FontSizeLabel = 0.055;
        FontSizeTitle = 0.055;
        YOffsetTitle = 1.6;
        break;
    }

    // use plain black on white colors
    pCTstyle->SetFrameBorderMode(0);
    pCTstyle->SetCanvasBorderMode(0);
    pCTstyle->SetPadBorderMode(0);
    pCTstyle->SetCanvasBorderSize(0);
    pCTstyle->SetFrameBorderSize(0);
    pCTstyle->SetDrawBorder(0);
    pCTstyle->SetTitleBorderSize(0);

    pCTstyle->SetPadColor(0);
    pCTstyle->SetCanvasColor(0);
    pCTstyle->SetStatColor(0);
    pCTstyle->SetFillColor(0);

    pCTstyle->SetEndErrorSize(4);
    pCTstyle->SetStripDecimals(kFALSE);

    pCTstyle->SetLegendBorderSize(0);
    pCTstyle->SetLegendFont(FontStyle);

    // // set the paper & margin sizes
    // pCTstyle->SetPaperSize(20, 26);
    // pCTstyle->SetPadTopMargin(0.1);
    // pCTstyle->SetPadBottomMargin(0.15);
    // pCTstyle->SetPadRightMargin(0.13); // 0.075 -> 0.13 for colz option
    // pCTstyle->SetPadLeftMargin(0.16);//to include both large/small font options

    // // Fonts, sizes, offsets
    pCTstyle->SetTextFont(FontStyle);
    pCTstyle->SetTextSize(0.08);

    pCTstyle->SetLabelFont(FontStyle, "x");
    pCTstyle->SetLabelFont(FontStyle, "y");
    pCTstyle->SetLabelFont(FontStyle, "z");
    pCTstyle->SetLabelFont(FontStyle, "t");
    pCTstyle->SetLabelSize(FontSizeLabel, "x");
    pCTstyle->SetLabelSize(FontSizeLabel, "y");
    pCTstyle->SetLabelSize(FontSizeLabel, "z");
    pCTstyle->SetLabelOffset(0.015, "x");
    pCTstyle->SetLabelOffset(0.015, "y");
    pCTstyle->SetLabelOffset(0.015, "z");

    pCTstyle->SetLabelSize(FontSizeLabel);

    pCTstyle->SetTitleFont(FontStyle, "x");
    pCTstyle->SetTitleFont(FontStyle, "y");
    pCTstyle->SetTitleFont(FontStyle, "z");
    pCTstyle->SetTitleFont(FontStyle, "t");
    pCTstyle->SetTitleSize(FontSizeTitle, "y");
    pCTstyle->SetTitleSize(FontSizeTitle, "x");
    pCTstyle->SetTitleSize(FontSizeTitle, "z");
    pCTstyle->SetTitleOffset(1.14, "x");
    pCTstyle->SetTitleOffset(YOffsetTitle, "y");
    pCTstyle->SetTitleOffset(1.2, "z");

    pCTstyle->SetTitleStyle(0);
    pCTstyle->SetTitleFontSize(0.06);//0.08
    pCTstyle->SetTitleFont(FontStyle, "pad");
    pCTstyle->SetTitleBorderSize(0);
    pCTstyle->SetTitleX(0.1f);
    pCTstyle->SetTitleW(0.8f);

    // use bold lines and markers
    pCTstyle->SetMarkerStyle(20);
    pCTstyle->SetHistLineWidth( Width_t(2.5) );
    pCTstyle->SetLineStyleString(2, "[12 12]"); // postscript dashes

    // get rid of X error bars and y error bar caps
    pCTstyle->SetErrorX(0.001);

    // do not display any of the standard histogram decorations
    // pCTstyle->SetOptTitle(0);
    // pCTstyle->SetOptStat(0);
    // pCTstyle->SetOptFit(0);

    // put tick marks on top and RHS of plots
    pCTstyle->SetPadTickX(1);
    pCTstyle->SetPadTickY(1);

    // -- color --
    // functions blue
    //pCTstyle->SetFuncColor(600-4);
    pCTstyle->SetFuncColor(2);
    pCTstyle->SetFuncWidth(2);

    // pCTstyle->SetFillColor(1); // make color fillings (not white)
    // // - color setup for 2D -
    // // - "cold"/ blue-ish -
    // Double_t red[]   = { 0.00, 0.00, 0.00 };
    // Double_t green[] = { 1.00, 0.00, 0.00 };
    // Double_t blue[]  = { 1.00, 1.00, 0.25 };
    // // - "warm" red-ish colors -
    // //  Double_t red[]   = {1.00, 1.00, 0.25 };
    // //  Double_t green[] = {1.00, 0.00, 0.00 };
    // //  Double_t blue[]  = {0.00, 0.00, 0.00 };

    // Double_t stops[] = { 0.25, 0.75, 1.00 };
    // const Int_t NRGBs = 3;
    // const Int_t NCont = 500;

    // TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    // pCTstyle->SetNumberContours(NCont);

    // - Rainbow -
    pCTstyle->SetPalette(kMint);  // use the rainbow color set

    // -- axis --
    pCTstyle->SetStripDecimals(kFALSE); // don't do 1.0 -> 1
    //  TGaxis::SetMaxDigits(3); // doesn't have an effect
    // no supressed zeroes!
    pCTstyle->SetHistMinimumZero(kTRUE);


   return(pCTstyle);
  }
