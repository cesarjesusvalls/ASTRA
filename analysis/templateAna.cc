#include "CMOSPixel.hh"
#include "pCTEvent.hh"
#include "pCTXML.hh"
#include <iostream>
#include <fstream>
#include <string>
#include "TApplication.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TVirtualFitter.h"
 #include "Math/Minimizer.h"
 #include "Math/Factory.h"
 #include "Math/Functor.h"
//
#include "TLegend.h"
#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGraphErrors.h"


int nPlanes=3;
int x[4] = {0,0,0,0};
int y[4] = {0,0,0,0};
unsigned short int nTracks = 0;
/*
Double_t *lineZ(Double_t z, Double_t *par)
{

Double_t point[3]= {par[0]*z+par[2],par[1]*z+par[3]};
return point;
}*/

double chi2(const double *par)
//void chi2(Double_t *x,Double_t *y,Double_t *z,Double_t *par)
{

Double_t z[4]= {0,5,10,15};
Double_t chi2=0;

for (int i=0; i<nPlanes;i++)
{
double XY[2]={par[2]*z[i]+par[0],par[3]*z[i]+par[1]};
//std::cout << "This is X and Y values at each point = " << x[i] << " " << y[i] << std::endl;
chi2 += pow((x[i]-XY[0]),2)+pow((y[i]-XY[1]),2);

}
return chi2;
}

std::vector<std::pair<Double_t,std::vector<std::pair<int,int>>>> SortTracksByChi2(
std::vector< CMOSPixel*> Det1, 
std::vector< CMOSPixel*> Det2, 
std::vector< CMOSPixel*> Det3, 
std::vector< CMOSPixel*> Det4)

{   
    const char * minName = "Minuit2";
    const char * algoName = "minuit";
    std::vector< CMOSPixel*>::iterator it1, it2, it3, it4;
    std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> chi2Points;

            std::vector<std::pair <int,int>> points;
            for (it1=Det1.begin(); it1!=Det1.end(); it1++)
            {
                for (it2=Det2.begin(); it2!=Det2.end(); it2++)
                {
                    for (it3=Det3.begin(); it3!=Det3.end(); it3++)
                    {
                        for (it4=Det4.begin(); it4!=Det4.end(); it4++)
                        {
                         
//                        
                            x[0]=(*it1)->GetX();
                            x[1]=(*it2)->GetX();
                            x[2]=(*it3)->GetX();
                            x[3]=(*it4)->GetX();

                            y[0]=(*it1)->GetY();
                            y[1]=(*it2)->GetY();
                            y[2]=(*it3)->GetY();
                            y[3]=(*it4)->GetY();

                           

                            std::cout<< "X = ["<<x[0]<<", "<<x[1]<<", "<<x[2]<<", "<<x[3]<<"]"<<std::endl;
                            //col={(*it1)->GetY(),(*it2)->GetY(),(*it3)->GetY(),(*it4)->GetY()};

                           // int z[4] = {0,5,15,20};
                            //Double_t zpoints[4]= {0,5,15,20};
                            //Double_t p=trackParam(row,col,z);
                            for (int i =0; i<nPlanes; i++) {
                                points.push_back(make_pair(x[i],y[i]));
                            }
                            
                             ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2");
                            // set tolerance , etc...
                                minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
                                minimum->SetMaxIterations(100000);  // for GSL
                                minimum->SetTolerance(0.01);
                                minimum->SetPrintLevel(1);                            //TVirtualFitter* minuit = TVirtualFitter::Fitter(0,nPlanes);

                                // create function wrapper for minimizer
                                 // a IMultiGenFunction type
                                  ROOT::Math::Functor f(&chi2,4);
                                 double step[4] = {0.01,0.01,0.01,0.01};
                                 
 


                                minimum->SetFunction(f);
 
                                // Set the free variables to be minimized !
                                minimum->SetVariable(0,"x0",x[0], step[0]);
                                minimum->SetVariable(1,"y0",y[0], step[1]);
                                minimum->SetVariable(2,"Vx",0, step[2]);
                                minimum->SetVariable(3,"Vy",0, step[3]);
                             
                                // do the minimization
                                minimum->Minimize();
                             
                                const double *xs = minimum->X();
                                
                                //std::cout << "Minimum: f(" << xs[0] << "," << xs[1] << xs[2] << "," << xs[3] <<  "): "
                                //          << minimum->MinValue()  << std::endl;
                                Double_t chi2 = minimum->MinValue();
                                // expected minimum is 0
                               /* if ( minimum->MinValue()  < 1.E-4  && f(xs) < 1.E-4)
                                   std::cout << "Minimizer " << minName << " - " << algoName
                                             << "   converged to the right minimum" << std::endl;
                                else {
                                   std::cout << "Minimizer " << minName << " - " << algoName
                                             << "   failed to converge !!!" << std::endl;
                                   Error("NumericalMinimization","fail to converge");
                                }
                                */

                            chi2Points.push_back(make_pair(chi2,points));
                            points.clear();
                            //delete func;

                        }
                    }
                }
            }
            std::sort(chi2Points.begin(), chi2Points.end()); 
            
            return chi2Points;

 }

