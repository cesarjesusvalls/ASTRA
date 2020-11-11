void pCTTrackingManager::DoCMOSTracking(){
    isCMOSReco.clear();
    recoTracks.clear();
    int Nplanes    = 4;
    int maxTracks  = 10;
    int Ntracks    = 1000;
    double zPos[4] = {fconfig->GetPosZ0(),fconfig->GetPosZ1(),fconfig->GetPosZ2(),fconfig->GetPosZ3()};
    std::map<G4int, std::vector< CMOSPixel* > > planeToHits = fevent->GetPixelHitsMap();
    std::map<G4int, std::vector< CMOSPixel*> >::iterator plane;
    std::vector< CMOSPixel*>::iterator cmosHit;



    for(plane=planeToHits.begin(); plane!=planeToHits.end(); plane++) 
        if ((*plane).second.size() < Ntracks) Ntracks = (*plane).second.size(); 
    if(Ntracks>maxTracks) {return;}
    std::map<int, std::vector< std::vector<CMOSPixel*>>> cmos_tracks;
    // start creating 1 cmos_track per hit in the first plane
    for(uint i(0); i<planeToHits[0].size(); i++){
        isCMOSReco[i] = true;
        std::vector<std::vector<CMOSPixel*>> new_set_of_tracks;
        std::vector<CMOSPixel*> new_track;
        new_track.push_back(planeToHits[0][i]);
        new_set_of_tracks.push_back(new_track);
        cmos_tracks[i] = new_set_of_tracks;
        int trks_size = 1;
        while(isCMOSReco[i] and trks_size<Nplanes){
            std::vector<std::vector<CMOSPixel*>> tmp_tracks;
            int tracks_to_rm = (int) cmos_tracks[i].size();
            for(int trk(0); trk<(int)cmos_tracks[i].size(); trk++){
                int p_it = (int)cmos_tracks[i][trk].size();
                int compat_count = 0;
                for(uint j(0); j<planeToHits[p_it].size(); j++){
                    if(checkHitsCompatibility(cmos_tracks[i][trk].back(),planeToHits[p_it][j],zPos[p_it]-zPos[p_it-1])){
                        std::vector<CMOSPixel*> new_tmp_track;
                        for(auto tmpTrkHit:cmos_tracks[i][trk]) new_tmp_track.push_back(tmpTrkHit);
                        new_tmp_track.push_back(planeToHits[p_it][j]);
                        tmp_tracks.push_back(new_tmp_track);
                        compat_count++;
                    }
                }
                if(compat_count==1) for (auto tmpTrk:tmp_tracks) cmos_tracks[i].push_back((tmpTrk));
                else isCMOSReco[i] = false;
                cmos_tracks[i].erase(cmos_tracks[i].begin(), cmos_tracks[i].begin() + tracks_to_rm);
            }
            trks_size++;
        }
    }
    for(uint i(0); i<planeToHits[0].size(); i++){
        if(isCMOSReco[i])
            for(auto cmoshit:cmos_tracks[i][0]) std::cout << cmoshit->GetX() << "," << cmoshit->GetY() << std::endl;
    }
    fcmosTracks = cmos_tracks;
}