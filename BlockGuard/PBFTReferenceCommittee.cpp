//
//  PBFTReferenceCommittee.cpp
//  BlockGuard
//
//  Created by Kendric Hood on 4/3/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#include <random>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <set>
#include "PBFTReferenceCommittee.hpp"

PBFTReferenceCommittee::PBFTReferenceCommittee(){
    _currentRound = 0;
    _groupSize = -1;
    _peers = Network<PBFT_Message, PBFTPeer_Sharded>();
    _busyGroups = std::vector<int>();
    _freeGroups = std::vector<int>();
    _groupIds = std::vector<int>();
    _nextCommitteeId = 0;
    _nextSquenceNumber = 0;
    _requestQueue = std::vector<transactionRequest>();
    _groups = std::map<int,aGroup>();
    _faultTolerance = 1;
    _currentCommittees = std::vector<int>();
    _log = nullptr;

    int seed = (int)time(nullptr);
    _randomGenerator = std::default_random_engine(seed);
}

PBFTReferenceCommittee::PBFTReferenceCommittee(const PBFTReferenceCommittee &rhs){
    _currentRound = rhs._currentRound;
    _groupSize = rhs._groupSize;
    _peers = rhs._peers;
    _busyGroups = rhs._busyGroups;
    _freeGroups = rhs._freeGroups;
    _groupIds = rhs._groupIds;
    _nextCommitteeId = rhs._nextCommitteeId;
    _nextSquenceNumber = rhs._nextSquenceNumber;
    _requestQueue = rhs._requestQueue;
    _groups = rhs._groups;
    _faultTolerance = rhs._faultTolerance;
    _currentCommittees = rhs._currentCommittees;
    _log = rhs._log;
    
    int seed = (int)time(nullptr);
    _randomGenerator = std::default_random_engine(seed);
}

void PBFTReferenceCommittee::makeGroup(std::vector<PBFTPeer_Sharded*> group, int id){
    for(int i = 0; i < group.size(); i++){
        group[i]->setGroup(id);
        for(int j = 0; j < group.size(); j++){
            if(group[i]->id() != group[j]->id()){
                group[i]->addGroupMember(*group[j]);
            }
        }
    }
    _freeGroups.push_back(id);
    _groupIds.push_back(id);
    _groups[id] = group;
}

void PBFTReferenceCommittee::initNetwork(int numberOfPeers){
    
    // 5 is high (all groups )1 is low (4 groups for 1024 peers)
    SECURITY_LEVEL_5 = numberOfPeers/_groupSize;
    SECURITY_LEVEL_4 = SECURITY_LEVEL_5/2;
    SECURITY_LEVEL_3 = SECURITY_LEVEL_4/2;
    SECURITY_LEVEL_2 = SECURITY_LEVEL_3/2;
    SECURITY_LEVEL_1 = SECURITY_LEVEL_2/2;
    
    _peers.initNetwork(numberOfPeers);
    
    int groupId = 0;
    std::vector<PBFTPeer_Sharded*> group = std::vector<PBFTPeer_Sharded*>();
    
    for(int i = 0; i < _peers.size(); i++){
        group.push_back(_peers[i]);
        
        if(group.size() == _groupSize){
            makeGroup(group,groupId);
            group = std::vector<PBFTPeer_Sharded*>();
            groupId++;
        }
    }

    // connects left over peers into s small group
    if(!group.empty()){
        makeGroup(group,groupId);
    }
    setFaultTolerance(_faultTolerance);
    return;
}

double PBFTReferenceCommittee::pickSecrityLevel(){

    std::uniform_int_distribution<int> coin(0,1);
    int trails = 0;
    int heads = coin(_randomGenerator);
    while(!heads){
        trails++;
        heads = coin(_randomGenerator);
    }
    
    switch (trails) {
        case 0: return SECURITY_LEVEL_1; break;
        case 1: return SECURITY_LEVEL_2; break;
        case 2: return SECURITY_LEVEL_3; break;
        case 3: return SECURITY_LEVEL_4; break;
        case 4: return SECURITY_LEVEL_5; break;

        default: return SECURITY_LEVEL_1; break;
    }
}

transactionRequest PBFTReferenceCommittee::generateRequest(){
    if(_requestQueue.empty()){
        transactionRequest request;
        request.securityLevel = pickSecrityLevel();
        request.id = _currentRound;
        
        return request;
    }
    return _requestQueue.front();
}