std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> TrackSelector( std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> sortedTracks)
{
//std::cout<<"We got in! The number of tracks is " << nTracks <<std::endl;
std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>>::iterator it,it2; //This iterators run over tracks tracks
std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> outPut,chi2Points;
//std::vector<std::pair<int,int>>::iterator itPoint; //this iterator run over pairs (XY points) 
double bestChi2SUM=0;
double chi2 = 0;
unsigned int Try =0;
for(it=sortedTracks.begin(); it!=sortedTracks.end(); it++) //run over the full set of tracks
        {

   // std::cout<<"Chi2 of the first sorted track is " << (*it).first << " and the size of the input is " << sortedTracks.size() << std::endl;
/*
    for(int i = 0; i < int((*it).second.size()); i++)
                            {
                                 std::cout << (*it).second[i].first << ", " << (*it).second[i].second <<std::endl;
                            }   
*/
                        
                std::vector<std::pair<int,int>>::iterator itPoint; //this iterator run over pairs (XY points) 
                std::cout<<"This is the try number " << Try << std::endl;
                std::cout << "bestChi2 Sum = " << bestChi2SUM << " first chi2 is " << (*it).first << " and the cut is " << float(bestChi2SUM/(nTracks))<< std::endl;
                   if(bestChi2SUM != 0 and (*it).first >= float(bestChi2SUM/(nTracks))) {
                    std::cout<<"We get out here! (By chi2 Limit)"<<std::endl;
                    return outPut;}
                    

 
                std::vector<std::pair<int,int>> points;
                //std::vector<std::pair<int,int>>::iterator itBestPoints;
                for(it2=it; it2!=sortedTracks.end(); it2++) //run over the new set of tracks starting from a new point
                {
                    std::cout << " Entering loop size is: " << int(chi2Points.size()) << std::endl;
                    if (int(chi2Points.size())==nTracks) break;
                    if (int(chi2Points.size())==0)
                    {
                        chi2+=(*it2).first;
                        chi2Points.push_back(*it2);
                        std::cout << "Points added size is now: " << int(chi2Points.size()) << std::endl;
                        continue;
                      }
                     
                    bool addTrack =true; //this bool will only be true if any of the points pair fo the track is not
                    for (int i =0; i< int(chi2Points.size()); i++)
                        {
                        for(itPoint=(*it2).second.begin();itPoint!=(*it2).second.end();itPoint++)
                            {
                            for (int j = 0; j< int(chi2Points[i].second.size());j++)
                                 {
                                     int a =(*itPoint).first;
                                     int b =(*itPoint).second;
                                    auto c = chi2Points[i];

                                    int d = c.second[j].first;
                                    int e = c.second[j].second;
                                     
                                    if (a==d && b==e)
                                        { 
                                        std::cout << "Does this really happen? "<< a << " == " << d << " and " << b << " == " << e << std::endl;
                                         addTrack = false;
                                           break;
                                        }

                                    if (!addTrack) break;
                                }
                            if (!addTrack) break;
                            }
                       } 
                    if (addTrack)
                        {
                         chi2+=(*it2).first;
                         chi2Points.push_back(*it2);
                         std::cout << "Adding points to existing vector" << std::endl;
                            if (int(chi2Points.size())==nTracks)     break;
                          }
                }
                if ((bestChi2SUM == 0 || chi2 < bestChi2SUM) && int(chi2Points.size())==nTracks)
                {
                    bestChi2SUM = chi2;
                    outPut = chi2Points;
                }
                Try += 1;
        } 
    if (int(outPut.size())!=nTracks)
        {
        std::cout<< "Error 0, Not enough tracks were reconstructed!"<<std::endl;
        //std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>>  eOutPut = NULL;
        return outPut;
        // return eOutPut;
            }
return outPut;
}





