//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;


//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
//gshare
uint32_t BHR;
int *predictTable;

//tournament
uint32_t globalhistory;
uint32_t *lht;   //local history table
uint8_t *lCounter ;    //local counter 2bit
uint8_t *gCounter;   //global Counter 2bit
uint8_t *choicePredict; 
uint8_t localPredict;
uint8_t globalPredict;
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

void
init_Tournament(){
  int numLHT = 1<<pcIndexBits;  
  int numLCounter = 1<<lhistoryBits;
  int numGCounter = 1<<ghistoryBits;
  
  lht = (uint32_t*) malloc(sizeof(uint32_t) * numLHT);
  lCounter = (uint8_t*)malloc(sizeof(uint8_t) * numLCounter );
  
  gCounter= (uint8_t*) malloc(sizeof(uint8_t) * numGCounter );
  choicePredict= (uint8_t*) malloc(sizeof(uint8_t) * numGCounter);

  for(int i = 0; i < numLHT; i++){
    lht[i] = 0;
  }
  for(int i = 0; i < numLCounter; i++){
    lCounter[i] = WN;
  }
  for(int i = 0; i < numGCounter; i++){
    gCounter[i] = WN;
    choicePredict[i] = WN;
  }
  globalhistory = 0;
  localPredict = NOTTAKEN;
  globalPredict = NOTTAKEN;
}

void
init_Gshare(){
  predictTable = (int *)malloc((1<<ghistoryBits) * sizeof(int));
  for (int i = 0; i < (1<<ghistoryBits); i++) predictTable[i] = WN;
  BHR = 0;
}

void
init_Custom(){
  int numLHT = 1<<pcIndexBits;  
  int numLCounter = 1<<lhistoryBits;
  int numGCounter = 1<<ghistoryBits;
  
  lht = (uint32_t*) malloc(sizeof(uint32_t) * numLHT);
  lCounter = (uint8_t*)malloc(sizeof(uint8_t) * numLCounter );
  
  gCounter= (uint8_t*) malloc(sizeof(uint8_t) * numGCounter );
  choicePredict= (uint8_t*) malloc(sizeof(uint8_t) * numGCounter);

  for(int i = 0; i < numLHT; i++){
    lht[i] = 0;
  }
  for(int i = 0; i < numLCounter; i++){
    lCounter[i] = WN3;
  }
  for(int i = 0; i < numGCounter; i++){
    gCounter[i] = WN3;
    choicePredict[i] = WN3;
  }
  globalhistory = 0;
  localPredict = NOTTAKEN;
  globalPredict = NOTTAKEN;
}


void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  switch (bpType) {
    case GSHARE:
    init_Gshare();
      break;
    case TOURNAMENT:
      init_Tournament();
      break;
      case CUSTOM:
       init_Custom();

    default:
      break;
  }

}

uint32_t 
mask(int bits){
  return (uint32_t)((1 << bits) - 1);
}



uint8_t 
makeTournamentPrediction(uint32_t pc) {
   
  //Local predictor
  
  uint32_t lhtIndex = (pc) & (mask(pcIndexBits));  
  uint32_t lCounterIndex= (lht[lhtIndex])& (mask(lhistoryBits));
  if (lCounter[lCounterIndex] >= WT) {
    localPredict = TAKEN;
  }
  else localPredict = NOTTAKEN;
  
  //Global predictor
  uint32_t gCounterIndex = (globalhistory) & (mask(ghistoryBits));
  if (gCounter[gCounterIndex] >= WT) {
    globalPredict = TAKEN;
  }
  else  globalPredict= NOTTAKEN;

  uint32_t predictor = choicePredict[gCounterIndex];

  if (predictor >= WT) {     
      return localPredict;
  } else {                                      
      return globalPredict;
  }

}



