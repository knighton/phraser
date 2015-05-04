#include <string>
#include <vector>

#include "cc/analysis/analysis_options.h"
#include "cc/analysis/analysis_result.h"
#include "cc/analysis/analyzer.h"
#include "cc/comment/comment.h"
#include "cc/comment/comment_reader.h"

using std::string;
using std::vector;

#define RED "\033[1;31m"
#define RESET "\033[0m"

static void OutputError(const string& error) {
    printf(RED "Failed: %s\n" RESET, error.data());
}

#undef RESET
#undef RED

static bool IsImpermiumThreat(const vector<string>& tags) {
    for (auto& tag : tags) {
        if (tag == "threat" || tag == "mild_threat" ||
                tag == "strong_threat") {
            return true;
        }
    }
    return false;
}

static bool HandleComment(
        const Analyzer& anal, AnalysisOptions& options, const Comment& comment,
        FILE* james_f, FILE* both_f, FILE* impermium_f, string* error) {
    AnalysisResult result;
    if (!anal.Analyze(comment.text(), options, &result, error)) {
        return false;
    }

    bool in_james = result.phrase_results.size();
    bool in_impermium = IsImpermiumThreat(comment.tags());
    FILE* f = in_james ? (in_impermium ? both_f : james_f) :
                         (in_impermium ? impermium_f : NULL);
    if (!f) {
        return true;
    }

    string s;
    result.AppendAsLine(&s);
    fprintf(f, "%s\n", s.data());
    fflush(f);  // Finding matches is rare and this runs for a long time.
    return true;
}

static bool Run(
        const string& lapos_model_f, const vector<string>& phrase_config_ff,
        const vector<string>& comment_ff, string* error) {
    Analyzer anal;
    if (!anal.Init(lapos_model_f, phrase_config_ff, error)) {
        return false;
    }

    CommentReader reader;
    if (!reader.Init(comment_ff)) {
        *error = "Comment reader init failed.";
        return false;
    }

    FILE* james_f = fopen("james_only.txt", "wb");
    FILE* both_f = fopen("both.txt", "wb");
    FILE* impermium_f = fopen("impermium_only.txt", "wb");

    vector<FILE*> files = {james_f, both_f, impermium_f};

    AnalysisOptions options;
    Comment comment;
    CommentReaderStatus r;
    while ((r = reader.Next(&comment, error)) == CRS_HAVE_COMMENT) {
        if (!HandleComment(anal, options, comment, james_f, both_f, impermium_f,
                           error)) {
            return false;
        }
    }

    switch (r) {
    case CRS_HAVE_COMMENT:
        *error = "Code error.";
        return false;
    case CRS_OUT_OF_COMMENTS:
        break;
    case CRS_ERROR:
        return false;
    }

    for (auto& f : files) {
        fclose(f);
    }

    return true;
}

int main(int argc, char* argv[]) {
    string lapos_model_f = "phraser/cc/third_party/lapos/model_wsj02-21/model.la";

    vector<string> phrase_config_ff = {
        "phraser/config/threat_command.txt",
        "phraser/config/threat_statement.txt",
    };

    vector<string> comment_ff;
    for (auto i = 1u; i < argc; ++i) {
        comment_ff.emplace_back(argv[i]);
    }

    string error;
    if (!Run(lapos_model_f, phrase_config_ff, comment_ff, &error)) {
        OutputError(error);
        return 1;
    } 

    return 0;
}
