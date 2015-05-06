#include <Python.h>
#include <string>
#include <vector>

#include "cc/misc/unicode.h"
#include "cc/analysis/analyzer.h"

using std::string;
using std::vector;

namespace {

char PHRASER_DOC[] =
    "Python extension that detects phrases in text.\n";

Analyzer* ANALYZER = NULL;

char INIT_DOC[] =
    "phrase config texts -> error str or None.\n"
    "\n"
    "Initialize the module.  Must call this first.\n"
    "\n"
    "    phrase_config_ff = [...]\n"
    "    phrases = map(lambda f: open(f).read(), phrase_config_ff)\n"
    "    err = phraser.Init(phrases)\n"
    "    assert not err\n";

PyObject* Init(PyObject* self, PyObject* args) {
    // Get args.
    PyObject* list;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) {
        return NULL;
    }

    // Extract phrase configs from list.
    vector<string> phrase_configs;
    Py_ssize_t length = PyList_Size(list);
    for (Py_ssize_t i = 0; i < length; ++i) {
        PyObject* s = PyList_GetItem(list, i);
        if (!PyString_Check(s)) {
            return PyUnicode_FromFormat(
                "[Phraser] List item at index %ld was not a string.", i);
        }
        phrase_configs.emplace_back(PyString_AsString(s));
    }

    // Allocate the Analyzer.
    if (!ANALYZER) {
        ANALYZER = new Analyzer();
    }

    // Call Init().
    string error;
    if (!ANALYZER->Init(phrase_configs, &error)) {
        return PyUnicode_FromString(error.data());
    }

    return Py_None;
}

char TO_D_DOC[] =
    "-> dict.\n"
    "\n"
    "Dump my state as a recursive dict.\n"
    "\n"
    "You can call phrase_detector_json_to_html.py to visualize the output as\n"
    "pretty HTML.\n"
    "\n"
    "    d = phrasser.to_d()\n"
    "    print d\n";

PyObject* ToD(PyObject* self, PyObject* args) {
    return NULL;
}

char ANALYZE_DOC[] =
    "text, options -> phrase detection result dict, error str.\n"
    "\n"
    "Analyze the text.  Returns a pair.  Either the dict or the error str\n"
    "will be None.\n"
    "\n"
    "    text = u'This is a comment.'\n"
    "    options = {\n"
    "        'destutter_max_consecutive': 3,\n"
    "        'replace_html_entities: True,\n"
    "    }\n"
    "    err = phraser.Analyze(text, options)\n"
    "    assert not err\n";

bool AnalysisOptionsFromDict(
        PyObject* obj, AnalysisOptions* options, string* error) {
    Py_ssize_t pos = 0;
    PyObject* key;
    PyObject* value;
    while (PyDict_Next(obj, &pos, &key, &value)) {
        if (!PyString_Check(key)) {
            *error = "[Phraser] Analysis options key is not a string.";
            return false;
        }
        const char* key_s = PyString_AsString(key);
        if (!strcmp(key_s, "destutter_max_consecutive")) {
            if (!PyInt_Check(value)) {
                *error = "[Phraser] Analysis option "
                         "'destutter_max_consecutive' is an integer.";
                return false;
            }
            Py_ssize_t size = PyInt_AsSsize_t(value);
            options->destutter_max_consecutive = static_cast<size_t>(size);
        } else if (!strcmp(key_s, "replace_html_entities")) {
            if (!PyBool_Check(value)) {
                *error = "[Phraser] Analysis option 'replace_html_entities' is "
                         "a bool.";
                return false;
            }
            options->replace_html_entities = PyObject_IsTrue(value);
        } else {
            *error = "[Phraser] Unknown analysis option.";
            return false;
        }
    }
    return true;
}

PyObject* UnicodeFromUstring(const ustring& s) {
    size_t bytes = sizeof(Py_UNICODE);

    // UCS 4 (eg, Linux).
    if (bytes == 4) {
        return PyUnicode_FromUnicode(s.data(), s.size());
    }

    // UCS 2 (eg, Mac OS X).
    if (bytes == 2) {
        vector<Py_UNICODE> v;
        for (auto& c : s) {
            v.emplace_back(static_cast<Py_UNICODE>(c));
        }
        return PyUnicode_FromUnicode(v.data(), v.size());
    }

    // Unknown.
    return NULL;
}

