#ifndef CC_THIRD_PARTY_LAPOS_CRF_H_
#define CC_THIRD_PARTY_LAPOS_CRF_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>
#include <cstdio>

#include "string_store.h"

#include <tr1/unordered_map>

extern bool USE_EDGE_TRIGRAMS;

extern int LOOKAHEAD_DEPTH;

// use this option and change MAX_LABEL_TYPES below accordingly, if you want to
// use more than 255 labels
//#define USE_MANY_LABELS

//
// data format for each sample for training/testing
//
struct CRF_State {
  public:
    CRF_State() : label("") {}
    CRF_State(const std::string& l) : label(l) {}
    void set_label(const std::string& l) {
        label = l;
    }

    // to add a binary feature
    void add_feature(const std::string& f) {
        if (f.find_first_of('\t') != std::string::npos) {
            std::cerr << "error: illegal characters in a feature string"
                      << std::endl;
            exit(1);
        }
        features.push_back(f);
    }

  public:
    std::string label;
    std::vector<std::string> features;
};

struct CRF_Sequence {
  public:
    void add_state(const CRF_State& s) {
        vs.push_back(s);
    }

  public:
    std::vector<CRF_State> vs;
};

class CRF_Model {
  public:
    enum OptimizationMethod { BFGS, PERCEPTRON, SGD };

    void add_training_sample(const CRF_Sequence& s);
    int train(OptimizationMethod method, const int cutoff = 0,
              const double sigma = 0, const double widthfactor = 0);
    //  std::vector<double> classify(CRF_State & s) const;
    void
    decode_forward_backward(CRF_Sequence& s,
                            std::vector<std::map<std::string, double>>& tagp);
    void decode_viterbi(CRF_Sequence& s);
    void decode_nbest(
        CRF_Sequence& s0,
        std::vector<std::pair<double, std::vector<std::string>>>& nbest,
        const int num, const double min_prob);
    void decode_lookahead(CRF_Sequence& s0);
    bool load_from_file(const std::string& filename, bool verbose = true);
    bool save_to_file(const std::string& filename, const double t = 0) const;
    int num_classes() const {
        return _num_classes;
    }
    std::string get_class_label(int i) const {
        return _label_bag.Str(i);
    }
    int get_class_id(const std::string& s) const {
        return _label_bag.Id(s);
    }
    void get_features(
        std::list<std::pair<std::pair<std::string, std::string>, double>>& fl);
    void set_heldout(const int h, const int n = 0) {
        _nheldout = h;
        _early_stopping_n = n;
    }
    //  bool load_from_array(const CRF_Model_Data data[]);

    enum { MAX_LABEL_TYPES = 50 };
    //  const static int MAX_LABEL_TYPES = 1000;
    enum { MAX_LEN = 1000 };

    void incr_line_counter() {
        _line_counter++;
    }

    CRF_Model();
    ~CRF_Model();

  private:
    struct Sample {
        int label;
        std::vector<int> positive_features;
    };
    struct Sequence {
        std::vector<Sample> vs;
    };

#ifdef USE_MANY_LABELS
    typedef uint64_t mefeature_type;

    class ME_Feature {
      public:
        ME_Feature(uint32_t l, uint32_t f) : _label(l), _feature(f) {}

        uint32_t label() const {
            return _label;
        }

        uint32_t feature() const {
            return _feature;
        }

        //    mefeature_type body() const { return (((uint64_t)_feature) << 32) +
        //    _label; }
        mefeature_type body() const {
            uint64_t a = _feature;
            return (a << 32) + _label;
        }

      private:
        uint32_t _label;
        uint32_t _feature;
    };
#else
    typedef uint32_t mefeature_type;

    struct ME_Feature {
        ME_Feature(const int l, const int f) : _body((f << 8) + l) {
            assert(l >= 0 && l <= MAX_LABEL_TYPES);
            assert(f >= 0 && f <= 0xffffff);
        }

