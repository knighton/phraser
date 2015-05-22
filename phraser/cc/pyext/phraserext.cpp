#include <Python.h>
#include "structmember.h"
#include <string>
#include <vector>

#include "cc/analysis/analyzer.h"
#include "cc/base/unicode/unicode.h"

using std::string;
using std::vector;

namespace {

/*-----------------------------------------------------------------------------
 *  PhraserExt object
 *-----------------------------------------------------------------------------*/
typedef struct {
        PyObject_HEAD
        Analyzer* ANALYZER;
} PhraserExt;


char PHRASER_DOC[] =
    "Python extension that detects phrases in text.\n"
    "\n"
    "phrase config texts -> error str or None.\n"
    "\n"
    "    >>> open('plaudit.txt', 'wb').write('\\n'.join([\n"
    "            'plaudit = verb object',\n"
    "            '----------',\n"
    "            'thanks',\n"
    "            '----------',\n"
    "            'obama',\n"
    "            'hitler',\n"
    "        ]))\n"
    "    >>> phrases_config_ff = ['plaudit.txt']\n"
    "    >>> phrase_configs = map(lambda f: open(f).read(), phrase_config_ff)\n"
    "    >>> phraser = PhraserExt(phrase_configs)\n"
    "    >>> assert phraser\n";


static PyObject *
PhraserExt_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PhraserExt *self;

    if ((self = (PhraserExt *)type->tp_alloc(type, 0)) == NULL) {
        PyErr_SetString(PyExc_MemoryError, "cannot allocate PhraserExt instance");
        return NULL;
    }

    self->ANALYZER = new Analyzer();
    //printf("Created analyzer instance %p\n", (void*)self->ANALYZER);
    if (self->ANALYZER == NULL) {
        Py_DECREF(self);
        PyErr_SetString(PyExc_MemoryError, "cannot allocate analyzer object");
        return NULL;
    }

    return (PyObject *)self;
}


static void
PhraserExt_dealloc(PhraserExt* self)
{
    //printf("Removing analyzer instance %p\n", (void*)self->ANALYZER);
    delete self->ANALYZER;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
PhraserExt_init(PhraserExt *self, PyObject *args, PyObject *kwds)
{
    // Get args.
    PyObject* list;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) {
        return -1;
    }

    // Extract phrase configs from list.
    vector<string> phrase_configs;
    Py_ssize_t length = PyList_Size(list);
    for (Py_ssize_t i = 0; i < length; ++i) {
        PyObject* s = PyList_GetItem(list, i);
        if (!PyString_Check(s)) {
            PyErr_Format(PyExc_ValueError,
                    "[PhraserExt] List item at index %ld was not a string.", i);
            return -1;
        }
        phrase_configs.emplace_back(PyString_AsString(s));
    }

    if (!self->ANALYZER) {
        PyErr_SetString(PyExc_RuntimeError, "not initialized");
        return -1;
    }

    // Call Init().
    string error;
    if (!self->ANALYZER->Init(phrase_configs, &error)) {
        PyErr_SetString(PyExc_RuntimeError, error.data());
        return -1;
    }

    return 0;
}

char ANALYZE_DOC[] =
    "Given an input Unicode string and a dict of options, analyze the string "
    "and return a result dict\n"
    "\n"
    "text, options -> phrase detection result dict.\n"
    "\n"
    "Analyze the text. Any errors are raised as exceptions\n"
    "\n"
    "    >>> text = u'This is a comment.'\n"
    "    >>> options = {\n"
    "            'destutter_max_consecutive': 3,\n"
    "            'replace_html_entities': True,\n"
    "        }\n"
    "    >>> result = phraserext.analyze(text, options)\n"
    "    >>> assert isinstance(result, dict)\n";

bool AnalysisOptionsFromDict(
        PyObject* obj, AnalysisOptions* options, string* error) {
    Py_ssize_t pos = 0;
    PyObject* key;
    PyObject* value;
    while (PyDict_Next(obj, &pos, &key, &value)) {
        if (!PyString_Check(key)) {
            *error = "[PhraserExt] Analysis options key is not a string.";
            return false;
        }
        const char* key_s = PyString_AsString(key);
        if (!strcmp(key_s, "destutter_max_consecutive")) {
            if (!PyInt_Check(value)) {
                *error = "[PhraserExt] Analysis option "
                         "'destutter_max_consecutive' is an integer.";
                return false;
            }
            Py_ssize_t size = PyInt_AsSsize_t(value);
            options->destutter_max_consecutive = static_cast<size_t>(size);
        } else if (!strcmp(key_s, "replace_html_entities")) {
            if (!PyBool_Check(value)) {
                *error = "[PhraserExt] Analysis option 'replace_html_entities' is "
                         "a bool.";
                return false;
            }
            options->replace_html_entities = PyObject_IsTrue(value);
        } else {
            *error = "[PhraserExt] Unknown analysis option.";
            return false;
        }
    }
    return true;
}

