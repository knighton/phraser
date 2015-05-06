from ext import phraser


def init(phrase_config_ff):
    phrase_configs = map(lambda f: open(f).read(), phrase_config_ff)
    err = phraser.init(phrase_configs)
    if err:
        raise err
    return True


def to_d():
    return phraser.to_d()


class AnalysisOptions(object):
    def __init__(self):
        self.destutter_max_consecutive = 3
        self.replace_html_entities = True

    def to_d(self):
        return {
            'destutter_max_consecutive': self.destutter_max_consecutive,
            'replace_html_entities': self.replace_html_entities,
        }


class PhraseResult(object):
    def __init__(self, name, subsequence_names, index_lists):
        self.name = name
        self.subsequence_names = subsequence_names
        self.index_lists = index_lists


class AnalysisResult(object):
    def __init__(self, d):
        self.original_text = d['original_text']
        self.clean_text = d['clean_text']
        self.tokens = d['tokens']
        self.phrase_matches = []
        for sub in d['phrase_matches']:
            print '?', sub
            phrase = PhraseResult(
                sub['phrase_name'], sub['subsequence_names'],
                sub['index_lists'])
            self.phrase_matches.append(phrase)


def analyze(text, options=None):
    assert isinstance(text, unicode)
    if options:
        assert isinstance(options, AnalysisOptions)
    else:
        options = AnalysisOptions()
    d, err = phraser.analyze(text, options.to_d())
    if err:
        raise err

    return AnalysisResult(d)
