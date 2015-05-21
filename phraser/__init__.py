from phraser.phraserext import PhraserExt as ext


def init(phrase_config_ff):
    """
    phrase config file names -> None.

    Initialize the module with some phrase configs to match against.

    Call this first.
    """
    phrase_configs = map(lambda f: open(f).read(), phrase_config_ff)
    err = ext(phrase_configs)
    if err:
        raise err
    return True


def to_dict():
    """
    -> dict

    Dump the module's state as a recursive dict.
    """
    return ext.to_dict()


def to_html():
    """
    -> unicode

    Dump the module's state as pretty HTML.
    """
    raise NotImplementedError


class AnalysisOptions(object):
    def __init__(self):
        self.destutter_max_consecutive = 3
        self.replace_html_entities = True

    def to_dict(self):
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
            phrase = PhraseResult(
                sub['phrase_name'], sub['subsequence_names'],
                sub['index_lists'])
            self.phrase_matches.append(phrase)


def analyze(text, options=None):
    """
    unicode -> AnalysisResult
    unicode, AnalysisOptions -> AnalysisResult

    Analyze the test.  Returns (a) what it did to the text and (b) phrase match
    results.
    """
    assert isinstance(text, unicode)
    if options:
        assert isinstance(options, AnalysisOptions)
    else:
        options = AnalysisOptions()
    result = ext.analyze(text, options.to_dict())
    return AnalysisResult(result)