        uint32_t label() const {
            return _body & 0xff;
        }

        uint32_t feature() const {
            return _body >> 8;
        }

        mefeature_type body() const {
            return _body;
        }

      private:
        mefeature_type _body;
    };
#endif

    struct ME_FeatureBag {
        //    typedef __gnu_cxx::hash_map<mefeature_type, int> map_type;
        typedef std::tr1::unordered_map<mefeature_type, int32_t> map_type;
        map_type mef2id;
        std::vector<ME_Feature> id2mef;

        int32_t Put(const ME_Feature& i) {
            map_type::const_iterator j = mef2id.find(i.body());
            if (j == mef2id.end()) {
                int32_t id = static_cast<int32_t>(id2mef.size());
                id2mef.push_back(i);
                mef2id[i.body()] = id;
                return id;
            }
            return j->second;
        }

        int32_t Id(const ME_Feature& i) const {
            map_type::const_iterator j = mef2id.find(i.body());
            if (j == mef2id.end()) {
                return -1;
            }
            return j->second;
        }

        ME_Feature Feature(int32_t id) const {
            assert(id >= 0 && (size_t)id < id2mef.size());
            return id2mef[id];
        }

        size_t Size() const {
            return id2mef.size();
        }

        void Clear() {
            mef2id.clear();
            id2mef.clear();
        }
    };

    struct hashfun_str {
        size_t operator()(const std::string& s) const {
            const int32_t* p = reinterpret_cast<const int32_t*>(s.c_str());
            size_t v = 0;
            int32_t n = static_cast<int32_t>(s.size() / 4);
            for (int32_t i = 0; i < n; i++, p++) {
                //      v ^= *p;
                v ^= *p << (4 * (i % 2));  // note) 0 <= char < 128  // bug??
                                           //        v ^= *p << (i % 2);
            }
            int32_t m = s.size() % 4;
            for (int32_t i = 0; i < m; i++) {
                v ^= s[4 * n + i] << (i * 8);
            }
            return v;
        }
    };

    struct MiniStringBag {
        typedef std::tr1::unordered_map<std::string, int32_t, hashfun_str> map_type;
        size_t _size;
        map_type str2id;

        MiniStringBag() : _size(0ul) {}

        int32_t Put(const std::string& i) {
            map_type::const_iterator j = str2id.find(i);
            if (j == str2id.end()) {
                int32_t id = (int32_t)_size;
                _size++;
                str2id[i] = id;
                return id;
            }
            return j->second;
        }

        int32_t Id(const std::string& i) const {
            map_type::const_iterator j = str2id.find(i);
            if (j == str2id.end())
                return -1;
            return j->second;
        }

        size_t Size() const {
            return _size;
        }

        void Clear() {
            str2id.clear();
            _size = 0;
        }

        map_type::const_iterator begin() const {
            return str2id.begin();
        }

        map_type::const_iterator end() const {
            return str2id.end();
        }
    };

    struct StringBag : public MiniStringBag {
        std::vector<std::string> id2str;

        int32_t Put(const std::string& i) {
            map_type::const_iterator j = str2id.find(i);
            if (j == str2id.end()) {
                int32_t id = static_cast<int32_t>(id2str.size());
                id2str.push_back(i);
                str2id[i] = id;
                return id;
            }

            return j->second;
        }

        std::string Str(const int32_t id) const {
            assert(id >= 0 && static_cast<size_t>(id) < id2str.size());
            return id2str[static_cast<size_t>(id)];
        }

        size_t Size() const {
            return id2str.size();
        }

        void Clear() {
            str2id.clear();
            id2str.clear();
        }
    };

