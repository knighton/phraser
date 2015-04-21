


class SentenceTagging {
  public:
    const vector<string>& tags() const { return tags_; }
    double score() const { return score_; }

    void Init(const vector<string>& tags, double score);

  private:
    vector<string> tags_;
    double score_;
};




class LaposTagger {
  public:
    bool Init(const string& model_f);

    void GetTaggings(const vector<string>& tokens, size_t top_n, vector<<vector<string>>& tag_lists);
};

bool LaposTagger::Init(const string& model_f) {
    bool is_enju = false;
    return crfm_.load_from_file(model_f, is_enju);
}

void LaposTagger::Tag(const vector<string>& tokens, 
