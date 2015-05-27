__version__ = '0.1.4'

import collections
from phraser.phraserext import PhraserExt


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


class Phraser(object):

    def __init__(self, config_paths):
        """
        :param config_paths: an iterable of paths to config files
        """
        config_contents = []
        for config_path in config_paths:
            with open(config_path, "r") as fh:
                config_contents.append(fh.read())
        self._extension = PhraserExt(config_contents)

    def analyze(self, text, encoding='utf-8', options=None):
        """
        Analyze input text

        :param text: input text
        :param encoding: which encoding to use if input text is `str`
        :param options: dict or `AnalysisOptions` instance
        :rtype: AnalysisResult

        This method returns (a) what was done to the text,
        and (b) phrase match results.
        """
        if isinstance(text, str):
            text = text.decode(encoding)
        elif not isinstance(text, unicode):
            raise TypeError("Expected string or unicode type but got %s",
                            type(options))
        if isinstance(options, AnalysisOptions):
            options = options.to_dict()
        elif options is None:
            options = {}
        elif not isinstance(collections.Mapping):
            raise TypeError("Expected a mapping type but got %s", type(options))

        result = self._extension.analyze(text, options)
        return AnalysisResult(result)
