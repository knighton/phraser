#include "frontend.h"

#include "cc/frontend/destutter/destutterer_step.h"
#include "cc/frontend/html/html_step.h"
#include "cc/frontend/americanize/americanizer_step.h"
#include "cc/frontend/ptb_escape/ptb_escaper_step.h"

namespace {

bool MakeUnicodeSteps(
        vector<UnicodeRewriteStep*>* steps_html,
        vector<UnicodeRewriteStep*>* steps_no_html, string* error) {
    {
        auto* step = new DestuttererStep();
        if (!step) {
            *error = "[Frontend] Allocation failed.";
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
            *error = "[Frontend] Allocation failed.";
            return false;
        }

        if (!step->Init(error)) {
            return false;
        }

        steps_no_html->emplace_back(step);
    }

    return true;
}

bool MakeTokenSteps(vector<TokenRewriteInPlaceStep*>* steps, string* error) {
    {
        auto* am = new AmericanizerStep();
        if (!am) {
            *error = "[Frontend] Allocation failed.";
            return false;
        }

        if (!am->Init(error)) {
            return false;
        }

        steps->emplace_back(am);
    }

    {
        auto* esc = new PtbEscaperStep();
        if (!esc) {
            *error = "[Frontend] Allocation failed.";
            return false;
        }

        if (!esc->Init(error)) {
            return false;
        }

        steps->emplace_back(esc);
    }

    return true;
}

}  // namespace

Frontend::~Frontend() {
    for (auto& p : unicode_steps_with_html_) {
        delete p;
    }

    for (auto& p : token_steps_) {
        delete p;
    }
}

bool Frontend::Init(string* error) {
    if (!MakeUnicodeSteps(&unicode_steps_no_html_, &unicode_steps_with_html_,
                          error)) {
        return false;
    }

    if (!MakeTokenSteps(&token_steps_, error)) {
        return false;
    }

    if (!unreduct_.InitDefault(error)) {
        return false;
    }

    if (!tag_.Init()) {
        *error = "[Frontend] LaposTagger initialization failed.";
        return false;
    }

    uncontract_.Init();

    is_ok_ = true;
    return true;
}

bool Frontend::Analyze(
        const ustring& original_text, const FrontendOptions& options,
        FrontendResult* result, string* error) const {
    return false;  // XXX
}
