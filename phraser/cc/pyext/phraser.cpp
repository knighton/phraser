#include <Python.h>
#include <string>
#include <vector>

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
        return PyUnicode_FromString("[Phraser] Invalid arguments.");
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

PyObject* Analyze(PyObject* self, PyObject* args) {
    return NULL;
}

PyMethodDef PHRASER_METHODS[] = {
    {"init",    Init,    METH_VARARGS, INIT_DOC},
    {"to_d",    ToD,     METH_VARARGS, TO_D_DOC},
    {"analyze", Analyze, METH_VARARGS, ANALYZE_DOC},
};

}  // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
PyMODINIT_FUNC initphraser(void) {
#pragma clang diagnostic pop
    Py_InitModule3("phraser", PHRASER_METHODS, PHRASER_DOC);
}
