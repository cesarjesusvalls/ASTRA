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


const int nPlanes= 3;
int x[nPlanes] = {0};
int y[nPlanes] = {0};
int trackID[nPlanes] = {0};
int goodTracks = 0;
unsigned short int nTracks = 0;

double chi2(const double *par)
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

std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>>SortTracksByChi2(
//std::vector<std::pair<Double_t,std::vector<std::pair<int,int>>>> SortTracksByChi2(
std::vector< CMOSPixel*> Det1, 
std::vector< CMOSPixel*> Det2, 
std::vector< CMOSPixel*> Det3, 
std::vector< CMOSPixel*> Det4)


{   
    std::vector< CMOSPixel*>::iterator it1, it2, it3, it4;
    //std::vector<std::vector< CMOSPixel*>::iterator> its[4] = {it1,it2,it3,it4};

    //std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> chi2Points;
    std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> chi2Points;

            std::vector<std::pair <int,int>> points;
            std::vector<std::pair <int,std::pair <int,int>>> pointsTrackID;

            for (it1=Det1.begin(); it1!=Det1.end(); it1++)
            { 
                for (it2=Det2.begin(); it2!=Det2.end(); it2++)
                {	
                    for (it3=Det3.begin(); it3!=Det3.end(); it3++)
                    {	
                     for (it4=Det4.begin(); it4!=Det4.end(); it4++)
                        { std::cout<< "Entering detector 4 loop!" << std::endl;

                     		/*
								for( uint i(0); i<4; i++)
								if ((*its[i])->GetTrackID() != -999)
								{
									x[i]=(*its[i])->GetX();
                            		y[i]=(*its[i])->GetY();
                            		trackID[i]=(*its[i])->GetTrackID();
								}

								*/

                            if ((*it1)->GetTrackID()!=-999)
                            	{
                            		x[0]=(*it1)->GetX();
                            		y[0]=(*it1)->GetY();
                            		trackID[0]=(*it1)->GetTrackID();
                            	}

                            if ((*it2)->GetTrackID()!=-999)
                            	{
                            		x[1]=(*it2)->GetX();
                            		y[1]=(*it2)->GetY();
                            		trackID[1]=(*it2)->GetTrackID();
                            	}

                            if ((*it3)->GetTrackID()!=-999)
                            	{
                            		x[2]=(*it3)->GetX();
                            		y[2]=(*it3)->GetY();
                            		trackID[2]=(*it3)->GetTrackID();
                            	}

                            if ((*it4)->GetTrackID()!=-999)
                            	{
                            		x[3]=(*it4)->GetX();
                            		y[3]=(*it4)->GetY();
                            		trackID[3]=(*it4)->GetTrackID();
                            	}
                           
                            
                            for (int i =0; i<nPlanes; i++) {



                            	std::cout<< "XYT= ["<<x[i]<<", "<<y[i]<<", "<<trackID[i]<<"]"<<std::endl;
                                points.push_back(make_pair(x[i],y[i]));
                                
                                std::pair<int,int> XY = make_pair(x[i],y[i]);
                                pointsTrackID.push_back(make_pair(trackID[i],XY));
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
                                Double_t chi2 = minimum->MinValue();
                      
                            chi2Points.push_back(make_pair(chi2,pointsTrackID));
                            points.clear();
                            pointsTrackID.clear();
                           
                        }
                    }
                }
            }
            std::sort(chi2Points.begin(), chi2Points.end()); 
            
            return chi2Points;

 }



//std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> TrackSelector( std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> sortedTracks)
std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> TrackSelector(std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> sortedTracks)

{

//std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>>::iterator it,it2; //This iterators run over tracks tracks
//std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> outPut,chi2Points;

std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>>::iterator it,it2;
std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> outPut,chi2Points;

double bestChi2SUM=0;
double chi2 = 0;
unsigned int Try =0;
for(it=sortedTracks.begin(); it!=sortedTracks.end(); it++) //run over the full set of tracks
        {
                        
                //std::vector<std::pair<int,int>>::iterator itPoint; //this iterator run over pairs (XY points) 
                std::vector<std::pair <int ,std::pair <int,int>>>::iterator itPoint; //this iterator run over pairs (XY points) 
                std::cout<<"This is the try number " << Try << std::endl;
                std::cout << "bestChi2 Sum = " << bestChi2SUM << " first chi2 is " << (*it).first << " and the cut is " << float(bestChi2SUM/(nTracks))<< std::endl;
                   if(bestChi2SUM != 0 and (*it).first >= float(bestChi2SUM/(nTracks))) {
                    std::cout<<"We get out here! (By chi2 Limit)"<<std::endl;
                    return outPut;}
                    

 
                std::vector<std::pair<int,int>> points;
                std::vector<std::pair <int ,std::pair <int,int>>> pointsTrackID;

    
                for(it2=it; it2!=sortedTracks.end(); it2++) //run over the new set of tracks starting from a new point
                {
                    if (int(chi2Points.size())==nTracks) break;
                    if (int(chi2Points.size())==0)
                    {
                        chi2+=(*it2).first;
                        chi2Points.push_back(*it2);
                        continue;
                      }
                     
                    bool addTrack =true; //this bool will only be true if any of the points pair fo the track is not
                    for (int i =0; i< int(chi2Points.size()); i++)
                        {
                        for(itPoint=(*it2).second.begin();itPoint!=(*it2).second.end();itPoint++)
                            {
                            	
                            for (int j = 0; j< int(chi2Points[i].second.size());j++)
                                 {
                                     int a =(*itPoint).second.first;
                                     int b =(*itPoint).second.second;
                                    auto c = chi2Points[i];

                                    int d = c.second[j].second.first;
                                    int e = c.second[j].second.second;
                                     
                                    if (a==d && b==e)
                                        { 
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
                            if (int(chi2Points.size())==nTracks)     break;
                          }
                }
                if ((bestChi2SUM == 0 || chi2 < bestChi2SUM) && int(chi2Points.size())==nTracks)
                {   if (int(chi2Points.size())==nTracks) bestChi2SUM = chi2;
	                if (int(chi2Points.size())==nTracks)    outPut = chi2Points;
                }
                Try += 1;
        } 
    if (int(outPut.size())!=nTracks)
        {
        std::cout<< "Error 0, Not enough tracks were reconstructed! "<< nTracks << " were expected."<<std::endl;
        return outPut;
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
    

    //chi2 hist
    TH1F* chi2Hist = new TH1F("chi2Hist","chi2Hist",40,0,60);



    for(int ievt(0); ievt<data->GetEntries(); ievt++)
    {
        data->GetEntry(ievt);

        std::cout << "-oOo--oOo--oOo--oOo--oOo--oOo-\n";
        std::cout << "Event: " << ievt << std::endl;
        std::cout << "-------------CMOS-------------" << std::endl;
        std::cout << "SciDet: " << ievt << std::endl;
        std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        std::cout << "Nhits:" << Counter.size() << std::endl;
        std::map<G4int, std::vector< CMOSPixel*> >::iterator it2;

        CMOSPixel* def = new CMOSPixel();
        std::vector< CMOSPixel*>  det1, det2, det3, det4;
        det1.push_back(def);
        det2.push_back(def);
        det3.push_back(def);
        det4.push_back(def);

        std::cout<< "X = " << det1[0]->GetX() << std::endl;
    	nTracks =0;
        for(it2=Counter.begin(); it2!=Counter.end(); it2++)
        {
         unsigned short int Plane = (*it2).first;
         unsigned short int nHitsInPlane = (*it2).second.size();

                 std::cout << "Plane " << Plane << " has " << nHitsInPlane << " pixels above threshold" << std::endl;
        

        if (nHitsInPlane < nTracks || nTracks ==0) nTracks = nHitsInPlane;
        if (Plane==0)  det1 = (*it2).second;
        if (Plane==1)  det2 = (*it2).second;
        if (Plane==2)  det3 = (*it2).second;
        if (Plane==3)  det4 = (*it2).second;

     
  
		}

		//if (det1.size == 0) det1 =
		//std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> chi2points = SortTracksByChi2(det1,det2,det3,det4);
		std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> chi2points = SortTracksByChi2(det1,det2,det3,det4);
        std::cout << "Size of chi2Points: "<< chi2points.size() << std::endl;
        
        std::cout << "Test of imput Tracks" << std::endl;
        for(int i = 0; i < int(chi2points.size()); i++)
                            {
                                 std::cout <<"chi2 = " <<chi2points[i].first <<std::endl;

                                 for(int j=0; j< int(chi2points[i].second.size());j++)
                                 {std::cout << chi2points[i].second[j].second.first << ", " << chi2points[i].second[j].second.second << " TrackID = " << chi2points[i].second[j].first <<std::endl;}
                            }  

                std::cout << "There is nothing else in the input of TrackSelector" << std::endl;
                
        //std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> finalTracks = TrackSelector(chi2points);
        std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> finalTracks = TrackSelector(chi2points);

        

        std::cout << "OutputTrackSelector" << std::endl;
                  for(int i = 0; i < int(finalTracks.size()); i++)
                            {
                                 std::cout <<"chi2 = " <<finalTracks[i].first <<std::endl;	
                                 std::cout << "Size of the output = " << int(finalTracks[i].second.size()) << std::endl;	
 								chi2Hist->Fill(finalTracks[i].first);
 								int prevTrackID = 0;
 								bool realTrack = true;
                                 for(int j=0; j< int(finalTracks[i].second.size());j++)
                                 {
                                 	std::cout << finalTracks[i].second[j].second.first << ", " << finalTracks[i].second[j].second.second << " TrackID = " << finalTracks[i].second[j].first <<std::endl;
                                 	
                                 	if(j!=0 && finalTracks[i].second[j].first != prevTrackID) {
                                 		std::cout<< "A false track has been found!"<<std::endl;
                                 		realTrack = false;}
                                 	prevTrackID = finalTracks[i].second[j].first;
                                 }
                                 if (realTrack) goodTracks++;
                            }   

	    
	    }

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



    std::cout << goodTracks << " good tracks were found" << std::endl;
    */
	inputFile->Close();
    theApp->Run();
    return 0;
}

        
