
#include <TVector3.h>

#include <TMath.h>
#include <TNtuple.h>
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH3.h>
#include <TF2.h>
#include <TH1.h>
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>
#include <cassert>
using namespace ROOT::Math;

void line(double t, const double *p, double &x, double &y, double &z, double x0=0, double y0=0, double z0=0) {
    // p[0],p[1],p[2], p[3],p[4],p[5] are free parameters
    if (x0 && y0 && z0){
        x = x0 + p[3]*t;
        y = y0 + p[4]*t;
        z = z0 + p[5]*t;
    }
    else{
        x = p[0] + p[3]*t;
        y = p[1] + p[4]*t;
        z = p[2] + p[5]*t;
    }
}

//***********************************************************************************************
std::vector<double> TrackFitter(std::vector <TVector3> inputVoxels, bool DEBUG){
//***********************************************************************************************
    
    std::vector<double> fitParameters;

    // function Object to be minimized
    struct SumDistance2 {
      const std::vector <TVector3> fVoxels;

      SumDistance2(const std::vector <TVector3> g) : fVoxels(g) {}

      // calculate distance line-point
      double distance2(double x,double y,double z, const double *p) {
         // distance line point is D= | (xp-x0) cross  ux |
         // where ux is direction of line and x0 is a point in the line (like t = 0)
         ROOT::Math::XYZVector xp(x,y,z);
         ROOT::Math::XYZVector x0(p[0], p[1], p[2]);
         ROOT::Math::XYZVector x1(p[0] + p[3], p[1] + p[4], p[2]+ p[5]);
         ROOT::Math::XYZVector u = (x1-x0).Unit();
         double d2 = ((xp-x0).Cross(u)).Mag2();
         return d2;
      }

      // implementation of the function to be minimized
      double operator() (const double *par) {
         assert(fVoxels.size() != 0);
         double sum = 0;
         for (auto v:fVoxels) {
            double d = distance2(v.X(),v.Y(),v.Z(),par);
            // possible improvement, weight by RecoQ: d*=v->GetRecoQ() ---> maybe?
            sum += d;
         }
         return sum;
      }

    };

    gStyle->SetOptStat(0);
    gStyle->SetOptFit();

    gStyle->SetCanvasColor(0);
    gStyle->SetMarkerStyle(21);
    gStyle->SetMarkerSize(1.05);

    ROOT::Fit::Fitter  fitter;

    // make the functor objet
    SumDistance2 sdist(inputVoxels);
    ROOT::Math::Functor fcn(sdist,6);
    // set the function and the initial parameter values
    double pStart[6] = {1,1,1,1,1,1};
    fitter.SetFCN(fcn,pStart);
    // set step sizes different than default ones (0.3 times parameter values)
    for (int i = 0; i < 6; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);

    std::vector <double> p_result;

    bool ok = fitter.FitFCN();
    if (!ok) {
        //Error("line3Dfit","Line3D Fit failed");
        return fitParameters;
    }

    const ROOT::Fit::FitResult & result = fitter.Result();

    if(DEBUG){
        std::cout << "Total final distance square: " << result.MinFcnValue() << std::endl;
        std::cout << "Ave Dist: " << result.MinFcnValue() / (inputVoxels.size()) << std::endl;
        result.Print(std::cout);
    }

    // get fit parameters
    const double * parFit = result.GetParams();
    const double * parFitErr = result.GetErrors();

    double p0 = parFit[0];
    double p1 = parFit[1];
    double p2 = parFit[2];
    double p3 = parFit[3];
    double p4 = parFit[4];
    double p5 = parFit[5];


    fitParameters.push_back(result.MinFcnValue() / (inputVoxels.size()));  // ave dist
    fitParameters.push_back(p0); // point x
    fitParameters.push_back(p1); // point y
    fitParameters.push_back(p2); // point z
    fitParameters.push_back(p3); // vector x
    fitParameters.push_back(p4); // vector y
    fitParameters.push_back(p5); // vector z
    fitParameters.push_back(parFitErr[0]);
    fitParameters.push_back(parFitErr[1]);
    fitParameters.push_back(parFitErr[2]);
    fitParameters.push_back(parFitErr[3]);
    fitParameters.push_back(parFitErr[4]);
    fitParameters.push_back(parFitErr[5]);

    // draw the fitted line
    int n = 1000;
    double t0 = 0;
    double unitVecNorm = pow(pow(p3,2)+pow(p4,2)+pow(p5,2),0.5);
    double dt = 50/unitVecNorm;
    TPolyLine3D *l = new TPolyLine3D(n);

    int x0 = 0;
    int y0 = 0;
    int z0 = 0;

    for(auto v:inputVoxels){
        x0+=v.X();
        y0+=v.Y();
        z0+=v.Z();
    }

    x0/=inputVoxels.size();
    y0/=inputVoxels.size();
    z0/=inputVoxels.size();

    // fitParameters.push_back(x0);
    // fitParameters.push_back(y0);
    // fitParameters.push_back(z0);
    // fitParameters.push_back(unitVecNorm);

    if(DEBUG){
        for (int i = 0; i <n;++i) {
            double t = t0+ dt*i/n-dt/(2);
            double x,y,z;
            line(t,parFit,x,y,z,x0,y0,z0);
            l->SetPoint(i,x,y,z);
        }
        l->SetLineColor(kRed);
        std::cout << "drawing " << inputVoxels.size() << "voxels." << std::endl;
        TNtuple *event3D = new TNtuple("event3D", "event3D", "x:y:z:c");
        for (auto v:inputVoxels){
          event3D->Fill(v.Z(),v.Y(),v.X(),1);
        }
        TCanvas *canv = new TCanvas("canv", "canv", 800, 600, 800, 600);
        canv->cd(1);
        event3D->Draw("x:y:z:c","","box2");
        TH3F *htemp = (TH3F*)gPad->GetPrimitive("htemp");
        htemp->GetXaxis()->SetLimits(-100,2300);
        htemp->GetYaxis()->SetLimits(-100,2300);
        htemp->GetZaxis()->SetLimits(-5,20);
        htemp->SetTitle("");
        l->Draw("same");
        canv->Update();
        canv->WaitPrimitive();
        delete htemp;
        delete canv;
        delete event3D;
        std::cout << "Fitt completed: solution is: " << p0 << "," << p1 << "," << p2 << "," << p3 << "," << p4 << "," << p5 << std::endl;
    }

    return fitParameters;

}