    struct Path {
        double score;
        double new_score;
        std::vector<int> vs;
        Path(const double s, const std::vector<int>& v) : score(s), vs(v) {}
        bool operator<(const Path& p) const {
            return score > p.score;
        }
        std::string str() const {
            char buf[100];
            sprintf(buf, "%f\t", score);
            std::string s(buf);
            for (std::vector<int>::const_iterator i = vs.begin(); i != vs.end();
                 i++) {
                sprintf(buf, "%d ", *i);
                s += std::string(buf);
            }
            return s;
        }
    };

    std::vector<Sequence> _vs;  // vector of training_samples
    StringBag _label_bag;
    //  MiniStringBag _featurename_bag;
    StringStore _featurename_bag;
    double _sigma;  // Gaussian prior
    double _inequality_width;
    std::vector<double> _vl;  // vector of lambda
    std::vector<bool> is_edge;
    ME_FeatureBag _fb;
    int _num_classes;
    std::vector<double> _vee;  // empirical expectation
    std::vector<double> _vme;  // model expectation
    std::vector<std::vector<int>> _feature2mef;
    std::vector<Sequence> _heldout;
    double _train_error;    // current error rate on the training data
    double _heldout_error;  // current error rate on the heldout data
    int _nheldout;
    int _early_stopping_n;
    std::vector<double> _vhlogl;

    double heldout_likelihood();
    double heldout_lookahead_error();
    double forward_backward(const Sequence& s);
    double viterbi(const Sequence& seq, std::vector<int>& best_seq);
    void initialize_edge_weights();
    void initialize_state_weights(const Sequence& seq);
    //  void lookahead_initialize_edge_weights();
    void lookahead_initialize_state_weights(const Sequence& seq);
    int make_feature_bag(const int cutoff);
    //  int classify(const Sample & nbs, std::vector<double> & membp) const;
    double update_model_expectation();
    double add_sample_model_expectation(const Sequence& seq,
                                        std::vector<double>& vme,
                                        int& ncorrect);
    void add_sample_empirical_expectation(const Sequence& seq,
                                          std::vector<double>& vee);
    int perform_BFGS();
    int perform_AveragedPerceptron();
    int perform_StochasticGradientDescent();
    int perform_LookaheadTraining();

    double lookahead_search(const Sequence& seq, std::vector<int>& history,
                            const int start, const int max_depth,
                            const int depth, double current_score,
                            std::vector<int>& best_seq,
                            const bool follow_gold = false,
                            const std::vector<int>* forbidden_seq = NULL);
    void calc_diff(const double val, const Sequence& seq, const int start,
                   const std::vector<int>& history, const int depth,
                   const int max_depth, std::map<int, double>& diff);
    int update_weights_sub(const Sequence& seq, std::vector<int>& history,
                           const int x, std::map<int, double>& diff);
    int update_weights_sub2(const Sequence& seq, std::vector<int>& history,
                            const int x, std::map<int, double>& diff);
    int update_weights_sub3(const Sequence& seq, std::vector<int>& history,
                            const int x, std::map<int, double>& diff);
    int lookaheadtrain_sentence(const Sequence& seq, int& t,
                                std::vector<double>& wa);
    int decode_lookahead_sentence(const Sequence& seq, std::vector<int>& vs);

    void init_feature2mef();
    double calc_loglikelihood(const Sequence& seq);
    //  std::vector<double> calc_state_weight(const Sequence & seq, const int i)
    //  const;
    std::vector<double> calc_state_weight(const int i) const;
    void nbest_search(const double lb, const int len, const int x, const int y,
                      const double rhs_score, std::vector<Path>& vp);
    double nbest(const Sequence& seq, std::vector<Path>& sequences,
                 const int max_num, const double min_prob);

    double FunctionGradient(const std::vector<double>& x,
                            std::vector<double>& grad);
    static double FunctionGradientWrapper(const std::vector<double>& x,
                                          std::vector<double>& grad);

    int _line_counter;  // for error message. Incremented at forward_backward