void PBFTReferenceCommittee::makeRequest(){
    queueRequest();
    int groupsNeeded = std::ceil(_requestQueue.front().securityLevel);
    updateBusyGroup();
    
    // return if there is not enough free groups to make the committee
    if(_freeGroups.size() < groupsNeeded){
        return;
    }
    _requestQueue.erase(_requestQueue.begin());
    
    std::vector<int> groupsInCommittee = std::vector<int>();
    while(groupsInCommittee.size() < groupsNeeded){
        int groupId = _freeGroups.back();
        _busyGroups.push_back(groupId);
        groupsInCommittee.push_back(groupId);
        _freeGroups.pop_back();
    }
    
    makeCommittee(groupsInCommittee);
    initCommittee(groupsInCommittee);
    
    for(int i = 0; i < groupsInCommittee.size(); i++){
        aGroup group = getGroup(groupsInCommittee[i]);
        for(int j = 0; j < group.size(); j++){
            if(group[j]->isPrimary()){
                group[j]->makeRequest(_nextSquenceNumber);
                _nextSquenceNumber++;
                return;
            }
        }
    }
    
}

void PBFTReferenceCommittee::updateBusyGroup(){
    std::vector<int> aliveCommittees = std::vector<int>(); // if a group is still bust we need the committee it belongs to
    // check each busy group to see if they are still busy
    auto id = _busyGroups.begin(); 
    while(id != _busyGroups.end()){
        aGroup group= getGroup(*id);
        bool stillBusy = false;
        for(int i = 0; i < group.size(); i++){
            // if the group is still busy it will still have a committee id and thus we need to track it
            if(group[i]->getCommittee() != -1){
                stillBusy = true;
                // find works in n^2 time and lower bound is log(n) is sorted
                auto found = std::lower_bound(aliveCommittees.begin(),aliveCommittees.end(),group[i]->getCommittee());
                if( found == aliveCommittees.end()){
                    aliveCommittees.push_back(group[i]->getCommittee());
                    std::sort(aliveCommittees.begin(),aliveCommittees.end()); // sort for next check
                }
                break; // no need to check rest of group
            }
        }
        // if the group is not still alive then we need to move its group id to free and remove from busy
        if(!stillBusy){
            _freeGroups.push_back(*id);
            _busyGroups.erase(id);
        // else we just continue
        }else{
            id++;
        }
    }

    _currentCommittees = aliveCommittees; // update list of currently active committees
}

std::vector<aGroup> PBFTReferenceCommittee::getCommittee(int committeeId){
    std::vector<aGroup> committee = std::vector<aGroup>();

    // need to find every group that belongs to committee id
    for(auto id = _groupIds.begin(); id != _groupIds.end(); id++){ 
        aGroup group = getGroup(*id);
        // check each group member to see if they belong to that group
        bool inCommittee = true;
        for(auto peer = group.begin(); peer != group.end(); peer++){
            if((*peer)->getCommittee() != committeeId){
                inCommittee = false;
                break;
            }
        }
        if(inCommittee){
            committee.push_back(group);
        }
    }

    return committee;
}

void PBFTReferenceCommittee::initCommittee(std::vector<int> groupsInCommittee){
    for(auto groupId = groupsInCommittee.begin(); groupId != groupsInCommittee.end(); groupId++){
        aGroup groupInCommittee = getGroup(*groupId);
        for(int peerIndex = 0; peerIndex < groupInCommittee.size(); peerIndex++){
            groupInCommittee[peerIndex]->clearPrimary();
            groupInCommittee[peerIndex]->initPrimary();
        }
    }
}


void PBFTReferenceCommittee::makeCommittee(std::vector<int> groupsForCommittee){
    // gets list of all committee members
    std::vector<PBFTPeer_Sharded*> committeeMembers =  std::vector<PBFTPeer_Sharded*>();
    for(auto groupId = groupsForCommittee.begin(); groupId != groupsForCommittee.end(); groupId++){
        aGroup groupInCommitte = getGroup(*groupId);
        for(int peerIndex = 0; peerIndex < groupInCommitte.size(); peerIndex++){
            committeeMembers.push_back(groupInCommitte[peerIndex]);
        }
    }

    // clear old committee
    for(int peer = 0; peer < committeeMembers.size(); peer++){
            committeeMembers[peer]->clearCommittee();
    }

    // joins them togeather
    for(int peer = 0; peer < committeeMembers.size(); peer++){
        committeeMembers[peer]->setCommittee(_nextCommitteeId);
        for(int otherPeer = 0; otherPeer < committeeMembers.size(); otherPeer++){
            if(!(committeeMembers[peer]->id() == committeeMembers[otherPeer]->id())){
                committeeMembers[peer]->addCommitteeMember(*committeeMembers[otherPeer]);
            }
        }
    }
    _currentCommittees.push_back(_nextCommitteeId);
    _nextCommitteeId++;
}

void PBFTReferenceCommittee::setFaultTolerance(double f){
    _faultTolerance = f;
    for(int i = 0; i < _peers.size(); i++){
        _peers[i]->setFaultTolerance(_faultTolerance);
    }
}

