#include "phrase_config.h"

static void DumpList(const vector<string>& v) {
    printf("[");
    if (v.size()) {
        printf("%s", v[0].c_str());
    }
    for (auto i = 1u; i < v.size(); ++i) {
        printf(", %s", v[i].c_str());
    }
    printf("]");
}

json::Object* PhraseConfig::ToJSON() const {
    vector<json::Object*> block_objects;
    block_objects.reserve(blocks.size());
    for (auto i = 0u; i < blocks.size(); ++i) {
        auto& options = blocks[i];
        vector<json::Object*> option_objects;
        option_objects.reserve(options.size());
        for (auto j = 0u; j < options.size(); ++j) {
            auto& atoms = options[j];
            vector<json::Object*> atom_objects;
            atom_objects.reserve(atoms.size());
            for (auto k = 0u; k < atoms.size(); ++k) {
                auto& atom = atoms[k];
                auto* obj = new json::Object(atom);
                atom_objects.emplace_back(obj);
            }
            option_objects.emplace_back(new json::Object(atom_objects));
        }
        block_objects.emplace_back(new json::Object(option_objects));
    }

    return new json::Object({
        {"name", new json::Object(name)},
        {"block_names", new json::Object(block_names)},
        {"blocks", new json::Object(block_objects)},
    });
}