// Returned dicts look like
//
// {
//     'original_text':  'Some texxxxxxxt',
//     'clean_text':     'Some texxxt',
//     'tokens':         ['some', 'text'],
//     'phrase_results': [...],
// }
//
// where 'phrase_results' is a list like
//
// {
//     'phrase_name': 'threat_statement',
//     'subsequence_names': ['subject', 'aux', 'verb'],
//     'index_lists': [...]
// }
bool DictFromAnalysisResult(
        const AnalysisResult& result, PyObject** d, string* error) {
    *d = PyDict_New();
    if (!*d) {
        *error = "[Phraser] Result dict allocation failed.";
        return false;
    }

    PyObject* key;
    PyObject* value;
    vector<PyObject*> keys;
    vector<PyObject*> values;

    key = PyString_FromString("original_text");
    value = UnicodeFromUstring(result.original_text);
    keys.emplace_back(key);
    values.emplace_back(value);

    key = PyString_FromString("clean_text");
    value = UnicodeFromUstring(result.clean_text);
    keys.emplace_back(key);
    values.emplace_back(value);

    key = PyString_FromString("tokens");
    value = PyList_New(result.tokens.size());
    for (auto i = 0u; i < result.tokens.size(); ++i) {
        PyObject* token = PyString_FromString(result.tokens[i].data());
        PyList_SET_ITEM(value, i, token);
    }
    keys.emplace_back(key);
    values.emplace_back(value);

    key = PyString_FromString("phrase_matches");
    value = PyList_New(result.phrase_results.size());
    for (auto i = 0u; i < result.phrase_results.size(); ++i) {
        auto& phrase_result = result.phrase_results[i];

        vector<PyObject*> tmp_keys;
        vector<PyObject*> tmp_values;

        PyObject* tmp_key = PyString_FromString("phrase_name");
        PyObject* tmp_value = PyString_FromString(
            phrase_result.phrase_name.data());
        tmp_keys.emplace_back(tmp_key);
        tmp_values.emplace_back(tmp_value);

        tmp_key = PyString_FromString("subsequence_names");
        tmp_value = PyList_New(phrase_result.piece_names.size());
        for (auto j = 0u; j < phrase_result.piece_names.size(); ++j) {
            PyObject* subsequence_name =
                PyString_FromString(phrase_result.piece_names[j].data());
            PyList_SET_ITEM(tmp_value, j, subsequence_name);
        }
        tmp_keys.emplace_back(tmp_key);
        tmp_values.emplace_back(tmp_value);

        tmp_key = PyString_FromString("index_lists");
        tmp_value = PyList_New(phrase_result.matches.size());
        for (auto j = 0u; j < phrase_result.matches.size(); ++j) {
            auto& match = phrase_result.matches[j];
            PyObject* index_list = PyList_New(
                match.piece_begin_indexes.size() + 1);
            for (auto k = 0u; k < match.piece_begin_indexes.size(); ++k) {
                PyObject* item = PyInt_FromLong(match.piece_begin_indexes[k]);
                PyList_SET_ITEM(index_list, k, item);
            }
            PyObject* item = PyInt_FromLong(match.end_excl);
            PyList_SET_ITEM(index_list, match.piece_begin_indexes.size(), item);
        }
        tmp_keys.emplace_back(tmp_key);
        tmp_values.emplace_back(tmp_value);

        PyObject* phrase_result_d = PyDict_New();
        for (auto j = 0u; j < tmp_keys.size(); ++j) {
            tmp_key = tmp_keys[j];
            tmp_value = tmp_values[j];
            if (PyDict_SetItem(phrase_result_d, tmp_key, tmp_value)) {
                *error = "[Phraser] Dict set failed.";
                return false;
            }
        }
        PyList_SET_ITEM(value, i, phrase_result_d);
    }
    keys.emplace_back(key);
    values.emplace_back(value);

    for (auto i = 0u; i < keys.size(); ++i) {
        key = keys[i];
        value = values[i];
        if (PyDict_SetItem(*d, key, value)) {
            *error = "[Phraser] Dict set failed.";
            return false;
        }
    }

    return true;
}

PyObject* MakeTuple(PyObject* first, PyObject* second) {
    PyObject* r = PyTuple_New(2);
    PyTuple_SetItem(r, 0, first);
    PyTuple_SetItem(r, 1, second);
    return r;
}

PyObject* Analyze(PyObject* self, PyObject* args) {
    // Get args.
    PyObject* py_text;
    PyObject* options_dict;
    if (!PyArg_ParseTuple(args, "UO!", &py_text, &PyDict_Type, &options_dict)) {
        return NULL;
    }

    // Check if initialized.
    if (!ANALYZER) {
        return MakeTuple(
            Py_None, PyUnicode_FromString("[Phraser] Call init() first."));
    }

    // Get the input text to analyze.
    ustring text;
    Py_ssize_t size = PyUnicode_GetSize(py_text);
    for (Py_ssize_t i = 0; i < size; ++i) {
        text.emplace_back(PyUnicode_AsUnicode(py_text)[i]);
    }

    // Set the analysis options.
    AnalysisOptions options;
    string error;
    if (!AnalysisOptionsFromDict(options_dict, &options, &error)) {
        return MakeTuple(
            Py_None, PyUnicode_FromString(error.data()));
    }

    // Analyze the text.
    AnalysisResult result;
    if (!ANALYZER->Analyze(text, options, &result, &error)) {
        return MakeTuple(
            Py_None, PyUnicode_FromString(error.data()));
    }

    // Convert the results to a python dict.
    PyObject* result_dict;
    if (!DictFromAnalysisResult(result, &result_dict, &error)) {
        return MakeTuple(
            Py_None, PyUnicode_FromString(error.data()));
    }

    return MakeTuple(result_dict, Py_None);
}

PyMethodDef PHRASER_METHODS[] = {
    {"init",    Init,    METH_VARARGS, INIT_DOC},
    {"to_d",    ToD,     METH_VARARGS, TO_D_DOC},
    {"analyze", Analyze, METH_VARARGS, ANALYZE_DOC},
};

}  // namespace

extern "C" void initphraser(void);

PyMODINIT_FUNC initphraser(void) {
    Py_InitModule3("phraser", PHRASER_METHODS, PHRASER_DOC);
}