int main(int argc,char** argv){
    
  TApplication* theApp=new TApplication("App",&argc,argv); 
    std::cout << "Starting Analysis Example..." << std::endl;
    
    TString inputFileName = "/Users/marc/pCT_Geant4/protonCT/output/outputFile.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);
   

    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");
    std::cout << config->GetSciDetNBars()  <<"-------------------------------\n";

    TH2F* h_trueEvsEDEP = new TH2F("h_trueEvsEDEP","",100,50,250,75,0,150);
    TH2F* h_trueEvsEDEP_max = new TH2F("h_trueEvsEDEP","",100,50,250,75,0,150);

    TH1F* trueE = new TH1F("trueE","",50,0,300);
    TH1F* EDEPCMOS = new TH1F("EDEPCMOS","", 250,0,500);

    // 2D angles
    TH1F* chi2Hist = new TH1F("chi2Hist","chi2Hist",100,0,100);
    TH1F* phiX = new TH1F("phiX","phiX",50,-0.05,0.05);

    TH1F* phiY = new TH1F("phiY","phiY",50,-0.05,0.05);
    TH2F* phi2D = new TH2F("phi2D", "phi2D",20,-0.02,0.02,20,-0.02,0.02);
    //3D Conical Variables Theta (angle against Z) Rho (radious) psi (rotation angle)
    TH1F* theta = new TH1F("theta", "theta",25,0.,0.05);
    TH1F* rho = new TH1F("rho", "rho",30,0,1);
    TH1F* psi = new TH1F("psi", "psi",34, -3.4, 3.4);
    TH2F* thetaVSrho = new TH2F("thetaVSrho","thetaVSrho",25,0.,0.025,30,0,1);
    TH2F* thetaVSpsi = new TH2F("thetaVSpsi","thetaVSpsi",25,0.,0.025,16, -3.2, 3.2);
    TH2F* psiVSrho = new TH2F("psiVSrho","psiVSrho",16, -3.2, 3.2,30,0,1.5);

    //Phantom Info
    TH1F* trackID = new TH1F("trackID","trackID",60,0,60);
    TH1F* posX = new TH1F("posX","posX",50, -50,50);
    TH1F* posY = new TH1F("posY","posY",50, -50,50);
    TH1F* posZ = new TH1F("posZ","posZ",50, -50,50);
    TH1F* eDep = new TH1F("eDep","eDep",30, 0,30);
    TH1F* totalEdep = new TH1F("totalEdep","totalEdep",20, 20,40);
    TH1F* totalR =new TH1F("totalR","totalR",30,0,15);
    TH2F* edepVsR = new TH2F("edepVsR","edepVsR",10,0,10,25,0,25);

    
    int x_;
    int y_;
    double pi = 3.141592;
    double tEdep;
    double totR;
    TVector3 prevPos;
    /*
    std::vector<int> vX;
    std::vector<int> vY;
    std::vector<int> vZ;
*/



    for(int ievt(0); ievt<data->GetEntries(); ievt++)
    {
        data->GetEntry(ievt);
         std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::cout << "-oOo--oOo--oOo--oOo--oOo--oOo-\n";
        std::cout << "Event: " << ievt << std::endl;
        std::cout << "-------------CMOS-------------" << std::endl;
        std::cout << "SciDet: " << ievt << std::endl;
        std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        std::cout << "Nhits:" << Counter.size() << std::endl;
        std::map<G4int, std::vector< CMOSPixel*> >::iterator it2;

        std::vector< CMOSPixel*>  det1;
        std::vector< CMOSPixel*>  det2;
        std::vector< CMOSPixel*>  det3;
        std::vector< CMOSPixel*>  det4;
    
        for(it2=Counter.begin(); it2!=Counter.end(); it2++)
        {
         unsigned short int Plane = (*it2).first;
         unsigned short int nHitsInPlane = (*it2).second.size();

                 std::cout << "Plane " << Plane << " has " << nHitsInPlane << " pixels above threshold" << std::endl;
        

        if (nHitsInPlane<nTracks || nTracks ==0) nTracks = nHitsInPlane;

        if (Plane==0)  det1 = (*it2).second;
        if (Plane==1)  det2 = (*it2).second;
        if (Plane==2)  det3 = (*it2).second;
        if (Plane==3)  det4 = (*it2).second;


         for(unsigned index(0); index<nHitsInPlane; index++)
            {
              int X = (*it2).second.at(index)->GetX();
              int Y = (*it2).second.at(index)->GetY();
              int trackID = (*it2).second.at(index)->GetTrackID();
              unsigned int e = (*it2).second.at(index)->GetElectronsLiberated();
              double eCMOS = e*0.0036;
              h_trueEvsEDEP->Fill((*trackIdToGunEnergy.begin()).second,eCMOS);
              trueE->Fill((*trackIdToGunEnergy.begin()).second);
              EDEPCMOS->Fill(eCMOS);

              std::cout << "Stats: "<<X << ", " << Y << ", "<< Plane <<", " << e <<", " << (*trackIdToGunEnergy.begin()).second << std::endl;
	      if (Plane == 0)
		{
       		 x_=X;
		 y_=Y;
		}
	      if (Plane == 1)
		{
		  std::cout << "dx= " << X << " - " << x_ << " dy = " << Y << " - " << y_ << std::endl;
		  double dx=(X-x_)*0.1;//*0.04;
		  double dy=(Y-y_)*0.1;//*0.036;
		  double rho_ = sqrt(pow(dx,2)+pow(dy,2));
		  std::cout << "dX and dY are: "<< dx << ", " << dy << std::endl;
		  double phiX_ = atan(dx/50);
		  double phiY_ = atan(dy/50);
		  double theta_ = atan(rho_/50);
		  double psi_;
		  if (dx >0 &&  dy>=0) {psi_ = atan(dy/dx);}
		  if (dx <0 &&  dy>=0) {psi_ = pi + atan(dy/dx);}
		  if (dx >0 &&  dy<0) {psi_ = atan(dy/dx);}
		  if (dx <0 &&  dy<0) {psi_ = atan(dy/dx)-pi;}
		  if (dx == 0 &&  dy>0) {psi_=pi/2;}
		  if (dx == 0 &&  dy<0) {psi_=-pi/2;}
		  if (dx == 0 && dy == 0) psi_=0;
		  
		  //2D angles
		  phiX->Fill(phiX_);
		  phiY->Fill(phiY_);
		  phi2D->Fill(phiX_,phiY_);
		  //3D angles
		  theta->Fill(theta_);
		  rho->Fill(rho_);
		  psi->Fill(psi_);

		  thetaVSrho->Fill(theta_,rho_);
		  thetaVSpsi->Fill(theta_,psi_);
		  psiVSrho->Fill(psi_,rho_);
		}
	    }
        }



        std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> chi2points = SortTracksByChi2(det1,det2,det3,det4);
        std::cout << "Size of chi2Points: "<< chi2points.size() << std::endl;

        std::cout << "Catastrofic test" << std::endl;
        for(int i = 0; i < int(chi2points.size()); i++)
                            {
                                 std::cout <<"chi2 = " <<chi2points[i].first <<std::endl;

                                 for(int j=0; j< int(chi2points[i].second.size());j++)
                                 {std::cout << chi2points[i].second[j].first << ", " << chi2points[i].second[j].second <<std::endl;}
                            }   
                std::cout << "Catastrofic test Done! There is nothing else in the input of TrackSelector" << std::endl;
        std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> finalTracks = TrackSelector(chi2points);


        std::cout << "Output test TrackSelector" << std::endl;
                for(int i = 0; i < int(finalTracks.size()); i++)
                            {
                                 std::cout <<"chi2 = " <<finalTracks[i].first <<std::endl;
                                 chi2Hist->Fill(finalTracks[i].first);
                                 for(int j=0; j< int(finalTracks[i].second.size());j++)
                                    {std::cout << finalTracks[i].second[j].first << ", " << finalTracks[i].second[j].second <<std::endl;}
                            }   
        
/*
        std::cout << "Size of chi2Points: "<< chi2points.size() << std::endl;
         for(unsigned index(0); index<chi2points.size(); index++)
         {
            std::cout << "Show the chi2 values " << chi2points.at(index).first << endl;
            for(unsigned idx(0); idx<chi2points.at(index).second.size(); idx++) std::cout << "Show the points =[" <<  chi2points.at(index).second.at(idx).first<< ", " << chi2points.at(index).second.at(idx).second << "]" << endl;
         }
*/

        std::cout << "------------SciDet------------" << std::endl;
        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::cout << "# SciHits: " << listOfSciHits.size() << std::endl;
        std::vector< SciDetHit* >::iterator sciHit;
//        for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++) std::cout << "Edep: " <<  (*sciHit)->GetEnergyDeposited() << endl;
    
	//        event->DrawSciDetHits(config);
	//        event->DrawCMOSHits(config);
/*
	std::cout << "----------------Phantom-------------" << std::endl;
	std::map <G4int,std::vector<std::pair<TVector3,double>>> phantomHitsMap = event->GetPhantomHits(); //int= TrackID--pair(Pos,eDep)
	std::map <G4int,std::vector<std::pair<TVector3,double>>>:: iterator iter;
	std::cout << "Phantom info " << phantomHitsMap.size() << std::endl;
	
    for(iter=phantomHitsMap.begin(); iter!=phantomHitsMap.end(); iter++)
	  {
         int particleID = (*iter).first;
         unsigned short int nHits = (*iter).second.size();
	 std::cout << "Phantom has " << nHits  << " hits" <<std::endl;
	 tEdep=0;
	 totR=0;
	 bool first=true;
	    for(unsigned index(0); index<nHits; index++)
            {
              int edep = (*iter).second.at(index).second;
	      TVector3 pos = (*iter).second.at(index).first;
	      if ( (*iter).first == 1)
		 {
		   tEdep=tEdep+edep;
		   posX->Fill(pos.X());
		   posY->Fill(pos.Y());
		   posZ->Fill(pos.Z());
	      
      		   eDep->Fill(edep);
		   if (first) {first=false;}
		   else {
		     //  eDep->Fill(edep);
		     double modulus = (pos-prevPos).Mag();
		     totR= totR+modulus;
		     edepVsR->Fill(modulus,edep);
		     
		   }
	       	      prevPos = pos;
	      }
	    }
	    trackID->Fill((*iter).first);
	    std::cout << "Track ID" << (*iter).first<< std::endl;
	    if ((*iter).first==1)
	      {
		totalEdep->Fill(tEdep);
		totalR->Fill(totR);
	      }
	  }
*/


}//end of event's for
    //_____________________


/*
TCanvas* c1 = new TCanvas("c1","c1",1400,1000);
    gStyle->SetOptStat(0);
    //   gStyle->SetPadRightMargin(1.5);
    //gStyle->SetPadLeftMargin(0.01);
     c1->cd(1);
     chi2Hist->SetTitle("#chi^{2} of the reconstructed tracks");
     chi2Hist->GetXaxis()->SetTitle("#chi^{2} [arb. units]");
     chi2Hist->GetXaxis()->SetTickLength(0.);
     chi2Hist->GetYaxis()->SetTickLength(0.);
     chi2Hist->Draw();
    c1->Update();

    

    TCanvas* c1 = new TCanvas("c1","c1",1400,1000);
    gStyle->SetOptStat(0);
    //   gStyle->SetPadRightMargin(1.5);
    //gStyle->SetPadLeftMargin(0.01);
    c1->Divide(2,1);
       
     c1->cd(1);
     phiX->SetTitle("Outgoing angles #phi");
     phiX->Fit("gaus");
     phiX->GetXaxis()->SetTitle("#phi [rad]");
     phiX->GetXaxis()->SetTickLength(0.);
     phiX->GetYaxis()->SetTickLength(0.);
     
     phiX->SetLineColor(kGreen);
     phiY->Fit("gaus");
     phiX->Draw();
     phiY->Draw("SAME");
     auto legend = new TLegend(0.1,0.7,0.48,0.9);
     legend->SetHeader("#phi in X and Y axis","C"); // option "C" allows to center the header
     legend->AddEntry(phiX,"X Axis","f");
     legend->AddEntry(phiY,"Y Axis","f");
     legend->Draw();
     c1->cd(2);

     
     phi2D->SetTitle("");
     phi2D->GetXaxis()->SetTitle("#phi_x [rad]");
     phi2D->GetYaxis()->SetTitle("#phi_y [rad]");
     phi2D->Draw("COLZ");
     c1->Update();
     
     TCanvas* c2 = new TCanvas("c2","c2",1500,500);
     c2->Divide(3,1);
     c2->cd(1);
     theta->GetXaxis()->SetTitle("#theta [rad]");
     theta->SetTitle("Outgoing 3D angle #theta");
     theta->Draw();
     c2->cd(2);
     rho->GetXaxis()->SetTitle("#rho [mm]");
     rho->SetTitle("Displacement projection");
     rho->Draw();
     c2->cd(3);
     psi->GetXaxis()->SetTitle("#varphi [rad]");
     psi->SetTitle("Outgoing angle #varphi");
     psi->Draw();
     c2->Update();

     TCanvas* c3 = new TCanvas("c3","c3",1500,500);
     c3->Divide(3,1);
     c3->cd(1);
     
     thetaVSpsi->GetXaxis()->SetTitle("#theta [rad]");
     thetaVSpsi->GetYaxis()->SetTitle("#varphi [rad]");
     thetaVSpsi->Draw("COLZ");
     thetaVSpsi->SetTitle("");

     c3->cd(2);
     
     thetaVSrho->SetTitle("");
     thetaVSrho->GetXaxis()->SetTitle("#theta [rad]");
     thetaVSrho->GetYaxis()->SetTitle("#rho [mm]");
     thetaVSrho->Draw("COLZ");
     c3->cd(3);
     psiVSrho->SetTitle("");
     psiVSrho->GetYaxis()->SetTitle("#rho [mm]");
     psiVSrho->GetXaxis()->SetTitle("#varphi [rad]");
     psiVSrho->Draw("COLZ");
     c3->Update();

     TCanvas* c4 = new TCanvas("c4","c4",1000,400);
     c4->Divide(2,1);
     c4->cd(1);
     posZ->GetXaxis()->SetTitle("Position inside the phantom [mm]");
     posZ->Draw();
     posY->SetLineColor(kRed);
     posY->Draw("same");
     posZ->SetLineColor(kGreen);
     posX->Draw("same");
     auto legend2 = new TLegend(0.1,0.7,0.48,0.9);
     legend2->SetHeader("Proton positions inside the phantom","C"); // option "C" allows to center the header
     legend2->AddEntry(posX,"X Axis","f");
     legend2->AddEntry(posY,"Y Axis","f");
     legend2->AddEntry(posZ,"Z Axis","f");
     legend2->Draw();
     
     c4->cd(2);
     eDep->Draw();
     c4->Update();
*/



       for(int i(0); i<100; i++){
            int maxBin      = -1;
            int maxBinCnt   = -1;
            for(int j(0); j<75; j++)
                if(maxBinCnt < h_trueEvsEDEP->GetBinContent(i+1,j+1))
                    {maxBin = j; maxBinCnt = h_trueEvsEDEP->GetBinContent(i+1,j+1);}
            for(int j(0); j<75; j++)
                if(h_trueEvsEDEP->GetBinContent(i+1,j+1) < maxBinCnt) h_trueEvsEDEP->SetBinContent(i+1,j+1,0);
        }
       // h_trueEvsEDEP->Fit(fitval,"RQ");



    TFile* outFile = new TFile("energyHistograms.root","RECREATE");
    TF1* fitval = new TF1("fitval","pol4",50,250);
    fitval->SetParameters(0,0,0,0);
    h_trueEvsEDEP->Fit(fitval,"RQ");

     TCanvas* c5 = new TCanvas("c5", "c5", 1600,1500);
     //c5->Divide(2,1);
     //c5->cd(1);
     h_trueEvsEDEP->SetTitle("Edep in CMOS vs True Energy");
     h_trueEvsEDEP->GetXaxis()->SetTitle("True Energy [MeV]");
     h_trueEvsEDEP->GetYaxis()->SetTitle("EDep in CMOS [KeV]");
     h_trueEvsEDEP->Draw("COLZ");
     c5->Update();
     
     c5->cd(2);
     h_trueEvsEDEP_max->SetTitle("Edep in CMOS vs True Energy max bin");
     h_trueEvsEDEP_max->GetXaxis()->SetTitle("True Energy [MeV]");
     h_trueEvsEDEP_max->GetYaxis()->SetTitle("EDep in CMOS [KeV]");
     h_trueEvsEDEP_max->Draw("COLZ");
     c5->Update();
     
     
     TCanvas* c6 = new TCanvas("c6", "c6", 1600,1500);
     c6->Divide(2,1);
     c6->cd(1);
     EDEPCMOS->Draw();
     EDEPCMOS->GetXaxis()->SetTitle("Energy Deposited in CMOS [KeV]");

     c6->cd(2);
     trueE->Draw();
     trueE->GetXaxis()->SetTitle("True Energy [MeV]");
     c6->Update();











outFile->cd();
fitval->Write();
//h_trueEvsEDEP_max->Write();
h_trueEvsEDEP->Write();
outFile->Close();




    inputFile->Close();
    theApp->Run();
    return 0;
}