    int nbest_search_path[CRF_Model::MAX_LEN];
    /*
    static int
    edge_feature_id[CRF_Model::MAX_LABEL_TYPES][CRF_Model::MAX_LABEL_TYPES];
    static double state_weight[MAX_LEN][CRF_Model::MAX_LABEL_TYPES];
    static double
    edge_weight[CRF_Model::MAX_LABEL_TYPES][CRF_Model::MAX_LABEL_TYPES];
    static double forward_cache[MAX_LEN][CRF_Model::MAX_LABEL_TYPES];
    static double backward_cache[MAX_LEN][CRF_Model::MAX_LABEL_TYPES];
    static int backward_pointer[MAX_LEN][CRF_Model::MAX_LABEL_TYPES];
    */
    int* p_edge_feature_id;
    int* p_edge_feature_id2;
    int* p_edge_feature_id3;
    double* p_state_weight;
    double* p_edge_weight;
    double* p_edge_weight2;
    double* p_edge_weight3;
    double* p_forward_cache;
    double* p_backward_cache;
    int* p_backward_pointer;

    int& edge_feature_id3(const int w, const int x, const int y,
                          const int z) const {
        assert(w >= 0 && w < MAX_LABEL_TYPES);
        assert(x >= 0 && x < MAX_LABEL_TYPES);
        assert(y >= 0 && y < MAX_LABEL_TYPES);
        assert(z >= 0 && z < MAX_LABEL_TYPES);
        return p_edge_feature_id3[w * MAX_LABEL_TYPES * MAX_LABEL_TYPES *
                                      MAX_LABEL_TYPES +
                                  x * MAX_LABEL_TYPES * MAX_LABEL_TYPES +
                                  y * MAX_LABEL_TYPES + z];
    }
    int& edge_feature_id2(const int x, const int y, const int z) const {
        assert(x >= 0 && x < MAX_LABEL_TYPES);
        assert(y >= 0 && y < MAX_LABEL_TYPES);
        assert(z >= 0 && z < MAX_LABEL_TYPES);
        //      std::cout << x << " " << y << " " << z << std::endl;
        return p_edge_feature_id2[x * MAX_LABEL_TYPES * MAX_LABEL_TYPES +
                                  y * MAX_LABEL_TYPES + z];
    }
    int& edge_feature_id(const int l, const int r) const {
        assert(l >= 0 && l < MAX_LABEL_TYPES);
        assert(r >= 0 && r < MAX_LABEL_TYPES);
        return p_edge_feature_id[l * MAX_LABEL_TYPES + r];
    }
    double& state_weight(const int x, const int l) const {
        return p_state_weight[x * MAX_LABEL_TYPES + l];
    }
    double& edge_weight2(const int x, const int y, const int z) const {
        return p_edge_weight2[x * MAX_LABEL_TYPES * MAX_LABEL_TYPES +
                              y * MAX_LABEL_TYPES + z];
    }
    double& edge_weight3(const int w, const int x, const int y,
                         const int z) const {
        return p_edge_weight3[w * MAX_LABEL_TYPES * MAX_LABEL_TYPES *
                                  MAX_LABEL_TYPES +
                              x * MAX_LABEL_TYPES * MAX_LABEL_TYPES +
                              y * MAX_LABEL_TYPES + z];
    }
    double& edge_weight(const int l, const int r) const {
        return p_edge_weight[l * MAX_LABEL_TYPES + r];
    }
    double& forward_cache(const int x, const int l) const {
        return p_forward_cache[x * MAX_LABEL_TYPES + l];
    }
    double& backward_cache(const int x, const int l) const {
        return p_backward_cache[x * MAX_LABEL_TYPES + l];
    }
    int& backward_pointer(const int x, const int l) const {
        return p_backward_pointer[x * MAX_LABEL_TYPES + l];
    }

    double forward_prob(const int len);
    double backward_prob(const int len);
};

#endif  // CC_THIRD_PARTY_LAPOS_CRF_H_