PyObject* UnicodeFromUstring(const ustring& s) {
#ifdef Py_UNICODE_WIDE
    // UCS 4 (eg, Linux).
    return PyUnicode_FromUnicode(s.data(), s.size());
#else
    // UCS 2 (eg, Mac OS X).
    vector<Py_UNICODE> v;
    for (auto& c : s) {
        v.emplace_back(static_cast<Py_UNICODE>(c));
    }
    return PyUnicode_FromUnicode(v.data(), v.size());
#endif
}

PyObject* MakeDict(const vector<PyObject*>& keys,
                   const vector<PyObject*>& values)
{
    auto num_keys = keys.size();
    auto num_vals = values.size();
    auto num_pairs = std::min(num_keys, num_vals);
    PyObject* d = PyDict_New();
    for (auto i = 0u; i < num_pairs; ++i) {
        PyObject* k = keys[i];
        PyObject* v = values[i];
        if (!k) {
            PyErr_Format(PyExc_KeyError, "Missing key at index %u", i);
            Py_DECREF(d);
            return NULL;
        }
        else if (!v) {
            PyErr_Format(PyExc_ValueError, "Missing value at index %u", i);
            Py_DECREF(d);
            return NULL;
        }
        auto code = PyDict_SetItem(d, k, v);
        Py_DECREF(k);
        Py_DECREF(v);
        if (code) {
            PyErr_Format(PyExc_RuntimeError, "Could not set item at index %u", i);
            Py_DECREF(d);
            return NULL;
        }
    }
    return d;
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
PyObject* DictFromAnalysisResult(const AnalysisResult& result, string* error) {
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
            PyList_SET_ITEM(tmp_value, j, index_list);
        }
        tmp_keys.emplace_back(tmp_key);
        tmp_values.emplace_back(tmp_value);

        PyObject* tmp_d = MakeDict(tmp_keys, tmp_values);
        PyList_SET_ITEM(value, i, tmp_d);
    }
    keys.emplace_back(key);
    values.emplace_back(value);

    return MakeDict(keys, values);
}

static PyObject*
PhraserExt_analyze(PhraserExt* self, PyObject* args) {
    // Get args.
    //
    PyObject* py_text;
    PyObject* options_dict;
    if (!PyArg_ParseTuple(args, "UO!", &py_text, &PyDict_Type, &options_dict)) {
        // TODO: call PyErr_SetString here as well?
        return NULL;
    }

    // Check if initialized.
    if (!self->ANALYZER) {
        PyErr_SetString(PyExc_RuntimeError, "PhraserExt has not been initialized");
        return NULL;
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
        PyErr_SetString(PyExc_RuntimeError, error.data());
        return NULL;
    }

    // Analyze the text.
    AnalysisResult result;
    if (!self->ANALYZER->Analyze(text, options, &result, &error)) {
        PyErr_SetString(PyExc_RuntimeError, error.data());
        return NULL;
    }

    // Convert the results to a python dict.
    PyObject* result_dict;
    if (!(result_dict = DictFromAnalysisResult(result, &error))) {
        PyErr_SetString(PyExc_RuntimeError, error.data());
        return NULL;
    }

    return result_dict;
}

static PyMethodDef PhraserExt_methods[] = {
    {"analyze", (PyCFunction)PhraserExt_analyze, METH_VARARGS, ANALYZE_DOC},
    { NULL, NULL, 0, NULL }  /* sentinel */
};

static PyMemberDef PhraserExt_members[] = {
    { NULL, 0, 0, 0, NULL }  /* sentinel */
};

static PyTypeObject PhraserExtType = {
        PyVarObject_HEAD_INIT(NULL, 0)
                "phraserext.PhraserExt",     /* tp_name */
        sizeof(PhraserExt),             /* tp_basicsize */
        0,                              /* tp_itemsize */
        (destructor)PhraserExt_dealloc, /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_reserved */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        0,                         /* tp_as_sequence */
        0,                         /* tp_as_mapping */
        0,                         /* tp_hash  */
        0,                         /* tp_call */
        0,                         /* tp_str */
        0,                         /* tp_getattro */
        0,                         /* tp_setattro */
        0,                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
        PHRASER_DOC,           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        PhraserExt_methods,        /* tp_methods */
        PhraserExt_members,        /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)PhraserExt_init, /* tp_init */
        0,                         /* tp_alloc */
        PhraserExt_new,            /* tp_new */
        0,                      /*  tp_free */
        0,                      /* tp_is_gc */
        0,                      /* tp_bases */
        0,                      /* tp_mro */
        0,                      /* tp_cache */
        0,                      /* tp_subclasses */
        0,                      /* tp_weaklist */
        0,                      /*  tp_del */
        0,                      /*  tp_version_tag */
};

}  // namespace

extern "C" void initphraserext(void);

PyMODINIT_FUNC initphraserext(void) {

    PyObject* m;

    if (PyType_Ready(&PhraserExtType) < 0)
        return;

    m = Py_InitModule3("phraserext", PhraserExt_methods,
                       "Example module that creates an extension type.");

    Py_INCREF(&PhraserExtType);
    PyModule_AddObject(m, "PhraserExt", (PyObject *)&PhraserExtType);
}
