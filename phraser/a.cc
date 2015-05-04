#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>

#include "cc/misc/files.h"
#include "cc/misc/unicode.h"
#include "cc/misc/utf8.h"
#include "cc/analysis/analysis_options.h"
#include "cc/analysis/analysis_result.h"
#include "cc/analysis/analyzer.h"

using std::cin;
using std::getline;
using std::string;
using std::vector;

#define RED "\033[1;31m"
#define RESET "\033[0m"

static void OutputError(const string& error) {
        printf(RED "Failed: %s\n" RESET, error.c_str());
}

#undef RESET
#undef RED

static bool ParseLine(const ustring& line, ustring* comment,
                      vector<string>* tags) {
    comment->clear();
    tags->clear();

    size_t count = 0;
    size_t x = 0;
    uchar c;
    bool found = false;
    while (x < line.size() && (c = line[x++]) != ' ') {
        found = true;
        if (!('0' <= c && c <= '9')) {
            OutputError("Not a digit.");
            return false;
        }
        count += c - '0';
    }
    if (!found) {
        OutputError("Number of tags is missing.");
        return false;
    }

    for (auto i = 0; i < count; ++i) {
        while (x < line.size() && (c = line[x]) == ' ') {
            ++x;
        }

        string tag;
        while (x < line.size() && (c = line[x++]) != ' ') {
            if (!('a' <= c && c <= 'z') && c != '_') {
                OutputError("Not a letter.");
                return false;
            }
            tag += static_cast<char>(c);
        }
        tags->emplace_back(tag);
    }

    while (x < line.size() && (c = line[x]) == ' ') {
        ++x;
    }

    comment->reserve(line.size() - x);
    while (x < line.size()) {
        comment->emplace_back(line[x]);
        ++x;
    }

    return true;
}

static bool IsImpermiumThreat(const vector<string>& tags) {
    for (auto& tag : tags) {
        if (tag == "threat" || tag == "mild_threat" ||
                tag == "strong_threat") {
            return true;
        }
    }
    return false;
}

static void WriteHTMLHead(FILE* f) {
    fprintf(f, "<html>\n");
    fprintf(f, "<head>\n");
    fprintf(f, "    <style type=\"text/css\">\n");
    fprintf(f, "body { background: #ace; }\n");
    fprintf(f, ".comment { background: #bdf; margin: 5px; padding: 5px; }\n");
    fprintf(f, ".match { background: #fe0; }\n");
    fprintf(f, "    </style>\n");
    fprintf(f, "</head>\n");
    fprintf(f, "<body>\n");
}

static void WriteHTMLFoot(FILE* f) {
    fprintf(f, "</body></html>");
}

static bool DoFile(const Analyzer& anal, const AnalysisOptions& options,
                   const string& log_f, unordered_set<string>* james,
                   unordered_set<string>* both,
                   unordered_set<string>* impermium) {
    fprintf(stderr, "Processing [%s]\n", log_f.c_str());

    string bytes;
    if (!files::FileToString(log_f.data(), &bytes)) {
        OutputError("Data file DNE.\n");
        return false;
    }

    ustring line;
    size_t index = 0;
    ustring comment;
    vector<string> tags;
    AnalysisResult result;
    while (utf8::NextLine(bytes.data(), bytes.size(), &index, &line)) {
        if (!ParseLine(line, &comment, &tags)) {
            OutputError("Failed to parse line.");
            return false;
        }

        string error;
        if (!anal.Analyze(comment, options, &result, &error)) {
            OutputError(error);
            return false;
        }

        bool in_james = result.phrase_results.size();
        bool in_impermium = IsImpermiumThreat(tags);
        unordered_set<string>* set = in_james ?
            (in_impermium ? both : james) : (in_impermium ? impermium : NULL);
        if (!set) {
            continue;
        }

        string s;
        result.ToHTML(&s);
        set->insert(s);
    }

    return true;
}

static void Flatten(const unordered_set<string>& in, vector<string>* out) {
    out->clear();
    for (auto& s : in) {
        out->emplace_back(s);
    }
    sort(out->begin(), out->end());
}

int main(int argc, char* argv[]) {
    string lapos_model_f = "phraser/cc/third_party/lapos/model_wsj02-21/model.la";

    vector<string> phrase_config_ff = {
        "phraser/config/threat.txt",
    };

    Analyzer anal;
    string error;
    if (!anal.Init(lapos_model_f, phrase_config_ff, &error)) {
        OutputError(error);
        return 1;
    }

    AnalysisOptions options;
    unordered_set<string> james;
    unordered_set<string> both;
    unordered_set<string> impermium;
    for (auto i = 1u; i < argc; ++i) {
        if (!DoFile(anal, options, argv[i], &james, &both, &impermium)) {
            return 2;
        }
    }

    vector<string> file_names = {
        "james_only.html",
        "both.html",
        "impermium_only.html"
    };

    vector<unordered_set<string>*> lines_sets = {
        &james,
        &both,
        &impermium
    };

    for (auto i = 0; i < file_names.size(); ++i) {
        FILE* f = fopen(file_names[i].data(), "wb");

        vector<string> v;
        Flatten(*lines_sets[i], &v);

        WriteHTMLHead(f);
        for (auto& s : v) {
            fprintf(f, "%s", s.data());
        }
        WriteHTMLFoot(f);
    }

    printf("Done.\n");
}