uint8_t 
makeCustomerPrediction(uint32_t pc) {
   
  //Local predictor
  
  uint32_t lhtIndex = (pc) & (mask(pcIndexBits));  
  uint32_t lCounterIndex= (lht[lhtIndex])& (mask(lhistoryBits));
  if (lCounter[lCounterIndex] >= WT1) {
    localPredict = TAKEN;
  }
  else localPredict = NOTTAKEN;
  
  //Global predictor
  uint32_t gCounterIndex = (globalhistory) & (mask(ghistoryBits));
  if (gCounter[gCounterIndex] >= WT1) {
    globalPredict = TAKEN;
  }
  else  globalPredict= NOTTAKEN;

  uint32_t predictor = choicePredict[gCounterIndex];

  if (predictor >= WT1) {     
      return localPredict;
  } else {                                      
      return globalPredict;
  }

}








uint8_t 
makeGsharePrediction(uint32_t pc){
  uint32_t index = (BHR ^ pc)&((1 << ghistoryBits)-1);
  uint8_t pred = predictTable[index];
  return (pred == WT || pred == ST)?TAKEN:NOTTAKEN;
}








// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return makeGsharePrediction(pc);
     
    case TOURNAMENT:
      return makeTournamentPrediction(pc);
    case CUSTOM:
      return makeCustomerPrediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}


void
train_choicePredictor(uint8_t outcome){
  
    uint32_t g_mask = mask(ghistoryBits);
    uint32_t gCounterIndex = globalhistory & g_mask;
    //choice predictor
    if (outcome == localPredict && outcome != globalPredict && choicePredict[gCounterIndex] !=ST) {
      choicePredict[gCounterIndex] +=1;
    } 
    else if(outcome != localPredict && outcome == globalPredict && choicePredict[gCounterIndex] !=SN)  {
      choicePredict[gCounterIndex] -=1;
    }
}




void
train_localPredictor(uint32_t pc, uint8_t outcome){
    //local predictor
  
    uint32_t lhtIndex = (pc) & (mask(pcIndexBits));  
    uint32_t lCounterIndex= (lht[lhtIndex])& (mask(lhistoryBits));
  

    if (outcome == TAKEN && lCounter[lCounterIndex] != ST) {       
      lCounter[lCounterIndex]  += 1;
    }
    else if (outcome == NOTTAKEN && lCounter[lCounterIndex] != SN) {
      lCounter[lCounterIndex]  -= 1;
    }

    lht[lhtIndex] = ((lht[lhtIndex] << 1) | outcome);
    lht[lhtIndex] = lht[lhtIndex] & (mask(lhistoryBits));

}

void 
train_globalPredictor(uint8_t outcome){
    //global predictor
  
    uint32_t g_mask = mask(ghistoryBits);
    uint32_t gCounterIndex = globalhistory & g_mask;

    if (outcome == TAKEN && gCounter[gCounterIndex] != ST) {       
      gCounter[gCounterIndex]  += 1;
    }
    else if (outcome == NOTTAKEN && gCounter[gCounterIndex] != SN) {
      gCounter[gCounterIndex]  -= 1;
    }

    globalhistory = ((globalhistory << 1) | outcome);
    globalhistory = globalhistory & (mask(ghistoryBits));

}







void
train_tournament(uint32_t pc, uint8_t outcome){
  train_choicePredictor(outcome);
  train_localPredictor(pc, outcome);
  train_globalPredictor(outcome); 
}


uint8_t
newVal(uint8_t pred, uint8_t outcome) {
  if(outcome == NOTTAKEN){
    if(pred == WN) return SN;
    else if(pred == ST) return WT;
    else if(pred == WT) return WN;
  }
  else{
    if(pred == WT) return ST;
    else if(pred == WN) return WT;
    else if(pred == SN) return WN;
  }
  return pred;
}

void
train_gsahre_predictor(uint32_t pc, uint8_t outcome)
{ 
  uint32_t index = (BHR ^ pc)&((1 << ghistoryBits)-1);
  uint8_t pred = predictTable[index];
  predictTable[index] = newVal(pred, outcome);
  BHR = (BHR<<1 | outcome) & ((1<<ghistoryBits) - 1);
}


// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  switch (bpType) {
    case GSHARE:
      train_gsahre_predictor(pc, outcome);
      break;
    case TOURNAMENT:
      train_tournament(pc, outcome);
      break;
    case CUSTOM:
      train_tournament(pc, outcome);
      break;
    default:
      break;
    }
}
