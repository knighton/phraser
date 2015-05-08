#include "frontend.h"

#include "cc/frontend/destutter/destutterer_step.h"
#include "cc/frontend/html/html_step.h"

namespace {

bool MakeUnicodeSteps(
        vector<UnicodeRewriteStep*>* steps_html,
        vector<UnicodeRewriteStep*>* steps_no_html, string* error) {
    {
        auto* step = new DestuttererStep();
        if (!step) {
            return false;
        }

        if (!step->Init(error)) {
            return false;
        }

        steps_html->emplace_back(step);
        steps_no_html->emplace_back(step);
    }

    {
        auto* step = new HtmlStep();
        if (!step) {
            return false;
        }

        if (!step->Init(error)) {
            return false;
        }

        steps_no_html->emplace_back(step);
    }

    return true;
}

}  // namespace

Frontend::~Frontend() {
    for (auto& p : unicode_steps_with_html_) {
        delete p;
    }
}

bool Frontend::Init(string* error) {
    if (!MakeUnicodeSteps(&unicode_steps_no_html_, &unicode_steps_with_html_,
                          error)) {
        return false;
    }

    is_ok_ = true;
    return true;
}
