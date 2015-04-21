#ifndef LAPOS_CRFPOS_H_
#define LAPOS_CRFPOS_H_

#include <map>
#include <string>
#include <vector>

#include "common.h"
#include "crf.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

int crftrain(const CRF_Model::OptimizationMethod method, CRF_Model& m,
             const vector<Sentence>& vs, double gaussian, const bool use_l1);

void crf_decode_lookahead(Sentence& s, CRF_Model& m,
                          vector<map<string, double>>& tagp);

void crf_decode_forward_backward(Sentence& s, CRF_Model& m,
                                 vector<map<string, double>>& tagp);

void crf_decode_nbest(Sentence& s, CRF_Model& m,
                      vector<pair<double, vector<string>>>& nbest_seqs, int n);

#endif  // LAPOS_CRFPOS_H_
