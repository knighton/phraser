#ifndef CC_THIRD_PARTY_LAPOS_CRFPOS_H_
#define CC_THIRD_PARTY_LAPOS_CRFPOS_H_

#include <map>
#include <string>
#include <vector>

#include "cc/third_party/lapos/common.h"
#include "cc/third_party/lapos/crf.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace lapos {

int crftrain(const CRF_Model::OptimizationMethod method, CRF_Model& m,
             const vector<Sentence>& vs, double gaussian, const bool use_l1);

void crf_decode_lookahead(Sentence& s, CRF_Model& m,
                          vector<map<string, double>>& tagp);

void crf_decode_forward_backward(Sentence& s, CRF_Model& m,
                                 vector<map<string, double>>& tagp);

void crf_decode_nbest(Sentence& s, CRF_Model& m,
                      vector<pair<double, vector<string>>>& nbest_seqs, int n);

}  // namespace lapos

#endif  // CC_THIRD_PARTY_LAPOS_CRFPOS_H_