aGroup PBFTReferenceCommittee::getGroup(int id){
    return _groups[id];
}

std::vector<PBFTPeer_Sharded> PBFTReferenceCommittee::getPeers()const{
    std::vector<PBFTPeer_Sharded> peers = std::vector<PBFTPeer_Sharded>();
    for(int i = 0; i < _peers.size(); i++){
        peers.push_back(*_peers[i]);
    }
    return peers;
}

std::vector<PBFT_Message> PBFTReferenceCommittee::getGlobalLedger()const{
    std::vector<PBFT_Message> globalLegder;
    for(int i = 0; i < _peers.size(); i++){
        auto localLedger = _peers[i]->getLedger();
        for(auto transaction = localLedger.begin(); transaction != localLedger.end(); transaction++){
            bool found = false;
            for(auto global = globalLegder.begin(); global != globalLegder.end(); global++){
                if(*transaction == *global){
                    found = true;
                }
            }
            if(!found){
                globalLegder.push_back(*transaction);
            }
        }
    }
    return globalLegder;
}

void PBFTReferenceCommittee::setMaxSecurityLevel(int max){
    if(SECURITY_LEVEL_5 > max){
        SECURITY_LEVEL_5 = max;
    }
    if(SECURITY_LEVEL_4 > max){
        SECURITY_LEVEL_4 = max;
    }
    if(SECURITY_LEVEL_3 > max){
        SECURITY_LEVEL_3 = max;
    }
    if(SECURITY_LEVEL_2 > max){
        SECURITY_LEVEL_2 = max;
    }
    if(SECURITY_LEVEL_1 > max){
        SECURITY_LEVEL_1 = max;
    }
}

void PBFTReferenceCommittee::setMinSecurityLevel(int min){
    if(SECURITY_LEVEL_5 < min){
        SECURITY_LEVEL_5 = min;
    }
    if(SECURITY_LEVEL_4 < min){
        SECURITY_LEVEL_4 = min;
    }
    if(SECURITY_LEVEL_3 < min){
        SECURITY_LEVEL_3 = min;
    }
    if(SECURITY_LEVEL_2 < min){
        SECURITY_LEVEL_2 = min;
    }
    if(SECURITY_LEVEL_1 < min){
        SECURITY_LEVEL_1 = min;
    }
}

PBFTReferenceCommittee& PBFTReferenceCommittee::operator=(const PBFTReferenceCommittee &rhs){
    _currentRound = rhs._currentRound;
    _groupSize = rhs._groupSize;
    _peers = rhs._peers;
    _busyGroups = rhs._busyGroups;
    _freeGroups = rhs._freeGroups;
    _groupIds = rhs._groupIds;
    _nextCommitteeId = rhs._nextCommitteeId;
    _nextSquenceNumber = rhs._nextSquenceNumber;
    _requestQueue = rhs._requestQueue;
    _groups = rhs._groups;
    _faultTolerance = rhs._faultTolerance;
    _currentCommittees = rhs._currentCommittees;
    _log = rhs._log;

    int seed = (int)time(nullptr);
    _randomGenerator = std::default_random_engine(seed);
    
    return *this;
}

std::ostream& PBFTReferenceCommittee::printTo(std::ostream& out)const{
    out<< "-- REFERENCE COMMITTEE SETUP --"<< std::endl<< std::endl;
    out<< std::left;
    out<< '\t'<< std::setw(LOG_WIDTH)<< "Current Round"<< std::setw(LOG_WIDTH)<< "Group Size"<< std::setw(LOG_WIDTH)<< "Number Of Groups"<< std::setw(LOG_WIDTH)<< "Number Of Free Groups"<< std::setw(LOG_WIDTH)<< "Number Of Busy Groups"<< std::setw(LOG_WIDTH)<< std::endl;
    out<< '\t'<< std::setw(LOG_WIDTH)<< _currentRound<< std::setw(LOG_WIDTH)<< _groupSize<< std::setw(LOG_WIDTH)<< _groupIds.size()<< std::setw(LOG_WIDTH)<< _freeGroups.size()<< std::setw(LOG_WIDTH)<< _busyGroups.size()<< std::setw(LOG_WIDTH)<< std::endl;
    out<< '\t'<< std::setw(LOG_WIDTH)<< "Request Queue Size"<< std::setw(LOG_WIDTH)<< "Next Committee Id"<< std::endl;
    out<< '\t'<< std::setw(LOG_WIDTH)<< _requestQueue.size()<< std::setw(LOG_WIDTH)<< _nextCommitteeId<< std::endl;
    
    _peers.printTo(out);
    
    return out;
}
